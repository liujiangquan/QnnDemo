package com.breeze.qnn.ner

import android.content.Context
import android.util.Log
import com.breeze.qnn.InferenceEngine
import com.breeze.qnn.QnnNative
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.withContext
import java.io.File

/** 一次识别的结果与统计。 */
data class NerResult(
    val entities: List<Entity>,
    val sentenceCount: Int,
    val failedSentences: Int,
    val elapsedMs: Long,
)

/**
 * 中文敏感信息识别编排层。
 *
 * 流程：切句 → 逐句 tokenize + DLC 推理 + BIO 解码 → 整段跑正则 → 合并去重。
 *
 * **后端选 fp32 DLC + HTP**。实测（见 docs/superpowers/specs 里的 NER 设计文档）：
 * fp32 在 HTP 上余弦 0.99697、~34ms/句，而 INT8/W8A16 量化后判别力全失（预测全塌成 O）
 * 且反而慢 1.6x。所以这里不用量化模型。
 */
class NerBackend(private val context: Context) : AutoCloseable {

    private val engine = InferenceEngine(context)
    private var tokenizer: WordPieceTokenizer? = null
    private var graphName: String = ""
    /** DLC 声明的输入张量顺序，喂数据必须按名字对齐（见 TokenizedInput.toTensorBytes） */
    private var inputNames: List<String> = emptyList()
    private var ready = false

    /** 加载 vocab 并初始化 native 运行时。 */
    fun init(): Boolean {
        tokenizer = try {
            context.assets.open(VOCAB_ASSET).use { WordPieceTokenizer.fromStream(it) }
        } catch (e: Exception) {
            Log.e(TAG, "加载 $VOCAB_ASSET 失败: ${e.message}")
            return false
        }
        if (!engine.init()) {
            Log.e(TAG, "InferenceEngine.init 失败")
            return false
        }
        Log.i(TAG, "init 完成, vocabSize=${tokenizer?.vocabSize}")
        return true
    }

    /** 模型是否已预置（context binary 或 DLC 任一）。 */
    fun modelReady(): Boolean =
        ctxFile().let { it.exists() && it.length() > 100_000_000L } ||
        dlcFile().let { it.exists() && it.length() > 100_000_000L }

    private fun ctxFile() = File(context.filesDir, "ner/$CTX_NAME")
    private fun dlcFile() = File(context.filesDir, "ner/$DLC_NAME")

    /**
     * 加载模型，backend 默认 HTP。
     *
     * HTP 上**优先用预编译 context binary**（205MB / 加载 371ms），
     * 没有才回退 DLC（388MB / 加载 5344ms）。两者精度相同（余弦 0.99697），
     * 单句耗时也基本一样（~17ms），收益是体积对半 + 加载快 14x。
     * context binary 是 HTP graph-prepare 的产物，CPU 后端加载不了，也与 SoC 绑死，
     * 所以 DLC 保留作 CPU / 换机型的兜底。
     */
    suspend fun loadModel(backend: QnnNative.Backend = QnnNative.Backend.HTP): Boolean {
        val ctx = ctxFile()
        val useCtx = backend == QnnNative.Backend.HTP &&
            ctx.exists() && ctx.length() > 100_000_000L
        val ok = if (useCtx) {
            Log.i(TAG, "用 context binary: ${ctx.name}")
            engine.loadContextBinary(ctx.absolutePath, backend)
        } else {
            val f = dlcFile()
            if (!f.exists()) {
                Log.e(TAG, "模型不存在: ${ctx.absolutePath} / ${f.absolutePath}")
                return false
            }
            Log.i(TAG, "用 DLC: ${f.name} (backend=$backend)")
            engine.loadDlc(f.absolutePath, backend)
        }
        if (!ok) return false
        val g = engine.graphInfos.firstOrNull() ?: run {
            Log.e(TAG, "模型里没有图")
            return false
        }
        graphName = g.name
        inputNames = g.inputs.map { it.name }
        Log.i(TAG, "输入张量顺序 = $inputNames")
        ready = true
        Log.i(TAG, "模型已加载, graph=$graphName backend=$backend")
        warmup()
        return true
    }

    /**
     * 预热一次推理并丢弃结果。
     *
     * HTP 上 graphFinalize 之后的**第一次 execute 输出是不可靠的**（实测："马化腾在深圳
     * 创办了腾讯公司"首次只解出 (LOC, 圳)，第二次起就完全正确）。不预热的话用户第一次
     * 点"识别"会看到垃圾结果。
     */
    private suspend fun warmup() {
        val tok = tokenizer ?: return
        val input = tok.encode(WARMUP_TEXT)
        engine.executeWithOutput(graphName, input.toTensorBytes(inputNames))
        Log.i(TAG, "warmup 完成")
    }

    /**
     * 识别整段文本里的敏感实体。
     * @param onProgress (已处理句数, 总句数)
     */
    suspend fun recognize(
        text: String,
        onProgress: ((Int, Int) -> Unit)? = null,
    ): NerResult = withContext(Dispatchers.IO) {
        val t0 = System.currentTimeMillis()
        val tok = tokenizer
        if (!ready || tok == null) {
            return@withContext NerResult(emptyList(), 0, 0, 0)
        }

        var sentences = SentenceSplitter.split(text)
        if (sentences.size > MAX_SENTENCES) {
            Log.w(TAG, "文本过长，截断到 $MAX_SENTENCES 句（原 ${sentences.size} 句）")
            sentences = sentences.take(MAX_SENTENCES)
        }

        val modelEntities = ArrayList<Entity>()
        var failed = 0
        sentences.forEachIndexed { idx, sent ->
            val input = tok.encode(sent.text)
            val outputs = engine.executeWithOutput(graphName, input.toTensorBytes(inputNames))
            val logitsBytes = outputs?.firstOrNull()
            if (logitsBytes == null) {
                failed++
                Log.w(TAG, "第 $idx 句推理失败，跳过")
            } else {
                modelEntities += BioDecoder.decode(
                    logits = BioDecoder.parseLogits(logitsBytes),
                    offsets = input.offsets,
                    validLen = input.validLen,
                    baseOffset = sent.charOffset,
                    fullText = text,
                )
            }
            onProgress?.invoke(idx + 1, sentences.size)
        }

        val merged = EntityMerger.merge(modelEntities, RegexDetector.detect(text))
        val elapsed = System.currentTimeMillis() - t0
        Log.i(TAG, "识别完成: ${merged.size} 个实体 / ${sentences.size} 句 / ${elapsed}ms" +
            if (failed > 0) " (失败 $failed 句)" else "")
        NerResult(merged, sentences.size, failed, elapsed)
    }

    override fun close() {
        engine.close()
        ready = false
    }

    companion object {
        private const val TAG = "NerBackend"
        const val VOCAB_ASSET = "ner_vocab.txt"
        const val DLC_NAME = "bert-ner-fp32.dlc"
        /** 预编译 context binary（fp16 已烘入），优先用它 */
        const val CTX_NAME = "bert-ner-fp16.bin"
        /** 防 UI 卡死的上限 */
        const val MAX_SENTENCES = 50
        /** 预热用的短句，内容无所谓，只为触发一次 execute */
        const val WARMUP_TEXT = "预热。"
    }
}

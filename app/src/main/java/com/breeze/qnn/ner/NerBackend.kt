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

    /** DLC 是否已预置到设备。 */
    fun modelReady(): Boolean = dlcFile().let { it.exists() && it.length() > 100_000_000L }

    private fun dlcFile() = File(context.filesDir, "ner/$DLC_NAME")

    /** 加载 DLC。backend 默认 HTP。 */
    suspend fun loadModel(backend: QnnNative.Backend = QnnNative.Backend.HTP): Boolean {
        val f = dlcFile()
        if (!f.exists()) {
            Log.e(TAG, "DLC 不存在: ${f.absolutePath}")
            return false
        }
        if (!engine.loadDlc(f.absolutePath, backend)) return false
        graphName = engine.graphInfos.firstOrNull()?.name ?: run {
            Log.e(TAG, "DLC 里没有图")
            return false
        }
        ready = true
        Log.i(TAG, "DLC 已加载, graph=$graphName backend=$backend")
        return true
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
            val outputs = engine.executeWithOutput(graphName, input.toTensorBytes())
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
        /** 防 UI 卡死的上限 */
        const val MAX_SENTENCES = 50
    }
}

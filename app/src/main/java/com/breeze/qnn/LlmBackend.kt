package com.breeze.qnn

import android.content.Context
import android.util.Log
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.withContext
import org.json.JSONArray
import org.json.JSONObject
import java.io.File

/**
 * LLM 推理上层 API。
 *
 * 封装 [LlmNative] JNI 调用，提供协程友好的 suspend 接口。
 *
 * 模型默认从公共目录 `/data/models/chat` 读取（见 [ModelStore]）；Genie 的 config 由
 * [prepareConfig] 在运行时生成到可写目录 `filesDir/llm-config/`，其中所有相对路径都
 * 改写为绝对路径——native 是把 JSON 内容直接交给 Genie，相对路径会按进程 cwd 解析。
 */
class LlmBackend(private val context: Context) : AutoCloseable {

    private val native = LlmNative()
    private var handle: Long = 0L
    private var initialized = false
    private var loaded = false

    /**
     * 初始化 native 运行时，设置库搜索路径。
     * @return true 表示初始化成功
     */
    fun init(): Boolean {
        if (handle == 0L) handle = native.nativeCreate()
        if (handle == 0L) { Log.e(TAG, "nativeCreate 失败"); return false }
        val libDir = context.applicationInfo.nativeLibraryDir
        val htpDir = "${context.filesDir.absolutePath}/htp"
        File(htpDir).mkdirs()
        val searchPath = "$libDir;$htpDir"
        initialized = native.nativeInit(handle, searchPath)
        if (!initialized) Log.e(TAG, "nativeInit 失败, searchPath=$searchPath")
        else Log.i(TAG, "nativeInit 成功, searchPath=$searchPath")
        return initialized
    }

    /**
     * 检查模型权重文件是否就绪：config 可解析、ctx-bins 全部存在且总大小 ≥ 500MB。
     */
    fun weightsReady(modelId: String): Boolean {
        val source = ModelStore.llm(context, modelId) ?: run {
            Log.w(TAG, "未找到模型目录（/data/models/chat 或 filesDir/llm/$modelId）")
            return false
        }
        val config = ModelStore.llmConfig(source.dir, modelId) ?: run {
            Log.w(TAG, "模型目录缺少 *htp*.json: ${source.dir}")
            return false
        }
        val bins = ctxBins(config, source.dir)
        val ready = bins.isNotEmpty() && bins.all { it.isFile } && bins.sumOf { it.length() } >= MIN_WEIGHTS_BYTES
        if (!ready) Log.w(TAG, "权重不完整: dir=${source.dir} bins=${bins.size}")
        return ready
    }

    /**
     * 加载模型（I/O 密集，挂起在 Dispatchers.IO）。
     */
    suspend fun loadModel(modelId: String): Boolean = withContext(Dispatchers.IO) {
        if (!initialized) return@withContext false
        val source = ModelStore.llm(context, modelId) ?: run {
            Log.e(TAG, "未找到模型目录: $modelId")
            return@withContext false
        }
        val sourceConfig = ModelStore.llmConfig(source.dir, modelId) ?: run {
            Log.e(TAG, "缺少 *htp*.json: ${source.dir}")
            return@withContext false
        }
        val configPath = prepareConfig(sourceConfig, source.dir, modelId) ?: return@withContext false
        Log.i(TAG, "loadModel: dir=${source.dir} config=$configPath")
        loaded = native.nativeLoadModel(handle, configPath)
        if (!loaded) Log.e(TAG, "nativeLoadModel 返回 false: $configPath")
        return@withContext loaded
    }

    /**
     * 生成可加载的 config 到 `filesDir/llm-config/<modelId>-htp.json`：
     * 1. 所有指向模型目录内已有文件的相对路径改写为绝对路径；
     * 2. Genie 2.48 要求 rope 参数放在 `engine.model.positional-encoding`，而预置模型可能
     *    沿用新 schema（`QnnHtp.rope-theta` / `pos-id-dim`），这里按需搬过去。
     *
     * `/data/models` 只读，所以不能原地改，统一生成副本。
     */
    private fun prepareConfig(sourceConfig: File, modelDir: File, modelId: String): String? {
        val root = try {
            JSONObject(sourceConfig.readText())
        } catch (e: Exception) {
            Log.e(TAG, "解析 config 失败: ${sourceConfig.absolutePath}: ${e.message}")
            return null
        }

        absolutizeFilePaths(root, modelDir)
        normalizeRopeSchema(root)

        val outDir = File(context.filesDir, "llm-config").apply { mkdirs() }
        val out = File(outDir, "$modelId-htp.json")
        return try {
            out.writeText(root.toString(2))
            out.absolutePath
        } catch (e: Exception) {
            Log.e(TAG, "写 config 失败: ${out.absolutePath}: ${e.message}")
            null
        }
    }

    /** 递归把所有"能对应到模型目录下真实文件"的字符串改成绝对路径。 */
    private fun absolutizeFilePaths(node: Any, baseDir: File) {
        when (node) {
            is JSONObject -> {
                for (key in node.keys().asSequence().toList()) {
                    val value = node.opt(key)
                    if (value is String) {
                        val child = File(baseDir, value)
                        if (!File(value).isAbsolute && value.isNotEmpty() && child.isFile) {
                            node.put(key, child.absolutePath)
                        }
                    } else if (value is JSONObject || value is JSONArray) {
                        absolutizeFilePaths(value, baseDir)
                    }
                }
            }
            is JSONArray -> {
                for (i in 0 until node.length()) {
                    val value = node.opt(i)
                    if (value is String) {
                        val child = File(baseDir, value)
                        if (!File(value).isAbsolute && value.isNotEmpty() && child.isFile) {
                            node.put(i, child.absolutePath)
                        }
                    } else if (value is JSONObject || value is JSONArray) {
                        absolutizeFilePaths(value, baseDir)
                    }
                }
            }
        }
    }

    /** 把 `QnnHtp.rope-theta` / `pos-id-dim` 搬到 `engine.model.positional-encoding`。 */
    private fun normalizeRopeSchema(root: JSONObject) {
        val dialog = root.optJSONObject("dialog") ?: return
        val engine = dialog.optJSONObject("engine") ?: return
        val model = engine.optJSONObject("model") ?: return
        if (model.optJSONObject("positional-encoding") != null) return

        val htp = engine.optJSONObject("backend")?.optJSONObject("QnnHtp") ?: return
        val theta = htp.optDouble("rope-theta", Double.NaN)
        val dim = htp.optInt("pos-id-dim", 0)
        if (theta.isNaN() && dim <= 0) return

        model.put("positional-encoding", JSONObject().apply {
            put("type", "rope")
            if (dim > 0) put("rope-dim", dim)
            if (!theta.isNaN()) put("rope-theta", theta)
        })
        htp.remove("rope-theta")
        htp.remove("pos-id-dim")
    }

    /** 从 config 解析 ctx-bins 列表；相对路径按模型目录拼接。 */
    private fun ctxBins(config: File, modelDir: File): List<File> = runCatching {
        val bins = JSONObject(config.readText())
            .optJSONObject("dialog")
            ?.optJSONObject("engine")
            ?.optJSONObject("model")
            ?.optJSONObject("binary")
            ?.optJSONArray("ctx-bins")
            ?: return@runCatching emptyList()
        (0 until bins.length()).map { File(ModelStore.absolutePath(bins.getString(it), modelDir)) }
    }.getOrDefault(emptyList())

    /**
     * 执行推理生成（挂起在 Dispatchers.IO，通过回调逐 token 输出）。
     *
     * 默认给 prompt 套 Qwen3 chat template（`<|im_start|>...<|im_end|>\n<|im_start|>assistant\n`），
     * 让 instruct-tuned 模型进入"回答"而非"续写"模式。已经带 `<|im_start|>` 的 prompt 会原样透传，
     * 上层想手工组多轮 / 加 system prompt 时可以直接传完整拼好的字符串。
     *
     * onToken 拿到的文本已经过 [ThinkFilter] 过滤，`<think>...</think>` 段和其后紧邻的
     * 前导空白都会被吞掉；UI 只看到最终答案。stripThink=false 关掉过滤（诊断用）。
     */
    suspend fun generate(
        prompt: String,
        params: SamplingParams,
        onToken: (String) -> Unit,
        onComplete: (GenerateStats) -> Unit,
        onError: (String) -> Unit,
        stripThink: Boolean = true,
    ) = withContext(Dispatchers.IO) {
        if (!loaded) { onError("模型未加载"); return@withContext }
        val wrapped = wrapChatTemplate(prompt)
        var errored = false
        val filter = if (stripThink) ThinkFilter() else null
        val cb = object : LlmNative.TokenCallback {
            override fun onToken(tokenText: String) {
                if (filter != null) filter.feed(tokenText, onToken)
                else onToken(tokenText)
            }

            override fun onError(message: String) {
                errored = true
                onError(message)
            }
        }
        val resultJson = native.nativeGenerate(
            handle, wrapped,
            params.temperature, params.topP, params.topK,
            params.maxTokens, params.seed, cb
        )
        filter?.flush(onToken)
        // 如果 native 层通过 callback 报了错，就不再调 onComplete；
        // 否则解析 JSON 汇报 stats。JSON 里的 stoppedReason=ERROR 也会走 onComplete
        // 让 UI 层看到（业务决定是否显示"生成失败"）。
        if (!errored) {
            val stats = parseStats(resultJson)
            onComplete(stats)
        }
    }

    /**
     * Qwen3-Instruct 的 chat template。已经带 `<|im_start|>` 视为用户自己拼好，不再包一层。
     *
     * **默认关闭 thinking mode**（预填空 `<think>\n\n</think>\n\n`）：
     * Qwen3 见到已闭合的 think 块，会跳过 CoT 直接输出答案；chat 场景下用户要
     * 直接答复不要看推理过程。要开思考模式的话上层自己拼完整 `<|im_start|>` 传进来
     * （wrap 逻辑会原样透传）。
     */
    private fun wrapChatTemplate(prompt: String): String =
        if (prompt.contains("<|im_start|>")) prompt
        else "<|im_start|>user\n$prompt<|im_end|>\n" +
             "<|im_start|>assistant\n<think>\n\n</think>\n\n"

    /** 中断正在执行的生成 */
    fun stop() { if (handle != 0L) native.nativeStop(handle) }

    override fun close() {
        if (handle != 0L) { native.nativeDestroy(handle); handle = 0L }
        initialized = false; loaded = false
    }

    private fun parseStats(json: String): GenerateStats {
        val o = JSONObject(json)
        val reasonStr = o.optString("stoppedReason", "ERROR")
        val reason = try {
            StopReason.valueOf(reasonStr)
        } catch (_: IllegalArgumentException) {
            StopReason.ERROR
        }
        return GenerateStats(
            elapsedMs = o.optLong("elapsedMs", 0),
            tokensGenerated = o.optInt("tokensGenerated", 0),
            tokensPerSec = o.optDouble("tokensPerSec", 0.0),
            stoppedReason = reason,
        )
    }

    companion object {
        private const val TAG = "LlmBackend"
        private const val MIN_WEIGHTS_BYTES = 500_000_000L
    }
}

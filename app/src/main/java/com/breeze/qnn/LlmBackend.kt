package com.breeze.qnn

import android.content.Context
import android.util.Log
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.withContext
import org.json.JSONObject
import java.io.File

/**
 * LLM 推理上层 API。
 *
 * 封装 [LlmNative] JNI 调用，提供协程友好的 suspend 接口。
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
     * 检查模型权重文件是否就绪。
     * 期望目录结构: filesDir/llm/{modelId}/{modelId}-htp.json + {modelId}-context.bin + tokenizer.json
     */
    fun weightsReady(modelId: String): Boolean {
        val dir = File(context.filesDir, "llm/$modelId")
        val config = File(dir, "${modelId}-htp.json")
        val binary = File(dir, "${modelId}-context.bin")
        val tokenizer = File(dir, "tokenizer.json")
        return config.exists() && binary.exists() && tokenizer.exists()
            && binary.length() >= 500_000_000L  // 至少 500MB
    }

    /**
     * 加载模型（I/O 密集，挂起在 Dispatchers.IO）。
     */
    suspend fun loadModel(modelId: String): Boolean = withContext(Dispatchers.IO) {
        if (!initialized) return@withContext false
        val configPath = "${context.filesDir}/llm/$modelId/${modelId}-htp.json"
        loaded = native.nativeLoadModel(handle, configPath)
        return@withContext loaded
    }

    /**
     * 执行推理生成（挂起在 Dispatchers.IO，通过回调逐 token 输出）。
     *
     * 默认给 prompt 套 Qwen3 chat template（`<|im_start|>...<|im_end|>\n<|im_start|>assistant\n`），
     * 让 instruct-tuned 模型进入"回答"而非"续写"模式。已经带 `<|im_start|>` 的 prompt 会原样透传，
     * 上层想手工组多轮 / 加 system prompt 时可以直接传完整拼好的字符串。
     */
    suspend fun generate(
        prompt: String,
        params: SamplingParams,
        onToken: (String) -> Unit,
        onComplete: (GenerateStats) -> Unit,
        onError: (String) -> Unit,
    ) = withContext(Dispatchers.IO) {
        if (!loaded) { onError("模型未加载"); return@withContext }
        val wrapped = wrapChatTemplate(prompt)
        var errored = false
        val cb = object : LlmNative.TokenCallback {
            override fun onToken(tokenText: String) { onToken(tokenText) }
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

    companion object { private const val TAG = "LlmBackend" }
}

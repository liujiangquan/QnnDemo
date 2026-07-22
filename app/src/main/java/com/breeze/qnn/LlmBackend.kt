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
     */
    suspend fun generate(
        prompt: String,
        params: SamplingParams,
        onToken: (String) -> Unit,
        onComplete: (GenerateStats) -> Unit,
        onError: (String) -> Unit,
    ) = withContext(Dispatchers.IO) {
        if (!loaded) { onError("模型未加载"); return@withContext }
        val cb = object : LlmNative.TokenCallback {
            override fun onToken(tokenText: String) { onToken(tokenText) }
            override fun onError(message: String) { onError(message) }
        }
        val resultJson = native.nativeGenerate(
            handle, prompt,
            params.temperature, params.topP, params.topK,
            params.maxTokens, params.seed, cb
        )
        val stats = parseStats(resultJson)
        onComplete(stats)
    }

    /** 中断正在执行的生成 */
    fun stop() { if (handle != 0L) native.nativeStop(handle) }

    override fun close() {
        if (handle != 0L) { native.nativeDestroy(handle); handle = 0L }
        initialized = false; loaded = false
    }

    private fun parseStats(json: String): GenerateStats {
        val o = JSONObject(json)
        return GenerateStats(
            elapsedMs = o.optLong("elapsedMs", 0),
            tokensGenerated = o.optInt("tokensGenerated", 0),
            tokensPerSec = o.optDouble("tokensPerSec", 0.0),
            stoppedReason = StopReason.valueOf(o.optString("stoppedReason", "ERROR")),
        )
    }

    companion object { private const val TAG = "LlmBackend" }
}

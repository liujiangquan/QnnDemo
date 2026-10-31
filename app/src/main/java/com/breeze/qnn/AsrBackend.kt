package com.breeze.qnn

import android.content.Context
import android.util.Log
import java.io.File
import java.util.concurrent.Executors
import kotlinx.coroutines.asCoroutineDispatcher
import kotlinx.coroutines.withContext

/**
 * 语音识别后端：管理 voice_asr 模型生命周期，提供流式识别。
 *
 * 模型目录：filesDir/asr/voice_asr/（由 docs/setup_voice_asr.sh 预置，不进 APK）。
 * ASREngine 非线程安全，所有 native 调用经单线程 [dispatcher] 串行化。
 */
class AsrBackend(private val context: Context) : AutoCloseable {

    private val native = AsrNative()
    private val dispatcher = Executors.newSingleThreadExecutor { r ->
        Thread(r, "asr-native").apply { isDaemon = true }
    }.asCoroutineDispatcher()

    @Volatile private var handle: Long = 0L

    /** 模型目录的 model_config.json 路径。 */
    private val configPath: String
        get() = "${context.filesDir.absolutePath}/asr/voice_asr/model_config.json"

    /** 模型文件是否齐全（配置、tokenizer、编码器、LLM 两个 shard、embedding）。 */
    fun modelReady(): Boolean {
        val root = File(context.filesDir, "asr/voice_asr")
        return File(root, "model_config.json").isFile &&
            File(root, "stream_model/tokenizer.json").isFile &&
            File(root, "stream_model/asr_step2_0p6b_streaming_320ms_audio_fp16.serialized.bin").isFile &&
            File(root, "stream_model/embedding_fp32.bin").isFile &&
            File(root, "device_payload_w4a16_lpbq_kv16_no_seqmse/qwen3_base_weight_sharing_model_ar128_ar1_cl2048_1_of_2.serialized.bin").isFile &&
            File(root, "device_payload_w4a16_lpbq_kv16_no_seqmse/qwen3_base_weight_sharing_model_ar128_ar1_cl2048_2_of_2.serialized.bin").isFile
    }

    /** 加载模型（挂起在 native 线程）。返回 true 表示 ASREngine::init 成功。 */
    suspend fun init(): Boolean = withContext(dispatcher) {
        if (handle != 0L) return@withContext true
        if (!modelReady()) {
            Log.e(TAG, "模型文件不齐全，请先运行 docs/setup_voice_asr.sh")
            return@withContext false
        }
        handle = native.nativeCreate(configPath)
        if (handle == 0L) {
            Log.e(TAG, "ASREngine 初始化失败，errCode=${native.nativeLastInitError()}")
            false
        } else {
            Log.i(TAG, "ASREngine 就绪")
            true
        }
    }

    /**
     * 送入一段 PCM（16-bit LE，16 kHz，单声道）。[isFinal]=true 表示最后一段，
     * 引擎会输出剩余文本。返回该段识别文本。
     */
    suspend fun feed(pcm: ByteArray, isFinal: Boolean): String = withContext(dispatcher) {
        val h = handle
        if (h == 0L) return@withContext ""
        native.nativeGenerate(h, pcm, isFinal)
    }

    /**
     * 整段 WAV 文件识别：去掉 44 字节头，按 320 ms（10240 字节）分块流式送入，
     * 最后一块标记为 isFinal，拼接各段文本。
     */
    suspend fun transcribeWav(wav: File): String {
        val bytes = wav.readBytes()
        require(bytes.size > WAV_HEADER_BYTES) { "wav 文件过短: ${wav.name}" }
        val pcm = bytes.copyOfRange(WAV_HEADER_BYTES, bytes.size)
        return transcribePcm(pcm)
    }

    /** 整段 PCM 分块流式识别，返回拼接后的文本。 */
    suspend fun transcribePcm(pcm: ByteArray): String {
        val out = StringBuilder()
        var offset = 0
        while (offset < pcm.size) {
            val end = minOf(offset + CHUNK_BYTES, pcm.size)
            val chunk = pcm.copyOfRange(offset, end)
            val isFinal = end >= pcm.size
            out.append(feed(chunk, isFinal))
            offset = end
        }
        return out.toString()
    }

    override fun close() {
        val h = handle
        handle = 0L
        if (h != 0L) {
            // 释放也走同一个 native 线程，保证与推理串行
            dispatcher.executor.execute { native.nativeRelease(h) }
        }
    }

    companion object {
        private const val TAG = "AsrBackend"
        /** 320 ms @ 16 kHz 单声道 16-bit = 5120 样本 × 2 字节 */
        const val CHUNK_BYTES = 10240
        const val WAV_HEADER_BYTES = 44
    }
}

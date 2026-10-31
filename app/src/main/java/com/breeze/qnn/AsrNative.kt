package com.breeze.qnn

/**
 * 语音识别（ASR）JNI 声明，对应 cpp/asr_jni.cpp。
 * 同一句柄的调用须由上层串行化（ASREngine 非线程安全）。
 */
class AsrNative {
    companion object {
        init {
            System.loadLibrary("qnndemo")
        }
    }

    /** 加载 model_config.json，返回句柄；失败返回 0，错误码见 [lastInitError]。 */
    external fun nativeCreate(configPath: String): Long

    /** 最近一次 nativeCreate 的 ASRErrorCode（0 为成功）。 */
    external fun nativeLastInitError(): Int

    /** 送入一段 16-bit LE / 16 kHz / 单声道 PCM，返回该段识别文本（失败返回空串）。 */
    external fun nativeGenerate(handle: Long, pcm: ByteArray, isFinal: Boolean): String

    /** 释放句柄。 */
    external fun nativeRelease(handle: Long)
}

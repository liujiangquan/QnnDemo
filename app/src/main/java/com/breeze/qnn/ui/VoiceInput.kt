package com.breeze.qnn.ui

import android.annotation.SuppressLint
import android.content.Context
import android.media.AudioFormat
import android.media.AudioRecord
import android.media.MediaRecorder
import androidx.lifecycle.LifecycleCoroutineScope
import com.breeze.qnn.AsrBackend
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.Job
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import android.util.Log

/**
 * 语音输入：麦克风录音（16 kHz 单声道 PCM16）→ 320 ms 分块 → [AsrBackend] 流式识别。
 *
 * 调用方式：[start] 开始录音，[stop] 结束并输出最终文本（经 [onFinal] 回调）。
 * 录音与识别都在 [scope] 内串行执行，[stop] 会补零冲刷最后一段。
 */
class VoiceInput(
    private val context: Context,
    private val scope: LifecycleCoroutineScope,
    private val onPartial: (String) -> Unit,
    private val onFinal: (String) -> Unit,
    private val onError: (String) -> Unit,
) {
    private var backend: AsrBackend? = null
    private var recorder: AudioRecord? = null
    private var job: Job? = null
    @Volatile private var recording = false

    val isRecording: Boolean get() = recording

    /** 开始录音。调用前须已取得 RECORD_AUDIO 权限。 */
    @SuppressLint("MissingPermission")
    fun start() {
        if (recording) return
        recording = true
        val minBuf = AudioRecord.getMinBufferSize(
            SAMPLE_RATE, AudioFormat.CHANNEL_IN_MONO, AudioFormat.ENCODING_PCM_16BIT)
        val rec = AudioRecord(
            MediaRecorder.AudioSource.MIC, SAMPLE_RATE,
            AudioFormat.CHANNEL_IN_MONO, AudioFormat.ENCODING_PCM_16BIT,
            maxOf(minBuf, AsrBackend.CHUNK_BYTES * 2))
        recorder = rec

        job = scope.launch {
            val b = backend ?: AsrBackend(context.applicationContext).also { backend = it }
            if (!b.init()) {
                recording = false
                onError("语音模型加载失败，请运行 docs/setup_voice_asr.sh")
                return@launch
            }
            rec.startRecording()
            val buf = ByteArray(AsrBackend.CHUNK_BYTES)
            var filled = 0
            var reads = 0
            val sb = StringBuilder()
            while (recording) {
                val n = withContext(Dispatchers.IO) {
                    rec.read(buf, filled, AsrBackend.CHUNK_BYTES - filled)
                }
                reads++
                if (reads <= 3 || reads % 50 == 0) Log.i(TAG, "read n=$n filled=$filled reads=$reads")
                if (n <= 0) continue
                filled += n
                if (filled == AsrBackend.CHUNK_BYTES) {
                    val part = b.feed(buf.copyOf(), isFinal = false)
                    if (part.isNotEmpty()) {
                        sb.append(part)
                        onPartial(sb.toString())
                    }
                    filled = 0
                }
            }
            // 停止：补零冲刷残余音频，取最后一段
            val tail = ByteArray(AsrBackend.CHUNK_BYTES)
            if (filled > 0) System.arraycopy(buf, 0, tail, 0, filled)
            Log.i(TAG, "stop: flushing filled=$filled totalReads=$reads")
            val last = b.feed(tail, isFinal = true)
            Log.i(TAG, "final text len=${last.length}")
            sb.append(last)
            releaseRecorder()
            onFinal(sb.toString().trim())
        }
    }

    /** 结束录音，触发 [onFinal]。 */
    fun stop() {
        recording = false
    }

    private fun releaseRecorder() {
        recorder?.let {
            runCatching { it.stop() }
            it.release()
        }
        recorder = null
    }

    /** 释放全部资源（Fragment 销毁时调用）。 */
    fun close() {
        recording = false
        job?.cancel()
        releaseRecorder()
        backend?.close()
        backend = null
    }

    companion object {
        private const val SAMPLE_RATE = 16000
        private const val TAG = "VoiceInput"
    }
}

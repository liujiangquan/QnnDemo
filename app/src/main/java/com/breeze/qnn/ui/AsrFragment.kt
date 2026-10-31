package com.breeze.qnn.ui

import android.Manifest
import android.annotation.SuppressLint
import android.content.pm.PackageManager
import android.media.AudioFormat
import android.media.AudioRecord
import android.media.MediaRecorder
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.Toast
import androidx.activity.result.contract.ActivityResultContracts
import androidx.core.content.ContextCompat
import androidx.fragment.app.Fragment
import androidx.lifecycle.lifecycleScope
import com.breeze.qnn.AsrBackend
import com.breeze.qnn.R
import com.breeze.qnn.databinding.FragmentAsrBinding
import java.io.File
import kotlinx.coroutines.Job
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import kotlinx.coroutines.Dispatchers

/**
 * 语音识别标签页：识别样例 WAV、麦克风录音（16 kHz 单声道）实时识别。
 */
class AsrFragment : Fragment() {

    private var _binding: FragmentAsrBinding? = null
    private val binding get() = _binding!!

    private var backend: AsrBackend? = null
    private var recorder: AudioRecord? = null
    private var recordJob: Job? = null
    @Volatile private var recording = false
    @Volatile private var busy = false

    private val permissionLauncher =
        registerForActivityResult(ActivityResultContracts.RequestPermission()) { granted ->
            if (granted) startRecording()
            else toast("需要麦克风权限才能录音")
        }

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?, savedInstanceState: Bundle?
    ): View {
        _binding = FragmentAsrBinding.inflate(inflater, container, false)
        return binding.root
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)
        binding.btnAsrSample.setOnClickListener { runSample() }
        binding.btnAsrRecord.setOnClickListener {
            if (recording) stopRecording() else ensurePermissionThenRecord()
        }
    }

    /** 懒加载模型，首次使用时在 native 线程初始化。 */
    private suspend fun ensureBackend(): AsrBackend? {
        val b = backend ?: AsrBackend(requireContext().applicationContext).also { backend = it }
        return if (b.init()) b else null
    }

    private fun runSample() {
        if (busy || recording) return
        val wav = File(requireContext().filesDir, "asr/asr_example.wav")
        if (!wav.isFile) {
            toast("样例不存在：请先运行 docs/setup_voice_asr.sh")
            return
        }
        busy = true
        setStatus("加载模型…")
        viewLifecycleOwner.lifecycleScope.launch {
            val b = ensureBackend()
            if (b == null) {
                setStatus("模型加载失败，请查看 logcat")
                busy = false
                return@launch
            }
            setStatus("识别样例…")
            val text = b.transcribeWav(wav)
            setResult(text)
            setStatus("完成")
            busy = false
        }
    }

    private fun ensurePermissionThenRecord() {
        if (busy) return
        val granted = ContextCompat.checkSelfPermission(
            requireContext(), Manifest.permission.RECORD_AUDIO
        ) == PackageManager.PERMISSION_GRANTED
        if (granted) startRecording()
        else permissionLauncher.launch(Manifest.permission.RECORD_AUDIO)
    }

    @SuppressLint("MissingPermission")
    private fun startRecording() {
        if (recording) return
        recording = true
        binding.btnAsrRecord.text = "停止录音"
        setResult("")
        setStatus("录音中…")

        val minBuf = AudioRecord.getMinBufferSize(
            SAMPLE_RATE, AudioFormat.CHANNEL_IN_MONO, AudioFormat.ENCODING_PCM_16BIT
        )
        val rec = AudioRecord(
            MediaRecorder.AudioSource.MIC, SAMPLE_RATE,
            AudioFormat.CHANNEL_IN_MONO, AudioFormat.ENCODING_PCM_16BIT,
            maxOf(minBuf, AsrBackend.CHUNK_BYTES * 2)
        )
        recorder = rec

        recordJob = viewLifecycleOwner.lifecycleScope.launch {
            val b = ensureBackend()
            if (b == null) {
                setStatus("模型加载失败，请查看 logcat")
                stopRecordingInternal()
                return@launch
            }
            rec.startRecording()
            val buf = ByteArray(AsrBackend.CHUNK_BYTES)
            var filled = 0
            // 320 ms 一块流式送入；不足一块的残余在停止时补零作为最后一段
            while (recording) {
                val n = withContext(Dispatchers.IO) {
                    rec.read(buf, filled, AsrBackend.CHUNK_BYTES - filled)
                }
                if (n <= 0) continue
                filled += n
                if (filled == AsrBackend.CHUNK_BYTES) {
                    val text = b.feed(buf.copyOf(), isFinal = false)
                    if (text.isNotEmpty()) appendResult(text)
                    filled = 0
                }
            }
            // 停止：补零冲刷剩余音频，输出最后一段
            val tail = ByteArray(AsrBackend.CHUNK_BYTES)
            if (filled > 0) System.arraycopy(buf, 0, tail, 0, filled)
            val last = b.feed(tail, isFinal = true)
            if (last.isNotEmpty()) appendResult(last)
            setStatus("完成")
            stopRecordingInternal()
        }
    }

    private fun stopRecording() {
        recording = false
        binding.btnAsrRecord.text = "开始录音"
    }

    private fun stopRecordingInternal() {
        recording = false
        recorder?.let {
            runCatching { it.stop() }
            it.release()
        }
        recorder = null
        if (_binding != null) binding.btnAsrRecord.text = "开始录音"
    }

    private fun setStatus(s: String) = uiRun { binding.tvAsrStatus.text = s }

    private fun setResult(s: String) = uiRun { binding.tvAsrResult.text = s }

    private fun appendResult(s: String) = uiRun {
        binding.tvAsrResult.text = binding.tvAsrResult.text.toString() + s
    }

    private fun toast(msg: String) = uiRun { Toast.makeText(requireContext(), msg, Toast.LENGTH_SHORT).show() }

    private inline fun uiRun(crossinline block: () -> Unit) {
        activity?.runOnUiThread { if (_binding != null) block() }
    }

    override fun onDestroyView() {
        // 停止录音并等待协程退出，再释放资源
        recording = false
        recordJob?.cancel()
        stopRecordingInternal()
        backend?.close()
        backend = null
        _binding = null
        super.onDestroyView()
    }

    companion object {
        private const val SAMPLE_RATE = 16000
    }
}

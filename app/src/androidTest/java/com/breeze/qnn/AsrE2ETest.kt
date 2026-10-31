package com.breeze.qnn

import androidx.test.ext.junit.runners.AndroidJUnit4
import androidx.test.platform.app.InstrumentationRegistry
import kotlinx.coroutines.runBlocking
import org.junit.After
import org.junit.Assert.assertTrue
import org.junit.Assume.assumeTrue
import org.junit.Before
import org.junit.Test
import org.junit.runner.RunWith
import java.io.File

/**
 * ASR 回归：voice_asr 模型经 AsrBackend（libstep_infer ASREngine）识别样例 WAV。
 *
 * 需要设备预置：files/asr/voice_asr/（setup_voice_asr.sh）与 files/asr/asr_example.wav。
 * 缺失时 skip，不计为通过。用 am instrument 运行，保留 files。
 */
@RunWith(AndroidJUnit4::class)
class AsrE2ETest {

    private lateinit var backend: AsrBackend
    private lateinit var wav: File

    @Before
    fun setup() {
        val ctx = InstrumentationRegistry.getInstrumentation().targetContext
        wav = File(ctx.filesDir, "asr/asr_example.wav")
        assumeTrue("样例 WAV 缺失，请先运行 setup_voice_asr.sh", wav.isFile)
        backend = AsrBackend(ctx)
        assumeTrue("voice_asr 模型未就绪", backend.modelReady())
    }

    @After
    fun teardown() {
        backend.close()
    }

    @Test
    fun asrTranscribesSample(): Unit = runBlocking {
        assumeTrue("ASREngine::init 失败", backend.init())
        val text = backend.transcribeWav(wav)
        assertTrue("识别结果为空", text.isNotBlank())
        assertTrue("识别结果不含预期语句：$text", text.contains("语音识别"))
    }
}

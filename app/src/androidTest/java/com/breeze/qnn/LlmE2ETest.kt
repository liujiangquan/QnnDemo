package com.breeze.qnn

import android.util.Log
import androidx.test.ext.junit.runners.AndroidJUnit4
import androidx.test.platform.app.InstrumentationRegistry
import kotlinx.coroutines.delay
import kotlinx.coroutines.launch
import kotlinx.coroutines.runBlocking
import org.junit.After
import org.junit.Assume.assumeTrue
import org.junit.Before
import org.junit.Test
import org.junit.runner.RunWith
import kotlin.system.measureTimeMillis

private const val TAG = "LlmE2ETest"
private const val MODEL_ID = "qwen3-3b"

@RunWith(AndroidJUnit4::class)
class LlmE2ETest {

    private lateinit var backend: LlmBackend

    @Before fun setup() {
        val ctx = InstrumentationRegistry.getInstrumentation().targetContext
        backend = LlmBackend(ctx)
        val ok = backend.init()
        assumeTrue("LlmBackend init 失败", ok)
        assumeTrue("权重未预置，跳过。请先 bash docs/setup_qwen3_llm.sh",
            backend.weightsReady(MODEL_ID))
    }

    @After fun teardown() { backend.close() }

    @Test fun llmLoadAndPrefill() = runBlocking {
        val ms = measureTimeMillis { assert(backend.loadModel(MODEL_ID)) }
        assert(ms <= 10_000) { "loadModel 耗时 ${ms}ms 应 ≤ 10s" }
        Log.i(TAG, "PASS: loadModel elapsed=${ms}ms")
    }

    @Test fun llmGenerate() = runBlocking {
        backend.loadModel(MODEL_ID)
        val tokens = StringBuilder()
        var stats: GenerateStats? = null
        var errMsg: String? = null
        backend.generate(
            prompt = "介绍下苏州，一句话",
            params = SamplingParams(temperature = 0.7f, maxTokens = 100),
            onToken = { tokens.append(it) },
            onComplete = { stats = it },
            onError = { errMsg = it },
        )
        assert(errMsg == null) { "generate 报错: $errMsg" }
        assert(tokens.length >= 5) { "应生成非空输出，实际='$tokens'" }
        assert(stats != null)
        assert(stats!!.tokensPerSec >= 15.0) { "tokens/s ${stats!!.tokensPerSec} 应 ≥ 15" }
        assert(tokens.any { it in '一'..'鿿' }) { "回答应含中文" }
        Log.i(TAG, "PASS: tokens=${stats!!.tokensGenerated} " +
            "speed=${stats!!.tokensPerSec} tok/s output='${tokens.take(60)}'")
    }

    @Test fun llmStop() = runBlocking {
        backend.loadModel(MODEL_ID)
        val job = launch {
            backend.generate(
                prompt = "写一个 500 字的故事，请详细展开",
                params = SamplingParams(maxTokens = 500),
                onToken = {}, onComplete = {}, onError = {},
            )
        }
        delay(1000)
        val stopMs = measureTimeMillis { backend.stop(); job.join() }
        assert(stopMs <= 3000) { "stop 耗时 ${stopMs}ms 应 ≤ 3s" }
        Log.i(TAG, "PASS: stop elapsed=${stopMs}ms")
    }

    @Test fun llmMultiTurn() = runBlocking {
        backend.loadModel(MODEL_ID)
        var round1 = ""
        backend.generate("我叫小明。你叫什么？",
            SamplingParams(maxTokens = 50),
            onToken = { round1 += it }, onComplete = {}, onError = {})
        val history = buildString {
            append("用户：我叫小明。你叫什么？\n助手：$round1\n用户：请问我叫什么？")
        }
        var round2 = ""
        backend.generate(history, SamplingParams(maxTokens = 50),
            onToken = { round2 += it }, onComplete = {}, onError = {})
        assert(round2.contains("小明")) { "第二轮应记得名字，实际='$round2'" }
        Log.i(TAG, "PASS: multiTurn round1='${round1.take(30)}' round2='${round2.take(30)}'")
    }
}

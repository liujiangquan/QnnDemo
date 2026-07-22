package com.breeze.qnn

import android.util.Log
import androidx.test.ext.junit.runners.AndroidJUnit4
import androidx.test.platform.app.InstrumentationRegistry
import kotlinx.coroutines.delay
import kotlinx.coroutines.launch
import kotlinx.coroutines.runBlocking
import kotlinx.coroutines.withTimeout
import org.junit.After
import org.junit.Assert.assertNotNull
import org.junit.Assert.assertTrue
import org.junit.Assume.assumeTrue
import org.junit.Before
import org.junit.Test
import org.junit.runner.RunWith
import kotlin.system.measureTimeMillis

private const val TAG = "LlmE2ETest"
private const val MODEL_ID = "qwen3-3b"

/**
 * LLM 端到端测试 —— 覆盖 loadModel / generate / stop / 多轮对话 4 个场景。
 *
 * 需要设备预置 Qwen3-3B 权重（`bash docs/setup_qwen3_llm.sh <src>`）；
 * 权重不在时 @Before 里 `assumeTrue` 会 skip 所有测试，CI 不 red。
 *
 * 跟 BackendE2ETest 平级；SDK 2.48 Genie 框架 + vendor 2.46 HTP + signed PD。
 * 详见 `docs/superpowers/plans/2026-07-22-8845-llm-qwen3.md` Task 4。
 */
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

    @Test fun llmLoadAndPrefill(): Unit = runBlocking {
        withTimeout(30_000L) {
            val ms = measureTimeMillis { assertTrue("loadModel 应返回 true", backend.loadModel(MODEL_ID)) }
            assertTrue("loadModel 耗时 ${ms}ms 应 ≤ 10s", ms <= 10_000)
            Log.i(TAG, "PASS: loadModel elapsed=${ms}ms")
        }
    }

    @Test fun llmGenerate(): Unit = runBlocking {
        withTimeout(60_000L) {
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
            assertTrue("generate 报错: $errMsg", errMsg == null)
            assertTrue("应生成非空输出，实际='$tokens'", tokens.length >= 5)
            assertNotNull("stats 应非空", stats)
            assertTrue("tokens/s ${stats!!.tokensPerSec} 应 ≥ 15", stats!!.tokensPerSec >= 15.0)
            assertTrue("回答应含中文", tokens.any { it in '一'..'鿿' })
            Log.i(TAG, "PASS: tokens=${stats!!.tokensGenerated} " +
                "speed=${stats!!.tokensPerSec} tok/s output='${tokens.take(60)}'")
        }
    }

    @Test fun llmStop(): Unit = runBlocking {
        withTimeout(60_000L) {
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
            assertTrue("stop 耗时 ${stopMs}ms 应 ≤ 3s", stopMs <= 3000)
            Log.i(TAG, "PASS: stop elapsed=${stopMs}ms")
        }
    }

    @Test fun llmMultiTurn(): Unit = runBlocking {
        withTimeout(60_000L) {
            backend.loadModel(MODEL_ID)
            var round1 = ""
            backend.generate("我叫小明。你叫什么？",
                SamplingParams(temperature = 0.0f, maxTokens = 50),
                onToken = { round1 += it }, onComplete = {}, onError = {})
            val history = buildString {
                append("用户：我叫小明。你叫什么？\n助手：$round1\n用户：请问我叫什么？")
            }
            var round2 = ""
            backend.generate(history, SamplingParams(temperature = 0.0f, maxTokens = 50),
                onToken = { round2 += it }, onComplete = {}, onError = {})
            assertTrue("第二轮应记得名字，实际='$round2'", round2.contains("小明"))
            Log.i(TAG, "PASS: multiTurn round1='${round1.take(30)}' round2='${round2.take(30)}'")
        }
    }
}

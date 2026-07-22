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
            // wrapChatTemplate 默认关 thinking：assistant 位置预填空 <think></think>，
            // 100 tokens 应该能直接看到中文答案。
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
            assertTrue("回答应含中文，实际='${tokens.take(60)}'", tokens.any { it in '一'..'鿿' })
            Log.i(TAG, "PASS: tokens=${stats!!.tokensGenerated} " +
                "speed=${stats!!.tokensPerSec} tok/s output='${tokens.take(80)}'")
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
            // 多轮走完整 chat template（含空 <think></think> 关思考模式）拼历史。
            // 100 tokens 每轮应直接给出中文答案。
            val nothink = "<think>\n\n</think>\n\n"
            val round1Prompt =
                "<|im_start|>system\nYou are a helpful assistant.<|im_end|>\n" +
                "<|im_start|>user\n我叫小明。你叫什么？<|im_end|>\n" +
                "<|im_start|>assistant\n$nothink"
            var round1 = ""
            backend.generate(round1Prompt,
                SamplingParams(temperature = 0.0f, maxTokens = 100),
                onToken = { round1 += it }, onComplete = {}, onError = {})

            val round2Prompt = round1Prompt + round1 + "<|im_end|>\n" +
                "<|im_start|>user\n请问我叫什么？<|im_end|>\n" +
                "<|im_start|>assistant\n$nothink"
            var round2 = ""
            backend.generate(round2Prompt, SamplingParams(temperature = 0.0f, maxTokens = 100),
                onToken = { round2 += it }, onComplete = {}, onError = {})
            assertTrue("第二轮应记得名字，实际='${round2.take(200)}'", round2.contains("小明"))
            Log.i(TAG, "PASS: multiTurn round1='${round1.take(80)}' round2='${round2.take(80)}'")
        }
    }

    /**
     * 强制开 thinking 模式（手拼 prompt 不预填空 <think></think>），
     * 验证 UI 侧的 ThinkFilter 把 <think>...</think> 段吞掉。
     */
    @Test fun llmThinkFilter(): Unit = runBlocking {
        withTimeout(120_000L) {
            backend.loadModel(MODEL_ID)
            // 完整 <|im_start|> prompt 会走 wrapChatTemplate 透传分支，
            // assistant 位置不预填空 think 块 → Qwen3 会输出 <think>...</think>
            val thinkingPrompt =
                "<|im_start|>user\n介绍下苏州，一句话<|im_end|>\n<|im_start|>assistant\n"
            val out = StringBuilder()
            var stats: GenerateStats? = null
            // maxTokens 1500 够跑完 think + 答案（~58s @ 26 tok/s）；
            // 短了会 MAX_TOKENS 卡在 think 里，filter 就没内容可 emit。
            backend.generate(thinkingPrompt,
                SamplingParams(temperature = 0.7f, maxTokens = 1500),
                onToken = { out.append(it) },
                onComplete = { stats = it },
                onError = {})
            val text = out.toString()
            assertTrue("UI 层不应看到 <think> 开标签，实际='${text.take(120)}'",
                !text.contains("<think>"))
            assertTrue("UI 层不应看到 </think> 闭标签，实际='${text.take(120)}'",
                !text.contains("</think>"))
            // think 段闭合了才应该有内容；若还卡在 think 里（MAX_TOKENS 提前收），filter drop 全部是正确行为
            if (stats?.stoppedReason != StopReason.MAX_TOKENS) {
                assertTrue("think 段闭合了 UI 应有实质内容，实际='${text.take(120)}'",
                    text.trim().isNotEmpty())
            }
            Log.i(TAG, "PASS: thinkFilter stop=${stats?.stoppedReason} " +
                "output='${text.take(120)}'")
        }
    }
}

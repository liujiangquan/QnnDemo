package com.breeze.qnn.laya

import org.junit.Assert.assertEquals
import org.junit.Assert.assertTrue
import org.junit.Test

class LayaPostprocessorTest {
    private val act = floatArrayOf(0.7f, 0.3f)  // 原始 act_logits，softmax 后取 [0]

    @Test fun `softmax 归一化`() {
        val p = LayaPostprocessor.softmax(floatArrayOf(1f, 1f, 1f))
        assertEquals(1.0f, p.sum(), 1e-5f)
        p.forEach { assertEquals(1f / 3, it, 1e-5f) }
    }

    @Test fun `choice 取 argmax 与概率`() {
        val q = ChoiceQuestion("q", linkedMapOf("a" to null, "b" to null, "c" to null))
        val logits = floatArrayOf(0.1f, 5.0f, 0.2f)
        val ans = LayaPostprocessor.decode(q, logits, 3, act) as ChoiceAnswer
        assertEquals("b", ans.choice)
        assertEquals(3, ans.probabilities.size)
        assertTrue(ans.probabilities["b"]!! > 0.9f)
    }

    @Test fun `score 期望值`() {
        val q = ScoreQuestion("q", listOf("low", "mid", "high"))
        val logits = floatArrayOf(-10f, -10f, 10f)
        val ans = LayaPostprocessor.decode(q, logits, 3, act) as ScoreAnswer
        assertEquals(2.0f, ans.score, 1e-2f)
    }

    @Test fun `noul 取 p1 为 P(true)`() {
        val q = NoulQuestion("q")
        val logits = floatArrayOf(-2f, 2f)
        val ans = LayaPostprocessor.decode(q, logits, 2, act) as NoulAnswer
        assertTrue(ans.noul > 0.9f)
        assertEquals(LayaPostprocessor.softmax(act)[0], ans.actProbability, 1e-5f)
    }

    @Test fun `confidence 均匀分布为 0 单点为 1`() {
        assertEquals(0f, LayaPostprocessor.confidence(floatArrayOf(0.5f, 0.5f)), 1e-4f)
        assertEquals(1f, LayaPostprocessor.confidence(floatArrayOf(1f, 0f)), 1e-3f)
    }
}

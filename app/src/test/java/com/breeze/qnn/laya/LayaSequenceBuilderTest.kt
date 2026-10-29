package com.breeze.qnn.laya

import org.junit.Assert.assertEquals
import org.junit.Assert.assertFalse
import org.junit.Assert.assertTrue
import org.junit.Test

class LayaSequenceBuilderTest {
    // stub：每个「词」映射到固定 id；空格分词；未知词用其 hash。
    private val encode: (String) -> IntArray = { text ->
        if (text.isEmpty()) IntArray(0)
        else text.trim().split(Regex("\\s+")).filter { it.isNotEmpty() }
            .map { (it.hashCode() and 0x7fff) + 100 }.toIntArray()
    }
    private val bos = 2; private val eos = 1; private val mask = 4

    private fun builder(maxLen: Int = 64, headMaxLen: Int = 48) =
        LayaSequenceBuilder(encode, bos, eos, mask, maxLen, headMaxLen)

    @Test fun `choice 布局：CLS 头 SEP 各option前有MASK SEP state SEP`() {
        val q = ChoiceQuestion("pick one", linkedMapOf("a" to null, "b" to null))
        val built = builder().build("hello world", q)
        assertEquals(bos, built.ids.first())
        assertEquals(eos, built.ids.last())
        assertEquals(2, built.markers.size)
        built.markers.forEach { assertEquals(mask, built.ids[it]) }
        assertEquals(q.renderOptions().size, built.markers.size)
    }

    @Test fun `noul 恒两个 option`() {
        val built = builder().build("s", NoulQuestion("is it true?"))
        assertEquals(2, built.markers.size)
    }

    @Test fun `超长截断到 maxLen`() {
        val longState = (1..500).joinToString(" ") { "w$it" }
        val built = builder(maxLen = 32).build(longState, NoulQuestion("q"))
        assertTrue(built.ids.size <= 32)
        built.markers.forEach { assertTrue(it < 32) }
    }

    @Test fun `scrub 掉用户文本里的 mask 字面量`() {
        val q = ChoiceQuestion("say <mask> now", linkedMapOf("x" to null))
        val built = builder().build("state", q)
        assertEquals(1, built.markers.size)
    }

    @Test fun `默认 256 长度保留结尾 EOS 并报告 state 截断`() {
        val built = LayaSequenceBuilder(encode, bos, eos, mask)
            .build((1..500).joinToString(" ") { "state$it" }, NoulQuestion("q"))
        assertEquals(256, LayaConfig.MAX_LEN)
        assertEquals(256, built.ids.size)
        assertEquals(eos, built.ids.last())
        assertTrue(built.stateTokensDropped > 0)
        assertTrue(built.truncated)
        assertFalse(built.headTruncated)
    }

    @Test fun `16 个长选项保留所有 marker 且为 state 留空间`() {
        val options = linkedMapOf<String, String?>()
        repeat(16) { options["option$it"] = (1..100).joinToString(" ") { "detail$it" } }
        val q = ChoiceQuestion((1..300).joinToString(" ") { "question$it" }, options)
        val built = LayaSequenceBuilder(encode, bos, eos, mask).build("uniqueStateToken", q)
        assertEquals(16, built.markers.size)
        built.markers.forEach { assertEquals(mask, built.ids[it]) }
        assertTrue(built.ids.contains(encode("uniqueStateToken").single()))
        assertTrue(built.headTruncated)
        assertEquals(0, built.stateTokensDropped)
        assertEquals(eos, built.ids.last())
        assertTrue(built.ids.size <= LayaConfig.HEAD_MAX_LEN + 5)
    }

    @Test fun `短输入 token 序列与旧 512 配置一致`() {
        val q = ChoiceQuestion("Which team should handle this ticket?", linkedMapOf(
            "billing" to "payments refunds invoices", "support" to "product help and bugs",
        ))
        val state = "I cancelled my subscription and have not received my refund."
        val old = builder(maxLen = 512, headMaxLen = 256).build(state, q)
        val now = LayaSequenceBuilder(encode, bos, eos, mask).build(state, q)
        assertEquals(old.ids.toList(), now.ids.toList())
        assertEquals(old.markers.toList(), now.markers.toList())
        assertFalse(now.truncated)
    }

    @Test(expected = IllegalArgumentException::class)
    fun `超过16选项不能静默丢弃`() {
        val q = ScoreQuestion("rank", List(17) { "level$it" })
        LayaSequenceBuilder(encode, bos, eos, mask).build("state", q)
    }
}

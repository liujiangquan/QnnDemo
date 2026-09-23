package com.breeze.qnn.laya

import org.junit.Assert.assertEquals
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
}

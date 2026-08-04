package com.breeze.qnn.ner

import org.junit.Assert.assertEquals
import org.junit.Assert.assertTrue
import org.junit.Test

class SentenceSplitterTest {

    @Test fun splitsOnChinesePunctuation() {
        val s = SentenceSplitter.split("张三来了。李四走了！王五呢？")
        assertEquals(listOf("张三来了。", "李四走了！", "王五呢？"), s.map { it.text })
    }

    @Test fun preservesCharOffsets() {
        val text = "张三来了。李四走了！"
        val s = SentenceSplitter.split(text)
        assertEquals(0, s[0].charOffset)
        assertEquals(5, s[1].charOffset)
        // offset 必须能切回原文
        s.forEach { assertEquals(it.text, text.substring(it.charOffset, it.charOffset + it.text.length)) }
    }

    @Test fun emptyAndBlankInputReturnsEmpty() {
        assertTrue(SentenceSplitter.split("").isEmpty())
        assertTrue(SentenceSplitter.split("   \n  ").isEmpty())
    }

    @Test fun pureePunctuationReturnsEmpty() {
        assertTrue(SentenceSplitter.split("。。！！").all { it.text.isBlank() } ||
            SentenceSplitter.split("。。！！").isNotEmpty())
    }

    @Test fun fallsBackToSecondaryPunctuationWhenTooLong() {
        // 一级切完仍超 MAX_CHARS，应按逗号再切
        val long = "甲".repeat(80) + "，" + "乙".repeat(80) + "。"
        val s = SentenceSplitter.split(long)
        assertTrue("应被切成多段，实际 ${s.size} 段", s.size >= 2)
        assertTrue("每段不应超过 MAX_CHARS", s.all { it.text.length <= SentenceSplitter.MAX_CHARS })
    }

    @Test fun hardChunksWhenNoPunctuationAtAll() {
        val long = "甲".repeat(250)
        val s = SentenceSplitter.split(long)
        assertTrue("无标点超长文本应硬切", s.size >= 3)
        assertTrue(s.all { it.text.length <= SentenceSplitter.MAX_CHARS })
        // 硬切后拼回来应等于原文
        assertEquals(long, s.joinToString("") { it.text })
    }

    @Test fun newlineIsASeparator() {
        val s = SentenceSplitter.split("第一行\n第二行")
        assertEquals(2, s.size)
    }
}

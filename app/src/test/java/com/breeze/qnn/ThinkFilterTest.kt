package com.breeze.qnn

import org.junit.Assert.assertEquals
import org.junit.Test

/**
 * 覆盖 [ThinkFilter] 的三个关键边界：
 * 1. 无 think 段 → 全部透传
 * 2. 有 think 段 → 段内被吞，段后前导空白被吞
 * 3. 跨 token 边界的开/闭标签 → 拼齐后正确识别
 */
class ThinkFilterTest {

    private fun runFilter(chunks: List<String>): String {
        val filter = ThinkFilter()
        val sb = StringBuilder()
        chunks.forEach { filter.feed(it) { s -> sb.append(s) } }
        filter.flush { s -> sb.append(s) }
        return sb.toString()
    }

    @Test fun passThroughWhenNoThinkTag() {
        assertEquals("苏州是江南水乡的代表。",
            runFilter(listOf("苏州", "是江南", "水乡的代表。")))
    }

    @Test fun stripsWholeThinkBlockAndLeadingWhitespace() {
        // 前后正常内容 + 中间完整 think 段 + 段后 \n\n 应被 trim
        val out = runFilter(listOf(
            "<think>Okay, I need to think about Suzhou. ",
            "Let me consider its features.</think>\n\n",
            "苏州是江南水乡的代表。"
        ))
        assertEquals("苏州是江南水乡的代表。", out)
    }

    @Test fun handlesTagsSplitAcrossTokenBoundaries() {
        // "<think>" 拆成 "<th" + "ink>"；"</think>" 拆成 "</thi" + "nk>"
        val out = runFilter(listOf(
            "<th", "ink>", "reasoning text", "</thi", "nk>\n\n", "答案在这里"
        ))
        assertEquals("答案在这里", out)
    }

    @Test fun emitsPreThinkContent() {
        // think 段之前的内容应正常输出
        val out = runFilter(listOf(
            "prefix ", "<think>cot</think>", " suffix"
        ))
        assertEquals("prefix suffix", out)
    }

    @Test fun holdsBackPartialOpenTagUntilFlush() {
        // 只喂一半开标签，flush 时把 "<thi" 当普通文本 emit（模型输出未闭合的情况）
        val out = runFilter(listOf("hello <thi"))
        assertEquals("hello <thi", out)
    }

    @Test fun dropsUnclosedThinkOnFlush() {
        // 开了 think 但没闭合 → flush 时丢掉整段 think buffer
        val out = runFilter(listOf("start ", "<think>never closed"))
        assertEquals("start ", out)
    }

    @Test fun multipleThinkBlocks() {
        val out = runFilter(listOf(
            "a<think>x</think>b<think>y</think>c"
        ))
        assertEquals("abc", out)
    }
}

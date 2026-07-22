package com.breeze.qnn

/**
 * 流式过滤 `<think>...</think>` 段。
 *
 * Qwen3 是 reasoning model，思考模式（`enable_thinking=True`）下会先输出 `<think>...</think>`
 * 再给出正式答案。UI 场景下用户只想看结果，用这个 filter 把 think 段吞掉。
 *
 * 需要处理三种边界：
 * 1. **跨 token 边界的开/闭标签**：`<think>` 可能作为 `<th` + `ink>` 两个 token 到达
 * 2. **未闭合的 think 段**：模型异常时可能只发 `<think>` 不发 `</think>`；flush() 时按不需要输出处理
 * 3. **think 段后的多余空白**：Qwen3 常在 `</think>` 后接 `\n\n`，UI 不希望看到——skip 掉 tag 之后紧跟的 whitespace
 *
 * 用法：
 * ```
 * val filter = ThinkFilter()
 * genieCallback { token -> filter.feed(token) { text -> uiEmit(text) } }
 * onDone { filter.flush { text -> uiEmit(text) } }  // 输出可能被 hold 住的 tail
 * ```
 */
class ThinkFilter {
    private val buffer = StringBuilder()
    private var cursor = 0           // 已经处理过（emit 或跳过）的 buffer 位置
    private var inThink = false
    private var skipLeadingWs = false // 刚出 </think>，下一批 emit 前跳掉前导空白

    /** 收到一段新的 token 文本；把 UI 应显示部分通过 emit 传出去。 */
    fun feed(chunk: String, emit: (String) -> Unit) {
        buffer.append(chunk)
        advance(emit)
    }

    /** 生成结束时调，把 hold 住的 tail flush 出去（未闭合的 think 段直接丢）。 */
    fun flush(emit: (String) -> Unit) {
        if (!inThink && cursor < buffer.length) {
            emitSlice(buffer.substring(cursor), emit)
            cursor = buffer.length
        }
    }

    /** 从 cursor 起继续推进状态机；能 emit 的都 emit，遇到不确定就等更多输入。 */
    private fun advance(emit: (String) -> Unit) {
        while (cursor < buffer.length) {
            if (inThink) {
                val end = buffer.indexOf(CLOSE, cursor)
                if (end < 0) {
                    // 闭合标签还没来齐；hold 住剩余 buffer 等下一次 feed
                    return
                }
                cursor = end + CLOSE.length
                inThink = false
                skipLeadingWs = true
            } else {
                val next = buffer.indexOf(OPEN, cursor)
                if (next < 0) {
                    // 没有完整 <think>，emit 到"安全边界"（避免 emit 出 <thi 这种半标签）
                    val safeEnd = safeEmitEnd()
                    if (safeEnd > cursor) {
                        emitSlice(buffer.substring(cursor, safeEnd), emit)
                        cursor = safeEnd
                    }
                    return
                }
                if (next > cursor) {
                    emitSlice(buffer.substring(cursor, next), emit)
                }
                cursor = next + OPEN.length
                inThink = true
            }
        }
    }

    /** emit 时按需吃掉 </think> 后紧跟的前导空白，只吃一次。 */
    private fun emitSlice(text: String, emit: (String) -> Unit) {
        val out = if (skipLeadingWs) {
            skipLeadingWs = false
            text.trimStart()
        } else text
        if (out.isNotEmpty()) emit(out)
    }

    /**
     * 若 buffer 结尾是 `<think>` 的真子前缀（如 `<`, `<t`, ..., `<think`），
     * 就把它留在 buffer 里等下次 feed 拼齐；否则整段可 emit。
     */
    private fun safeEmitEnd(): Int {
        val len = buffer.length
        for (i in OPEN.length - 1 downTo 1) {
            if (len - i >= cursor && buffer.subSequence(len - i, len).toString() == OPEN.substring(0, i)) {
                return len - i
            }
        }
        return len
    }

    companion object {
        private const val OPEN = "<think>"
        private const val CLOSE = "</think>"
    }
}

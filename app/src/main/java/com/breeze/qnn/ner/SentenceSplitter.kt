package com.breeze.qnn.ner

/** 一个待推理的句子及其在原文中的起始字符偏移。 */
data class Sentence(val text: String, val charOffset: Int)

/**
 * 中文按标点切句。
 *
 * BERT NER 的 seq_len 固定 128，长文本必须切开逐句跑。中文句号/问号/叹号/分号是
 * 天然的实体边界，实体跨句概率极低，所以不需要滑窗 + overlap 去重。
 *
 * 每个 [Sentence] 带原文 charOffset，供上层把识别结果映射回完整文本做高亮。
 */
object SentenceSplitter {

    /** 一级切分符：句末标点 */
    private val PRIMARY = charArrayOf('。', '！', '？', '；', '\n', '.', '!', '?', ';')

    /** 二级切分符：一级切完还超长时用逗号类再切 */
    private val SECONDARY = charArrayOf('，', '、', ',', '：', ':')

    /**
     * 单句字符上限。中文最坏情况每字 1 token，加 [CLS]/[SEP] 后仍 ≤ 128，
     * 取 100 是保守值（留给英文 subword 膨胀的余量）。
     */
    const val MAX_CHARS = 100

    fun split(text: String): List<Sentence> {
        if (text.isBlank()) return emptyList()
        val out = ArrayList<Sentence>()
        for (seg in splitBy(text, 0, PRIMARY)) {
            if (seg.text.length <= MAX_CHARS) {
                out += seg
            } else {
                // 一级切完仍超长 → 二级标点再切
                for (sub in splitBy(seg.text, seg.charOffset, SECONDARY)) {
                    if (sub.text.length <= MAX_CHARS) {
                        out += sub
                    } else {
                        out += hardChunk(sub)
                    }
                }
            }
        }
        return out
    }

    /** 按给定分隔符切分，分隔符归入前一段，保留原文偏移。 */
    private fun splitBy(text: String, baseOffset: Int, delims: CharArray): List<Sentence> {
        val out = ArrayList<Sentence>()
        var start = 0
        for (i in text.indices) {
            if (text[i] in delims) {
                val piece = text.substring(start, i + 1)
                if (piece.isNotBlank()) out += Sentence(piece, baseOffset + start)
                start = i + 1
            }
        }
        if (start < text.length) {
            val tail = text.substring(start)
            if (tail.isNotBlank()) out += Sentence(tail, baseOffset + start)
        }
        return out
    }

    /** 无标点的超长段：按 MAX_CHARS 硬切。 */
    private fun hardChunk(s: Sentence): List<Sentence> {
        val out = ArrayList<Sentence>()
        var i = 0
        while (i < s.text.length) {
            val end = minOf(i + MAX_CHARS, s.text.length)
            out += Sentence(s.text.substring(i, end), s.charOffset + i)
            i = end
        }
        return out
    }
}

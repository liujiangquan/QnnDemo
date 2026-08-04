package com.breeze.qnn.ner

import java.io.InputStream
import java.nio.ByteBuffer
import java.nio.ByteOrder

/**
 * tokenize 结果。三个 int 数组定长 [WordPieceTokenizer.MAX_LEN]，可直接喂 DLC。
 *
 * @param offsets 每个 token 对应**句内**字符区间；`[CLS]`/`[SEP]`/`[PAD]` 位置为 `IntRange.EMPTY`
 * @param validLen 含 `[CLS]`/`[SEP]` 的有效 token 数（= attention_mask 里 1 的个数）
 */
data class TokenizedInput(
    val ids: IntArray,
    val mask: IntArray,
    val typeIds: IntArray,
    val offsets: List<IntRange>,
    val validLen: Int,
) {
    /** 转成 DLC 需要的 little-endian int32 字节序（QNN 张量是 Int_32）。 */
    fun toTensorBytes(): List<ByteArray> = listOf(ids, mask, typeIds).map { arr ->
        ByteBuffer.allocate(arr.size * 4).order(ByteOrder.LITTLE_ENDIAN).apply {
            arr.forEach { putInt(it) }
        }.array()
    }
}

/**
 * 中文 BERT WordPiece tokenizer。
 *
 * 算法与 `tools/gen_ner_calibration.py` 里的 Python 实现 1:1 对应 —— 那份已经用
 * onnxruntime 跑 ONNX 原模型交叉验证过输出正确（例："马化腾在深圳创办了腾讯公司"
 * 正确切出 `[CLS] 马 化 腾 在 深 圳 创 办 了 腾 讯 公 司 。 [SEP]`）。改这里务必同步改那边。
 *
 * 规则：
 * - CJK 字符与标点各自成 token，直接查表
 * - 连续 ASCII 字母/数字合成 word，走 greedy longest-match subword（后续片段加 `##`）
 * - 查不到映射为 `[UNK]`
 */
class WordPieceTokenizer(vocabLines: List<String>) {

    private val vocab: Map<String, Int> = HashMap<String, Int>(vocabLines.size * 2).apply {
        vocabLines.forEachIndexed { idx, token -> putIfAbsent(token, idx) }
    }

    private val padId = requireId(PAD)
    private val unkId = requireId(UNK)
    private val clsId = requireId(CLS)
    private val sepId = requireId(SEP)

    private fun requireId(token: String): Int =
        vocab[token] ?: error("vocab 缺少特殊 token: $token")

    val vocabSize: Int get() = vocab.size

    fun encode(text: String): TokenizedInput {
        val ids = IntArray(MAX_LEN) { padId }
        val mask = IntArray(MAX_LEN)
        val offsets = ArrayList<IntRange>(MAX_LEN)

        ids[0] = clsId
        offsets += IntRange.EMPTY
        var n = 1

        outer@ for ((word, wordStart) in basicSplit(text)) {
            var consumed = 0
            for (piece in wordPiece(word)) {
                if (n >= MAX_LEN - 1) break@outer  // 留一格给 [SEP]
                ids[n] = vocab[piece] ?: unkId
                // subword 的 ## 前缀不占原文字符
                val rawLen = if (piece.startsWith("##")) piece.length - 2 else piece.length
                val len = if (piece == UNK) word.length else rawLen
                offsets += (wordStart + consumed) until (wordStart + consumed + len)
                consumed += len
                n++
            }
        }

        ids[n] = sepId
        offsets += IntRange.EMPTY
        n++

        for (i in 0 until n) mask[i] = 1
        while (offsets.size < MAX_LEN) offsets += IntRange.EMPTY

        return TokenizedInput(ids, mask, IntArray(MAX_LEN), offsets, n)
    }

    /** 切成 (word, 句内起始偏移)。CJK 与标点各自成词，连续 ASCII 成词。 */
    private fun basicSplit(text: String): List<Pair<String, Int>> {
        val out = ArrayList<Pair<String, Int>>()
        val buf = StringBuilder()
        var bufStart = -1

        fun flush() {
            if (buf.isNotEmpty()) {
                out += buf.toString() to bufStart
                buf.setLength(0)
                bufStart = -1
            }
        }

        for (i in text.indices) {
            val ch = text[i]
            when {
                isCjk(ch) || isPunctOrSymbol(ch) -> { flush(); out += ch.toString() to i }
                ch.isWhitespace() -> flush()
                else -> {
                    if (buf.isEmpty()) bufStart = i
                    buf.append(ch)
                }
            }
        }
        flush()
        return out
    }

    /** greedy longest-match-first；失败返回 `[UNK]` 单元素。 */
    private fun wordPiece(word: String): List<String> {
        if (vocab.containsKey(word)) return listOf(word)
        val lowered = word.lowercase()
        if (vocab.containsKey(lowered)) return listOf(lowered)

        val out = ArrayList<String>()
        var start = 0
        while (start < lowered.length) {
            var end = lowered.length
            var found: String? = null
            while (start < end) {
                val piece = if (start > 0) "##" + lowered.substring(start, end)
                            else lowered.substring(start, end)
                if (vocab.containsKey(piece)) { found = piece; break }
                end--
            }
            if (found == null) return listOf(UNK)
            out += found
            start = end
        }
        return out
    }

    private fun isCjk(ch: Char): Boolean {
        val cp = ch.code
        return cp in 0x4E00..0x9FFF || cp in 0x3400..0x4DBF || cp in 0xF900..0xFAFF
    }

    private fun isPunctOrSymbol(ch: Char): Boolean {
        val cp = ch.code
        if (cp in 33..47 || cp in 58..64 || cp in 91..96 || cp in 123..126) return true
        return when (Character.getType(ch)) {
            Character.CONNECTOR_PUNCTUATION.toInt(),
            Character.DASH_PUNCTUATION.toInt(),
            Character.START_PUNCTUATION.toInt(),
            Character.END_PUNCTUATION.toInt(),
            Character.INITIAL_QUOTE_PUNCTUATION.toInt(),
            Character.FINAL_QUOTE_PUNCTUATION.toInt(),
            Character.OTHER_PUNCTUATION.toInt() -> true
            else -> false
        }
    }

    companion object {
        /** 与 DLC 编译期固定的 seq_len 一致，不可改 */
        const val MAX_LEN = 128

        const val PAD = "[PAD]"
        const val UNK = "[UNK]"
        const val CLS = "[CLS]"
        const val SEP = "[SEP]"

        /** 从 assets / resources 的 vocab.txt 流构造 */
        fun fromStream(input: InputStream): WordPieceTokenizer =
            input.bufferedReader().use { WordPieceTokenizer(it.readLines()) }
    }
}

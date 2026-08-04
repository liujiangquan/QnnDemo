package com.breeze.qnn.ner

import java.nio.ByteBuffer
import java.nio.ByteOrder
import kotlin.math.exp

/**
 * 把 BERT token-classification 的 logits 解成实体列表。
 *
 * 标签顺序来自模型 `onnx/config.json` 的 `id2label`，**不可改**。
 *
 * 容错策略对齐 HuggingFace 的 simple 聚合：`I-X` 前面不是 `B-X`/`I-X` 时按 `B-X` 处理，
 * 因为量化/边界情况下模型偶尔会漏掉起始的 B 标签。
 */
object BioDecoder {

    /** 索引即 label id，顺序来自 config.json 的 id2label */
    val ID2LABEL = arrayOf(
        "I-ORG", "B-LOC", "O", "B-ORG", "I-LOC",
        "I-PER", "B-TIME", "I-TIME", "B-PER",
    )

    const val NUM_LABELS = 9

    /** 从 DLC 输出的 little-endian float32 字节解析成 [MAX_LEN][9] */
    fun parseLogits(bytes: ByteArray): Array<FloatArray> {
        val bb = ByteBuffer.wrap(bytes).order(ByteOrder.LITTLE_ENDIAN)
        val rows = bytes.size / 4 / NUM_LABELS
        return Array(rows) { FloatArray(NUM_LABELS) { bb.float } }
    }

    /**
     * @param logits    [seqLen][9]
     * @param offsets   每个 token 对应句内字符区间（来自 [TokenizedInput.offsets]）
     * @param validLen  含 [CLS]/[SEP] 的有效 token 数
     * @param baseOffset 该句在完整文本中的起始偏移
     * @param fullText  完整原文，用于切出实体文本
     */
    fun decode(
        logits: Array<FloatArray>,
        offsets: List<IntRange>,
        validLen: Int,
        baseOffset: Int,
        fullText: String,
    ): List<Entity> {
        val out = ArrayList<Entity>()
        var curType: String? = null
        var curStartTok = -1
        val curScores = ArrayList<Float>()

        fun close(endTokExclusive: Int) {
            val type = curType ?: return
            val startRange = offsets.getOrNull(curStartTok)
            val endRange = offsets.getOrNull(endTokExclusive - 1)
            curType = null
            curStartTok = -1
            if (startRange == null || endRange == null ||
                startRange == IntRange.EMPTY || endRange == IntRange.EMPTY
            ) {
                curScores.clear(); return
            }
            val cs = baseOffset + startRange.first
            val ce = baseOffset + endRange.last + 1
            if (cs !in fullText.indices || ce > fullText.length || cs >= ce) {
                curScores.clear(); return
            }
            val avg = if (curScores.isEmpty()) 0f else curScores.sum() / curScores.size
            curScores.clear()
            out += Entity(
                text = fullText.substring(cs, ce),
                type = EntityType.valueOf(type),
                charStart = cs,
                charEnd = ce,
                score = avg,
                source = EntitySource.MODEL,
            )
        }

        // 跳过 [CLS]（位置 0）与 [SEP]（位置 validLen-1）
        val last = (validLen - 1).coerceAtMost(logits.size)
        for (i in 1 until last) {
            val row = logits[i]
            var best = 0
            for (k in 1 until NUM_LABELS) if (row[k] > row[best]) best = k
            val label = ID2LABEL[best]

            if (label == "O") { close(i); continue }

            val dash = label.indexOf('-')
            val prefix = label.substring(0, dash)
            val type = label.substring(dash + 1)

            if (prefix == "B" || curType != type) {
                close(i)
                curType = type
                curStartTok = i
            }
            curScores += softmaxAt(row, best)
        }
        close(last)
        return out
    }

    private fun softmaxAt(row: FloatArray, idx: Int): Float {
        val max = row.max()
        var sum = 0.0
        for (v in row) sum += exp((v - max).toDouble())
        return (exp((row[idx] - max).toDouble()) / sum).toFloat()
    }
}

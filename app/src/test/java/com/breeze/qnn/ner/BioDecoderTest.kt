package com.breeze.qnn.ner

import org.junit.Assert.assertEquals
import org.junit.Assert.assertTrue
import org.junit.Test

class BioDecoderTest {

    /** 造一行 logits：让指定 label 拿最高分 */
    private fun row(labelIdx: Int, margin: Float = 5f) =
        FloatArray(BioDecoder.NUM_LABELS) { if (it == labelIdx) margin else 0f }

    private fun idOf(label: String) = BioDecoder.ID2LABEL.indexOf(label)

    /** 构造 [CLS] + 各 token + [SEP] 的 logits 与 offsets */
    private fun build(labels: List<String>): Triple<Array<FloatArray>, List<IntRange>, Int> {
        val logits = ArrayList<FloatArray>()
        val offsets = ArrayList<IntRange>()
        logits += row(idOf("O")); offsets += IntRange.EMPTY          // [CLS]
        labels.forEachIndexed { i, l -> logits += row(idOf(l)); offsets += i..i }
        logits += row(idOf("O")); offsets += IntRange.EMPTY          // [SEP]
        val validLen = labels.size + 2
        while (logits.size < WordPieceTokenizer.MAX_LEN) {
            logits += row(idOf("O")); offsets += IntRange.EMPTY
        }
        return Triple(logits.toTypedArray(), offsets, validLen)
    }

    private fun decode(labels: List<String>, text: String): List<Entity> {
        val (lg, off, vl) = build(labels)
        return BioDecoder.decode(lg, off, vl, 0, text)
    }

    @Test fun id2labelOrderMatchesModelConfig() {
        // 顺序来自 onnx/config.json，改动会导致所有识别结果错位
        assertEquals(
            listOf("I-ORG", "B-LOC", "O", "B-ORG", "I-LOC", "I-PER", "B-TIME", "I-TIME", "B-PER"),
            BioDecoder.ID2LABEL.toList(),
        )
    }

    @Test fun decodesStandardBioSequence() {
        val e = decode(listOf("B-PER", "I-PER", "I-PER", "O", "B-LOC", "I-LOC"), "马化腾在深圳")
        assertEquals(2, e.size)
        assertEquals(EntityType.PER to "马化腾", e[0].type to e[0].text)
        assertEquals(EntityType.LOC to "深圳", e[1].type to e[1].text)
    }

    @Test fun toleratesIPrefixWithoutLeadingB() {
        // 模型偶尔漏掉起始 B，按 HuggingFace simple 策略当 B 处理
        val e = decode(listOf("I-PER", "I-PER"), "小明")
        assertEquals(1, e.size)
        assertEquals(EntityType.PER, e[0].type)
        assertEquals("小明", e[0].text)
    }

    @Test fun typeSwitchClosesPreviousEntity() {
        val e = decode(listOf("B-PER", "I-LOC"), "甲乙")
        assertEquals(2, e.size)
        assertEquals(EntityType.PER, e[0].type)
        assertEquals(EntityType.LOC, e[1].type)
    }

    @Test fun allOutsideYieldsNoEntities() {
        assertTrue(decode(listOf("O", "O", "O"), "abc").isEmpty())
    }

    @Test fun entityAtSequenceEndIsClosed() {
        val e = decode(listOf("O", "B-ORG", "I-ORG"), "在腾讯")
        assertEquals(1, e.size)
        assertEquals("腾讯", e[0].text)
    }

    @Test fun scoreIsSoftmaxAveragedAndInRange() {
        val e = decode(listOf("B-PER", "I-PER"), "小明")
        assertEquals(1, e.size)
        assertTrue("score 应在 (0,1]，实际 ${e[0].score}", e[0].score > 0f && e[0].score <= 1f)
    }

    @Test fun sourceIsAlwaysModel() {
        decode(listOf("B-PER"), "甲").forEach { assertEquals(EntitySource.MODEL, it.source) }
    }

    @Test fun baseOffsetShiftsCharPositions() {
        val (lg, off, vl) = build(listOf("B-PER", "I-PER"))
        val full = "前缀。小明"
        val e = BioDecoder.decode(lg, off, vl, 3, full)
        assertEquals(1, e.size)
        assertEquals(3, e[0].charStart)
        assertEquals("小明", e[0].text)
    }

    @Test fun parseLogitsReadsLittleEndianFloat32() {
        val n = 2 * BioDecoder.NUM_LABELS
        val bb = java.nio.ByteBuffer.allocate(n * 4).order(java.nio.ByteOrder.LITTLE_ENDIAN)
        repeat(n) { bb.putFloat(it.toFloat()) }
        val parsed = BioDecoder.parseLogits(bb.array())
        assertEquals(2, parsed.size)
        assertEquals(0f, parsed[0][0], 1e-6f)
        assertEquals(9f, parsed[1][0], 1e-6f)
    }
}

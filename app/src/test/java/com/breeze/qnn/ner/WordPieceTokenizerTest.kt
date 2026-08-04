package com.breeze.qnn.ner

import org.junit.Assert.assertEquals
import org.junit.Assert.assertNotEquals
import org.junit.Assert.assertTrue
import org.junit.BeforeClass
import org.junit.Test

class WordPieceTokenizerTest {

    companion object {
        private lateinit var tok: WordPieceTokenizer

        @BeforeClass @JvmStatic fun load() {
            val stream = WordPieceTokenizerTest::class.java.classLoader!!
                .getResourceAsStream("ner_vocab.txt")!!
            tok = WordPieceTokenizer.fromStream(stream)
        }
    }

    @Test fun vocabSizeMatchesModelConfig() {
        assertEquals(21128, tok.vocabSize)
    }

    @Test fun specialTokenIdsMatchBertChinese() {
        val t = tok.encode("字")
        // [CLS]=101 ... [SEP]=102，与 config.json / vocab.txt 顺序一致
        assertEquals(101, t.ids[0])
        assertEquals(102, t.ids[t.validLen - 1])
        assertEquals(0, t.ids[t.validLen])  // [PAD]
    }

    @Test fun chineseCharsBecomeOneTokenEach() {
        val t = tok.encode("马化腾")
        // [CLS] 马 化 腾 [SEP] = 5
        assertEquals(5, t.validLen)
        // 每个中文字占原文 1 个字符
        assertEquals(0..0, t.offsets[1])
        assertEquals(1..1, t.offsets[2])
        assertEquals(2..2, t.offsets[3])
    }

    @Test fun outputIsAlwaysFixedLength() {
        for (s in listOf("短", "马化腾在深圳创办了腾讯公司。", "甲".repeat(300))) {
            val t = tok.encode(s)
            assertEquals(WordPieceTokenizer.MAX_LEN, t.ids.size)
            assertEquals(WordPieceTokenizer.MAX_LEN, t.mask.size)
            assertEquals(WordPieceTokenizer.MAX_LEN, t.typeIds.size)
            assertEquals(WordPieceTokenizer.MAX_LEN, t.offsets.size)
        }
    }

    @Test fun maskIsOneForValidAndZeroForPadding() {
        val t = tok.encode("你好")
        assertTrue((0 until t.validLen).all { t.mask[it] == 1 })
        assertTrue((t.validLen until WordPieceTokenizer.MAX_LEN).all { t.mask[it] == 0 })
    }

    @Test fun typeIdsAllZeroForSingleSentenceTask() {
        assertTrue(tok.encode("单句任务").typeIds.all { it == 0 })
    }

    @Test fun truncatesOverlongInputWithoutLosingSep() {
        val t = tok.encode("甲".repeat(300))
        assertEquals(WordPieceTokenizer.MAX_LEN, t.validLen)
        assertEquals(102, t.ids[WordPieceTokenizer.MAX_LEN - 1])  // 末位仍是 [SEP]
    }

    @Test fun asciiWordUsesSubwordWithHashPrefix() {
        val t = tok.encode("tokenization")
        // 应被切成多个 subword 而不是单个 [UNK]
        val pieces = t.validLen - 2
        assertTrue("英文长词应切 subword，实际 $pieces 片", pieces >= 2)
        assertNotEquals(100, t.ids[1])  // 不是 [UNK]
    }

    @Test fun unknownCharMapsToUnk() {
        // 用一个不在中文 BERT 词表里的字符（emoji）
        val t = tok.encode("😀")
        assertTrue("生僻字符应落到 [UNK] 或被跳过", t.validLen <= 3)
    }

    @Test fun offsetsMapBackToOriginalText() {
        val text = "马化腾在深圳"
        val t = tok.encode(text)
        for (i in 1 until t.validLen - 1) {
            val r = t.offsets[i]
            if (r == IntRange.EMPTY) continue
            assertTrue("offset $r 应落在原文范围内", r.first >= 0 && r.last < text.length)
        }
    }

    @Test fun tensorBytesAreLittleEndianInt32() {
        val t = tok.encode("字")
        val names = listOf("input_ids", "token_type_ids", "attention_mask")
        val bytes = t.toTensorBytes(names)
        assertEquals(3, bytes.size)
        assertEquals(WordPieceTokenizer.MAX_LEN * 4, bytes[0].size)
        // [CLS]=101 的 little-endian int32 = 65 00 00 00
        assertEquals(101.toByte(), bytes[0][0])
        assertEquals(0.toByte(), bytes[0][1])
    }

    @Test fun tensorBytesBindByNameNotPosition() {
        val t = tok.encode("你好")
        // 故意用 DLC 的真实顺序：input_ids, token_type_ids, attention_mask
        val b = t.toTensorBytes(listOf("input_ids", "token_type_ids", "attention_mask"))
        // 第 2 个应是全 0 的 token_type_ids，而不是 attention_mask
        assertTrue("token_type_ids 应全 0", b[1].all { it == 0.toByte() })
        // 第 3 个是 attention_mask，首位应为 1
        assertEquals(1.toByte(), b[2][0])
    }

    @Test fun tensorBytesRejectsUnknownTensorName() {
        val t = tok.encode("字")
        try {
            t.toTensorBytes(listOf("input_ids", "bogus_name"))
            throw AssertionError("未知张量名应抛异常")
        } catch (e: IllegalStateException) {
            assertTrue(e.message!!.contains("bogus_name"))
        }
    }
}

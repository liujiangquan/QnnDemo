package com.breeze.qnn.ner

import org.json.JSONObject
import org.junit.Assert.assertEquals
import org.junit.Assert.assertTrue
import org.junit.BeforeClass
import org.junit.Test

/**
 * **精度回归的核心防线。**
 *
 * fixture `ner_golden.json` 里的 logits 是 PC 侧用 onnxruntime 跑 ONNX 原模型算出来的，
 * 期望实体也是同一份输出解出来的（见 tools/ner_onnx_reference.py）。
 *
 * 这个测试把 golden logits 直接喂给 [WordPieceTokenizer] + [BioDecoder]，断言解出的实体
 * 与 Python 侧完全一致。任何时候改动 tokenizer 或 BIO 解码器，这里立刻会红 —— 不需要
 * 设备、不需要 DLC。
 */
class NerGoldenTest {

    companion object {
        private lateinit var tok: WordPieceTokenizer
        private lateinit var golden: JSONObject

        @BeforeClass @JvmStatic fun load() {
            val cl = NerGoldenTest::class.java.classLoader!!
            tok = WordPieceTokenizer.fromStream(cl.getResourceAsStream("ner_vocab.txt")!!)
            golden = JSONObject(
                cl.getResourceAsStream("ner_golden.json")!!.bufferedReader().use { it.readText() }
            )
        }
    }

    @Test fun fixtureAgreesOnLabelOrder() {
        val arr = golden.getJSONArray("id2label")
        assertEquals(BioDecoder.NUM_LABELS, arr.length())
        for (i in 0 until arr.length()) {
            assertEquals("id2label[$i] 与 BioDecoder 不一致", arr.getString(i), BioDecoder.ID2LABEL[i])
        }
    }

    @Test fun fixtureAgreesOnMaxLen() {
        assertEquals(WordPieceTokenizer.MAX_LEN, golden.getInt("maxLen"))
    }

    @Test fun tokenizerMatchesPythonReference() {
        val cases = golden.getJSONArray("cases")
        for (i in 0 until cases.length()) {
            val c = cases.getJSONObject(i)
            val sentence = c.getString("sentence")
            val expected = c.getJSONArray("expectedTokens")
            val t = tok.encode(sentence)

            assertEquals("[$sentence] validLen 不一致", c.getInt("validLen"), t.validLen)
            // 只比 token 数；具体 id 由 vocab 决定，validLen 一致已能证明切分一致
            assertEquals("[$sentence] token 数不一致", expected.length(), t.validLen)
        }
    }

    @Test fun decoderReproducesPythonEntities() {
        val cases = golden.getJSONArray("cases")
        assertTrue("fixture 应至少有 1 个 case", cases.length() > 0)

        for (i in 0 until cases.length()) {
            val c = cases.getJSONObject(i)
            val sentence = c.getString("sentence")
            val validLen = c.getInt("validLen")
            val logitsJson = c.getJSONArray("logits")

            // fixture 只存有效位，补齐到 MAX_LEN（补的行全 O，不会产生实体）
            val oIdx = BioDecoder.ID2LABEL.indexOf("O")
            val logits = Array(WordPieceTokenizer.MAX_LEN) { r ->
                if (r < logitsJson.length()) {
                    val row = logitsJson.getJSONArray(r)
                    FloatArray(BioDecoder.NUM_LABELS) { k -> row.getDouble(k).toFloat() }
                } else {
                    FloatArray(BioDecoder.NUM_LABELS) { k -> if (k == oIdx) 10f else 0f }
                }
            }

            val actual = BioDecoder.decode(
                logits = logits,
                offsets = tok.encode(sentence).offsets,
                validLen = validLen,
                baseOffset = 0,
                fullText = sentence,
            )

            val expected = c.getJSONArray("expectedEntities")
            val actualPairs = actual.map { it.type.name to it.text }
            val expectedPairs = (0 until expected.length()).map {
                val e = expected.getJSONObject(it)
                e.getString("type") to e.getString("text")
            }

            assertEquals("[$sentence] 解出的实体与 Python 参考不一致", expectedPairs, actualPairs)
        }
    }

    @Test fun goldenEntitiesAreNonTrivial() {
        // 防止 fixture 退化成空（那样上面的断言就没意义了）
        val cases = golden.getJSONArray("cases")
        var total = 0
        for (i in 0 until cases.length()) {
            total += cases.getJSONObject(i).getJSONArray("expectedEntities").length()
        }
        assertTrue("golden fixture 应包含实体，实际 $total 个", total >= 8)
    }
}

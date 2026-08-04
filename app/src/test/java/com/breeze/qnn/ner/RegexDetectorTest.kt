package com.breeze.qnn.ner

import org.junit.Assert.assertEquals
import org.junit.Assert.assertFalse
import org.junit.Assert.assertTrue
import org.junit.Test

class RegexDetectorTest {

    private fun typesIn(text: String) = RegexDetector.detect(text).map { it.type }

    @Test fun detectsMobilePhone() {
        val e = RegexDetector.detect("我的手机号是13812345678，请联系我。")
        assertEquals(1, e.size)
        assertEquals(EntityType.PHONE, e[0].type)
        assertEquals("13812345678", e[0].text)
    }

    @Test fun rejectsNonMobilePrefixAndWrongLength() {
        assertFalse(typesIn("12812345678").contains(EntityType.PHONE))  // 第二位 2 非法
        assertFalse(typesIn("1381234567").contains(EntityType.PHONE))   // 只有 10 位
    }

    @Test fun detectsEmail() {
        val e = RegexDetector.detect("邮箱 zhang.san+work@example.com.cn 收")
        assertEquals(1, e.size)
        assertEquals(EntityType.EMAIL, e[0].type)
    }

    @Test fun luhnRejectsInvalidBankCard() {
        // 全 1 的 16 位过不了 Luhn
        assertFalse(RegexDetector.luhnValid("1111111111111111"))
        // 经典 Luhn 合法测试号
        assertTrue(RegexDetector.luhnValid("4539578763621486"))
    }

    @Test fun bankCardOnlyReportedWhenLuhnPasses() {
        assertTrue(typesIn("卡号4539578763621486").contains(EntityType.BANK_CARD))
        assertFalse(typesIn("卡号1111111111111111").contains(EntityType.BANK_CARD))
    }

    @Test fun idCardChecksumValidation() {
        // 构造一个校验位正确的身份证号
        val body = "11010519491231002"
        val weights = intArrayOf(7, 9, 10, 5, 8, 4, 2, 1, 6, 3, 7, 9, 10, 5, 8, 4, 2)
        var sum = 0
        body.forEachIndexed { i, c -> sum += (c - '0') * weights[i] }
        val valid = body + "10X98765432"[sum % 11]
        assertTrue("构造的 $valid 应通过校验", RegexDetector.validateIdCard(valid))
        // 改掉校验位应失败
        val wrongLast = if (valid.last() == '1') '2' else '1'
        assertFalse(RegexDetector.validateIdCard(body + wrongLast))
    }

    @Test fun idCardWinsOverBankCardOnSameDigits() {
        val body = "11010519491231002"
        val weights = intArrayOf(7, 9, 10, 5, 8, 4, 2, 1, 6, 3, 7, 9, 10, 5, 8, 4, 2)
        var sum = 0
        body.forEachIndexed { i, c -> sum += (c - '0') * weights[i] }
        val id = body + "10X98765432"[sum % 11]
        val types = typesIn("证件号$id")
        // 18 位数字也会被银行卡正则命中，身份证规则更强应胜出
        assertTrue(types.contains(EntityType.ID_CARD))
        assertFalse(types.contains(EntityType.BANK_CARD))
    }

    @Test fun detectsLicensePlate() {
        val e = RegexDetector.detect("车牌京A12345已登记")
        assertEquals(1, e.size)
        assertEquals(EntityType.PLATE, e[0].type)
    }

    @Test fun noFalsePositivesOnPlainText() {
        assertTrue(RegexDetector.detect("今天天气不错，适合出去走走。").isEmpty())
    }

    @Test fun allRegexEntitiesHaveScoreOneAndRegexSource() {
        RegexDetector.detect("13812345678 和 a@b.cn").forEach {
            assertEquals(1.0f, it.score, 1e-6f)
            assertEquals(EntitySource.REGEX, it.source)
        }
    }

    @Test fun resultsAreSortedByCharStart() {
        val e = RegexDetector.detect("邮箱a@b.cn，电话13812345678")
        assertEquals(e.map { it.charStart }.sorted(), e.map { it.charStart })
    }

    @Test fun charOffsetsSliceBackToOriginalText() {
        val text = "联系 13812345678 或 x@y.cn"
        RegexDetector.detect(text).forEach {
            assertEquals(it.text, text.substring(it.charStart, it.charEnd))
        }
    }
}

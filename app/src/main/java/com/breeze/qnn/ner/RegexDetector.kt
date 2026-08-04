package com.breeze.qnn.ner

/**
 * 结构化敏感字段的正则识别。
 *
 * 模型 README 明确要求：手机号 / 证件号 / 银行卡 / 邮箱这类字段必须用确定性规则，
 * BERT NER 识别不了也不该指望它。银行卡加 Luhn 校验、身份证加校验位算法，
 * 否则纯长度匹配误报率极高。
 */
object RegexDetector {

    private val PHONE = Regex("""(?<!\d)1[3-9]\d{9}(?!\d)""")
    private val ID_CARD = Regex(
        """(?<!\d)[1-9]\d{5}(?:19|20)\d{2}(?:0[1-9]|1[0-2])(?:0[1-9]|[12]\d|3[01])\d{3}[\dXx](?![\dXx])"""
    )
    private val BANK_CARD = Regex("""(?<!\d)\d{16,19}(?!\d)""")
    private val EMAIL = Regex("""[\w.+-]+@[\w-]+\.[\w.]+""")
    private val PLATE = Regex(
        """[京津沪渝冀豫云辽黑湘皖鲁新苏浙赣鄂桂甘晋蒙陕吉闽贵粤青藏川宁琼][A-Z][A-Z0-9]{4,5}[A-Z0-9挂学警港澳]"""
    )

    fun detect(text: String): List<Entity> {
        val out = ArrayList<Entity>()
        // 先收更强的规则，再收 BANK_CARD —— 身份证也是 18 位数字，会被银行卡正则命中
        collect(text, ID_CARD, EntityType.ID_CARD, out) { validateIdCard(it) }
        collect(text, PHONE, EntityType.PHONE, out) { true }
        collect(text, EMAIL, EntityType.EMAIL, out) { true }
        collect(text, PLATE, EntityType.PLATE, out) { true }
        collect(text, BANK_CARD, EntityType.BANK_CARD, out) { luhnValid(it) }
        return out.sortedBy { it.charStart }
    }

    private inline fun collect(
        text: String,
        regex: Regex,
        type: EntityType,
        out: MutableList<Entity>,
        validate: (String) -> Boolean,
    ) {
        for (m in regex.findAll(text)) {
            val v = m.value
            if (!validate(v)) continue
            // 已被更强规则占掉的区间不再重复收（如身份证 vs 银行卡）
            if (out.any { it.charStart < m.range.last + 1 && m.range.first < it.charEnd }) continue
            out += Entity(
                text = v,
                type = type,
                charStart = m.range.first,
                charEnd = m.range.last + 1,
                score = 1.0f,
                source = EntitySource.REGEX,
            )
        }
    }

    /** Luhn 校验，降低 16-19 位数字的误报 */
    fun luhnValid(s: String): Boolean {
        if (s.isEmpty() || s.any { !it.isDigit() }) return false
        var sum = 0
        var alt = false
        for (i in s.indices.reversed()) {
            var d = s[i] - '0'
            if (alt) { d *= 2; if (d > 9) d -= 9 }
            sum += d
            alt = !alt
        }
        return sum % 10 == 0
    }

    /** 二代身份证第 18 位校验码（GB 11643） */
    fun validateIdCard(s: String): Boolean {
        if (s.length != 18) return false
        val weights = intArrayOf(7, 9, 10, 5, 8, 4, 2, 1, 6, 3, 7, 9, 10, 5, 8, 4, 2)
        val checkCodes = "10X98765432"
        var sum = 0
        for (i in 0 until 17) {
            val c = s[i]
            if (!c.isDigit()) return false
            sum += (c - '0') * weights[i]
        }
        return s[17].uppercaseChar() == checkCodes[sum % 11]
    }
}

package com.breeze.qnn.laya

/**
 * 预置测试样例：一段 state + 一个问题（含类型/instructions/选项）。
 *
 * 用于 App 测试界面快速验证：选择样例 → 自动填入输入数据 → 点「决策」看结果与耗时。
 * 前三个样例与真机 E2E（LayaE2ETest）用例一致，便于比对。
 */
data class LayaSample(
    val name: String,
    val state: String,
    val qtype: String,          // "choice" / "score" / "noul"
    val instructions: String,
    /** 每行一个选项；noul 为空 */
    val options: List<String> = emptyList(),
    /** 8845 真机 HTP fp16 实测结论（供人工核对；非断言。耗时随设备频率波动，见界面实时显示） */
    val expectation: String = "",
) {
    /** 选项多行文本（与 UI 的 options 输入框一致：每行一个） */
    fun optionsText(): String = options.joinToString("\n")

    /** 用于展示的输入数据摘要 */
    fun describe(): String = buildString {
        append("【state】\n").append(state).append("\n\n")
        append("【类型】").append(qtype).append("\n")
        append("【问题】").append(instructions).append("\n")
        if (options.isNotEmpty()) {
            append("【选项】\n")
            options.forEach { append("  - ").append(it).append("\n") }
        } else {
            append("【选项】(noul 无需选项，内部恒为 [false, true])\n")
        }
        if (expectation.isNotEmpty()) append("【实测】").append(expectation)
    }
}

object LayaSamples {

    /**
     * 每行 "键: 说明" → key→说明；无冒号则整行作为 key、说明为 null。
     * UI 与样例测试共用，保证喂给模型的文本一致（renderOptions 会拼回 "键: 说明"），
     * 而返回的 choice 是简短 key 而非整行。
     */
    fun parseCriteria(lines: List<String>): LinkedHashMap<String, String?> {
        val m = LinkedHashMap<String, String?>()
        for (line in lines) {
            val i = line.indexOf(':')
            if (i > 0) {
                val k = line.substring(0, i).trim()
                val d = line.substring(i + 1).trim()
                if (k.isNotEmpty()) { m[k] = d.ifEmpty { null }; continue }
            }
            m[line] = null
        }
        return m
    }

    /** 按样例类型构造 Question（与 UI 同一套解析）。 */
    fun questionOf(s: LayaSample): Question = when (s.qtype) {
        "choice" -> ChoiceQuestion(s.instructions, parseCriteria(s.options))
        "score" -> ScoreQuestion(s.instructions, s.options)
        else -> NoulQuestion(s.instructions)
    }

    /** 客服工单（英文，与 E2E 用例一致） */
    private const val TICKET_EN =
        """{"subject":"Refund not received","body":"I cancelled my subscription two weeks ago and still have not received my refund. If not resolved I will dispute with my bank."}"""

    /** 客服工单（中文，验证多语言） */
    private const val TICKET_ZH =
        "客户连续三次反馈 App 登录后立即闪退，已提交日志，情绪激动，威胁取消年度订阅。"

    val ALL: List<LayaSample> = listOf(
        LayaSample(
            name = "1. 分派团队 (choice·英文)",
            state = TICKET_EN,
            qtype = "choice",
            instructions = "Which team should handle this ticket?",
            options = listOf(
                "billing: payments, refunds, invoices",
                "support: product help and bugs",
                "sales: new purchases and upgrades",
            ),
            expectation = "choice=billing，p≈0.9999",
        ),
        LayaSample(
            name = "2. 紧急度 (score·英文)",
            state = "The customer is furious and threatens to leave.",
            qtype = "score",
            instructions = "How urgent is this ticket?",
            options = listOf("not urgent", "somewhat", "urgent", "critical"),
            expectation = "score≈1.71（0..3 区间）",
        ),
        LayaSample(
            name = "3. 流失风险 (noul·英文)",
            state = "The customer is furious and threatens to leave.",
            qtype = "noul",
            instructions = "Is the customer likely to cancel?",
            expectation = "P(true)≈0.076（模型判低）",
        ),
        LayaSample(
            name = "4. 分派团队 (choice·中文)",
            state = TICKET_ZH,
            qtype = "choice",
            instructions = "该工单应分派给哪个团队？",
            options = listOf(
                "engineering: 崩溃/技术缺陷",
                "billing: 计费与订阅",
                "support: 一般咨询",
            ),
            expectation = "choice=billing p≈0.65 / engineering p≈0.33（模型与直觉不同）",
        ),
        LayaSample(
            name = "5. 情绪倾向 (choice·中文)",
            state = "这家店的服务态度非常好，菜品也很新鲜，下次还会再来。",
            qtype = "choice",
            instructions = "这段评价的情绪倾向是？",
            options = listOf("positive: 正面", "neutral: 中性", "negative: 负面"),
            expectation = "choice=positive，p≈0.92",
        ),
        LayaSample(
            name = "6. 是否需人工介入 (noul·中文)",
            state = TICKET_ZH,
            qtype = "noul",
            instructions = "该工单是否需要人工立即介入？",
            expectation = "P(true)≈0.012（模型判低）",
        ),
    )

    val NAMES: List<String> get() = ALL.map { it.name }
}

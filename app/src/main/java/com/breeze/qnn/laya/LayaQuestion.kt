package com.breeze.qnn.laya

/** 问题类型（映射 receptron/laya types.ts）。 */
sealed interface Question {
    val instructions: String
    fun qtype(): Int
    /** 按 label 顺序渲染 option 文本；noul 恒为 [false, true]，故 p[1]==P(true)。 */
    fun renderOptions(): List<String>
}

/** criteria: option -> 描述(可空)。用 LinkedHashMap 保序。 */
data class ChoiceQuestion(
    override val instructions: String,
    val criteria: LinkedHashMap<String, String?>,
) : Question {
    override fun qtype() = LayaConfig.QTYPE_CHOICE
    override fun renderOptions() = criteria.entries.map { (k, v) -> if (v != null) "$k: $v" else k }
    val keys: List<String> get() = criteria.keys.toList()
}

/** levels: 由低到高的序数等级描述。 */
data class ScoreQuestion(
    override val instructions: String,
    val levels: List<String>,
) : Question {
    override fun qtype() = LayaConfig.QTYPE_SCORE
    override fun renderOptions() = levels.mapIndexed { i, c -> "level $i: $c" }
}

data class NoulQuestion(
    override val instructions: String,
    val trueDesc: String? = null,
    val falseDesc: String? = null,
) : Question {
    override fun qtype() = LayaConfig.QTYPE_NOUL
    override fun renderOptions() = listOf(
        "false: " + (falseDesc ?: "no, the statement does not hold"),
        "true: " + (trueDesc ?: "yes, the statement holds"),
    )
    fun typeName() = "noul"
}

fun Question.typeName(): String = when (this) {
    is ChoiceQuestion -> "choice"
    is ScoreQuestion -> "score"
    is NoulQuestion -> "noul"
}

/** 回答类型（映射 types.ts）。 */
sealed interface Answer { val actProbability: Float }
data class ChoiceAnswer(
    val choice: String, val probabilities: Map<String, Float>,
    val confidence: Float, override val actProbability: Float,
) : Answer
data class ScoreAnswer(
    val score: Float, val probabilities: Map<Int, Float>, val legend: Map<Int, String>,
    val confidence: Float, override val actProbability: Float,
) : Answer
data class NoulAnswer(val noul: Float, override val actProbability: Float) : Answer

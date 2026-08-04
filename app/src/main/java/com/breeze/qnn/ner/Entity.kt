package com.breeze.qnn.ner

/** 敏感实体类型。前四个来自 BERT NER 模型，其余来自正则规则。 */
enum class EntityType {
    PER, LOC, ORG, TIME,
    PHONE, ID_CARD, BANK_CARD, EMAIL, PLATE,
}

/** 实体来源。重叠时正则优先——确定性规则比模型可信。 */
enum class EntitySource { MODEL, REGEX }

/**
 * 一个识别出的实体。
 *
 * @param charStart 相对**完整输入文本**的字符偏移（含）
 * @param charEnd   相对**完整输入文本**的字符偏移（不含）
 * @param score     MODEL 来源为该实体内 token 的 softmax 概率均值；REGEX 来源恒为 1.0
 */
data class Entity(
    val text: String,
    val type: EntityType,
    val charStart: Int,
    val charEnd: Int,
    val score: Float,
    val source: EntitySource,
) {
    /** 与另一个实体的字符区间是否有交集 */
    fun overlaps(other: Entity): Boolean =
        charStart < other.charEnd && other.charStart < charEnd
}

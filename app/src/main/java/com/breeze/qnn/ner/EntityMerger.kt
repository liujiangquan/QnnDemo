package com.breeze.qnn.ner

/**
 * 合并模型实体与正则实体。
 *
 * 区间重叠时**正则优先** —— 确定性规则比模型可信。典型冲突：身份证号里的连续数字
 * 被模型误识别成 LOC/ORG，这时应该保留 ID_CARD。
 */
object EntityMerger {

    fun merge(model: List<Entity>, regex: List<Entity>): List<Entity> {
        val out = ArrayList<Entity>(regex)
        for (m in model) {
            if (regex.none { it.overlaps(m) }) out += m
        }
        return out.sortedWith(compareBy({ it.charStart }, { it.charEnd }))
    }
}

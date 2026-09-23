package com.breeze.qnn.laya

/**
 * 移植 receptron/laya sequence.ts::buildSequence。
 * 布局：[bos] <type> question: <ins> [eos] [mask]opt0 [mask]opt1 ... [eos] <state> [eos]
 * @param encode 文本->token（不加特殊 token）
 */
class LayaSequenceBuilder(
    private val encode: (String) -> IntArray,
    private val bosId: Int,
    private val eosId: Int,
    private val maskId: Int,
    private val maxLen: Int = LayaConfig.MAX_LEN,
    private val headMaxLen: Int = LayaConfig.HEAD_MAX_LEN,
) {
    data class Built(val ids: IntArray, val markers: IntArray)

    private fun scrub(s: String) = s.replace(LayaConfig.TOK_MASK, " ")

    fun build(state: String, q: Question): Built {
        val opts = q.renderOptions()
        var headIds = encode("${q.typeName()} question: ${scrub(q.instructions)}").toMutableList()
        var optIds = opts.map { o ->
            (listOf(maskId) + encode(" " + scrub(o)).take(LayaConfig.OPT_TOKEN_CAP))
        }
        fun total(xs: List<List<Int>>) = xs.sumOf { it.size }
        var optBudget = headMaxLen - total(optIds)
        if (optBudget < 16) {
            val per = maxOf(4, (headMaxLen - 16) / maxOf(1, optIds.size))
            optIds = optIds.map { it.take(per) }
            optBudget = headMaxLen - total(optIds)
        }
        headIds = headIds.take(maxOf(8, optBudget)).toMutableList()

        val seq = ArrayList<Int>(maxLen)
        seq.add(bosId); seq.addAll(headIds); seq.add(eosId)
        val markers = ArrayList<Int>(optIds.size)
        for (o in optIds) { markers.add(seq.size); seq.addAll(o) }
        seq.add(eosId)
        val room = maxOf(0, maxLen - seq.size - 1)
        val st = encode(scrub(state)).take(room)
        seq.addAll(st); seq.add(eosId)

        val ids = seq.take(maxLen).toIntArray()
        val kept = markers.filter { it < maxLen }.toIntArray()
        return Built(ids, kept)
    }
}

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
    data class Built(
        val ids: IntArray,
        val markers: IntArray,
        val headTruncated: Boolean = false,
        val stateTokensDropped: Int = 0,
    ) {
        val truncated: Boolean get() = headTruncated || stateTokensDropped > 0
    }

    private fun scrub(s: String) = s.replace(LayaConfig.TOK_MASK, " ")

    fun build(state: String, q: Question): Built {
        val opts = q.renderOptions()
        require(opts.size in 1..LayaConfig.MAX_OPTS) { "选项数量必须为 1..${LayaConfig.MAX_OPTS}" }
        // 留出 BOS 和三个 EOS；不能把问题头或最后的 EOS 一起截掉。
        val headBudget = minOf(headMaxLen, maxLen - 4)
        require(headBudget >= 8) { "序列长度或问题头预算过小" }
        var headIds = encode("${q.typeName()} question: ${scrub(q.instructions)}").toMutableList()
        var headTruncated = false
        var optIds = opts.map { o ->
            val tokens = encode(" " + scrub(o))
            if (tokens.size > LayaConfig.OPT_TOKEN_CAP) headTruncated = true
            listOf(maskId) + tokens.take(LayaConfig.OPT_TOKEN_CAP)
        }
        fun total(xs: List<List<Int>>) = xs.sumOf { it.size }
        var optBudget = headBudget - total(optIds)
        if (optBudget < 16) {
            val per = maxOf(4, (headBudget - 16) / optIds.size)
            optIds = optIds.map {
                if (it.size > per) headTruncated = true
                it.take(per)
            }
            optBudget = headBudget - total(optIds)
        }
        if (headIds.size > maxOf(8, optBudget)) headTruncated = true
        headIds = headIds.take(maxOf(8, optBudget)).toMutableList()
        require(headIds.size + total(optIds) <= headBudget) { "问题头预算不足以容纳所有选项" }

        val seq = ArrayList<Int>(maxLen)
        seq.add(bosId); seq.addAll(headIds); seq.add(eosId)
        val markers = ArrayList<Int>(optIds.size)
        for (o in optIds) { markers.add(seq.size); seq.addAll(o) }
        seq.add(eosId)
        val room = maxOf(0, maxLen - seq.size - 1)
        val stateIds = encode(scrub(state))
        val st = stateIds.take(room)
        seq.addAll(st); seq.add(eosId)

        return Built(seq.toIntArray(), markers.toIntArray(), headTruncated, stateIds.size - st.size)
    }
}

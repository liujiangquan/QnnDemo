package com.breeze.qnn.laya

import kotlin.math.exp
import kotlin.math.ln
import kotlin.math.max

object LayaPostprocessor {
    fun softmax(z: FloatArray): FloatArray {
        val m = z.maxOrNull() ?: 0f
        val e = FloatArray(z.size) { exp((z[it] - m).toDouble()).toFloat() }
        val s = e.sum().takeIf { it > 0f } ?: 1f
        return FloatArray(z.size) { e[it] / s }
    }

    /** 1 - 归一化熵；单点=1，均匀=0。 */
    fun confidence(p: FloatArray): Float {
        val k = p.size
        if (k < 2) return 1f
        var ent = 0.0
        for (x in p) ent -= x * ln(max(x, 1e-12f).toDouble())
        return (1.0 - ent / ln(k.toDouble())).toFloat()
    }

    /**
     * @param logits 模型 logits[0, :]（长度 = MAX_OPTS）
     * @param k 实际 option 数（只取前 k 个做 softmax）
     * @param actLogits 模型 act_logits[0, :]（长度 2，原始 logits）
     */
    fun decode(q: Question, logits: FloatArray, k: Int, actLogits: FloatArray): Answer {
        val temp = LayaConfig.TEMPERATURE
        val z = FloatArray(k) { logits[it] / temp }
        val p = softmax(z)
        val actProb = softmax(actLogits).getOrElse(0) { 0f }
        return when (q) {
            is ChoiceQuestion -> {
                val keys = q.keys
                val best = p.indices.maxByOrNull { p[it] } ?: 0
                ChoiceAnswer(
                    choice = keys.getOrElse(best) { best.toString() },
                    probabilities = keys.mapIndexed { i, kk -> kk to (p.getOrElse(i) { 0f }) }.toMap(),
                    confidence = confidence(p), actProbability = actProb,
                )
            }
            is ScoreQuestion -> {
                var s = 0f; for (i in p.indices) s += i * p[i]
                ScoreAnswer(
                    score = s,
                    probabilities = p.indices.associateWith { p[it] },
                    legend = q.levels.indices.associateWith { q.levels[it] },
                    confidence = confidence(p), actProbability = actProb,
                )
            }
            is NoulQuestion -> NoulAnswer(noul = p.getOrElse(1) { 0f }, actProbability = actProb)
        }
    }
}

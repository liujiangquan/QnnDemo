package com.breeze.qnn.laya

import android.content.Context
import android.util.Log
import com.breeze.qnn.InferenceEngine
import com.breeze.qnn.QnnNative
import java.io.File
import java.nio.ByteBuffer
import java.nio.ByteOrder

/**
 * Laya 决策后端：加载 HTP context binary + tokenizer，对 state+问题做单次前向决策。
 * batch=1，多问题循环多次前向。
 */
class LayaBackend(private val context: Context) : AutoCloseable {
    private val engine = InferenceEngine(context)
    private var tokenizer: LayaTokenizer? = null
    private lateinit var seqBuilder: LayaSequenceBuilder
    private var graphName: String = ""
    private var loaded = false

    private fun dir() = File(context.filesDir, LayaConfig.SUBDIR)
    private fun binFile() = File(dir(), LayaConfig.BIN_NAME)
    private fun tokFile() = File(dir(), LayaConfig.TOKENIZER_NAME)

    fun modelReady(): Boolean =
        binFile().let { it.exists() && it.length() > 100L * 1024 * 1024 } && tokFile().exists()

    /** 初始化 + 加载 HTP 模型 + tokenizer。 */
    suspend fun load(): Boolean {
        if (!modelReady()) { Log.e(TAG, "模型未就绪，请先运行 setup_laya.sh"); return false }
        if (!engine.init()) return false
        if (!engine.loadContextBinary(binFile().absolutePath, QnnNative.Backend.HTP)) return false
        val g = engine.graphInfos.firstOrNull() ?: return false
        graphName = g.name
        val tk = LayaTokenizer(tokFile().toPath())
        tokenizer = tk
        val bos = tk.tokenToId(LayaConfig.TOK_BOS)
        val eos = tk.tokenToId(LayaConfig.TOK_EOS)
        val mask = tk.tokenToId(LayaConfig.TOK_MASK)
        require(bos >= 0 && eos >= 0 && mask >= 0) { "特殊 token 解析失败: bos=$bos eos=$eos mask=$mask" }
        seqBuilder = LayaSequenceBuilder(tk::encode, bos, eos, mask)
        loaded = true
        Log.i(TAG, "Laya 加载成功 graph=$graphName inputs=${g.inputs.map { it.name to it.dataType }}")
        return true
    }

    /** 对一个 state 回答多个问题（保序）。 */
    suspend fun decide(state: String, questions: LinkedHashMap<String, Question>): Map<String, Answer> {
        check(loaded) { "请先 load()" }
        val padId = tokenizer!!.tokenToId(LayaConfig.TOK_PAD).coerceAtLeast(0)
        val order = engine.graphInfos.first().inputs.map { it.name }
        val out = LinkedHashMap<String, Answer>()
        for ((qid, q) in questions) {
            val built = seqBuilder.build(state, q)
            val k = built.markers.size
            val byName = mapOf(
                "input_ids" to packInt32(built.ids, padId, LayaConfig.MAX_LEN),
                "attention_mask" to packInt32Ones(built.ids.size, LayaConfig.MAX_LEN),
                "marker_pos" to packInt32(built.markers, 0, LayaConfig.MAX_OPTS),
                "marker_mask" to packMask(k, LayaConfig.MAX_OPTS),
                "qtype" to packInt32(intArrayOf(q.qtype()), 0, 1),
            )
            val inputs = order.map { byName[it] ?: error("图输入名未知: $it") }
            val res = engine.executeWithOutput(graphName, inputs)
                ?: run { Log.e(TAG, "推理失败: $qid"); continue }
            val outNames = engine.graphInfos.first().outputs.map { it.name }
            val logits = toFloats(res[outNames.indexOf("logits").coerceAtLeast(0)])
            val actIdx = outNames.indexOf("act_logits").let { if (it >= 0) it else 1 }
            val act = toFloats(res[actIdx])
            out[qid] = LayaPostprocessor.decode(q, logits, k, act)
        }
        return out
    }

    override fun close() { tokenizer?.close(); engine.close(); loaded = false }

    companion object {
        private const val TAG = "LayaBackend"

        fun packInt32(values: IntArray, pad: Int, size: Int): ByteArray {
            val bb = ByteBuffer.allocate(size * 4).order(ByteOrder.LITTLE_ENDIAN)
            for (i in 0 until size) bb.putInt(if (i < values.size) values[i] else pad)
            return bb.array()
        }
        fun packInt32Ones(n: Int, size: Int): ByteArray {
            val bb = ByteBuffer.allocate(size * 4).order(ByteOrder.LITTLE_ENDIAN)
            for (i in 0 until size) bb.putInt(if (i < n) 1 else 0)
            return bb.array()
        }
        fun packMask(realCount: Int, size: Int): ByteArray =
            ByteArray(size) { if (it < realCount) 1 else 0 }

        fun toFloats(bytes: ByteArray): FloatArray {
            val bb = ByteBuffer.wrap(bytes).order(ByteOrder.LITTLE_ENDIAN)
            return FloatArray(bytes.size / 4) { bb.float }
        }
    }
}

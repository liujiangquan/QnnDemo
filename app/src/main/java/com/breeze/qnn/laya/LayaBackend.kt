package com.breeze.qnn.laya

import android.content.Context
import android.util.Log
import com.breeze.qnn.InferenceEngine
import com.breeze.qnn.QnnNative
import kotlinx.coroutines.CancellationException
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
    val isLoaded: Boolean get() = loaded
    var lastError: String? = null
        private set
    var lastInputTruncated: Boolean = false
        private set

    private fun dir() = File(context.filesDir, LayaConfig.SUBDIR)
    private fun binFile() = File(dir(), LayaConfig.BIN_NAME)
    private fun tokFile() = File(dir(), LayaConfig.TOKENIZER_NAME)

    fun modelReady(): Boolean =
        binFile().let { it.exists() && it.length() > 100L * 1024 * 1024 } && tokFile().exists()

    /** 初始化 + 加载 HTP 模型 + tokenizer。 */
    suspend fun load(): Boolean {
        close()
        lastError = null
        try {
            check(modelReady()) { "模型未就绪，请先运行 setup_laya.sh 部署 ${LayaConfig.MAX_LEN}-token FP16 模型" }
            check(engine.init()) { "QNN 初始化失败，见 logcat" }
            check(engine.loadContextBinary(binFile().absolutePath, QnnNative.Backend.HTP)) { "HTP 模型加载失败，见 logcat" }
            val g = engine.graphInfos.singleOrNull() ?: error("Laya 模型需要恰好一张图")
            LayaModelContract.validate(g)
            graphName = g.name
            val tk = LayaTokenizer(tokFile().toPath())
            tokenizer = tk
            val bos = tk.tokenToId(LayaConfig.TOK_BOS)
            val eos = tk.tokenToId(LayaConfig.TOK_EOS)
            val mask = tk.tokenToId(LayaConfig.TOK_MASK)
            require(bos >= 0 && eos >= 0 && mask >= 0) { "特殊 token 解析失败: bos=$bos eos=$eos mask=$mask" }
            seqBuilder = LayaSequenceBuilder(tk::encode, bos, eos, mask)
            loaded = true
            Log.i(TAG, "Laya 加载成功 graph=$graphName maxLen=${LayaConfig.MAX_LEN} inputs=${g.inputs.map { it.name to it.dimensions }}")
            return true
        } catch (e: CancellationException) {
            close()
            throw e
        } catch (e: Exception) {
            lastError = e.message ?: "Laya 加载失败"
            Log.e(TAG, lastError, e)
            close()
            return false
        }
    }

    /** 对一个 state 回答多个问题（保序）。 */
    suspend fun decide(state: String, questions: LinkedHashMap<String, Question>): Map<String, Answer> {
        check(loaded) { "请先 load()" }
        lastInputTruncated = false
        val padId = tokenizer!!.tokenToId(LayaConfig.TOK_PAD).coerceAtLeast(0)
        val inputInfos = engine.graphInfos.first().inputs
        val order = inputInfos.map { it.name }
        // 名 -> 每元素字节宽度：按图声明的 dtype 打包（int32/int64/uint8/bool 等一律适配），缺失回退 4。
        val widthOf = inputInfos.associate { it.name to it.elementBytes.toInt() }
        fun w(name: String) = widthOf[name] ?: 4
        val out = LinkedHashMap<String, Answer>()
        for ((qid, q) in questions) {
            val built = seqBuilder.build(state, q)
            lastInputTruncated = lastInputTruncated || built.truncated
            if (built.truncated) Log.w(TAG, "输入已截断: $qid head=${built.headTruncated} stateDropped=${built.stateTokensDropped}")
            val k = built.markers.size
            val byName = mapOf(
                "input_ids" to packIntsAs(built.ids, padId, LayaConfig.MAX_LEN, w("input_ids")),
                "attention_mask" to packIntsAs(IntArray(built.ids.size) { 1 }, 0, LayaConfig.MAX_LEN, w("attention_mask")),
                "marker_pos" to packIntsAs(built.markers, 0, LayaConfig.MAX_OPTS, w("marker_pos")),
                "marker_mask" to packIntsAs(IntArray(k) { 1 }, 0, LayaConfig.MAX_OPTS, w("marker_mask")),
                "qtype" to packIntsAs(intArrayOf(q.qtype()), 0, 1, w("qtype")),
            )
            val inputs = order.map { byName[it] ?: error("图输入名未知: $it") }
            val res = engine.executeWithOutput(graphName, inputs)
                ?: run { Log.e(TAG, "推理失败: $qid"); continue }
            val outInfos = engine.graphInfos.first().outputs
            val outNames = outInfos.map { it.name }
            // 按输出张量声明宽度解码：fp16(2B)/fp32(4B) 自适应，缺失回退 4。
            val logIdx = outNames.indexOf("logits").coerceAtLeast(0)
            val logits = toFloatsAuto(res[logIdx], outInfos[logIdx].elementBytes.toInt())
            val actIdx = outNames.indexOf("act_logits").let { if (it >= 0) it else 1 }
            val act = toFloatsAuto(res[actIdx], outInfos[actIdx].elementBytes.toInt())
            out[qid] = LayaPostprocessor.decode(q, logits, k, act)
        }
        return out
    }

    override fun close() {
        tokenizer?.close()
        tokenizer = null
        engine.close()
        loaded = false
        lastInputTruncated = false
    }

    companion object {
        private const val TAG = "LayaBackend"

        /** 把逻辑整数序列按目标张量的每元素字节宽度(1/2/4/8)小端打包，长度 count，不足用 pad 填充。 */
        fun packIntsAs(values: IntArray, pad: Int, count: Int, elementBytes: Int): ByteArray {
            val bb = java.nio.ByteBuffer.allocate(count * elementBytes).order(java.nio.ByteOrder.LITTLE_ENDIAN)
            for (i in 0 until count) {
                val v = if (i < values.size) values[i] else pad
                when (elementBytes) {
                    1 -> bb.put(v.toByte())
                    2 -> bb.putShort(v.toShort())
                    4 -> bb.putInt(v)
                    8 -> bb.putLong(v.toLong())
                    else -> throw IllegalArgumentException("不支持的 elementBytes=$elementBytes")
                }
            }
            return bb.array()
        }

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

        /** IEEE-754 binary16 -> Float。纯位运算，JVM/Android 通用。 */
        fun halfToFloat(h: Int): Float {
            val s = (h ushr 15) and 0x1
            val e = (h ushr 10) and 0x1f
            val m = h and 0x3ff
            val bits: Int = when (e) {
                0 -> if (m == 0) s shl 31 else {            // 0 或 subnormal
                    // 规格化：把隐含位左移进 bit10，指数相应减小
                    var mant = m
                    var exp = -1
                    do { mant = mant shl 1; exp++ } while (mant and 0x400 == 0)
                    mant = mant and 0x3ff
                    (s shl 31) or ((127 - 15 - exp) shl 23) or (mant shl 13)
                }
                0x1f -> (s shl 31) or (0xff shl 23) or (m shl 13)   // Inf/NaN
                else -> (s shl 31) or ((e + (127 - 15)) shl 23) or (m shl 13)   // normal
            }
            return Float.fromBits(bits)
        }

        /**
         * 按输出张量声明的每元素字节宽度解码为 FloatArray。
         * elementBytes==2 -> 小端 binary16 (fp16)；==4 -> 小端 fp32；其它抛异常。
         */
        fun toFloatsAuto(bytes: ByteArray, elementBytes: Int): FloatArray = when (elementBytes) {
            2 -> {
                val bb = ByteBuffer.wrap(bytes).order(ByteOrder.LITTLE_ENDIAN)
                FloatArray(bytes.size / 2) { halfToFloat(bb.short.toInt() and 0xFFFF) }
            }
            4 -> toFloats(bytes)
            else -> throw IllegalArgumentException("不支持的输出 elementBytes=$elementBytes")
        }
    }
}

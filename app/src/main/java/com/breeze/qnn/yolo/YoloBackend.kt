package com.breeze.qnn.yolo

import android.content.Context
import android.util.Log
import com.breeze.qnn.InferenceEngine
import com.breeze.qnn.QnnNative
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.withContext
import java.io.File

class YoloBackend(private val context: Context) : AutoCloseable {

    private val engine = InferenceEngine(context)
    private var graphName: String = ""
    private var inputNames: List<String> = emptyList()
    private var ready = false

    var modelKind: String = ""
        private set

    /** COCO-pose 80 类名（person 排第 0，与 tensor cls_id 对齐） */
    val classNames: List<String> = COCO_POSE_80

    fun modelReady(): Boolean =
        ctxFile().let { it.exists() && it.length() > 5_000_000L }

    private fun ctxFile() = File(context.filesDir, "yolo/yolo26n-pose.bin")

    /** 默认 HTP；CPU 走 .dlc（本 spec 暂不提供 .dlc 部署，CPU 回退到报错） */
    suspend fun loadModel(backend: QnnNative.Backend = QnnNative.Backend.HTP): Boolean {
        val f = ctxFile()
        if (!f.exists() || f.length() <= 5_000_000L) {
            Log.e(TAG, "模型不存在: ${f.absolutePath}")
            return false
        }
        Log.i(TAG, "加载 context binary: ${f.name}")
        modelKind = "fp16 ctx"
        val ok = engine.loadContextBinary(f.absolutePath, backend)
        if (!ok) return false
        val g = engine.graphInfos.firstOrNull() ?: run {
            Log.e(TAG, "模型里没有图"); return false
        }
        graphName = g.name
        inputNames = g.inputs.map { it.name }
        ready = true
        Log.i(TAG, "YOLO 已加载 graph=$graphName inputs=$inputNames ($modelKind)")
        warmup()
        return true
    }

    private suspend fun warmup() {
        val input = ByteArray(640 * 640 * 3 * 4) { 0f.toBits().toByte() }  // 全零 fp32 NCHW
        engine.executeWithOutput(graphName, listOf(input))
        Log.i(TAG, "warmup 完成")
    }

    /** 喂 fp32 NCHW 640×640 ByteArray（已 letterbox + 归一化），返回原始 output bytes */
    suspend fun infer(fp32Input: ByteArray): List<ByteArray>? =
        withContext(Dispatchers.IO) {
            if (!ready) { Log.e(TAG, "未加载"); return@withContext null }
            engine.executeWithOutput(graphName, listOf(fp32Input))
        }

    override fun close() {
        engine.close(); ready = false
    }

    companion object {
        private const val TAG = "YoloBackend"
        private const val CTX_NAME = "yolo26n-pose.bin"
        private const val IMG_SIZE = 640
        const val EXPECTED_INPUT_BYTES = 1 * 3 * 640 * 640 * 4  // fp32 NCHW

        /** COCO-pose 80 类（YOLO26-pose 用完整 COCO 类集 + 17 kpts） */
        val COCO_POSE_80: List<String> = listOf(
            "person","bicycle","car","motorcycle","airplane","bus","train","truck","boat",
            "traffic light","fire hydrant","stop sign","parking meter","bench","bird","cat",
            "dog","horse","sheep","cow","elephant","bear","zebra","giraffe","backpack",
            "umbrella","handbag","tie","suitcase","frisbee","skis","snowboard","sports ball",
            "kite","baseball bat","baseball glove","skateboard","surfboard","tennis racket",
            "bottle","wine glass","cup","fork","knife","spoon","bowl","banana","apple",
            "sandwich","orange","broccoli","carrot","hot dog","pizza","donut","cake","chair",
            "couch","potted plant","bed","dining table","toilet","tv","laptop","mouse",
            "remote","keyboard","cell phone","microwave","oven","toaster","sink",
            "refrigerator","book","clock","vase","scissors","teddy bear","hair drier",
            "toothbrush"
        )
    }
}

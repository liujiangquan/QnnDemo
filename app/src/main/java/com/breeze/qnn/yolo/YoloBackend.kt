package com.breeze.qnn.yolo

import android.content.Context
import android.graphics.Bitmap
import android.util.Log
import kotlin.coroutines.resume
import kotlin.coroutines.resumeWithException
import kotlin.coroutines.suspendCoroutine
import java.io.File

/**
 * YOLO26-pose HTP 推理封装。
 *
 * 不走 InferenceEngine 抽象（那头是按 fp32 NCHW 推 CNN DLC 设计的，跟 vendor bin
 * W8A16 uint16 张量对不上）。改走 [YoloNative] 直接桥接 [yolo_pose.cpp]：
 * native 读 bin 真实量化编码，自己做 letterbox+量化再 execute，返回 flat float[]
 * [count, count × (conf, x1, y1, x2, y2, 17×3 kpt)]。
 *
 * 参考 QnnYolo 项目的 QairtYoloPose 实现（SM8845 真机验证通过）。
 */
class YoloBackend(private val context: Context) : AutoCloseable {

    private val native = YoloNative()
    private var ready = false

    var modelKind: String = ""
        private set

    /** COCO-pose 80 类名（person 排第 0，与 tensor cls_id 对齐） */
    val classNames: List<String> = COCO_POSE_80

    fun modelReady(): Boolean =
        ctxFile().let { it.exists() && it.length() > MIN_BIN_BYTES }

    private fun ctxFile() = File(context.filesDir, "yolo/yolo26n-pose.bin")

    /** 加载 vendor bin 并初始化 HTP session。默认 HTP。 */
    suspend fun loadModel(): Boolean = suspendCoroutine { cont ->
        val f = ctxFile()
        if (!f.exists() || f.length() <= MIN_BIN_BYTES) {
            Log.e(TAG, "模型不存在: ${f.absolutePath}")
            cont.resume(false)
            return@suspendCoroutine
        }
        val libDir = context.applicationInfo.nativeLibraryDir
        try {
            val binBytes = f.readBytes()
            val err = native.nativeInit(libDir, binBytes)
            // nativeInit 在成功时 JNI 返回 nullptr（Kotlin 看到 null），失败时返回错误字符串。
            // 用 isNullOrEmpty 同时兼容两种 JNI 风格。
            if (err.isNullOrEmpty()) {
                ready = true
                modelKind = "fp16 ctx (vendor bin, build v2.46)"
                Log.i(TAG, "YOLO 加载成功 modelKind=$modelKind")
                cont.resume(true)
            } else {
                Log.e(TAG, "nativeInit 失败: $err")
                cont.resume(false)
            }
        } catch (t: Throwable) {
            Log.e(TAG, "loadModel 异常", t)
            cont.resumeWithException(t)
        }
    }

    /**
     * 喂一张 Bitmap（ARGB8888），返回 flat float[]：
     * [count, count × (conf, x1, y1, x2, y2, 17 × (x, y, vis))]
     * 全部坐标都在 640×640 tensor 空间（未反 letterbox）。
     */
    suspend fun infer(bitmap: Bitmap): FloatArray? = suspendCoroutine { cont ->
        if (!ready) {
            Log.e(TAG, "未加载")
            cont.resume(null)
            return@suspendCoroutine
        }
        try {
            val pixels = bitmapToRgbaBytes(bitmap)
            val result = native.nativeInferRgb(bitmap.width, bitmap.height, pixels)
            cont.resume(result)
        } catch (t: Throwable) {
            Log.e(TAG, "infer 异常", t)
            cont.resumeWithException(t)
        }
    }

    /** 从 Bitmap 抠出 ARGB8888 字节（每像素 4 字节，顺序 R,G,B,A） */
    private fun bitmapToRgbaBytes(bitmap: Bitmap): ByteArray {
        val w = bitmap.width
        val h = bitmap.height
        val out = ByteArray(w * h * 4)
        val p = IntArray(w * h)
        bitmap.getPixels(p, 0, w, 0, 0, w, h)
        var i = 0
        for (px in p) {
            out[i + 0] = ((px ushr 16) and 0xFF).toByte()  // R
            out[i + 1] = ((px ushr 8) and 0xFF).toByte()   // G
            out[i + 2] = (px and 0xFF).toByte()             // B
            out[i + 3] = ((px ushr 24) and 0xFF).toByte()   // A
            i += 4
        }
        return out
    }

    override fun close() {
        native.nativeRelease()
        ready = false
    }

    companion object {
        private const val TAG = "YoloBackend"
        private const val CTX_NAME = "yolo26n-pose.bin"
        private const val IMG_SIZE = 640
        private const val MIN_BIN_BYTES = 3_500_000L  // vendor yolo26n-pose.bin 实测 4.1MB

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

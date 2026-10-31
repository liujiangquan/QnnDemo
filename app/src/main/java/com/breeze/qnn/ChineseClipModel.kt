package com.breeze.qnn

import android.graphics.Bitmap
import android.util.Log
import org.json.JSONObject
import java.io.File

/**
 * Chinese-CLIP 文搜图模型（MNN，CPU）。
 *
 * - [init] 读取模型目录下的 bundle_config.json，解析图像/文本两个 .mnn 文件名。
 * - [embedImage] / [embedText] 返回 1024 维 L2 归一化特征。
 * - [rank] 对候选图片做打分：输出为 softmax(100 × cosine) 的概率，**不是余弦相似度**，
 *   各候选之间的相对大小有意义，绝对值不可直接当阈值（AIEngine 的 0.492 阈值即按此口径标定）。
 */
class ChineseClipModel : AutoCloseable {

    private var handle: Long = 0L

    val isReady: Boolean get() = handle != 0L

    /** modelDir 下需包含 bundle_config.json 及两个 .mnn 文件。成功返回 true。 */
    fun init(modelDir: File): Boolean {
        if (handle != 0L) return true
        if (!ChineseClipJni.ensureLoaded()) {
            Log.w(TAG, "native 未加载")
            return false
        }
        val pair = resolveModelPaths(modelDir) ?: run {
            Log.w(TAG, "模型文件缺失: $modelDir")
            return false
        }
        handle = ChineseClipJni.nativeInit(pair.first, pair.second)
        if (handle == 0L) Log.e(TAG, "nativeInit 失败: ${pair.first} + ${pair.second}")
        else Log.i(TAG, "Chinese-CLIP 就绪 (dim=${ChineseClipJni.EMBEDDING_DIM})")
        return handle != 0L
    }

    /** 图像特征；失败返回 null。 */
    fun embedImage(bitmap: Bitmap): FloatArray? {
        val h = handle
        if (h == 0L) return null
        val out = FloatArray(ChineseClipJni.EMBEDDING_DIM)
        val code = ChineseClipJni.nativeExtractImageFeature(h, bitmap, out)
        return if (code == 0) out else null.also { Log.w(TAG, "embedImage ret=$code") }
    }

    /** 文本特征；失败返回 null。 */
    fun embedText(text: String): FloatArray? {
        val h = handle
        if (h == 0L || text.isBlank()) return null
        val out = FloatArray(ChineseClipJni.EMBEDDING_DIM)
        val code = ChineseClipJni.nativeExtractTextFeature(h, text, out)
        return if (code == 0) out else null.also { Log.w(TAG, "embedText ret=$code") }
    }

    /**
     * 对候选图片打分。[imageFeatures] 为每张图的特征（各长 1024）。
     * 返回与候选同序的概率数组（softmax 后，和为 1）；失败返回 null。
     */
    fun rank(textFeature: FloatArray, imageFeatures: List<FloatArray>): FloatArray? {
        val h = handle
        if (h == 0L || imageFeatures.isEmpty()) return null
        val dim = ChineseClipJni.EMBEDDING_DIM
        val flat = FloatArray(imageFeatures.size * dim)
        imageFeatures.forEachIndexed { i, f ->
            if (f.size != dim) return null
            System.arraycopy(f, 0, flat, i * dim, dim)
        }
        val scores = FloatArray(imageFeatures.size)
        val code = ChineseClipJni.nativeSelectBestImage(h, textFeature, flat, imageFeatures.size, scores)
        return if (code == 0) scores else null.also { Log.w(TAG, "rank ret=$code") }
    }

    override fun close() {
        val h = handle
        handle = 0L
        if (h != 0L) ChineseClipJni.nativeRelease(h)
    }

    companion object {
        private const val TAG = "ChineseClipModel"
        private const val DEFAULT_IMG = "rn50_img_fp16.mnn"
        private const val DEFAULT_TXT = "rn50_txt_fp16.mnn"

        /** 与 AIEngine resolveClipModelPaths 一致：优先读 bundle_config.json 的文件名，缺省用默认名。 */
        fun resolveModelPaths(dir: File): Pair<String, String>? {
            if (!dir.isDirectory) return null
            var imgName = DEFAULT_IMG
            var txtName = DEFAULT_TXT
            val bundle = File(dir, "bundle_config.json")
            if (bundle.isFile) {
                runCatching {
                    val props = JSONObject(bundle.readText()).optJSONObject("properties")
                    props?.optString("chineseclip_img")?.takeIf { it.isNotEmpty() }?.let { imgName = it }
                    props?.optString("chineseclip_txt")?.takeIf { it.isNotEmpty() }?.let { txtName = it }
                }
            }
            val img = File(dir, imgName)
            val txt = File(dir, txtName)
            return if (img.isFile && txt.isFile) img.absolutePath to txt.absolutePath else null
        }
    }
}

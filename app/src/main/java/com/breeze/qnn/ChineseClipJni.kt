package com.breeze.qnn

import android.graphics.Bitmap
import android.util.Log

/**
 * Chinese-CLIP 原生 JNI 声明，对应 cpp/chineseclip/clip_jni.cpp。
 *
 * 加载顺序与 AIEngine 一致：先 loadLibrary("mnn_aiengine")，保证 qnndemo 的
 * DT_NEEDED 解析到本包的 MNN（SONAME 已改名，避免被系统 libMNN.so 截胡）。
 * CLIP 的 JNI 与 qnndemo 同库，不另建 .so。
 */
object ChineseClipJni {

    private const val TAG = "ChineseClipJni"

    @Volatile private var loadAttempted = false
    @Volatile private var loadOk = false

    /** 首次调用时加载原生库；失败返回 false，不抛异常。 */
    fun ensureLoaded(): Boolean {
        if (loadOk) return true
        synchronized(this) {
            if (loadOk) return true
            if (loadAttempted) return false
            loadAttempted = true
            return try {
                System.loadLibrary("mnn_aiengine")
                System.loadLibrary("qnndemo")
                loadOk = true
                true
            } catch (e: UnsatisfiedLinkError) {
                Log.w(TAG, e.message ?: "loadLibrary failed")
                false
            }
        }
    }

    /** 加载图像塔与文本塔（.mnn），返回句柄；失败返回 0。 */
    external fun nativeInit(imageModelPath: String, textModelPath: String): Long

    /** 提取图像特征（1024 维，写入 result），返回 0 表示成功。 */
    external fun nativeExtractImageFeature(handle: Long, bitmap: Bitmap, result: FloatArray): Int

    /** 提取文本特征（1024 维，写入 result），返回 0 表示成功。 */
    external fun nativeExtractTextFeature(handle: Long, text: String, result: FloatArray): Int

    /** 计算文本特征与多张图片特征的余弦相似度，写入 result（长度 imageCount）。 */
    external fun nativeSelectBestImage(
        handle: Long,
        textFeature: FloatArray,
        imagesFeature: FloatArray,
        imageCount: Int,
        result: FloatArray
    ): Int

    /** 释放句柄。 */
    external fun nativeRelease(handle: Long)

    /** CLIP 向量维度，与 clip_jni.cpp 的校验保持一致。 */
    const val EMBEDDING_DIM = 1024
}

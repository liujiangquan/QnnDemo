package com.breeze.qnn.yolo

import android.graphics.Bitmap
import android.media.Image

/** 工具：从 CameraX ImageProxy (YUV_420_888) 转 Bitmap。仅被 YoloFragment 用。
 *  letterbox + 量化喂给 vendor bin 的事交给 native [YoloPoseSession.inferRgb]。 */
object YoloPreprocessor {

    fun imageToBitmap(image: Image): Bitmap {
        val w = image.width; val h = image.height
        val yuv = image.planes[0].buffer
        val yBytes = ByteArray(yuv.remaining()).also { yuv.get(it) }
        val uPlane = image.planes[1]
        val vPlane = image.planes[2]
        val uRowStride = uPlane.rowStride
        val vRowStride = vPlane.rowStride
        val uPixStride = uPlane.pixelStride
        val vPixStride = vPlane.pixelStride
        val rgb = IntArray(w * h)
        val uv = IntArray((w / 2) * (h / 2))
        for (j in 0 until h step 2) {
            for (i in 0 until w step 2) {
                val uOff = (j / 2) * uRowStride + (i / 2) * uPixStride
                val vOff = (j / 2) * vRowStride + (i / 2) * vPixStride
                val u = uPlane.buffer[uOff].toInt() and 0xFF
                val v = vPlane.buffer[vOff].toInt() and 0xFF
                uv[(j / 2) * (w / 2) + (i / 2)] = (u shl 8) or v
            }
        }
        for (j in 0 until h) {
            for (i in 0 until w) {
                val y = yBytes[j * w + i].toInt() and 0xFF
                val uvIdx = (j / 2) * (w / 2) + (i / 2)
                val u = (uv[uvIdx] ushr 8) and 0xFF
                val v = uv[uvIdx] and 0xFF
                val r = (y + 1.402f * (v - 128)).toInt().coerceIn(0, 255)
                val g = (y - 0.344f * (u - 128) - 0.714f * (v - 128)).toInt().coerceIn(0, 255)
                val b = (y + 1.772f * (u - 128)).toInt().coerceIn(0, 255)
                rgb[j * w + i] = (0xFF shl 24) or (r shl 16) or (g shl 8) or b
            }
        }
        return Bitmap.createBitmap(rgb, w, h, Bitmap.Config.ARGB_8888)
    }
}

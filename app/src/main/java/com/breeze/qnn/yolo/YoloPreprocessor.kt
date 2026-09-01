package com.breeze.qnn.yolo

import android.graphics.Bitmap
import android.media.Image
import kotlin.math.min

/** Letterbox resize + CHW 重排 + /255 归一化，输出 fp32 NCHW 640×640 ByteArray */
object YoloPreprocessor {

    private const val SIZE = 640

    /** 从 ImageProxy (YUV_420_888) 转 fp32 NCHW 字节（letterbox pad=114 gray） */
    fun imageToTensor(image: Image): LetterboxResult {
        val rgb = yuv420ToRgb(image)
        return bitmapToTensor(Bitmap.createBitmap(rgb, image.width, image.height, Bitmap.Config.ARGB_8888))
    }

    /** 从 Bitmap 转 fp32 NCHW 字节。返回 tensor 同时返回 letterbox 参数供 overlay 反演。 */
    fun bitmapToTensor(src: Bitmap): LetterboxResult {
        val (resized, padX, padY, scale) = letterbox(src, SIZE, SIZE, 114f)
        val pixels = IntArray(SIZE * SIZE)
        resized.getPixels(pixels, 0, SIZE, 0, 0, SIZE, SIZE)
        val tensor = pixelsToTensor(pixels, SIZE, SIZE, padX, padY, scale)
        return LetterboxResult(tensor, padX, padY, scale)
    }

    /**
     * 纯函数：把任意尺寸的 RGB IntArray (premultiplied ARGB) 转成 fp32 NCHW 640×640 ByteArray。
     * 不依赖 android.graphics，方便 JVM 单测覆盖。
     *
     * @param rgb    宽度 w × 高度 h 的 ARGB8888 像素数组
     * @param w      像素数组宽度
     * @param h      像素数组高度
     * @param padX   letterbox 在 640×640 tensor 中的 X 偏移
     * @param padY   letterbox 在 640×640 tensor 中的 Y 偏移
     * @param scale  短边缩放比 (src→tensor)
     * @param padVal padding 区域 RGB (默认 114 半均值)
     */
    fun pixelsToTensor(
        rgb: IntArray, w: Int, h: Int,
        padX: Int, padY: Int, scale: Float,
        padVal: Int = 114,
    ): ByteArray {
        val padColor = (0xFF.toInt() shl 24) or
            ((padVal and 0xFF) shl 16) or ((padVal and 0xFF) shl 8) or (padVal and 0xFF)
        val out = ByteArray(3 * SIZE * SIZE * 4)
        for (c in 0 until 3) {
            for (th in 0 until SIZE) {
                for (tw in 0 until SIZE) {
                    // letterbox 反演回源 bitmap 坐标；越界用 padColor
                    val sx = (tw - padX) / scale
                    val sy = (th - padY) / scale
                    val px = if (sx >= 0f && sx < w.toFloat() && sy >= 0f && sy < h.toFloat())
                        rgb[sy.toInt() * w + sx.toInt()] else padColor
                    val r = (px ushr 16) and 0xFF
                    val g = (px ushr 8) and 0xFF
                    val b = px and 0xFF
                    val v = when (c) {
                        0 -> r / 255f
                        1 -> g / 255f
                        else -> b / 255f
                    }
                    val idx = (c * SIZE * SIZE + th * SIZE + tw) * 4
                    val bits = java.lang.Float.floatToRawIntBits(v)
                    out[idx] = (bits ushr 0).toByte()
                    out[idx + 1] = (bits ushr 8).toByte()
                    out[idx + 2] = (bits ushr 16).toByte()
                    out[idx + 3] = (bits ushr 24).toByte()
                }
            }
        }
        return out
    }

    /** 反演 letterbox：把 tensor 坐标映射回原始 bitmap 坐标 */
    fun unletterbox(
        tensorX: Float, tensorY: Float,
        padX: Int, padY: Int, scale: Float,
    ): Pair<Float, Float> {
        val origX = (tensorX - padX) / scale
        val origY = (tensorY - padY) / scale
        return origX to origY
    }

    /** 一次预处理的全部结果（tensor + letterbox 参数） */
    data class LetterboxResult(
        val tensor: ByteArray,
        val padX: Int,
        val padY: Int,
        val scale: Float,
    ) {
        override fun equals(other: Any?): Boolean {
            if (this === other) return true
            if (other !is LetterboxResult) return false
            return tensor.contentEquals(other.tensor) && padX == other.padX &&
                padY == other.padY && scale == other.scale
        }

        override fun hashCode(): Int =
            tensor.contentHashCode() + 31 * (padX * 31 + padY * 7 + scale.hashCode())
    }

    private fun letterbox(src: Bitmap, tw: Int, th: Int, padVal: Float): LetterboxParams {
        val r = min(tw.toFloat() / src.width, th.toFloat() / src.height)
        val newW = (src.width * r).toInt()
        val newH = (src.height * r).toInt()
        val padX = (tw - newW) / 2
        val padY = (th - newH) / 2
        val resized = Bitmap.createBitmap(tw, th, Bitmap.Config.ARGB_8888)
        val gray = (0xFF.toInt() shl 24) or
            ((padVal.toInt() and 0xFF) shl 16) or
            ((padVal.toInt() and 0xFF) shl 8) or (padVal.toInt() and 0xFF)
        resized.eraseColor(gray)
        android.graphics.Canvas(resized).drawBitmap(src, null,
            android.graphics.Rect(padX, padY, padX + newW, padY + newH), null)
        return LetterboxParams(resized, padX, padY, r)
    }

    private data class LetterboxParams(
        val bitmap: Bitmap,
        val padX: Int,
        val padY: Int,
        val scale: Float,
    )

    private fun yuv420ToRgb(image: Image): IntArray {
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
        var yi = 0; var ui = 0
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
                val u = (uv[(j / 2) * (w / 2) + (i / 2)] ushr 8) and 0xFF
                val v = uv[(j / 2) * (w / 2) + (i / 2)] and 0xFF
                val r = (y + 1.402f * (v - 128)).toInt().coerceIn(0, 255)
                val g = (y - 0.344f * (u - 128) - 0.714f * (v - 128)).toInt().coerceIn(0, 255)
                val b = (y + 1.772f * (u - 128)).toInt().coerceIn(0, 255)
                rgb[j * w + i] = (0xFF shl 24) or (r shl 16) or (g shl 8) or b
            }
        }
        return rgb
    }
}

package com.breeze.qnn.yolo

import org.junit.Assert.assertEquals
import org.junit.Test
import java.nio.ByteBuffer
import java.nio.ByteOrder

class YoloPreprocessorTest {

    private fun floatAt(t: ByteArray, c: Int, th: Int, tw: Int): Float {
        val idx = (c * 640 * 640 + th * 640 + tw) * 4
        return ByteBuffer.wrap(t, idx, 4).order(ByteOrder.nativeOrder()).float
    }

    @Test fun `pixelsToTensor size is 1x3x640x640 fp32`() {
        val rgb = IntArray(640 * 640)  // 640×640 square, all black
        val t = YoloPreprocessor.pixelsToTensor(rgb, 640, 640, 0, 0, 1f)
        assertEquals(1 * 3 * 640 * 640 * 4, t.size)
    }

    @Test fun `pixelsToTensor pure white R_0 G_0 B_0 at tensor origin`() {
        // 1×1 全白图，letterbox scale=1, padX=0, padY=0：tensor (0,0) R=G=B=1.0f
        val rgb = IntArray(1 * 1) { 0xFFFFFFFF.toInt() }
        val t = YoloPreprocessor.pixelsToTensor(rgb, 1, 1, 0, 0, 1f)
        assertEquals(1.0f, floatAt(t, 0, 0, 0), 1e-5f)
        assertEquals(1.0f, floatAt(t, 1, 0, 0), 1e-5f)
        assertEquals(1.0f, floatAt(t, 2, 0, 0), 1e-5f)
    }

    @Test fun `pixelsToTensor padding fills gray 114_255 when coordinates out of bounds`() {
        // 1×1 源图，scale=1, padX=0, padY=0：所有非 (0,0) 像素都是 padding (114/255)
        val rgb = IntArray(1 * 1) { 0xFFFFFFFF.toInt() }
        val t = YoloPreprocessor.pixelsToTensor(rgb, 1, 1, 0, 0, 1f, padVal = 114)
        // 中心像素：源 (0,0) 全白 → 1.0
        assertEquals(1.0f, floatAt(t, 0, 0, 0), 1e-5f)
        // 边角 (639, 639) → 反演源 (−639, −639) 越界 → padding
        assertEquals(114f / 255f, floatAt(t, 0, 639, 639), 1e-5f)
        assertEquals(114f / 255f, floatAt(t, 1, 639, 639), 1e-5f)
        assertEquals(114f / 255f, floatAt(t, 2, 639, 639), 1e-5f)
    }

    @Test fun `pixelsToTensor upscale tall input 320x640 to 640x640 scale_1 pad_160_0`() {
        // 源 320×640 (portrait)：letterbox 到 640×640
        //   scale = min(640/320, 640/640) = min(2.0, 1.0) = 1.0
        //   newW = 320, newH = 640, padX = 160, padY = 0
        val w = 320; val h = 640
        val rgb = IntArray(w * h) { i ->
            (0xFF.toInt() shl 24) or ((i and 0xFF) shl 16) or 0 or 0
        }
        val t = YoloPreprocessor.pixelsToTensor(rgb, w, h, padX = 160, padY = 0, scale = 1f)
        // center column: tensor (320, 0) → source (160, 0) → RGB[160].R = 160 & 0xFF = 160 → 160/255
        assertEquals(160f / 255f, floatAt(t, 0, 0, 320), 1e-5f)
        // left-of-center: tensor (319, 0) → source (159, 0) → 159/255
        assertEquals(159f / 255f, floatAt(t, 0, 0, 319), 1e-5f)
        // left padding area: tensor (0, 0) → source (-160, 0) → 越界 → padding
        assertEquals(114f / 255f, floatAt(t, 0, 0, 0), 1e-5f)
        // middle of letterbox padding: tensor (160, 0) → source (0, 0) → 0/255
        assertEquals(0f, floatAt(t, 0, 0, 160), 1e-5f)
    }
}

package com.breeze.qnn.yolo

import org.junit.Assert.assertEquals
import org.junit.Assert.assertTrue
import org.junit.Test

/**
 * YoloPostprocessor.parseFlat JVM coverage。
 *
 * flat float[] 格式 = [count, count × (conf, x1, y1, x2, y2, 17×3 kpt)]，
 * 跟 native decodePose 的产出对齐。android.jar JVM stub 让 PointF/RectF
 * 字段为 no-op，所以这里只验证筛选 / 计数 / 17 kpt / visible 过滤这些纯逻辑；
 * 实际坐标在 T13 InstrumentedTest (YoloE2ETest) 实机覆盖。
 */
class YoloPostprocessorTest {

    private companion object {
        const val STRIDE = 1 + 4 + 17 * 3  // 56
    }

    private fun flatOf(count: Int, fill: (Int) -> Float): FloatArray {
        val out = FloatArray(1 + count * STRIDE)
        out[0] = count.toFloat()
        for (i in 0 until count) {
            val base = 1 + i * STRIDE
            out[base + 0] = fill(i)             // conf
            out[base + 1] = 320f                // x1
            out[base + 2] = 240f                // y1
            out[base + 3] = 420f                // x2
            out[base + 4] = 360f                // y2
            for (k in 0 until 17) {
                out[base + 5 + k * 3 + 0] = (k * 10).toFloat()  // kx
                out[base + 5 + k * 3 + 1] = (k * 10 + 1).toFloat()  // ky
                out[base + 5 + k * 3 + 2] = if (k % 2 == 0) 1f else 0f  // vis
            }
        }
        return out
    }

    @Test fun `parseFlat empty array yields empty list`() {
        assertEquals(0, YoloPostprocessor.parseFlat(floatArrayOf()).size)
    }

    @Test fun `parseFlat filters low-confidence detections`() {
        // 2 个候选：conf 0.9 vs 0.05；threshold=0.25 应只保留 1 个
        val flat = flatOf(2) { i -> if (i == 0) 0.9f else 0.05f }
        val r = YoloPostprocessor.parseFlat(flat, listOf("person"), confThr = 0.25f, iouThr = 0.45f)
        assertEquals(1, r.size)
        assertEquals(0, r[0].cls)
        assertEquals(17, r[0].keypoints.size)
    }

    @Test fun `parseFlat maps each detection to 17 keypoints`() {
        val flat = flatOf(1) { 0.95f }
        val r = YoloPostprocessor.parseFlat(flat, listOf("person"), confThr = 0.25f, iouThr = 0.45f)
        assertEquals(1, r.size)
        assertEquals(17, r[0].keypoints.size)
    }

    @Test fun `parseFlat visibleKeypoints excludes low-visibility entries`() {
        // vis 模式：[1,0,1,0,...]，8 个可见 / 9 个不可见
        val flat = flatOf(1) { 0.95f }
        val r = YoloPostprocessor.parseFlat(flat, listOf("person"), confThr = 0.25f, iouThr = 0.45f)
        assertEquals(1, r.size)
        assertEquals(17, r[0].keypoints.size)
        assertEquals(8, r[0].visibleKeypoints.size)
    }

    @Test fun `parseFlat dedups overlapping boxes via NMS`() {
        // 两个候选框几乎重合 (IoU > 0.45)，NMS 应只保留 1 个
        val flat = FloatArray(1 + 2 * STRIDE)
        flat[0] = 2f
        // 候选 0：conf 0.9, box (320,240,420,360)
        flat[1 + 0 * STRIDE + 0] = 0.9f
        flat[1 + 0 * STRIDE + 1] = 320f; flat[1 + 0 * STRIDE + 2] = 240f
        flat[1 + 0 * STRIDE + 3] = 420f; flat[1 + 0 * STRIDE + 4] = 360f
        // 候选 1：conf 0.8, box 偏移 1 像素 (几乎重合)
        flat[1 + 1 * STRIDE + 0] = 0.8f
        flat[1 + 1 * STRIDE + 1] = 321f; flat[1 + 1 * STRIDE + 2] = 241f
        flat[1 + 1 * STRIDE + 3] = 421f; flat[1 + 1 * STRIDE + 4] = 361f
        val r = YoloPostprocessor.parseFlat(flat, listOf("person"), confThr = 0.25f, iouThr = 0.45f)
        assertEquals(1, r.size)
        assertEquals(0, r[0].cls)
    }

    @Test fun `parseFlat keeps distinct boxes`() {
        // 两个候选框相距远 (IoU ~0)，都应保留
        val flat = FloatArray(1 + 2 * STRIDE)
        flat[0] = 2f
        flat[1 + 0 * STRIDE + 0] = 0.9f
        flat[1 + 0 * STRIDE + 1] = 10f; flat[1 + 0 * STRIDE + 2] = 10f
        flat[1 + 0 * STRIDE + 3] = 50f; flat[1 + 0 * STRIDE + 4] = 50f
        flat[1 + 1 * STRIDE + 0] = 0.85f
        flat[1 + 1 * STRIDE + 1] = 500f; flat[1 + 1 * STRIDE + 2] = 500f
        flat[1 + 1 * STRIDE + 3] = 600f; flat[1 + 1 * STRIDE + 4] = 600f
        val r = YoloPostprocessor.parseFlat(flat, listOf("person"), confThr = 0.25f, iouThr = 0.45f)
        assertEquals(2, r.size)
    }
}

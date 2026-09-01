package com.breeze.qnn.yolo

import org.junit.Assert.assertEquals
import org.junit.Test
import java.nio.ByteBuffer
import java.nio.ByteOrder

class YoloPostprocessorTest {

    /** 把 N 个候选 (每个 [YoloPostprocessor.CHANNELS] floats) 拼成 end2end 单张量 flat fp32 ByteArray */
    private fun detsBytes(vararg dets: FloatArray): ByteArray {
        val n = YoloPostprocessor.CHANNELS
        val all = FloatArray(dets.size * n) { 0f }
        dets.forEachIndexed { i, d ->
            require(d.size == n) { "det $i must have $n floats, got ${d.size}" }
            System.arraycopy(d, 0, all, i * n, n)
        }
        val bb = ByteBuffer.allocate(all.size * 4).order(ByteOrder.nativeOrder())
        bb.asFloatBuffer().put(all)
        val out = ByteArray(bb.remaining())
        bb.get(out)
        return out
    }

    @Test fun `parse empty outputs yields empty list`() {
        assertEquals(0, YoloPostprocessor.parse(emptyList()).size)
    }

    @Test fun `parse filters low-confidence detections`() {
        // 高置信 person + 低置信 car，拼成单个 flat ByteArray（end2end 单张量输出）
        val n = YoloPostprocessor.CHANNELS
        val hi = FloatArray(n) { 0f }.also {
            it[0] = 320f; it[1] = 240f; it[2] = 100f; it[3] = 200f; it[4] = 0.9f
        }
        val lo = FloatArray(n) { 0f }.also {
            it[0] = 100f; it[1] = 100f; it[2] = 50f; it[3] = 50f; it[4] = 0.05f
        }
        val r = YoloPostprocessor.parse(listOf(detsBytes(hi, lo)),
            listOf("person", "car"), confThr = 0.25f)
        assertEquals(1, r.size)
        assertEquals(0, r[0].cls)
        assertEquals(17, r[0].keypoints.size)
    }

    @Test fun `parse maps one detection to a Detection with 17 keypoints`() {
        // android.jar JVM stub 让 PointF/RectF 构造函数变成 no-op：
        //   box 字段 stay 0、PointF stay NaN-stub-to-0，所以只能验证 shape（T12 E2E 实机覆盖坐标）。
        val kx = FloatArray(17) { it * 10f }
        val ky = FloatArray(17) { it * 10f + 1f }
        val kv = FloatArray(17) { 1f }
        val n = YoloPostprocessor.CHANNELS
        val raw = FloatArray(n) { 0f }.also {
            it[0] = 320f; it[1] = 240f; it[2] = 100f; it[3] = 200f; it[4] = 0.95f
            for (k in 0 until 17) { it[5 + k * 3 + 0] = kx[k]; it[5 + k * 3 + 1] = ky[k]; it[5 + k * 3 + 2] = kv[k] }
        }
        val r = YoloPostprocessor.parse(listOf(detsBytes(raw)), listOf("person"), confThr = 0.25f)
        assertEquals(1, r.size)
        assertEquals(17, r[0].keypoints.size)
        assertEquals(17, r[0].visibleKeypoints.size)
    }

    @Test fun `parse produces 17 keypoints per detection even with mixed visibility`() {
        // 第 0 个 kpt vis=0，其它 vis=1。android.jar stub 让 PointF(NaN,NaN)→PointF(0,0)，
        // 所以 visibleKeypoints 无法在 JVM 单测里反映 vis 过滤（纯函数版本在 YoloBackendTest 覆盖）。
        // 这里只验证 parser 解析出 1 个 det 且挂 17 个 kpt 槽位。
        val n = YoloPostprocessor.CHANNELS
        val kv = FloatArray(17) { 1f }.also { it[0] = 0f }
        val kx = FloatArray(17) { it * 10f }
        val ky = FloatArray(17) { it * 10f + 1f }
        val raw = FloatArray(n) { 0f }.also {
            it[0] = 320f; it[1] = 240f; it[2] = 100f; it[3] = 200f; it[4] = 0.95f
            for (k in 0 until 17) {
                it[5 + k * 3 + 0] = kx[k]; it[5 + k * 3 + 1] = ky[k]; it[5 + k * 3 + 2] = kv[k]
            }
        }
        val r = YoloPostprocessor.parse(listOf(detsBytes(raw)), listOf("person"))
        assertEquals(1, r.size)
        assertEquals(17, r[0].keypoints.size)
    }

    @Test fun `parse clamps to TOP_K candidates even if output buffer is larger`() {
        val n = YoloPostprocessor.CHANNELS
        val one = FloatArray(n) { 0f }.also {
            it[0] = 0f; it[1] = 0f; it[2] = 1f; it[3] = 1f; it[4] = 0.5f
        }
        val big = detsBytes(*Array(600) { one })
        val r = YoloPostprocessor.parse(listOf(big), listOf("person"), confThr = 0.4f)
        assertEquals(YoloPostprocessor.TOP_K, r.size)
    }
}

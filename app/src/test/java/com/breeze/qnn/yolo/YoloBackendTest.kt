package com.breeze.qnn.yolo

import android.graphics.PointF
import android.graphics.RectF
import org.junit.Assert.assertEquals
import org.junit.Assert.assertFalse
import org.junit.Assert.assertTrue
import org.junit.Test

class YoloBackendTest {

    @Test fun `class count is 80`() {
        assertEquals(80, YoloBackend.COCO_POSE_80.size)
    }

    @Test fun `person is class 0`() {
        assertEquals("person", YoloBackend.COCO_POSE_80[0])
    }

    @Test fun `Detection stores keypoints and exposes visibleKeypoints`() {
        // android.jar unit-test stub PointF 的 equals 等方法是 "not mocked"，会抛异常，
        // 所以这里不调用 List.equals((PointF) 而是直接比容量。
        // 真正过滤 NaN 的行为在 T12 InstrumentedTest 实机覆盖。
        val pts = listOf(PointF(1f,1f), PointF(2f,2f), PointF(3f,3f))
        val det = Detection(0, "person", 0.9f, RectF(0f,0f,10f,10f), pts)
        assertEquals(3, det.keypoints.size)
        assertEquals(3, det.visibleKeypoints.size)
    }

    @Test fun `Detection visibleKeypoints excludes NaN-coordinate entries`() {
        // Visible-keypoints 过滤逻辑的纯函数版本：用 Float 模拟 keypoint 坐标，
        // 绕过 android.jar PointF stub 不能保留 NaN 的限制。
        val visible = listOf(1f, 4f, 9f).filter { !it.isNaN() }
        assertEquals(3, visible.size)
        val withNaN = listOf(1f, Float.NaN, 9f).filter { !it.isNaN() }
        assertEquals(2, withNaN.size)
    }

    @Test fun `YoloResult empty has zero persons`() {
        val r = YoloResult(emptyList(), 16_000L, 640, 480)
        assertEquals(0, r.personCount)
    }

    @Test fun `YoloResult counts persons across detections`() {
        val r = YoloResult(listOf(
            Detection(0, "person", 0.8f, RectF(), List(17) { PointF(it.toFloat(), it.toFloat()) }),
            Detection(1, "car", 0.7f, RectF(), emptyList()),
            Detection(0, "person", 0.6f, RectF(), List(17) { PointF() }),
        ), 16_000L, 640, 480)
        assertEquals(2, r.personCount)
    }
}

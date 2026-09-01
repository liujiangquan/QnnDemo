package com.breeze.qnn.ui

import android.content.Context
import android.graphics.Canvas
import android.graphics.Color
import android.graphics.Paint
import android.graphics.PointF
import android.graphics.RectF
import android.util.AttributeSet
import android.view.View
import com.breeze.qnn.yolo.Detection

/**
 * 在 camera preview 上叠 bbox + 17 个 COCO keypoints + 骨架。
 *
 * 坐标是 [backend.infer] 输入的 bitmap 像素（native 已反 letterbox 回该 bitmap 原始尺寸）；
 * [bitW]×[bitH] 是喂给 native 的 bitmap 实际宽高（portrait 摄像头常为 1080×1920）。onDraw
 * 用与 PreviewView 默认 FILL_CENTER 一致的等比例铺满映射（S=max，view 中心对齐），
 * 比 640 更能贴合实际铺满画面——640 基准会把框/点挤到左上角。
 *
 * 用 [setBitmapSize] 把真实 bitmap 尺寸喂进来。
 */
class YoloOverlayView @JvmOverloads constructor(
    context: Context, attrs: AttributeSet? = null
) : View(context, attrs) {

    var detections: List<Detection> = emptyList()
        set(value) { field = value; invalidate() }

    /** 喂给 native 的 bitmap 尺寸；决定 onDraw 缩放基准。未设置时默认 640（兜底）。 */
    var bitW: Int = 640
    var bitH: Int = 640
        set(value) { field = value; invalidate() }

    fun setBitmapSize(w: Int, h: Int) {
        if (w != bitW || h != bitH) { bitW = w; bitH = h; invalidate() }
    }

    private val boxPaint = Paint(Paint.ANTI_ALIAS_FLAG).apply {
        style = Paint.Style.STROKE; strokeWidth = 4f; color = Color.GREEN
    }
    private val kptPaint = Paint(Paint.ANTI_ALIAS_FLAG).apply {
        style = Paint.Style.FILL; color = Color.RED; strokeWidth = 6f
    }
    private val bonePaint = Paint(Paint.ANTI_ALIAS_FLAG).apply {
        style = Paint.Style.STROKE; strokeWidth = 3f; color = Color.YELLOW
    }

    /**
     * 17 点 COCO pose 标准骨架连线（pair index → pair index）。
     * 索引对应：0 nose, 1 left_eye, 2 right_eye, 3 left_ear, 4 right_ear,
     * 5 left_shoulder, 6 right_shoulder, 7 left_elbow, 8 right_elbow,
     * 9 left_wrist, 10 right_wrist, 11 left_hip, 12 right_hip,
     * 13 left_knee, 14 right_knee, 15 left_ankle, 16 right_ankle
     */
    private val SKELETON: List<Pair<Int, Int>> = listOf(
        0 to 1, 0 to 2,              // nose ↔ eyes
        1 to 3, 2 to 4,              // eyes ↔ ears
        5 to 6,                      // shoulders
        5 to 7, 7 to 9,              // left arm
        6 to 8, 8 to 10,             // right arm
        5 to 11, 6 to 12,            // torso sides
        11 to 12,                    // hips
        11 to 13, 13 to 15,          // left leg
        12 to 14, 14 to 16,          // right leg
    )

    override fun onDraw(canvas: Canvas) {
        super.onDraw(canvas)
        // 与 PreviewView (默认 FILL_CENTER) 一致的等比例铺满映射：统一缩放 S=max(…/bitW, …/bitH)
        // 填满 view，并以 view 中心对齐（bitmap 中心 ↔ view 中心）。bitmap 宽高比 == view 宽高比
        // 时退化为简单线性映射；不等时按中心裁切，避免边缘框点被各向异性缩放挤偏。
        val vw = width.toFloat(); val vh = height.toFloat()
        val s = maxOf(vw / bitW, vh / bitH)
        val ox = (vw - bitW * s) * 0.5f; val oy = (vh - bitH * s) * 0.5f
        fun tx(bx: Float) = bx * s + ox
        fun ty(by: Float) = by * s + oy
        for (det in detections) {
            val l = tx(det.box.left); val t = ty(det.box.top)
            val r = tx(det.box.right); val b = ty(det.box.bottom)
            canvas.drawRect(l, t, r, b, boxPaint)
            for (kp in det.visibleKeypoints) {
                canvas.drawCircle(tx(kp.x), ty(kp.y), 5f, kptPaint)
            }
            for ((a, bIdx) in SKELETON) {
                if (a >= det.keypoints.size || bIdx >= det.keypoints.size) continue
                val ka = det.keypoints[a]; val kb = det.keypoints[bIdx]
                if (ka.x.isNaN() || kb.x.isNaN()) continue
                canvas.drawLine(tx(ka.x), ty(ka.y), tx(kb.x), ty(kb.y), bonePaint)
            }
        }
    }
}

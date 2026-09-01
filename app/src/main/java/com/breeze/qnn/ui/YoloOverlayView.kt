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
 * 坐标是 640×640 tensor 空间；按 view 尺寸等比缩放到当前 view。
 * letterbox 反演交给 fragment 传入的 detections 处理（当前 MVP 阶段先 linear scale，
 * 因 camera preview 多已裁切到接近 640×640，T13 实测时再视偏差补 un-letterbox）。
 */
class YoloOverlayView @JvmOverloads constructor(
    context: Context, attrs: AttributeSet? = null
) : View(context, attrs) {

    var detections: List<Detection> = emptyList()
        set(value) { field = value; invalidate() }

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
        val vw = width.toFloat(); val vh = height.toFloat()
        val sx = vw / 640f; val sy = vh / 640f
        for (det in detections) {
            val l = det.box.left * sx; val t = det.box.top * sy
            val r = det.box.right * sx; val b = det.box.bottom * sy
            canvas.drawRect(l, t, r, b, boxPaint)
            for (kp in det.visibleKeypoints) {
                canvas.drawCircle(kp.x * sx, kp.y * sy, 5f, kptPaint)
            }
            for ((a, bIdx) in SKELETON) {
                if (a >= det.keypoints.size || bIdx >= det.keypoints.size) continue
                val ka = det.keypoints[a]; val kb = det.keypoints[bIdx]
                if (ka.x.isNaN() || kb.x.isNaN()) continue
                canvas.drawLine(ka.x * sx, ka.y * sy, kb.x * sx, kb.y * sy, bonePaint)
            }
        }
    }
}

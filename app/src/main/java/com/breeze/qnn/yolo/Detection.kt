package com.breeze.qnn.yolo

import android.graphics.PointF
import android.graphics.RectF

/** 单次检测结果：bbox (xyxy) + keypoints (17 COCO pose, 每点 xy)
 *
 * 坐标全部在 [backend.infer] 输入的 bitmap 原始像素空间（native 已反 letterbox 回
 * srcWidth×srcHeight），YoloOverlayView 按 bitmap 实际宽高比缩放到 view 即可。
 * 注意：不是 640×640 tensor 空间——若直接拿 640 当缩放基准会错位。 */
data class Detection(
    val cls: Int,
    val clsName: String,
    val score: Float,
    val box: RectF,
    val keypoints: List<PointF>,
) {
    val visibleKeypoints: List<PointF>
        get() = keypoints.filter { !it.x.isNaN() && !it.y.isNaN() }
}

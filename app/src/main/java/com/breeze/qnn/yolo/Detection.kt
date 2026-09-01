package com.breeze.qnn.yolo

import android.graphics.PointF
import android.graphics.RectF

/** 单次检测结果：bbox (xyxy, 640x640 tensor 空间) + keypoints (17 COCO pose, 每点 xy) */
data class Detection(
    val cls: Int,
    val clsName: String,
    val score: Float,
    val box: RectF,            // 640×640 tensor 空间坐标，preview 渲染前要反演
    val keypoints: List<PointF>, // 17 个 (x, y)，每个都是 640×640 tensor 坐标（未反演）
) {
    val visibleKeypoints: List<PointF>
        get() = keypoints.filter { !it.x.isNaN() && !it.y.isNaN() }
}

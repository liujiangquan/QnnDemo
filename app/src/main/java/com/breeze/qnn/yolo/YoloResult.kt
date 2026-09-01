package com.breeze.qnn.yolo

/** 一次推理的完整结果 + 性能统计 */
data class YoloResult(
    val detections: List<Detection>,
    val elapsedMs: Long,
    val frameWidth: Int,
    val frameHeight: Int,
) {
    val personCount: Int get() = detections.count { it.clsName == "person" }
}

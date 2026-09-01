package com.breeze.qnn.yolo

import android.graphics.PointF
import android.graphics.RectF

/**
 * 解析 YoloPoseSession::decodePose 的 flat float[] 输出。
 *
 * 输出布局（native yolo_pose.cpp 产出）：
 *   flat[0]                       = 检出数 count
 *   flat[1 + i*70 + 0]            = 第 i 个候选的 conf
 *   flat[1 + i*70 + 1 .. +4]      = x1, y1, x2, y2（640×640 tensor 空间，已反 letterbox 回原始输入坐标）
 *   flat[1 + i*70 + 5 .. +5+51]   = 17 keypoints × (x, y, visible)
 *
 * 推理坐标就是原始输入 bitmap 像素坐标（native decodePose 里减掉了 mPadX/mPadY 并除以 mScale），
 * 所以 [Detection.box] / [Detection.keypoints] 直接在 bitmap 坐标系里，YoloOverlayView 直接按
 * view/bmp 宽高比缩放开画即可，无需再 un-letterbox。
 *
 * 单类 person 模型（cls 恒为 0）。
 */
object YoloPostprocessor {

    /** 每个候选占 1 conf + 4 bbox + 51 kpt = 56 个 float */
    private const val STRIDE = 1 + 4 + 17 * 3

    fun parseFlat(
        flat: FloatArray,
        classNames: List<String> = listOf("person"),
        confThr: Float = 0.25f,
        iouThr: Float = 0.45f,
    ): List<Detection> {
        if (flat.isEmpty()) return emptyList()
        val count = flat[0].toInt().coerceIn(0, flat.size)
        val raw = ArrayList<ScaledDet>(count)
        for (i in 0 until count) {
            val base = 1 + i * STRIDE
            if (base + STRIDE > flat.size) break
            val conf = flat[base + 0]
            if (conf < confThr) continue
            val x1 = flat[base + 1]
            val y1 = flat[base + 2]
            val x2 = flat[base + 3]
            val y2 = flat[base + 4]
            val kpts = ArrayList<PointF>(17)
            for (k in 0 until 17) {
                val kx = flat[base + 5 + k * 3 + 0]
                val ky = flat[base + 5 + k * 3 + 1]
                val vis = flat[base + 5 + k * 3 + 2]
                kpts.add(if (vis > 0.5f) PointF(kx, ky) else PointF(Float.NaN, Float.NaN))
            }
            raw.add(ScaledDet(conf, RectF(x1, y1, x2, y2), kpts))
        }
        if (raw.size <= 1) {
            return raw.map { toDetection(it, classNames) }
        }
        val indices = nms(raw, iouThr)
        return indices.map { raw[it] }.map { toDetection(it, classNames) }
    }

    private fun toDetection(d: ScaledDet, classNames: List<String>): Detection =
        Detection(0, classNames.getOrElse(0) { "person" }, d.conf, d.box, d.keypoints)

    /**
     * 贪婪 NMS：按 conf 降序挑出候选，剔除 IoU > iouThr 的同框候选。
     *
     * native decodePose 虽已做一次 NMS，但多 anchor 同一个人仍可能挤出重复框，
     * 这里再兜底一次。若 T13 实测发现无重复框，可去掉本调用直接返回 raw。
     */
    fun nms(items: List<ScaledDet>, iouThr: Float): List<Int> {
        val sorted = items.withIndex().sortedByDescending { it.value.conf }.toMutableList()
        val kept = ArrayList<Int>()
        while (sorted.isNotEmpty()) {
            val cur = sorted.removeAt(0)
            kept.add(cur.index)
            val it = sorted.iterator()
            while (it.hasNext()) {
                val n = it.next()
                if (iouXyxy(cur.value.box, n.value.box) > iouThr) it.remove()
            }
        }
        return kept
    }

    private fun iouXyxy(a: RectF, b: RectF): Float {
        val ix1 = maxOf(a.left, b.left); val iy1 = maxOf(a.top, b.top)
        val ix2 = minOf(a.right, b.right); val iy2 = minOf(a.bottom, b.bottom)
        val iw = (ix2 - ix1).coerceAtLeast(0f); val ih = (iy2 - iy1).coerceAtLeast(0f)
        val inter = iw * ih
        val areaA = (a.right - a.left) * (a.bottom - a.top)
        val areaB = (b.right - b.left) * (b.bottom - b.top)
        val union = areaA + areaB - inter
        return if (union <= 0f) 0f else inter / union
    }

    class ScaledDet(
        val conf: Float,
        val box: RectF,
        val keypoints: List<PointF>,
    )
}

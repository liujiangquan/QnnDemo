package com.breeze.qnn.yolo

import android.graphics.PointF
import android.graphics.RectF
import java.nio.ByteBuffer
import java.nio.ByteOrder

/**
 * 解析 YOLO26-pose end2end head 输出。
 *
 * T2 实测：单张量 output0 展平 = [1, 300, 57]，即 300 个 top-K 候选（NMS 烘焙在 head 内）、
 * 每个候选 57 通道。通道布局（4 + 1 + 51 + 1 = 57）：
 *   [0:4]   = bbox (cx, cy, w, h) 640×640 tensor 空间
 *   [4]     = objectness / cls_conf（end2end head 单类 person 模型，可当分数用）
 *   [5:56]  = 17 keypoints × 3 (x, y, visible) → 51 通道
 *   [56]    = padding
 *
 * 反演 letterbox 到预览坐标留给 T8 YoloOverlayView 用 padX/padY/scale 一起做，
 * postprocessor 只负责把 raw bytes 解成 tensor 坐标的 List<Detection>。
 *
 * 若 T13 实测解码偏（kpt 越界 / 框飞掉），回头调 [0:4] / [4] / [5:6] 三个索引区间：
 * - 若 51 不是 kpt 数（比如是 17×3 + 3 别的东西），调整 kptOffset
 * - 若分数要 (objectness × cls_prob) 才稳，把 conf 计算换成乘法
 * - end2end head 通常已烘焙 NMS；若发现大量重复框，把下面 val raw 后打开 nms 调用
 */
object YoloPostprocessor {

    /** 一次推理的 top-K 候选数（见 DLC 输出 shape [1, 300, 57]） */
    const val TOP_K = 300
    /** 每个候选的通道数（4 bbox + 1 conf + 51 kpt + 1 pad） */
    const val CHANNELS = 57

    fun parse(
        outputs: List<ByteArray>,
        classNames: List<String> = listOf("person"),
        confThr: Float = 0.25f,
        iouThr: Float = 0.45f,
    ): List<Detection> {
        val buf = outputs.singleOrNull() ?: return emptyList()
        val fb = ByteBuffer.wrap(buf).order(ByteOrder.nativeOrder()).asFloatBuffer()
        val flat = FloatArray(fb.capacity()).also { fb.get(it) }
        val raw = ArrayList<Triple<Float, FloatArray, FloatArray>>()
        for (i in 0 until TOP_K) {
            val base = i * CHANNELS
            if (base + CHANNELS > flat.size) break
            val cx = flat[base + 0]
            val cy = flat[base + 1]
            val w = flat[base + 2]
            val h = flat[base + 3]
            val conf = flat[base + 4]
            if (conf < confThr) continue
            val kpts = FloatArray(17 * 3)
            for (k in 0 until 17) {
                kpts[k * 3 + 0] = flat[base + 5 + k * 3 + 0]  // x
                kpts[k * 3 + 1] = flat[base + 5 + k * 3 + 1]  // y
                kpts[k * 3 + 2] = flat[base + 5 + k * 3 + 2]  // visible
            }
            raw += Triple(conf, floatArrayOf(cx, cy, w, h), kpts)
        }
        // end2end head 通常已烘焙 NMS；若 T13 发现重复框多，打开下一行
        // val indices = nms(raw, iouThr)
        val indices = raw.indices.toList()

        val result = ArrayList<Detection>(indices.size)
        for (i in indices) {
            val (conf, box, kpts) = raw[i]
            val x1 = box[0] - box[2] / 2f
            val y1 = box[1] - box[3] / 2f
            val x2 = box[0] + box[2] / 2f
            val y2 = box[1] + box[3] / 2f
            val pts = ArrayList<PointF>(17)
            for (k in 0 until 17) {
                val vx = kpts[k * 3 + 0]
                val vy = kpts[k * 3 + 1]
                val vis = kpts[k * 3 + 2]
                if (vis > 0.5f && vx.isFinite() && vy.isFinite()) {
                    pts.add(PointF(vx, vy))  // 640×640 tensor 坐标，待 overlay un-letterbox
                } else {
                    pts.add(PointF(Float.NaN, Float.NaN))
                }
            }
            // 单类 person 模型，cls 恒为 0
            result.add(Detection(0, classNames.getOrElse(0) { "person" }, conf, RectF(x1, y1, x2, y2), pts))
        }
        return result
    }

    /**
     * 贪婪 NMS：按 conf 降序挑出候选，剔除 IoU > iouThr 的同框候选。
     *
     * 仅当 T13 实测发现重复框很多时使用；正常 end2end head 不需要。
     */
    fun nms(items: List<Triple<Float, FloatArray, FloatArray>>, iouThr: Float): List<Int> {
        val sorted = items.withIndex().sortedByDescending { it.value.first }.toMutableList()
        val kept = ArrayList<Int>()
        while (sorted.isNotEmpty()) {
            val cur = sorted.removeAt(0)
            kept.add(cur.index)
            val it = sorted.iterator()
            while (it.hasNext()) {
                val n = it.next()
                if (iou(cur.value.second, n.value.second) > iouThr) it.remove()
            }
        }
        return kept
    }

    private fun iou(a: FloatArray, b: FloatArray): Float {
        val ax1 = a[0] - a[2] / 2; val ay1 = a[1] - a[3] / 2
        val ax2 = a[0] + a[2] / 2; val ay2 = a[1] + a[3] / 2
        val bx1 = b[0] - b[2] / 2; val by1 = b[1] - b[3] / 2
        val bx2 = b[0] + b[2] / 2; val by2 = b[1] + b[3] / 2
        val ix1 = maxOf(ax1, bx1); val iy1 = maxOf(ay1, by1)
        val ix2 = minOf(ax2, bx2); val iy2 = minOf(ay2, by2)
        val iw = (ix2 - ix1).coerceAtLeast(0f); val ih = (iy2 - iy1).coerceAtLeast(0f)
        val inter = iw * ih
        val union = (ax2 - ax1) * (ay2 - ay1) + (bx2 - bx1) * (by2 - by1) - inter
        return if (union <= 0f) 0f else inter / union
    }
}

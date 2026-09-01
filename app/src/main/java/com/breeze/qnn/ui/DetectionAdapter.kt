package com.breeze.qnn.ui

import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.TextView
import androidx.recyclerview.widget.RecyclerView
import com.breeze.qnn.R
import com.breeze.qnn.yolo.Detection

/** YOLO 检测列表 adapter：每行显示 cls 名 + 置信度 + 可见 kpt 数 */
class DetectionAdapter : RecyclerView.Adapter<DetectionAdapter.VH>() {
    private val items = ArrayList<Detection>()

    fun submit(d: List<Detection>) {
        items.clear(); items.addAll(d); notifyDataSetChanged()
    }

    override fun onCreateViewHolder(p: ViewGroup, i: Int): VH {
        val v = LayoutInflater.from(p.context).inflate(R.layout.item_detection, p, false)
        return VH(v)
    }

    override fun onBindViewHolder(h: VH, i: Int) {
        val d = items[i]
        h.tv.text = "${d.clsName} ${(d.score * 100).toInt()}% · " +
            "${d.visibleKeypoints.size}/17 kpts"
    }

    override fun getItemCount() = items.size

    class VH(v: View) : RecyclerView.ViewHolder(v) {
        val tv: TextView = v.findViewById(R.id.tv_detection)
    }
}

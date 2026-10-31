package com.breeze.qnn.ui

import android.content.Context
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.view.LayoutInflater
import android.view.ViewGroup
import android.widget.ImageView
import android.widget.TextView
import androidx.recyclerview.widget.RecyclerView
import com.breeze.qnn.ClipIndex
import com.breeze.qnn.R
import java.util.concurrent.Executors

/**
 * 搜图结果列表：每行 = 缩略图 + 概率。
 * 概率是对 100×cosine 做 softmax 后的相对值（候选间可比，绝对值不作阈值）。
 */
class ClipResultAdapter(
    private val context: Context,
    private val index: ClipIndex,
) : RecyclerView.Adapter<ClipResultAdapter.Holder>() {

    /** 一条结果：图片 ID 与其概率。 */
    data class Result(val id: Long, val probability: Float)

    private var items: List<Result> = emptyList()
    private val thumbPool = Executors.newFixedThreadPool(2)

    fun submit(list: List<Result>) {
        items = list
        notifyDataSetChanged()
    }

    class Holder(view: android.view.View) : RecyclerView.ViewHolder(view) {
        val thumb: ImageView = view.findViewById(R.id.iv_clip_thumb)
        val score: TextView = view.findViewById(R.id.tv_clip_score)
        var boundId: Long = -1L
    }

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): Holder {
        val v = LayoutInflater.from(parent.context).inflate(R.layout.item_clip_result, parent, false)
        return Holder(v)
    }

    override fun getItemCount(): Int = items.size

    override fun onBindViewHolder(holder: Holder, position: Int) {
        val item = items[position]
        holder.boundId = item.id
        holder.thumb.setImageBitmap(null)
        holder.score.text = "相关度 %.1f%%".format(item.probability * 100f)
        thumbPool.execute {
            val bmp = loadThumb(item.id)
            holder.thumb.post {
                // 复用导致错位时丢弃过期结果
                if (holder.boundId == item.id) holder.thumb.setImageBitmap(bmp)
            }
        }
    }

    /** 以 2 倍采样解码缩略图，避免整图解码拖慢列表。 */
    private fun loadThumb(id: Long): Bitmap? = runCatching {
        val opts = BitmapFactory.Options().apply { inSampleSize = 2 }
        context.contentResolver.openInputStream(index.uriOf(id))?.use { s ->
            BitmapFactory.decodeStream(s, null, opts)
        }
    }.getOrNull()

    fun shutdown() {
        thumbPool.shutdownNow()
    }
}

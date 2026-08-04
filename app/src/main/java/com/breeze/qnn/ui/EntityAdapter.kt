package com.breeze.qnn.ui

import android.graphics.Color
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.TextView
import androidx.recyclerview.widget.RecyclerView
import com.breeze.qnn.R
import com.breeze.qnn.ner.Entity
import com.breeze.qnn.ner.EntitySource
import com.breeze.qnn.ner.EntityType

/** 实体类型 → 高亮/标签颜色。TIME 用灰色弱化（模型 README 说它不算敏感信息）。 */
object EntityColors {
    fun of(type: EntityType): Int = when (type) {
        EntityType.PER -> Color.parseColor("#D32F2F")
        EntityType.LOC -> Color.parseColor("#1976D2")
        EntityType.ORG -> Color.parseColor("#388E3C")
        EntityType.TIME -> Color.parseColor("#9E9E9E")
        // 正则类统一紫色系
        EntityType.PHONE, EntityType.ID_CARD, EntityType.BANK_CARD,
        EntityType.EMAIL, EntityType.PLATE -> Color.parseColor("#7B1FA2")
    }
}

class EntityAdapter : RecyclerView.Adapter<EntityAdapter.VH>() {

    private val items = mutableListOf<Entity>()

    fun submit(list: List<Entity>) {
        items.clear()
        items += list
        notifyDataSetChanged()
    }

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): VH =
        VH(LayoutInflater.from(parent.context).inflate(R.layout.item_entity, parent, false))

    override fun getItemCount() = items.size

    override fun onBindViewHolder(holder: VH, position: Int) {
        val e = items[position]
        holder.type.text = e.type.name
        holder.type.setTextColor(EntityColors.of(e.type))
        holder.text.text = e.text
        val src = if (e.source == EntitySource.REGEX) "正则" else "模型"
        holder.meta.text = "[${e.charStart},${e.charEnd}) %.2f %s".format(e.score, src)
    }

    class VH(v: View) : RecyclerView.ViewHolder(v) {
        val type: TextView = v.findViewById(R.id.entity_type)
        val text: TextView = v.findViewById(R.id.entity_text)
        val meta: TextView = v.findViewById(R.id.entity_meta)
    }
}

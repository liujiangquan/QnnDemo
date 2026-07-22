package com.breeze.qnn.ui

import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.TextView
import androidx.recyclerview.widget.RecyclerView
import com.breeze.qnn.R

class ChatAdapter : RecyclerView.Adapter<ChatAdapter.VH>() {

    private val items = mutableListOf<ChatMessage>()

    fun append(msg: ChatMessage) {
        items += msg
        notifyItemInserted(items.size - 1)
    }

    fun appendToLast(text: String) {
        if (items.isEmpty()) return
        val last = items.last()
        last.text += text
        notifyItemChanged(items.size - 1)
    }

    fun markLastComplete(tokensPerSec: Double) {
        if (items.isEmpty()) return
        val last = items.last()
        last.isStreaming = false
        last.speedTokensPerSec = tokensPerSec
        notifyItemChanged(items.size - 1)
    }

    fun snapshot(): List<ChatMessage> = items.toList()

    override fun getItemViewType(position: Int) =
        if (items[position].role == ChatMessage.Role.USER) 0 else 1

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): VH {
        val layout = if (viewType == 0) R.layout.item_chat_user else R.layout.item_chat_assistant
        val v = LayoutInflater.from(parent.context).inflate(layout, parent, false)
        return VH(v, viewType)
    }

    override fun getItemCount() = items.size

    override fun onBindViewHolder(holder: VH, position: Int) {
        val msg = items[position]
        holder.text.text = msg.text
        holder.meta?.let {
            if (msg.role == ChatMessage.Role.ASSISTANT && msg.speedTokensPerSec != null) {
                it.text = "%.1f tok/s".format(msg.speedTokensPerSec)
                it.visibility = View.VISIBLE
            } else {
                it.visibility = View.GONE
            }
        }
    }

    class VH(v: View, viewType: Int) : RecyclerView.ViewHolder(v) {
        val text: TextView = v.findViewById(
            if (viewType == 0) R.id.user_text else R.id.assistant_text)
        val meta: TextView? = if (viewType == 1) v.findViewById(R.id.assistant_meta) else null
    }
}

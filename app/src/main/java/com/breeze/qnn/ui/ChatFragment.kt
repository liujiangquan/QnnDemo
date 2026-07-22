package com.breeze.qnn.ui

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.Button
import android.widget.EditText
import android.widget.TextView
import androidx.fragment.app.Fragment
import androidx.fragment.app.viewModels
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView
import com.breeze.qnn.R

class ChatFragment : Fragment() {

    private val viewModel: LlmViewModel by viewModels()
    private lateinit var adapter: ChatAdapter
    private lateinit var rv: RecyclerView
    private lateinit var etInput: EditText
    private lateinit var btnSend: Button
    private lateinit var btnStop: Button
    private lateinit var banner: TextView

    override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?, savedInstanceState: Bundle?): View =
        inflater.inflate(R.layout.fragment_chat, container, false)

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        rv       = view.findViewById(R.id.rv_messages)
        etInput  = view.findViewById(R.id.et_input)
        btnSend  = view.findViewById(R.id.btn_send)
        btnStop  = view.findViewById(R.id.btn_stop)
        banner   = view.findViewById(R.id.status_banner)

        adapter = ChatAdapter()
        rv.layoutManager = LinearLayoutManager(requireContext())
        rv.adapter = adapter

        // Render existing history messages
        viewModel.messages.forEach { adapter.append(it) }

        viewModel.state.observe(viewLifecycleOwner) { state ->
            when (state) {
                LlmViewModel.State.INIT -> {
                    banner.text = "初始化中..."; banner.visibility = View.VISIBLE
                    btnSend.isEnabled = false
                }
                LlmViewModel.State.LOADING -> {
                    banner.text = "加载模型中..."; banner.visibility = View.VISIBLE
                    btnSend.isEnabled = false
                }
                LlmViewModel.State.READY -> {
                    banner.visibility = View.GONE
                    btnSend.isEnabled = true
                    btnStop.visibility = View.GONE
                }
                LlmViewModel.State.GENERATING -> {
                    btnSend.isEnabled = false
                    btnStop.visibility = View.VISIBLE
                }
                LlmViewModel.State.WEIGHTS_MISSING -> {
                    banner.text = "权重未预置。请跑：\nbash docs/setup_qwen3_llm.sh <src_dir>"
                    banner.visibility = View.VISIBLE
                    btnSend.isEnabled = false
                }
                else -> {}
            }
        }
        viewModel.error.observe(viewLifecycleOwner) { msg ->
            msg?.let { banner.text = "错误: $it"; banner.visibility = View.VISIBLE }
        }

        btnSend.setOnClickListener {
            val text = etInput.text.toString().trim()
            if (text.isEmpty()) return@setOnClickListener
            etInput.text.clear()

            val params = SamplingSettings(requireContext()).toSamplingParams()
            viewModel.send(text, params,
                onTokenAppended = { idx ->
                    requireActivity().runOnUiThread {
                        if (adapter.itemCount <= idx) {
                            adapter.append(viewModel.messages[idx - 1])  // user
                            adapter.append(viewModel.messages[idx])       // assistant
                        }
                        adapter.notifyItemChanged(idx)
                        rv.smoothScrollToPosition(adapter.itemCount - 1)
                    }
                },
                onComplete = { idx ->
                    requireActivity().runOnUiThread {
                        val speed = viewModel.messages[idx].speedTokensPerSec ?: 0.0
                        adapter.markLastComplete(speed)
                    }
                })
        }

        btnStop.setOnClickListener { viewModel.stopCurrentGeneration() }

        viewModel.initIfNeeded()
    }
}

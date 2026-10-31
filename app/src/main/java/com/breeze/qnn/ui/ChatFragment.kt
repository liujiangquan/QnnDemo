package com.breeze.qnn.ui

import android.Manifest
import android.content.pm.PackageManager
import android.os.Bundle
import androidx.activity.result.contract.ActivityResultContracts
import androidx.core.content.ContextCompat
import android.widget.Toast
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.Button
import android.widget.ImageButton
import android.view.MotionEvent
import android.widget.EditText
import android.widget.TextView
import androidx.fragment.app.Fragment
import androidx.fragment.app.viewModels
import androidx.lifecycle.lifecycleScope
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
    private lateinit var btnVoice: ImageButton
    private var voice: VoiceInput? = null

    private val micPermission =
        registerForActivityResult(ActivityResultContracts.RequestPermission()) { granted ->
            if (granted) toast("已授权，请按住语音按钮说话") else toast("需要麦克风权限才能语音输入")
        }
    private lateinit var banner: TextView
    /** true 时 token 流入自动滚到底；用户往上滑就变 false，回到底部再变 true。 */
    private var autoScroll: Boolean = true

    override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?, savedInstanceState: Bundle?): View =
        inflater.inflate(R.layout.fragment_chat, container, false)

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        rv       = view.findViewById(R.id.rv_messages)
        etInput  = view.findViewById(R.id.et_input)
        btnSend  = view.findViewById(R.id.btn_send)
        btnStop  = view.findViewById(R.id.btn_stop)
        btnVoice = view.findViewById(R.id.btn_voice)
        banner   = view.findViewById(R.id.status_banner)

        adapter = ChatAdapter()
        rv.layoutManager = LinearLayoutManager(requireContext())
        rv.adapter = adapter
        // 关闭 item 动画，避免流式 notifyItemChanged 每帧触发变高动画 → 视觉抖动
        rv.itemAnimator = null
        // 监听用户是否手动往上滑，滑离底部就停止 autoScroll；滑回底部恢复。
        rv.addOnScrollListener(object : RecyclerView.OnScrollListener() {
            override fun onScrolled(v: RecyclerView, dx: Int, dy: Int) {
                val lm = v.layoutManager as LinearLayoutManager
                val lastVisible = lm.findLastVisibleItemPosition()
                autoScroll = lastVisible >= adapter.itemCount - 1
            }
        })

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
            sendText(text)
        }
        btnStop.setOnClickListener { viewModel.stopCurrentGeneration() }

        btnVoice.setOnTouchListener { view, event ->
            when (event.actionMasked) {
                MotionEvent.ACTION_DOWN -> {
                    view.alpha = 0.6f
                    val granted = ContextCompat.checkSelfPermission(
                        requireContext(), Manifest.permission.RECORD_AUDIO) == PackageManager.PERMISSION_GRANTED
                    if (granted) startVoice() else micPermission.launch(Manifest.permission.RECORD_AUDIO)
                    true
                }
                MotionEvent.ACTION_UP, MotionEvent.ACTION_CANCEL -> {
                    view.alpha = 1f
                    voice?.stop()
                    if (event.actionMasked == MotionEvent.ACTION_UP) view.performClick()
                    true
                }
                else -> false
            }
        }

        viewModel.initIfNeeded()
    }

    private fun startVoice() {
        if (viewModel.state.value != LlmViewModel.State.READY) {
            toast("模型未就绪，稍后再试")
            return
        }
        val v = voice ?: VoiceInput(
            context = requireContext(),
            scope = viewLifecycleOwner.lifecycleScope,
            onPartial = { text -> requireActivity().runOnUiThread { etInput.setText(text) } },
            onFinal = { text ->
                requireActivity().runOnUiThread {
                    btnSend.isEnabled = true
                    if (text.isEmpty()) {
                        toast("未识别到语音")
                    } else {
                        etInput.setText(text)
                        sendText(text)
                    }
                }
            },
            onError = { msg -> requireActivity().runOnUiThread { toast(msg) } },
        ).also { voice = it }
        btnSend.isEnabled = false
        etInput.text.clear()
        v.start()
    }

    private fun toast(msg: String) {
        if (isAdded) Toast.makeText(requireContext(), msg, Toast.LENGTH_SHORT).show()
    }

    override fun onDestroyView() {
        voice?.close()
        voice = null
        super.onDestroyView()
    }

    private fun sendText(text: String) {
        etInput.text.clear()
        val params = SamplingSettings(requireContext()).toSamplingParams()
            viewModel.send(text, params,
                onTokenAppended = { idx ->
                    requireActivity().runOnUiThread {
                        if (adapter.itemCount <= idx) {
                            adapter.append(viewModel.messages[idx - 1])  // user
                            adapter.append(viewModel.messages[idx])       // assistant
                            autoScroll = true  // 新一轮开始默认粘底
                        }
                        adapter.notifyItemChanged(idx)
                        // 用 scrollToPosition 走瞬时跳而不是 smoothScrollToPosition ——
                        // 流式 token 每 ~38ms 一次，smooth 动画会互相打断造成抖。
                        // 只有用户没手动往上滑时才自动粘底。
                        if (autoScroll) rv.scrollToPosition(adapter.itemCount - 1)
                    }
                },
                onComplete = { idx ->
                    requireActivity().runOnUiThread {
                        val speed = viewModel.messages[idx].speedTokensPerSec ?: 0.0
                        adapter.markLastComplete(speed)
                    }
                })
    }
}

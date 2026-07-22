package com.breeze.qnn.ui

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.ArrayAdapter
import androidx.fragment.app.Fragment
import androidx.lifecycle.lifecycleScope
import com.breeze.qnn.InferenceEngine
import com.breeze.qnn.QnnNative
import com.breeze.qnn.R
import com.breeze.qnn.databinding.FragmentBackendBinding
import kotlinx.coroutines.launch

/**
 * CNN 后端推理 Fragment。
 *
 * 从旧 MainActivity 迁移而来，行为不变：
 *   1. 选择后端（CPU/GPU/HTP）
 *   2. 点击「初始化」创建 QNN runtime
 *   3. 点击「加载 DLC」从 assets 加载模型
 *   4. 查看图与张量元信息
 *   5. 点击「执行推理」生成随机输入并执行一次推理
 *   6. 显示耗时与输出张量大小
 */
class BackendFragment : Fragment() {

    private var _binding: FragmentBackendBinding? = null
    private val binding get() = _binding!!

    private lateinit var engine: InferenceEngine
    private var selectedBackend: QnnNative.Backend = QnnNative.Backend.CPU

    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View {
        _binding = FragmentBackendBinding.inflate(inflater, container, false)
        return binding.root
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        engine = InferenceEngine(requireContext())

        // 后端选择
        binding.radioGroupBackend.setOnCheckedChangeListener { _, id ->
            selectedBackend = when (id) {
                R.id.radioCpu -> QnnNative.Backend.CPU
                R.id.radioGpu -> QnnNative.Backend.GPU
                R.id.radioHtp -> QnnNative.Backend.HTP
                else -> QnnNative.Backend.CPU
            }
        }

        // 初始化
        binding.btnInit.setOnClickListener {
            viewLifecycleOwner.lifecycleScope.launch {
                setBusy(true)
                val ok = engine.init()
                appendLog(if (ok) "初始化成功" else "初始化失败")
                setBusy(false)
            }
        }

        // 加载 DLC
        binding.btnLoadDlc.setOnClickListener {
            val assetName = binding.editDlcName.text.toString().trim().ifEmpty { DEFAULT_DLC }
            viewLifecycleOwner.lifecycleScope.launch {
                setBusy(true)
                val ok = engine.loadDlcFromAssets(assetName, selectedBackend)
                if (ok) {
                    showGraphInfos()
                    appendLog("DLC 加载成功: $assetName (backend=$selectedBackend)")
                } else {
                    appendLog("DLC 加载失败: $assetName")
                }
                setBusy(false)
            }
        }

        // 执行推理
        binding.btnExecute.setOnClickListener {
            if (engine.graphInfos.isEmpty()) {
                appendLog("请先加载 DLC 模型")
                return@setOnClickListener
            }
            val graph = engine.graphInfos[binding.spinnerGraph.selectedItemPosition.coerceAtMost(
                engine.graphInfos.size - 1
            )]
            viewLifecycleOwner.lifecycleScope.launch {
                setBusy(true)
                val inputs = engine.makeRandomInputs(graph)
                appendLog("执行推理: graph=${graph.name}, 输入数=${inputs.size}")
                val result = engine.execute(graph.name, inputs)
                if (result.isSuccess) {
                    appendLog("推理成功: 耗时=%.3f ms, 输出数=%d".format(
                        result.elapsedMs, result.outputSizes.size))
                    result.outputSizes.forEachIndexed { i, s ->
                        appendLog("  输出[$i] 大小=$s 字节")
                    }
                } else {
                    appendLog("推理失败: ${result.error}")
                }
                setBusy(false)
            }
        }
    }

    override fun onDestroyView() {
        super.onDestroyView()
        engine.close()
        _binding = null
    }

    private fun showGraphInfos() {
        val graphs = engine.graphInfos
        val names = graphs.map { it.name }.toTypedArray()
        val adapter = ArrayAdapter(
            requireContext(), android.R.layout.simple_spinner_item, names
        )
        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item)
        binding.spinnerGraph.adapter = adapter

        val sb = StringBuilder()
        sb.append("图数量: ${graphs.size}\n")
        graphs.forEach { g ->
            sb.append("\n图: ${g.name}\n")
            sb.append("  输入 (${g.inputs.size}):\n")
            g.inputs.forEach { t ->
                sb.append("    - ${t.name}: [${t.dimString()}] ${t.dataType} (${t.totalBytes} B)\n")
            }
            sb.append("  输出 (${g.outputs.size}):\n")
            g.outputs.forEach { t ->
                sb.append("    - ${t.name}: [${t.dimString()}] ${t.dataType} (${t.totalBytes} B)\n")
            }
        }
        binding.textGraphInfo.text = sb.toString()
    }

    private fun appendLog(msg: String) {
        val cur = binding.textLog.text.toString()
        val new = if (cur.isEmpty()) msg else "$cur\n$msg"
        binding.textLog.text = new
    }

    private fun setBusy(busy: Boolean) {
        binding.progressBar.visibility = if (busy) View.VISIBLE else View.GONE
        binding.btnInit.isEnabled = !busy
        binding.btnLoadDlc.isEnabled = !busy
        binding.btnExecute.isEnabled = !busy
    }

    private companion object {
        const val DEFAULT_DLC = "model.dlc"
    }
}

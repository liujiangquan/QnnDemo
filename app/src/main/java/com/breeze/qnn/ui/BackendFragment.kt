package com.breeze.qnn.ui

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.fragment.app.Fragment
import androidx.lifecycle.lifecycleScope
import com.breeze.qnn.InferenceEngine
import com.breeze.qnn.QnnNative
import com.breeze.qnn.R
import com.breeze.qnn.databinding.FragmentBackendBinding
import kotlinx.coroutines.launch

/** 原通用 QNN Demo：后端选择 / 加载 DLC / 执行推理（逻辑不变，从 MainActivity 平移）。 */
class BackendFragment : Fragment() {
    private var _b: FragmentBackendBinding? = null
    private val b get() = _b!!
    private lateinit var engine: InferenceEngine
    private var selectedBackend = QnnNative.Backend.CPU

    override fun onCreateView(i: LayoutInflater, c: ViewGroup?, s: Bundle?): View {
        _b = FragmentBackendBinding.inflate(i, c, false); return b.root
    }

    override fun onViewCreated(view: View, s: Bundle?) {
        engine = InferenceEngine(requireContext())
        b.radioGroupBackend.setOnCheckedChangeListener { _, id ->
            selectedBackend = when (id) {
                R.id.radioGpu -> QnnNative.Backend.GPU
                R.id.radioHtp -> QnnNative.Backend.HTP
                else -> QnnNative.Backend.CPU
            }
        }
        b.btnInit.setOnClickListener {
            lifecycleScope.launch { setBusy(true); appendLog(if (engine.init()) "初始化成功" else "初始化失败"); setBusy(false) }
        }
        b.btnLoadDlc.setOnClickListener {
            val name = b.editDlcName.text.toString().trim().ifEmpty { "model.dlc" }
            lifecycleScope.launch {
                setBusy(true)
                if (engine.loadDlcFromAssets(name, selectedBackend)) { showGraphInfos(); appendLog("DLC 加载成功: $name") }
                else appendLog("DLC 加载失败: $name")
                setBusy(false)
            }
        }
        b.btnExecute.setOnClickListener {
            if (engine.graphInfos.isEmpty()) { appendLog("请先加载 DLC 模型"); return@setOnClickListener }
            val g = engine.graphInfos[b.spinnerGraph.selectedItemPosition.coerceAtMost(engine.graphInfos.size - 1)]
            lifecycleScope.launch {
                setBusy(true)
                val r = engine.execute(g.name, engine.makeRandomInputs(g))
                if (r.isSuccess) appendLog("推理成功: 耗时=%.3f ms, 输出数=%d".format(r.elapsedMs, r.outputSizes.size))
                else appendLog("推理失败: ${r.error}")
                setBusy(false)
            }
        }
    }

    private fun showGraphInfos() {
        val names = engine.graphInfos.map { it.name }.toTypedArray()
        val ad = android.widget.ArrayAdapter(requireContext(), android.R.layout.simple_spinner_item, names)
        ad.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item)
        b.spinnerGraph.adapter = ad
        val sb = StringBuilder("图数量: ${engine.graphInfos.size}\n")
        engine.graphInfos.forEach { g ->
            sb.append("\n图: ${g.name}\n  输入(${g.inputs.size}):\n")
            g.inputs.forEach { sb.append("    - ${it.name}: [${it.dimString()}] ${it.dataType}\n") }
            sb.append("  输出(${g.outputs.size}):\n")
            g.outputs.forEach { sb.append("    - ${it.name}: [${it.dimString()}] ${it.dataType}\n") }
        }
        b.textGraphInfo.text = sb.toString()
    }

    private fun appendLog(m: String) { b.textLog.text = b.textLog.text.toString().let { if (it.isEmpty()) m else "$it\n$m" } }
    private fun setBusy(x: Boolean) {
        b.progressBar.visibility = if (x) View.VISIBLE else View.GONE
        b.btnInit.isEnabled = !x; b.btnLoadDlc.isEnabled = !x; b.btnExecute.isEnabled = !x
    }
    override fun onDestroyView() { engine.close(); _b = null; super.onDestroyView() }
}

package com.breeze.qnn

import android.os.Bundle
import android.text.method.ScrollingMovementMethod
import android.view.View
import androidx.appcompat.app.AppCompatActivity
import androidx.lifecycle.lifecycleScope
import com.breeze.qnn.databinding.ActivityMainBinding
import kotlinx.coroutines.launch

/**
 * QNN Demo 主界面。
 *
 * 流程：
 *   1. 选择后端（CPU/GPU/HTP）
 *   2. 点击「加载 DLC」从 assets 加载模型
 *   3. 查看图与张量元信息
 *   4. 点击「执行推理」生成随机输入并执行一次推理
 *   5. 显示耗时与输出张量大小
 */
class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding
    private lateinit var engine: InferenceEngine
    private var selectedBackend: QnnNative.Backend = QnnNative.Backend.CPU

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        engine = InferenceEngine(this)

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
            lifecycleScope.launch {
                setBusy(true)
                val ok = engine.init()
                appendLog(if (ok) "初始化成功" else "初始化失败")
                setBusy(false)
            }
        }

        // 加载 DLC
        binding.btnLoadDlc.setOnClickListener {
            val assetName = binding.editDlcName.text.toString().trim().ifEmpty { DEFAULT_DLC }
            lifecycleScope.launch {
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
            lifecycleScope.launch {
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

    override fun onDestroy() {
        super.onDestroy()
        engine.close()
    }

    private fun showGraphInfos() {
        val graphs = engine.graphInfos
        val names = graphs.map { it.name }.toTypedArray()
        @Suppress("UNCHECKED_CAST")
        val adapter = android.widget.ArrayAdapter(
            this, android.R.layout.simple_spinner_item, names
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
        runOnUiThread {
            val cur = binding.textLog.text.toString()
            val new = if (cur.isEmpty()) msg else "$cur\n$msg"
            binding.textLog.text = new
        }
    }

    private fun setBusy(busy: Boolean) {
        runOnUiThread {
            binding.progressBar.visibility = if (busy) View.VISIBLE else View.GONE
            binding.btnInit.isEnabled = !busy
            binding.btnLoadDlc.isEnabled = !busy
            binding.btnExecute.isEnabled = !busy
        }
    }

    private companion object {
        // 默认从 assets 加载的 DLC 文件名（由 setup 脚本放入 assets）
        const val DEFAULT_DLC = "model.dlc"
    }
}

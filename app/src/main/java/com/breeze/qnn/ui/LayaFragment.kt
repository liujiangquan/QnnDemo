package com.breeze.qnn.ui

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.AdapterView
import android.widget.ArrayAdapter
import androidx.fragment.app.Fragment
import androidx.lifecycle.lifecycleScope
import com.breeze.qnn.R
import com.breeze.qnn.databinding.FragmentLayaBinding
import com.breeze.qnn.laya.*
import kotlinx.coroutines.launch

class LayaFragment : Fragment() {
    private var _b: FragmentLayaBinding? = null
    private val b get() = _b!!
    private var backend: LayaBackend? = null
    private val qtypes = listOf("choice", "score", "noul")

    override fun onCreateView(i: LayoutInflater, c: ViewGroup?, s: Bundle?): View {
        _b = FragmentLayaBinding.inflate(i, c, false); return b.root
    }

    override fun onViewCreated(v: View, s: Bundle?) {
        b.spinnerQType.adapter = ArrayAdapter(requireContext(),
            android.R.layout.simple_spinner_dropdown_item, qtypes)

        // 预置样例：第 0 项为「自定义」，其后为 LayaSamples.ALL
        val sampleNames = listOf(getString(R.string.laya_sample_custom)) + LayaSamples.NAMES
        b.spinnerSample.adapter = ArrayAdapter(requireContext(),
            android.R.layout.simple_spinner_dropdown_item, sampleNames)
        b.spinnerSample.onItemSelectedListener = object : AdapterView.OnItemSelectedListener {
            override fun onItemSelected(p: AdapterView<*>?, view: View?, pos: Int, id: Long) {
                if (pos == 0) { b.textSampleInput.text = ""; return }
                applySample(LayaSamples.ALL[pos - 1])
            }
            override fun onNothingSelected(p: AdapterView<*>?) {}
        }

        b.btnLayaLoad.setOnClickListener {
            viewLifecycleOwner.lifecycleScope.launch {
                busy(true)
                backend = LayaBackend(requireContext())
                val t0 = System.nanoTime()
                val msg = if (!backend!!.modelReady())
                    "模型未就绪：请先运行 docs/setup_laya.sh 推送 laya.bin + tokenizer.json"
                else if (backend!!.load())
                    "模型已加载 (HTP)，耗时 %.0f ms".format((System.nanoTime() - t0) / 1e6)
                else "加载失败，见 logcat"
                if (_b == null) return@launch
                result(msg)
                busy(false)
            }
        }
        b.btnDecide.setOnClickListener {
            val be = backend
            if (be == null || !be.modelReady()) { result("请先加载模型"); return@setOnClickListener }
            val q = buildQuestion() ?: run { result("请填写问题与选项"); return@setOnClickListener }
            viewLifecycleOwner.lifecycleScope.launch {
                busy(true)
                val msg = try {
                    val t0 = System.nanoTime()
                    val ans = be.decide(b.editState.text.toString(), linkedMapOf("q" to q))["q"]
                    val ms = (System.nanoTime() - t0) / 1e6
                    render(ans) + "\n\n⏱ 决策耗时: %.1f ms".format(ms)
                } catch (e: Exception) { "异常: ${e.message}" }
                if (_b == null) return@launch
                result(msg)
                busy(false)
            }
        }
    }

    /** 把样例填入输入框，并在预览区展示完整输入数据。 */
    private fun applySample(s: LayaSample) {
        b.editState.setText(s.state)
        b.editInstructions.setText(s.instructions)
        b.editOptions.setText(s.optionsText())
        b.spinnerQType.setSelection(qtypes.indexOf(s.qtype).coerceAtLeast(0))
        b.textSampleInput.text = s.describe()
    }

    private fun buildQuestion(): Question? {
        val ins = b.editInstructions.text.toString().trim().ifEmpty { return null }
        val opts = b.editOptions.text.toString().lines().map { it.trim() }.filter { it.isNotEmpty() }
        return when (qtypes[b.spinnerQType.selectedItemPosition]) {
            // "键: 说明" 拆成 key + 描述（与上游 renderOptions 一致：喂给模型的文本不变，
            // 但返回的 choice 只是简短的 key，而不是整行）
            "choice" -> if (opts.isEmpty()) null else ChoiceQuestion(ins, LayaSamples.parseCriteria(opts))
            "score" -> if (opts.isEmpty()) null else ScoreQuestion(ins, opts)
            else -> NoulQuestion(ins)
        }
    }

    private fun render(a: Answer?): String = when (a) {
        is ChoiceAnswer -> "选择: ${a.choice}\n置信度: %.3f\n概率:\n".format(a.confidence) +
            a.probabilities.entries.joinToString("\n") { "  ${it.key}: %.4f".format(it.value) } +
            "\nact: %.3f".format(a.actProbability)
        is ScoreAnswer -> "期望分: %.3f\n置信度: %.3f\n分布:\n".format(a.score, a.confidence) +
            a.probabilities.entries.joinToString("\n") { "  L${it.key}(${a.legend[it.key]}): %.4f".format(it.value) }
        is NoulAnswer -> "P(true): %.4f\nact: %.3f".format(a.noul, a.actProbability)
        null -> "无结果"
    }

    private fun result(t: String) { b.textLayaResult.text = t }
    private fun busy(x: Boolean) {
        b.layaProgress.visibility = if (x) View.VISIBLE else View.GONE
        b.btnDecide.isEnabled = !x; b.btnLayaLoad.isEnabled = !x
    }
    override fun onDestroyView() { backend?.close(); backend = null; _b = null; super.onDestroyView() }
}

package com.breeze.qnn.ui

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.ArrayAdapter
import androidx.fragment.app.Fragment
import androidx.lifecycle.lifecycleScope
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
        b.btnLayaLoad.setOnClickListener {
            viewLifecycleOwner.lifecycleScope.launch {
                busy(true)
                backend = LayaBackend(requireContext())
                val msg = if (!backend!!.modelReady())
                    "模型未就绪：请先运行 docs/setup_laya.sh 推送 laya.bin + tokenizer.json"
                else if (backend!!.load()) "模型已加载 (HTP)" else "加载失败，见 logcat"
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
                    val ans = be.decide(b.editState.text.toString(), linkedMapOf("q" to q))["q"]
                    render(ans)
                } catch (e: Exception) { "异常: ${e.message}" }
                if (_b == null) return@launch
                result(msg)
                busy(false)
            }
        }
    }

    private fun buildQuestion(): Question? {
        val ins = b.editInstructions.text.toString().trim().ifEmpty { return null }
        val opts = b.editOptions.text.toString().lines().map { it.trim() }.filter { it.isNotEmpty() }
        return when (qtypes[b.spinnerQType.selectedItemPosition]) {
            "choice" -> if (opts.isEmpty()) null else
                ChoiceQuestion(ins, LinkedHashMap(opts.associateWith { null as String? }))
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

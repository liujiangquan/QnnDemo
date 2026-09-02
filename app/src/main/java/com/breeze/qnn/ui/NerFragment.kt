package com.breeze.qnn.ui

import android.graphics.Color
import android.os.Bundle
import android.text.Spannable
import android.text.SpannableString
import android.text.style.BackgroundColorSpan
import android.text.style.ForegroundColorSpan
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
import com.breeze.qnn.QnnNative
import com.breeze.qnn.R
import com.breeze.qnn.ner.Entity

/**
 * 中文敏感信息识别（BERT NER on 8845 HTP）。
 *
 * 输入文本 → 高亮标记 PER/LOC/ORG/TIME 与正则命中的手机号/身份证/银行卡/邮箱/车牌，
 * 下方列出实体明细。固定走 HTP（预编译 context binary，单句 ~17ms）。
 */
class NerFragment : Fragment() {

    private val viewModel: NerViewModel by viewModels()
    private lateinit var banner: TextView
    private lateinit var etInput: EditText
    private lateinit var btnRecognize: Button
    private lateinit var tvHighlight: TextView
    private lateinit var rvEntities: RecyclerView
    private lateinit var tvStats: TextView
    private lateinit var adapter: EntityAdapter

    override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?, savedInstanceState: Bundle?): View =
        inflater.inflate(R.layout.fragment_ner, container, false)

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        banner = view.findViewById(R.id.status_banner)
        etInput = view.findViewById(R.id.et_ner_input)
        btnRecognize = view.findViewById(R.id.btn_recognize)
        tvHighlight = view.findViewById(R.id.tv_highlight)
        rvEntities = view.findViewById(R.id.rv_entities)
        tvStats = view.findViewById(R.id.tv_ner_stats)

        adapter = EntityAdapter()
        rvEntities.layoutManager = LinearLayoutManager(requireContext())
        rvEntities.adapter = adapter
        tvHighlight.movementMethod = android.text.method.ScrollingMovementMethod()

        etInput.setText(SAMPLE_TEXT)

        viewModel.state.observe(viewLifecycleOwner) { st ->
            when (st) {
                NerViewModel.State.INIT -> showBanner("初始化中…")
                NerViewModel.State.LOADING -> showBanner("加载模型中（HTP 预编译 ctx，约 0.4s）…")
                NerViewModel.State.MODEL_MISSING -> showBanner(
                    "模型未预置。请跑：\nbash docs/setup_bert_ner.sh"
                )
                NerViewModel.State.READY -> { banner.visibility = View.GONE; btnRecognize.isEnabled = true }
                NerViewModel.State.RUNNING -> btnRecognize.isEnabled = false
                NerViewModel.State.ERROR -> btnRecognize.isEnabled = false
                null -> {}
            }
            if (st != NerViewModel.State.READY) btnRecognize.isEnabled = false
        }

        viewModel.error.observe(viewLifecycleOwner) { msg -> msg?.let { showBanner("错误: $it") } }

        viewModel.progress.observe(viewLifecycleOwner) { p ->
            p?.let { (done, total) -> tvStats.text = "识别中 $done/$total 句…" }
        }

        viewModel.result.observe(viewLifecycleOwner) { r ->
            r ?: return@observe
            adapter.submit(r.entities)
            tvHighlight.text = buildHighlight(etInput.text.toString(), r.entities)
            tvStats.text = buildString {
                append("耗时 ${r.elapsedMs}ms · ${r.sentenceCount} 句 · ${r.entities.size} 个实体")
                if (r.failedSentences > 0) append(" · ${r.failedSentences} 句失败")
                append(" · 后端 ${viewModel.backendChoice} ${viewModel.modelKind}")
            }
        }

        btnRecognize.setOnClickListener {
            viewModel.recognize(etInput.text.toString())
        }

        viewModel.initIfNeeded()
    }

    private fun showBanner(msg: String) {
        banner.text = msg
        banner.visibility = View.VISIBLE
    }

    /** 用 SpannableString 按实体类型上色：背景淡色 + 文字本色。 */
    private fun buildHighlight(text: String, entities: List<Entity>): SpannableString {
        val sp = SpannableString(text)
        for (e in entities) {
            if (e.charStart < 0 || e.charEnd > text.length || e.charStart >= e.charEnd) continue
            val c = EntityColors.of(e.type)
            sp.setSpan(ForegroundColorSpan(c), e.charStart, e.charEnd, Spannable.SPAN_EXCLUSIVE_EXCLUSIVE)
            // 同色 20% 透明度做底色
            val bg = Color.argb(51, Color.red(c), Color.green(c), Color.blue(c))
            sp.setSpan(BackgroundColorSpan(bg), e.charStart, e.charEnd, Spannable.SPAN_EXCLUSIVE_EXCLUSIVE)
        }
        return sp
    }

    private companion object {
        const val SAMPLE_TEXT =
            "马化腾在深圳创办了腾讯公司。李四是北京大学的教授，手机号13812345678。" +
            "钱七于2022年8月前往苏州参加华为的技术峰会，邮箱 qian7@example.com。"
    }
}

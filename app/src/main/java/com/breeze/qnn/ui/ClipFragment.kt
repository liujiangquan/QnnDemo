package com.breeze.qnn.ui

import android.Manifest
import android.content.pm.PackageManager
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.Toast
import androidx.activity.result.contract.ActivityResultContracts
import androidx.core.content.ContextCompat
import androidx.fragment.app.Fragment
import androidx.lifecycle.lifecycleScope
import androidx.recyclerview.widget.LinearLayoutManager
import com.breeze.qnn.ChineseClipModel
import com.breeze.qnn.ClipIndex
import com.breeze.qnn.R
import java.io.File
import java.util.concurrent.Executors
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.asCoroutineDispatcher
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext

/**
 * 搜图标签页：刷新本机图片索引，输入中文查询，按 CLIP 相似度排序返回图片。
 *
 * 所有 CLIP 调用经单线程 [clipDispatcher] 串行（模型句柄非线程安全）。
 */
class ClipFragment : Fragment() {

    private var _binding: com.breeze.qnn.databinding.FragmentClipBinding? = null
    private val binding get() = _binding!!

    private val clipExecutor = Executors.newSingleThreadExecutor { r ->
        Thread(r, "clip-native").apply { isDaemon = true }
    }
    private val clipDispatcher = clipExecutor.asCoroutineDispatcher()

    private var clip: ChineseClipModel? = null
    private var index: ClipIndex? = null
    private var indexed: List<ClipIndex.Entry> = emptyList()
    private lateinit var adapter: ClipResultAdapter

    private val mediaPermission =
        registerForActivityResult(ActivityResultContracts.RequestPermission()) { granted ->
            if (granted) refreshIndex() else toast("需要读取图片权限才能建立索引")
        }

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?, savedInstanceState: Bundle?
    ): View {
        _binding = com.breeze.qnn.databinding.FragmentClipBinding.inflate(inflater, container, false)
        return binding.root
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)
        val idx = ClipIndex(requireContext().applicationContext).also { index = it }
        adapter = ClipResultAdapter(requireContext().applicationContext, idx)
        binding.rvClipResults.layoutManager = LinearLayoutManager(requireContext())
        binding.rvClipResults.adapter = adapter

        binding.btnClipRefresh.setOnClickListener { requestOrRefresh() }
        binding.btnClipSearch.setOnClickListener {
            val q = binding.etClipQuery.text.toString().trim()
            if (q.isEmpty()) toast("请输入查询文字") else search(q)
        }

        viewLifecycleOwner.lifecycleScope.launch {
            indexed = withContext(clipDispatcher) { idx.load() }
            updateStatus()
        }
    }

    private fun requestOrRefresh() {
        val granted = ContextCompat.checkSelfPermission(
            requireContext(), Manifest.permission.READ_MEDIA_IMAGES
        ) == PackageManager.PERMISSION_GRANTED
        if (granted) refreshIndex() else mediaPermission.launch(Manifest.permission.READ_MEDIA_IMAGES)
    }

    /** 确保模型已加载（在 clip 线程上）。 */
    private suspend fun ensureModel(): ChineseClipModel? = withContext(clipDispatcher) {
        clip ?: ChineseClipModel().also { m ->
            val dir = File(requireContext().filesDir, "clip/chineseclip")
            if (m.init(dir)) clip = m else { m.close(); toast("模型未就绪：请先预置 chineseclip") }
        }
        clip
    }

    private fun refreshIndex() {
        binding.btnClipRefresh.isEnabled = false
        binding.tvClipStatus.text = "建立索引中…"
        viewLifecycleOwner.lifecycleScope.launch {
            val m = ensureModel()
            val idx = index
            if (m == null || idx == null) {
                binding.btnClipRefresh.isEnabled = true
                return@launch
            }
            indexed = withContext(clipDispatcher) {
                idx.refresh(
                    embed = { bmp -> m.embedImage(bmp) },
                    onProgress = { done, total ->
                        activity?.runOnUiThread {
                            if (_binding != null) binding.tvClipStatus.text = "索引中 $done / $total"
                        }
                    },
                )
            }
            binding.btnClipRefresh.isEnabled = true
            updateStatus()
        }
    }

    private fun search(query: String) {
        viewLifecycleOwner.lifecycleScope.launch {
            if (indexed.isEmpty()) {
                toast("索引为空，请先刷新索引")
                return@launch
            }
            val m = ensureModel()
            if (m == null) return@launch
            val ranked = withContext(clipDispatcher) {
                val qv = m.embedText(query)
                if (qv == null) return@withContext null
                val scores = m.rank(qv, indexed.map { it.vector })
                if (scores == null) return@withContext null
                indexed.indices
                    .map { ClipResultAdapter.Result(indexed[it].id, scores[it]) }
                    .sortedByDescending { it.probability }
                    .take(TOP_N)
            }
            if (ranked == null) toast("检索失败") else adapter.submit(ranked)
        }
    }

    private fun updateStatus() {
        if (_binding == null) return
        binding.tvClipStatus.text = if (indexed.isEmpty()) "未索引" else "已索引 ${indexed.size} 张"
    }

    private fun toast(msg: String) {
        if (isAdded) Toast.makeText(requireContext(), msg, Toast.LENGTH_SHORT).show()
    }

    override fun onDestroyView() {
        adapter.shutdown()
        _binding = null
        super.onDestroyView()
    }

    override fun onDestroy() {
        // 释放模型句柄与线程，在 clip 线程上执行
        clipExecutor.execute {
            clip?.close()
            clip = null
        }
        clipExecutor.shutdown()
        super.onDestroy()
    }

    companion object {
        private const val TOP_N = 20
    }
}

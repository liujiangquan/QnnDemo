package com.breeze.qnn.ui

import android.Manifest
import android.content.pm.PackageManager
import android.graphics.Bitmap
import android.os.Bundle
import android.util.Log
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.TextView
import android.widget.Toast
import androidx.camera.core.CameraSelector
import androidx.camera.core.ImageAnalysis
import androidx.camera.core.ImageProxy
import androidx.camera.core.Preview
import androidx.camera.lifecycle.ProcessCameraProvider
import androidx.camera.view.PreviewView
import androidx.core.content.ContextCompat
import androidx.fragment.app.Fragment
import androidx.fragment.app.viewModels
import androidx.lifecycle.lifecycleScope
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView
import com.breeze.qnn.R
import com.breeze.qnn.yolo.YoloBackend
import com.breeze.qnn.yolo.YoloPostprocessor
import com.breeze.qnn.yolo.YoloPreprocessor
import com.breeze.qnn.yolo.YoloResult
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import java.util.concurrent.Executors

/**
 * CameraX + YOLO26-pose 实时摄像头人体姿态识别。
 *
 * - 默认 HTP 后端；模型文件 /data/user/0/<pkg>/files/yolo/yolo26n-pose.bin（setup_yolo.sh 推过去）
 * - 摄像头每帧 → letterbox fp32 NCHW → backend.infer → YoloPostprocessor.parse →
 *   YoloOverlayView 画框 / 骨架 / RecyclerView 列表
 *
 * 未授权 / 模型未就绪都给出 Toast，不崩溃。
 */
class YoloFragment : Fragment() {

    private lateinit var backend: YoloBackend
    private lateinit var previewView: PreviewView
    private lateinit var overlay: YoloOverlayView
    private lateinit var tvStats: TextView
    private lateinit var rv: RecyclerView
    private lateinit var adapter: DetectionAdapter

    private var started = false
    private val cameraExecutor = Executors.newSingleThreadExecutor()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        backend = YoloBackend(requireContext())
    }

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?, savedInstanceState: Bundle?,
    ): View = inflater.inflate(R.layout.fragment_yolo, container, false)

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        previewView = view.findViewById(R.id.preview_view)
        overlay = view.findViewById(R.id.overlay_view)
        tvStats = view.findViewById(R.id.tv_stats)
        rv = view.findViewById(R.id.rv_detections)
        adapter = DetectionAdapter()
        rv.layoutManager = LinearLayoutManager(requireContext())
        rv.adapter = adapter

        if (allPermissionsGranted()) startCamera() else requestPermissions(
            arrayOf(Manifest.permission.CAMERA), REQ_CAMERA)

        viewLifecycleOwner.lifecycleScope.launch {
            val ok = backend.loadModel()
            if (!ok) {
                Toast.makeText(requireContext(),
                    "模型未加载（请先跑 docs/setup_yolo.sh 推 fp16 ctx 到设备）",
                    Toast.LENGTH_LONG).show()
                tvStats.text = "模型未加载"
            }
        }
    }

    private fun startCamera() {
        val provider = ProcessCameraProvider.getInstance(requireContext()).get()
        val preview = Preview.Builder().build()
            .also { it.setSurfaceProvider(previewView.surfaceProvider) }
        val analysis = ImageAnalysis.Builder()
            .setBackpressureStrategy(ImageAnalysis.STRATEGY_KEEP_ONLY_LATEST)
            .build()
            .also { ia ->
                ia.setAnalyzer(cameraExecutor) { image -> runInference(image) }
            }
        provider.unbindAll()
        provider.bindToLifecycle(viewLifecycleOwner, CameraSelector.DEFAULT_BACK_CAMERA,
            preview, analysis)
        started = true
    }

    private fun runInference(image: ImageProxy) {
        if (!backend.modelReady()) { image.close(); return }
        val t0 = System.nanoTime()
        val input = YoloPreprocessor.imageToTensor(image.image!!).tensor
        viewLifecycleOwner.lifecycleScope.launch(Dispatchers.Default) {
            val outs = backend.infer(input) ?: run { image.close(); return@launch }
            val dets = YoloPostprocessor.parse(
                outs, YoloBackend.COCO_POSE_80,
                confThr = 0.25f, iouThr = 0.45f,
            )
            val elapsed = (System.nanoTime() - t0) / 1_000_000L
            val result = YoloResult(dets, elapsed, image.width, image.height)
            withContext(Dispatchers.Main) {
                overlay.detections = dets
                adapter.submit(dets)
                tvStats.text = "${result.elapsedMs}ms · ${dets.size} 检测 · " +
                    "${result.personCount} 人 · FPS ${(1000f / result.elapsedMs.coerceAtLeast(1)).toInt()}"
            }
        }.invokeOnCompletion { image.close() }
    }

    private fun allPermissionsGranted() =
        ContextCompat.checkSelfPermission(requireContext(),
            Manifest.permission.CAMERA) == PackageManager.PERMISSION_GRANTED

    override fun onRequestPermissionsResult(
        requestCode: Int, permissions: Array<out String>, grantResults: IntArray,
    ) {
        if (requestCode == REQ_CAMERA && grantResults.isNotEmpty() &&
            grantResults[0] == PackageManager.PERMISSION_GRANTED
        ) {
            startCamera()
        } else {
            Toast.makeText(requireContext(), "需要摄像头权限才能识别姿态", Toast.LENGTH_SHORT).show()
        }
    }

    override fun onDestroyView() {
        cameraExecutor.shutdown()
        backend.close()
        super.onDestroyView()
    }

    companion object {
        private const val REQ_CAMERA = 42
        private const val TAG = "YoloFragment"
    }
}

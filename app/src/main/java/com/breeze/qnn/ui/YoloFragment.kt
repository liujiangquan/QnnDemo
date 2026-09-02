package com.breeze.qnn.ui

import android.Manifest
import android.content.pm.PackageManager
import android.os.Bundle
import android.view.Surface
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.TextView
import android.widget.Toast
import androidx.camera.core.CameraSelector
import androidx.camera.core.ImageAnalysis
import androidx.camera.core.ImageProxy
import androidx.camera.core.Preview
import androidx.camera.core.UseCaseGroup
import androidx.camera.lifecycle.ProcessCameraProvider
import androidx.camera.view.PreviewView
import androidx.core.content.ContextCompat
import androidx.fragment.app.Fragment
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
 * - 摄像头每帧 → ARGB8888 Bitmap → backend.infer(bitmap) → native 做 letterbox+量化+HTP infer
 *   → parseFlat → YoloOverlayView 画框 / 骨架 / RecyclerView 列表
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

        if (allPermissionsGranted()) {
            // viewPort 要等 PreviewView layout 完成才非空（参考 QnnYolo 项目的做法）
            previewView.post { startCamera() }
        } else {
            requestPermissions(arrayOf(Manifest.permission.CAMERA), REQ_CAMERA)
        }

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
        // ViewPort 必须取 PreviewView 自己的（与它实际宽高比一致），硬编码比例会让两条流
        // crop 不一致 → overlay 相对 preview 错位。
        // setOutputImageRotationEnabled 让 CameraX 直接把 buffer 旋到目标朝向，
        // 此后 rotationDegrees 为 0，bitmap 无需再旋。
        val viewPort = previewView.viewPort ?: return
        val rotation = previewView.display?.rotation ?: Surface.ROTATION_0
        val analysis = ImageAnalysis.Builder()
            .setOutputImageRotationEnabled(true)
            .setTargetRotation(rotation)
            .setBackpressureStrategy(ImageAnalysis.STRATEGY_KEEP_ONLY_LATEST)
            .build()
            .also { ia ->
                ia.setAnalyzer(cameraExecutor) { image -> runInference(image) }
            }
        provider.unbindAll()
        provider.bindToLifecycle(viewLifecycleOwner, CameraSelector.DEFAULT_BACK_CAMERA,
            UseCaseGroup.Builder().addUseCase(preview).addUseCase(analysis)
                .setViewPort(viewPort).build())
        started = true
    }

    private fun runInference(image: ImageProxy) {
        if (!backend.modelReady()) { image.close(); return }
        // imageInfo.rotationDegrees：把 sensor 朝向的像素旋到自然朝向，native 才能据此输出
        // 与 PreviewView 展示一致的 bitmap 坐标系，overlay 才不会错位。
        val bmp = YoloPreprocessor.imageToBitmap(image.image!!, image.imageInfo.rotationDegrees)
        viewLifecycleOwner.lifecycleScope.launch(Dispatchers.Default) {
            val t0 = System.nanoTime()
            val flat = backend.infer(bmp) ?: run { image.close(); return@launch }
            val dets = YoloPostprocessor.parseFlat(
                flat, YoloBackend.COCO_POSE_80,
                confThr = 0.25f, iouThr = 0.45f,
            )
            val elapsed = (System.nanoTime() - t0) / 1_000_000L
            val result = YoloResult(dets, elapsed, image.width, image.height)
            withContext(Dispatchers.Main) {
                overlay.setBitmapSize(bmp.width, bmp.height)
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

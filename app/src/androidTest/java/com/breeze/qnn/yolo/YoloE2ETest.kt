package com.breeze.qnn.yolo

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.util.Log
import androidx.test.ext.junit.runners.AndroidJUnit4
import androidx.test.platform.app.InstrumentationRegistry
import com.breeze.qnn.yolo.YoloBackend
import com.breeze.qnn.yolo.YoloPreprocessor
import com.breeze.qnn.yolo.YoloPostprocessor
import kotlinx.coroutines.runBlocking
import kotlinx.coroutines.withTimeout
import org.junit.After
import org.junit.Assert.assertEquals
import org.junit.Assert.assertFalse
import org.junit.Assert.assertTrue
import org.junit.Assume.assumeTrue
import org.junit.Before
import org.junit.Test
import org.junit.runner.RunWith
import kotlin.system.measureTimeMillis

private const val TAG = "YoloE2ETest"

/**
 * YOLO26-pose 静态图端到端测试（真机 HTP）。
 *
 * 需要设备预置 fp16 context binary (`bash docs/setup_yolo.sh`)；未预置时
 * @Before 里 `assumeTrue` 会 skip 所有测试，CI 不 red。
 *
 * 期望：从 assets 读一张单人陈迹图 → letterbox → fp32 NCHW → HTP infer → postprocess
 * → 应至少检出 1 个 person，且其 ≥5 个可见 kpt、keypoints 总数 17。
 */
@RunWith(AndroidJUnit4::class)
class YoloE2ETest {

    private lateinit var backend: YoloBackend
    private lateinit var ctx: android.content.Context

    @Before fun setup() {
        ctx = InstrumentationRegistry.getInstrumentation().targetContext
        backend = YoloBackend(ctx)
        assumeTrue("模型未预置，跳过。请先 bash docs/setup_yolo.sh", backend.modelReady())
        runBlocking {
            assertTrue("loadModel 应返回 true", backend.loadModel())
        }
    }

    @After fun teardown() { backend.close() }

    @Test fun `detect person in static test image`(): Unit = runBlocking {
        withTimeout(90_000L) {
            // HTP 首次 execute 要编译图 + warmup；asset 图 letterbox 在 preprocessor 内部做，
            // 这里不 inSampleSize，由 bitmapToTensor 统一输出 640×640 fp32 NCHW。
            val am = ctx.assets.open("yolo_test.jpg")
            val bmp = BitmapFactory.decodeStream(am) ?: throw AssertionError("加载测试图失败")
            val input = YoloPreprocessor.bitmapToTensor(bmp).tensor
            Log.i(TAG, "input tensor bytes=${input.size} (expect ${YoloBackend.EXPECTED_INPUT_BYTES})")
            val ms = measureTimeMillis {
                val outs = backend.infer(input) ?: throw AssertionError("推理返回 null")
                Log.i(TAG, "output tensors=${outs.size} bytes=${outs.firstOrNull()?.size}")
                val dets = YoloPostprocessor.parse(
                    outs, YoloBackend.COCO_POSE_80, confThr = 0.25f, iouThr = 0.45f,
                )
                Log.i(TAG, "dets=${dets.size} persons=${dets.count { it.clsName == "person" }}")
                assertFalse("应至少检测到 1 个", dets.isEmpty())
                val persons = dets.filter { it.clsName == "person" }
                assertTrue("应至少 1 个 person", persons.isNotEmpty())
                val p = persons.first()
                assertTrue("应有 ≥5 个可见 keypoint（实际 ${p.visibleKeypoints.size}）",
                    p.visibleKeypoints.size >= 5)
                assertEquals(17, p.keypoints.size)
            }
            Log.i(TAG, "PASS: detect person elapsed=${ms}ms")
        }
    }
}

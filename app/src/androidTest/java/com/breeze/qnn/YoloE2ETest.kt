package com.breeze.qnn

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.util.Log
import androidx.test.ext.junit.runners.AndroidJUnit4
import androidx.test.platform.app.InstrumentationRegistry
import com.breeze.qnn.yolo.YoloBackend
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
private const val YOLO_BIN_ASSET = "yolo26n-pose.bin"

/**
 * YOLO26-pose 静态图端到端测试（真机 HTP）。
 *
 * 需要设备预置 fp16 context binary (`bash docs/setup_yolo.sh`)；未预置时
 * @Before 里 `assumeTrue` 会 skip 所有测试，CI 不 red。
 *
 * 期望：从 assets 读一张单人陈迹图 → backend.infer(bitmap) → native 做 letterbox+量化+HTP infer
 * → 返回 flat float[] [count, count×(conf, x1, y1, x2, y2, 17×3 kpt)] → postprocess parseFlat
 * → 应至少检出 1 个 person，且其 ≥5 个可见 kpt、keypoints 总数 17。
 *
 * 类必须放在 `com.breeze.qnn` 顶级包（不要放 `com.breeze.qnn.yolo` 子包），
 * 否则会落入 test APK 的 classes3.dex，AndroidJUnitRunner 加载不到。
 */
@RunWith(AndroidJUnit4::class)
class YoloE2ETest {

    private lateinit var backend: YoloBackend
    private lateinit var ctx: android.content.Context

    @Before fun setup() {
        ctx = InstrumentationRegistry.getInstrumentation().targetContext
        android.util.Log.i(TAG, "targetContext pkg=${ctx.packageName} filesDir=${ctx.filesDir}")
        // connectedDebugAndroidTest 默认 uninstall_after_test=true，filesDir 每次被清空。
        // 这里从 test APK 的 assets 把 bin 拷回 filesDir，使测试不依赖 setup_yolo.sh 预置。
        ensureBinFromAssets()
        backend = YoloBackend(ctx)
        val f = ctx.filesDir.resolve("yolo/yolo26n-pose.bin")
        android.util.Log.i(TAG, "ctxFile path=${f.absolutePath} exists=${f.exists()} len=${f.length()}")
        android.util.Log.i(TAG, "modelReady=${backend.modelReady()}")
        assumeTrue("模型未预置，跳过。请先 bash docs/setup_yolo.sh", backend.modelReady())
        runBlocking {
            assertTrue("loadModel 应返回 true", backend.loadModel())
        }
    }

    /** 把 test APK assets 里的 yolo26n-pose.bin 拷到 filesDir/yolo/（缺才拷）。
     *  test APK assets 只能用 instrumentation context 取（targetContext 看不到 test APK）。 */
    private fun ensureBinFromAssets() {
        val dst = ctx.filesDir.resolve("yolo/yolo26n-pose.bin")
        if (dst.exists() && dst.length() > 3_500_000L) {
            android.util.Log.i(TAG, "bin 已在 ${dst.absolutePath}，跳过拷入")
            return
        }
        dst.parentFile?.mkdirs()
        val testCtx = InstrumentationRegistry.getInstrumentation().context
        testCtx.assets.open(YOLO_BIN_ASSET).use { input ->
            dst.outputStream().use { output -> input.copyTo(output) }
        }
        android.util.Log.i(TAG, "从 test APK assets 恢复 bin 到 ${dst.absolutePath} len=${dst.length()}")
    }

    @After fun teardown() { backend.close() }

    @Test fun `detect person in static test image`(): Unit = runBlocking {
        withTimeout(90_000L) {
            val am = ctx.assets.open("yolo_test.jpg")
            val bmp = BitmapFactory.decodeStream(am) ?: throw AssertionError("加载测试图失败")
            val ms = measureTimeMillis {
                val flat = backend.infer(bmp) ?: throw AssertionError("推理返回 null")
                Log.i(TAG, "flat size=${flat.size}")
                val dets = YoloPostprocessor.parseFlat(
                    flat, YoloBackend.COCO_POSE_80, confThr = 0.25f, iouThr = 0.45f,
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

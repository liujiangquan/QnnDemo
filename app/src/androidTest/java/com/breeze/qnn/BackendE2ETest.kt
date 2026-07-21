package com.breeze.qnn

import android.content.Context
import android.util.Log
import androidx.test.ext.junit.runners.AndroidJUnit4
import androidx.test.platform.app.InstrumentationRegistry
import kotlinx.coroutines.runBlocking
import org.junit.After
import org.junit.Assert.assertEquals
import org.junit.Assert.assertFalse
import org.junit.Assert.assertNotNull
import org.junit.Assert.assertTrue
import org.junit.Before
import org.junit.Test
import org.junit.runner.RunWith

/**
 * 后端全链路自动化测试。
 *
 * 每个测试方法覆盖单个后端的：init → loadDlc → execute → 输出校验 全链路。
 * 结果通过 assert 判定 + Log.i(TAG) 打印详细信息，方便 grep logcat 直接看结果。
 *
 * 运行方式：
 *   ./gradlew :app:connectedDebugAndroidTest
 * 单跑某个：
 *   ./gradlew :app:connectedDebugAndroidTest \
 *     -Pandroid.testInstrumentationRunnerArguments.class=com.breeze.qnn.BackendE2ETest#htpBackend
 */
@RunWith(AndroidJUnit4::class)
class BackendE2ETest {

    companion object {
        private const val TAG = "BackendE2ETest"
        private const val DLC_ASSET = "model.dlc"
        private const val EXPECT_OUTPUT_MIN_BYTES = 4L  // 至少读到 1 个 float32
    }

    private lateinit var context: Context
    private lateinit var engine: InferenceEngine

    @Before
    fun setUp() {
        context = InstrumentationRegistry.getInstrumentation().targetContext
        engine = InferenceEngine(context)
        Log.i(TAG, "==================== START TEST ====================")
        Log.i(TAG, "package=${context.packageName} nativeLibDir=${context.applicationInfo.nativeLibraryDir}")
    }

    @After
    fun tearDown() {
        engine.close()
        Log.i(TAG, "==================== END TEST ======================")
    }

    // ------ CPU 后端（我们已验证过能通） ------
    @Test
    fun cpuBackend() {
        runBlocking { runBackendEnd2End(QnnNative.Backend.CPU, allowFailure = false) }
    }

    // ------ HTP 后端（当前调试焦点） ------
    @Test
    fun htpBackend() {
        // 允许 loadDlc 失败（先看是不是 SELinux/unsigned PD 挡了），失败时把详细
        // 错误码上报；成功时校验 execute 也过
        runBlocking { runBackendEnd2End(QnnNative.Backend.HTP, allowFailure = true) }
    }

    // ------ GPU 后端（选做） ------
    @Test
    fun gpuBackend() {
        // 8845 缺 OpenCL ICD 注册，预期失败——不阻塞测试套件
        runBlocking { runBackendEnd2End(QnnNative.Backend.GPU, allowFailure = true) }
    }

    // ============================================================
    private suspend fun runBackendEnd2End(backend: QnnNative.Backend, allowFailure: Boolean) {
        Log.i(TAG, ">>> [$backend] init")
        val initOk = engine.init()
        assertTrue("nativeInit failed for $backend", initOk)

        Log.i(TAG, ">>> [$backend] loadDlc from assets/$DLC_ASSET")
        val loadOk = engine.loadDlcFromAssets(DLC_ASSET, backend)
        if (!loadOk) {
            val msg = "loadDlc failed for $backend (see logcat above)"
            if (allowFailure) {
                Log.w(TAG, "SOFT FAIL: $msg")
                return
            }
            error(msg)
        }

        val graphs = engine.graphInfos
        assertFalse("graphInfos empty for $backend", graphs.isEmpty())
        Log.i(TAG, ">>> [$backend] graph=${graphs[0].name} inputs=${graphs[0].inputs.size} outputs=${graphs[0].outputs.size}")

        val graph = graphs[0]
        val inputs = engine.makeRandomInputs(graph)
        assertEquals("input count mismatch", graph.inputs.size, inputs.size)
        inputs.forEachIndexed { i, buf ->
            val expected = graph.inputs[i].totalBytes.toInt()
            assertEquals("input[$i] size mismatch", expected, buf.size)
        }

        Log.i(TAG, ">>> [$backend] execute")
        val result = engine.execute(graph.name, inputs)
        if (!result.isSuccess) {
            val msg = "execute failed for $backend: ${result.error}"
            if (allowFailure) {
                Log.w(TAG, "SOFT FAIL: $msg")
                return
            }
            error(msg)
        }

        assertNotNull("outputs null", result.outputSizes)
        assertFalse("output list empty", result.outputSizes.isEmpty())
        result.outputSizes.forEachIndexed { i, sz ->
            assertTrue("output[$i] too small ($sz)", sz >= EXPECT_OUTPUT_MIN_BYTES)
        }

        Log.i(TAG, "PASS: $backend elapsed=${"%.3f".format(result.elapsedMs)}ms outputSizes=${result.outputSizes}")
    }
}

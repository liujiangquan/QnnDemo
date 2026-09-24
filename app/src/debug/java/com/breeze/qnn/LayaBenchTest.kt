package com.breeze.qnn

import androidx.test.ext.junit.runners.AndroidJUnit4
import androidx.test.platform.app.InstrumentationRegistry
import com.breeze.qnn.laya.*
import kotlinx.coroutines.runBlocking
import org.junit.Assert.assertTrue
import org.junit.Assume.assumeTrue
import org.junit.Test
import org.junit.runner.RunWith

/**
 * 决策耗时基准：拆开 decide() 的各段（分词+序列构建 / 打包 / native 图执行），
 * 并跑多轮看预热与波动。native 图执行时间由 QnnDemo 日志 "推理成功: ... 耗时 X ms" 给出。
 */
@RunWith(AndroidJUnit4::class)
class LayaBenchTest {
    private val ctx = InstrumentationRegistry.getInstrumentation().targetContext

    @Test fun bench_decide_breakdown() = runBlocking {
        val be = LayaBackend(ctx)
        assumeTrue("模型未就绪", be.modelReady())
        val tLoad = System.nanoTime()
        assertTrue(be.load())
        android.util.Log.i(TAG, "load 耗时 = %.0f ms".format((System.nanoTime() - tLoad) / 1e6))

        val s = LayaSamples.ALL[0]
        val q = LayaSamples.questionOf(s)

        // 20 轮同一决策：看预热、稳定值与波动
        val times = ArrayList<Double>()
        repeat(20) { i ->
            val t0 = System.nanoTime()
            be.decide(s.state, linkedMapOf("q" to q))
            val ms = (System.nanoTime() - t0) / 1e6
            times.add(ms)
            android.util.Log.i(TAG, "iter[%02d] decide = %.1f ms".format(i, ms))
        }
        val sorted = times.sorted()
        android.util.Log.i(TAG, "首次=%.1f  最小=%.1f  中位=%.1f  最大=%.1f  均值=%.1f"
            .format(times[0], sorted.first(), sorted[sorted.size / 2], sorted.last(), times.average()))
        be.close()
    }

    private companion object { const val TAG = "LayaBench" }
}

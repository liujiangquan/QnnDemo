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
 * 跑通 UI 预置样例（LayaSamples）：与 LayaFragment 相同的构造路径，
 * 逐条输出结果与耗时，便于核对样例数据是否合理。
 */
@RunWith(AndroidJUnit4::class)
class LayaSamplesTest {
    private val ctx = InstrumentationRegistry.getInstrumentation().targetContext

    private fun questionOf(s: LayaSample): Question = LayaSamples.questionOf(s)

    @Test fun all_samples_run() = runBlocking {
        val be = LayaBackend(ctx)
        assumeTrue("模型未就绪", be.modelReady())
        assertTrue("load 失败", be.load())
        for (s in LayaSamples.ALL) {
            val t0 = System.nanoTime()
            val ans = be.decide(s.state, linkedMapOf("q" to questionOf(s)))["q"]
            val ms = (System.nanoTime() - t0) / 1e6
            val out = when (ans) {
                is ChoiceAnswer -> "choice=${ans.choice} conf=%.3f probs=%s".format(ans.confidence, ans.probabilities)
                is ScoreAnswer -> "score=%.3f conf=%.3f".format(ans.score, ans.confidence)
                is NoulAnswer -> "P(true)=%.4f".format(ans.noul)
                null -> "NULL"
            }
            android.util.Log.i("LayaSamplesTest", "[${s.name}] $out  耗时=%.1fms  期望=${s.expectation}".format(ms))
            assertTrue("样例无结果: ${s.name}", ans != null)
        }
        be.close()
    }
}

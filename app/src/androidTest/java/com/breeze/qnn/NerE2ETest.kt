package com.breeze.qnn

import android.util.Log
import androidx.test.ext.junit.runners.AndroidJUnit4
import androidx.test.platform.app.InstrumentationRegistry
import com.breeze.qnn.ner.EntitySource
import com.breeze.qnn.ner.EntityType
import com.breeze.qnn.ner.NerBackend
import kotlinx.coroutines.runBlocking
import kotlinx.coroutines.withTimeout
import org.junit.After
import org.junit.Assert.assertEquals
import org.junit.Assert.assertTrue
import org.junit.Assume.assumeTrue
import org.junit.Before
import org.junit.Test
import org.junit.runner.RunWith
import kotlin.system.measureTimeMillis

private const val TAG = "NerE2ETest"

/**
 * 中文敏感信息识别端到端测试（真机 HTP）。
 *
 * 需要设备预置 fp32 DLC（`bash docs/setup_bert_ner.sh`）；未预置时 @Before 里
 * `assumeTrue` 会 skip 所有测试，CI 不 red。
 *
 * 期望值来自 PC 侧 onnxruntime 跑 ONNX 原模型的输出（见 tools/ner_onnx_reference.py），
 * 已验证 fp32 DLC 在 HTP 上与之余弦 0.99697、实体集合完全一致。
 */
@RunWith(AndroidJUnit4::class)
class NerE2ETest {

    private lateinit var backend: NerBackend

    @Before fun setup() {
        val ctx = InstrumentationRegistry.getInstrumentation().targetContext
        backend = NerBackend(ctx)
        assumeTrue("NerBackend init 失败", backend.init())
        assumeTrue("模型未预置，跳过。请先 bash docs/setup_bert_ner.sh", backend.modelReady())
    }

    @After fun teardown() { backend.close() }

    @Test fun nerLoadModel(): Unit = runBlocking {
        withTimeout(60_000L) {
            val ms = measureTimeMillis {
                assertTrue("loadModel(HTP) 应返回 true", backend.loadModel(QnnNative.Backend.HTP))
            }
            // HTP 首次要在设备上编译图，实测约 3-5s
            assertTrue("loadModel 耗时 ${ms}ms 应 ≤ 30s", ms <= 30_000)
            Log.i(TAG, "PASS: loadModel(HTP) elapsed=${ms}ms")
        }
    }

    @Test fun nerRecognizeEntities(): Unit = runBlocking {
        withTimeout(60_000L) {
            assertTrue(backend.loadModel(QnnNative.Backend.HTP))
            val r = backend.recognize("马化腾在深圳创办了腾讯公司。")

            assertEquals("不应有失败的句子", 0, r.failedSentences)
            val pairs = r.entities.map { it.type to it.text }
            // 与 ONNX 参考输出一致
            assertTrue("应识别出 PER 马化腾，实际 $pairs",
                pairs.contains(EntityType.PER to "马化腾"))
            assertTrue("应识别出 LOC 深圳，实际 $pairs",
                pairs.contains(EntityType.LOC to "深圳"))
            assertTrue("应识别出 ORG 腾讯公司，实际 $pairs",
                pairs.contains(EntityType.ORG to "腾讯公司"))
            Log.i(TAG, "PASS: recognize ${r.elapsedMs}ms entities=$pairs")
        }
    }

    @Test fun nerLatencyPerSentence(): Unit = runBlocking {
        withTimeout(120_000L) {
            assertTrue(backend.loadModel(QnnNative.Backend.HTP))
            // 预热一次，排除首次 execute 的额外开销
            backend.recognize("预热句子。")
            val r = backend.recognize("南开大学位于天津市南开区。钱七于2022年8月前往苏州。")
            assertTrue("应切成 2 句，实际 ${r.sentenceCount}", r.sentenceCount == 2)
            val perSentence = r.elapsedMs / r.sentenceCount
            // 实测 HTP fp32 约 34ms/句，给宽松上限防抖
            assertTrue("单句 ${perSentence}ms 应 ≤ 300ms", perSentence <= 300)
            Log.i(TAG, "PASS: ${r.elapsedMs}ms / ${r.sentenceCount} 句 = ${perSentence}ms/句")
        }
    }

    @Test fun nerRegexPipelineMergesWithModel(): Unit = runBlocking {
        withTimeout(60_000L) {
            assertTrue(backend.loadModel(QnnNative.Backend.HTP))
            val r = backend.recognize("李四的手机号是13812345678，邮箱 li4@example.com。")

            val regexTypes = r.entities.filter { it.source == EntitySource.REGEX }.map { it.type }
            assertTrue("应正则命中手机号，实际 $regexTypes", regexTypes.contains(EntityType.PHONE))
            assertTrue("应正则命中邮箱，实际 $regexTypes", regexTypes.contains(EntityType.EMAIL))
            // 合并结果必须按 charStart 有序且区间不重叠
            val sorted = r.entities.sortedBy { it.charStart }
            assertEquals("结果应已排序", sorted.map { it.charStart }, r.entities.map { it.charStart })
            Log.i(TAG, "PASS: regex+model 合并 ${r.entities.size} 个实体")
        }
    }

    @Test fun nerLongTextOffsetsMapBackToOriginal(): Unit = runBlocking {
        withTimeout(120_000L) {
            assertTrue(backend.loadModel(QnnNative.Backend.HTP))
            val text = "马化腾在深圳创办了腾讯公司。李四是北京大学的教授。" +
                "王五昨天从上海飞往广州。赵六在杭州阿里巴巴工作。"
            val r = backend.recognize(text)

            assertTrue("应切成 4 句，实际 ${r.sentenceCount}", r.sentenceCount == 4)
            assertTrue("应识别出实体", r.entities.isNotEmpty())
            // 每个实体的 charStart/charEnd 必须能切回原文得到 text 字段
            r.entities.forEach { e ->
                assertEquals(
                    "实体 ${e.type} 的 offset 映射错误",
                    e.text,
                    text.substring(e.charStart, e.charEnd),
                )
            }
            Log.i(TAG, "PASS: 长文本 ${r.sentenceCount} 句 ${r.entities.size} 个实体，offset 全部对齐")
        }
    }
}

# Laya Multilingual 决策模型 QNN/HTP 集成 实施计划

> **For agentic workers:** REQUIRED SUB-SKILL: 用 superpowers:subagent-driven-development（推荐）或 superpowers:executing-plans 逐任务实施本计划。步骤用 `- [ ]` 复选框跟踪。

**目标：** 在 `laya-decision-8845` 分支上，把 `laya-multilingual`（mmBERT-base 决策模型）以 HTP fp16 context binary 形式集成进 QnnDemo，新增一个 Laya 页面，输入 state + 问题（choice/score/noul）实时返回校准决策。

**架构：** 复用 main 上的通用 QNN Direct-API runtime（补 `loadContextBinary` + `executeWithOutput`）；Kotlin 侧移植 `receptron/laya` 的序列构造与后处理协议；DJL HuggingFace Tokenizer 加载 256k BPE `tokenizer.json`；引入 BottomNav，原 Demo 变 `BackendFragment`，新增 `LayaFragment`。模型与 tokenizer 由 `setup_laya.sh` 推到 `filesDir/laya/`，不入 APK。

**技术栈：** Kotlin、Android NDK/CMake（C++17）、QAIRT/QNN 2.48 Direct API、onnx-simplifier + qnn-* 转换工具链、`ai.djl.huggingface:tokenizers`、JUnit4、AndroidX Fragment/Material BottomNav。

**协议真值来源：** `receptron/laya`（`src/sequence.ts`、`src/laya.ts`、`export/export_onnx.py`）；multilingual 配置来自 `convaiinnovations/laya-multilingual` 的 `rl_agent_config.json`。

**全局约定（跨任务一致）：**
- 固定形状 batch=1：`input_ids/attention_mask=[1,512]`、`marker_pos/marker_mask=[1,16]`、`qtype=[1]`。
- qtype 编码：`choice=0, score=1, noul=2`；温度恒为 `1.0`（multilingual `temperature_by_options` 为空）。
- 特殊 token 字面量 `<bos>`(cls)/`<eos>`(sep)/`<mask>`/`<pad>`；数值 id 运行时从 tokenizer 解析，不硬编码。
- 输入字节序：小端（ARM）；`input_ids/attention_mask/marker_pos/qtype` 打包为 **int32**，`marker_mask` 打包为 **uint8**（转换后图的真实 dtype 在 T3 用 graph info JSON 校验，若不同则在 T9 按实际 dtype 调整打包）。
- 文件落位：`filesDir/laya/laya.bin`、`filesDir/laya/tokenizer.json`。

---

## 文件结构（新建/修改总览）

**Native（C++/JNI）**
- 覆盖：`app/src/main/cpp/qairt_runtime.h`、`qairt_runtime.cpp`、`qnn_jni.cpp`（从 `yolo-pose-8845` 分支取通用超集，含 `loadContextBinary`+`executeWithOutput`）
- 不改：`app/src/main/cpp/CMakeLists.txt`（已编译上述两个 .cpp）

**Kotlin —— 通用层（修改）**
- `app/src/main/java/com/breeze/qnn/QnnNative.kt`（+2 external 声明）
- `app/src/main/java/com/breeze/qnn/InferenceEngine.kt`（+`loadContextBinary`、`executeWithOutput`）

**Kotlin —— Laya 包（新建 `app/src/main/java/com/breeze/qnn/laya/`）**
- `LayaConfig.kt`（常量）
- `LayaQuestion.kt`（`Question`/`Answer` sealed 类型 + `qtype()`/`renderOptions()`）
- `LayaSequenceBuilder.kt`（序列构造，纯逻辑，可 JVM 单测）
- `LayaPostprocessor.kt`（softmax/argmax/期望分/熵置信度，纯逻辑，可 JVM 单测）
- `LayaTokenizer.kt`（DJL 封装）
- `LayaBackend.kt`（编排：build→pad→executeWithOutput→decode）

**UI（新建/修改）**
- 修改：`app/src/main/java/com/breeze/qnn/MainActivity.kt`（BottomNav 宿主）
- 新建：`app/src/main/java/com/breeze/qnn/ui/BackendFragment.kt`（原 Demo 逻辑搬入）
- 新建：`app/src/main/java/com/breeze/qnn/ui/LayaFragment.kt`
- 新建：`app/src/main/res/layout/fragment_backend.xml`（原 `activity_main.xml` 内容平移）
- 重写：`app/src/main/res/layout/activity_main.xml`（FrameLayout + BottomNav）
- 新建：`app/src/main/res/layout/fragment_laya.xml`、`app/src/main/res/menu/bottom_nav.xml`
- 修改：`app/src/main/res/values/strings.xml`（新增文案）、`app/build.gradle.kts`（依赖）

**测试**
- `app/src/test/java/com/breeze/qnn/laya/LayaSequenceBuilderTest.kt`
- `app/src/test/java/com/breeze/qnn/laya/LayaPostprocessorTest.kt`
- `app/src/androidTest/java/com/breeze/qnn/LayaE2ETest.kt`

**脚本/文档**
- `docs/convert_laya.md`、`docs/setup_laya.sh`、`docs/Laya-使用指南.md`

---

## Task 1: 模型转换 spike —— 产出 HTP fp16 `laya.bin`（风险闸门）

> 主机执行（Windows/WSL，QAIRT 2.48 + Python 3.10 环境）。这是**头号风险**任务：算子不被 HTP 支持则走回退。产物是后续所有 on-device 任务的前提。

**Files:**
- Create: `docs/convert_laya.md`
- 产物（不入 git，>100MB）：`out/laya.bin`、复制 `tokenizer.json`

- [ ] **Step 1: 建立转换环境并固定输入形状 + 简化**

```bash
# 在 QAIRT 2.48 的 python3.10 venv 中
pip install onnx onnxsim onnxruntime
cd /path/to/laya_out
python -m onnxsim /mnt/d/workspace/models/laya/laya_multilingual.onnx laya_fixed.onnx \
  --overwrite-input-shape "input_ids:1,512" "attention_mask:1,512" \
    "marker_pos:1,16" "marker_mask:1,16" "qtype:1"
```
Expected: 生成 `laya_fixed.onnx`；onnxsim 打印的算子表中 `IsNaN` 计数显著下降或归零（固定 mask 下 NaN-guard 被常量折叠）。

- [ ] **Step 2: 记录 onnxruntime 黄金输出（供 T12 E2E 对照）**

用固定 onnx 跑一遍客服工单示例，落盘期望值。示例脚本写入 `docs/convert_laya.md` 附录，核心：
```python
import onnxruntime as ort, numpy as np, json
sess = ort.InferenceSession("laya_fixed.onnx", providers=["CPUExecutionProvider"])
# 用 T5/T6 的同一套序列构造（可先用 python 版 rl_common 或手工构造）得到 feeds
# 断言并打印 logits[0,:k] 与 softmax 概率，保存到 golden_ticket.json
```
Expected: 得到 `department` 各选项概率，argmax 落在 billing/退款类；保存 `golden_ticket.json`（记录 input_ids、marker_pos、qtype 与期望 logits）。

- [ ] **Step 3: ONNX → QNN fp16 → HTP context binary**

```bash
# 1) 转 QNN 模型 (fp16 浮点，不做整型量化，保护校准概率)
qnn-onnx-converter --input_network laya_fixed.onnx \
  --float_bitwidth 16 -o laya.cpp
# 2) 生成 aarch64-android model lib
qnn-model-lib-generator -c laya.cpp -b laya.bin -t aarch64-android -o model_libs
# 3) 生成 HTP V81 context binary（unsigned PD，与本仓 YOLO/NER 一致）
qnn-context-binary-generator \
  --model model_libs/aarch64-android/libLaya.so \
  --backend libQnnHtp.so \
  --binary_file laya \
  --output_dir out \
  --config_file htp_v81.json
```
`htp_v81.json`（HTP V81、unsigned）：
```json
{ "graphs": [{ "graph_names": ["laya"], "fp16_relaxed_precision": 1 }],
  "devices": [{ "soc_model": 69, "dsp_arch": "v81", "use_signed_processdomain": false }] }
```
Expected: 生成 `out/laya.bin`（约 600–650MB）。**若第 1 或第 3 步报不支持算子**（`IsNaN`/`Where`/sliding-attn/RoPE），转 Step 4 回退。

- [ ] **Step 4:（仅当 Step 3 失败）回退：重导出干净图**

```bash
# 拉检查点 + 参考导出脚本（干净图：加性 -1e4 mask，无 IsNaN，内置 act_probs softmax）
huggingface-cli download convaiinnovations/laya-multilingual --local-dir laya_ckpt
git clone https://github.com/receptron/laya laya_ref
python laya_ref/export/export_onnx.py laya_ckpt laya_reexport   # 需 laya_ckpt 内含 rl_common.py
python -m onnxsim laya_reexport/laya.onnx laya_fixed.onnx --overwrite-input-shape <同 Step1>
# 回到 Step 3；注意此图输出名为 act_probs（已 softmax），T9 后处理需相应改为直接取概率
```
Expected: 干净图通过 Step 3；若仍失败，停下来向用户汇报（是否接受 CPU fp32 兜底）。**不得静默降级到 CPU。**

- [ ] **Step 5: 写 `docs/convert_laya.md`**

把 Step 1–4 的命令、`htp_v81.json`、黄金输出脚本、回退说明整理成文档（中文）。

- [ ] **Step 6: Commit**

```bash
git add docs/convert_laya.md
git commit -m "docs(laya): T1 模型转换流水线 (onnxsim + qnn fp16 + HTP ctx bin) 与回退"
```

---

## Task 2: 移植通用 runtime 的 context-binary 加载与取回输出（C++/JNI）

**Files:**
- Overwrite: `app/src/main/cpp/qairt_runtime.h`、`app/src/main/cpp/qairt_runtime.cpp`、`app/src/main/cpp/qnn_jni.cpp`（取自 `yolo-pose-8845` 通用超集）

- [ ] **Step 1: 从 yolo 分支取通用 runtime 三件套**

```bash
git checkout yolo-pose-8845 -- \
  app/src/main/cpp/qairt_runtime.h \
  app/src/main/cpp/qairt_runtime.cpp \
  app/src/main/cpp/qnn_jni.cpp
```
说明：这三文件在 yolo 分支是**通用**代码（`grep -i yolo` 命中的均为 `compose`/`composedGraphs` 子串，无 yolo 专有逻辑），相对 main 仅新增 `loadContextBinary()` 与 `nativeExecuteWithOutput()`，已在 8845 HTP 上验证。

- [ ] **Step 2: 确认新增接口存在**

Run:
```bash
grep -n "loadContextBinary" app/src/main/cpp/qairt_runtime.h
grep -n "nativeLoadContextBinary\|nativeExecuteWithOutput" app/src/main/cpp/qnn_jni.cpp
```
Expected: `qairt_runtime.h` 有 `bool loadContextBinary(const std::string& binPath, BackendType backend);`；`qnn_jni.cpp` 有两个新 JNI 导出函数。

- [ ] **Step 3: Commit**

```bash
git add app/src/main/cpp/qairt_runtime.h app/src/main/cpp/qairt_runtime.cpp app/src/main/cpp/qnn_jni.cpp
git commit -m "feat(laya): T2 通用 runtime 补 loadContextBinary + executeWithOutput (移植自 yolo 分支)"
```

---

## Task 3: Kotlin 通用层对接新 JNI

**Files:**
- Modify: `app/src/main/java/com/breeze/qnn/QnnNative.kt`
- Modify: `app/src/main/java/com/breeze/qnn/InferenceEngine.kt`

- [ ] **Step 1: QnnNative.kt 增加两个 external 声明**

在 `nativeExecute` 声明之后追加：
```kotlin
    /** 加载预编译的 QNN context binary(.bin) 并构建运行时 */
    external fun nativeLoadContextBinary(handle: Long, binPath: String, backend: Int): Boolean

    /**
     * 执行推理并取回实际输出字节（nativeExecute 只返回尺寸）。
     * @return 每个输出张量的 ByteArray；失败返回 null
     */
    external fun nativeExecuteWithOutput(handle: Long, graphName: String, inputs: Array<ByteArray>): Array<ByteArray>?
```

- [ ] **Step 2: InferenceEngine.kt 增加两个 suspend 方法**

在 `loadDlc(...)` 之后追加：
```kotlin
    /** 加载预编译 context binary(.bin) */
    suspend fun loadContextBinary(binPath: String, backend: QnnNative.Backend): Boolean =
        withContext(Dispatchers.IO) {
            if (!initialized) { Log.e(TAG, "请先调用 init()"); return@withContext false }
            loaded = native.nativeLoadContextBinary(handle, binPath, backend.value)
            if (loaded) {
                graphInfos = GraphInfo.listFromJson(native.nativeGetGraphInfoJson(handle))
                Log.i(TAG, "context binary 加载成功, 图数量=${graphInfos.size}, backend=$backend")
            } else Log.e(TAG, "context binary 加载失败: $binPath")
            return@withContext loaded
        }

    /**
     * 执行推理并取回实际输出字节。[execute] 只返回尺寸。
     * @return 每个输出张量字节数组；未加载或 native 报错返回 null
     */
    suspend fun executeWithOutput(graphName: String, inputs: List<ByteArray>): List<ByteArray>? =
        withContext(Dispatchers.IO) {
            if (!loaded) { Log.e(TAG, "executeWithOutput: 运行时未加载模型"); return@withContext null }
            native.nativeExecuteWithOutput(handle, graphName, inputs.toTypedArray())?.toList()
        }
```

- [ ] **Step 3: 编译验证**

Run: `./gradlew :app:compileDebugKotlin :app:externalNativeBuildDebug`
Expected: BUILD SUCCESSFUL（native 库 `libqnndemo.so` 含新 JNI 符号，Kotlin 编译通过）。

- [ ] **Step 4: Commit**

```bash
git add app/src/main/java/com/breeze/qnn/QnnNative.kt app/src/main/java/com/breeze/qnn/InferenceEngine.kt
git commit -m "feat(laya): T3 Kotlin 层对接 loadContextBinary/executeWithOutput"
```

---

## Task 4: 加入 DJL Tokenizer 依赖并封装

**Files:**
- Modify: `app/build.gradle.kts`
- Create: `app/src/main/java/com/breeze/qnn/laya/LayaTokenizer.kt`

- [ ] **Step 1: build.gradle.kts 增加依赖**

在 `dependencies {` 块内追加：
```kotlin
    // HuggingFace tokenizer（256k BPE），内含 arm64-android 原生库 libtokenizers.so
    implementation("ai.djl.huggingface:tokenizers:0.30.0")
    // 单测协程支持
    testImplementation("org.jetbrains.kotlinx:kotlinx-coroutines-test:1.7.3")
```

- [ ] **Step 2: 验证 arm64-android 原生库确实被打包**

Run:
```bash
./gradlew :app:assembleDebug
unzip -l app/build/outputs/apk/debug/app-debug.apk | grep -i "libtokenizers.so" || \
  echo "缺 arm64 native —— 走回退"
```
Expected: APK 内含 `lib/arm64-v8a/libtokenizers.so`。
**若缺失（回退）：** 改用为 aarch64-android 交叉编译 Rust `tokenizers` crate 生成 `libtokenizers.so` 放入 `jniLibs/arm64-v8a/` + 薄 JNI 包装（另起子任务，接口与 `LayaTokenizer` 一致），本任务其余步骤不变。把结论记入 `docs/Laya-使用指南.md`。

- [ ] **Step 3: 写 `LayaTokenizer.kt`**

```kotlin
package com.breeze.qnn.laya

import ai.djl.huggingface.tokenizers.HuggingFaceTokenizer
import java.nio.file.Path

/** DJL HuggingFace tokenizer 封装：加载 tokenizer.json，提供不加特殊 token 的编码与特殊 token id 解析。 */
class LayaTokenizer(tokenizerJson: Path) : AutoCloseable {
    private val tok = HuggingFaceTokenizer.builder()
        .optTokenizerPath(tokenizerJson)
        .optAddSpecialTokens(false)
        .build()

    /** 文本 -> token id（不加特殊 token） */
    fun encode(text: String): IntArray =
        tok.encode(text, false).ids.map { it.toInt() }.toIntArray()

    /** 特殊 token 字面量 -> id（如 "<bos>"），未知返回 -1 */
    fun tokenToId(token: String): Int {
        val id = tok.getVocabulary()?.getIndex(token) ?: return -1
        return id.toInt()
    }

    override fun close() = tok.close()
}
```

- [ ] **Step 4: 编译验证 + Commit**

Run: `./gradlew :app:compileDebugKotlin`
Expected: BUILD SUCCESSFUL。
```bash
git add app/build.gradle.kts app/src/main/java/com/breeze/qnn/laya/LayaTokenizer.kt
git commit -m "feat(laya): T4 DJL HuggingFace tokenizer 依赖与封装"
```

---

## Task 5: LayaConfig + Question/Answer 类型

**Files:**
- Create: `app/src/main/java/com/breeze/qnn/laya/LayaConfig.kt`
- Create: `app/src/main/java/com/breeze/qnn/laya/LayaQuestion.kt`

- [ ] **Step 1: 写 `LayaConfig.kt`**

```kotlin
package com.breeze.qnn.laya

object LayaConfig {
    const val MAX_LEN = 512        // 固定序列长度（HTP 图输入）
    const val MAX_OPTS = 16        // 固定 option 槽位（marker 维度）
    const val HEAD_MAX_LEN = 256   // 问题头(含 option) token 预算
    const val OPT_TOKEN_CAP = 48   // 单个 option 文本最大 token 数

    const val QTYPE_CHOICE = 0
    const val QTYPE_SCORE = 1
    const val QTYPE_NOUL = 2
    const val TEMPERATURE = 1.0f   // multilingual 无温度缩放

    const val TOK_BOS = "<bos>"    // 充当 cls
    const val TOK_EOS = "<eos>"    // 充当 sep
    const val TOK_MASK = "<mask>"
    const val TOK_PAD = "<pad>"

    const val SUBDIR = "laya"
    const val BIN_NAME = "laya.bin"
    const val TOKENIZER_NAME = "tokenizer.json"
}
```

- [ ] **Step 2: 写 `LayaQuestion.kt`（含 qtype/renderOptions 逻辑）**

移植 `sequence.ts::QTYPES/renderOptions` 与 `types.ts`：
```kotlin
package com.breeze.qnn.laya

/** 问题类型（映射 receptron/laya types.ts）。 */
sealed interface Question {
    val instructions: String
    fun qtype(): Int
    /** 按 label 顺序渲染 option 文本；noul 恒为 [false, true]，故 p[1]==P(true)。 */
    fun renderOptions(): List<String>
}

/** criteria: option -> 描述(可空)。用 LinkedHashMap 保序。 */
data class ChoiceQuestion(
    override val instructions: String,
    val criteria: LinkedHashMap<String, String?>,
) : Question {
    override fun qtype() = LayaConfig.QTYPE_CHOICE
    override fun renderOptions() = criteria.entries.map { (k, v) -> if (v != null) "$k: $v" else k }
    val keys: List<String> get() = criteria.keys.toList()
}

/** levels: 由低到高的序数等级描述。 */
data class ScoreQuestion(
    override val instructions: String,
    val levels: List<String>,
) : Question {
    override fun qtype() = LayaConfig.QTYPE_SCORE
    override fun renderOptions() = levels.mapIndexed { i, c -> "level $i: $c" }
}

data class NoulQuestion(
    override val instructions: String,
    val trueDesc: String? = null,
    val falseDesc: String? = null,
) : Question {
    override fun qtype() = LayaConfig.QTYPE_NOUL
    override fun renderOptions() = listOf(
        "false: " + (falseDesc ?: "no, the statement does not hold"),
        "true: " + (trueDesc ?: "yes, the statement holds"),
    )
    fun typeName() = "noul"
}

fun Question.typeName(): String = when (this) {
    is ChoiceQuestion -> "choice"
    is ScoreQuestion -> "score"
    is NoulQuestion -> "noul"
}

/** 回答类型（映射 types.ts）。 */
sealed interface Answer { val actProbability: Float }
data class ChoiceAnswer(
    val choice: String, val probabilities: Map<String, Float>,
    val confidence: Float, override val actProbability: Float,
) : Answer
data class ScoreAnswer(
    val score: Float, val probabilities: Map<Int, Float>, val legend: Map<Int, String>,
    val confidence: Float, override val actProbability: Float,
) : Answer
data class NoulAnswer(val noul: Float, override val actProbability: Float) : Answer
```

- [ ] **Step 3: 编译 + Commit**

Run: `./gradlew :app:compileDebugKotlin`
Expected: BUILD SUCCESSFUL。
```bash
git add app/src/main/java/com/breeze/qnn/laya/LayaConfig.kt app/src/main/java/com/breeze/qnn/laya/LayaQuestion.kt
git commit -m "feat(laya): T5 LayaConfig + Question/Answer 类型"
```

---

## Task 6: LayaSequenceBuilder（TDD）

移植 `sequence.ts::buildSequence`。纯逻辑，用 encode stub 做 JVM 单测。

**Files:**
- Create: `app/src/main/java/com/breeze/qnn/laya/LayaSequenceBuilder.kt`
- Test: `app/src/test/java/com/breeze/qnn/laya/LayaSequenceBuilderTest.kt`

- [ ] **Step 1: 写失败测试**

```kotlin
package com.breeze.qnn.laya

import org.junit.Assert.assertEquals
import org.junit.Assert.assertTrue
import org.junit.Test

class LayaSequenceBuilderTest {
    // stub：每个「词」映射到固定 id；空格分词；未知词用其长度当 id。
    private val encode: (String) -> IntArray = { text ->
        if (text.isEmpty()) IntArray(0)
        else text.trim().split(Regex("\\s+")).filter { it.isNotEmpty() }
            .map { (it.hashCode() and 0x7fff) + 100 }.toIntArray()
    }
    private val bos = 2; private val eos = 1; private val mask = 4

    private fun builder(maxLen: Int = 64, headMaxLen: Int = 48) =
        LayaSequenceBuilder(encode, bos, eos, mask, maxLen, headMaxLen)

    @Test fun `choice 布局：CLS 头 SEP 各option前有MASK SEP state SEP`() {
        val q = ChoiceQuestion("pick one", linkedMapOf("a" to null, "b" to null))
        val built = builder().build("hello world", q)
        assertEquals(bos, built.ids.first())
        assertEquals(eos, built.ids.last())
        // 两个 option → 两个 marker，且 marker 位置处的 token 就是 mask
        assertEquals(2, built.markers.size)
        built.markers.forEach { assertEquals(mask, built.ids[it]) }
        assertEquals(q.renderOptions().size, built.markers.size)
    }

    @Test fun `noul 恒两个 option`() {
        val built = builder().build("s", NoulQuestion("is it true?"))
        assertEquals(2, built.markers.size)
    }

    @Test fun `超长截断到 maxLen`() {
        val longState = (1..500).joinToString(" ") { "w$it" }
        val built = builder(maxLen = 32).build(longState, NoulQuestion("q"))
        assertTrue(built.ids.size <= 32)
        // marker 必须落在截断范围内
        built.markers.forEach { assertTrue(it < 32) }
    }

    @Test fun `scrub 掉用户文本里的 mask 字面量`() {
        val q = ChoiceQuestion("say <mask> now", linkedMapOf("x" to null))
        val built = builder().build("state", q)
        // 头部里除 option 的 marker 外不应因用户文本额外冒出 mask id 于 marker 之外
        assertEquals(1, built.markers.size)
    }
}
```

- [ ] **Step 2: 跑测试确认失败**

Run: `./gradlew :app:testDebugUnitTest --tests "com.breeze.qnn.laya.LayaSequenceBuilderTest"`
Expected: 编译失败/测试失败（`LayaSequenceBuilder` 尚不存在）。

- [ ] **Step 3: 实现 `LayaSequenceBuilder.kt`**

```kotlin
package com.breeze.qnn.laya

/**
 * 移植 receptron/laya sequence.ts::buildSequence。
 * 布局：[bos] <type> question: <ins> [eos] [mask]opt0 [mask]opt1 ... [eos] <state> [eos]
 * @param encode 文本->token（不加特殊 token）
 */
class LayaSequenceBuilder(
    private val encode: (String) -> IntArray,
    private val bosId: Int,
    private val eosId: Int,
    private val maskId: Int,
    private val maxLen: Int = LayaConfig.MAX_LEN,
    private val headMaxLen: Int = LayaConfig.HEAD_MAX_LEN,
) {
    data class Built(val ids: IntArray, val markers: IntArray)

    private fun scrub(s: String) = s.replace(LayaConfig.TOK_MASK, " ")

    fun build(state: String, q: Question): Built {
        val opts = q.renderOptions()
        var headIds = encode("${q.typeName()} question: ${scrub(q.instructions)}").toMutableList()
        // 每个 option = [mask] + encode(" "+opt)[:OPT_TOKEN_CAP]
        var optIds = opts.map { o ->
            (listOf(maskId) + encode(" " + scrub(o)).take(LayaConfig.OPT_TOKEN_CAP))
        }
        fun total(xs: List<List<Int>>) = xs.sumOf { it.size }
        var optBudget = headMaxLen - total(optIds)
        if (optBudget < 16) {
            val per = maxOf(4, (headMaxLen - 16) / maxOf(1, optIds.size))
            optIds = optIds.map { it.take(per) }
            optBudget = headMaxLen - total(optIds)
        }
        headIds = headIds.take(maxOf(8, optBudget)).toMutableList()

        val seq = ArrayList<Int>(maxLen)
        seq.add(bosId); seq.addAll(headIds); seq.add(eosId)
        val markers = ArrayList<Int>(optIds.size)
        for (o in optIds) { markers.add(seq.size); seq.addAll(o) }
        seq.add(eosId)
        val room = maxOf(0, maxLen - seq.size - 1)
        val st = encode(scrub(state)).take(room)
        seq.addAll(st); seq.add(eosId)

        val ids = seq.take(maxLen).toIntArray()
        val kept = markers.filter { it < maxLen }.toIntArray()
        return Built(ids, kept)
    }
}
```

- [ ] **Step 4: 跑测试确认通过**

Run: `./gradlew :app:testDebugUnitTest --tests "com.breeze.qnn.laya.LayaSequenceBuilderTest"`
Expected: PASS（4 个测试全绿）。

- [ ] **Step 5: Commit**

```bash
git add app/src/main/java/com/breeze/qnn/laya/LayaSequenceBuilder.kt app/src/test/java/com/breeze/qnn/laya/LayaSequenceBuilderTest.kt
git commit -m "feat(laya): T6 LayaSequenceBuilder 序列构造 (TDD)"
```

---

## Task 7: LayaPostprocessor（TDD）

移植 `laya.ts` 的后处理：softmax、置信度、choice/score/noul 解码。

**Files:**
- Create: `app/src/main/java/com/breeze/qnn/laya/LayaPostprocessor.kt`
- Test: `app/src/test/java/com/breeze/qnn/laya/LayaPostprocessorTest.kt`

- [ ] **Step 1: 写失败测试**

```kotlin
package com.breeze.qnn.laya

import org.junit.Assert.assertEquals
import org.junit.Assert.assertTrue
import org.junit.Test

class LayaPostprocessorTest {
    private val act = floatArrayOf(0.7f, 0.3f)  // 原始 act_logits，softmax 后取 [0]

    @Test fun `softmax 归一化`() {
        val p = LayaPostprocessor.softmax(floatArrayOf(1f, 1f, 1f))
        assertEquals(1.0f, p.sum(), 1e-5f)
        p.forEach { assertEquals(1f / 3, it, 1e-5f) }
    }

    @Test fun `choice 取 argmax 与概率`() {
        val q = ChoiceQuestion("q", linkedMapOf("a" to null, "b" to null, "c" to null))
        val logits = floatArrayOf(0.1f, 5.0f, 0.2f) // b 最大
        val ans = LayaPostprocessor.decode(q, logits, 3, act) as ChoiceAnswer
        assertEquals("b", ans.choice)
        assertEquals(3, ans.probabilities.size)
        assertTrue(ans.probabilities["b"]!! > 0.9f)
    }

    @Test fun `score 期望值`() {
        val q = ScoreQuestion("q", listOf("low", "mid", "high"))
        // 全部质量在 level 2 -> 期望≈2
        val logits = floatArrayOf(-10f, -10f, 10f)
        val ans = LayaPostprocessor.decode(q, logits, 3, act) as ScoreAnswer
        assertEquals(2.0f, ans.score, 1e-2f)
    }

    @Test fun `noul 取 p1 为 P(true)`() {
        val q = NoulQuestion("q")
        val logits = floatArrayOf(-2f, 2f) // true 侧大
        val ans = LayaPostprocessor.decode(q, logits, 2, act) as NoulAnswer
        assertTrue(ans.noul > 0.9f)
        assertEquals(LayaPostprocessor.softmax(act)[0], ans.actProbability, 1e-5f)
    }

    @Test fun `confidence 均匀分布为 0 单点为 1`() {
        assertEquals(0f, LayaPostprocessor.confidence(floatArrayOf(0.5f, 0.5f)), 1e-4f)
        assertEquals(1f, LayaPostprocessor.confidence(floatArrayOf(1f, 0f)), 1e-3f)
    }
}
```

- [ ] **Step 2: 跑测试确认失败**

Run: `./gradlew :app:testDebugUnitTest --tests "com.breeze.qnn.laya.LayaPostprocessorTest"`
Expected: 失败（`LayaPostprocessor` 不存在）。

- [ ] **Step 3: 实现 `LayaPostprocessor.kt`**

```kotlin
package com.breeze.qnn.laya

import kotlin.math.exp
import kotlin.math.ln
import kotlin.math.max

object LayaPostprocessor {
    fun softmax(z: FloatArray): FloatArray {
        val m = z.maxOrNull() ?: 0f
        val e = FloatArray(z.size) { exp((z[it] - m).toDouble()).toFloat() }
        val s = e.sum().takeIf { it > 0f } ?: 1f
        return FloatArray(z.size) { e[it] / s }
    }

    /** 1 - 归一化熵；单点=1，均匀=0。 */
    fun confidence(p: FloatArray): Float {
        val k = p.size
        if (k < 2) return 1f
        var ent = 0.0
        for (x in p) ent -= x * ln(max(x, 1e-12f).toDouble())
        return (1.0 - ent / ln(k.toDouble())).toFloat()
    }

    /**
     * @param logits 模型 logits[0, :]（长度 = MAX_OPTS）
     * @param k 实际 option 数（只取前 k 个做 softmax）
     * @param actLogits 模型 act_logits[0, :]（长度 2，原始 logits）
     */
    fun decode(q: Question, logits: FloatArray, k: Int, actLogits: FloatArray): Answer {
        val temp = LayaConfig.TEMPERATURE
        val z = FloatArray(k) { logits[it] / temp }
        val p = softmax(z)
        val actProb = softmax(actLogits).getOrElse(0) { 0f }
        return when (q) {
            is ChoiceQuestion -> {
                val keys = q.keys
                val best = p.indices.maxByOrNull { p[it] } ?: 0
                ChoiceAnswer(
                    choice = keys.getOrElse(best) { best.toString() },
                    probabilities = keys.mapIndexed { i, kk -> kk to (p.getOrElse(i) { 0f }) }.toMap(),
                    confidence = confidence(p), actProbability = actProb,
                )
            }
            is ScoreQuestion -> {
                var s = 0f; for (i in p.indices) s += i * p[i]
                ScoreAnswer(
                    score = s,
                    probabilities = p.indices.associateWith { p[it] },
                    legend = q.levels.indices.associateWith { q.levels[it] },
                    confidence = confidence(p), actProbability = actProb,
                )
            }
            is NoulQuestion -> NoulAnswer(noul = p.getOrElse(1) { 0f }, actProbability = actProb)
        }
    }
}
```

- [ ] **Step 4: 跑测试确认通过**

Run: `./gradlew :app:testDebugUnitTest --tests "com.breeze.qnn.laya.LayaPostprocessorTest"`
Expected: PASS（5 个测试全绿）。

- [ ] **Step 5: Commit**

```bash
git add app/src/main/java/com/breeze/qnn/laya/LayaPostprocessor.kt app/src/test/java/com/breeze/qnn/laya/LayaPostprocessorTest.kt
git commit -m "feat(laya): T7 LayaPostprocessor 后处理 (TDD)"
```

---

## Task 8: LayaBackend 编排

把 tokenizer + sequence builder + padding + native 执行 + 后处理串起来。padding 逻辑纯函数抽出，可 JVM 单测。

**Files:**
- Create: `app/src/main/java/com/breeze/qnn/laya/LayaBackend.kt`
- Test: `app/src/test/java/com/breeze/qnn/laya/LayaPackTest.kt`

- [ ] **Step 1: 写 padding 打包的失败测试**

```kotlin
package com.breeze.qnn.laya

import org.junit.Assert.assertEquals
import org.junit.Test
import java.nio.ByteBuffer
import java.nio.ByteOrder

class LayaPackTest {
    @Test fun `int32 小端打包长度正确`() {
        val buf = LayaBackend.packInt32(intArrayOf(1, 2, 3), pad = 0, size = 5)
        assertEquals(5 * 4, buf.size)
        val bb = ByteBuffer.wrap(buf).order(ByteOrder.LITTLE_ENDIAN)
        assertEquals(1, bb.int); assertEquals(2, bb.int); assertEquals(3, bb.int)
        assertEquals(0, bb.int); assertEquals(0, bb.int) // pad
    }

    @Test fun `uint8 mask 打包`() {
        val buf = LayaBackend.packMask(realCount = 2, size = 4)
        assertEquals(byteArrayOf(1, 1, 0, 0).toList(), buf.toList())
    }

    @Test fun `attention mask 前 n 个为 1`() {
        val buf = LayaBackend.packInt32Ones(n = 3, size = 5)
        val bb = ByteBuffer.wrap(buf).order(ByteOrder.LITTLE_ENDIAN)
        val got = IntArray(5) { bb.int }
        assertEquals(listOf(1, 1, 1, 0, 0), got.toList())
    }
}
```

- [ ] **Step 2: 跑测试确认失败**

Run: `./gradlew :app:testDebugUnitTest --tests "com.breeze.qnn.laya.LayaPackTest"`
Expected: 失败（`LayaBackend` 不存在）。

- [ ] **Step 3: 实现 `LayaBackend.kt`**

```kotlin
package com.breeze.qnn.laya

import android.content.Context
import android.util.Log
import com.breeze.qnn.InferenceEngine
import com.breeze.qnn.QnnNative
import java.io.File
import java.nio.ByteBuffer
import java.nio.ByteOrder

/**
 * Laya 决策后端：加载 HTP context binary + tokenizer，对 state+问题做单次前向决策。
 * batch=1，多问题循环多次前向。
 */
class LayaBackend(private val context: Context) : AutoCloseable {
    private val engine = InferenceEngine(context)
    private var tokenizer: LayaTokenizer? = null
    private lateinit var seqBuilder: LayaSequenceBuilder
    private var graphName: String = ""
    private var loaded = false

    private fun dir() = File(context.filesDir, LayaConfig.SUBDIR)
    private fun binFile() = File(dir(), LayaConfig.BIN_NAME)
    private fun tokFile() = File(dir(), LayaConfig.TOKENIZER_NAME)

    fun modelReady(): Boolean =
        binFile().let { it.exists() && it.length() > 100L * 1024 * 1024 } && tokFile().exists()

    /** 初始化 + 加载 HTP 模型 + tokenizer。 */
    suspend fun load(): Boolean {
        if (!modelReady()) { Log.e(TAG, "模型未就绪，请先运行 setup_laya.sh"); return false }
        if (!engine.init()) return false
        if (!engine.loadContextBinary(binFile().absolutePath, QnnNative.Backend.HTP)) return false
        val g = engine.graphInfos.firstOrNull() ?: return false
        graphName = g.name
        val tk = LayaTokenizer(tokFile().toPath())
        tokenizer = tk
        val bos = tk.tokenToId(LayaConfig.TOK_BOS)
        val eos = tk.tokenToId(LayaConfig.TOK_EOS)
        val mask = tk.tokenToId(LayaConfig.TOK_MASK)
        require(bos >= 0 && eos >= 0 && mask >= 0) { "特殊 token 解析失败: bos=$bos eos=$eos mask=$mask" }
        seqBuilder = LayaSequenceBuilder(tk::encode, bos, eos, mask)
        loaded = true
        Log.i(TAG, "Laya 加载成功 graph=$graphName inputs=${g.inputs.map { it.name to it.dataType }}")
        return true
    }

    /** 对一个 state 回答多个问题（保序）。 */
    suspend fun decide(state: String, questions: LinkedHashMap<String, Question>): Map<String, Answer> {
        check(loaded) { "请先 load()" }
        val padId = tokenizer!!.tokenToId(LayaConfig.TOK_PAD).coerceAtLeast(0)
        val order = engine.graphInfos.first().inputs.map { it.name }
        val out = LinkedHashMap<String, Answer>()
        for ((qid, q) in questions) {
            val built = seqBuilder.build(state, q)
            val k = built.markers.size
            // 按名字组织输入，再按图输入顺序拼列表
            val byName = mapOf(
                "input_ids" to packInt32(built.ids, padId, LayaConfig.MAX_LEN),
                "attention_mask" to packInt32Ones(built.ids.size, LayaConfig.MAX_LEN),
                "marker_pos" to packInt32(built.markers, 0, LayaConfig.MAX_OPTS),
                "marker_mask" to packMask(k, LayaConfig.MAX_OPTS),
                "qtype" to packInt32(intArrayOf(q.qtype()), 0, 1),
            )
            val inputs = order.map { byName[it] ?: error("图输入名未知: $it") }
            val res = engine.executeWithOutput(graphName, inputs)
                ?: run { Log.e(TAG, "推理失败: $qid"); continue }
            // 按图输出顺序找 logits / act_logits
            val outNames = engine.graphInfos.first().outputs.map { it.name }
            val logits = toFloats(res[outNames.indexOf("logits").coerceAtLeast(0)])
            val actIdx = outNames.indexOf("act_logits").let { if (it >= 0) it else 1 }
            val act = toFloats(res[actIdx])
            out[qid] = LayaPostprocessor.decode(q, logits, k, act)
        }
        return out
    }

    override fun close() { tokenizer?.close(); engine.close(); loaded = false }

    companion object {
        private const val TAG = "LayaBackend"

        fun packInt32(values: IntArray, pad: Int, size: Int): ByteArray {
            val bb = ByteBuffer.allocate(size * 4).order(ByteOrder.LITTLE_ENDIAN)
            for (i in 0 until size) bb.putInt(if (i < values.size) values[i] else pad)
            return bb.array()
        }
        fun packInt32Ones(n: Int, size: Int): ByteArray {
            val bb = ByteBuffer.allocate(size * 4).order(ByteOrder.LITTLE_ENDIAN)
            for (i in 0 until size) bb.putInt(if (i < n) 1 else 0)
            return bb.array()
        }
        fun packMask(realCount: Int, size: Int): ByteArray =
            ByteArray(size) { if (it < realCount) 1 else 0 }

        fun toFloats(bytes: ByteArray): FloatArray {
            val bb = ByteBuffer.wrap(bytes).order(ByteOrder.LITTLE_ENDIAN)
            return FloatArray(bytes.size / 4) { bb.float }
        }
    }
}
```

- [ ] **Step 4: 跑测试确认通过 + 全量单测**

Run: `./gradlew :app:testDebugUnitTest`
Expected: 所有单测 PASS（含 T6/T7/T8）。

- [ ] **Step 5: Commit**

```bash
git add app/src/main/java/com/breeze/qnn/laya/LayaBackend.kt app/src/test/java/com/breeze/qnn/laya/LayaPackTest.kt
git commit -m "feat(laya): T8 LayaBackend 编排 + 打包 (TDD)"
```

---

## Task 9: setup 脚本

**Files:**
- Create: `docs/setup_laya.sh`

- [ ] **Step 1: 写 `setup_laya.sh`**

```bash
#!/usr/bin/env bash
# 把 laya.bin 与 tokenizer.json 推到 App 私有目录 filesDir/laya/
# 用法：./setup_laya.sh <laya.bin 路径> <tokenizer.json 路径> [applicationId]
set -euo pipefail
BIN="${1:?laya.bin 路径}"
TOK="${2:?tokenizer.json 路径}"
APP_ID="${3:-com.breeze.qnn}"
DST="/data/local/tmp/laya"
FILES_DIR="/data/data/${APP_ID}/files/laya"

adb shell mkdir -p "$DST"
adb push "$BIN" "$DST/laya.bin"
adb push "$TOK" "$DST/tokenizer.json"
# 经由 run-as 拷进 app 私有目录（需 debuggable）
adb shell "run-as $APP_ID mkdir -p files/laya"
adb shell "run-as $APP_ID sh -c 'cat $DST/laya.bin > files/laya/laya.bin'"
adb shell "run-as $APP_ID sh -c 'cat $DST/tokenizer.json > files/laya/tokenizer.json'"
adb shell "run-as $APP_ID sh -c 'ls -l files/laya'"
echo "完成：laya.bin + tokenizer.json 已就绪于 filesDir/laya/"
```

- [ ] **Step 2: 校验语法 + Commit**

Run: `bash -n docs/setup_laya.sh`
Expected: 无输出（语法 OK）。
```bash
chmod +x docs/setup_laya.sh
git add docs/setup_laya.sh
git commit -m "feat(laya): T9 setup_laya.sh 推送模型与 tokenizer"
```

---

## Task 10: 导航重构（BottomNav + BackendFragment）

把现有单屏 Demo 逻辑平移到 `BackendFragment`，`MainActivity` 变 BottomNav 宿主，保持原功能不变。

**Files:**
- Create: `app/src/main/res/layout/fragment_backend.xml`（=原 activity_main.xml 内容，id 不变）
- Create: `app/src/main/res/menu/bottom_nav.xml`
- Rewrite: `app/src/main/res/layout/activity_main.xml`
- Create: `app/src/main/java/com/breeze/qnn/ui/BackendFragment.kt`
- Rewrite: `app/src/main/java/com/breeze/qnn/MainActivity.kt`
- Modify: `app/src/main/res/values/strings.xml`

- [ ] **Step 1: 平移布局**

```bash
git mv app/src/main/res/layout/activity_main.xml app/src/main/res/layout/fragment_backend.xml
```
（`fragment_backend.xml` 内容原样保留，所有 id 不变。）

- [ ] **Step 2: 新建 `bottom_nav.xml`**

```xml
<?xml version="1.0" encoding="utf-8"?>
<menu xmlns:android="http://schemas.android.com/apk/res/android">
    <item android:id="@+id/nav_backend" android:title="@string/nav_backend"
        android:icon="@android:drawable/ic_menu_manage" />
    <item android:id="@+id/nav_laya" android:title="@string/nav_laya"
        android:icon="@android:drawable/ic_menu_help" />
</menu>
```

- [ ] **Step 3: 新建 `activity_main.xml`（宿主）**

```xml
<?xml version="1.0" encoding="utf-8"?>
<LinearLayout xmlns:android="http://schemas.android.com/apk/res/android"
    android:layout_width="match_parent" android:layout_height="match_parent"
    android:orientation="vertical">
    <FrameLayout android:id="@+id/fragmentContainer"
        android:layout_width="match_parent" android:layout_height="0dp"
        android:layout_weight="1" />
    <com.google.android.material.bottomnavigation.BottomNavigationView
        android:id="@+id/bottomNav"
        android:layout_width="match_parent" android:layout_height="wrap_content"
        app:menu="@menu/bottom_nav"
        xmlns:app="http://schemas.android.com/apk/res-auto" />
</LinearLayout>
```

- [ ] **Step 4: 新建 `BackendFragment.kt`（把原 MainActivity 逻辑搬入）**

```kotlin
package com.breeze.qnn.ui

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.fragment.app.Fragment
import androidx.lifecycle.lifecycleScope
import com.breeze.qnn.InferenceEngine
import com.breeze.qnn.QnnNative
import com.breeze.qnn.R
import com.breeze.qnn.databinding.FragmentBackendBinding
import kotlinx.coroutines.launch

/** 原通用 QNN Demo：后端选择 / 加载 DLC / 执行推理（逻辑不变，从 MainActivity 平移）。 */
class BackendFragment : Fragment() {
    private var _b: FragmentBackendBinding? = null
    private val b get() = _b!!
    private lateinit var engine: InferenceEngine
    private var selectedBackend = QnnNative.Backend.CPU

    override fun onCreateView(i: LayoutInflater, c: ViewGroup?, s: Bundle?): View {
        _b = FragmentBackendBinding.inflate(i, c, false); return b.root
    }

    override fun onViewCreated(view: View, s: Bundle?) {
        engine = InferenceEngine(requireContext())
        b.radioGroupBackend.setOnCheckedChangeListener { _, id ->
            selectedBackend = when (id) {
                R.id.radioGpu -> QnnNative.Backend.GPU
                R.id.radioHtp -> QnnNative.Backend.HTP
                else -> QnnNative.Backend.CPU
            }
        }
        b.btnInit.setOnClickListener {
            lifecycleScope.launch { setBusy(true); appendLog(if (engine.init()) "初始化成功" else "初始化失败"); setBusy(false) }
        }
        b.btnLoadDlc.setOnClickListener {
            val name = b.editDlcName.text.toString().trim().ifEmpty { "model.dlc" }
            lifecycleScope.launch {
                setBusy(true)
                if (engine.loadDlcFromAssets(name, selectedBackend)) { showGraphInfos(); appendLog("DLC 加载成功: $name") }
                else appendLog("DLC 加载失败: $name")
                setBusy(false)
            }
        }
        b.btnExecute.setOnClickListener {
            if (engine.graphInfos.isEmpty()) { appendLog("请先加载 DLC 模型"); return@setOnClickListener }
            val g = engine.graphInfos[b.spinnerGraph.selectedItemPosition.coerceAtMost(engine.graphInfos.size - 1)]
            lifecycleScope.launch {
                setBusy(true)
                val r = engine.execute(g.name, engine.makeRandomInputs(g))
                if (r.isSuccess) appendLog("推理成功: 耗时=%.3f ms, 输出数=%d".format(r.elapsedMs, r.outputSizes.size))
                else appendLog("推理失败: ${r.error}")
                setBusy(false)
            }
        }
    }

    private fun showGraphInfos() {
        val names = engine.graphInfos.map { it.name }.toTypedArray()
        val ad = android.widget.ArrayAdapter(requireContext(), android.R.layout.simple_spinner_item, names)
        ad.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item)
        b.spinnerGraph.adapter = ad
        val sb = StringBuilder("图数量: ${engine.graphInfos.size}\n")
        engine.graphInfos.forEach { g ->
            sb.append("\n图: ${g.name}\n  输入(${g.inputs.size}):\n")
            g.inputs.forEach { sb.append("    - ${it.name}: [${it.dimString()}] ${it.dataType}\n") }
            sb.append("  输出(${g.outputs.size}):\n")
            g.outputs.forEach { sb.append("    - ${it.name}: [${it.dimString()}] ${it.dataType}\n") }
        }
        b.textGraphInfo.text = sb.toString()
    }

    private fun appendLog(m: String) { b.textLog.text = b.textLog.text.toString().let { if (it.isEmpty()) m else "$it\n$m" } }
    private fun setBusy(x: Boolean) {
        b.progressBar.visibility = if (x) View.VISIBLE else View.GONE
        b.btnInit.isEnabled = !x; b.btnLoadDlc.isEnabled = !x; b.btnExecute.isEnabled = !x
    }
    override fun onDestroyView() { engine.close(); _b = null; super.onDestroyView() }
}
```

- [ ] **Step 5: 重写 `MainActivity.kt`（BottomNav 宿主）**

```kotlin
package com.breeze.qnn

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import androidx.fragment.app.Fragment
import com.breeze.qnn.databinding.ActivityMainBinding
import com.breeze.qnn.ui.BackendFragment
import com.breeze.qnn.ui.LayaFragment

class MainActivity : AppCompatActivity() {
    private lateinit var binding: ActivityMainBinding
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)
        if (savedInstanceState == null) switchTo(BackendFragment())
        binding.bottomNav.setOnItemSelectedListener {
            when (it.itemId) {
                R.id.nav_backend -> { switchTo(BackendFragment()); true }
                R.id.nav_laya -> { switchTo(LayaFragment()); true }
                else -> false
            }
        }
    }
    private fun switchTo(f: Fragment) {
        supportFragmentManager.beginTransaction().replace(R.id.fragmentContainer, f).commit()
    }
}
```

- [ ] **Step 6: strings.xml 增加导航文案**

在 `<resources>` 内追加：
```xml
    <string name="nav_backend">通用</string>
    <string name="nav_laya">Laya</string>
```

- [ ] **Step 7: 临时占位 LayaFragment 以便编译**

先建最小 `LayaFragment`（T11 再充实），保证本任务可编译：
```kotlin
package com.breeze.qnn.ui
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.TextView
import androidx.fragment.app.Fragment
class LayaFragment : Fragment() {
    override fun onCreateView(i: LayoutInflater, c: ViewGroup?, s: Bundle?): View =
        TextView(requireContext()).apply { text = "Laya (T11 待实现)" }
}
```

- [ ] **Step 8: 编译 + 冒烟**

Run: `./gradlew :app:assembleDebug`
Expected: BUILD SUCCESSFUL。安装后底部两个 tab 可切换，「通用」页功能与原来一致。

- [ ] **Step 9: Commit**

```bash
git add app/src/main/res app/src/main/java/com/breeze/qnn/MainActivity.kt app/src/main/java/com/breeze/qnn/ui/BackendFragment.kt app/src/main/java/com/breeze/qnn/ui/LayaFragment.kt
git commit -m "feat(laya): T10 引入 BottomNav，原 Demo 迁至 BackendFragment"
```

---

## Task 11: LayaFragment 决策界面

**Files:**
- Create: `app/src/main/res/layout/fragment_laya.xml`
- Rewrite: `app/src/main/java/com/breeze/qnn/ui/LayaFragment.kt`
- Modify: `app/src/main/res/values/strings.xml`

- [ ] **Step 1: 写 `fragment_laya.xml`**

一个可滚动表单：state 多行输入、问题类型下拉（choice/score/noul）、instructions 输入、options 多行输入（每行一个）、执行按钮、结果 TextView、进度条。
```xml
<?xml version="1.0" encoding="utf-8"?>
<ScrollView xmlns:android="http://schemas.android.com/apk/res/android"
    android:layout_width="match_parent" android:layout_height="match_parent" android:fillViewport="true">
  <LinearLayout android:layout_width="match_parent" android:layout_height="wrap_content"
      android:orientation="vertical" android:padding="16dp">
    <TextView android:layout_width="wrap_content" android:layout_height="wrap_content"
        android:text="@string/laya_title" android:textSize="20sp" android:textStyle="bold" android:paddingBottom="8dp"/>
    <Button android:id="@+id/btnLayaLoad" android:layout_width="match_parent" android:layout_height="wrap_content" android:text="@string/laya_load"/>
    <EditText android:id="@+id/editState" android:layout_width="match_parent" android:layout_height="wrap_content"
        android:hint="@string/laya_state_hint" android:minLines="3" android:gravity="top" android:inputType="textMultiLine"/>
    <Spinner android:id="@+id/spinnerQType" android:layout_width="match_parent" android:layout_height="wrap_content" android:paddingTop="8dp"/>
    <EditText android:id="@+id/editInstructions" android:layout_width="match_parent" android:layout_height="wrap_content" android:hint="@string/laya_ins_hint"/>
    <EditText android:id="@+id/editOptions" android:layout_width="match_parent" android:layout_height="wrap_content"
        android:hint="@string/laya_opts_hint" android:minLines="2" android:gravity="top" android:inputType="textMultiLine"/>
    <Button android:id="@+id/btnDecide" android:layout_width="match_parent" android:layout_height="wrap_content" android:text="@string/laya_decide"/>
    <ProgressBar android:id="@+id/layaProgress" android:layout_width="wrap_content" android:layout_height="wrap_content"
        android:layout_gravity="center" android:visibility="gone"/>
    <TextView android:id="@+id/textLayaResult" android:layout_width="match_parent" android:layout_height="wrap_content"
        android:paddingTop="12dp" android:textIsSelectable="true" android:fontFamily="monospace"/>
  </LinearLayout>
</ScrollView>
```

- [ ] **Step 2: strings.xml 追加文案**

```xml
    <string name="laya_title">Laya 决策</string>
    <string name="laya_load">加载模型 (HTP)</string>
    <string name="laya_state_hint">输入 state 文本，或粘贴 JSON</string>
    <string name="laya_ins_hint">问题（instructions），如：该工单应由哪个团队处理？</string>
    <string name="laya_opts_hint">choice/score 每行一个选项；noul 可留空</string>
    <string name="laya_decide">决策</string>
</resources>
```
（注意把原 `</resources>` 收尾对齐。）

- [ ] **Step 3: 实现 `LayaFragment.kt`**

```kotlin
package com.breeze.qnn.ui

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.ArrayAdapter
import androidx.fragment.app.Fragment
import androidx.lifecycle.lifecycleScope
import com.breeze.qnn.databinding.FragmentLayaBinding
import com.breeze.qnn.laya.*
import kotlinx.coroutines.launch

class LayaFragment : Fragment() {
    private var _b: FragmentLayaBinding? = null
    private val b get() = _b!!
    private var backend: LayaBackend? = null
    private val qtypes = listOf("choice", "score", "noul")

    override fun onCreateView(i: LayoutInflater, c: ViewGroup?, s: Bundle?): View {
        _b = FragmentLayaBinding.inflate(i, c, false); return b.root
    }

    override fun onViewCreated(v: View, s: Bundle?) {
        b.spinnerQType.adapter = ArrayAdapter(requireContext(),
            android.R.layout.simple_spinner_dropdown_item, qtypes)
        b.btnLayaLoad.setOnClickListener {
            lifecycleScope.launch {
                busy(true)
                backend = LayaBackend(requireContext())
                if (!backend!!.modelReady()) result("模型未就绪：请先运行 docs/setup_laya.sh 推送 laya.bin + tokenizer.json")
                else result(if (backend!!.load()) "模型已加载 (HTP)" else "加载失败，见 logcat")
                busy(false)
            }
        }
        b.btnDecide.setOnClickListener {
            val be = backend
            if (be == null || !be.modelReady()) { result("请先加载模型"); return@setOnClickListener }
            val q = buildQuestion() ?: run { result("请填写问题与选项"); return@setOnClickListener }
            lifecycleScope.launch {
                busy(true)
                try {
                    val ans = be.decide(b.editState.text.toString(), linkedMapOf("q" to q))["q"]
                    result(render(ans))
                } catch (e: Exception) { result("异常: ${e.message}") }
                busy(false)
            }
        }
    }

    private fun buildQuestion(): Question? {
        val ins = b.editInstructions.text.toString().trim().ifEmpty { return null }
        val opts = b.editOptions.text.toString().lines().map { it.trim() }.filter { it.isNotEmpty() }
        return when (qtypes[b.spinnerQType.selectedItemPosition]) {
            "choice" -> if (opts.isEmpty()) null else
                ChoiceQuestion(ins, LinkedHashMap(opts.associateWith { null as String? }))
            "score" -> if (opts.isEmpty()) null else ScoreQuestion(ins, opts)
            else -> NoulQuestion(ins)
        }
    }

    private fun render(a: Answer?): String = when (a) {
        is ChoiceAnswer -> "选择: ${a.choice}\n置信度: %.3f\n概率:\n".format(a.confidence) +
            a.probabilities.entries.joinToString("\n") { "  ${it.key}: %.4f".format(it.value) } +
            "\nact: %.3f".format(a.actProbability)
        is ScoreAnswer -> "期望分: %.3f\n置信度: %.3f\n分布:\n".format(a.score, a.confidence) +
            a.probabilities.entries.joinToString("\n") { "  L${it.key}(${a.legend[it.key]}): %.4f".format(it.value) }
        is NoulAnswer -> "P(true): %.4f\nact: %.3f".format(a.noul, a.actProbability)
        null -> "无结果"
    }

    private fun result(t: String) { b.textLayaResult.text = t }
    private fun busy(x: Boolean) {
        b.layaProgress.visibility = if (x) View.VISIBLE else View.GONE
        b.btnDecide.isEnabled = !x; b.btnLayaLoad.isEnabled = !x
    }
    override fun onDestroyView() { backend?.close(); backend = null; _b = null; super.onDestroyView() }
}
```

- [ ] **Step 4: 编译**

Run: `./gradlew :app:assembleDebug`
Expected: BUILD SUCCESSFUL。

- [ ] **Step 5: Commit**

```bash
git add app/src/main/res app/src/main/java/com/breeze/qnn/ui/LayaFragment.kt
git commit -m "feat(laya): T11 LayaFragment 决策界面"
```

---

## Task 12: 真机 E2E（instrumented）

需真机 8845 + `setup_laya.sh` 已推送模型。

**Files:**
- Create: `app/src/androidTest/java/com/breeze/qnn/LayaE2ETest.kt`

- [ ] **Step 1: 写 E2E 测试**

用 T1/Step2 记录的客服工单黄金输出作对照。
```kotlin
package com.breeze.qnn

import androidx.test.ext.junit.runners.AndroidJUnit4
import androidx.test.platform.app.InstrumentationRegistry
import com.breeze.qnn.laya.*
import kotlinx.coroutines.runBlocking
import org.junit.Assert.assertTrue
import org.junit.Assume.assumeTrue
import org.junit.Test
import org.junit.runner.RunWith

@RunWith(AndroidJUnit4::class)
class LayaE2ETest {
    private val ctx = InstrumentationRegistry.getInstrumentation().targetContext

    @Test fun choice_department_billing() = runBlocking {
        val be = LayaBackend(ctx)
        assumeTrue("模型未就绪，跳过（先跑 setup_laya.sh）", be.modelReady())
        assertTrue("load 失败", be.load())
        val state = """{"subject":"Refund not received","body":"I cancelled my subscription two weeks ago and still have not received my refund. If not resolved I will dispute with my bank."}"""
        val q = ChoiceQuestion(
            "Which team should handle this ticket?",
            linkedMapOf(
                "billing" to "payments, refunds, invoices",
                "support" to "product help and bugs",
                "sales" to "new purchases and upgrades",
            ),
        )
        val ans = be.decide(state, linkedMapOf("department" to q))["department"] as ChoiceAnswer
        android.util.Log.i("LayaE2ETest", "choice=${ans.choice} probs=${ans.probabilities} conf=${ans.confidence}")
        assertTrue("期望 billing，实得 ${ans.choice}", ans.choice == "billing")
        be.close()
    }

    @Test fun noul_and_score_run() = runBlocking {
        val be = LayaBackend(ctx)
        assumeTrue(be.modelReady()); assertTrue(be.load())
        val state = "The customer is furious and threatens to leave."
        val res = be.decide(state, linkedMapOf(
            "urgency" to ScoreQuestion("How urgent?", listOf("not urgent", "somewhat", "urgent", "critical")),
            "churn" to NoulQuestion("Is the customer likely to cancel?"),
        ))
        val score = res["urgency"] as ScoreAnswer
        val noul = res["churn"] as NoulAnswer
        android.util.Log.i("LayaE2ETest", "urgency=${score.score} churn=${noul.noul}")
        assertTrue(score.score in 0f..3f)
        assertTrue(noul.noul in 0f..1f)
        be.close()
    }
}
```

- [ ] **Step 2: 部署模型并跑 E2E**

Run:
```bash
./docs/setup_laya.sh out/laya.bin /path/to/tokenizer.json
./gradlew :app:connectedDebugAndroidTest --tests "com.breeze.qnn.LayaE2ETest"
```
Expected: 两个测试 PASS；logcat 打印 `choice=billing`、合理的 urgency/churn。若模型未推送则 `assumeTrue` 跳过（不算失败）。若 `choice != billing`，与 onnxruntime 黄金值对比排查（fp16 漂移 or 序列构造差异）。

- [ ] **Step 3: Commit**

```bash
git add app/src/androidTest/java/com/breeze/qnn/LayaE2ETest.kt
git commit -m "test(laya): T12 真机 E2E 决策验证"
```

---

## Task 13: 使用指南与收尾

**Files:**
- Create: `docs/Laya-使用指南.md`
- Modify: `README.md`

- [ ] **Step 1: 写 `docs/Laya-使用指南.md`**

覆盖：模型来源与许可（Apache-2.0）、转换步骤（引用 `docs/convert_laya.md` + 回退）、`setup_laya.sh` 用法、App 内 Laya 页操作、三种问题类型示例、DJL tokenizer 原生库结论、HTP fp16 与 onnxruntime 对照的注意事项、已知限制（batch=1、seq512、opts16）。

- [ ] **Step 2: README 增加 Laya 小节**

在功能列表/项目结构处加入 Laya 决策模型条目与指南链接。

- [ ] **Step 3: 全量校验**

Run: `./gradlew :app:testDebugUnitTest :app:assembleDebug`
Expected: 单测全绿 + APK 构建成功。

- [ ] **Step 4: Commit**

```bash
git add docs/Laya-使用指南.md README.md
git commit -m "docs(laya): T13 使用指南与 README 收尾"
```

---

## 自查（spec 覆盖对照）

- 转换流水线（spec §3）→ T1（含回退 §3、风险 §11）。
- Native 扩展（spec §4）→ T2 + T3。
- Tokenizer（spec §5）→ T4（含 arm64 回退）。
- 配置/协议（spec §1.2/§6）→ T5、T6、T7、T8。
- 序列布局/后处理与参考实现一致（spec §6）→ T6、T7 移植自 `sequence.ts`/`laya.ts`。
- UI（spec §7）→ T10（BottomNav + BackendFragment）+ T11（LayaFragment）。
- 测试（spec §8）→ T6/T7/T8 JVM 单测 + T12 真机 E2E（含黄金输出对照 T1/Step2）。
- 文档（spec §9）→ T1（convert_laya.md）、T9（setup 脚本）、T13（使用指南 + README）。
- 风险缓解（spec §11）：HTP 算子（T1 Step3/4 闸门 + 不静默降级）、DJL 原生库（T4 Step2 回退）、fp16 校准漂移（T12 与黄金值对照）、dtype（全局约定 + T3/T8 按 graph info 校验）。

**类型一致性核对**：`Question.qtype()/renderOptions()/typeName()`、`LayaSequenceBuilder.build()→Built(ids,markers)`、`LayaPostprocessor.decode(q,logits,k,actLogits)`、`LayaBackend.pack*`、`InferenceEngine.loadContextBinary/executeWithOutput`、`QnnNative.nativeLoadContextBinary/nativeExecuteWithOutput` 在各任务间签名一致。

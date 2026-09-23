# Laya Multilingual 决策排序模型 — QNN/HTP 集成设计

- **日期**: 2026-09-23
- **分支**: `laya-decision-8845`（基于 `origin/main`）
- **目标设备**: Snapdragon 8845 / Hexagon V81，QAIRT SDK 2.48（host）+ vendor 2.46 HTP 库
- **源模型**: `D:\workspace\models\laya\laya_multilingual.onnx`（用户自行导出）

---

## 1. 背景与模型本质

**Laya** 是开源的 System-1「类型化决策」模型（[convaiinnovations/laya-multilingual](https://huggingface.co/convaiinnovations/laya-multilingual)，Apache-2.0）。`laya-multilingual` 是一个 **mmBERT-base（ModernBERT）编码器**（322M 参数），带类型化决策头，在**单次前向传播**中回答关于一段文本状态（state）的三类问题并返回校准概率，**不做 token 生成**：

- `choice`（多选，选一个）
- `score`（序数评分，返回期望等级）
- `noul`（布尔判断，返回 P(true)）

参考实现：[receptron/laya](https://github.com/receptron/laya)（TS + onnxruntime）与其 `export/export_onnx.py`、`src/sequence.ts`、`src/laya.ts` 是本集成的协议真值来源。

### 1.1 ONNX 接口（实测自用户提供的 onnx）

| 张量 | 方向 | 类型 | 形状（动态） |
|---|---|---|---|
| `input_ids` | in | int64 | [batch, seq] |
| `attention_mask` | in | int64 | [batch, seq] |
| `marker_pos` | in | int64 | [batch, opts] |
| `marker_mask` | in | bool | [batch, opts] |
| `qtype` | in | int64 | [batch] |
| `logits` | out | f32 | [batch, opts] |
| `act_logits` | out | f32 | [batch, 2] |

- 架构：hidden 768、22 层、intermediate 1152（GeGLU）、RoPE、sliding(128)+global（每 3 层一次 global）attention；GELU（Erf）；~3624 节点。
- 用户提供的 onnx 为**较原始的导出**：`act_logits` 未 softmax（参考实现导出的是 `act_probs`）；图中含 `IsNaN`/`And`/`Where` 的 NaN-guard（22 组），来源于 attention mask 处理。协议（序列布局、qtype、后处理）与参考实现一致。

### 1.2 关键配置（`rl_agent_config.json`，multilingual 与英文版不同）

- `max_len = 1024`，`head_max_len = 256`
- **`temperature = [1.0, 1.0, 1.0]`，`temperature_by_options = {}`** → 后处理为**纯 softmax，无温度缩放**（英文版有校准温度，multilingual 没有）。
- **特殊 token 为 Gemma 风格（非 BERT）**：`cls=<bos>`(id 2)、`sep=<eos>`(id 1)、`mask=<mask>`(id 4)、`pad=<pad>`(id 0)、`unk=<unk>`(id 3)。tokenizer 为 BPE，vocab 256000，`tokenizer.json` 约 34MB（HF LFS）。
- `qtype` 编码：`choice=0, score=1, noul=2`。

---

## 2. 设计决策（已与用户确认）

| 决策 | 选择 |
|---|---|
| 交付范围 | **完整端到端 feature**（转换 + native 扩展 + Kotlin backend + UI + E2E 测试 + 文档） |
| 后端/精度 | **仅 HTP fp16**（context binary，约 640MB） |
| Tokenizer | **DJL HuggingFace Tokenizers**（AAR，加载 `tokenizer.json`） |
| 固定形状 | **seq = 512 × opts = 16**，batch = 1（padding+mask 处理更短输入） |
| ONNX 来源 | **先用用户提供的 onnx**（+ onnx-simplifier 折叠 NaN-guard）；HTP 拒绝算子时回退到用参考 `export_onnx.py` 重导出干净图 |
| 导航 | main 上无 BottomNav；**引入 BottomNav，Laya 作为新 item**（现有单屏 Demo 重构为一个 Fragment） |

---

## 3. 模型转换流水线（`docs/convert_laya.md` + `docs/convert_laya.ps1` + `docs/setup_laya.sh`）

主机侧（QAIRT 2.48 + Python 3.10 环境）：

1. **固定形状 + 简化**：`onnxsim laya_multilingual.onnx laya_fixed.onnx --overwrite-input-shape input_ids:1,512 attention_mask:1,512 marker_pos:1,16 marker_mask:1,16 qtype:1`
   - onnx-simplifier 常量折叠可消除大部分 `IsNaN` NaN-guard（这些在固定 mask 下是常量分支）。
2. **转 QNN + fp16**：`qairt-converter --input_network laya_fixed.onnx --float_bitwidth 16 --out_node logits --out_node act_logits -o laya.cpp`（int64 输入由 converter 自动降为 int32，bool→uint8）。
3. **生成 model lib**：`qnn-model-lib-generator -c laya.cpp -b laya.bin -t aarch64-android -o model_libs`
4. **生成 HTP context binary**：`qnn-context-binary-generator --model model_libs/aarch64-android/libLaya.so --backend libQnnHtp.so --binary_file laya --config <htp_v81_unsigned>` → **`laya.bin`**（HTP V81，unsigned PD，与 main 的 HTP 打通方式一致）。

`docs/setup_laya.sh` 将 `laya.bin` 与 `tokenizer.json` `adb push` 到 `filesDir/laya/`（不入 APK；APK 只保留小体积代码与配置）。

**回退路径**（若第 2/4 步 HTP 拒绝 `IsNaN`/sliding-attn/RoPE 相关算子）：从 HF 拉取 `convaiinnovations/laya-multilingual` 检查点 + `rl_common.py`，用参考 `export/export_onnx.py` 重导出——该脚本用加性 `-1e4` mask（无 `IsNaN`）且内置 `act_probs` softmax，图对 HTP 更友好。文档中记录此路径。

**风险登记**：HTP 算子支持是**头号风险**。先做转换 spike（见 Plan T1），失败即启用回退，必要时与用户确认是否接受 CPU fp32 兜底作为正确性基线。

---

## 4. Native 层（扩展通用 `qairt_runtime`，不新增专用 runtime）

main 的通用 QNN Direct-API runtime 已支持任意多输入/输出（`execute()` 按图输入顺序接收 `vector<vector<uint8_t>>`，`InferenceResult.outputs` 已含各输出字节）。仅需补两处，均可从其它分支的实现移植：

1. **`loadContextBinary(binPath, backend)`**（C++）+ `nativeLoadContextBinary`（JNI）+ `QnnNative`/`InferenceEngine` 声明 —— 加载预编译 `.bin`。
2. **`nativeExecuteWithOutput(handle, graphName, inputs) : Array<ByteArray>`**（JNI）—— 把 `InferenceResult.outputs`（logits、act_logits）真正回传到 Kotlin（现有 `nativeExecute` 只回传 size）。

输入按图顺序传：`[input_ids(int32), attention_mask(int32), marker_pos(int32), marker_mask(uint8), qtype(int32)]`（dtype 以转换后图的实际类型为准，加载后用 `nativeGetGraphInfoJson` 校验顺序与类型）。CMakeLists 无需加源文件（复用 `qairt_runtime.cpp`/`qnn_jni.cpp`）。

---

## 5. Tokenizer（DJL HuggingFace Tokenizers）

- 依赖：`ai.djl.huggingface:tokenizers`（AAR，内含 `libtokenizers.so`），`HuggingFaceTokenizer.newInstance(Path(tokenizer.json))`，`encode(text, addSpecialTokens=false)` 取 `getIds()`。
- **待验证**：DJL tokenizers 是否提供 arm64-android 原生库。若不提供，回退方案 = 为 aarch64-android 编译 Rust `tokenizers` crate（DJL 底层同一库）+ 薄 JNI 包装（作为 Plan 中的独立验证任务，先于 Kotlin backend）。
- 特殊 token id 运行时用 `tokenizer.getVocabulary()` / token→id 解析 `<bos>/<eos>/<mask>/<pad>`，不硬编码数值。

---

## 6. Kotlin 层（新包 `com.breeze.qnn.laya`）

- **`LayaConfig`**：`maxLen=512`、`headMaxLen=256`、`maxOpts=16`、`qtypeOf(choice=0,score=1,noul=2)`、`temperature=1.0`。
- **`Question` / `Answer`**（data class，映射 `types.ts`）：
  - `ChoiceQuestion(instructions, criteria: Map<String,String?> | List<String>)`
  - `ScoreQuestion(instructions, levels: List<String>)`
  - `NoulQuestion(instructions, trueDesc?, falseDesc?)`
  - `ChoiceAnswer(choice, probabilities, confidence, actProbability)` / `ScoreAnswer(score, probabilities, legend, confidence, actProbability)` / `NoulAnswer(noul, actProbability)`
- **`LayaSequenceBuilder`**（`sequence.ts::buildSequence` 的 Kotlin 移植，可纯 JVM 单测）：
  - 序列布局：`[<bos>] <type> question: <instructions> [<eos>] [<mask>]opt0 [<mask>]opt1 … [<eos>] <state> [<eos>]`
  - `renderOptions`：choice→`"key: value"`（value 为 null 时仅 `key`）；score→`"level i: desc"`；noul→`["false: …","true: …"]`（恒 2 项，p[1]=P(true)）。
  - option 每项 `[<mask>] + encode(" "+scrub(opt))[:48]`；`headMaxLen=256` 预算逻辑同参考；scrub 掉用户文本里的 `<mask>` 字面量。
  - 返回 `ids` 与各 option 的 `<mask>` 位置 `markers`；state 填充至 `maxLen` 后接 `<eos>`，整体截断到 `maxLen`。
- **`LayaBackend`**（`AutoCloseable`，`suspend`）：
  - `modelReady()`：检查 `filesDir/laya/laya.bin`（>100MB）与 `tokenizer.json`。
  - `load()`：`InferenceEngine.init()` → `loadContextBinary(laya.bin, HTP)` → 建 tokenizer。
  - `decide(state, questions)`：对每个 question 调 `LayaSequenceBuilder` → 右 pad 到 `[1,512]`/`[1,16]`（`input_ids` 用 `<pad>`；`attention_mask` 真实=1；`marker_pos` pad=0；`marker_mask` 真实=1 其余 0）→ `executeWithOutput` → `LayaPostprocessor`。**batch=1，多 question 循环多次前向**。
- **`LayaPostprocessor`**：取 `logits[0, 0:k]` 做 `softmax`（temperature=1）；choice→argmax→key + 每项概率；score→`Σ i·p[i]` 期望 + legend；noul→`p[1]`；`confidence = 1 − 归一化熵`；`actProbability = softmax(act_logits[0,:])[0]`。

---

## 7. UI 层

- **导航重构**：`MainActivity` 引入 `BottomNavigationView`（`res/menu/bottom_nav.xml`），把现有单屏 Demo 逻辑（后端选择/加载 DLC/执行）搬到 **`BackendFragment`**（保持原功能不变），新增 **`LayaFragment`**。两个 item：`nav_backend`、`nav_laya`。`activity_main.xml` 改为 `FrameLayout` 容器 + BottomNav。
- **`LayaFragment`**：
  - 输入：state 文本框；question 构造区（类型选择 choice/score/noul + instructions + 动态 option 列表）。
  - 输出：answer（choice 的选中项 / score 的分值 / noul 的 P(true)）、各 option 概率条、confidence、act 概率。
  - 首次进入检查 `modelReady()`，未就绪则提示运行 `setup_laya.sh`。
  - 预置一个示例（客服工单 department/urgency/churn_risk）便于一键验证。

---

## 8. 测试

- **JVM 单测**（`test/`）：`LayaSequenceBuilder` 对齐参考——给定固定 encode stub，断言序列布局、marker 位置、option 预算截断、scrub 行为；`LayaPostprocessor` 的 softmax/argmax/期望分/熵置信度数值。
- **Instrumented E2E**（`androidTest/`，需真机 8845）：`setup_laya.sh` 就绪后加载 `laya.bin`（HTP），跑客服工单示例，断言 `department` 的 argmax 合理（如 billing/退款类）、各输出维度正确、单次前向耗时打印。参考 onnxruntime 侧结果作为对照（离线记录期望向量）。

---

## 9. 文档

- 本设计 spec + `docs/superpowers/plans/2026-09-23-laya-multilingual-decision-8845-plan.md`
- `docs/Laya-使用指南.md`：转换步骤、`setup_laya.sh` 用法、UI 操作、HTP 回退说明。
- `docs/convert_laya.md`：完整转换命令与回退（重导出）流程。

---

## 10. 交付顺序（概览，详见 plan）

1. 转换 spike：固定形状 + 简化 + HTP fp16 转换，产出 `laya.bin`（**风险闸门**）。
2. Native：移植 `loadContextBinary` + `executeWithOutput`（JNI + Kotlin），最小验证加载与前向。
3. Tokenizer：DJL 集成 + arm64 原生库验证（含回退）。
4. Kotlin：`LayaSequenceBuilder`（+JVM 测试）→ `Question/Answer` → `LayaBackend` → `LayaPostprocessor`。
5. UI：BottomNav 重构 + `BackendFragment` + `LayaFragment`。
6. E2E + 文档。

---

## 11. 风险与缓解

| 风险 | 影响 | 缓解 |
|---|---|---|
| HTP 拒绝 `IsNaN`/sliding-attn/RoPE 算子 | 转换失败 | onnx-simplifier 折叠；回退重导出干净图；必要时 CPU fp32 兜底并与用户确认 |
| DJL tokenizers 无 arm64-android 原生库 | tokenizer 不可用 | 回退：aarch64-android 编译 Rust `tokenizers` + JNI |
| fp16 影响校准概率 | 决策概率漂移 | fp16 已是保守选择；E2E 与 onnxruntime 对照，超阈值再议（如混合精度保留头部） |
| 322M/fp16 ≈640MB 的 HTP 内存/加载 | 加载慢/失败 | context binary 预编译；`useLegacyPackaging` + `noCompress`；只在 `filesDir` 存放 |
| int64/bool 输入在 HTP 的处理 | 类型不匹配 | converter 降 int32/uint8；加载后用 graph info JSON 校验类型并在 Kotlin 侧按实际 dtype 打包 |

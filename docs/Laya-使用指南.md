# Laya 决策模型 · 端到端使用指南

本指南是把 **Laya 决策模型**从训练权重跑到 Android 真机 Demo 的**单一权威流程**。请按顺序阅读——尤其是「前置准备」里两件**本仓尚未完成、必须由你在自己环境完成**的事，否则真机运行会失败：

1. **模型转换（T1）尚未执行**——需在 Windows 侧 QAIRT 2.48 环境转出 `laya.bin`（本开发机 WSL 无 `qnn-*` 工具链，见 §1）。
2. **DJL tokenizer 缺 arm64-android 原生库**——`ai.djl.huggingface:tokenizers:0.30.0` 的 Maven jar 只带桌面（linux/win/osx）原生库，**不含 android/arm64 的 `libtokenizers.so`**，必须自行交叉编译放进 `jniLibs/`（见 §2），否则设备上加载 tokenizer 会崩。

> 已完成的部分：Kotlin 决策链路（序列构建 / 后处理 / 编排 / UI）、JNI context-binary 加载与取回、推送脚本、真机 E2E 用例。构建/测试状态见 §7。

---

## 0. 模型来源与许可

| 项 | 内容 |
|----|------|
| 模型 | `convaiinnovations/laya-multilingual`（HuggingFace） |
| 骨干 | mmBERT-base（约 322M 参数），带 typed decision heads |
| 许可 | Apache-2.0 |
| 协议来源 | 决策协议（choice / score / noul、marker 布局）移植自 `receptron/laya` |

三类问题都是**单次前向、batch=1**：给定一段 `state` 文本 + 一个问题，模型输出 `logits`（各 option 的分数，长度 = `MAX_OPTS=16`）与 `act_logits`（长度 2，用于 P(true) / 动作概率）。

---

## 1. 模型转换 → `laya.bin`（在 Windows QAIRT 2.48 环境执行）

> 本仓的 WSL 开发机**没有** `qnn-onnx-converter` / `qnn-model-lib-generator` / `qnn-context-binary-generator`。以下命令请在装有 **QAIRT SDK 2.48** 的 Windows（或 Linux x86）主机上跑。产物 `laya.bin` 约 **640MB**（HTP fp16）。

### 1.1 导出并固定形状的 ONNX

从 HF 检查点导出 ONNX，然后用 `onnxsim` 把动态维度**固定**成下列形状（HTP 图要求静态形状）：

| 输入 | 形状 | 原始 ONNX dtype | 转换后典型 dtype |
|------|------|------|------|
| `input_ids` | `[1, 512]` | int64 | int32/int64 |
| `attention_mask` | `[1, 512]` | int64 | int32/int64 |
| `marker_pos` | `[1, 16]` | int64 | int32/int64 |
| `marker_mask` | `[1, 16]` | **bool** | uint8/int32 |
| `qtype` | `[1]` | int64 | int32/int64 |

```bash
python -m onnxsim laya.onnx laya_fixed.onnx \
  --overwrite-input-shape \
  input_ids:1,512 attention_mask:1,512 marker_pos:1,16 marker_mask:1,16 qtype:1
```

> **dtype 说明**：`qnn-onnx-converter` 会自行决定各输入的落地 dtype（int64 常被降为 int32，bool 常映射为 uint8）。**App 侧无需与某个固定 dtype 对齐**——`LayaBackend.decide` 在加载后读取图元信息里每个输入的 `elementBytes`，按图声明的实际字节宽度（1/2/4/8）小端打包（见 `LayaBackend.packIntsAs`）。因此上表「转换后 dtype」只要保持整型/布尔即可，具体值由转换工具决定，App 会自动适配。
>
> 输出名需为 `logits`（`[1,16]`）与 `act_logits`（`[1,2]`），App 侧按名字取回（见 `LayaBackend.decide`）。

### 1.2 ONNX → QNN 模型 C++/权重（fp16）

```bash
qnn-onnx-converter \
  --input_network laya_fixed.onnx \
  --float_bitwidth 16 \
  --output_path laya.cpp
```

### 1.3 编译成 aarch64-android 模型库

```bash
qnn-model-lib-generator \
  -c laya.cpp -b laya.bin.raw \
  -t aarch64-android \
  -o model_libs
# 产出 model_libs/aarch64-android/liblaya.so
```

### 1.4 生成 HTP context binary（V81，unsigned PD）

```bash
qnn-context-binary-generator \
  --model model_libs/aarch64-android/liblaya.so \
  --backend libQnnHtp.so \
  --binary_file laya \
  --output_dir out \
  --config_file htp_config.json
# 产出 out/laya.bin（≈640MB）
```

`htp_config.json`（关键项）：目标 **HTP V81**、**unsigned PD**（与本仓 `jniLibs/arm64-v8a/libQnnHtpV81Stub.so` 匹配的 8845 / SD 8 Elite 一档）：

```json
{
  "graphs": [{ "graph_names": ["laya"], "fp16_relaxed_precision": 1 }],
  "devices": [{
    "dsp_arch": "v81",
    "soc_model": "SM8845",
    "pd_session": "unsigned"
  }]
}
```

> **回退方案**：若 HF 直接导出的 ONNX 图不干净（含 converter 不支持的算子 / 动态控制流），用 `receptron/laya` 仓的 `export_onnx.py` 从 HF 检查点**重新导出干净图**，再回到 §1.1。

---

## 2. 交叉编译 arm64-android 的 `libtokenizers.so`（关键前置）

**为什么必须做**：Kotlin 侧用 DJL 的 `HuggingFaceTokenizer` 加载 `tokenizer.json`（见 `LayaTokenizer.kt`）。DJL 底层是 HuggingFace 的 **Rust `tokenizers`** crate，靠 JNI 调 native。`ai.djl.huggingface:tokenizers:0.30.0` 的 jar **只打包了 linux/win/osx 的原生库**，APK 里**没有** `lib/arm64-v8a/libtokenizers.so`——在真机加载 tokenizer 时会因找不到原生库而失败。`app/build.gradle.kts` 的依赖处也标注了这一点。

**做法**：为 `aarch64-linux-android` 交叉编译 HuggingFace `tokenizers` 的 C-API / JNI，产出 `libtokenizers.so`，放到 `app/src/main/jniLibs/arm64-v8a/`。

```bash
# 1) 装 Rust + Android target
rustup target add aarch64-linux-android

# 2) 配置 NDK 交叉编译器（NDK r25c+）
export ANDROID_NDK_HOME=/path/to/ndk/25.2.9519653
TOOLCHAIN=$ANDROID_NDK_HOME/toolchains/llvm/prebuilt/linux-x86_64
export CC_aarch64_linux_android=$TOOLCHAIN/bin/aarch64-linux-android34-clang
export CARGO_TARGET_AARCH64_LINUX_ANDROID_LINKER=$TOOLCHAIN/bin/aarch64-linux-android34-clang

# 3) 取与 DJL 0.30.0 对应版本的 tokenizers 源码，编译 JNI 动态库
#    （DJL 的 native 封装在 huggingface/tokenizers 的 bindings，或 djl 的 tokenizers-jni 子项目）
git clone https://github.com/huggingface/tokenizers && cd tokenizers/bindings/... # 对应 JNI/C-API 子目录
cargo build --release --target aarch64-linux-android

# 4) 产物拷入 jniLibs
cp target/aarch64-linux-android/release/libtokenizers.so \
   <repo>/app/src/main/jniLibs/arm64-v8a/libtokenizers.so
```

> 版本务必与 `tokenizers:0.30.0` 内嵌的 Rust 库版本一致，避免 JNI 符号不匹配。若你能找到**可用的第三方 Android 构建**（已针对 `arm64-v8a` 编好的 `libtokenizers.so`），直接放进 `jniLibs/arm64-v8a/` 亦可。
>
> 校验：`ls app/src/main/jniLibs/arm64-v8a/libtokenizers.so` 存在；打出的 APK 内 `lib/arm64-v8a/` 含该 `.so`。

---

## 3. 模型部署到设备

准备两个文件：

1. `laya.bin`——§1 转换得到（≈640MB）。
2. `tokenizer.json`——从 HF `convaiinnovations/laya-multilingual` 下载（LFS，约 34MB）。

用推送脚本推进 App 私有目录 `filesDir/laya/`（脚本经 `run-as` 拷贝，需 debuggable 包）：

```bash
docs/setup_laya.sh <laya.bin 路径> <tokenizer.json 路径> [applicationId]
# 默认 applicationId = com.breeze.qnn
```

脚本完成后 `filesDir/laya/` 下应有 `laya.bin` + `tokenizer.json`。
（`LayaBackend.modelReady()` 会检查 `laya.bin > 100MB` 且 `tokenizer.json` 存在。）

---

## 4. App 内使用

1. 底部导航切到 **「Laya 决策」** 页。
2. 点 **「加载模型 (HTP)」**——初始化 QAIRT 运行时、加载 `laya.bin`（HTP 后端）、加载 tokenizer。
3. 在 **state** 文本框填情景文本（或粘贴 JSON）。
4. 选**问题类型**：`choice` / `score` / `noul`。
5. 填 **instructions**（问题描述，如「该工单应由哪个团队处理？」）。
6. 填**选项**：
   - `choice`：每行一个候选（可 `键: 说明`）。
   - `score`：每行一个等级，由低到高（自动映射 `level 0..N`）。
   - `noul`：**无需选项**（内部恒为 `[false, true]`，`P[1]` 即 P(true)）。
7. 点 **「决策」**，查看输出：
   - **choice**：选中项 `choice` + 各选项概率 `probabilities` + 置信度 `confidence` + 动作概率 `actProbability`。
   - **score**：期望分 `score`（= Σ i·p[i]）+ 各等级概率 + legend + 置信度。
   - **noul**：`P(true)`（`noul`）+ 动作概率。

> 置信度 = `1 − 归一化熵`：单点分布=1，均匀=0（见 `LayaPostprocessor.confidence`）。

---

## 5. 三类问题示例（客服工单场景）

同一段 `state`，可对不同问题多次决策（batch=1，逐题前向）。

**state（示例）**：
> 「客户连续三次反馈 App 登录后立即闪退，已提交日志，情绪激动，威胁取消年度订阅。」

### 5.1 choice —— 分派团队（department）

- 问题类型：`choice`
- instructions：`该工单应分派给哪个团队？`
- 选项（每行一个）：
  ```
  engineering: 崩溃/技术缺陷
  billing: 计费与订阅
  support: 一般咨询
  ```
- 期望：`choice=engineering`，`probabilities` 里 engineering 最高。

### 5.2 score —— 紧急度（urgency）

- 问题类型：`score`
- instructions：`该工单的紧急程度？`
- 选项（由低到高，每行一个）：
  ```
  可延后处理
  常规
  尽快
  紧急，需立即介入
  ```
- 期望：`score` 偏高（接近 3.0），期望分反映「紧急」。

### 5.3 noul —— 流失风险（churn）

- 问题类型：`noul`
- instructions：`该客户是否存在流失风险？`
- 选项：**留空**
- 期望：`P(true)` 高（客户已威胁取消订阅）。

---

## 6. 已知限制

- **batch=1**：一次前向只处理一个问题；多问题在 `LayaBackend.decide` 内**循环多次前向**。
- **固定形状**：`seq=512`（`MAX_LEN`）、`opts=16`（`MAX_OPTS`）。更长的 state / 更多选项会被**截断**（单 option 文本上限 `OPT_TOKEN_CAP=48` token，问题头预算 `HEAD_MAX_LEN=256`）。
- **HTP fp16**：设备上以 fp16 relaxed 精度推理，校准出的概率**可能与 onnxruntime fp32 有小幅漂移**。真机 E2E（`LayaE2ETest`）会与黄金输出对照，允许一定容差。
- **无温度缩放**：`multilingual` 变体 `TEMPERATURE=1.0`。

---

## 7. 当前构建 / 测试状态（如实）

- **JVM 单测全绿**：`LayaSequenceBuilderTest` / `LayaPostprocessorTest` / `LayaPackTest`（共 12 个用例）+ 示例单测，`:app:testDebugUnitTest` 通过。
- **`:app:assembleDebug` 可构建**（APK 产出）。
- **真机 E2E**：`LayaE2ETest` 已写好并可编译，**待设备 + 模型部署（§1–§3）后运行**。运行参考 `docs/run_e2e_tests.sh`。

验证命令：

```bat
cmd.exe /c "cd /d D:\workspace\AICode\qnn\QnnDemo && gradlew.bat :app:testDebugUnitTest :app:assembleDebug"
```

---

## 8. 相关文件索引

| 关注点 | 文件 |
|--------|------|
| 配置常量（形状/特殊 token） | `app/src/main/java/com/breeze/qnn/laya/LayaConfig.kt` |
| 问题/回答类型 | `app/src/main/java/com/breeze/qnn/laya/LayaQuestion.kt` |
| 序列构建（marker 布局） | `app/src/main/java/com/breeze/qnn/laya/LayaSequenceBuilder.kt` |
| 后处理（softmax/置信度/decode） | `app/src/main/java/com/breeze/qnn/laya/LayaPostprocessor.kt` |
| 编排 + 字节打包 + 取回 | `app/src/main/java/com/breeze/qnn/laya/LayaBackend.kt` |
| tokenizer 封装（DJL） | `app/src/main/java/com/breeze/qnn/laya/LayaTokenizer.kt` |
| 决策界面 | `app/src/main/java/com/breeze/qnn/ui/LayaFragment.kt` |
| 推送脚本 | `docs/setup_laya.sh` |
| 真机 E2E | `app/src/androidTest/java/com/breeze/qnn/LayaE2ETest.kt` |

# Laya 决策模型 · 端到端使用指南

本指南是把 **Laya 决策模型**从训练权重跑到 Android 真机 Demo 的**单一权威流程**。请按顺序阅读：

1. **模型转换（T1）已完成** —— `laya.bin` 已转出（654 MB，HTP V81 fp16，build ID `v2.48.40.260702151143`，图名 `laya_qnn`）。完整流水线、踩坑与复现命令见 **`docs/convert_laya.md`**（§1 只留结论）。**并已把 `jniLibs` 的 HTP 一套统一到 SDK 2.48**，使 ctx binary 的 build ID 与设备端 skel 匹配。
2. **DJL tokenizer 缺 arm64-android 原生库** —— `ai.djl.huggingface:tokenizers:0.30.0` 的 Maven jar 只带桌面（linux/win/osx）原生库，**不含 android/arm64 的 `libtokenizers.so`**，必须自行交叉编译放进 `jniLibs/`（见 §2），否则设备上加载 tokenizer 会崩。**这一件仍未完成。**
3. **输出是 fp16** —— ctx binary 的 `logits` / `act_logits` 是 `FLOAT_16`（2 字节），而 `LayaBackend.toFloats()` 目前按 4 字节读，会解出错误数值。**这一件仍未完成**，见 §1.1 与 `convert_laya.md` §7。

> 已完成：模型转换（T1）、jniLibs HTP 栈统一到 SDK 2.48、Kotlin 决策链路（序列构建 / 后处理 / 编排 / UI）、JNI context-binary 加载与取回、推送脚本、真机 E2E 用例。构建/测试状态见 §7。

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

## 1. 模型转换 → `laya.bin`（已完成）

**完整流水线、踩坑与复现命令见 `docs/convert_laya.md`**，本节只留结论。

> 工具链在 **WSL**（Ubuntu-22.04 + QAIRT **2.48.40.260702**）里跑，**不要在 Windows 上跑**——Windows 版 QAIRT 的 Python 链缺 DLL，`qnn-onnx-converter` 之类都是 `#!/usr/bin/env python3` 脚本，没有原生 exe。
> 目标：HTP **V81** / `soc_id=soc_model=97` / **unsigned PD** / fp16。
> 产物：`D:\workspace\models\laya\out\laya.bin`，**654,363,400 字节**，build ID `v2.48.40.260702151143`，
> **图名是 `laya_qnn`**（由输入 ONNX 文件名生成，**不是** `laya`）。

### 1.1 实际 IO 契约（从 ctx binary 读出）

由 `qnn-context-binary-utility` 从 `laya.bin` 导出（命令见 `convert_laya.md` §5.4）：

| 张量 | 方向 | dtype | 形状 |
|------|------|-------|------|
| `input_ids` | 输入 | INT_32 | `[1,512]` |
| `attention_mask` | 输入 | INT_32 | `[1,512]` |
| `marker_pos` | 输入 | INT_32 | `[1,16]` |
| `marker_mask` | 输入 | **BOOL_8** | `[1,16]` |
| `qtype` | 输入 | INT_32 | `[1]` |
| `logits` | 输出 | **FLOAT_16** | `[1,16]` |
| `act_logits` | 输出 | **FLOAT_16** | `[1,2]` |

> **输入侧 ✓**：`LayaBackend.packIntsAs` 在加载后读图元信息里每个输入的 `elementBytes`，按图声明的实际字节宽度（INT_32→4、BOOL_8→1）小端打包，**无需与某个固定 dtype 对齐**。
>
> **输出侧 ⚠️ 待修**：`logits` / `act_logits` 是 **fp16（2 字节）**，而 `LayaBackend.toFloats()` 目前按 4 字节读（`bb.float`）→ 会解出错误数值。
> 修法：按输出张量的 `elementBytes` 分派 fp16 / fp32 解码。详见 `convert_laya.md` §7。
>
> 输出名保持 `logits`（`[1,16]`）与 `act_logits`（`[1,2]`），App 侧按名字取回（见 `LayaBackend.decide`）。

### 1.2 转换要点（完整命令见 `convert_laya.md` §5 / §8）

- **model lib 用 `-t x86_64-linux-clang`，不是 `aarch64-android`。** ctx-gen 是 **host 进程**，它 `dlopen` 的 model lib 必须是 host 架构；`aarch64-android` 那条路还要 Android NDK 的 `ndk-build`（本机只有 Windows NDK，WSL 里没有），而且那是「在设备端跑 ctx-gen」的路线。**本流程是离线 host prepare，完全不需要 NDK。**
- **ctx-gen 的 `--config_file` 只接受 `backend_extensions` 块** —— `graphs` / `devices` 必须放到被它指向的**第二份** config 里（外层套内层）。直接写在 `--config_file` 里会 `Unknown Key` → Device Creation failure。
- **`graph_names` 必须写 `laya_qnn`**，写 `laya` 会找不到图。
- **不要用 `C:\Qualcomm\AIStack\QAIRT\2.49.40.260810`**：生成出来 build ID 会是 v2.49，与 jniLibs 里的 2.48 库不符 → `contextCreateFromBinary` 报 `0x1388`。

> **已处理的图问题**：HF 导出的 ONNX 里有**唯一一个 bool `Gather`**（HTP 的 Gather 只吃整型 activation，会报 `0x c26`/`3110`）。
> 实测其 index 是 `arange(512)`、元素顺序恒等，等价于 Reshape，已用最小手术删除并验证 logits/act_logits **逐位一致**。
> 注意：**不需要**为图中残留的 `IsNaN`×22 / `Where`×26 / `TopK` / `Erf` 改图 —— HTP 支持 fp16 下的它们
> （`htp_opdef_version_history.html`：`IsNan enabled on FP16`、`Gather (Activation type: FP16)`）。
> 若将来又冒出 HTP 不支持的算子，再考虑整图重导出（见 `convert_laya.md` §3.5）。

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

- **模型转换（T1）已完成**：`laya.bin` 654 MB、build ID `v2.48.40.260702151143`、图名 `laya_qnn`。流水线见 `docs/convert_laya.md`。
- **jniLibs 的 HTP 一套已统一到 SDK 2.48**（`libQnnHtp.so` / `libQnnHtpV81Stub.so` / `libQnnHtpPrepare.so` / `libQnnHtpV81Skel.so` unsigned / `V81CalculatorStub` / `NetRunExtensions`），使 ctx binary 的 build ID 与设备端 skel 匹配（否则 `0x1388`）。
- **JVM 单测全绿**：`LayaSequenceBuilderTest` / `LayaPostprocessorTest` / `LayaPackTest`（共 12 个用例）+ 示例单测。
- **`:app:assembleDebug` 通过**，APK 内已含上述 2.48 HTP 全套库（已核验）。
- **仍未完成 —— 上真机前必须先解决**：
  1. **arm64-android 的 `libtokenizers.so`**（§2）。APK 里目前只有桌面版 `native/*/libtokenizers.*`，**没有** `lib/arm64-v8a/libtokenizers.so`，真机加载 tokenizer 会失败。
  2. **`logits` / `act_logits` 的 fp16 解码**（§1.1）。`LayaBackend.toFloats` 按 4 字节读，而图输出是 `FLOAT_16`（2 字节），会解出错误数值。
- **真机 E2E**：`LayaE2ETest` 已写好并可编译，**待上述两项 + 设备/模型部署（§1–§3）后运行**。运行参考 `docs/run_e2e_tests.sh`。

验证命令（在 Git Bash 或 WSL 里跑）：

```bash
./gradlew :app:testDebugUnitTest :app:assembleDebug
```

> 注意：`cmd.exe /c "... && gradlew.bat ..."` 从 Git Bash 里调用时，`/c`、`/d` 会被 MSYS 当成路径改写，导致命令静默不执行。要么加 `MSYS_NO_PATHCONV=1`，要么直接用上面的 `./gradlew`。

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

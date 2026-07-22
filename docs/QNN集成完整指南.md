# QNN Android 集成完整指南

> 面向 Snapdragon 平台的 Qualcomm Neural Network（QNN）Android 集成手册，附带 QAIRT/QNN 版本鸿沟事故复盘。
>
> 目标读者：需要在生产 Android 设备上让 HTP/DSP 后端跑起来的应用开发。
>
> 项目实测环境：Snapdragon 8845（SM8480XP，Hexagon V81）+ Android 15，vendor firmware 内置 QNN 2.46。

---

## 目录

- [第一部分：核心原理与踩坑复盘](#第一部分核心原理与踩坑复盘)
  - [1.1 Qualcomm AI 栈分层](#11-qualcomm-ai-栈分层)
  - [1.2 FastRPC / stub-skel 通信模型](#12-fastrpc--stub-skel-通信模型)
  - [1.3 Signed PD 与 Unsigned PD](#13-signed-pd-与-unsigned-pd)
  - [1.4 为什么 2.48 跑不通 / 2.46 可以通](#14-为什么-248-跑不通--246-可以通)
- [第二部分：真实使用 QNN 的硬性条件](#第二部分真实使用-qnn-的硬性条件)
  - [2.1 硬件条件](#21-硬件条件)
  - [2.2 OS / 内核条件](#22-os--内核条件)
  - [2.3 应用签名与权限](#23-应用签名与权限)
  - [2.4 需要内置的文件清单](#24-需要内置的文件清单)
  - [2.5 大模型（LLM）额外条件](#25-大模型llm额外条件)
- [第三部分：完整集成指南](#第三部分完整集成指南)
  - [3.1 前置准备](#31-前置准备)
  - [3.2 项目结构](#32-项目结构)
  - [3.3 Gradle 与签名配置](#33-gradle-与签名配置)
  - [3.4 AndroidManifest 配置](#34-androidmanifest-配置)
  - [3.5 jniLibs 库对齐](#35-jnilibs-库对齐)
  - [3.6 Native 代码骨架（QNN Direct API）](#36-native-代码骨架qnn-direct-api)
  - [3.7 模型（DLC）准备](#37-模型dlc准备)
  - [3.8 集成验证清单](#38-集成验证清单)
- [第四部分：故障排查手册](#第四部分故障排查手册)
  - [4.1 错误码字典](#41-错误码字典)
  - [4.2 常见故障模式](#42-常见故障模式)
- [附录 A：SoC / Hexagon 版本速查](#附录-asochexagon-版本速查)
- [附录 B：本项目的 QNN Direct API 参考实现](#附录-b本项目的-qnn-direct-api-参考实现)

---

# 第一部分：核心原理与踩坑复盘

## 1.1 Qualcomm AI 栈分层

Qualcomm 的 AI 推理栈从上到下共 4 层：

```
+-------------------------------------+
|  Application (Kotlin/Java + JNI)    |   业务代码
+-------------------------------------+
|  QAIRT Easy API (SDK 2.28+)          |   可选：Qualcomm 新加的高层封装
|    libQairtSystem.so, libQairtHtp.so |   (2.48 大改，跟老 vendor 不兼容)
+-------------------------------------+
|  QNN Core API                         |   稳定的 C 接口，几乎所有版本 ABI 兼容
|    libQnnSystem.so                   |   host-only, DLC 解析 / composeGraphs
|    libQnnCpu.so                      |   CPU backend（vendor 一般不带）
|    libQnnHtp.so                      |   HTP backend，包 FastRPC stub
|    libQnnHtpV81Stub.so               |   FastRPC 客户端 stub（架构相关）
|    libQnnHtpPrepare.so               |   图编译时的辅助库
+-------------------------------------+
|  FastRPC / DSP driver (kernel)       |   /dev/adsprpc-smd, ADSP_LIBRARY_PATH
+-------------------------------------+
|  Hexagon DSP skel (on DSP)           |   /vendor/lib/rfsa/adsp/*.so
|    libQnnHtpV81Skel.so               |   由 Qualcomm 签名，跟 firmware 绑定
+-------------------------------------+
```

**关键概念**：

- **QAIRT**：Qualcomm 在 SDK 2.28 引入的更高层封装，把 QNN 的多步骤 API 收敛成 SystemBuilder 一站式调用，同时提供 DLC 到 context 的抽象。**vendor firmware 不带 QAIRT**——它是纯 SDK 侧的产物。
- **QNN**：稳定 C 接口，vendor firmware 里刻的就是这一层。分 `libQnnSystem`（host DLC/context 管理）+ 各 backend 库（`libQnnCpu` / `libQnnGpu` / `libQnnHtp`）。
- **HTP / Hexagon**：真正跑推理的加速器。ARM 侧 stub 通过 FastRPC 把请求转发到 DSP 侧 skel。
- **skel**：DSP 侧的共享库（PIL image），必须由 Qualcomm 签名后放到 `/vendor/lib/rfsa/adsp/`，firmware 里的 DSP 内核只信这条路径下签名过的 skel。

## 1.2 FastRPC / stub-skel 通信模型

HTP 推理的每一步都要走 FastRPC 到 DSP：

```
[ARM 用户态]                           [Kernel]                [DSP]
libQnnHtpV81Stub.so
   |
   | remote_handle_open("file:///lib/rfsa/adsp/libQnnHtpV81Skel.so?_modver=1.0")
   V
libcdsprpc.so / libadsprpc.so    ->  /dev/adsprpc-smd  ->  DSP driver ->  fastrpc loader
                                                                              |
                                                                              V
                                                                       load libQnnHtpV81Skel.so
                                                                       检查签名
                                                                       返回 module handle
   handle -->                                                                  |
   remote_handle_invoke(...)                                                   V
                                                                       skel 处理，回结果
```

三个必须匹配的东西：

1. **stub 版本 = skel 版本**（因为 `_modver` handle 名称、RPC method ordinals 都是编译期定的）
2. **skel 签名 = 设备 DSP 私钥（Qualcomm）**（vendor 里的 skel 由 Qualcomm 用 firmware-batch 私钥签的）
3. **ADSP_LIBRARY_PATH 得能找到 skel**（由 fastrpc loader 在 DSP 侧解析）

任何一处对不上，都会在开 session / open module 阶段炸掉。

## 1.3 Signed PD 与 Unsigned PD

DSP 里的每个应用运行在独立的 **Protected Domain (PD)** 里。两种类型：

| PD 类型 | 加载 skel 的位置 | 需要 skel 签名 | 需要 app 权限 |
|---|---|---|---|
| **Signed PD** | `/vendor/lib/rfsa/adsp/` | 必须（Qualcomm 私钥） | app 必须在 `<uses-native-library>` 白名单，且 shell 或 `system` UID |
| **Unsigned PD** | app-writable path（如 `/data/user/0/<pkg>/files/htp/`） | 不需要 | 生产设备通常在内核 disable（`fastrpc.disable_unsigned_pd=1`）|

在 8845 这台机器上：

- Signed PD **可用**，但只对 platform-signed `android.uid.system` app 或 shell 开放
- Unsigned PD **在内核禁用**（`err 4000 loadRemoteSymbols failed`）
- 结果：**必须走 signed PD + 平台签名 priv-app 路线**

## 1.4 为什么 2.48 跑不通 / 2.46 可以通

事故起因：项目最初用 Qualcomm QAIRT SDK 2.48.40，通过 `<uses-native-library>` 把 vendor 里的 2.46 QNN 库开放给 app 命名空间，然后走 QAIRT Easy API 加载 DLC。CPU 后端能跑，HTP 后端在 `QnnContext_create` 之后立即挂：

```
QnnDsp <E> DspTransport.openSession qnn_open failed, 0x80000406, prio 100
QnnDsp <E> IDspTransport: Unable to load lib 0x80000406
QnnDsp <E> Failed to create context with err 0x36b1
```

`0x80000406` = `AEE_ENOSUCHMOD`（DSP 侧找不到模块 handle）。

### 根因链

**Layer 1 — 版本鸿沟**：设备的 vendor firmware 里所有 QNN 库都是 **2.46.0**（`AISW_VERSION: 2.46.0`），signed skel 也是 2.46 的；而我们的 APK 打包了 **2.48.40** 的 QAIRT + QNN 库。

**Layer 2 — QAIRT 层的破坏性变更**：QAIRT 2.28 → 2.48 期间大改，2.48 版 QAIRT 在启动 backend 时会通过 FastRPC 做一系列版本相关的握手（新增字段、新增 platform capability 探测）。这些握手请求会传递到 stub 层的 `qnn_open()`。

**Layer 3 — 谁在被加载**：由于 `<uses-native-library>` + linker namespace 规则，app 里 dlopen 到的是 vendor `/vendor/lib64/libQnnHtpV81Stub.so`（2.46 stub），vendor 2.46 stub 只认识 2.46 skel 提供的 module handle。QAIRT 2.48 层往下发一个 2.48 引入的新协议字段，vendor 2.46 stub 转发给 vendor 2.46 skel，skel 返回 `AEE_ENOSUCHMOD`。

**Layer 4 — QNN Core API 却是稳定的**：QNN Core API（`QnnBackend_create` / `QnnContext_create` / `QnnGraph_execute`）的 ABI 从 2.24 到 2.48 都是稳定的。只要不走 QAIRT，2.48 SDK 的 `libQnnSystem.so`（纯 host、不碰 FastRPC）加 vendor 2.46 的 backend 库就能正常协作。

### 为什么 2.46 又能通

把整个技术栈按下面这样重新对齐后 HTP 直接跑通：

| 层 | 来源 | 版本 |
|---|---|---|
| App 业务代码 | 项目自写，QNN Direct API | — |
| libQnnSystem.so（DLC 解析 / composeGraphs） | **SDK 2.48**（vendor 2.46 那版本没实现 systemDlcComposeGraphs 接口） | 2.48.40 |
| libQnnCpu.so | SDK 2.48（vendor 不带 CPU backend） | 2.48.40 |
| libQnnHtp.so / libQnnHtpV81Stub.so / libQnnHtpPrepare.so | **vendor 2.46**（从设备 `/vendor/lib64/` 拉的） | 2.46.0 |
| libQnnHtpV81Skel.so (on DSP) | vendor firmware，Qualcomm 签名 | 2.46.0 |

关键：**ARM 侧和 DSP 侧的 HTP 三件套（backend + stub + skel）版本完全对齐**，且 ARM 侧 host-only 的 System / CPU 库版本无所谓（因为它们不参与 FastRPC）。

### 为什么不能反过来"让 vendor 升 2.48"

理论上只要 vendor firmware 更新到 2.48 QNN 就能所有栈都 2.48。但：

- 需要手机厂 OTA 一版新 firmware，包含 Qualcomm 用 2.48 私钥签的 signed skel
- 时间不可控（手机厂产品化流程）
- 我们做为 app 侧无权推动
- 就算 vendor 升级到某一次的 2.48.x，两周后 SDK 又出 2.49，可能又是同样问题

**结论：app 应该跟 vendor 版本对齐，而不是反过来**。所以本项目最终架构选 QNN Direct API + 拉 vendor 库。

### 五个关键错误码在这条链路上的含义

| 错误码 | 位置 | 含义 | 复现条件 |
|---|---|---|---|
| `err 4000` | `loadRemoteSymbols` | FastRPC 通道打不开（unsigned PD 权限不足） | 非 platform-signed app 尝试用 unsigned PD |
| `0x80000406` (AEE_ENOSUCHMOD) | `qnn_open` | DSP 找不到 stub 请求的 module handle | stub/skel 版本不匹配 或 QAIRT 版本鸿沟 |
| `0x36b1` | `QnnContext_create` | Transport setup 失败上抛 | 只是上层错误码，看下面的 Dsp 错 |
| `0x1787` | `QnnGraph_execute` | Graph 未 finalize | `composeGraphs` 后忘调 `graphFinalize` |
| `err 14001` | Transport / Platform info | 加载 default device 失败 | 前面 4000/0x80000406 的次生错误 |
| `err 1002` | `Failed to load skel` | skel 加载失败 | 版本不匹配 or ADSP_LIBRARY_PATH 找不到 |
| `err 30104` | `QairtSystemBuilder_build` | 构建 QAIRT 运行时失败 | 平台不支持 or 后端不可用 |

---

# 第二部分：真实使用 QNN 的硬性条件

## 2.1 硬件条件

### 支持的 SoC / Hexagon 架构

QNN HTP 只跑在 Qualcomm Hexagon NPU 上。项目上常见的 SoC:

| Hexagon 版本 | 常见 SoC | SDK arch 常量 |
|---|---|---|
| V66 | SDM865 (Snapdragon 865) | `QNN_HTP_DEVICE_ARCH_V66` |
| V68 | SM8350 (SD888) | `QNN_HTP_DEVICE_ARCH_V68` |
| V69 | SM8450 (SD8G1) | `QNN_HTP_DEVICE_ARCH_V69` |
| V73 | SM8550 (SD8G2) | `QNN_HTP_DEVICE_ARCH_V73` |
| V75 | SM8650 (SD8G3) | `QNN_HTP_DEVICE_ARCH_V75` |
| V79 | SM8750 (SD8 Elite) | `QNN_HTP_DEVICE_ARCH_V79` |
| **V81** | **SM8480XP (车规 8845)** | **`QNN_HTP_DEVICE_ARCH_V81`** |
| V85 / V89 | future | `QNN_HTP_DEVICE_ARCH_V85` / `_V89` |

每一代 stub 是 `libQnnHtpV<N>Stub.so`，skel 是 `libQnnHtpV<N>Skel.so`，**跨代不兼容**。

### 内存要求

HTP 的 workload 主要吃 **VTCM**（片上高速缓存）：

- V66 / V68：约 4 MB VTCM
- V73 / V75 / V79：约 8 MB
- V81（8845）：**8 MB VTCM**，DDR 带宽约 68 GB/s
- 大模型量化后要 profile 每层 VTCM 用量，超过就会 fallback 到 DDR（性能下降 10 倍以上）

## 2.2 OS / 内核条件

### Android 版本

- **Android 12+**（API 31+）推荐
- Android 14+ 才能用 `<uses-native-library>` 白名单机制（更早版本用 `android_dlopen_ext` + namespace 手动打开）
- 本项目在 Android 15 上验证

### 内核 / DSP driver

- 需要设备 `/dev/adsprpc-smd`、`/dev/cdsprpc-smd` 等 FastRPC 节点存在
- 需要 SELinux 允许 app 域访问上述 dev（vendor 通常 hardcode）
- `/proc/sys/fastrpc/*`（若存在）可以查询 unsigned PD 开关

### vendor firmware 必须内置

对每一代 Hexagon，vendor firmware 必须内置：

```
/vendor/lib64/libQnnSystem.so
/vendor/lib64/libQnnHtp.so
/vendor/lib64/libQnnHtpPrepare.so
/vendor/lib64/libQnnHtpV<N>Stub.so
/vendor/lib/rfsa/adsp/libQnnHtpV<N>Skel.so    (Qualcomm 签名的 signed skel)
/vendor/lib/rfsa/adsp/libcdsprpc.so           (fastrpc DSP-side loader)
/vendor/etc/public.libraries*.txt             (声明上面的库可被 app namespace 访问)
```

用 `adb shell ls /vendor/lib/rfsa/adsp/` 快速验证。如果 skel 缺失，说明这台机器的 firmware 没做 QNN 集成，任何 HTP 都跑不了。

## 2.3 应用签名与权限

### 平台签名（Signed PD 的强制要求）

生产设备上要走 signed PD，app 必须满足：

1. **APK 用 platform key 签名**：跟 firmware `/system/framework/framework-res.apk` 用的是同一把 keystore。手机厂商内部（如 StepOS 的统签服务）会通过 HTTP 远程签名服务提供，本项目走 `com.stepos.unifiedsigning.signing` 插件。
2. **Manifest 声明 `sharedUserId="android.uid.system"`**：让 app 以 uid=1000 运行，在 SELinux 上进入 `system_app` 域。
3. **`<uses-native-library>` 白名单**：Android 12 之后 vendor library 默认不在 app namespace 里，必须显式声明。
4. **安装为 priv-app**：`/system/priv-app/` 或 `/system_ext/priv-app/`（本项目仍走普通 install，因为 platform 签名 + sharedUserId 已足够）。

### 平台签名的实操约束

以本项目使用的 `com.stepos.unifiedsigning.signing` 统签插件为例：

- 只能在内网环境构建（签名走 `http://10.142.39.57:30080/api/v1/sign-bytes`）
- `signType=platform` 需要 review（新增走审批流）
- **不能同时写原生 `signingConfigs`**（跟插件的 packageXxx 任务重定向冲突，报 `in-process unified-apksig-signer failed: null`）
- 插件跟 Gradle 9.6.1 的 configuration-cache 不兼容，需要 `org.gradle.configuration-cache=false`

其它平台（AOSP 原生构建 / 三星 / 华为）同理，keystore 各家自己保存。

### 权限验证

签名成功后，通过 `apksigner verify --print-certs` 检查证书 DN：

```
CN=StepOS Team, OU=StepOS, O=Step, L=ShangHai, ST=ShangHai, C=CN
```

MD5 应为：`DA 02 86 46 F2 6A 15 17 FA B3 15 26 6B BD FF C8`（platform 类型的固定值）。

安装到设备后，`adb shell 'ps -A | grep <pkg>'` 看到 `system` uid 即证明生效。

## 2.4 需要内置的文件清单

### APK 内 `jniLibs/arm64-v8a/` 目录

| 库 | 版本策略 | 说明 |
|---|---|---|
| `libQnnSystem.so` | **SDK 版**（本项目 2.48） | 纯 host 库，负责 DLC 解析 + `systemDlcComposeGraphs`。vendor 的 2.46 版本没有这个接口。 |
| `libQnnCpu.so` | **SDK 版** | vendor 一般不带 CPU backend；只在 CPU 后端可用。 |
| `libQnnGpu.so` | SDK 版（可选） | 需要设备有 OpenCL ICD 才能真跑通 |
| `libQnnHtp.so` | **vendor 版**（从 `/vendor/lib64/` 拉） | HTP host 层，跟 FastRPC 强相关，必须跟 skel 同版本 |
| `libQnnHtpV<N>Stub.so` | **vendor 版** | FastRPC stub，`<N>` 匹配设备 Hexagon 版本 |
| `libQnnHtpPrepare.so` | **vendor 版** | HTP 图准备阶段的辅助（补 op / 优化） |

> `libQnnHtpV<N>Skel.so` **不放到 APK**——signed skel 只能在 `/vendor/lib/rfsa/adsp/` 下由 DSP 加载。APK 里没有它的位置。

### APK 内 `assets/` 目录

| 文件 | 说明 |
|---|---|
| `model.dlc` | 模型文件。运行时会拷贝到 `filesDir/cache/model.dlc` 再加载。 |
| `model_meta.json`（可选） | 模型 metadata（label、preprocessing 描述、期望 shape） |

### App 运行时环境

- `ADSP_LIBRARY_PATH` 环境变量，本 project 里在 native `init()` 里 setenv：
  ```
  /vendor/lib/rfsa/adsp;/vendor/dsp/cdsp;/vendor/dsp/adsp;/system/lib/rfsa/adsp;/dsp
  ```
- 必须在 dlopen `libQnnHtp.so` **之前** setenv，否则 FastRPC 找不到 skel

## 2.5 大模型（LLM）额外条件

跑 MobileNet / InceptionV3 级别的 CNN 用上面这套已经够了。要跑 Llama / Qwen 这种 LLM 还需要：

### 数据面

- **量化格式**：INT4 或 INT8 权重（fp16 activations），vendor firmware 只支持 Qualcomm 认可的量化 scheme（AIMET 输出的 `.encodings`）
- **上下文长度 vs VTCM**：8k+ context 大概率超过单个 HTP graph 能塞的 VTCM，需要走 **KV cache 分块**（HTP context binary 生成时做）
- **权重共享**：多个 graph（prefill + decode）共享同一份权重，用 QNN 2.28+ 引入的 `sharedWeights` context binary

### 运行时

- **Genie 框架**（`libGenie.so` in SDK `bin/aarch64-android/`）：Qualcomm 官方 LLM 编排层，封装了 tokenizer + prefill/decode 循环 + KV cache 管理。
- **DeepSeek/Llama 常用组合**：ONNX → QAIRT → DLC → context binary（HTP arch-specific）→ Genie config JSON。
- **VTCM / 功率配置**：`QnnHtpPerfInfrastructure` 里的 `setPowerConfig`，把 DSP 拉到 burst mode 保持推理吞吐。
- **模型分片**：LLM 单个 graph 太大就 split multi-graph，用 `QnnContext_setConfig` 声明 graphs 顺序。

### 参考项目

- **AIEngine**（`D:\workspace\AStudio\AIEngine`）：本厂内跑 LLM 的方案，用 Genie 框架封装 QNN 2.28-era API + 自研 `libllm.so` / `libstep_infer.so` 做 pipeline。它是"CNN QNN Direct API"路子的 LLM 扩展版本。

大模型条件超出本文档主线（本项目跑 MobileNetV2），需要时另开专题。

---

# 第三部分：完整集成指南

以下步骤在一个干净的 Android 应用工程上从零集成 QNN。

## 3.1 前置准备

### 需要的 SDK / 工具

1. **Qualcomm QAIRT SDK 2.48.40+**：从 Qualcomm 官网或 QPM 下载。安装到 `C:\Qualcomm\AIStack\QAIRT\2.48.40.260702\`。里面包含：
   - `include/QNN/` — QNN Direct API 头文件
   - `lib/aarch64-android/` — ARM 侧库
   - `bin/x86_64-windows-msvc/` — 转换工具（`qairt-onnx-to-dlc`、`qnn-context-binary-generator`）
2. **Android SDK / NDK**：NDK 25+，本项目用 28.2.13676358
3. **Android Studio**（推荐）+ Gradle 9.x
4. **平台签名服务 / keystore**：内网签名 URL 或 keystore.jks，视厂家规范
5. **目标设备**：内网测试机开好 ADB，vendor firmware 里带 QNN。用 `adb shell ls /vendor/lib/rfsa/adsp/libQnnHtpV*Skel.so` 快速验证。

### 目标设备预检

```bash
adb shell 'ls /vendor/lib/rfsa/adsp/'                 # 应看到 libQnnHtpV<N>Skel.so
adb shell 'ls /vendor/lib64/libQnn*.so'               # 应看到 libQnnSystem/libQnnHtp*
adb shell 'grep -l Qnn /vendor/etc/public.libraries*.txt'  # 应能列到白名单条目
adb shell 'getprop ro.soc.model'                      # 记录 SoC，用来确定 Hexagon 版本
```

## 3.2 项目结构

```
QnnDemo/
├── app/
│   ├── build.gradle.kts                  # 应用配置 + 签名插件
│   └── src/main/
│       ├── AndroidManifest.xml            # sharedUserId + uses-native-library
│       ├── assets/
│       │   └── model.dlc                  # 模型
│       ├── cpp/
│       │   ├── CMakeLists.txt
│       │   ├── qairt_include/QNN/         # SDK 拷来的 QNN 头
│       │   ├── qairt_runtime.cpp/.h       # QNN Direct API runtime
│       │   └── qnn_jni.cpp                # JNI bridge
│       ├── java/com/breeze/qnn/
│       │   ├── InferenceEngine.kt         # Kotlin 封装
│       │   ├── QnnNative.kt               # native 接口声明
│       │   └── MainActivity.kt
│       ├── jniLibs/arm64-v8a/
│       │   ├── libQnnSystem.so            # SDK 2.48
│       │   ├── libQnnCpu.so               # SDK 2.48
│       │   ├── libQnnHtp.so               # vendor 拉 (2.46)
│       │   ├── libQnnHtpV81Stub.so        # vendor 拉
│       │   └── libQnnHtpPrepare.so        # vendor 拉
│       └── res/...
├── gradle.properties                      # signType=platform
├── settings.gradle.kts                    # 内网 Nexus 仓库（statign 插件）
├── docs/                                  # 本目录
└── build.gradle.kts
```

## 3.3 Gradle 与签名配置

### `settings.gradle.kts`

添加内网 Nexus 仓库以拉签名插件：

```kotlin
pluginManagement {
    repositories {
        google { content { includeGroupByRegex("com\\.android.*"); includeGroupByRegex("com\\.google.*"); includeGroupByRegex("androidx.*") } }
        maven {
            isAllowInsecureProtocol = true
            url = uri("http://nexus.zyql.com/repository/maven-public/")
            credentials { username = "maven_ro"; password = "aZ8nP63#kR8d" }
        }
        maven {
            isAllowInsecureProtocol = true
            url = uri("http://nexus.zyql.com/repository/maven-snapshots/")
            credentials {
                username = System.getenv("NEXUS_USERNAME") ?: providers.gradleProperty("nexus.username").orNull
                password = System.getenv("NEXUS_PASSWORD") ?: providers.gradleProperty("nexus.password").orNull
            }
        }
        mavenCentral()
        gradlePluginPortal()
    }
}
```

`dependencyResolutionManagement` 里同样加内网 Nexus。

### `gradle.properties`

```properties
org.gradle.jvmargs=-Xmx2048m -Dfile.encoding=UTF-8
# 统签插件 (1.0.5-alpha01-SNAPSHOT) 用 project/task 引用，跟 Gradle 9.6.1 的
# configuration-cache 不兼容，会报 "cannot serialize object of type DefaultProject"
org.gradle.configuration-cache=false

# 平台签名 —— 新增 platform 需要评审，理由是 QNN HTP 需要 signed PD
unifiedsigning.signType=platform
nexus.username=maven_ro
```

### `app/build.gradle.kts`

```kotlin
plugins {
    alias(libs.plugins.android.application)
    id("com.stepos.unifiedsigning.signing") version "1.0.5-alpha01-SNAPSHOT"
}

android {
    namespace = "com.breeze.qnn"
    compileSdk { version = release(36) { minorApiLevel = 1 } }

    defaultConfig {
        applicationId = "com.breeze.qnn"
        minSdk = 34               // <uses-native-library> 稳态
        targetSdk = 36
        versionCode = 1
        versionName = "1.0"
        testInstrumentationRunner = "androidx.test.runner.AndroidJUnitRunner"

        ndk { abiFilters += "arm64-v8a" }    // QNN 只有 arm64 build

        externalNativeBuild {
            cmake {
                cppFlags("-std=c++17 -fexceptions -frtti -O2")
                arguments("-DANDROID_STL=c++_static")
            }
        }
    }

    // 关键：不能写 signingConfigs，跟统签插件冲突
    // signingConfigs { ... }   // <-- 千万别写

    externalNativeBuild { cmake { path = file("src/main/cpp/CMakeLists.txt"); version = "3.22.1" } }

    // QAIRT/QNN 库在运行时 dlopen，需要 uncompressed
    packaging { jniLibs { useLegacyPackaging = true } }

    lint { abortOnError = false }
    buildFeatures { viewBinding = true }
    compileOptions { sourceCompatibility = JavaVersion.VERSION_11; targetCompatibility = JavaVersion.VERSION_11 }
}
```

## 3.4 AndroidManifest 配置

```xml
<manifest xmlns:android="http://schemas.android.com/apk/res/android"
    xmlns:tools="http://schemas.android.com/tools"
    android:sharedUserId="android.uid.system">

    <application ...>
        <!-- FastRPC 通道 -->
        <uses-native-library android:name="libadsprpc.so" android:required="false" />
        <uses-native-library android:name="libcdsprpc.so" android:required="false" />

        <!-- QNN vendor libraries：ARM 侧 -->
        <uses-native-library android:name="libQnnSystem.so"       android:required="false" />
        <uses-native-library android:name="libQnnHtp.so"          android:required="false" />
        <uses-native-library android:name="libQnnHtpPrepare.so"   android:required="false" />
        <uses-native-library android:name="libQnnHtpV81Stub.so"   android:required="false" />
        <!-- DSP 侧 skel（fastrpc 通过 ADSP_LIBRARY_PATH 加载，声明是给 linker 白名单 -->
        <uses-native-library android:name="libQnnHtpV81Skel.so"   android:required="false" />

        <activity ... />
    </application>
</manifest>
```

> `V81` 换成目标设备的 Hexagon 版本；如果要跨代兼容，可以并列声明多个（V73/V75/V79/V81 等）。

## 3.5 jniLibs 库对齐

### CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.22.1)
project("qnndemo" LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

set(QAIRT_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/qairt_include")

add_library(qnndemo SHARED
    qnn_jni.cpp
    qairt_runtime.cpp
)

target_include_directories(qnndemo PRIVATE
    ${QAIRT_INCLUDE_DIR}
    ${QAIRT_INCLUDE_DIR}/QNN         # 让 QnnHtpDevice.h 里 #include "QnnCommon.h" 生效
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# QNN 库在 runtime 动态 dlopen，编译期不 link
target_link_libraries(qnndemo log android dl)

set_target_properties(qnndemo PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
)
```

### 库准备脚本

**从 SDK 拷 QNN 头 + libQnnSystem/libQnnCpu**（一次性，Windows PowerShell）：

```powershell
# docs/setup_demo.ps1
$SdkPath = "C:\Qualcomm\AIStack\QAIRT\2.48.40.260702"
$JniLibs = "app\src\main\jniLibs\arm64-v8a"

# 1. QNN Headers
Copy-Item "$SdkPath\include\QNN\*" "app\src\main\cpp\qairt_include\QNN\" -Recurse -Force

# 2. SDK 版 host/CPU 库
Copy-Item "$SdkPath\lib\aarch64-android\libQnnSystem.so" $JniLibs -Force
Copy-Item "$SdkPath\lib\aarch64-android\libQnnCpu.so"    $JniLibs -Force
```

**从 vendor 拉 HTP 三件套**（每次接一台新机器都要跑一次）：

```bash
# docs/pull_vendor_htp_libs.sh
JNI="app/src/main/jniLibs/arm64-v8a"
adb pull /vendor/lib64/libQnnHtp.so        "$JNI/"
adb pull /vendor/lib64/libQnnHtpPrepare.so "$JNI/"
adb pull /vendor/lib64/libQnnHtpV81Stub.so "$JNI/"

# 版本核对
for f in libQnnHtp.so libQnnHtpPrepare.so libQnnHtpV81Stub.so libQnnSystem.so libQnnCpu.so; do
    grep -aoE 'AISW_VERSION: [0-9.]+' "$JNI/$f" | head -1
done
```

期望输出：
```
libQnnHtp.so:AISW_VERSION: 2.46.0        # vendor
libQnnHtpPrepare.so:AISW_VERSION: 2.46.0 # vendor
libQnnHtpV81Stub.so:AISW_VERSION: 2.46.0 # vendor
libQnnSystem.so:  (SDK 版本，无字符串)   # SDK 2.48
libQnnCpu.so:AISW_VERSION: 2.48.40       # SDK 2.48
```

## 3.6 Native 代码骨架（QNN Direct API）

### 初始化：`init(libSearchPath)`

```cpp
bool QairtRuntime::init(const std::string& libSearchPath) {
    // 1) 关键：dlopen libQnnHtp.so 之前必须 setenv ADSP_LIBRARY_PATH，
    //    否则 FastRPC 在 DSP 侧找不到 skel
    setenv("ADSP_LIBRARY_PATH",
           "/vendor/lib/rfsa/adsp;/vendor/dsp/cdsp;/vendor/dsp/adsp;"
           "/system/lib/rfsa/adsp;/dsp",
           /*overwrite*/ 1);

    // 2) dlopen libQnnSystem.so，getProviders 拿到 host 端 API 表
    impl_->systemLib = dlopen("libQnnSystem.so", RTLD_NOW | RTLD_LOCAL);
    auto sysProviders = reinterpret_cast<
        Qnn_ErrorHandle_t (*)(const QnnSystemInterface_t***, uint32_t*)>(
        dlsym(impl_->systemLib, "QnnSystemInterface_getProviders"));

    const QnnSystemInterface_t** providers = nullptr;
    uint32_t n = 0;
    sysProviders(&providers, &n);
    impl_->sysIface = providers[0];    // { systemApiVersion, systemLogCreate, systemDlcCreateFromFile, systemDlcComposeGraphs, ... }

    // 3) 建 host-side log（DLC 解析报错时打印）
    impl_->sysIface->QNN_SYSTEM_INTERFACE_VER_NAME.systemLogCreate(
        qnnLogCallback, QNN_LOG_LEVEL_WARN, &impl_->sysLog);
    return true;
}
```

### 加载 DLC 与建 runtime：`loadDlc(dlcPath, backend)`

```cpp
bool QairtRuntime::loadDlc(const std::string& dlcPath, BackendType backend) {
    // 1) 挑对应后端库 & 释放旧 runtime（切换 backend 场景）
    const char* backendLibName =
        backend == BackendType::HTP ? "libQnnHtp.so" :
        backend == BackendType::GPU ? "libQnnGpu.so" :
                                       "libQnnCpu.so";
    impl_->backendLib = dlopen(backendLibName, RTLD_NOW | RTLD_LOCAL);

    // 2) getProviders 拿 backend interface（QnnBackend/Context/Device/Graph/...）
    auto beProviders = reinterpret_cast<
        Qnn_ErrorHandle_t (*)(const QnnInterface_t***, uint32_t*)>(
        dlsym(impl_->backendLib, "QnnInterface_getProviders"));
    const QnnInterface_t** providers = nullptr; uint32_t n = 0;
    beProviders(&providers, &n);
    impl_->qnnIface = providers[0];
    const auto& I = impl_->qnnIface->QNN_INTERFACE_VER_NAME;

    // 3) log / backend
    I.logCreate(qnnLogCallback, QNN_LOG_LEVEL_WARN, &impl_->backendLog);
    I.backendCreate(impl_->backendLog, /*cfgs*/ nullptr, &impl_->backendHandle);

    // 4) device：HTP 必须 signed PD + socModel
    if (backend == BackendType::HTP) {
        QnnHtpDevice_CustomConfig_t socCfg{};
        socCfg.option = QNN_HTP_DEVICE_CONFIG_OPTION_SOC;
        socCfg.socModel = 685;   // 8845，见附录 A

        QnnHtpDevice_CustomConfig_t signedPd{};
        signedPd.option = QNN_HTP_DEVICE_CONFIG_OPTION_SIGNEDPD;
        signedPd.useSignedProcessDomain.deviceId = 0;
        signedPd.useSignedProcessDomain.useSignedProcessDomain = true;

        QnnDevice_Config_t c1{}, c2{};
        c1.option = QNN_DEVICE_CONFIG_OPTION_CUSTOM; c1.customConfig = &socCfg;
        c2.option = QNN_DEVICE_CONFIG_OPTION_CUSTOM; c2.customConfig = &signedPd;
        const QnnDevice_Config_t* cfgs[] = { &c1, &c2, nullptr };

        I.deviceCreate(impl_->backendLog, cfgs, &impl_->deviceHandle);
    } else {
        // CPU/GPU 直接 NULL device
        I.deviceCreate(impl_->backendLog, nullptr, &impl_->deviceHandle);
    }

    // 5) context
    I.contextCreate(impl_->backendHandle, impl_->deviceHandle, nullptr, &impl_->contextHandle);

    // 6) DLC → context 上 compose graphs
    const auto& S = impl_->sysIface->QNN_SYSTEM_INTERFACE_VER_NAME;
    S.systemDlcCreateFromFile(impl_->sysLog, dlcPath.c_str(), &impl_->dlcHandle);

    QnnSystemContext_GraphInfo_t* infos = nullptr; uint32_t numGraphs = 0;
    S.systemDlcComposeGraphs(
        impl_->dlcHandle, /*graphConfigs*/ nullptr, /*numConfigs*/ 0,
        impl_->backendHandle, impl_->contextHandle,
        *impl_->qnnIface,               // 传整个 QnnInterface_t（含版本）
        QNN_SYSTEM_CONTEXT_GRAPH_INFO_VERSION_1,
        &infos, &numGraphs);

    // 7) 遍历 composed graphs：retrieve + finalize
    for (uint32_t i = 0; i < numGraphs; ++i) {
        const auto& v1 = infos[i].graphInfoV1;
        Qnn_GraphHandle_t gh = nullptr;
        I.graphRetrieve(impl_->contextHandle, v1.graphName, &gh);
        //  关键：composeGraphs 只加节点，不 finalize，必须显式调
        I.graphFinalize(gh, /*profile*/ nullptr, /*signal*/ nullptr);
        // 存 graph handle + input/output tensor 模板（模板里 dim/name 指到 DLC 内部内存）
        ...
    }
    return true;
}
```

### 执行推理：`execute(graphName, inputs)`

```cpp
InferenceResult QairtRuntime::execute(const std::string& graphName,
                                       const std::vector<std::vector<uint8_t>>& inputs) {
    // 1) 拷贝 tensor 模板，修改 clientBuf 指向我们的 buffer（不能改原 DLC 里的）
    std::vector<Qnn_Tensor_t> inTensors  = slot->inputs;    // shallow copy
    std::vector<Qnn_Tensor_t> outTensors = slot->outputs;

    for (size_t j = 0; j < inTensors.size(); ++j) {
        Qnn_ClientBuffer_t* cb = &inTensors[j].v1.clientBuf;   // 或 v2.clientBuf
        cb->data = const_cast<uint8_t*>(inputs[j].data());
        cb->dataSize = inputs[j].size();
    }
    // 输出 buffer 分配后同样绑上

    // 2) graphExecute（同步）
    const auto& I = impl_->qnnIface->QNN_INTERFACE_VER_NAME;
    I.graphExecute(slot->handle,
                   inTensors.data(),  inTensors.size(),
                   outTensors.data(), outTensors.size(),
                   nullptr, nullptr);
}
```

完整可编译的参考实现见项目内 `app/src/main/cpp/qairt_runtime.cpp`。

## 3.7 模型（DLC）准备

### DLC 是什么

DLC = Deep Learning Container，Qualcomm 定义的模型文件格式，里面包含：

- 图拓扑（op / tensor 关系）
- 权重（`raw_weights`，可以带量化编码）
- 元数据（`metadata`）
- 可选：预先编译好的 HTP 缓存（`HTP_CACHE_RECORD`），首次加载可省编译时间

### 转换流程

```
ONNX / TFLite / PyTorch(saved_model)   Source
        |
        v
qairt-onnx-to-dlc / qairt-tflite-to-dlc      offline convert
        |
        v
model_fp32.dlc                                    (float，还没量化)
        |
        v
qairt-quantize --input model_fp32.dlc \
               --bitwidth 8 \
               --calibration-list calib.list
        |
        v
model_quant.dlc                                    (量化后)
        |
        v
[optional] qnn-context-binary-generator            device-specific，可提前编译好 HTP 缓存
        |
        v
model_with_htp_cache.dlc  or  model_htp.bin
```

具体命令、量化数据集准备、精度调优见 `docs/DLC转换指南.md` 与 `docs/QNN模型产物转换指南.md`。

### 运行时载入

APK 里放的是最终 `.dlc`，`InferenceEngine.kt` 里从 assets 拷到 cacheDir，再传绝对路径给 native：

```kotlin
context.assets.open("model.dlc").use { input ->
    File(context.cacheDir, "model.dlc").outputStream().use { input.copyTo(it) }
}
loadDlc("${context.cacheDir}/model.dlc", QnnNative.Backend.HTP)
```

## 3.8 集成验证清单

一步步验证下列每一项：

### 构建阶段

```bash
./gradlew :app:assembleDebug
apksigner verify --print-certs app/build/outputs/apk/debug/app-debug.apk
```

- 输出的证书 DN 与预期匹配（如 `CN=StepOS Team, ...`）
- `apksigner verify --print-certs` MD5 = platform 类型（如 `DA 02 86 46 F2 6A 15 17 FA B3 15 26 6B BD FF C8`）

### 安装 + 权限

```bash
adb install -r app/build/outputs/apk/debug/app-debug.apk
adb shell 'ps -A | grep <pkg>'
```

- 进程 UID 显示为 `system`（uid=1000），说明 sharedUserId 生效
- 若显示为 `u0_a<N>`，说明签名或 sharedUserId 未生效，检查证书

### 库对齐

```bash
adb shell "ls -la /data/app/*/com.breeze.qnn-*/lib/arm64/" | grep libQnn
```

- 应该看到 libQnnCpu / libQnnHtp / libQnnHtpV81Stub / libQnnHtpPrepare / libQnnSystem 全在

### E2E 测试

用项目自带的 `BackendE2ETest` 分别跑 CPU / HTP / GPU：

```bash
bash docs/run_e2e_tests.sh cpuBackend
bash docs/run_e2e_tests.sh htpBackend
bash docs/run_e2e_tests.sh              # 全部
```

期望：

```
PASS: CPU elapsed=xx.xxxms outputSizes=[N]
PASS: HTP elapsed=xx.xxxms outputSizes=[N]
```

CPU 通常 20-40 ms（MobileNetV2），HTP 应在 2-5 ms 左右（8845 上），加速比 5-15x。

---

# 第四部分：故障排查手册

## 4.1 错误码字典

QNN / DSP 错误码在 logcat 里通常带 `QNN` 或 `QnnDsp` tag。

| 错误码 | 十进制 | 出现位置 | 含义 | 排查方向 |
|---|---|---|---|---|
| `0x03e8` | 1000 | `Backend/Device SetConfig` | `NOT_SUPPORTED` | 用了当前 backend 不支持的 config option（如 CPU 传 HTP CustomConfig） |
| `0x0FA0` | 4000 | `loadRemoteSymbols` | FastRPC PD 权限不足 | 检查是否 signed PD + platform signing |
| `0x1787` | 6023 | `QnnGraph_execute` | Graph not finalized | 加 `QnnGraph_finalize` 调用 |
| `0x36B1` | 14001 | `QnnContext_create` | Transport / platform info 失败 | 看前面的 DspTransport 错，通常是版本问题 |
| `0x75A8` | 30104 | `QairtSystemBuilder_build` | 平台或后端不可用 | 缺库 / SoC 不匹配 |
| `0x80000406` | AEE_ENOSUCHMOD | `DspTransport.openSession qnn_open` | DSP 找不到模块 handle | stub/skel 版本不匹配 or 层间版本鸿沟 |
| `0x80000008` | AEE_EFAILED | `DspTransport.getHandle` | 通用失败 | 看前一条错，一般是次生 |

## 4.2 常见故障模式

### 症状 1：`err 4000 loadRemoteSymbols failed`

**含义**：FastRPC 通道 open 失败，通常权限或 PD 类型不对。

**排查**：
1. `adb shell 'ps -A | grep <pkg>'` 看 UID。不是 `system` 说明 platform 签名没生效。
2. `adb shell 'cat /proc/sys/fastrpc/*'`（如果存在）看 unsigned PD 开关。生产设备通常 `disable_unsigned_pd=1`，只能走 signed PD。
3. HtpDevice CustomConfig 里 `useSignedProcessDomain` 得设 `true`。

### 症状 2：`qnn_open failed 0x80000406 (AEE_ENOSUCHMOD)`

**含义**：FastRPC 通道通了，但 DSP 找不到 stub 请求的模块。

**排查**：
1. 对比 stub 与 skel 版本（`grep -aoE 'AISW_VERSION: [0-9.]+' libQnnHtpV81Stub.so`），必须一致。
2. 是否用了 QAIRT 层？QAIRT 版本鸿沟会传递到这里。改成 QNN Direct API。
3. skel 是否 signed？`adb shell ls -la /vendor/lib/rfsa/adsp/` 看权限，属于 vendor firmware 的 signed 才能在 signed PD 加载。

### 症状 3：`Attempting to free non-owning handle`

**含义**：Handle 所有权错误，通常是同一个 graph handle 被 free 两次（一次显式 free，一次 context free 时自动清）。

**排查**：graph handle 是 context 拥有的，只在 context free 时清；不要单独 `graph.free`。

### 症状 4：`Graph 256 was not Finalized`

**含义**：`composeGraphs` 只添加节点，没 finalize；`graphExecute` 前必须调 `graphFinalize`。

**排查**：见 3.6 代码骨架中"6.5) 遍历 composed graphs：retrieve + finalize"。

### 症状 5：CPU 后端 OK，HTP 后端 SIGSEGV

**含义**：常见于 QAIRT Easy API 的 `Qairt_executeGraph` 拿错了 tensor handle 类型：`QairtSystem_Context_GraphInfo_getInputAt` 返回 opaque handle，而 execute 要"raw pointer"。用 `Qairt_getModelInputInfoAt` 拿真指针。

（改成 QNN Direct API 后不会有这个坑，因为 `QnnSystemContext_GraphInfoV1_t` 直接就有 `Qnn_Tensor_t*` 数组。）

### 症状 6：init 阶段 dlopen(libQnnHtp.so) 失败

**含义**：linker 找不到库。

**排查**：
1. `<uses-native-library>` 白名单里声明了没
2. `adb shell "cat /vendor/etc/public.libraries*.txt | grep Qnn"` 看 vendor 是不是公开了这个库
3. 该库的 NEEDED 依赖（如 `libcdsprpc.so`）是否也在白名单里

### 症状 7：模型加载慢（首次 10s+）

**含义**：HTP 首次加载 DLC 会做 op 编译（`libQnnHtpPrepare.so` 参与），可以几秒到几十秒。

**优化**：
- 用 `qnn-context-binary-generator` 提前编译成 `.bin`，运行时用 `QnnContext_createFromBinary` 载入，冷启动降到 100 ms 级
- DLC 里塞 `HTP_CACHE_RECORD` 也能加速（但仍要跟 SoC / 编译器版本对齐，不能跨设备缓存）

---

# 附录 A：SoC / Hexagon 版本速查

`QnnHtpDevice_CustomConfig_t.socModel` 值：

| SoC 名 | 内部代号 | `socModel` |
|---|---|---|
| SM8250 (SD865) | kona | 21 |
| SM8350 (SD888) | lahaina | 30 |
| SM8450 (SD8G1) | waipio | 36 |
| SM8475 (SD8+G1) | fillmore | 42 |
| SM8550 (SD8G2) | kailua | 43 |
| SM8650 (SD8G3) | pineapple | 57 |
| SM8750 (SD8 Elite) | sun | 69 |
| **SM8480XP (车规 8845)** | **canoe** | **685** |

（完整列表见 SDK 里 `<sdk>/docs/QNN/general/htp/htp_backend.html` 或 `QnnHtpCommon.h` 内部枚举。）

Hexagon arch 值直接在 `QnnHtpDeviceConfigShared.h`：

```c
QNN_HTP_DEVICE_ARCH_V68 = 68,
QNN_HTP_DEVICE_ARCH_V69 = 69,
QNN_HTP_DEVICE_ARCH_V73 = 73,
QNN_HTP_DEVICE_ARCH_V75 = 75,
QNN_HTP_DEVICE_ARCH_V79 = 79,
QNN_HTP_DEVICE_ARCH_V81 = 81,       // 8845
QNN_HTP_DEVICE_ARCH_V85 = 85,
QNN_HTP_DEVICE_ARCH_V89 = 89,
```

# 附录 B：本项目的 QNN Direct API 参考实现

以下文件即完整参考，可以作为其它项目 copy-paste 起点：

| 文件 | 作用 |
|---|---|
| `app/src/main/cpp/qairt_runtime.cpp` | QNN Direct API 主体（`init` / `loadDlc` / `execute` / `cleanup`） |
| `app/src/main/cpp/qairt_runtime.h` | 公开的 C++ 接口（`QairtRuntime` 类，`BackendType` 枚举等） |
| `app/src/main/cpp/qnn_jni.cpp` | JNI bridge，把 Kotlin 调用桥到 QairtRuntime |
| `app/src/main/cpp/CMakeLists.txt` | native 构建配置 |
| `app/src/main/java/com/breeze/qnn/InferenceEngine.kt` | Kotlin 封装 |
| `app/src/main/java/com/breeze/qnn/QnnNative.kt` | JNI 声明 |
| `app/src/androidTest/java/com/breeze/qnn/BackendE2ETest.kt` | 三后端 E2E 自动化测试 |
| `docs/setup_demo.ps1` | 从 SDK 拷头 + 拷 host 库 |
| `docs/pull_vendor_htp_libs.sh` | 从设备拉 vendor HTP 库对齐版本 |
| `docs/run_e2e_tests.sh` | E2E 一键跑 + logcat 汇总 |

## 快速起步

- 全新环境：`docs/setup_demo.ps1 -SdkPath "C:\Qualcomm\AIStack\QAIRT\2.48.40.260702"`
- 接新机器：`bash docs/pull_vendor_htp_libs.sh`（拉 vendor HTP 三件套）
- 验证：`bash docs/run_e2e_tests.sh`（跑三后端 E2E，看 PASS / SOFT FAIL）
- 出错时先查 `docs/HTP-8845-诊断报告.md` 里的失败模式清单

## 版本演进注意事项

- 升 QAIRT SDK：只需要 `docs/setup_demo.ps1` 重跑（换 SDK 路径）；只影响 host libQnnSystem + CPU/GPU 库
- 换目标设备：`docs/pull_vendor_htp_libs.sh` 重跑，检查 stub/skel arch（如 V81 → V85 要同步改 manifest `<uses-native-library>` 名称、代码里 `socModel`）
- vendor firmware 升 QNN 版本：拉一次新的 vendor 库，其它不用改
- 换到大模型：本文档以 MobileNetV2 为例。跑 LLM 需要额外接入 Genie 框架 + `libGenie.so`，见 AIEngine 项目

---

## LLM 集成（Qwen3-3B on 8845）

在 CNN 集成基础上扩展 LLM 场景（Qwen3-3B via Qualcomm Genie 框架）：

- `docs/superpowers/specs/2026-07-22-8845-llm-inference-design.md` — Design spec
- `docs/superpowers/plans/2026-07-22-8845-llm-qwen3.md` — Implementation plan
- `docs/LLM-baseline.md` — 性能 baseline
- `docs/setup_qwen3_llm.sh` — 权重预置脚本（`bash docs/setup_qwen3_llm.sh <src_dir>`）
- 跑 test：`bash docs/run_e2e_tests.sh llmGenerate`

关键差异（vs CNN）：
- 用 Genie 高层框架而非 QNN Direct API 直接编排 prefill/decode
- 权重 1.5-2 GB 不进 APK，走 adb push 到 `filesDir/llm/qwen3-3b/`
- signed PD + 8 MB VTCM 满配（unsigned PD 会导致 KV cache 分块，性能腰斩）

---

**文档版本**：v1.0，2026-07-21
**作者**：QnnDemo 项目组
**基线**：Snapdragon 8845 (Hexagon V81), Android 15, QAIRT SDK 2.48.40, vendor QNN 2.46.0

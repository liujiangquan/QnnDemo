# QnnDemo — Android QNN/QAIRT 推理示例

基于 Qualcomm AI Runtime (QAIRT) SDK 2.48.40 的 Android 端 QNN 推理 Demo。
通过 JNI 桥接 QAIRT Easy API，在 Android 设备上加载 DLC 模型并执行推理。

## 功能特性

- **QAIRT Easy API 集成**：通过 `QairtSystemBuilder` + `Qairt_V1_t` 高层 C API 完成模型加载与推理，无需手写复杂的 QNN 函数指针表
- **多后端支持**：CPU / GPU / HTP（NPU）后端可切换
- **运行时 dlopen**：QAIRT 共享库在运行时通过 `dlopen` 加载，不静态链接，APK 体积可控
- **图/张量元信息查询**：加载模型后展示图名、输入/输出张量维度、数据类型、字节数
- **推理计时**：单次推理耗时统计（毫秒级）
- **Material Design 3 风格 UI**：卡片式布局，后端选择、模型加载、推理执行一目了然

## 项目结构

```
QnnDemo/
├── app/
│   ├── build.gradle.kts                    # AGP 配置：NDK/CMake/ViewBinding
│   └── src/main/
│       ├── AndroidManifest.xml            # MainActivity 声明
│       ├── cpp/
│       │   ├── CMakeLists.txt             # native 构建配置
│       │   ├── qairt_runtime.h/.cpp       # QAIRT Easy API C++ 封装
│       │   ├── qnn_jni.cpp                # JNI 桥接
│       │   └── qairt_include/             # QAIRT 头文件（由 setup 脚本拷贝）
│       ├── java/com/breeze/qnn/
│       │   ├── MainActivity.kt            # 主界面
│       │   ├── QnnNative.kt               # JNI 接口声明
│       │   ├── InferenceEngine.kt         # 推理引擎封装
│       │   └── Model.kt                   # 数据类（GraphInfo/TensorInfo/InferenceResult）
│       ├── res/layout/activity_main.xml   # UI 布局
│       ├── res/values/strings.xml         # 字符串
│       ├── assets/                         # DLC 模型（由 setup 脚本拷贝）
│       └── jniLibs/arm64-v8a/              # 预编译 .so（由 setup 脚本拷贝）
├── docs/                                   # 文档与脚本目录
│   ├── setup_demo.ps1                     # 依赖准备脚本
│   ├── convert_model.ps1                  # ONNX → DLC 转换脚本
│   ├── get_logcat.bat                     # logcat 抓取脚本
│   └── DLC转换指南.md                      # DLC 模型获取与转换详细指南
└── README.md
```

## 环境要求

| 项 | 要求 |
|----|------|
| Android Studio | Hedgehog / Iguana 或更高 |
| AGP | 9.3.0（项目已配置） |
| Gradle | 9.6.1（项目已配置） |
| NDK | r25c 或更高（CMake 3.22.1） |
| compileSdk | 36，minSdk 34 |
| 目标设备 | arm64-v8a 架构（绝大多数现代 Android 手机） |
| QAIRT SDK | 2.48.40.260702 或兼容版本 |

## 快速开始

### 1. 准备依赖

运行 `docs/` 目录下的 PowerShell 脚本，自动从 SDK 拷贝头文件、预编译库与示例模型：

```powershell
.\docs\setup_demo.ps1 -SdkPath "F:\AI\sdk\qairt\2.48.40.260702"
```

脚本完成的工作：
- 拷贝 `include/QAIRT/` → `app/src/main/cpp/qairt_include/`
- 拷贝 `lib/aarch64-android/libQairtSystem.so`、`libQairtCpu.so` 等 → `app/src/main/jniLibs/arm64-v8a/`
- 尝试生成 InceptionV3 DLC → `app/src/main/assets/model.dlc`

若自动生成 DLC 失败，可使用 `docs/convert_model.ps1` 从 ONNX 模型转换（详见 `docs/DLC转换指南.md`），
或手动将任意 `.dlc` 文件拷贝到 `app/src/main/assets/model.dlc`。

### 2. 在 Android Studio 中构建

1. 用 Android Studio 打开 `QnnDemo` 项目
2. 等待 Gradle 同步完成（首次会下载 NDK 与 CMake）
3. 连接 arm64-v8a Android 设备（开启 USB 调试）
4. 点击 **Run 'app'**

### 3. 使用 App

1. 选择后端（CPU / GPU / HTP）
2. 点击 **初始化** → 加载 QAIRT 运行时
3. 确认 DLC 文件名（默认 `model.dlc`），点击 **加载 DLC**
4. 查看图与张量信息
5. 选择图，点击 **执行推理**
6. 在日志区查看耗时与输出张量大小

## 架构说明

### 推理流程

```
Kotlin (MainActivity)
   │  调用 InferenceEngine
   ▼
InferenceEngine (Kotlin)
   │  通过 QnnNative (JNI) 调用 native
   ▼
QnnNative (JNI 声明)
   │  System.loadLibrary("qnndemo")
   ▼
qnn_jni.cpp (C++)
   │  调用 QairtRuntime
   ▼
QairtRuntime (C++ 封装)
   │  dlopen libQairtSystem.so
   │  QairtSystemInterface_getInterface(id, &iface)
   ▼
QAIRT Easy API (C)
   ├── QairtSystemBuilder_V1_t  (id=30010)  构建
   ├── QairtSystem_DlcV1_t      (id=35000)  加载 DLC
   ├── Qairt_V1_t               (id=30011)  执行推理
   ├── QairtSystem_Context_GraphInfoV1_t  (id=30002)  图信息
   └── QairtSystem_Context_TensorInfoV1_t (id=30003)  张量信息
```

### 关键接口

| 接口 | ID | 用途 |
|------|-----|------|
| `QairtSystemLog_V1_t` | 30100 | 创建系统日志 |
| `QairtSystem_DlcV1_t` | 35000 | 从文件加载 DLC |
| `QairtSystemBuilder_V1_t` | 30010 | 配置并构建运行时 |
| `Qairt_V1_t` | 30011 | 执行推理、查询图信息 |
| `QairtSystem_Context_GraphInfoV1_t` | 30002 | 读取图名、输入/输出张量 |
| `QairtSystem_Context_TensorInfoV1_t` | 30003 | 读取张量维度、数据类型 |

## 常见问题

### Q: 运行时提示 `dlopen("libQairtSystem.so") 失败`

确保已运行 `docs/setup_demo.ps1`，且 `app/src/main/jniLibs/arm64-v8a/libQairtSystem.so` 存在。
检查 `build.gradle.kts` 中 `packaging.jniLibs.useLegacyPackaging = true`。

### Q: 加载 DLC 失败，提示 `QairtSystemDlc_createFromFile 失败`

- 确认 `app/src/main/assets/model.dlc` 存在且为有效的 DLC 文件
- 查看 logcat（过滤 `QnnDemo`）获取详细错误
- 部分后端不支持某些算子，尝试切换为 CPU 后端

### Q: HTP 后端构建失败

HTP（NPU）后端需要：
- 高通骁龙平台设备（Snapdragon 8 Gen1 及以上）
- 额外的 `libQnnHtpV*Stub.so` 与 `libQnnHtp*Skel.so`（需从 SDK 的 `lib/hexagon-v*/unsigned/` 拷贝）
- 设置 `ADSP_LIBRARY_PATH` 环境变量

非高通设备请使用 CPU 后端。

### Q: 如何生成自定义 DLC 模型

使用 QAIRT SDK 的转换工具：

```bash
# ONNX → DLC
qairt-onnx-to-dlc -i model.onnx -o model.dlc

# TensorFlow → DLC
qairt-tensorflow-to-dlc -i model.pb -o model.dlc

# 量化
qairt-onnx-to-dlc -i model.onnx -o model_quantized.dlc --quantize_full
```

生成后将 `.dlc` 拷贝到 `app/src/main/assets/model.dlc`。

## 许可证

本项目为示例代码，QAIRT SDK 的许可请遵循 Qualcomm 的 EULA。

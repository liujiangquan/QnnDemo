# QNN 模型产物转换指南（DLC / SO / BIN）

以 `mobilenetv2-12.onnx` 为例，总结如何把任意框架模型（ONNX / TensorFlow / TFLite / PyTorch / Caffe）转换为 Qualcomm AI Runtime（QNN/QAIRT）可运行的三种产物：**DLC**、**model.so**（模型库）、**context binary .bin**（预编译上下文）。

> 本文档与 `docs/DLC转换指南.md`、`docs/convert_model.ps1` 互补：前者偏 DLC 单一路径的速查，本文系统覆盖三种产物及其流水线。

---

## 1. 三种产物总览

| 产物 | 形态 | 谁加载 | 加载速度 | 跨设备 | 适用场景 |
|------|------|--------|----------|--------|----------|
| **DLC** | `.dlc` 文件 | SNPE / QAIRT Easy API / QNN（via libQnnModelDlc.so） | 中（需解析） | ✅ 同架构通用 | 最简单、跨设备、Easy API |
| **Model Library** | `lib<name>.so` | QNN 低层 API（dlopen + model lib API） | 较快 | ✅ 按架构构建后通用 | 不发模型文件、QNN 低层控制 |
| **Context Binary** | `<name>.bin` | QNN context binary API | 最快 | ❌ 强绑定 SoC+后端 | 极致启动延迟、固定机型部署 |

依赖关系（后一种是前一种的进一步编译）：

```
ONNX/TF/TFLite/PT/Caffe
      │  qnn-onnx-converter / qairt-converter
      ├──────────────────────►  model.dlc        （DLC 路径，止于此）
      │
      └─►  model.cpp + model.bin
                │  qnn-model-lib-generator
                └─►  libmodel.so                 （Model Library 路径，止于此）
                          │  qnn-context-binary-generator --model=libmodel.so --backend=libQnnHtp.so
                          └─►  model.bin          （Context Binary 路径）
```

---

## 2. 前置准备

### 2.1 SDK 与环境变量

```powershell
# SDK 根目录
$sdk = "F:\AI\sdk\qairt\2.48.40.260702"
$env:QAIRT_SDK_ROOT = $sdk
$env:QNN_SDK_ROOT   = $sdk
$env:SNPE_SDK_ROOT  = $sdk

# 一键 source 官方环境（设置 PYTHONPATH / PATH / lib）
. "$sdk\bin\envsetup.ps1"

# Windows 控制台防 GBK 编码报错
$env:PYTHONIOENCODING = "utf-8"
```

### 2.2 Python 依赖

转换器是 Python 脚本，且其原生扩展（`libDlModelToolsPy312.pyd` 等）实际链接 `python310.dll`，**必须用 Python 3.10**：

```powershell
# 用 3.10（示例路径，按实际安装位置调整）
$py = "D:\tools\ProgramFile\Python\Python310\python.exe"
& $py -m pip install numpy==1.26.4 protobuf onnx==1.15.0 pyyaml packaging scipy attrs tabulate six decorator jsonschema pandas
```

> `qnn-onnx-converter` 额外依赖 `pandas`；`qairt-converter` 不需要。`onnx` 必须用 1.15.0（新版移除了 `onnx.version.version`，会导致转换器崩溃）。

### 2.3 示例模型

```powershell
# 下载 mobilenetv2-12.onnx（约 14 MB，输入 input:1x3x224x224，输出 output:1x1000）
$WorkDir = "F:\AI\code\AStudio\QnnDemo\build\model_convert"
New-Item -ItemType Directory -Force -Path $WorkDir | Out-Null
$onnx = Join-Path $WorkDir "mobilenetv2-12.onnx"
if (-not (Test-Path $onnx)) {
    Invoke-WebRequest "https://github.com/onnx/models/raw/main/validated/vision/classification/mobilenetv2/model/mobilenetv2-12.onnx" -OutFile $onnx
}
```

确认输入/输出名与形状（转换时要用）：

```powershell
& $py -c "import onnx; m=onnx.load(r'$onnx'); print([(i.name,[d.dim_value for d in i.type.tensor_type.shape.dim]) for i in m.graph.input]); print([(o.name,[d.dim_value for d in o.type.tensor_type.shape.dim]) for o in m.graph.output])"
# 期望输出：input [[1,3,224,224]]；output [[1,1000]]
```

---

## 3. 路径一：转成 DLC（最简单）

DLC 是 SNPE / QAIRT Easy API 的原生容器，**本项目 `QnnDemo` 用的就是这条路径**。

### 3.1 方式 A：qairt-converter（推荐，统一入口）

```powershell
& $py "$sdk\bin\x86_64-windows-msvc\qairt-converter" `
    -i "$onnx" `
    -o "$WorkDir\mobilenetv2.dlc" `
    --source_model_input_shape "input:1,3,224,224"
```

- `-i / --input_network`：输入模型路径
- `-o / --output_path`：输出 DLC 路径
- `--source_model_input_shape`：动态输入维度时显式指定（`mobilenetv2-12` 输入名 `input`）
- 产物：`mobilenetv2.dlc`

### 3.2 方式 B：snpe-onnx-to-dlc（SNPE 路径，等价）

```powershell
& $py "$sdk\bin\x86_64-windows-msvc\snpe-onnx-to-dlc" `
    -i "$onnx" `
    -o "$WorkDir\mobilenetv2.dlc" `
    -d "input" "1,3,224,224"
```

- `-d INPUT_NAME INPUT_DIM`：指定输入形状（注意是两个参数）

### 3.3 方式 C：qnn-onnx-converter（QNN 路径，需显式指定 DLC）

`qnn-onnx-converter` **默认产出 `.cpp`+`.bin`**（见路径二），要 DLC 必须加 `--export_format dlc`：

```powershell
& $py "$sdk\bin\x86_64-windows-msvc\qnn-onnx-converter" `
    -i "$onnx" `
    -o "$WorkDir\mobilenetv2.dlc" `
    -d "input" "1,3,224,224" `
    --export_format dlc
```

### 3.4 量化（可选，HTP 加速必备）

```powershell
# 转换时直接量化为 INT8
& $py "$sdk\bin\x86_64-windows-msvc\qairt-converter" `
    -i "$onnx" -o "$WorkDir\mobilenetv2_quantized.dlc" `
    --source_model_input_shape "input:1,3,224,224" `
    --quantize_full

# 或对已有 DLC 单独量化
& $py "$sdk\bin\x86_64-windows-msvc\qairt-quantizer" `
    --input_dlc "$WorkDir\mobilenetv2.dlc" `
    --output_dlc "$WorkDir\mobilenetv2_quantized.dlc"
```

### 3.5 查看 DLC 信息

```powershell
& $py "$sdk\bin\x86_64-windows-msvc\qairt-dlc-info" "$WorkDir\mobilenetv2.dlc"
```

### 3.6 运行时加载

放进 `app/src/main/assets/model.dlc`，由 `qairt_runtime.cpp` 的 `QairtSystemDlc_createFromFile` 加载（见本项目）。

## 4. 路径二：转成 model.so（QNN 模型库）

把模型图定义和权重编译进一个 `.so`，运行时 `dlopen` 加载，**不再发单独的模型文件**。这是 QNN 低层 API 的路径。

### 4.1 第一步：产出 .cpp + .bin

`qnn-onnx-converter` 默认就是产出 QNN 模型源码（`.cpp` + `.bin`），无需额外参数：

```powershell
& $py "$sdk\bin\x86_64-windows-msvc\qnn-onnx-converter" `
    -i "$onnx" `
    -o "$WorkDir\mobilenetv2" `
    -d "input" "1,3,224,224"
# 产物：mobilenetv2.cpp + mobilenetv2.bin
```

- `-o` 给的是**基础名**（不带扩展名），转换器会自动追加 `.cpp` / `.bin`
- 若要显式控制：`--export_format cpp`（默认值，可省略）

> 也可加 `--exclude_named_tensors` 减小最终 `.so` 体积（部署推荐）。

### 4.2 第二步：编译成 libmodel.so

`qnn-model-lib-generator` 用 clang++ 把 `.cpp` + `.bin` 编译成共享库：

```powershell
& $py "$sdk\bin\x86_64-windows-msvc\qnn-model-lib-generator" `
    -c "$WorkDir\mobilenetv2.cpp" `
    -b "$WorkDir\mobilenetv2.bin" `
    -l "mobilenetv2" `
    -t "aarch64-android" `
    -o "$WorkDir\lib"
# 产物：$WorkDir\lib\libmobilenetv2.so
```

参数说明：

| 参数 | 含义 |
|------|------|
| `-c / --cpp` | QNN 模型 `.cpp` 路径（必填） |
| `-b / --bin` | QNN 模型 `.bin` 路径（权重；若 .cpp 引用而未提供，运行时会失败） |
| `-l / --lib_name` | 输出库名，生成 `lib<name>.so` |
| `-t / --lib_targets` | 构建目标，可多次指定 |
| `-o / --output_dir` | 输出目录 |
| `--debug_build` | 构建调试版 |

**常用构建目标**（从源码 `available_targets` 提取）：

| 目标别名 | 用途 |
|---------|------|
| `aarch64-android` | **Android 手机（arm64-v8a）——本项目用这个** |
| `aarch64-ubuntu-gcc11.2` / `aarch64-ubuntu-gcc7.5` | Linux arm64 服务器/板子 |
| `aarch64-qnx` / `aarch64-qnx800` | QNX 车载 |
| `x86_64-linux-clang` | x86 Linux |
| `windows-x86_64` / `windows-aarch64` | Windows |

可同时构建多目标：`-t aarch64-android -t aarch64-ubuntu-gcc11.2`。

### 4.3 运行时加载（QNN 低层 API）

```cpp
// dlopen libmobilenetv2.so，取出 QnnModel_getFactoryV1 等入口
// → 构建图 → 创建 context → 执行推理
// 需要 libQnnSystem.so + libQnnCpu/Gpu/Htp.so 等后端库配合
```

- 把 `libmobilenetv2.so` 放进 `app/src/main/jniLibs/arm64-v8a/`
- native 层从 Easy API 切到 QNN 低层 API（`QnnInterface_getInterface` + model library 加载 + context 构建）
- 真正执行仍需对应后端 `.so`（`libQnnCpu/Gpu/Htp.so`）

## 5. 路径三：转成 context binary .bin（最快加载）

在 `libmodel.so` 基础上，结合具体后端 `.so` 预编译成序列化执行上下文，运行时直接加载、跳过图构建，**启动延迟最低**。代价：强绑定 SoC 与后端版本，跨设备不通用。

### 5.1 生成 context binary

```powershell
& "$sdk\bin\x86_64-windows-msvc\qnn-context-binary-generator.exe" `
    --model "$WorkDir\lib\libmobilenetv2.so" `
    --backend "$sdk\lib\aarch64-android\libQnnHtp.so" `
    --binary_file "mobilenetv2.bin" `
    --output_dir "$WorkDir\bin"
# 产物：$WorkDir\bin\mobilenetv2.bin
```

参数说明（来自 `--help`）：

| 参数 | 含义 |
|------|------|
| `--model` | 输入的 `lib<name>.so`（路径三的输入正是路径二的产物） |
| `--backend` | 后端 `.so`，如 `libQnnHtp.so` / `libQnnGpu.so` / `libQnnCpu.so` |
| `--binary_file` | 输出 `.bin` 文件名（带 `.bin` 扩展） |
| `--output_dir` | 输出目录，默认 `./output` |
| `--model_prefix` | `.so` 中模型函数前缀，默认 `QnnModel` |
| `--op_packages` | 自定义算子包（如有） |
| `--config_file` | JSON 配置（后端扩展、context 优先级等） |
| `--set_output_tensors` | 额外导出中间张量 |

多图场景：`--model` 支持逗号分隔多个 `.so`：`libA.so,libB.so`。

### 5.2 重要限制

- **HTP 后端的 context binary 通常需在目标 SoC 上（或带 skel 的环境）生成**，纯 host 跨平台生成 CPU/GPU 的 context binary 较常见，HTP 需 `libQnnHtpV*Stub.so` + skel 配合。
- 生成的 `.bin` **只能在与生成时相同的 SoC 型号 + 后端版本上运行**，换机型需重新生成。
- 仍需对应后端 `.so` 在运行时配合加载。

### 5.3 运行时加载（QNN context binary API）

```cpp
// 加载 .bin 字节流 → QnnContext_createFromBinary() → 直接 execute()
// 跳过图构建，启动最快
```

---

## 6. 三种产物对比与选择

| 维度 | DLC | libmodel.so | context binary .bin |
|------|-----|-------------|---------------------|
| 形态 | 模型文件 | 共享库 | 预编译二进制 |
| 生成步骤 | 1 步（转换器） | 2 步（转换+编译） | 3 步（转换+编译+预编译） |
| 加载速度 | 中（解析 DLC） | 较快（dlopen） | 最快（直接反序列化 context） |
| 跨设备/SoC | ✅ 同架构通用 | ✅ 按架构构建后通用 | ❌ 强绑定 SoC+后端 |
| 是否需后端 .so | 是 | 是 | 是 |
| 发模型文件 | 是 | 否（编进 .so） | 是（.bin） |
| API 路径 | Easy API / SNPE / QNN | QNN 低层 | QNN context binary |
| 控制力 | 低 | 高（可逐算子构图） | 中 |
| 适用场景 | 跨设备、Easy API、本项目 | 不发模型文件、低层调优 | 固定机型、极致启动延迟 |

**选型建议**：

- **本项目 / 一般部署** → DLC（`qairt-converter`，最省事）
- **不想发模型文件 / 要 QNN 低层控制** → `libmodel.so`（`qnn-model-lib-generator`）
- **量产固定机型、追求毫秒级启动** → context binary（`qnn-context-binary-generator`）

---

## 7. 完整流水线图

```
                        mobilenetv2-12.onnx
                                │
            ┌───────────────────┼───────────────────┐
            ▼                   ▼                   ▼
     qairt-converter     qnn-onnx-converter    qnn-onnx-converter
     snpe-onnx-to-dlc    (--export_format      (默认，不加 export_format)
            │                dlc)                      │
            │                   │                       ▼
            │                   │              mobilenetv2.cpp + .bin
            │                   │                       │
            ▼                   ▼                       ▼
      mobilenetv2.dlc      mobilenetv2.dlc        qnn-model-lib-generator
            │                   │                       │
            │                   │                       ▼
            │                   │                libmobilenetv2.so
            │                   │                       │
            ▼                   ▼                       ▼
     QAIRT Easy API      QNN(libQnnModelDlc)    QNN 低层 API(dlopen)
     / SNPE                                            │
                                                       ▼
                                        qnn-context-binary-generator
                                        --model=libmobilenetv2.so
                                        --backend=libQnnHtp.so
                                                       │
                                                       ▼
                                                mobilenetv2.bin
                                                       │
                                                       ▼
                                        QNN context binary API
```

---

## 8. 一键脚本（DLC 路径）

本项目 `docs/convert_model.ps1` 已封装 DLC 路径的下载+转换+拷贝：

```powershell
.\docs\convert_model.ps1 -ModelName mobilenetv2          # 默认即 mobilenetv2
.\docs\convert_model.ps1 -ModelName mobilenetv2 -Quantize # 量化版
```

`model.so` / context binary 路径可参照本文第 4、5 节手动执行，或在此基础上扩展。

---

## 9. 常见问题

### Q: `qnn-onnx-converter` 报 `No module named 'qti'` / `pandas`

未 source `envsetup.ps1`，或 Python 版本不是 3.10。见第 2 节。`qnn-onnx-converter` 比 `qairt-converter` 多依赖 `pandas`。

### Q: `qnn-onnx-converter` 默认产出是 `.cpp` 不是 `.dlc`

这是设计如此。QNN 路径默认产出模型源码供 `qnn-model-lib-generator` 编译。要 DLC 加 `--export_format dlc`，或直接用 `qairt-converter` / `snpe-onnx-to-dlc`。

### Q: `qnn-model-lib-generator` 报找不到 clang

Windows 上需要 `clang++`。SDK 自带工具链通常已配置；若未配置，安装 LLVM 或设置 `CLANG` 环境变量指向 `clang++` 路径。

### Q: context binary 在 A 设备生成、B 设备不能跑

正常。context binary 强绑定 SoC+后端版本，每类目标机型需各自生成。

### Q: 三种产物能在非高通设备上跑吗

不能。无论哪种产物，运行时都要 `libQnn*`/`libQairt*` 后端库，而后端库内置平台校验（`QNN_COMMON_ERROR_PLATFORM_NOT_SUPPORTED`），非高通 SoC 一律拒绝。详见 `README.md` 的"常见问题"。

### Q: PyTorch 模型怎么转

`qairt-converter` 支持 `pytorch/torch` 输入；也可先 `torch.onnx.export` 导出 ONNX 再走本文流程（更稳）。

### Q: 如何查看某产物支持哪些算子 / 是否可转换

```powershell
# 不实际转换，只检查支持情况
& $py "$sdk\bin\x86_64-windows-msvc\qnn-onnx-converter" -i "$onnx" --dry_run
```



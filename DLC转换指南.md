# DLC 模型获取与转换指南

本文档介绍如何获取 QAIRT/QNN 所需的 `.dlc` 模型文件，以及如何将 ONNX/TensorFlow/TFLite/PyTorch 模型转换为 DLC 格式。

## 目录

1. [DLC 是什么](#1-dlc-是什么)
2. [获取 DLC 的三种途径](#2-获取-dlc-的三种途径)
3. [转换工具一览](#3-转换工具一览)
4. [快速开始：一键脚本](#4-快速开始一键脚本)
5. [手动转换示例](#5-手动转换示例)
6. [量化（减小体积 + 加速）](#6-量化减小体积--加速)
7. [推荐的小型模型（< 500 MB）](#7-推荐的小型模型--500-mb)
8. [常见问题](#8-常见问题)

---

## 1. DLC 是什么

DLC（Deep Learning Container）是 Qualcomm AI Runtime（QAIRT/SNPE）的专有模型格式，类似 ONNX 但针对高通硬件优化：

- **二进制容器**：包含网络结构 + 权重 + 量化参数
- **运行时加载**：Android 端通过 `QairtSystemDlc_createFromFile()` 加载
- **后端无关**：同一份 DLC 可在 CPU/GPU/HTP 后端运行
- **支持量化**：INT8/INT16 量化后体积可缩小 4×~8×

> QNN 原生格式是 `.bin` + `.cpp`（需 `qnn-model-lib-generator` 编译为 `.so`），而 QAIRT Easy API 使用 DLC。本 Demo 使用 DLC 路径。

---

## 2. 获取 DLC 的三种途径

### 途径 A：使用现成 DLC（最快）

从以下来源直接下载已转换的 DLC：

| 来源 | 说明 |
|------|------|
| Qualcomm AI Hub | https://aihub.qualcomm.com 提供预转换的 DLC |
| SNPE/QAIRT 示例 | SDK 的 `examples/SNPE/android/` 中可能含 DLC |
| 团队内部 | 已有项目共享的 `.dlc` 文件 |

获取后重命名为 `model.dlc` 放入 `app/src/main/assets/` 即可。

### 途径 B：从 ONNX/TensorFlow 转换（推荐）

使用 SDK 自带转换器，详见下文 [§4 一键脚本](#4-快速开始一键脚本) 与 [§5 手动转换](#5-手动转换示例)。

### 途径 C：训练后导出

自己训练的模型，先导出为 ONNX/TFLite，再用途径 B 转为 DLC：

```python
# PyTorch 导出 ONNX
torch.onnx.export(model, dummy_input, "model.onnx",
                  input_names=["input"], output_names=["output"],
                  dynamic_axes={"input": {0: "batch"}, "output": {0: "batch"}})
```

---

## 3. 转换工具一览

SDK 路径：`F:\AI\sdk\qairt\2.48.40.260702\bin\x86_64-windows-msvc\`

> **注意**：Windows 端多数转换器是 **Python 脚本**（无 `.exe` 后缀），需先安装 Python 3 + 依赖。

### 推荐工具：`qairt-converter`（统一转换器）

单一工具支持所有框架，自动识别输入格式：

```bash
qairt-converter --input_network model.onnx --output_path model.dlc
qairt-converter --input_network model.pb   --output_path model.dlc \
    --source_model_input_shape input 1,224,224,3 \
    --out_tensor_node InceptionV3/Predictions/Reshape_1
```

### 传统 SNPE 工具（仍可用）

| 工具 | 源格式 |
|------|--------|
| `snpe-onnx-to-dlc` | ONNX |
| `snpe-tensorflow-to-dlc` | TensorFlow frozen graph (.pb) |
| `snpe-tflite-to-dlc` | TFLite（**仅 Linux**） |
| `snpe-pytorch-to-dlc` | PyTorch（**仅 Linux**） |

### QNN 框架转换器（生成 `.cpp`/`.bin`，非 DLC）

| 工具 | 说明 |
|------|------|
| `qnn-onnx-converter` | ONNX → QNN 模型源码 |
| `qnn-tensorflow-converter` | TF → QNN 模型源码 |
| `qnn-model-lib-generator` | 把 `.cpp`+`.bin` 编译为 `.so` |

> QNN 路径生成的是 `.so` 模型库（配合 `qnn-net-run` 使用），不是 DLC。本 Demo 用的是 DLC 路径。

### 辅助工具

| 工具 | 用途 |
|------|------|
| `qairt-dlc-info` | 查看 DLC 结构、算子列表、张量信息 |
| `qairt-dlc-diff` | 比较两个 DLC 的差异 |
| `qairt-dlc-to-json` | DLC 转为可读 JSON |
| `snpe-dlc-quant.exe` | 训练后量化（INT8） |
| `snpe-dlc-graph-prepare.exe` | HTP 离线图缓存准备 |
| `qairt-dlc-prepare.exe` | DLC 预处理（含量化） |

---

## 4. 快速开始：一键脚本

项目根目录提供了 `convert_model.ps1`，自动完成下载 + 转换 + 拷贝：

```powershell
# 默认：MobileNet v2（~14 MB），适合绝大多数 Android 设备
.\convert_model.ps1

# SqueezeNet 1.1（~5 MB，最小）
.\convert_model.ps1 -ModelName squeezenet

# ResNet-18（~45 MB，精度更高）
.\convert_model.ps1 -ModelName resnet18

# 量化为 INT8（体积再减 4×，HTP 后端加速）
.\convert_model.ps1 -ModelName mobilenetv2 -Quantize

# 指定 SDK 路径
.\convert_model.ps1 -SdkPath "F:\AI\sdk\qairt\2.48.40.260702"
```

脚本完成后，DLC 会自动拷贝到 `app/src/main/assets/model.dlc`，并生成 `model_meta.json` 记录输入/输出张量信息。

---

## 5. 手动转换示例

### 5.1 环境准备

> **重要**：QAIRT SDK 2.48.40 的原生 Python 扩展（`libDlModelToolsPy310.pyd` / `libDlModelToolsPy312.pyd`）
> 实际都链接到 `python310.dll`，因此**必须使用 Python 3.10**（Python 3.12 的 .pyd 也依赖 python310.dll，无法在 3.12 解释器下加载）。
> 同时 `onnx` 必须用 **1.15.0**（1.16+ 移除了 `onnx.version` 属性，会导致转换崩溃），`numpy` 用 **1.26.4**。

```powershell
# 1. 安装 Python 3.10（推荐 3.10.11）
#    下载地址：https://www.python.org/downloads/release/python-31011/
#    安装到 D:\tools\ProgramFile\Python\Python310\（脚本默认路径）

# 2. 安装依赖（必须用指定版本，否则转换会失败）
D:\tools\ProgramFile\Python\Python310\python.exe -m pip install `
    "numpy==1.26.4" "onnx==1.15.0" protobuf pyyaml scipy packaging attrs tabulate six decorator jsonschema

# 3. 配置 QAIRT 环境
. "F:\AI\sdk\qairt\2.48.40.260702\bin\envsetup.ps1"

# 4. 验证转换器可用（需指定 Python 3.10，并设置 UTF-8 输出避免 GBK 编码崩溃）
$env:PYTHONIOENCODING = "utf-8"
D:\tools\ProgramFile\Python\Python310\python.exe `
    "F:\AI\sdk\qairt\2.48.40.260702\bin\x86_64-windows-msvc\qairt-converter" --help
```

### 5.2 ONNX → DLC（最简单）

```powershell
# 下载一个 ONNX 模型（示例：MobileNet v2）
curl -L -o mobilenetv2.onnx `
  https://github.com/onnx/models/raw/main/validated/vision/classification/mobilenet/model/mobilenetv2-12.onnx

# 转换（一条命令）—— 必须用 python 3.10 显式调用脚本
$env:PYTHONIOENCODING = "utf-8"
D:\tools\ProgramFile\Python\Python310\python.exe `
    "F:\AI\sdk\qairt\2.48.40.260702\bin\x86_64-windows-msvc\qairt-converter" `
    --input_network mobilenetv2.onnx `
    --output_path model.dlc `
    --source_model_input_shape input 1,3,224,224

# 查看结果
D:\tools\ProgramFile\Python\Python310\python.exe `
    "F:\AI\sdk\qairt\2.48.40.260702\bin\x86_64-windows-msvc\qairt-dlc-info" `
    --input_dlc model.dlc
```

### 5.3 TensorFlow → DLC

```powershell
# 下载 InceptionV3 frozen graph
curl -L -o inception_v3_frozen.pb.tar.gz `
  https://storage.googleapis.com/download.tensorflow.org/models/inception_v3_2016_08_28_frozen.pb.tar.gz
tar -xzf inception_v3_frozen.pb.tar.gz

# 转换（需指定输入形状与输出节点）
qairt-converter `
    --input_network inception_v3_2016_08_28_frozen.pb `
    --source_model_input_shape input 1,299,299,3 `
    --out_tensor_node InceptionV3/Predictions/Reshape_1 `
    --output_path model.dlc
```

### 5.4 TFLite → DLC（需 Linux/WSL）

```bash
# Windows 缺少 qnn-tflite-converter，需在 WSL2/Ubuntu 中执行
qairt-converter --input_network model.tflite --output_path model.dlc
```

### 5.5 PyTorch → DLC（需 Linux/WSL）

```bash
# PyTorch 转换需先导出 ONNX，再转 DLC（更稳定）
python -c "
import torch, torchvision
model = torchvision.models.mobilenet_v2(pretrained=True).eval()
dummy = torch.randn(1, 3, 224, 224)
torch.onnx.export(model, dummy, 'mobilenetv2.onnx',
                  input_names=['input'], output_names=['output'])
"
qairt-converter --input_network mobilenetv2.onnx --output_path model.dlc
```

---

## 6. 量化（减小体积 + 加速）

### 训练后量化（PTQ）

```powershell
# 方式 1：转换时直接量化（需校准数据列表）
qairt-converter `
    --input_network model.onnx `
    --output_path model_quantized.dlc `
    --quantize_full `
    --input_list calibration_list.txt

# 方式 2：先转浮点 DLC，再单独量化
snpe-dlc-quant.exe `
    --input_dlc model.dlc `
    --input_list calibration_list.txt `
    --output_dlc model_quantized.dlc
```

### 校准数据

`calibration_list.txt` 是一个文本文件，每行一个输入 raw 文件路径：

```
data/img1.raw
data/img2.raw
data/img3.raw
```

raw 文件是预处理后的扁平二进制（如 `1x3x224x224` 的 float32）。可用 Python 生成：

```python
import numpy as np
from PIL import Image
img = Image.open("img.jpg").resize((224, 224))
arr = np.ascontiguousarray(img, dtype=np.float32).transpose(2, 0, 1) / 255.0
arr.tofile("img1.raw")
```

### 量化效果

| 模型 | 浮点 DLC | INT8 量化 DLC | 加速比（HTP） |
|------|----------|---------------|----------------|
| MobileNet v2 | ~14 MB | ~3.5 MB | 3-5× |
| InceptionV3 | ~95 MB | ~24 MB | 4-6× |
| ResNet-18 | ~45 MB | ~11 MB | 3-5× |

---

## 7. 推荐的小型模型（< 500 MB）

| 模型 | 源格式 | 源大小 | 量化 DLC | 任务 | 下载 |
|------|--------|--------|----------|------|------|
| **SqueezeNet 1.1** | ONNX | ~5 MB | ~1.5 MB | 图像分类 1000 类 | [onnx/models](https://github.com/onnx/models/tree/main/validated/vision/classification/squeezenet) |
| **MobileNet v2** | ONNX | ~14 MB | ~3.5 MB | 图像分类 1000 类 | [onnx/models](https://github.com/onnx/models/tree/main/validated/vision/classification/mobilenet) |
| **ResNet-18** | ONNX | ~45 MB | ~11 MB | 图像分类 1000 类 | [onnx/models](https://github.com/onnx/models/tree/main/validated/vision/classification/resnet) |
| **EfficientNet-Lite0** | TFLite | ~13 MB | ~4 MB | 图像分类 | [TF Hub](https://tfhub.dev/google/lite-model/efficientnet/lite0) |
| **InceptionV3 量化** | TF | ~95 MB | ~24 MB | 图像分类 | SDK `examples/Models/InceptionV3/` |
| **SESR** | ONNX | ~1 MB | ~0.3 MB | 超分辨率 | SDK `examples/Models/SESR/` |
| **word_rnn** | TF | <1 MB | <1 MB | 字符级 RNN | SDK `examples/Models/word_rnn/` |
| **spoken_digit** | TF | <5 MB | <2 MB | 语音数字识别 | SDK `examples/Models/spoken_digit/` |

**推荐选择**：
- **最快上手**：`.\convert_model.ps1 -ModelName squeezenet`（5 MB，秒级转换）
- **平衡选择**：`.\convert_model.ps1 -ModelName mobilenetv2`（14 MB，精度好）
- **演示 HTP 性能**：`.\convert_model.ps1 -ModelName mobilenetv2 -Quantize`（3.5 MB INT8）

---

## 8. 常见问题

### Q: 转换报错 `ModuleNotFoundError: No module named 'onnx'` / `'yaml'` / `'packaging'`

转换器是 Python 脚本，需安装依赖（**必须用 Python 3.10**，版本要严格匹配）：
```powershell
D:\tools\ProgramFile\Python\Python310\python.exe -m pip install `
    "numpy==1.26.4" "onnx==1.15.0" protobuf pyyaml scipy packaging attrs tabulate
```

### Q: 转换报错 `ImportError: DLL load failed while importing libDlModelToolsPy312`

SDK 的原生扩展 `.pyd`（包括 Py312 变体）实际都链接到 `python310.dll`。
**解决**：改用 Python 3.10 运行转换器，不要用 Python 3.12/3.13/3.14。

### Q: 转换报错 `AttributeError: module 'onnx' has no attribute 'version'`

`onnx` 版本太新（≥1.16）。**解决**：降级到 `onnx==1.15.0`。

### Q: 转换报错 `UnicodeEncodeError: 'gbk' codec can't encode character`

中文 Windows 控制台默认 GBK 编码，转换器打印日志崩溃。
**解决**：运行前设置 `$env:PYTHONIOENCODING = "utf-8"`（脚本已自动设置）。

### Q: 转换报错 `Missing command line inputs for dynamic inputs`

ONNX 模型有动态输入维度，需用 `--source_model_input_shape <name> <shape>` 显式指定，例如：
```
--source_model_input_shape input 1,3,224,224
```

运行 SDK 自带的依赖检查：
```powershell
. "F:\AI\sdk\qairt\2.48.40.260702\bin\check-windows-dependency.ps1"
```

### Q: Windows 上无法转换 TFLite/PyTorch 模型

Windows 端缺少 `qnn-tflite-converter` 和 `qnn-pytorch-converter`。解决方案：

1. **改用 ONNX 中转**：先把 TFLite/PyTorch 转 ONNX，再用 `qairt-converter` 转 DLC
2. **使用 WSL2**：在 Ubuntu 中安装 Linux 版 SDK，工具最全

### Q: 下载模型失败（GitHub 大文件）

GitHub release 下载可能较慢，可：
1. 使用代理/镜像
2. 手动从浏览器下载，放到 `build/model_convert/` 目录后重跑脚本（脚本会跳过已存在的文件）

### Q: 转换后的 DLC 在 App 中加载失败

1. 用 `qairt-dlc-info --input_dlc model.dlc` 检查 DLC 是否有效
2. 确认 DLC 放在 `app/src/main/assets/model.dlc`
3. 查看 logcat（过滤 `QnnDemo`）获取详细错误
4. 某些算子可能不被 CPU 后端支持，尝试切换 GPU 后端

### Q: 如何查看 DLC 的输入/输出张量信息

```powershell
qairt-dlc-info --input_dlc model.dlc
```

输出会列出所有图的输入/输出张量名、维度、数据类型。本 Demo 的 App 加载 DLC 后也会在「图与张量信息」区显示这些内容。

### Q: 如何转换自己训练的模型

1. 用 PyTorch/TensorFlow 训练模型
2. 导出为 ONNX（推荐）或 frozen graph
3. 用 `qairt-converter` 转 DLC
4. 可选：量化

详见 [§5 手动转换示例](#5-手动转换示例)。

---

## 参考文档

- SDK 转换文档：`F:\AI\sdk\qairt\2.48.40.260702\docs\QAIRT-Docs\SNPE\general\qairt_converter.html`
- ONNX 转换：`docs\QAIRT-Docs\SNPE\general\model_conv_onnx.html`
- TensorFlow 转换：`docs\QAIRT-Docs\SNPE\general\model_conv_tensorflow.html`
- 量化：`docs\QAIRT-Docs\SNPE\general\qairt_quantizer.html`
- 支持的 ONNX 算子：`docs\QAIRT-Docs\SNPE\general\supported_onnx_ops.html`

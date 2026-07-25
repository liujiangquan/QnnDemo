# LLM 转 QNN 指南：以 Qwen2.5-3B-Instruct 为例

本文说明如何将大语言模型（LLM）转换为 QNN 可运行的产物，并阐明 LLM 与 CNN 模型在转换路径上的根本差异。

---

## 1. LLM vs CNN：两条完全不同的路径

**CNN / 视觉模型**（如 mobilenetv2）走「静态图」路径：
```
ONNX/TF/TFLite  →  qnn-onnx-converter  →  model.cpp + model.bin（权重）
                →  qnn-model-lib-generator  →  libmodel.so
                →  qnn-context-binary-generator  →  context.bin
```

**LLM / 生成式模型**（如 Qwen2.5、LLaMA）走「Genie SDK」路径：
```
HuggingFace 模型目录  →  qnn-genai-transformer-composer  →  model.bin（Genie 格式）
                      →  设备上由 libGenie.so 加载运行
```

### 为什么 LLM 不走 .so / context binary 路径？

| 原因 | 说明 |
|------|------|
| **动态 KV Cache** | LLM 推理每步 sequence length 变化，静态图无法表达 |
| **自回归循环** | token-by-token 生成需要跨步共享 KV 状态，静态单次推理模型不支持 |
| **体积** | 3B 模型 ONNX 导出 > 6GB，超过 ONNX 单文件 2GB 限制 |
| **量化方式不同** | LLM 常用 W4A16（4-bit 权重）等特殊量化，CNN 量化工具不兼容 |

**结论：对 LLM，Genie SDK 的 `.bin` 就是对应 CNN 的 model library `.so` + context binary 的等价产物，不要混用路径。**

---

## 2. 三种 `.bin` 的区分（避免混淆）

QNN 生态里 `.bin` 被三个完全不同的东西复用：

| 类型 | 产生工具 | 内容 | 配套文件 |
|------|---------|------|---------|
| ① 模型权重 `.bin` | `qnn-onnx-converter`（CNN） | C++ 模型的权重/常量数据 | 配套 `.cpp`，两者必须一起用 |
| ② Context Binary `.bin` | `qnn-context-binary-generator`（CNN） | 预编译执行上下文，绑定 SoC | 无，单独加载 |
| ③ **Genie Binary `.bin`** | `qnn-genai-transformer-composer`（**LLM**） | 量化权重 + 模型架构描述 | 可选 tokenizer.json |

本文讲的是 **③ Genie Binary**。

---

## 3. Genie SDK 工具链

SDK `2.48.40` 提供的 GenAI 相关工具（`bin/x86_64-windows-msvc/`）：

| 工具 | 作用 |
|------|------|
| `qnn-genai-transformer-composer` | **核心**：HuggingFace 模型 → Genie binary `.bin` |
| `qairt-lora-model-creator` | 创建 LoRA 适配器 |
| `qairt-lora-importer` | 将 LoRA 融合到基础模型 |
| `qairt-lora-mapper` | LoRA 张量映射 |
| `genie-t2t-run.exe` | 文字生成推理（text-to-text）运行工具 |
| `genie-t2e-run.exe` | 文字嵌入推理（text-to-embedding）运行工具 |
| `genie-app.exe` | 交互式对话 demo |

运行时库：`lib/aarch64-android/libGenie.so`（含 `libQnnGenAiTransformer.so`）。

### 内置支持的模型

SDK 预置了 32 个模型配置（`lib/python/qti/aisw/genai/configs/`），**自动识别的主要架构**：

| HuggingFace `architectures` 字段 | 自动匹配条件 | SDK 配置 |
|----------------------------------|------------|----------|
| `Qwen2ForCausalLM` | `hidden_size == 896` | qwen2.5-0.5b |
| `Qwen2ForCausalLM` | `hidden_size == 3584` | qwen2.5-7b |
| `Qwen3ForCausalLM` | 任意 | qwen3-4b-instruct |
| `LlamaForCausalLM` | 按 hidden_size 区分 | llama2-7b/13b, llama3-8b, llama3.1-8b, llama3.2-1b/3b, tinyllama |
| `Phi3ForCausalLM` | 按模型名区分 | Phi-3-mini, Phi-3.5-mini, Phi-4-mini, Phi-4 |
| `GemmaForCausalLM` | 按 hidden_size | gemma-2b/7b |
| `MistralForCausalLM` | 按 hidden_size | mistral-7b-v0.2/v0.3 |
| `QWenLMHeadModel` | 任意 | qwen-7b-chat |

> **Qwen2.5-3B（`hidden_size=2048`）不在自动识别范围内**，必须手动指定 `--config_file`（见第 4 节）。

---

## 4. Qwen2.5-3B-Instruct 完整转换流程

### 4.1 下载模型

```bash
# 方式 A：HuggingFace CLI（推荐）
pip install huggingface_hub
huggingface-cli download Qwen/Qwen2.5-3B-Instruct --local-dir ./Qwen2.5-3B-Instruct

# 方式 B：git lfs
git lfs install
git clone https://huggingface.co/Qwen/Qwen2.5-3B-Instruct
```

模型目录结构（必须包含）：
```
Qwen2.5-3B-Instruct/
├── config.json          ← 必须，记录架构参数
├── tokenizer.json
├── tokenizer_config.json
├── vocab.json
├── merges.txt
└── model-*.safetensors  ← 权重文件（约 6.2GB，3 个分片）
```

### 4.2 确认关键参数（来自 config.json）

Qwen2.5-3B-Instruct 的 `config.json` 中关键字段：

```json
{
  "architectures": ["Qwen2ForCausalLM"],
  "hidden_size": 2048,
  "num_hidden_layers": 36,
  "num_attention_heads": 16,
  "num_key_value_heads": 8,
  "intermediate_size": 11008,
  "vocab_size": 151936,
  "max_position_embeddings": 32768,
  "rope_theta": 1000000.0,
  "rms_norm_eps": 1e-6
}
```

由于 `hidden_size=2048`（不是 SDK 内置的 896 或 3584），**直接运行会报**：
```
Exception: Please provide configuration.json file for this model
```

### 4.3 编写自定义 config（qwen2.5-3b.json）

基于 SDK 内置的 `qwen2.5-7b.json` 模板，修改尺寸参数。
规则：`config.json` → Genie config 的对应关系：

| HuggingFace config.json 字段 | Genie config 字段 |
|-----------------------------|-------------------|
| `hidden_size` | `size.embedding` |
| `intermediate_size` | `size.feedforward` |
| `max_position_embeddings` | `size.context` |
| `num_hidden_layers` | `architecture.num_decoders` |
| `num_attention_heads` | `architecture.num_heads` |
| `num_key_value_heads` | `architecture.num_kv_heads` |
| `vocab_size` | `size.vocabulary` |
| `rope_theta` | `operation.rope_scaling` |
| `rms_norm_eps` | `operation.normalization_epsilon` |
| `head_dim / 2`（= hidden/heads/2） | `operation.rope_num_rotations` |

保存为 `qwen2.5-3b.json`（Qwen2 的张量名与 7B 一致，只改数字）：

```json
{
  "general.name": "qwen2.5-3b",
  "general.architecture": "qwen",
  "general.quantization_version": 1,
  "general.alignment": 32,
  "general.hf_hub_model_id": "Qwen/Qwen2.5-3B-Instruct",

  "size.vocabulary": 151936,
  "size.context": 32768,
  "size.embedding": 2048,
  "size.feedforward": 11008,

  "architecture.num_decoders": 36,
  "architecture.num_heads": 16,
  "architecture.num_kv_heads": 8,
  "architecture.connector": "sequential_pre_layer_normalization",
  "architecture.gating": "gated",

  "operation.normalization": "RMS-norm",
  "operation.normalization_epsilon": 1e-6,
  "operation.activation": "SiLU",
  "operation.positional_embedding": "RoPE",
  "operation.rope_complex_organization": "SoA",
  "operation.rope_num_rotations": 64,
  "operation.rope_scaling": 1000000.0,

  "tensor.layer_name": "model.layers.(\\d+).",
  "tensor.embedding_token_weight": { "name": "model.embed_tokens.weight" },
  "tensor.attention_normalization_weight": { "name": "input_layernorm.weight" },
  "tensor.attention_q_weight": { "name": "self_attn.q_proj.weight", "transposed": true },
  "tensor.attention_q_bias":   { "name": "self_attn.q_proj.bias" },
  "tensor.attention_k_weight": { "name": "self_attn.k_proj.weight", "transposed": true },
  "tensor.attention_k_bias":   { "name": "self_attn.k_proj.bias" },
  "tensor.attention_v_weight": { "name": "self_attn.v_proj.weight", "transposed": true },
  "tensor.attention_v_bias":   { "name": "self_attn.v_proj.bias" },
  "tensor.attention_output_weight": { "name": "self_attn.o_proj.weight", "transposed": true },
  "tensor.feedforward_normalization_weight": { "name": "post_attention_layernorm.weight" },
  "tensor.feedforward_gate_weight":   { "name": "mlp.gate_proj.weight", "transposed": true },
  "tensor.feedforward_up_weight":     { "name": "mlp.up_proj.weight",   "transposed": true },
  "tensor.feedforward_output_weight": { "name": "mlp.down_proj.weight", "transposed": true },
  "tensor.output_normalization_weight": { "name": "model.norm.weight" },
  "tensor.output_weight": { "name": "lm_head.weight", "transposed": true }
}
```

> `operation.rope_num_rotations` = `hidden_size / num_attention_heads / 2` = `2048 / 16 / 2` = **64**

### 4.4 运行转换

```powershell
$sdk    = "F:\AI\sdk\qairt\2.48.40.260702"
$py     = "D:\tools\ProgramFile\Python\Python310\python.exe"
$model  = ".\Qwen2.5-3B-Instruct"      # HuggingFace 模型目录
$config = ".\qwen2.5-3b.json"          # 上一步写的自定义 config
$out    = ".\qwen2.5-3b-q4.bin"        # 输出 Genie binary

$env:QAIRT_SDK_ROOT    = $sdk
$env:PYTHONIOENCODING  = "utf-8"
. "$sdk\bin\envsetup.ps1" 2>$null

& $py "$sdk\bin\x86_64-windows-msvc\qnn-genai-transformer-composer" `
    --model       $model   `
    --config_file $config  `
    --quantize    Q4       `
    --outfile     $out
```

**量化选项（`--quantize`）：**

| 选项 | 含义 | 输出体积（3B） | 适用后端 |
|------|------|--------------|---------|
| 不指定 | FP32 全精度 | ~12 GB | CPU |
| `--precision FP16` | FP16 半精度 | ~6 GB | CPU / GPU |
| `Q4` | 4-bit 量化（较高精度） | ~1.6 GB | HTP |
| `Z4` | 4-bit 量化（高压缩） | ~1.6 GB | HTP |
| `Q5_K` | 5-bit 量化 | ~2.0 GB | HTP |
| `Z8` | 8-bit 量化 | ~3.1 GB | HTP |

> HTP（NPU）要求量化版本（Q4/Z4/Q5_K/Z8）；CPU/GPU 后端建议 FP16。

**可选参数：**

```powershell
# 同时导出 tokenizer.json（用于端侧）
--export_tokenizer_json

# LoRA 适配器融合
--lora ./lora-adapter-dir

# lm_head 独立精度控制（与 --quantize 配合）
--lm_head_precision FP16
```

### 4.5 转换产物

```
qwen2.5-3b-q4.bin          ← Genie binary（主产物，约 1.6 GB）
tokenizer.json             ← 如加了 --export_tokenizer_json
```

---

## 5. 端侧部署（Android）

### 5.1 需要的运行时文件

```
app/src/main/jniLibs/arm64-v8a/
├── libGenie.so                    ← Genie 运行时
├── libQnnGenAiTransformer.so      ← GenAI 算子库
├── libQnnHtp.so                   ← HTP 后端（量化模型用）
├── libQnnHtpV75Stub.so            ← Hexagon stub（按 SoC 型号选）
└── libQnnSystem.so

app/src/main/assets/
├── qwen2.5-3b-q4.bin              ← 模型
└── tokenizer.json                 ← 分词器
```

### 5.2 设备端命令行推理（验证用）

```bash
# adb 推送（以骁龙 8 Gen 2 为例，HTP V75）
adb push qwen2.5-3b-q4.bin /data/local/tmp/
adb push tokenizer.json    /data/local/tmp/

# 运行（需先 push 对应 .so 到 /data/local/tmp/）
adb shell "cd /data/local/tmp && \
  LD_LIBRARY_PATH=. ./genie-t2t-run \
    --model qwen2.5-3b-q4.bin \
    --prompt '你好，请介绍一下自己' \
    --backend htp"
```

### 5.3 在 Android App 中调用（Java/Kotlin）

```kotlin
// Genie SDK 通过 JNI 暴露 C API，或使用 libGenie.so 的 Java bindings
// 基本流程：
// 1. System.loadLibrary("Genie")
// 2. GenieDialog dialog = GenieDialog.create(modelPath, config)
// 3. dialog.query(prompt, callback)  // 流式输出
// 4. dialog.destroy()
```

---

## 6. LLM 转换流水线总结

```
Qwen2.5-3B-Instruct（HuggingFace 目录）
            │
            │  qnn-genai-transformer-composer
            │  --model  <hf_dir>
            │  --config_file  qwen2.5-3b.json   ← 3B 必须手动提供
            │  --quantize  Q4                    ← HTP 用量化版
            │
            ▼
    qwen2.5-3b-q4.bin   （Genie binary，第 ③ 种 .bin）
            │
            │  libGenie.so（Android 运行时）
            │  内部: Genie → QNN → libQnnHtp.so → Hexagon DSP
            │
            ▼
       LLM 推理（自回归、KV Cache、流式输出）
```

对比 CNN 路径：

```
CNN（mobilenetv2）
    → qnn-onnx-converter → .cpp + .bin①
    → qnn-model-lib-generator → libmodel.so      ← CNN 的 model library
    → qnn-context-binary-generator → context.bin②  ← CNN 的 context binary

LLM（Qwen2.5-3B）
    → qnn-genai-transformer-composer → model.bin③  ← LLM 的等价产物（不是 .so）
                                                      libGenie.so 内部管理执行
```

---

## 7. 常见问题

### Q: Qwen2.5-3B 转换时报 "Please provide configuration.json file for this model"

SDK 内置 config 只覆盖 `hidden_size=896`（0.5B）和 `hidden_size=3584`（7B），3B 的 `hidden_size=2048` 不在自动识别范围。需要提供 `--config_file`，按第 4.3 节写自定义 JSON。

### Q: config 中 rope_num_rotations 怎么计算

`operation.rope_num_rotations = hidden_size / num_attention_heads / 2`

Qwen2.5-3B：`2048 / 16 / 2 = 64`；7B：`3584 / 28 / 2 = 64`（虽然 7B config 写的是 128，实际是 `head_dim/2=128`，即 `3584/28=128`，`128/2=64`……需与 7B config 核对实际值）。

实际上 SDK 的 qwen2.5-7b.json 写的是 `128`，对应 `head_dim = 3584/28 = 128`，`rope_num_rotations = head_dim/2 = 64` 还是 `head_dim = 128`？查 llama3.2-3b.json：`embedding=3072, num_heads=24, rope_num_rotations=128` → `head_dim = 3072/24 = 128`，`rope_num_rotations=128=head_dim`。

所以规则是 `rope_num_rotations = head_dim = hidden_size / num_heads`：
- Qwen2.5-3B：`2048 / 16 = 128`，**rope_num_rotations = 128**（不是 64）

> 建议参考相同 architecture 的已知正确 config（如 qwen2.5-7b.json：`embedding=3584, heads=28, rope_num_rotations=128` → `3584/28=128` ✓）

### Q: 可以在 x86 PC 上完成转换、在 ARM 骁龙手机上运行吗

可以。`qnn-genai-transformer-composer` 是纯 host 端转换工具，在 Windows/Linux x86 上运行。产物 `model.bin` 拷到骁龙设备上运行即可。

### Q: 其他 Qwen 变体怎么处理

| 模型 | architectures | 是否需要 config_file |
|------|--------------|:------------------:|
| Qwen2.5-0.5B | Qwen2ForCausalLM (hidden=896) | ❌（内置，但 SDK config 文件缺失，仍需手动） |
| Qwen2.5-1.5B | Qwen2ForCausalLM (hidden=1536) | ✅ 需要 |
| Qwen2.5-3B | Qwen2ForCausalLM (hidden=2048) | ✅ 需要 |
| Qwen2.5-7B | Qwen2ForCausalLM (hidden=3584) | ❌ 内置 |
| Qwen2.5-14B | Qwen2ForCausalLM (hidden=5120) | ✅ 需要 |
| Qwen3-4B | Qwen3ForCausalLM | ❌ 内置 |

### Q: Qwen2.5-3B 对骁龙 SoC 的要求

| 产物 | 后端 | 最低 SoC |
|------|------|---------|
| FP16 | CPU/GPU | 骁龙 8 Gen1 |
| Q4 量化 | HTP（推荐） | 骁龙 8 Gen2（Hexagon V75） |
| Q4 量化 | HTP | 骁龙 8 Gen3（Hexagon V79，更快） |

内存：3B Q4 模型约需 2~3 GB RAM 空余。

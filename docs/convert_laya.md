# Laya 决策模型 → HTP context binary 转换流水线

把 `convaiinnovations/laya-multilingual`（mmBERT-base 决策模型）转成 **HTP V81 fp16 context binary**（`laya.bin`），供 `QnnDemo` 的 Laya 页面在 8845 上加载。

本文是 **T1 的实施记录**：记录实际跑通的命令、踩到的坑、以及与原计划不同的地方。上层使用说明见 `Laya-使用指南.md`。

> **状态：已跑通。** `laya.bin` = 654,363,400 字节，build ID `v2.48.40.260702151143`，图名 `laya_qnn`。产物在 `D:\workspace\models\laya\out\`（不入 git）。

---

## 0. 关键结论速查

| 项 | 值 | 说明 |
|----|----|----|
| 目标 SDK | **QAIRT 2.48.40.260702** | 必须与 `app/src/main/jniLibs/arm64-v8a/` 里的 HTP 库/build ID 一致 |
| 目标 SoC | HTP **v81**，`soc_id=97`，`soc_model=97` | 8845 |
| PD 模式 | **unsigned** | 与 `qairt_runtime.cpp` 的 `useSignedProcessDomain=false` 一致 |
| **图名** | **`laya_qnn`** | ⚠️ 由 `qnn-onnx-converter` 按输入 ONNX 文件名生成，**不是** `laya` |
| 输入 | `input_ids` INT_32 `[1,512]`、`attention_mask` INT_32 `[1,512]`、`marker_pos` INT_32 `[1,16]`、`marker_mask` **BOOL_8** `[1,16]`、`qtype` INT_32 `[1]` | App 按 `elementBytes` 自适配打包（4 / 1 字节） |
| 输出 | `logits` **FLOAT_16** `[1,16]`、`act_logits` **FLOAT_16** `[1,2]` | ⚠️ **fp16（2 字节）**，App 侧解码需按 2 字节处理，见 §7 |
| 产物 | `laya.bin` 654,363,400 B | 约 654 MB |

---

## 1. 环境

**必须在 WSL 里跑**，不要在 Windows 上跑（Windows 版 QAIRT 的 Python 链缺 DLL，`qairt-converter` / `qnn-onnx-converter` 是 `#!/usr/bin/env python3` 脚本，没有原生 exe）。

```bash
# WSL2 Ubuntu-22.04
# - SDK：D:\workspace\docs\qti\qairt\2.48.40.260702  →  WSL 里为 /mnt/d/workspace/docs/qti/qairt/2.48.40.260702
# - venv：/home/breeze/qairt-venv（Python 3.10.12）
# 依赖：onnx onnxruntime onnxsim numpy tokenizers pandas  + clang（编译 host model lib 用）
```

> **为什么是 2.48 而不是本机 `C:\Qualcomm\AIStack\QAIRT\2.49.40.260810`？**
> 本仓 `jniLibs` 的 HTP 一套是 **SDK 2.48**（`libQnnHtp.so` / `libQnnHtpV81Stub.so` / `libQnnHtpPrepare.so` / `libQnnHtpV81Skel.so` 全部 `v2.48.40.260702`）。
> ctx binary 内嵌的 build ID 必须与设备端 skel 一致，否则 `contextCreateFromBinary` 报 **`0x1388` "newer binary on old SDK"**。
> 本机**没有 2.46 SDK**（列表：2.40 / 2.47.1 / 2.48.40 / 2.49.40），所以路线是「jniLibs 统一升到 2.48 + 离线用 2.48 生成」。

**WSL 传参坑**：`wsl -- bash -lc 'VAR=x; ls $VAR'` 里 `$VAR` 会被外层吃掉。**一律用字面路径**，或在命令前加 `MSYS_NO_PATHCONV=1`。

后面命令统一用两个前缀变量（下文简写为 `$SDK` / `$PY`）：

```bash
SDK=/mnt/d/workspace/docs/qti/qairt/2.48.40.260702
PY=/home/breeze/qairt-venv/bin/python
```

---

## 2. Step 1 —— 固定输入形状 + 简化

源模型：`D:\workspace\models\laya\laya_multilingual.onnx`（1.29 GB，opset 17，producer pytorch 2.11.0，图名 `main_graph`）。

```bash
cd ~/laya_work
$PY -m onnxsim /mnt/d/workspace/models/laya/laya_multilingual.onnx laya_fixed.onnx \
  --overwrite-input-shape input_ids:1,512 attention_mask:1,512 \
    marker_pos:1,16 marker_mask:1,16 qtype:1
```

结果：**节点 3624 → 1220**，输入固定为 `[1,512] / [1,512] / [1,16] / [1,16] / [1]`。

> **坑：`RecursionError`。** onnxsim 会在简化完成后跑一遍 shape 校验，对这张图会在 sympy 的多项式 GCD 里递归爆栈
> （`RecursionError: maximum recursion depth exceeded`）。**输出文件此时已经写好且可用**（本文用的就是它），
> 但想要「干净退出」需要把递归上限调高，例如脚本化调用：
> ```python
> import sys, onnxsim
> sys.setrecursionlimit(1_000_000)
> m = onnxsim.simplify("laya_fixed.onnx")   # 或先 overwrite_input_shapes
> ```

> **反直觉点：`IsNaN` / `Where` 不会被折叠掉。**
> 原计划预期「固定 mask 后 NaN-guard 被常量折叠」。但 `marker_mask` 是**图输入**而非常量，简化后仍残留
> `IsNaN`×22、`Where`×26、`TopK`×1、`Erf`×24。好消息是 **HTP 支持它们**（见 §3 前的说明），无需为此改图。

---

## 3. Step 2 —— 手术掉唯一的 bool `Gather`（原计划 Step 4 回退的实际做法）

### 3.1 问题

第一次 `qnn-context-binary-generator` **失败**：

```
[ ERROR ] <E> QnnBackend_validateOpConfig failed 3110
[ ERROR ] <E> Failed to validate op _encoder_Gather_5 with error 0xc26
[ ERROR ] QnnModel::addNode() validating node _encoder_Gather_5 failed.
```

### 3.2 定位

HTP 的 op 定义补充文档（`$SDK/docs/QAIRT-Docs/QNN/general/htp/htp_opdef_version_history.html`）里：

- `IsNan enabled on FP16` → **IsNaN 支持**，不是它的问题
- `Gather (Activation type: FP16)` → **fp16 Gather 支持**

真正的问题在节点定义（`laya_qnn.cpp`）：

```
Gather( data    = /encoder/Flatten_output_0   BOOL  [512,1]
        indices = /encoder/Add_output_0       INT64 [1,1,1,512]
        axis    = 0 )
   -> /encoder/Gather_5_output_0              BOOL  [1,1,1,512,1]
```

**HTP 的 Gather 只吃整型 activation，不吃 BOOL**（报错里列出的可选 dtype 组合全是 `*_POINT_8` / `UINT_32` / `INT_32`，没有 bool）。全图审计（`bool_audit.py`）确认：**这是整张图唯一的 bool Gather**，其余 bool 用途（`Where` 的 condition、`And`、`IsNaN`、`Cast`）都正常。

### 3.3 关键观察：它是恒等 gather

`/encoder/Add_output_0` 是**常量**，逐值校验后确认它 **正好是 `arange(512)`**：

```
first 12: [ 0  1  2  3  4  5  6  7  8  9 10 11]
is_arange: True
```

index = `arange(n)` 时 Gather 的元素顺序恒等 → 这个节点**等价于一次 Reshape/Broadcast**，与真正的取数无关。

### 3.4 补丁

`/mnt/d/workspace/models/laya/patch_laya_onnx.py`：删除 `/encoder/Gather_5`，把它唯一的消费者 `/encoder/Reshape_1` 的输入直接改接 `/encoder/Flatten_output_0`。

```bash
$PY /mnt/d/workspace/models/laya/patch_laya_onnx.py laya_fixed.onnx laya_fixed_patched.onnx
```

脚本会**先证明**三件事再改图，任一不成立就中止（避免静默改错语义）：
1. 该节点是 `Gather`，且 index 输入是常量
2. index 逐值等于 `arange`
3. 该输出只有唯一消费者

### 3.5 数值等价验证（必须做）

用同一套序列与 tokenizer 分别跑补丁前后两张图，比较 `logits` / `act_logits`：

| case | 补丁前 | 补丁后 |
|------|--------|--------|
| department `logits[:3]` | `[7.157100200653076, -4.501200199127197, -4.570099830627441]` | **完全一致** |
| department `act_logits` | `[1226.020263671875, -1463.57568359375]` | **完全一致** |
| urgency / churn | — | **完全一致** |

**逐位相同**，确认补丁无数值影响。

> **与计划的差异**：计划 Step 4 的回退是「用 `receptron/laya` 的 `export_onnx.py` 从检查点重导出干净图」。
> 实测没必要：那张干净图的输出名是 `act_probs`（已 softmax），会**改掉 App 依赖的 `logits` / `act_logits` 契约**（见 `LayaBackend.decide`），
> 还要额外拉 1.3 GB 检查点 + 装 torch。手术方案改图 2 个字段、保持 IO 契约不变、且已验证数值等价，**优先采用**。
> 若将来又冒出别的 HTP 不支持算子，再考虑整图重导出。

---

## 4. Step 3 —— onnxruntime 黄金输出

`/mnt/d/workspace/models/laya/golden.py`：**严格复刻** Kotlin 侧的序列构造（`LayaSequenceBuilder.build`）与后处理（`LayaPostprocessor`），跑 `laya_fixed_patched.onnx`（CPU fp32），落盘 `golden_ticket.json` 供真机 E2E（`LayaE2ETest`）对照。

```bash
cd ~/laya_work
$PY /mnt/d/workspace/models/laya/golden.py laya_fixed_patched.onnx \
    /mnt/d/workspace/models/laya/tokenizer.json \
    /mnt/d/workspace/models/laya/golden_ticket.json
```

本次结果（**与 App 侧 T12 的期望一致**）：

| case | 类型 | 结果 | 备注 |
|------|------|------|------|
| `department` | choice | **`billing`**，p=0.999983，conf=0.9998 | T12 断言 `choice == "billing"` ✅ |
| `urgency` | score | 期望分 1.752，conf=0.123 | 分布偏平 |
| `churn` | noul | P(true)=0.0737 | 该 state 未提及"取消订阅"，判低合理 |

> `act_logits` 数值极大（±1200~±1800）→ softmax 饱和 → `actProbability` 恒为 `1.0`。

**tokenizer**：`HuggingFace` 上路径是 `tokenizer/tokenizer.json`（不是根目录，根目录 404），34,363,188 B。
特殊 token id：`<bos>`=2、`<eos>`=1、`<mask>`=4、`<pad>`=0、`<unk>`=3，vocab=256000。

**`rl_agent_config.json` 交叉核对**：`temperature: [1,1,1]`、`head_max_len: 256` → 与 `LayaConfig.TEMPERATURE=1.0`、`HEAD_MAX_LEN=256` 一致 ✓
（注意配置里 `max_len: 1024`，而 App 固定 512 —— 这是 HTP 图的静态形状取舍，属已知限制。）

---

## 5. Step 4 —— ONNX → QNN fp16 → HTP context binary

### 5.1 ONNX → QNN 模型（fp16）

```bash
cd ~/laya_work
env PYTHONPATH=$SDK/lib/python LD_LIBRARY_PATH=$SDK/lib/x86_64-linux-clang QNN_SDK_ROOT=$SDK \
  $PY $SDK/bin/x86_64-linux-clang/qnn-onnx-converter \
    --input_network laya_fixed_patched.onnx \
    --float_bitwidth 16 \
    --output_path laya_qnn.cpp
```

产出 `laya_qnn.cpp`（3.8 MB 图定义）+ `laya_qnn.bin`（644 MB 权重，fp16）。

**warning 均可接受**（不报错、不阻塞）：

- `GEMM operation is not supported in the general case, attempting to interpret as FC` ×14 —— 按 FC 解释
- `WARNING_OP_VERSION_NOT_SUPPORTED: Operation Clip ... expected version [1,6,11,12], got [13]` ×3 —— 这 3 个 Clip 的 min/max 是常量，按旧版处理
- `Only numerical type cast is supported ... will be interpreted at conversion time` ×2

> 若这里报**不支持算子**（`ERROR` 而非 `WARNING`），才需要回到 §3 的替代方案或整图重导出。
> 先跑 `--dry_run` 可以 2 分钟拿到算子支持摘要，避免白等正式转换。

### 5.2 编译 host model lib（不需要 NDK）

```bash
$PY $SDK/bin/x86_64-linux-clang/qnn-model-lib-generator \
  -c laya_qnn.cpp -b laya_qnn.bin \
  -t x86_64-linux-clang \
  -o model_libs
```

产出 `model_libs/x86_64-linux-clang/liblaya_qnn.so`（646 MB，权重已内嵌）。

> **为什么是 `x86_64-linux-clang` 而不是计划里的 `aarch64-android`？**
> ctx-gen 是**在 host 上**跑的进程，它 `dlopen` 的这个 model lib **必须是 host 架构**（x86_64），
> 否则加载不了。`-t aarch64-android` 那条路需要 Android NDK 的 `ndk-build`（本机只有 Windows NDK，WSL 里没有），
> 而且那是「在设备端跑 ctx-gen」的路线，与本流程无关——本流程是**离线 host prepare**，**完全不需要 NDK**。

### 5.3 生成 HTP context binary

ctx-gen 的 `--config_file` **只接受 `backend_extensions` 块**（直接写 `graphs`/`devices` 会 `Unknown Key` → Device Creation failure）。
所以用「外层套内层」两份配置（`/mnt/d/workspace/models/laya/cfg/`）：

`htp_ext_outer.json`（传给 `--config_file`）：

```json
{
  "backend_extensions": {
    "shared_library_path": "/mnt/d/workspace/docs/qti/qairt/2.48.40.260702/lib/x86_64-linux-clang/libQnnHtpNetRunExtensions.so",
    "config_file_path": "/mnt/d/workspace/models/laya/cfg/htp_ext_inner.json"
  }
}
```

`htp_ext_inner.json`（真正描述目标 HTP）：

```json
{
  "graphs": [
    { "graph_names": ["laya_qnn"], "fp16_relaxed_precision": 1 }
  ],
  "devices": [
    { "dsp_arch": "v81", "soc_id": 97, "soc_model": 97,
      "cores": [ { "perf_profile": "burst" } ] }
  ]
}
```

> ⚠️ `graph_names` 必须是 **`laya_qnn`**（见 §0）。写错会 Device/Graph 找不到。

```bash
env LD_LIBRARY_PATH=$SDK/lib/x86_64-linux-clang QNN_SDK_ROOT=$SDK \
  $SDK/bin/x86_64-linux-clang/qnn-context-binary-generator \
    --model model_libs/x86_64-linux-clang/liblaya_qnn.so \
    --backend $SDK/lib/x86_64-linux-clang/libQnnHtp.so \
    --binary_file laya \
    --output_dir out \
    --config_file /mnt/d/workspace/models/laya/cfg/htp_ext_outer.json
```

成功时依次走完：`Graph Preparation Initializing` → `Graph Optimizations` → `Post Graph Optimization`
→ `Graph Sequencing for Target`（带进度条）→ `VTCM Allocation` → `Parallelization Optimization`
→ `Finalizing Graph Sequence` → `Completion`。约 50 秒。

产出 `out/laya.bin`（654,363,400 B）。

### 5.4 校验产物元信息

```bash
env LD_LIBRARY_PATH=$SDK/lib/x86_64-linux-clang QNN_SDK_ROOT=$SDK \
  $SDK/bin/x86_64-linux-clang/qnn-context-binary-utility \
    --context_binary out/laya.bin --json_file out/laya_info.json

$PY /mnt/d/workspace/models/laya/dump_ctx_info.py out/laya_info.json
```

必须确认：

```
buildId   : v2.48.40.260702151143     ← 必须等于 jniLibs 里 HTP 库的 build ID
graph     : laya_qnn
  inputs : input_ids INT_32 [1,512] / attention_mask INT_32 [1,512] /
           marker_pos INT_32 [1,16] / marker_mask BOOL_8 [1,16] / qtype INT_32 [1]
  outputs: logits FLOAT_16 [1,16] / act_logits FLOAT_16 [1,2]
```

---

## 6. 产物落位

| 文件 | 路径 | 说明 |
|------|------|------|
| `laya.bin` | `D:\workspace\models\laya\out\laya.bin` | 654 MB，**不入 git** |
| `laya_info.json` | `D:\workspace\models\laya\out\laya_info.json` | ctx binary 元信息（校验 build ID 用） |
| `tokenizer.json` | `D:\workspace\models\laya\tokenizer.json` | 34 MB，来自 HF `tokenizer/tokenizer.json` |
| `golden_ticket.json` | `D:\workspace\models\laya\golden_ticket.json` | fp32 黄金输出 |
| `laya_fixed_patched.onnx` | `D:\workspace\models\laya\` | 打过补丁的固定形状图 |

推到设备：

```bash
docs/setup_laya.sh <laya.bin 路径> <tokenizer.json 路径>
```

---

## 7. ⚠️ 已知集成注意点

1. **输出是 fp16（`FLOAT_16`，2 字节），不是 fp32。**
   `LayaBackend.toFloats()` 目前按 4 字节读（`bb.float`），会解出错误数值。
   修法：按图元信息里 `logits` / `act_logits` 的 `elementBytes`（2 == fp16）分派解码
   （host 侧同样按 `elementBytes` 适配，参考输入侧 `packIntsAs` 的做法）。
   *输入侧没问题*：`packIntsAs` 已按 `elementBytes` 自适应（INT_32→4 字节、BOOL_8→1 字节），与本图声明一致。

2. **HTP fp16 数值漂移。** 设备上是 fp16 relaxed 精度，概率与 fp32 黄金值会有小幅偏差；
   `LayaE2ETest` 用「argmax 落在 billing」这类**定性**断言，不要用严格数值相等。

3. **质量回退风险。** YOLO 的先例里，SDK 2.48/2.49 离线重生成的 ctx binary 相比 vendor 版出现
   检测质量退化（`docs/setup_yolo.sh` 有记录）。Laya 是 BERT 编码器、不涉及 NMS 融合，
   风险低但**必须在真机上确认 `choice=billing`**；若质量异常，先怀疑这里。

4. **形状限制**：`seq=512`、`opts=16` 是编译进图里的静态形状，改尺寸要重新走整条流水线。

5. **换 SoC / 换 SDK 必须重生成**：`soc_id/soc_model/dsp_arch` 与 build ID 都固化在 bin 里。

---

## 8. 复现清单（全部命令）

```bash
# 0) 环境
SDK=/mnt/d/workspace/docs/qti/qairt/2.48.40.260702
PY=/home/breeze/qairt-venv/bin/python
cd ~/laya_work

# 1) 固定形状（注意 RecursionError 坑，见 §2）
$PY -m onnxsim /mnt/d/workspace/models/laya/laya_multilingual.onnx laya_fixed.onnx \
  --overwrite-input-shape input_ids:1,512 attention_mask:1,512 \
    marker_pos:1,16 marker_mask:1,16 qtype:1

# 2) 手术掉 bool Gather（含等价性前置校验）
$PY /mnt/d/workspace/models/laya/patch_laya_onnx.py laya_fixed.onnx laya_fixed_patched.onnx

# 3) 数值等价 + 黄金输出
$PY /mnt/d/workspace/models/laya/golden.py laya_fixed_patched.onnx \
    /mnt/d/workspace/models/laya/tokenizer.json \
    /mnt/d/workspace/models/laya/golden_ticket.json

# 4) ONNX → QNN fp16
env PYTHONPATH=$SDK/lib/python LD_LIBRARY_PATH=$SDK/lib/x86_64-linux-clang QNN_SDK_ROOT=$SDK \
  $PY $SDK/bin/x86_64-linux-clang/qnn-onnx-converter \
    --input_network laya_fixed_patched.onnx --float_bitwidth 16 --output_path laya_qnn.cpp

# 5) host model lib（无需 NDK）
$PY $SDK/bin/x86_64-linux-clang/qnn-model-lib-generator \
  -c laya_qnn.cpp -b laya_qnn.bin -t x86_64-linux-clang -o model_libs

# 6) HTP context binary
env LD_LIBRARY_PATH=$SDK/lib/x86_64-linux-clang QNN_SDK_ROOT=$SDK \
  $SDK/bin/x86_64-linux-clang/qnn-context-binary-generator \
    --model model_libs/x86_64-linux-clang/liblaya_qnn.so \
    --backend $SDK/lib/x86_64-linux-clang/libQnnHtp.so \
    --binary_file laya --output_dir out \
    --config_file /mnt/d/workspace/models/laya/cfg/htp_ext_outer.json

# 7) 校验 build ID / 图名 / dtype
env LD_LIBRARY_PATH=$SDK/lib/x86_64-linux-clang QNN_SDK_ROOT=$SDK \
  $SDK/bin/x86_64-linux-clang/qnn-context-binary-utility \
    --context_binary out/laya.bin --json_file out/laya_info.json
$PY /mnt/d/workspace/models/laya/dump_ctx_info.py out/laya_info.json
```

**辅助脚本**（均在 `D:\workspace\models\laya\`）：

| 脚本 | 用途 |
|------|------|
| `probe_onnx.py` | 打印 ONNX 的输入/输出/算子分布 |
| `bool_audit.py` | 审计 bool 张量流向（定位哪些 op 吃 bool，因为 HTP 的 Gather 不吃 bool） |
| `dump_subgraph.py` | 导出某节点及其上下游（定位失败节点用） |
| `patch_laya_onnx.py` | 删除恒等 bool Gather（§3.4） |
| `golden.py` | onnxruntime 黄金输出（§4） |
| `dump_ctx_info.py` | 打印 ctx binary 的 build ID / 图名 / 张量 dtype |
| `probe_tok.py` | 打印特殊 token id / 编码示例 |

---

## 附录 A：黄金输出脚本（`golden.py`）

严格复刻 Kotlin 侧逻辑，改这里必须同步改
`app/src/main/java/com/breeze/qnn/laya/LayaSequenceBuilder.kt` 与 `LayaPostprocessor.kt`。

```python
"""Laya 决策模型的 onnxruntime 黄金输出生成器。"""
import json
import sys

import numpy as np
import onnxruntime as ort
from tokenizers import Tokenizer

MAX_LEN = 512
MAX_OPTS = 16
HEAD_MAX_LEN = 256
OPT_TOKEN_CAP = 48
TEMPERATURE = 1.0

QTYPE = {"choice": 0, "score": 1, "noul": 2}


class Tok:
    def __init__(self, path):
        self.t = Tokenizer.from_file(path)
        self.bos = self.t.token_to_id("<bos>")
        self.eos = self.t.token_to_id("<eos>")
        self.mask = self.t.token_to_id("<mask>")
        self.pad = self.t.token_to_id("<pad>")

    def encode(self, text):
        if not text:
            return []
        return self.t.encode(text, add_special_tokens=False).ids


def scrub(s):
    return s.replace("<mask>", " ")


def build_sequence(tok, state, type_name, instructions, options):
    """-> (ids, markers)；与 LayaSequenceBuilder.build 一一对应。"""
    head_ids = list(tok.encode(f"{type_name} question: {scrub(instructions)}"))
    opt_ids = [[tok.mask] + tok.encode(" " + scrub(o))[:OPT_TOKEN_CAP] for o in options]

    total = lambda xs: sum(len(x) for x in xs)

    opt_budget = HEAD_MAX_LEN - total(opt_ids)
    if opt_budget < 16:
        per = max(4, (HEAD_MAX_LEN - 16) // max(1, len(opt_ids)))
        opt_ids = [x[:per] for x in opt_ids]
        opt_budget = HEAD_MAX_LEN - total(opt_ids)
    head_ids = head_ids[:max(8, opt_budget)]

    seq = [tok.bos] + head_ids + [tok.eos]
    markers = []
    for o in opt_ids:
        markers.append(len(seq))
        seq.extend(o)
    seq.append(tok.eos)
    room = max(0, MAX_LEN - len(seq) - 1)
    seq.extend(tok.encode(scrub(state))[:room])
    seq.append(tok.eos)

    return seq[:MAX_LEN], [m for m in markers if m < MAX_LEN]


def pack(tok, ids, markers, qtype):
    n = len(ids)
    input_ids = np.zeros((1, MAX_LEN), dtype=np.int64)
    input_ids[0, :n] = ids
    attention = np.zeros((1, MAX_LEN), dtype=np.int64)
    attention[0, :n] = 1
    marker_pos = np.zeros((1, MAX_OPTS), dtype=np.int64)
    k = len(markers)
    if k:
        marker_pos[0, :k] = markers
    marker_mask = np.zeros((1, MAX_OPTS), dtype=bool)
    marker_mask[0, :k] = True
    return {
        "input_ids": input_ids, "attention_mask": attention,
        "marker_pos": marker_pos, "marker_mask": marker_mask,
        "qtype": np.array([qtype], dtype=np.int64),
    }, k


def softmax(z):
    e = np.exp(z - np.max(z))
    return e / e.sum()


def confidence(p):
    k = len(p)
    if k < 2:
        return 1.0
    return float(1.0 + np.sum(p * np.log(np.maximum(p, 1e-12))) / np.log(k))


def decode(type_name, logits, k, act_logits, labels):
    p = softmax(logits[:k] / TEMPERATURE)
    act = softmax(act_logits)[0]
    out = {"actProbability": float(act)}
    if type_name == "choice":
        out.update(choice=labels[int(np.argmax(p))],
                   probabilities={labels[i]: float(p[i]) for i in range(k)},
                   confidence=confidence(p))
    elif type_name == "score":
        out.update(score=float(np.sum(np.arange(k) * p)),
                   probabilities={str(i): float(p[i]) for i in range(k)},
                   legend={str(i): labels[i] for i in range(k)},
                   confidence=confidence(p))
    else:
        out.update(noul=float(p[1]))
    return out


CASES = [
    dict(id="department", type="choice",
         instructions="Which team should handle this ticket?",
         options=["billing: payments, refunds, invoices",
                  "support: product help and bugs",
                  "sales: new purchases and upgrades"],
         state=json.dumps({
             "subject": "Refund not received",
             "body": "I cancelled my subscription two weeks ago and still have not "
                     "received my refund. If not resolved I will dispute with my bank.",
         }, ensure_ascii=False)),
    dict(id="urgency", type="score", instructions="How urgent?",
         options=["not urgent", "somewhat", "urgent", "critical"],
         state="The customer is furious and threatens to leave."),
    dict(id="churn", type="noul",
         instructions="Is the customer likely to cancel?", options=[],
         state="The customer is furious and threatens to leave."),
]


def main(onnx_path, tok_path, out_path):
    tok = Tok(tok_path)
    sess = ort.InferenceSession(onnx_path, providers=["CPUExecutionProvider"])
    in_names = [i.name for i in sess.get_inputs()]
    out_names = [o.name for o in sess.get_outputs()]

    golden = []
    for c in CASES:
        opts = c["options"]
        if c["type"] == "noul":
            labels = ["false", "true"]
            if not opts:
                opts = ["false: no, the statement does not hold",
                        "true: yes, the statement holds"]
        elif c["type"] == "score":
            labels = opts
            opts = [f"level {i}: {t}" for i, t in enumerate(opts)]
        else:
            labels = [o.split(":", 1)[0] for o in opts]

        ids, markers = build_sequence(tok, c["state"], c["type"], c["instructions"], opts)
        feeds, k = pack(tok, ids, markers, QTYPE[c["type"]])
        res = sess.run(out_names, {n: feeds[n] for n in in_names})
        logits = np.asarray(res[out_names.index("logits")]).reshape(-1)
        act = np.asarray(res[out_names.index("act_logits")]).reshape(-1)
        dec = decode(c["type"], logits, k, act, labels)
        print(f"[{c['id']}] -> {json.dumps(dec, ensure_ascii=False)}")

        golden.append(dict(id=c["id"], type=c["type"], qtype=QTYPE[c["type"]],
                           instructions=c["instructions"], options=opts, labels=labels,
                           state=c["state"], ids=ids, markers=markers, k=k,
                           logits=logits.tolist(), act_logits=act.tolist(),
                           expected=dec))

    with open(out_path, "w", encoding="utf-8") as f:
        json.dump(golden, f, ensure_ascii=False, indent=2)
    print(f"写入 {out_path}")


if __name__ == "__main__":
    main(sys.argv[1], sys.argv[2],
         sys.argv[3] if len(sys.argv) > 3 else "golden_ticket.json")
```

（注：仓库外的这个文件另含 `confidence()` 的熵上限保护与更详细的逐 case 打印，逻辑与此一致。）

# NER 使用/验证指南（8845 · BERT 中文敏感信息识别）

> 分支 `llm-qwen3-8845`；BERT-base 中文 NER，**fp16 context binary 直接跑 HTP V81**；
> 实测加载 **371ms**、单句 **17ms**、体积 **205MB**、余弦 0.99697 对齐 ONNX 原模型。

## 一、这是什么

识别中文文本里的敏感实体，两条通路合并：

| 来源 | 类型 | 说明 |
|---|---|---|
| BERT NER 模型 | `PER` `LOC` `ORG` `TIME` | 人名 / 地名 / 机构名 / 时间 |
| 正则规则 | `PHONE` `ID_CARD` `BANK_CARD` `EMAIL` `PLATE` | 手机号 / 身份证 / 银行卡 / 邮箱 / 车牌 |

正则那一半是必需的 —— 模型 README 明确说结构化字段识别不了，且 `ORG`/`TIME` 默认不算敏感信息（UI 里 `TIME` 用灰色弱化）。

区间重叠时**正则优先**（确定性规则比模型可信，比如身份证号里的数字被模型误识别成 LOC）。

## 二、前置

- Snapdragon 8845（SM8480XP / Hexagon V81）已 root，`adb devices` 能看到
- SDK：`C:/Qualcomm/AIStack/QAIRT/2.48.40.260702/`
- 原模型：`D:/workspace/models/bert-base-chinese-ner/`（`shibing624/bert4ner-base-chinese` 的 ONNX 导出，Apache-2.0）
- **WSL 转换环境**（converter 在 Windows 上不可用，详见 `docs/NER-转换环境.md` 或 memory `qairt_converter_wsl_only`）

## 三、三条命令跑通

```bash
# 1. 装 APK
./gradlew :app:installDebug

# 2. 推 205MB fp16 context binary 到 app filesDir
bash docs/setup_bert_ner.sh          # 换机型时用 dlc 兜底：setup_bert_ner.sh dlc

# 3. 跑 5 个 E2E 用例
bash docs/run_e2e_tests.sh ner
```

预期：`OK (5 tests)`，logcat 里能看到

```
用 context binary: bert-ner-fp16.bin
context binary 加载完成: 1 个图
PASS: recognize 28ms entities=[(PER, 马化腾), (LOC, 深圳), (ORG, 腾讯公司)]
PASS: loadModel(HTP) elapsed=371ms
PASS: 34ms / 2 句 = 17ms/句
PASS: 长文本 4 句 11 个实体，offset 全部对齐
```

`NerBackend` 优先找 `bert-ner-fp16.bin`，找不到才回退 `bert-ner-fp32.dlc`。两者精度相同，
但 context binary **与 SoC 绑死**（只能 v81 / soc_model 97），所以 DLC 保留作跨机型兜底。

## 四、UI 手动验证

启动 app → 底部导航 **NER** tab：

- 顶部多行输入框（预填了示例文本，可粘贴自己的）
- Spinner 切后端：`HTP`（~17ms/句）/ `CPU`（~101ms/句，只走 DLC）
- 点「识别」→ 中部按类型高亮原文，底部列实体明细（类型 / 文本 / offset / score / 来源）
- 底栏显示耗时、句数、实体数、当前后端

配色：`PER` 红 / `LOC` 蓝 / `ORG` 绿 / `TIME` 灰 / 正则类紫。

## 五、JVM 单测（无需设备）

```bash
./gradlew :app:testDebugUnitTest --tests 'com.breeze.qnn.ner.*'
```

56 个用例：

| 测试类 | 覆盖 |
|---|---|
| `SentenceSplitterTest` (7) | 标点切句 / 二级降级 / 无标点硬切 / offset 正确性 |
| `WordPieceTokenizerTest` (14) | 中文单字 / 英文 subword / UNK / 截断 / **按名绑定张量** |
| `BioDecoderTest` (10) | 标准 BIO / `I-X` 无前导容错 / 类型切换 / score / label 顺序 |
| `RegexDetectorTest` (12) | 五类正反例 / Luhn / 身份证校验位 / 身份证优先于银行卡 |
| `EntityMergerTest` (8) | 重叠取正则 / 完全包含 / 排序 / 半开区间 |
| `NerGoldenTest` (5) | **精度回归防线** |

`NerGoldenTest` 的 fixture（`app/src/test/resources/ner_golden.json`）里的 logits 与期望实体都来自 PC 侧 onnxruntime 跑 ONNX 原模型。改动 tokenizer 或 BioDecoder 会立刻变红，不需要设备也不需要 DLC。

## 六、实测基线

| 指标 | context binary（默认） | DLC（兜底） |
|---|---|---|
| `loadModel(HTP)` | **371ms** | ~5.3s（含 on-device 图编译） |
| 单句端到端（HTP） | 17~21ms | 18ms |
| 其中 `graphExecute` | 13ms | — |
| 单句端到端（CPU） | 不支持 | ~101ms |
| 体积 | **205MB** fp16 | 388MB fp32 |
| 精度（vs ONNX 原模型） | 余弦 **0.99697** | 余弦 **0.99697** |
| 换机型 | 要重新生成（绑 v81 / soc 97） | 直接用 |

seq_len 都是 128（编译期固定）。两条路径的 logits **逐位相同**（量化实验里 0.71937 == 0.71937 验证过同一结论）。

换 context binary 的收益是**体积对半 + 加载快 14x**；单句耗时在 app 内没有明显变化（shell 里 `qnn-net-run` 量到的 52ms→8ms 差距包含了 net-run 每次重新组图的开销，不是纯推理差）。

## 七、为什么用 fp16 而不是 INT8/INT4

实测量化全线失败：

| 配置 | 精度 | 单句 | 体积 |
|---|---|---|---|
| **fp16 context binary** | ✅ 余弦 0.99697 | 17ms | **205MB** |
| fp32 DLC | ✅ 余弦 0.99697 | 18ms | 388MB |
| W8A16 per-channel | ❌ 预测全塌成 `O` | 55ms | 113MB |
| W8A8 / A16W16 / percentile 标定 | ❌ 同样失败 | — | 97~194MB |
| fp16 **DLC** | ⛔ `composeGraphs` 组不出图（error 1002） | — | — |

关键证据：**位宽 8→16 毫无变化**（0.712 → 0.714）。精度损失必然随位宽改善，完全不动说明是功能性错误，不是量化噪声。所以 INT4 也没有意义。量化还多出 quantize/dequantize 算子，反而慢 1.6x。

排除过的假设：词嵌入粗量化（实际拿到 16-bit）、LayerNorm gamma 8-bit（A16W16 无改善）、标定方法（percentile 更差）、attention_mask/PAD 失效（填满 token 更差）、DLC 与 context binary 两条路差异（输出逐位相同）。

fp16 拿到体积减半的原因是 `fp16_relaxed_precision: 1` 在 **graph-prepare 阶段**把权重烘成 fp16。DLC 路径做不到这点（`QnnSystemDlc_composeGraphs` 吃不下 fp16 DLC），只有 `qnn-context-binary-generator` 这条路能用。

## 八、踩过的坑

### 输入张量必须按**名字**绑定

这份 DLC 的输入顺序是 `input_ids, token_type_ids, attention_mask` —— 跟直觉的 `ids/mask/typeIds` 不同。按位置喂会把 attention_mask 灌进 token_type_ids、把全 0 的 typeIds 灌进 attention_mask，**attention 屏蔽掉所有位置**，输出塌成几乎全 O（"马化腾在深圳创办了腾讯公司"只解出 `(LOC, 圳)`）。

`TokenizedInput.toTensorBytes(inputNames)` 按名映射，`inputNames` 来自 `engine.graphInfos[0].inputs`。未知张量名直接 error 不静默错位。

### HTP 必须走 unsigned PD

`useSignedProcessDomain=true` 会用 vendor 2.46 的 signed skel，跟我们 SDK 2.48 的 stub 跨版本不兼容，`deviceCreate` 报 `0x36b1`(14001)。改 `false` 走 jniLibs 里的 SDK 2.48 unsigned skel，并把 app nativeLibraryDir 排到 `ADSP_LIBRARY_PATH` 最前。

这条也顺带修了 CNN HTP 的 soft-fail（同一根因，现在 `PASS: HTP elapsed=1.741ms`）。

### HTP 首次 execute 输出不可靠

`graphFinalize` 之后第一次 execute 的结果是错的。`loadModel` 内部做一次 warmup 推理并丢弃，避免用户第一次点「识别」看到垃圾。

### `qnn-net-run` / `qairt-quantizer` 默认按 float32 读输入

不加 `--use_native_input_files` 时，int32 的 token id 被当浮点解析 → Gather 索引全 0 → 每个 token 位置输出完全相同的常量。第一版量化就是拿这种垃圾输入标定的。

### context binary 的图元信息是 V3，tensor 元数据是**借来的指针**

两个坑叠在一起，都在 `qairt_runtime.cpp::loadContextBinary`：

1. `QnnSystemContext_GraphInfo_t` 从 context binary 里读出来是 **V3**（DLC 那条路是 V1）。只认 V1 会静默跳过所有图 → 日志 `加载完成: 0 个图`。
2. `Qnn_Tensor_t` 里的 `name` / `dimensions` 是裸指针，指向 systemContext 拥有的内存。`systemContextFree` 之后它们是野指针，读出的 rank/dims 是垃圾，`execute` 按垃圾尺寸分配输出 buffer 直接 `std::bad_alloc` 崩掉。所以 `ownTensorMeta()` 把这些深拷进 `GraphSlot`（用 `deque` 存，`vector` 扩容会让已改好的指针失效）。

## 九、离线转换（重新生成模型时）

### 9.1 ONNX → fp32 DLC（必须在 WSL 里跑，Windows 版 converter 缺 DLL）

```bash
export MSYS_NO_PATHCONV=1
S=/mnt/c/Qualcomm/AIStack/QAIRT/2.48.40.260702
M=/mnt/d/workspace/models/bert-base-chinese-ner

wsl -d Ubuntu-22.04 -- env \
  PYTHONPATH=$S/lib/python \
  LD_LIBRARY_PATH=$S/lib/x86_64-linux-clang \
  QNN_SDK_ROOT=$S \
  ~/qairt-venv/bin/python $S/bin/x86_64-linux-clang/qairt-converter \
  -i $M/onnx/model.onnx -o $M/dlc/bert-ner-fp32.dlc \
  --source_model_input_shape input_ids 1,128 \
  --source_model_input_shape attention_mask 1,128 \
  --source_model_input_shape token_type_ids 1,128
```

不需要量化步骤、不需要 calibration 数据、不需要 backend extension config。

### 9.2 fp32 DLC → fp16 context binary（必须在**设备上**跑）

HTP graph-prepare 需要真实 v81 硬件，所以这一步在设备 shell 里完成：

```bash
export MSYS_NO_PATHCONV=1
SDK=C:/Qualcomm/AIStack/QAIRT/2.48.40.260702
DEV=/data/local/tmp/nerbench

# 依赖：ctx-gen 本体 + HTP 后端 + DLC 模型壳 + backend extension
adb push $SDK/bin/aarch64-android/qnn-context-binary-generator $DEV/qnn-ctx-gen
adb push $SDK/lib/aarch64-android/{libQnnHtp.so,libQnnModelDlc.so,libQnnHtpNetRunExtensions.so} $DEV/lib/
adb push tools/{ctxgen_htp_config.json,htp_ctx_fp16_config.json} $DEV/

adb shell "cd $DEV && chmod +x qnn-ctx-gen && LD_LIBRARY_PATH=$DEV/lib ./qnn-ctx-gen \
    --backend lib/libQnnHtp.so --model lib/libQnnModelDlc.so \
    --dlc_path dlc/bert-ner-fp32.dlc --binary_file ner_fp16_ctx \
    --output_dir ctxout --config_file ctxgen_htp_config.json"

adb pull $DEV/ctxout/ner_fp16_ctx.bin \
    D:/workspace/models/bert-base-chinese-ner/ctx/bert-ner-fp16.bin
```

两个配置文件的坑：

- `htp_ctx_fp16_config.json` 里 `graph_names` 是**必填**的（缺了报 "Valid 'graph_names' must be specified"）
- `devices.cores` 里**不能**写 `core_id`，否则报 "Number of cores requested 2 > available 1"
- `fp16_relaxed_precision: 1` 就是体积减半的开关
- `soc_id` / `soc_model` = 97、`dsp_arch` = v81 —— 换 SoC 要改这三个并重新生成

验证精度：

```bash
# 生成 ONNX golden
python3 tools/ner_onnx_reference.py --onnx $M/onnx/model.onnx \
    --vocab $M/onnx/vocab.txt --testset $M/testset --dump-dir $M/out_onnx
# 跑 DLC 后比对（门槛余弦 >= 0.99 且实体集合一致）
python3 tools/verify_ner_accuracy.py cmp --ref $M/out_onnx --test <dlc输出> --testset $M/testset
```

## 十、文件清单

```
tools/
  gen_ner_calibration.py       标定数据（当前方案不用，保留给未来量化尝试）
  ner_onnx_reference.py        onnxruntime 产 golden logits
  verify_ner_accuracy.py       余弦 + 实体一致性比对
  htp_ctx_fp16_config.json     ctx-gen 的 HTP backend ext（fp16 开关在这里）
  ctxgen_htp_config.json       ctx-gen --config_file 的外层壳
  htp_ner_fp16_config.json     net-run 的 HTP backend ext（fp16 DLC 尝试留档）
  netrun_ner_htp_config.json   net-run --config_file 的外层壳
docs/
  setup_bert_ner.sh            推模型到设备（ctx | dlc）
  NER-使用指南.md              本文档
app/src/main/assets/ner_vocab.txt          21128 词表
app/src/main/java/com/breeze/qnn/ner/      7 个类
app/src/main/java/com/breeze/qnn/ui/       NerFragment / NerViewModel / EntityAdapter
app/src/test/.../ner/                      6 个测试类 + golden fixture
app/src/androidTest/.../NerE2ETest.kt      5 个真机用例
```

## 十一、不做的事（YAGNI）

- 不做脱敏替换输出（只标记高亮）
- 不引入 onnxruntime-android（golden 由 PC 侧提供）
- 不做 seq_len 512 或多 graph 切换
- 不做滑窗 + overlap 去重（按句切分已足够）
- 不做实体链接 / 归一化（"北京" 与 "北京市" 不合并）
- 不集成到 LLM chat 通路

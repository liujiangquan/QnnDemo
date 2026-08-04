# NER 使用/验证指南（8845 · BERT 中文敏感信息识别）

> 分支 `llm-qwen3-8845`；BERT-base 中文 NER，**fp32 DLC 直接跑 HTP V81**；
> 实测 **18ms/句**、余弦 0.99697 对齐 ONNX 原模型。

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

# 2. 推 fp32 DLC（388MB）到 app filesDir
bash docs/setup_bert_ner.sh

# 3. 跑 5 个 E2E 用例
bash docs/run_e2e_tests.sh ner
```

预期：`OK (5 tests)`，logcat 里能看到

```
PASS: recognize 29ms entities=[(PER, 马化腾), (LOC, 深圳), (ORG, 腾讯公司)]
PASS: 36ms / 2 句 = 18ms/句
PASS: 长文本 4 句 11 个实体，offset 全部对齐
```

## 四、UI 手动验证

启动 app → 底部导航 **NER** tab：

- 顶部多行输入框（预填了示例文本，可粘贴自己的）
- Spinner 切后端：`HTP`（18ms/句）/ `CPU`（~101ms/句，精度略高）
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

| 指标 | 数值 |
|---|---|
| `loadModel(HTP)` | ~5.3s（含 on-device 图编译 + warmup） |
| **单句推理（HTP fp32）** | **18ms** |
| 单句推理（CPU fp32） | ~101ms |
| 精度（vs ONNX 原模型） | 余弦 **0.99697**，实体集合完全一致 |
| seq_len | 128（编译期固定） |
| DLC 大小 | 388MB fp32 |

## 七、为什么用 fp32 而不是量化

实测量化全线失败，fp32 反而最快：

| 配置 | 精度 | 单句 |
|---|---|---|
| **fp32** | ✅ 余弦 0.99697 | **34ms**（shell）/ 18ms（app 内） |
| W8A16 per-channel | ❌ 预测全塌成 `O` | 55ms |
| W8A8 / A16W16 / percentile 标定 | ❌ 同样失败 | — |
| fp16 DLC | ⛔ HTP 无法组图 | — |

关键证据：**位宽 8→16 毫无变化**（0.712 → 0.714）。精度损失必然随位宽改善，完全不动说明是功能性错误。量化还多出 quantize/dequantize 算子，反而慢 1.6x。所以直接喂 fp32 让 HTP 内部按 fp16 执行。

排除过的假设：词嵌入粗量化（实际拿到 16-bit）、LayerNorm gamma 8-bit（A16W16 无改善）、标定方法（percentile 更差）、attention_mask/PAD 失效（填满 token 更差）。

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

## 九、离线转换（重新生成 DLC 时）

必须在 WSL 里跑（Windows 版 converter 缺 DLL）：

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
  htp_ner_fp16_config.json     HTP backend ext（fp16 尝试留档，当前不用）
docs/
  setup_bert_ner.sh            推 fp32 DLC 到设备
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

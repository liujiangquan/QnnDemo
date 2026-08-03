# 中文敏感信息识别（BERT NER on 8845 HTP）设计

**日期**：2026-07-23
**分支**：`llm-qwen3-8845`（后续可能开 `ner-bert-8845`）
**状态**：设计已确认，待实施

---

## 1. 目标

在 QnnDemo 里加一条 BERT NER 推理通路，跑在 8845 Hexagon V81 HTP 上（INT8 量化），
识别中文文本里的**人名 / 地名 / 公司名**，并配合正则规则识别**手机号 / 身份证 /
银行卡 / 邮箱 / 车牌**，组成完整的敏感信息识别管线。产出形态为独立的文本扫描 demo：
输入一段文本，高亮标记出所有敏感实体。

## 2. 模型资产

来源 `D:\workspace\models\bert-base-chinese-ner`，上游是
[`shibing624/bert4ner-base-chinese`](https://huggingface.co/shibing624/bert4ner-base-chinese)
的 ONNX 导出版（Apache-2.0）。

| 项 | 值 |
|---|---|
| 架构 | BERT-base：12 层 / 768 hidden / 12 heads |
| vocab_size | 21128 |
| max_position_embeddings | 512 |
| type_vocab_size | 2 |
| 标签 | 9 个 BIO：`O` / `B-PER` `I-PER` / `B-LOC` `I-LOC` / `B-ORG` `I-ORG` / `B-TIME` `I-TIME` |
| ONNX | `onnx/model.onnx`，fp32，407MB，opset 18 |
| Tokenizer | `onnx/vocab.txt`（21128 行，109KB）+ `tokenizer.json` |
| 官方 CPU 基线 | ONNX Runtime 4 线程：21 token 31ms / 242 token 209ms / 512 token 416ms |

上游 README 的重要约束（本设计遵循）：
- `PER` 用于姓名、`LOC` 用于地点/地址是推荐用法
- **`ORG` / `TIME` 不应默认视为敏感信息** —— 本设计中 UI 会展示但视觉弱化
- **手机号 / 证件号 / 银行卡 / 邮箱 / 密钥等结构化字段必须用确定性规则**，不能靠模型
- 本模型不应单独作为数据脱敏服务

## 3. 已确认的关键决策

| 维度 | 决策 | 理由 |
|---|---|---|
| 使用场景 | 独立文本扫描 demo | 不跟 LLM chat 耦合，先验证 NER 本身 |
| 推理后端 | **只跑 QNN HTP INT8** | 切项目主题；CPU 基线由 PC 侧 Python 提供，不引入 onnxruntime-android |
| seq_len | **固定 128** | QNN 需静态 shape；attention O(n²)，128 最快最稳；中文约 100 字 |
| 长文本 | **按标点切句逐句跑** | 中文句子是天然实体边界，避免 overlap 去重的复杂度 |
| 输出形态 | **只标记 + 高亮** | 保留原文，展示"识别到什么"，不做脱敏替换 |
| 范围 | **NER + 正则完整管线** | 一次做完，符合"敏感信息识别"的完整语义 |
| 实施顺序 | **方案 A：转换优先** | BERT INT8 上 HTP 是唯一未知且 all-or-nothing，最早暴露 |

## 4. 架构

**关键洞察：零新增 native 代码。**
现有 `QairtRuntime::execute(graphName, vector<vector<uint8_t>>)` 已支持多输入，
`TensorInfo` 已带 `dataType`。BERT NER 就是另一个 DLC（3 输入 / 1 输出），
直接复用 `InferenceEngine`。

```
NerFragment (BottomNav 第 4 tab)
      │
      ▼
NerBackend ──────────────────── 编排层（Kotlin）
      ├─ SentenceSplitter    按 。！？；\n 切句，保留原文 charOffset
      ├─ WordPieceTokenizer  vocab.txt → (ids, mask, typeIds, offsets)
      ├─ RegexDetector       手机号/身份证/银行卡/邮箱/车牌
      ├─ BioDecoder          logits[128][9] → List<Entity>
      └─ EntityMerger        NER ∪ 正则，重叠时正则优先
      │
      ▼ 复用现有 QNN Direct API 通路
InferenceEngine.loadDlc(path, Backend.HTP)
InferenceEngine.execute("bert_ner", [idsBytes, maskBytes, typeIdsBytes])
      │
      ▼
bert-ner-int8.dlc  on  Hexagon V81 HTP
```

## 5. 离线转换管线

```
onnx/model.onnx (fp32, 动态 shape)
  │
  │  qairt-converter
  │    --input_network model.onnx
  │    --source_model_input_shape "input_ids:1,128"
  │    --source_model_input_shape "attention_mask:1,128"
  │    --source_model_input_shape "token_type_ids:1,128"
  ▼
bert-ner-fp32.dlc
  │
  │  qairt-quantizer
  │    --input_list calibration_list.txt
  │    --act_bitwidth 8
  │    --bias_bitwidth 32
  ▼
bert-ner-int8.dlc  (~110MB)
  │
  ├─ PC 验证：qnn-net-run（x86 CPU backend）输出 vs Python golden，比 logits 余弦相似度
  └─ 设备验证：HTP backend 输出 vs 同一份 golden
```

**Calibration 数据**：50-100 条覆盖人名/地名/公司名/时间的中文句子，
经 tokenizer 处理后存为 raw int32 文件供 quantizer 采样。由 `tools/gen_calibration.py` 生成。

**量化敏感点（转换阶段需重点验证）**：
- `LayerNorm` / `Softmax` / `GELU` 在 INT8 下精度损失大，必要时保 fp16 混合精度
- Embedding table 21128 × 768 = 16M 参数，8MB VTCM 装不下，走 DDR
- `attention_mask` 是 0/1 语义，量化后必须保持语义正确
- ONNX 原始输入可能是 int64（QNN 不支持），需确认 converter 降级到 int32

**精度验收门槛**：HTP INT8 输出与 fp32 golden 的 logits 余弦相似度 ≥ 0.99，
且在测试句集上解出的实体集合完全一致。达不到则退到 fp16 或混合精度。

## 6. Kotlin 组件

每个类单一职责、可独立单测。

### `Entity`

```kotlin
enum class EntityType { PER, LOC, ORG, TIME, PHONE, ID_CARD, BANK_CARD, EMAIL, PLATE }
enum class EntitySource { MODEL, REGEX }

data class Entity(
    val text: String,
    val type: EntityType,
    val charStart: Int,      // 相对完整输入文本的字符偏移
    val charEnd: Int,        // 开区间
    val score: Float,        // MODEL 来源为 softmax 均值；REGEX 来源恒为 1.0
    val source: EntitySource,
)
```

### `SentenceSplitter`

`String` → `List<Sentence(text, charOffset)>`

- 切分符：`。！？；\n` 及其全角变体
- 保留每句在原文中的起始 charOffset（高亮需要）
- 单句过长时强制二次切分：先按 `，、,` 次级标点切；仍超长则按每 100 个字符硬切
  （100 字符是保守上界 —— 中文最坏情况每字 1 token，加 `[CLS]`/`[SEP]` 后仍 ≤ 128）
- 空串 / 纯标点输入返回空列表

### `WordPieceTokenizer`

`String` → `TokenizedInput(ids: IntArray, mask: IntArray, typeIds: IntArray, offsets: List<IntRange>)`

- 从 `assets/ner_vocab.txt` 加载 21128 词表到 `HashMap<String, Int>`
- 特殊 token：`[PAD]`=0 `[UNK]`=100 `[CLS]`=101 `[SEP]`=102
- 中文字符逐字直查词表
- 连续 ASCII 字母/数字合成 word，走 greedy longest-match subword（后续片段加 `##` 前缀）
- 查不到映射为 `[UNK]`
- 输出定长 128：`[CLS]` + tokens + `[SEP]` + `[PAD]`…
- `offsets` 记录每个 token 对应句内字符 range，特殊 token 为空 range

### `BioDecoder`

`FloatArray(128*9)` + `offsets` + `sentenceCharOffset` → `List<Entity>`

- 每个位置 argmax 得标签
- 扫描规则：`B-X` 开新实体；`I-X` 延续同类型；`O` 或类型切换则结束当前实体
- **容错**：`I-X` 前面不是 `B-X`/`I-X` 时，按 `B-X` 处理（HuggingFace simple 聚合策略）
- 跳过 `[CLS]` / `[SEP]` / `[PAD]` 位置
- `score` = 实体内所有 token 对应标签的 softmax 概率均值
- charStart/charEnd 由 token offsets 映射并加上 sentenceCharOffset

### `RegexDetector`

`String` → `List<Entity>`

| 类型 | 规则 | 附加校验 |
|---|---|---|
| PHONE | `1[3-9]\d{9}` | 前后不能紧邻数字 |
| ID_CARD | `[1-9]\d{5}(19\|20)\d{2}(0[1-9]\|1[0-2])(0[1-9]\|[12]\d\|3[01])\d{3}[\dXx]` | 校验位算法 |
| BANK_CARD | `\d{16,19}` | **Luhn 校验**（降误报关键） |
| EMAIL | `[\w.+-]+@[\w-]+\.[\w.]+` | — |
| PLATE | `[京津沪渝冀豫云辽黑湘皖鲁新苏浙赣鄂桂甘晋蒙陕吉闽贵粤青藏川宁琼]{1}[A-Z]{1}[A-Z0-9]{4,5}[A-Z0-9挂学警港澳]{1}` | — |

### `EntityMerger`

`List<Entity>`（MODEL） × `List<Entity>`（REGEX） → `List<Entity>`

- 合并后按 `charStart` 升序排序
- 区间重叠时**正则优先**（确定性规则比模型可信），丢弃被覆盖的 MODEL 实体
- 完全包含也按重叠处理
- 不做同类型相邻实体合并（YAGNI）

### `NerBackend`

`String` → `List<Entity>`

编排层：
1. `SentenceSplitter` 切句
2. 逐句 `WordPieceTokenizer` → `InferenceEngine.execute` → `BioDecoder`
3. 整段文本跑一次 `RegexDetector`
4. `EntityMerger` 合并
5. 挂在 `Dispatchers.IO`，提供 suspend 接口 + 进度回调

模型资产就绪检查复用 LLM 那套 `weightsReady()` 模式。

## 7. 张量约定

| Tensor | Shape | dtype | 内容 |
|---|---|---|---|
| `input_ids` | [1, 128] | int32 | `[CLS]`(101) + tokens + `[SEP]`(102) + `[PAD]`(0)… |
| `attention_mask` | [1, 128] | int32 | 有效位 1，PAD 位 0 |
| `token_type_ids` | [1, 128] | int32 | 全 0（单句任务） |
| `logits` | [1, 128, 9] | float32 | 标签顺序按 `config.json` 的 `id2label` |

`id2label` 顺序（**不可改，来自模型 config**）：

```
0: I-ORG   1: B-LOC   2: O       3: B-ORG   4: I-LOC
5: I-PER   6: B-TIME  7: I-TIME  8: B-PER
```

## 8. 错误处理

| 情况 | 处理 |
|---|---|
| DLC 未预置 | UI banner 提示跑 `bash docs/setup_bert_ner.sh`，禁用识别按钮 |
| HTP 不可用 | soft fail，banner 显示原因，不 crash（对齐 `BackendE2ETest` 现有模式） |
| 单句 execute 失败 | 跳过该句，其余句继续，UI 标注"N 句中 M 句失败" |
| 文本超长（> 50 句） | 截断到 50 句并提示，避免 UI 卡死 |
| vocab.txt 加载失败 | init 返回 false，banner 提示资产损坏 |

## 9. 测试策略

### JVM 单测（`./gradlew :app:testDebugUnitTest`，无需设备）

| 测试 | 覆盖点 |
|---|---|
| `SentenceSplitterTest` | 标点切句 / 无标点强切 / 空串 / 纯标点 / charOffset 正确性 |
| `WordPieceTokenizerTest` | 中文单字 / 英文 subword / UNK / 超长截断 / offset 映射 / 定长 padding |
| `BioDecoderTest` | 标准 BIO / `I-X` 无前导容错 / 类型切换 / 全 O / 首尾边界 / score 计算 |
| `RegexDetectorTest` | 五类各正例 + 反例 / Luhn 校验 / 身份证校验位 |
| `EntityMergerTest` | 重叠取正则 / 无重叠保留双方 / 完全包含 / 空输入 / 排序 |
| `NerGoldenTest` | ★ 读 PC 侧 golden logits json，断言 `BioDecoder` 解出实体与 Python 侧一致 |

`NerGoldenTest` 是**精度回归的核心防线**：把 PC 上 onnxruntime 算出的
fp32 logits 存成 fixture，任何时候改动 `BioDecoder` 都能立刻发现回归。

### Instrumented 测（需设备 + DLC）

| 测试 | 断言 |
|---|---|
| `NerE2ETest.nerLoadModel` | `loadDlc(HTP)` 成功，耗时 ≤ 5s |
| `NerE2ETest.nerRecognize` | 固定测试句识别出预期 PER/LOC/ORG，单句延迟 ≤ 100ms |
| `NerE2ETest.nerRegexPipeline` | 含手机号/身份证的句子被正则命中，与 NER 结果正确合并 |
| `NerE2ETest.nerLongText` | 多句文本按句处理，实体 charOffset 映射回原文正确 |

## 10. UI

BottomNav 加第 4 个 tab「NER」。

```
┌─────────────────────────────────┐
│ [banner: 状态 / 错误提示]        │
├─────────────────────────────────┤
│ ┌─────────────────────────────┐ │
│ │ 多行 EditText               │ │
│ │ （可粘贴待扫描文本）        │ │
│ └─────────────────────────────┘ │
│              [识别]              │
├─────────────────────────────────┤
│ 高亮结果（SpannableString）      │
│ 张三 去了 北京市 找 阿里巴巴     │
│ ~~~~     ~~~~~~     ~~~~~~~~~   │
│ (红)      (蓝)       (绿)        │
├─────────────────────────────────┤
│ 实体汇总 RecyclerView            │
│ 张三     PER    [0,2)   0.99 模型│
│ 北京市   LOC    [6,9)   0.97 模型│
│ 138xxxx  PHONE  [20,31) 1.00 正则│
├─────────────────────────────────┤
│ 耗时 45ms · 3 句 · 5 个实体      │
└─────────────────────────────────┘
```

配色：`PER` 红 / `LOC` 蓝 / `ORG` 绿 / `TIME` 灰（弱化，非敏感）/ 正则类紫。

## 11. 新增文件清单

```
tools/
  convert_bert_ner.sh          onnx → fp32 dlc → int8 dlc
  gen_calibration.py           造 calibration raw 输入
  ner_golden.py                onnxruntime 生成 golden logits json
docs/
  setup_bert_ner.sh            push dlc 到设备 filesDir
  NER-使用指南.md              使用/验证文档
app/src/main/assets/
  ner_vocab.txt                21128 行词表（109KB，打进 APK）
app/src/main/java/com/breeze/qnn/ner/
  Entity.kt
  SentenceSplitter.kt
  WordPieceTokenizer.kt
  BioDecoder.kt
  RegexDetector.kt
  EntityMerger.kt
  NerBackend.kt
app/src/main/java/com/breeze/qnn/ui/
  NerFragment.kt
  NerViewModel.kt
  EntityAdapter.kt
app/src/main/res/layout/
  fragment_ner.xml
  item_entity.xml
app/src/main/res/menu/
  bottom_nav.xml               （修改：加 nav_ner）
app/src/test/java/com/breeze/qnn/ner/
  SentenceSplitterTest.kt
  WordPieceTokenizerTest.kt
  BioDecoderTest.kt
  RegexDetectorTest.kt
  EntityMergerTest.kt
  NerGoldenTest.kt
app/src/test/resources/
  ner_golden.json              PC 侧生成的 golden fixture
app/src/androidTest/java/com/breeze/qnn/
  NerE2ETest.kt
```

## 12. 不做的事（YAGNI）

- 不做脱敏替换输出（只标记高亮）
- 不引入 onnxruntime-android（CPU 基线由 PC 侧 Python 提供）
- 不做 seq_len 512 或多 graph 切换（先固定 128 跑通）
- 不做滑窗 + overlap 去重（按句切分已足够）
- 不做同类型相邻实体合并
- 不做实体链接 / 归一化（"北京" 与 "北京市" 不合并）
- 不集成到 LLM chat 通路

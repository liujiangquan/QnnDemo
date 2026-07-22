# LLM 使用/验证指南（8845 · Qwen3-4B · QNN Genie）

> 分支 `llm-qwen3-8845`；SDK 2.48.40 全栈 + AIEngine Qwen3-4B bundle 权重；
> 走 unsigned PD；实测 26 tok/s decode。CNN HTP 通路当前 soft-fail（见文末）。

## 一、前置环境

- Snapdragon 8845 设备（SM8480XP / Hexagon V81 / 8MB VTCM）已 root
- 电脑侧 `adb devices` 能看到设备
- SDK：`C:/Qualcomm/AIStack/QAIRT/2.48.40.260702/`
- 权重：`D:/workspace/models/chat_extracted/chat/`（AIEngine Qwen3-4B bundle 解压后）

Bundle 目录结构：

```
chat/
├── models/
│   ├── weight_sharing_model_1_of_4.serialized.bin  (~778MB)
│   ├── weight_sharing_model_2_of_4.serialized.bin  (~591MB)
│   ├── weight_sharing_model_3_of_4.serialized.bin  (~591MB)
│   └── weight_sharing_model_4_of_4.serialized.bin  (~965MB)
├── tokenizer.json                                  (~11MB)
├── htp_backend_ext_config-v81.json
├── qwen3-4b-htp-v81.json     (bundle 原生 v1.13 config，脚本会转成 v1.19)
├── lib/                       (bundle 自带 .so，本流程不用，jniLibs 已经用 SDK 2.48)
└── bin/genie-t2t-run
```

## 二、代码分支

```bash
git checkout llm-qwen3-8845
git log --oneline -3
```

期望看到:

```
a25dfcd build: setup 脚本适配 AIEngine 4-shard bundle + v1.19 config
b70ec7c feat(llm): 8845 Qwen3-4B via SDK 2.48 全栈 + Genie v1.19 config，26 tok/s
50087cf fix(test): @Test 方法加显式 :Unit 返回类型
```

## 三、一次性验证（3 条命令）

```bash
# 1. 装 debug APK（含全 SDK 2.48 jniLibs：Genie/Htp/Prepare/V81Stub/V81Skel 都 2.48.40）
./gradlew :app:installDebug

# 2. 推 3.1GB 权重 + Genie v1.19 config 到 app filesDir
bash docs/setup_qwen3_llm.sh D:/workspace/models/chat_extracted/chat

# 3. 跑 4 个 E2E 测试（loadModel / generate / stop / multiTurn）
bash docs/run_e2e_tests.sh llm
```

预期输出：

```
Starting 4 tests on PZ261A - 16
Finished 4 tests on PZ261A - 16
BUILD SUCCESSFUL in 29s
```

logcat 里会看到 `QnnDemoLlm: generate 结束: tokens=100 elapsed=~3800ms speed=~26 tok/s`。

## 四、跑单个 case

```bash
bash docs/run_e2e_tests.sh llmLoadAndPrefill   # 只跑 loadModel（≤10s 断言）
bash docs/run_e2e_tests.sh llmGenerate         # 跑 generate + 速度≥15 tok/s + 中文断言
bash docs/run_e2e_tests.sh llmStop             # 跑 stop（≤3s 断言）
bash docs/run_e2e_tests.sh llmMultiTurn        # 跑多轮，round2 必须记住 round1 的 "小明"
```

## 五、CLI 手动跑测试用例

绕开 gradle 直接 `am instrument`：

```bash
adb shell am instrument -w \
  -e class com.breeze.qnn.LlmE2ETest#llmGenerate \
  com.breeze.qnn.test/androidx.test.runner.AndroidJUnitRunner
```

同时开 logcat 看 native 侧输出：

```bash
adb logcat -v threadtime -s QnnDemoLlm:V LlmBackend:V TestRunner:V
```

## 六、UI 手动验证

启动 app → 底部导航切到 **Chat** tab → 打字发送。**Settings** tab 里改采样参数
（temperature / topK / topP / maxTokens），下一次 `generate` 生效。

## 七、关键基线（这次实测）

| 指标                                         | 数值           |
| -------------------------------------------- | -------------- |
| loadModel（4-shard 3.1GB → GenieDialog）     | **1.5s**       |
| generate speed（decode）                     | **26.2 tok/s** |
| stop（ABORT signal → dialogQuery 返回）      | **21ms**       |
| multiTurn（记住 round1 里的 "小明"）         | ✅ pass         |
| context size                                 | 2048 tokens    |
| 权重占用（`/data/user/0/.../files/llm/qwen3-3b/`） | 2.8GB          |

## 八、常见坑速查

### `err -7 GENIE_STATUS_ERROR_JSON_FORMAT`

Config schema 用了 v1.13 旧格式，SDK 2.48 Genie 1.19 不认。检查 `qwen3-3b-htp.json`：

- ✅ `model.positional-encoding.rope-dim` / `.rope-theta`
- ❌ `engine.backend.QnnHtp.pos-id-dim` / `.rope-theta`

也可能是 `createFromJson` 传的是文件路径而不是内容字符串（native 层 bug，已修）。

### `AEE_EBADCLASS 0x8000040e`

CDSP 侧 skel/stub 类版本不匹配。检查 jniLibs 版本一致：

```bash
grep -aoE 'AISW_VERSION: [0-9]+\.[0-9]+\.[0-9]+' \
  app/src/main/jniLibs/arm64-v8a/{libGenie,libQnnHtp,libQnnHtpPrepare,libQnnHtpV81Stub,libQnnHtpV81Skel,libQnnHtpNetRunExtensions}.so
```

应全部输出 `AISW_VERSION: 2.48.40`。

### `INSTALL_FAILED_UID_CHANGED`

sharedUserId 冲突。清干净重装：

```bash
adb uninstall com.breeze.qnn
adb uninstall com.breeze.qnn.test
./gradlew :app:installDebug
```

### `err 14001 device create failure`

fastrpc/HTP layer 报错。多半是当前 caller 走了 signed PD 但 skel 找不到，或 skel/stub 版本鸿沟。检查：

- `libQnnHtpV81Skel.so` 是 SDK 2.48 unsigned 版本（`hexagon-v81/unsigned/`），不是 signed 版
- ADSP_LIBRARY_PATH 里 app nativeLibDir 排第一位（`LlmRuntime::init` 已经处理）

### filesDir 里 llm 目录被冲掉

`adb install -r` 在 sharedUserId app 上偶尔会清 data。重推：

```bash
bash docs/setup_qwen3_llm.sh D:/workspace/models/chat_extracted/chat
```

## 九、`setup_qwen3_llm.sh` 里发生了什么

1. 把 4 个 `weight_sharing_model_*.bin` push 到 `/data/user/0/com.breeze.qnn/files/llm/qwen3-3b/models/`
2. Push `tokenizer.json` 和 `htp_backend_ext_config-v81.json`
3. Hardlink shard1 为 `qwen3-3b-context.bin`（兼容 `LlmBackend.weightsReady()` 里 ≥500MB 检查）
4. 在本地生成 v1.19 schema 的 Genie config（`positional-encoding` 块 + 4-shard `ctx-bins` 数组），
   push 为 `qwen3-3b-htp.json`

> 目录名 `qwen3-3b` 沿用 `LlmE2ETest.MODEL_ID` 常量，实际是 4B。后续如需
> 重命名为 `qwen3-4b-instruct`，改 `LlmE2ETest`、`LlmBackend.weightsReady()`、
> setup 脚本三处即可。

## 十、已知代价

**CNN HTP 通路 soft-fail**。main 分支的 QNN Direct API + signed PD + vendor 2.46
skel 组合，在这个分支上把 stub 换 SDK 2.48 之后 `deviceCreate` 报 `0x36b1`
（14001）。`BackendE2ETest` 走 softFail 分支不 hard fail。

LLM 与 CNN 两条 HTP 通路在同一 jniLibs 目录里共享 `libQnnHtp.so` /
`libQnnHtpV81Stub.so` 无法共存。合并到 main 时需要二选一或做运行时 lib
目录切换（详见对应 commit message）。

## 十一、参考

- `docs/superpowers/specs/2026-07-22-8845-llm-inference-design.md` — 方案设计
- `docs/superpowers/plans/2026-07-22-8845-llm-qwen3.md` — 实施 plan
- `docs/QNN集成完整指南.md` — 主分支 QNN Direct API 章节，含 signed PD 上下文
- `docs/LLM-baseline.md` — 待回填 baseline 数字
- Memory: `8845_llm_qwen3_baseline.md`, `genie_stack_version_alignment.md`

# 8845 上 Qwen3-3B 大模型推理集成 · Design Spec

> Feature: 在 main 分支（signed PD + 平台签 + 8845 vendor QNN 集成）之上，扩展 Snapdragon 8845 上 Qwen3-3B 大模型 (LLM) 的端到端推理能力。
>
> Author: QnnDemo 项目组
> Date: 2026-07-22
> Status: **Design approved by user，等待 spec 复审后进入 writing-plans**

## 摘要

在保留现有 MobileNetV2 CNN 场景（HTP 后端 2.3 ms）不变的前提下，把 QNN Direct API 层重构成通用推理抽象，把 CNN / LLM 作为并列 backend 归到抽象下。用 Qualcomm 官方 Genie 框架驱动 Qwen3-3B（Q_INT4 量化），在 8845 Hexagon V81 signed PD 上跑推理。UI 加两个 Fragment（Chat + Settings），CI 增加 LLM E2E 测试。

**成功定义**：能在 8845 signed PD 上加载 Qwen3-3B context binary，回答简单中文问题（如"介绍下苏州"），稳态 decode 速度 ≥ 15 tokens/s。

---

## 目录

- [1. 背景与目标](#1-背景与目标)
- [2. 关键决策](#2-关键决策)
- [3. 架构](#3-架构)
- [4. 组件清单](#4-组件清单)
- [5. 数据流](#5-数据流)
- [6. 错误处理](#6-错误处理)
- [7. 测试策略](#7-测试策略)
- [8. 性能 baseline](#8-性能-baseline)
- [9. 风险与已知缺口](#9-风险与已知缺口)
- [10. 不做的事（YAGNI）](#10-不做的事yagni)

---

## 1. 背景与目标

### 1.1 起点

main 分支当前状态：
- QNN Direct API 已跑通 CNN 场景（MobileNetV2，HTP 2.3 ms）
- signed PD + 平台签 + `sharedUserId=android.uid.system` + vendor 2.46 QNN 集成
- APK ~170 MB，jniLibs 混用 SDK 2.48 System/CPU + vendor 2.46 HTP

设备：Snapdragon 8845 (SM8480XP)，Hexagon **V81**，8 MB VTCM，userdebug ROM，vendor firmware 带 Qualcomm 签名的 `libQnnHtpV81Skel.so`。

### 1.2 目标

在同一个 App 里加入 Qwen3-3B 端到端聊天推理能力，覆盖：
- 加载权重（预置在设备 `filesDir`）
- prefill + streaming decode
- 多轮对话（上下文简单拼接）
- 停止 / 中断
- 采样参数（temperature / top-p / max_tokens）

### 1.3 不做

见 [§10](#10-不做的事yagni)。

---

## 2. 关键决策

在 brainstorming 阶段跟用户逐项确认的决策：

| # | 决策项 | 选定 |
|---|---|---|
| 1 | 目标模型规模 | **中模型（1-3B）** |
| 2 | 具体模型 | **Qwen3-3B**（中文场景优先，SDK examples 有 qwen3 htp 配置） |
| 3 | 框架 | **Genie**（Qualcomm 官方 LLM 高层封装） |
| 4 | 权重来源 | 现成包（内网 or AI Hub），落实作为独立子任务 |
| 5 | 权重部署 | **预置到 `/data/user/0/com.breeze.qnn/files/llm/qwen3-3b/`**（adb push 一次，不进 APK） |
| 6 | UI 形态 | **Chat Activity + LlmE2ETest 都要** |
| 7 | 精度目标 | **能跑 + 基本体验**（≥15 tokens/s、简单问题能回答） |
| 8 | 实施方案 | **方案 B：深度重构**——抽 `BaseInferenceRuntime`，让 `QairtRuntime` 和 `LlmRuntime` 都继承 |

### 2.1 为什么强绑 signed PD 而不是 unsigned PD

- LLM 需要 8 MB VTCM 满配（Qwen3-3B INT4 单层 activation cache 会超 4 MB）
- unsigned PD 通常被 kernel 限到 4 MB VTCM，会强制层切块 + DDR spill，性能腰斩 5-10x
- 8845 vendor 已有 Qualcomm 签的 V81 skel，signed PD 路径完备
- main 分支 CNN 场景本就是 signed PD 部署，Genie 复用该通道

参考 `docs/Signed-PD-vs-Unsigned-PD.md` §5 "Signed PD 独有的能力"。

---

## 3. 架构

### 3.1 分层视图

```
+-------------------------------------------------------+
| Application (Kotlin)                                  |
|                                                       |
|  MainActivity + BottomNavigation                      |
|    ├─ BackendFragment  (现有 CNN backend switch/E2E)   |
|    ├─ ChatFragment     (新增 LLM 对话)                 |
|    └─ SettingsFragment (采样参数 UI)                   |
|                                                       |
|  InferenceEngine (Kotlin facade)                       |
|    ├─ CnnBackend (调 QairtRuntime，跑 MobileNetV2)     |
|    └─ LlmBackend (调 LlmRuntime，跑 Qwen3-3B)          |
+---------------------- JNI -----------------------------+
| Native (C++)                                          |
|                                                       |
|  BaseInferenceRuntime (纯虚基类)                       |
|    virtual bool init(searchPath) = 0                  |
|    virtual void cleanup() = 0                         |
|    virtual bool isReady() const = 0                   |
|                                                       |
|  QairtRuntime : BaseInferenceRuntime                  |
|    (现有 QNN Direct API 实现，跑 CNN，只加继承)         |
|                                                       |
|  LlmRuntime : BaseInferenceRuntime  (全新)             |
|    ├─ bool loadModel(configPath)                      |
|    ├─ void generate(prompt, params, tokenCb)           |
|    └─ void stop()                                      |
+-------------------------------------------------------+
| Genie C API (libGenie.so + libQnnGenAiTransformer.so)  |
|   from SDK 2.48                                       |
+-------------------------------------------------------+
| QNN Direct (libQnn* vendor 2.46 HTP + SDK 2.48 CPU)    |
+-------------------------------------------------------+
| Hexagon V81 DSP (signed PD, 8 MB VTCM 满配)             |
+-------------------------------------------------------+
```

### 3.2 设计原则

- **CNN / LLM 并列独立**：`QairtRuntime` 和 `LlmRuntime` 分别处理，不试图共享底层实现细节。基类只提供最小通用接口 (`init` / `cleanup` / `isReady`)
- **Genie 独占 LLM 通道**：不用 QNN Direct API 自己实现 tokenizer + prefill/decode loop（工作量爆炸且失去 Genie 平台优化）
- **UI 三 Fragment**：BottomNav 切换，不做深度状态共享
- **权重外置**：APK 保持 ~170 MB，权重 1.5-2 GB 走设备预置
- **保留 main 分支现有签名 / UID 机制**：signed PD 强依赖，不允许改动

---

## 4. 组件清单

### 4.1 新增 native 文件

| 文件 | 作用 | 约行数 |
|---|---|---|
| `app/src/main/cpp/base_inference_runtime.h` | 纯虚基类 `BaseInferenceRuntime` | ~30 |
| `app/src/main/cpp/llm_runtime.h` | `LlmRuntime` 类定义 | ~80 |
| `app/src/main/cpp/llm_runtime.cpp` | dlopen `libGenie.so`，实现 loadModel / generate / stop | ~500 |
| `app/src/main/cpp/llm_jni.cpp` | JNI 桥，token callback 反调 Kotlin | ~200 |

### 4.2 修改 native 文件

| 文件 | 修改内容 |
|---|---|
| `app/src/main/cpp/qairt_runtime.h` | `class QairtRuntime : public BaseInferenceRuntime`，方法加 `override` |
| `app/src/main/cpp/qairt_runtime.cpp` | 只加 `override` 关键字，方法体不动 |
| `app/src/main/cpp/CMakeLists.txt` | 加 `llm_runtime.cpp` / `llm_jni.cpp` 到 sources |

### 4.3 新增 jniLibs（从 SDK 2.48 拷入）

| 库 | 大小 | 用途 |
|---|---|---|
| `libGenie.so` | ~500 KB | Genie 主运行时 |
| `libQnnGenAiTransformer.so` | ~3 MB | Transformer 图执行 |
| `libQnnGenAiTransformerModel.so` | ~1 MB | Model 加载器 |
| `libQnnHtpV81CalculatorStub.so` | ~250 KB | HTP V81 计算 stub（Genie 需要） |
| `libQnnHtpNetRunExtensions.so` | ~1.4 MB | HTP 扩展（Genie 依赖） |

APK 增量约 **6 MB**。

### 4.4 新增 Kotlin 文件

| 文件 | 作用 | 约行数 |
|---|---|---|
| `LlmNative.kt` | JNI 声明 | ~50 |
| `LlmBackend.kt` | Kotlin API 层（handle 生命周期 + Coroutine） | ~150 |
| `ui/ChatFragment.kt` | 对话 UI Fragment | ~200 |
| `ui/ChatAdapter.kt` | RecyclerView adapter | ~80 |
| `ui/BackendFragment.kt` | 从 MainActivity 迁移的 CNN backend switch | ~150 |
| `ui/SettingsFragment.kt` | 采样参数 UI（temperature / top-p / max_tokens） | ~100 |
| `LlmE2ETest.kt` (androidTest) | LLM E2E 自动化 | ~100 |

### 4.5 修改 Kotlin 文件

| 文件 | 修改内容 |
|---|---|
| `MainActivity.kt` | 改成 BottomNavigation 容器 |
| `InferenceEngine.kt` | 拆成 CnnBackend + LlmBackend 的 facade |
| `app/build.gradle.kts` | `androidResources { noCompress += "bin" }`（Genie context binary 不压缩） |

### 4.6 新增布局资源

- `layout/activity_main.xml`：BottomNavigation 容器
- `layout/fragment_chat.xml`：RecyclerView + EditText + 发送/停止 按钮
- `layout/fragment_backend.xml`：从原 activity_main.xml 拆出
- `layout/fragment_settings.xml`：采样参数 slider
- `layout/item_chat_user.xml` / `item_chat_assistant.xml`：消息气泡

### 4.7 新增脚本 / 文档

- `docs/setup_qwen3_llm.sh` — 一键 push Qwen3-3B 权重到设备 filesDir
- `docs/LLM-baseline.md` — 性能 baseline 数据记录（run 5 次 median）
- `docs/LLM-手工验证.md`（可选）— §7.4 手工验证清单

### 4.8 设备端预置（不进 APK）

adb push 到 `/data/user/0/com.breeze.qnn/files/llm/qwen3-3b/`：

```
├── qwen3-3b-htp.json        (Genie config，改自 SDK examples/Genie/configs/qwen3/)
├── qwen3-3b-context.bin     (~1.5 GB QNN context binary，signed PD V81 编译)
└── tokenizer.json           (Qwen tokenizer)
```

新增 `docs/setup_qwen3_llm.sh` 脚本一键 push。

### 4.9 依赖库汇总

保留 main 分支现有配置：
- vendor 2.46 `libQnnHtp/HtpPrepare/HtpV81Stub/QnnSystem`
- SDK 2.48 `libQnnSystem/libQnnCpu`
- `sharedUserId=android.uid.system` + 平台签

新增：SDK 2.48 Genie 相关五个库（§4.3）。

---

## 5. 数据流

### 5.1 生命周期状态机

```
   App 启动
     │
     ▼  setenv ADSP_LIBRARY_PATH（一次性）
   ┌──────────┐  dlopen libGenie.so
   │  READY   │◄─────  get Genie C API providers
   └─────┬────┘
         │ ChatFragment.onResume()
         ▼  GenieDialogConfig_createFromJson(qwen3-3b-htp.json)
   ┌──────────┐  GenieDialog_create(config) → dialogHandle
   │  LOADED  │◄─────  (~2-5 秒 context binary mmap + signed PD 建立)
   └─────┬────┘
         │ 用户点"发送"
         ▼  GenieDialog_execute(prompt, tokenCb, completeCb)
   ┌──────────┐  每 decode 一个 token → tokenCb → JNI → Kotlin
   │GENERATING│◄─────  (5-30 秒 for max_tokens=256)
   └─────┬────┘
         │ 结束 or 用户点"停止"
         ▼  GenieDialog_cancel() (中途停止时)
   ┌──────────┐  重新 ready 接收下一轮 prompt
   │  LOADED  │◄─────
   └──────────┘
         │ ChatFragment.onDestroy() 或 backend 切走
         ▼  GenieDialog_free(dialogHandle)
   ┌──────────┐  释放 signed PD context
   │  READY   │◄─────
   └──────────┘
```

### 5.2 单次 generate 详细流程

```
[Kotlin UI thread]
  ChatFragment.onSendClicked(promptText)
    ├─ 显示 user message 气泡
    ├─ 显示 assistant 气泡（空占位，"typing..."）
    └─ viewModelScope.launch { LlmBackend.generate(...) }

[Kotlin Coroutine (Dispatchers.IO)]
  LlmBackend.generate(prompt, params, listener):
    LlmNative.nativeGenerate(handle, prompt, temp, topP, maxTokens, callbackJava)
    ↑ 阻塞调用

[JNI - Java_com_breeze_qnn_LlmNative_nativeGenerate]
  从 handle 取 LlmRuntime*
  调 LlmRuntime::generate(prompt, params, [](tokenStr, isLast){
    env->CallVoidMethod(listener, onToken, env->NewStringUTF(tokenStr));
    if (isLast) env->CallVoidMethod(listener, onComplete, ...);
  });

[C++ LlmRuntime::generate]
  设 sampling params (GenieSampler_setParams: temp / top-p / max_tokens)
  GenieDialog_execute(dialogHandle, prompt, tokenCb, userdata);
    ↓ Genie 内部
    tokenize (Qwen BBPE) → tokenIds
    prefill (整个 prompt 一次前向)
    decode loop:
      for i in 0..max_tokens:
        one token forward on HTP V81 (signed PD)
        sampling
        tokenCb(env, listener, tokenStr, isLast=false)
        if token == EOS: break
    tokenCb(..., isLast=true)

[Kotlin main thread (via Handler)]
  onToken(tokenStr):
    chatAdapter.appendToLastMessage(tokenStr)
    recyclerView.smoothScrollToPosition(...)
  onComplete(stats):
    chatAdapter.markLastMessageComplete(tokensPerSec, elapsedMs)
```

### 5.3 JNI Callback 线程模型

- `GenieDialog_execute` **同步阻塞**（Genie 内部 loop），从 Coroutine IO dispatcher 调
- tokenCb 在 Genie 内部线程调（跟 IO dispatcher 线程一致），`env->CallVoidMethod` 安全
- Kotlin `onToken` 收到时在 IO 线程，`LlmBackend` 内部 `withContext(Dispatchers.Main)` 切主线程更新 UI
- ChatFragment 不感知线程

### 5.4 Stop 语义

用户点"停止"按钮：
1. `LlmBackend.stop()` → `LlmNative.nativeStop(handle)` → `LlmRuntime::stop()`
2. C++ 调 `GenieDialog_cancel(dialogHandle)` —— 异步 flag，**不阻塞** 当前 generate 调用
3. Genie decode loop 下一次迭代检测到 cancel flag，退出
4. `GenieDialog_execute` 返回，最后一次 tokenCb 带 `isLast=true` 和 `stoppedReason=USER_CANCEL`
5. Kotlin `onComplete(stats.stoppedReason=USER_CANCEL)` 更新 UI，取消 typing 状态

**注意**：stop 不 100% 秒停，可能还要跑完当前 token（最多 1-2 tokens 延迟）。

### 5.5 权重加载

`LlmRuntime::loadModel(configPath)`:
1. 检查 `configPath` 存在（如 `filesDir/llm/qwen3-3b/qwen3-3b-htp.json`）
2. Genie 从 config JSON 自己提取 context binary 相对路径 + tokenizer 路径
3. 调 `GenieDialogConfig_createFromJson(configPath)`
4. 调 `GenieDialog_create(config, &dialogHandle)` —— 内部 mmap context binary、建 signed PD、初始化 KV cache buffer

**性能预期**：loadModel 一次 2-5 秒（context binary mmap 主开销），之后 generate 单次 5-30 秒。

---

## 6. 错误处理

按"错误发生阶段 → 判定 → 用户可见动作"三列。

### 6.1 App 启动 / init 阶段

| 错误 | 判定 | 反馈 & 处理 |
|---|---|---|
| `dlopen libGenie.so` 失败 | libGenie == nullptr && dlerror 非空 | Log error；`LlmRuntime.isReady()` = false；ChatFragment 灰化 + banner"LLM 后端不可用" |
| `GenieInterface_getProviders` dlsym 失败 | 返回 null | 同上，v1 直接禁用 chat tab |
| `sharedUserId=system` 装不上 | `Process.myUid() != Process.SYSTEM_UID` | Log warn；不 crash（CNN CPU 仍可用）；ChatFragment 灰化并 banner"当前签名/UID 无法使用 signed PD" |

### 6.2 权重预置检查（loadModel 前置）

| 错误 | 判定 | 反馈 & 处理 |
|---|---|---|
| Genie config JSON 不存在 | File.exists() == false | ChatFragment 空态提示 + 完整 adb push 命令模板 + "复制命令"按钮 |
| context binary 不存在 | 同上 | 同上 |
| tokenizer.json 不存在 | 同上 | 同上 |
| 权重大小明显不对（< 500 MB） | File.length() 校验 | "权重不完整，请重新 push" |

### 6.3 loadModel 阶段

| 错误 | Genie 错误码 | 反馈 & 处理 |
|---|---|---|
| Genie config 解析失败 | `GENIE_STATUS_ERROR_INVALID_CONFIG` | Log config 路径 + 错误详情；提示"配置文件格式不对" |
| Context binary 版本不匹配 | `GENIE_STATUS_ERROR_INVALID_HANDLE` | "权重是用不同 SDK 版本编译的，需要用 SDK 2.48 重新转换"，附命令 |
| Context binary target arch 不是 V81 | Genie 内部 QnnDsp arch mismatch | "编译时 target arch 不是 V81，重新 `qnn-context-binary-generator ... --config <cfg with soc_model=8845>`" |
| Signed PD 打不开 | Genie 内部 `err 4000` 或 `qnn_open failed` | "HTP signed PD 打不开：(a) 检查平台签名 (b) 检查 vendor `libQnnHtpV81Skel.so`"，链接 `docs/QNN集成完整指南.md` §4 |
| VTCM 不足 | Genie 报 memory allocation failed | "8845 8MB VTCM 不够，检查 context binary VTCM budget" |
| loadModel 超时 (> 30 秒) | `withTimeout(30_000)` | Coroutine cancel + GenieDialog_free，UI 报错回到未加载态 |

### 6.4 generate 阶段

| 错误 | 判定 | 反馈 & 处理 |
|---|---|---|
| Prompt 空 | Kotlin 层前置校验 | 发送按钮灰化 |
| Prompt 超长 | Kotlin 层按 utf-8 字节 / 3 估算 | 提示 + 自动截断历史消息到最近 N 轮 |
| `GenieDialog_execute` 返回非 SUCCESS | 检查 status | assistant 消息标红显示错误码；不影响下次 generate |
| Generate 中 native SIGSEGV | 无法从 Kotlin 捕获 | Android crash 常规路径；下次启动清 dialogHandle 状态 |
| tokenCb 里 JNI throw exception | `env->ExceptionCheck()` | Log error + ExceptionClear + 继续下一 token |
| Decode 卡住 (< 1 tok/s) | Kotlin timer 监控 | 提示"decode 异常慢，可能 HTP profile 掉档" |

### 6.5 生命周期错误

| 错误 | 判定 | 反馈 & 处理 |
|---|---|---|
| 用户点 stop 但没在 generate | Kotlin state check | 按钮本就灰化，防御性 no-op |
| ChatFragment destroy 时 generate 还在跑 | Fragment lifecycle | viewModel.onCleared → stop → Genie cancel → 等 execute 返回后 free |
| 切到 Backend Tab 时 chat 还在 generate | 不切走 native handle | UI 保留 typing 状态，切回来能看到 |
| App 退后台被回收 | onSaveInstanceState 保存历史 | onCreate 重新 loadModel，用户看到历史消息 |

### 6.6 系统性异常

| 错误 | 判定 | 反馈 & 处理 |
|---|---|---|
| DSP SSR (Subsystem Restart) | `QNN_COMMON_ERROR_SYSTEM_COMMUNICATION` | 等 SSR 恢复（< 2 秒），自动 `GenieDialog_free` + `create` 重建；UI"DSP 重启中..." |
| 电池低 DSP 降频 | 观察 tokens/s 下降 | 不特殊处理 |

### 6.7 错误设计原则

1. **CNN / LLM 后端故障隔离**：LLM init 失败不影响 CNN；反之亦然
2. **native 尽可能不 crash**：try/catch 包 Genie 调用，转 error string 返回 Kotlin
3. **UI 显示具体错误码 + 可操作下一步**（如 push 命令），不给"internal error"黑箱
4. **权重预置是主要 friction**，UI 给足指引不指望用户看 docs

---

## 7. 测试策略

### 7.1 分层

```
+--------------------------------------------------+
| 手工验证         人肉判断"能不能用"                 |
+--------------------------------------------------+
| Instrumented E2E  跑 CI，回归防守（主）            |
+--------------------------------------------------+
| Unit test         不做（Kotlin 层几乎只是壳）       |
+--------------------------------------------------+
```

### 7.2 Instrumented E2E - `LlmE2ETest`

跟现有 `BackendE2ETest` 平级。用 `SOFT FAIL` 惯例：无权重时警告跳过，保证 CI 通过。

#### `llmLoadAndPrefill`
- 检查权重预置
- 加载 Qwen3-3B，断言 loadModel ≤ 10 秒
- 无 QNN error

#### `llmGenerate`
- 加载后跑 prompt "介绍下苏州，一句话"
- 断言：生成 ≥ 5 tokens、tokens/s ≥ 15、含中文字符
- 记录到 logcat：`PASS: generate tokens=N elapsed=Xms speed=Y tok/s response='...'`

#### `llmStop`
- 起 generate（max_tokens=500）
- 1 秒后 stop
- 断言 stop ≤ 2 秒返回

#### `llmMultiTurn`
- 第一轮问名字（"我叫小明。你叫什么？"）
- 第二轮拼接历史 + "请问我叫什么？"
- 断言第二轮回答含"小明"

### 7.3 集成到 run_e2e_tests.sh

```bash
bash docs/run_e2e_tests.sh llmGenerate     # 单跑
bash docs/run_e2e_tests.sh                 # 三后端 + LLM 共 7 test
```

### 7.4 手工验证清单（release 前）

| # | prompt | 期望 |
|---|---|---|
| 1 | "你好" | 有礼貌简短中文回应 |
| 2 | "介绍下苏州" | 3-5 句相关内容 |
| 3 | "what is 2+2" | "4" 或含正确答案 |
| 4 | "写一个 100 字关于狗的短故事" | 中文，有起承转合 |
| 5 | 多轮：先问名字、再问"我叫什么" | 记得上文 |
| 6 | 中途点 stop | ≤ 2s 停下 |
| 7 | 无权重打开 chat | 明确提示 + adb push 命令模板 |
| 8 | 切 Backend tab 跑 CNN 再切回 | LLM 状态保留 |

---

## 8. 性能 baseline

跑 5 次取 median 记到 `docs/LLM-baseline.md`。

| 指标 | 目标（8845 signed PD） |
|---|---|
| loadModel 时间 | ≤ 5 秒 |
| First token latency | ≤ 500 ms |
| Steady-state tokens/s | **≥ 15**（跑通门槛） |
| prefill tokens/s | ≥ 100（max_input=512） |

---

## 9. 风险与已知缺口

### 9.1 权重来源未落实

**风险**：spec 假设"现成 Qwen3-3B QNN 包可拿到"，但实际来源需要子任务确认：
- Qualcomm AI Hub 有没有 Qwen3-3B（Qwen3 是 2025 新模型，可能没上）
- AIEngine 项目内部有没有
- 内网 nexus 有没有

**缓解**：实施 plan 第一个 task 就是"获取权重包"；如果三条都没戏，退回到"从 HuggingFace 转"（工作量大 4-8 小时 + GPU 机器）。

### 9.2 Genie 版本兼容性

**风险**：SDK 2.48 的 libGenie 跟 vendor 2.46 的 QNN 底层是否兼容？之前 main 分支踩过 QAIRT 2.48 vs QNN 2.46 的坑（不兼容），Genie 是否同样受影响未验证。

**缓解**：实施 plan 早期 task 就在 8845 上跑 SDK 2.48 的 `genie-t2t-run` 命令行工具（用某个官方 config），只要它跑通就说明 Genie 2.48 + vendor 2.46 兼容。

### 9.3 tokenizer 集成

**风险**：Qwen tokenizer 是 BBPE，AIEngine 用了 `libcore_tokenizers.so`。SDK 2.48 的 Genie 是否内置 BBPE 支持不确定。

**缓解**：如果 Genie 不带，从 AIEngine 借 `libcore_tokenizers.so`（同厂内部资产）。

### 9.4 长上下文性能

**风险**：Qwen3-3B 上下文窗口 32k tokens，但 KV cache 装满会超过 8845 8 MB VTCM。

**缓解**：v1 只测试短 prompt (< 512 tokens)；长 prompt 优化留到后续。UI 层做截断预防（估算 token 数 > 4000 就截断历史）。

### 9.5 chat 多轮上下文简单实现

**做**：每次 generate 把整个历史拼进 prompt，Genie 每次重新 prefill  
**代价**：多轮对话每轮都要重新处理全部历史（O(N²) 复杂度）  
**未来优化**：KV cache 复用（Genie 提供 continuation API），留到 v2

---

## 10. 不做的事（YAGNI）

明确排除：

- ❌ **fp16 baseline 对齐** —— 用户选"能跑 + 基本体验"，不做严格评测
- ❌ **KV cache 复用** —— v1 简单每次重新 prefill
- ❌ **streaming API 到 Kotlin Flow** —— 只用 callback，够用
- ❌ **多模态（图片/语音）** —— 只做 text-to-text
- ❌ **多个 LLM 支持** —— 只跑 Qwen3-3B，硬编码模型 ID
- ❌ **本地 fine-tune / LoRA** —— 只跑基础模型
- ❌ **端上 embedding / RAG** —— 只做 chat
- ❌ **首次启动下载权重** —— 只做 adb push 预置
- ❌ **Espresso UI 测试** —— streaming 场景 Espresso 脆
- ❌ **完整 fp16 精度对齐评测集** —— 不跑 MMLU 之类的评测集
- ❌ **AOSP 分支支持 LLM** —— 现在只在 main 分支做（aosp 走 unsigned PD，VTCM 减半，LLM 性能会掉 5-10x）

---

## 相关文档

- `docs/QNN集成完整指南.md` — QNN 基础集成流程
- `docs/Signed-PD-vs-Unsigned-PD.md` — signed PD 是 LLM 必须
- `docs/HTP-8845-诊断报告.md` — 8845 signed PD 排查历史
- `docs/superpowers/specs/2026-07-22-8845-llm-inference-design.md` — 本文档

## 变更历史

- 2026-07-22 v1.0：初稿，用户 approved 五节设计

# YOLO26-pose 使用指南

在 Snapdragon 8845 (Hexagon V81) 上跑 YOLO26-pose 实时人体姿态识别。
HTP 后端平均 60ms / 帧（640×640 输入，单类 person + 17 keypoints）。

## 1. 真机验证基线

**设备：** Snapdragon 8845 (SM8845, Hexagon V81, 8MB VTCM)
**SoC model：** 97
**ADB 序列号：** `87e80eb`（按当前设备实际值替换）

已验证通过的测试 (`docs/run_e2e_tests.sh yolo` 跑 `YoloE2ETest`)：

```
I YoloE2ETest:  bin 已在 /data/user/0/com.breeze.qnn/files/yolo/yolo26n-pose.bin，跳过拷入
I YoloE2ETest:  ctxFile len=4104192
I YoloE2ETest:  modelReady=true
I YoloPose:     loaded /system/priv-app/QnnDemo/lib/arm64/libQnnSystem.so
I YoloPose:     loaded libQnnHtpV81Stub.so
I YoloPose:     loaded libQnnHtp.so
I YoloPose:     decode maxConf=0.807 preNms=10 kept=1 src=480x640 box0=[154,161,365,362]
I YoloE2ETest:  flat size=57
I YoloE2ETest:  dets=1 persons=1
I YoloE2ETest:  PASS: detect person elapsed=60ms
OK (1 test)
```

`ctxFile len=4104192`：4.1MB，vendor 2.46 build ID (`v2.46.0.260424121129`)。
60ms/帧 = 16 FPS，HTP 单车规 SoC 上这个量级可用。

## 2. 跑起来：5 步

### 2.1 Host：构建 APK + test APK

```bash
cd D:/workspace/AICode/qnn/QnnDemo
./gradlew :app:assembleDebug :app:assembleDebugAndroidTest
```

`build.gradle.kts` 里的 `stripSdkHtpJniLibs` preBuild 任务会自动剔除 SDK 2.48
`libQnnHtp*.so`——防止跨版本 handshake `0x80000600`（AEE_EBADCLASS）。

产物：
- `app/build/outputs/apk/debug/app-debug.apk`（93MB，已平台 sign）
- `app/build/outputs/apk/androidTest/debug/app-debug-androidTest.apk`（5.6MB，已平台 sign）

### 2.2 设备：准备 priv-app

平台签名 + `sharedUserId=android.uid.system` 让 app 起在 priv-app，能访问
`/vendor/lib/rfsa/adsp/` 下 vendor 2.46 signed skel。普通 untrusted_app 访问会被
OSError permission denied 拦。

```bash
adb root
adb shell "mount -o rw,remount /system"   # Git Bash 下 /system 会被 MSYS2 转义，
                                           # 用 PowerShell 或 adb shell "mount ..."
adb push app/build/outputs/apk/debug/app-debug.apk \
         /system/priv-app/QnnDemo/QnnDemo.apk
# push libqnndemo.so（CMake 原生重生后的 native 桥接）
adb push app/build/intermediates/merged_native_libs/debug/mergeDebugNativeLibs/out/lib/arm64-v8a/libqnndemo.so \
         /system/priv-app/QnnDemo/lib/arm64/
adb shell "am force-stop com.breeze.qnn"
```

### 2.3 设备：推 vendor 2.46 context binary

```bash
bash docs/setup_yolo.sh
```

产物：`D:/workspace/models/yolo/bin/yolo26n-pose.bin`（4.1MB，QnnYolo 项目
cbg_vendor.cpp 用 SDK 2.47 + vendor signed skel 在设备端生成）。

推送到 `/data/user/0/com.breeze.qnn/files/yolo/yolo26n-pose.bin`。

### 2.4 设备：安装 test APK（可选，验证用）

```bash
adb push app/build/outputs/apk/androidTest/debug/app-debug-androidTest.apk \
         /data/local/tmp/YoloE2E-test.apk
adb shell pm install -r -t /data/local/tmp/YoloE2E-test.apk
```

### 2.5 验证

手动：

```bash
adb shell "am instrument -w -r -e class 'com.breeze.qnn.YoloE2ETest' \
   com.breeze.qnn.test/androidx.test.runner.AndroidJUnitRunner"
```

期望输出：`OK (1 test)` + `PASS: detect person elapsed=60ms` 附近。

或 UI：

1. `adb shell monkey -p com.breeze.qnn -c android.intent.category.LAUNCHER 1`
2. app 启动 → 切到 YOLO tab（MainActivity 里有 BottomNav）
3. 授权摄像头权限
4. 看摄像头画面：若有人进入画面，overlay 会画 bbox + 17 keypoints + FPS

## 3. 重新生成 context binary（可选）

若换 SoC/换机型，或想从最新 ONNX 重新生成 bin：

```bash
bash docs/regenerate_yolo_ctx.sh
```

用 SDK 2.47 `qnn-context-binary-generator` + device signed skel + `pd_session=signed`。
生成的 bin embed build ID = v2.46（与 vendor skel 完全匹配）。

参考源：`D:\workspace\AICode\qnn\QnnYolo\export\cbg_vendor.cpp`（SM8845 真机验证通过）。

## 4. 架构要点

```
┌─ priv-app com.breeze.qnn (平台签名 + sharedUserId=android.uid.system)
│  ├─ jniLibs/arm64-v8a/（由 build.gradle.kts stripSdkHtpJniLibs 剔除 SDK HTP lib）
│  │   ├─ libQnnSystem.so      (vendor 不提供，SDK 自带)
│  │   ├─ libQnnCpu.so         (vendor 不提供)
│  │   ├─ libQnnGpu.so         (vendor 不提供)
│  │   ├─ libGenie.so          (LLM 用)
│  │   ├─ libQnnGenAiTransformer*.so
│  │   └─ libqnndemo.so        (CMake 原生：yolo_jni.cpp 桥 YoloPoseSession)
│  └─ QnnDemo.apk 内 classes.dex (YoloBackend/YoloPostprocessor/YoloFragment)
│
├─ /vendor/lib64/               (vendor 2.46，FastRPC signed skel)
│  ├─ libQnnHtp.so              (v2.46.0.260424121129)
│  ├─ libQnnHtpV81Stub.so       (v2.46)
│  └─ libQnnHtpV81Skel.so       (v2.46, signed) ← HTP 真正干活 lib
│
└─ /vendor/lib/rfsa/adsp/       (FastRPC 搜索路径)
   └─ libQnnHtpV81Skel.so        (同 vendor 2.46)
```

**关键不变量：bin build ID == skel build ID。** 任何一方版本漂移都会触发 FastRPC
handshake 失败：

| 症状 | 错误码 | 根因 |
|------|--------|------|
| AEE_EBADCLASS | 0x80000600 | SDK 2.48 stub + vendor 2.46 skel 跨版本混装 |
| context too new | 0x1388 "Using newer context binary on old SDK" | SDK 2.48/2.49 bin + vendor 2.46 skel，bin embed build ID 比 skel 新 |
| permission denied | - | app 不是 priv-app，访问 /vendor/lib/rfsa/adsp/ 被 SELinux 拦 |
| loadRemoteSymbols fail | - | libQnnSystem/libQnnHtp 等 vendor 不提供 lib 缺失 |

**dlopen candidate chain**（yolo_pose.cpp:openLib）：

1. 完整 nativeLibDir 路径：`/system/priv-app/QnnDemo/lib/arm64/libQnnHtp.so`（若 jniLibs 里有，已被 strip 剔除）
2. 库名：`dlopen("libQnnHtp.so")` → linker 宽松搜索 `/system/priv-app/QnnDemo/lib/arm64` → `/vendor/lib64` → `/vendor/lib/rfsa/adsp`
3. 由于 SDK HTP lib 都被 strip 掉了，candidate chain 必掉到 /vendor/lib64 → 加载 vendor 2.46 libQnnHtp.so + libQnnHtpV81Stub.so → 匹配 vendor 2.46 skel → OK

**ADSP_LIBRARY_PATH** 在 nativeInit 之前 setenv：

```cpp
setenv("ADSP_LIBRARY_PATH", "/vendor/lib/rfsa/adsp;/vendor/dsp/cdsp", 1);
dlopen("libcdsprpc.so", RTLD_NOW | RTLD_GLOBAL);  // 预先打开 FastRPC 客户端
```

让 FastRPC 直接去 /vendor 找 skel，而不是走 Android linker 的默认搜索路径。

## 5. 已知限制

- **HTP 单车规 SoC 上 16 FPS**：YOLO26-pose 640×640 on 8845 V81 = 60ms。实时摄像头勉强够流畅。
- **摄像头要求**：后置摄像头 (`CameraSelector.DEFAULT_BACK_CAMERA`)。前置摄像头画面 Mirror 未处理，如需镜像画在 YoloOverlayView。
- **NV21 / ImageAnalysis STRATEGY_KEEP_ONLY_LATEST**：ImageProxy 关闭不及时时可能漏帧，但对 demo 影响可忽略。
- **多 anchor 同一个人可能挤出重复框**：JNI `decodePose` 已做 NMS，Kotlin `YoloPostprocessor.nms` 再兜底一次（默认 IoU 阈值 0.45）。若 T13 实测无重复框，可去掉 JNI 内的二次 NMS 提速。

## 6. E2E 测试覆盖

`YoloE2ETest`（AndroidJUnit4，instrumented）：

| Test | 断言 |
|------|------|
| `detect person in static test image` | 从 app assets 的 `yolo_test.jpg` 推理 → 检出 ≥1 个 person → 每个 person 有 ≥5 个可见 keypoint 且恰好 17 keypoints |

前置 (`assumeTrue`)：bin ready (`modelReady=true`)。APK assets 里也有 bin，`@Before`
里 `ensureBinFromAssets()` 在 filesDir bin 缺或 <3.5MB 时把 assets 里的 bin 拷过去，
保证 CI 无需手动推 bin 也能跑通。

跑法：`bash docs/run_e2e_tests.sh yolo`（脚本用 `-Pandroid.testInstrumentationRunnerArguments.class=` 注跑单测）。

## 7. 故障排查

### 7.1 `0x80000600 AEE_EBADCLASS`

SDK 2.48 stub + vendor 2.46 skel 跨版本 FastRPC handshake 失败。

排查：
```bash
# 看 openLib candidate chain 实际加载了哪个 HTP lib
adb logcat -d | grep "YoloPose: loaded"
```

期望：`loaded libQnnHtpV81Stub.so` 与 `loaded libQnnHtp.so`（bare name，由 linker 宽松搜索解析到 /vendor/lib64）。
若看到 `loaded /system/priv-app/QnnDemo/lib/arm64/libQnnHtp.so` → SDK lib 没被 strip 掉，需重建 APK。

修：build.gradle.kts `stripSdkHtpJniLibs` preBuild 任务已剔除 SDK HTP lib。
若 setup_demo.ps1 重装了 SDK HTP lib，下次构建会自动剔除。

### 7.2 `0x1388 Using newer context binary on old SDK`

bin build ID 比 vendor skel 新。重新生成 vendor 2.46 bin（`bash docs/regenerate_yolo_ctx.sh`）。

### 7.3 `模型未加载` Toast / modelReady=false

`/data/user/0/com.breeze.qnn/files/yolo/yolo26n-pose.bin` 不存在或太小（<3.5MB）。

修：`bash docs/setup_yolo.sh` 推 bin。

### 7.4 `Permission denied` 加载 /vendor/lib/rfsa/adsp/libQnnHtpV81Skel.so

app 不是 priv-app。确保 `/system/priv-app/QnnDemo/QnnDemo.apk` 是新版（平台签名 +
sharedUserId=android.uid.system）。

```bash
adb shell "dumpsys package com.breeze.qnn | grep -E 'sharedUser|signingInfo'"
```

### 7.5 E2E 测试进不去（INSTRUMENTATION_FAILED）

检查 test APK  instrumentation runner 是否正确注册：

```bash
adb shell "pm list instrumentation"
# 期望：com.breeze.qnn.test/androidx.test.runner.AndroidJUnitRunner (target=com.breeze.qnn)
```

Runner 是 `androidx.test.runner.AndroidJUnitRunner`（不是 `AndroidJUnit4`）。
若 instrumentation 名对不上，重新跑 `./gradlew :app:assembleDebugAndroidTest` + `pm install -r -t`。

## 8. 提交 & 分支策略

YOLO-pose 真机验证逻辑上属于 **T13 of the YOLO-pose-8845 plan**。
commit message 形如：

```
docs(yolo): T13 真机验证 + 使用指南

- 真机 8845 (87e80eb) HTP 端到端跑通：vendor 2.46 bin + vendor 2.46 stub/skel，
  60ms/帧，检出 1 person (conf 0.807)
- strip SDK 2.48 HTP lib (jniLibs + build.gradle.kts preBuild 安全网)
- 替换 bin 为 vendor 2.46 build (4.1MB v2.46.0.260424121129)
- 补 YoloBackend.isNullOrEmpty 处理 JNI 返回 null 的兼容
- 新增 docs/YOLO-使用指南.md
```

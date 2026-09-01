# YOLO 姿态估计 8845 HTP 实时识别 设计文档

> **For agentic workers:** 本 spec 由 brainstorming skill 生成，实施用 superpowers:subagent-driven-development（推荐）或 superpowers:executing-plans 逐 task 实现。步骤用 checkbox `- [ ]` 跟踪。

## 1. Goal

在 8845 Hexagon V81 上跑 YOLO26-pose 模型，**通过手机摄像头实时识别画面中的人体姿态**：检测人体边界框 + 17 COCO keypoints（24 点输出）× 多人，并在 PreviewView 上叠加绘制骨架。

- 后端：QNN Direct API + fp16 context binary 走 HTP（复用 NerBackend 那套 Infrastructure）
- UI：MainActivity BottomNav 加第 5 个 tab（YOLO），实时预览 + 叠加框 + 检测列表 + FPS/延迟
- 性能目标：≥ 15 FPS（8845 HTP fp16 rame-to-overlay 全过程 ≤ 66ms）
- 模型大小目标：≤ 60MB（fp16 context binary，预计比 NER 的 205MB 小很多）

## 2. 模型与转换

### 2.1 模型事实

| 项 | 值 | 来源 |
|---|---|---|
| 文件 | `D:/workspace/models/yolo/yolo26n-pose.pt` (7.5MB) | 用户提供 |
| 格式 | Ultralytics 新版 zip-pt（best/data.pkl + 权重张量） | 文件头 `PK\\x03\\x04` |
| 框架 | ultralytics **8.3.222** | data.pkl 中 `version: 8.3.222` |
| Python 类 | `ultralytics.nn.tasks.PoseModel` | data.pkl |
| 项目 | `YOLO26-pose` | data.pkl `project` 字段 |
| 训练集 | COCO-pose (80 类, 24 keypoint) | data.pkl `dataset: coco-pose.yaml` |
| 输入 | 1×3×640×640, batch 训练用 128 | data.pkl `imgsz: 640, batch: 128` |
| Head | **end2end**（直接输出解码框，无 NMS） | 文件名 `o2m1-cls_w2-rle1-pose24-box75-df` + pickle 中 `end2end: True` |
| 优化 | one-to-many (1 个 N 候选) | `o2m1` |

COCO-pose 80 类（person + 79 类物体），24 keypoints（比标准 COCO 17 点多 7 个，是 YOLO26 扩展点集）。

### 2.2 转换 pipeline（在 WSL 跑，per memory QAIRT converter 只能在 WSL）

```
.pt ──► .onnx ──► .dlc ──► .bin (context binary, fp16)
   ultralytics    qnn-converter   qnn-context-binary-gen + HTP backend ext
   export         --float_bitwidth 32   --retrieve_context -fp16_relaxed_precision 1
```

**关键依赖环境**（per 项目 memory）：
- WSL + Python 3.10 + libc++（QAIRT converter 是 Linux ELF）
- `pip install torch torchvision ultralytics`（建议 2.2 + CUDA 11.8 / CPU 版）
- SDK 2.48 qnn-converter / qnn-context-binary-generator（QAIRT 2.48.40.260702）

### 2.3 各阶段命令速查

**Step 1: .pt → ONNX**（在 WSL）
```bash
cd /mnt/d/workspace/models/yolo
yolo export model=yolo26n-pose.pt format=onnx imgsz=640,640 simplify=True dynamic=False half=False nms=False opset=17
# end2end head 必须 nms=False（NMS 已烘焙进 head 输出，再 export 会重复/报错）
```
产物：`yolo26n-pose.onnx` + `yolo26n-pose.onnx_data`（可能 ~30-50MB）

**Step 2: ONNX → DLC**（在 WSL，用 SDK 2.48）
```bash
export PATH=/c/Qualcomm/AIStack/QAIRT/2.48.40.260702/bin/x86_64-windows-msvc:$PATH
export LD_LIBRARY_PATH=/c/Qualcomm/AIStack/QAIRT/2.48.40.260702/lib/x86_64-windows-msvc:$LD_LIBRARY_PATH
qnn-converter --input_network yolo26n-pose.onnx --output_path yolo26n-pose.dlc --float_bitwidth 32 --quantization_overrides /dev/null
```
注意：WSL 不能直接调 Windows exe，要走 `cmd.exe /c` 桥接或复制 SDk 到 WSL 路径。参考 `docs/NER-使用指南.md` 第九节的设备端转换做法。

**Step 3: DLC → fp16 context binary**（在设备端，跟 NER 一样）
```bash
# 把 .dlc push 到设备
adb push yolo26n-pose.dlc /data/local/tmp/yolo.dlc

# 设备端生成 context binary（HTP backend ext, fp16_relaxed_precision:1）
cat > /data/local/tmp/yolo-bin.json <<'EOF'
{
  "graphs": [{"graph_names": ["yolo26n-pose"]}],
  "backend": {"HTP": {"fp16_relaxed_precision": 1, "soc_id": 97, "devices": [{"dsp_arch": "v81", "cores": [{"id": 0}]}]}}
}
EOF
$QAIRT/bin/aarch64-android/qnn-context-binary-generator \
  --backend $QAIRT/lib/aarch64-android/libQnnHtp.so \
  --model /data/local/tmp/yolo.dlc \
  --output_dir /data/local/tmp/yolo-out \
  --binary_file yolo26n-pose \
  --config_file /data/local/tmp/yolo-bin.json
# 产物 /data/local/tmp/yolo-out/yolo26n-pose.bin（约 20-40MB）
```

### 2.4 风险：end2end head 可能含 QNN 不支持的 op

现代 YOLO end2end head 常见 op：`IndexSelect`、`Gather`、`Einsum`/`BMM`、`Square`、`Pow`、`Softmax`、`TopK`、 Hungarian 相关 op。经验：90% 能直通；若 converter 报 `op X not supported`，解法：
- (a) 去掉 end2end head，改用标准 `Detect(Pose)` 头 + 导出时设 `nms=False` + 在 native 手工 NMS + keypoint decode。**这是备选方案B**。

Step 1 之前先用 `qnn-converter --dry_run` 之类的选项探一下 op 支持（若没有 dry_run，先试最小 batch convert，失败再看 log）。

## 3. 架构

复用 NER 的三层结构：`InferenceEngine` (已存在) + `XxxBackend` (Kotlin 编排) + native (JNI 进入 InferenceEngine)。新增：

```
[CameraX]                          [Detection Overlay]
    │ ImageAnalysis /preview            ▲
    ▼                                   │ byte[] → Box[]
[YoloPreprocess]                       │
    │ ByteArray (1x3x640x640 NCHW fp32)│
    ▼                                   │
[InferenceEngine.executeWithOutput]─────┘
    │ List<ByteArray> (raw model outputs)
    ▼
[YoloPostprocess] → List<Detection> (box + cls + 24 kpts + score)
    │
    ▼
[YoloViewModel.state.publish] → [YoloFragment.render]
```

### 3.1 新增文件

```
app/src/main/java/com/breeze/qnn/
├── yolo/                                        (NEW package)
│   ├── YoloBackend.kt                           (编排：loadModel / detect / close)
│   ├── YoloViewModel.kt                         (状态：MODEL_MISSING / LOADING / RUNNING)
│   ├── YoloResult.kt                            (data class: detections, fps, latency)
│   └── Detection.kt                             (data class: box(xyxy), cls, score, keypoints)
├── ui/
│   ├── YoloFragment.kt                          (BottomNav 第 5 个 tab)
│   └── YoloOverlayView.kt                       (自定义 View，画 box + keypoint 骨架)
app/src/main/res/
├── layout/
│   └── fragment_yolo.xml                        (PreviewView + RecyclerView + 统计)
├── menu/
│   └── bottom_nav.xml                           (+ nav_yolo 一项)
└── values/
    └── strings.xml                              (+ yolo 文案)
app/src/main/
├── AndroidManifest.xml                          (+ CAMERA + android.hardware.camera)
├── cpp/
│   ├── yolo_prepost.cpp / yolo_prepost.hpp      (NEW: 可选 native pre+post。若全用 Kotlin 也可省)
├── CMakeLists.txt                               (+ yolo_prepost.cpp)
docs/
├── setup_yolo.sh                                (push .bin 到设备)
└── YOLO-使用指南.md                              (转换 + 跑通 + 风险)
app/src/androidTest/.../yolo/
└── YoloE2ETest.kt                               (喂静态 test 图, 断言检测到 person)
```

### 3.2 复用已有

- `InferenceEngine.loadContextBinary(binPath, HTP)` — 直接复用，Kotlin 一行
- `InferenceEngine.executeWithOutput(graphName, inputs)` — 直接复用
- `QnnNative.Backend` — 复用
- `NerBackend` 的整体编排模式（modelReady / loadModel / 状态机 / warmup）
- 统签插件 + priv-app + HTP stub/skel 路径 — 全复用

## 4. 各组件详细

### 4.1 YoloBackend (Kotlin 编排)

```kotlin
class YoloBackend(context: Context) : AutoCloseable {
    private val engine = InferenceEngine(context)
    var modelKind: String = ""        // "fp16 ctx" / "fp32 dlc"
    private var graphName: String = ""
    private var inputNames: List<String> = emptyList()
    private lateinit var inputShape: LongArray   // [1,3,640,640]
    private lateinit var outputInfo: List<GraphInfo>  // 后处理依赖
    var ready: Boolean = false; private set

    fun modelReady(): Boolean = ...   // 同 NerBackend.modelReady

    suspend fun loadModel(): Boolean  // HTP 优先用 ctx；CPU 回退 dlc

    /**
     * 喂一张 RGB float tensor (NCHW, /255, mean/std 已在 native 或 Kotlin 处理好)。
     * 返回原始 output bytes，交给 postprocess 解码。
     */
    suspend fun infer(input: ByteArray): List<ByteArray>? =
        engine.executeWithOutput(graphName, listOf(input))

    fun warmup() { ... }
    override fun close() { engine.close() }
}
```

### 4.2 Pre-process (Kotlin，起步)

输入：CameraX `ImageProxy` (YUV_420_888, ImageFormat.YUV_420_888)。
步骤：
1. 取 Y plane (Y + U + V) → 转 RGB byte[]
2. Letterbox resize 到 640×640 (保持比例，短边 padding 114 半均值，per YOLO 默认)
3. HWC → CHW 重排
4. fp32 → float[] (1×3×640×640, /255, mean=[0.485,0.456,0.406], std=[0.229,0.224,0.225] — per YOLOv8 default 也可不归一化直接 /255)
5. float[] → ByteArray (4-byte LE fp32)
6. 检查输入名对齐

> YAGNI：起步用最简 `/255 + reshape` 不做归一化，看精度。要是不太行再加 mean/std。

### 4.3 Post-process (Kotlin，起步)

输出（end2end head，3 个张量，典型形状）：
- `dets`: `[1, num_dets, 6]` = (cx, cy, w, h, cls_conf, cls_id) — top-K 已 NMS
- `kpts`: `[1, num_dets, 24, 3]` = (x, y, visible) per keypoint
- `num_dets` 是小维度（理论上限如 300），来自 TopK

若导出 ONNX 后 Shapes 不同，按实际改。

```kotlin
data class Detection(
    val cls: Int, val clsName: String,
    val score: Float,
    val box: RectF,           // xyxy in 640x640 tensor space
    val keypoints: List<PointF>  // 24 个 (x,y)
)

fun parse(raw: List<ByteArray>, names: List<String>, confThr: Float, iouThr: Float): List<Detection> {
    // 1. 读 dets: [1, N, 6] — 过滤 confThr
    // 2. NMS (IoU, 同类别)
    // 3. 读 kpts: [1, N, 24, 3] — 过滤 visible > 0.5
    // 4. 框和 keypoint 坐标从 letterbox tensor space 反演回原始预览分辨率
}
```

### 4.4 YoloFragment + YoloOverlayView

- `YoloFragment`: 持有 PreviewView (CameraX) + Overlay + RecyclerView + FPS TextView
- `YoloOverlayView`: 自定义 View，绑定到 PreviewView 坐标系，draw 框 + 骨架线（24 keypoint 按 YOLO26 pose 定义连边）
- Lifecycle 用 CameraX `ProcessCameraProvider` + `Preview` + `ImageAnalysis` (STRATEGY_KEEP_ONLY_LATEST)
- 推理在 ImageAnalysis callback 里异步跑（协程），避免阻塞
- 每帧：ImageProxy → 转 ByteBuffer → 推理 → decode → postValue → overlay 重绘

### 4.5 MainActivity + permissions

- `AndroidManifest.xml` 加 `<uses-permission android:name="android.permission.CAMERA"/>` + `<uses-feature android:name="android.hardware.camera" android:required="false"/>`
- 运行时权限：YoloFragment 用 `registerForActivityResult(RequestPermission())`
- BottomNav: 加 `<item android:id="@+id/nav_yolo" android:title="YOLO"/>`
- MainActivity: `R.id.nav_yolo -> switchTo(YoloFragment())`
- strings.xml: `section_yolo`, `btn_start_camera`, `hint_permission`, etc.

### 4.6 build.gradle.kts

```kotlin
dependencies {
    // ... existing ...
    implementation("androidx.camera:core:1.3.0")
    implementation("androidx.camera:lifecycle:1.3.0")
    implementation("androidx.camera:view:1.3.0")
}
```

noCompress 已对 `bin` 生效（见 androidResources.noCompress += listOf("dlc", "bin")），context binary 直接 assets 加载或产后 push filesDir。

## 5. 部署与运行

### 5.1 模型产物落位

两条路：
- **方案 a（推荐，同 NER）**：产后用 `adb push yolo26n-pose.bin /data/user/0/com.breeze.qnn/files/yolo/yolo26n-pose.bin`
- **方案 b**：assets/yolo26n-pose.bin，首次启动 copy 到 filesDir

选 a：沿 NER 模板，setup 脚本能复用 `adb push ... chmod 644` 模式。

### 5.2 setup_yolo.sh

```bash
#!/bin/bash
# bash docs/setup_yolo.sh
# 把 fp16 context binary push 到设备
SRC="D:/workspace/models/yolo/bin/yolo26n-pose.bin"
DST="/data/user/0/com.breeze.qnn/files/yolo/yolo26n-pose.bin"
adb push "$SRC" "$DST"
adb shell "chmod 644 $DST"
```

### 5.3 验证步骤

1. `bash docs/setup_yolo.sh` （若离线：用adb install或手动adb push）
2. 装 APK 启动 → YOLO tab
3. 授权 CAMERA → 看到摄像头预览
4. 识别到 person → 看到 bbox + 骨架
5. FPS ≥ 15，延迟 ≤ 66ms
6. `./gradlew :app:connectedDebugAndroidTest -Ptest=YoloE2ETest` 喂静态图

## 6. E2E 测试

`app/src/androidTest/java/com/breeze/qnn/yolo/YoloE2ETest.kt`：
- 喂一张包含单人站立的 test 图（从 assets/yolo_test.jpg 读取）
- Instrumentation → YoloBackend.detect(bitmap) → 断言返回 ≥ 1 个 detection, cls == person, box 合理, keypoints 数 == 24
- 跑通标准：至少检测到 1 个 person 框 + 至少 5 个可见 keypoint

提供测试图：从 COCO val 取一张单人图，压缩成 assets/yolo_test.jpg (480×640 即可)。

## 7. 风险与缓解

| 风险 | 可能性 | 缓解 |
|---|---|---|
| end2end head 含 QNN 不支持 op | 中 | Step 1 之前先用 qnn-converter 探伤；失败退回标准 Detect(Pose) 头 + 手工 NMS（方案 B） |
| 24 keypoints schema 与 YOLO26 自定义格式不符 | 中 | 导出 ONNX 后 `netron.app` 可视化 + 读 ultralytics PoseModel 源码确认 kpt 排列 |
| Camera2/PreviewView 坐标系与 overlay 不一致 | 低 | 用 `PreviewView.scaleType="fitCenter"` + 在 onDraw 用矩阵变换镜像 |
| 640×640 letterbox 反演坐标偏差 | 低 | 反演时用原始缩放比 (min(W,H)/640) × padding 偏移 |
| 实时 FPS 不够 | 中 | 起步 Kotlin 实现，不够再下沉 native（prepost.cpp） |
| 设备未 root，HTP skel 加载失败 | 低 | 复用已有统签 priv-app 路径（BERT NER 已验证） |

## 8. 不做的事（YAGNI）

- **不做 Seg**：模型是 pose，不做实例分割
- **不做视频录制**：只做实时预览 + 识别，录制是另一个 feature
- **不做多后端切换**：YOLO 只跑 HTP（CPU 备选仅在 HTP 失败时 fallback）
- **不做 FLIR/红外摄像头**：只用后置 RGB
- **不做模型量化**：7.5MB 的 .pt 很小，fp16 context binary 预计 ≤60MB，无需 INT4（BERT NER 已验证 PTQ 行不通，这个更小应该 fp16 即可）

## 9. 实施 Task 拆分（预排）

T0 前置：环境准备 — 装 torch/ultralytics/onnx（WSL）、确认 SDK 2.48 converter 工作（per memory QAIRT converter 只能在 WSL）
T1: 建 feature 分支 `yolo-pose-8845`
T2: .pt → ONNX (WSL 跑 ultralytics export)
T3: ONNX → DLC (WSL/qnn-converter)
T4: DLC → fp16 context binary (设备端 qnn-context-binary-generator)
T5: 注册 YoloBackend (InferenceEngine 复用) + Kotlin 状态机
T6: Preprocess (Kotlin, ImageProxy → fp32 NCHW ByteArray)
T7: Postprocess (Kotlin, output bytes → List<Detection>)
T8: YoloFragment + YoloOverlayView + 布局
T9: AndroidManifest + CameraX 依赖 + 运行时权限
T10: MainActivity BottomNav + nav_yolo
T11: setup_yolo.sh + 验证脚本
T12: YoloE2ETest (assets 测试图 + 断言)
T13: 真机验证 + 文档 + PR

依赖：T0→T2→T3→T4→T5→T6→T7→T8→T9→T10→T11→T12→T13（线性，T6/T7 在 T5 之后可并行走）

## 10. 验收

- [ ] APK 多一个 "YOLO" BottomNav tab，不闪退
- [ ] Camera 预览 ≤ 1s 出图
- [ ] 人体 bbox + 24 keypoints 实时叠加
- [ ] FPS ≥ 15, latency ≤ 66ms
- [ ] CAMERA 运行时权限处理正确（拒绝 → 提示；授权 → 启动预览）
- [ ] 切到 Chat/NER/Settings 再切回 YOLO 不重启预览
- [ ] 5/5 E2E PASS
- [ ] PR 提交 + 文档
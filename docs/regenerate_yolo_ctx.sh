#!/bin/bash
# =============================================================================
# regenerate_yolo_ctx.sh
#
# 在 8845 设备上用 SDK 2.47 的 qnn-context-binary-generator + 设备 signed skel
# (pd_session=signed) 重新生成 yolo26n-pose.bin。
#
# 背景：
#   - setup_yolo.sh 原先推的 bin 是 SDK 2.49 生成，embed build ID = v2.49。
#     设备 vendor skel 是 v2.46，libQnnSystemContext 报 0x1388
#     "Using newer context binary on old SDK"。
#   - 之前的 patched bin（把 build ID 字符串手工改成 v2.48）仍失败 —— 因为
#     SDK 2.49 ctx-gen embed 的不只是 build ID 字符串，还有 integer minor 等字段。
#   - qnnyolo_247/cbg_vendor  BinaryGenerator (vendor skel 版) 用 SDK 2.47
#     + pd_session=signed 生成过 vendor bin（4.1MB，build ID v2.46 匹配 skel）。
#   - 该 vendor bin 在设备上工作 —— 因为 bin 的 buildId == skel 的 buildId，
#     版本检查通过。
#
# 本脚本用 SDK 2.47 标准 qnn-context-binary-generator 复刻这条 recipe：
#   - 用 signed PD（即加载 vendor 2.46 skel）
#   - bin 内 buildId 字段被自动填成 skel 的 build ID (v2.46)
#   - 与设备 vendor skel 完全匹配 → 0x1388 消失
#
# 用法：
#   bash docs/regenerate_yolo_ctx.sh
# =============================================================================
set -eu
export MSYS_NO_PATHCONV=1

SDK="D:/workspace/docs/qti/qairt/2.47.1.260610"
DEV=/data/local/tmp/yolo_ctxgen47
PKG="com.breeze.qnn"

# 1) 设备就绪
if ! timeout 5 adb devices | grep -q 'device$'; then
    echo "[ERR] 设备未连接"; exit 1
fi
adb root >/dev/null 2>&1 || echo "[WARN] adb root 未拿到"

# 2) mkdir + 推 binary generator + 依赖库（signed PD 用不到 SDK unsigned stub，
#    因为 libQnnHtp.so 直接加载 vendor signed skel）
adb shell "mkdir -p $DEV/bin $DEV/lib $DEV/dlc $DEV/cfg"
adb push "$SDK/bin/aarch64-android/qnn-context-binary-generator" "$DEV/qnn-ctx-gen"
for lib in libQnnHtp.so libQnnModelDlc.so libQnnHtpNetRunExtensions.so libQnnSystem.so; do
    adb push "$SDK/lib/aarch64-android/$lib" "$DEV/lib/"
done

# 3) 推 DLC + configs
adb push "D:/workspace/models/yolo/dlc_bin/yolo26n-pose.dlc" "$DEV/dlc/"
adb push "D:/workspace/models/yolo/bin/backend_ext.json" "$DEV/cfg/"
adb push "D:/workspace/models/yolo/bin/htp_signed.json" "$DEV/cfg/"

adb shell "chmod +x $DEV/qnn-ctx-gen"

# 4) 设备端执行 graph-prepare（V81 HTP + signed PD）
echo "[INFO] 设备端生成 fp16 ctx binary（约 1-3 分钟）..."
adb shell "cd $DEV && LD_LIBRARY_PATH=$DEV/lib ./qnn-ctx-gen \
    --backend lib/libQnnHtp.so \
    --model lib/libQnnModelDlc.so \
    --dlc_path dlc/yolo26n-pose.dlc \
    --binary_file yolo26n-pose \
    --output_dir ctxout \
    --config_file cfg/backend_ext.json" 2>&1 | tail -20

# 5) 验证 bin 已生成 + build ID 是否匹配 vendor skel (v2.46)
echo "--- bin 生成结果 ---"
adb shell "ls -la $DEV/ctxout/" 2>&1
adb shell "strings $DEV/ctxout/yolo26n-pose.bin 2>/dev/null | grep -E 'v2\.4[6789]'" 2>&1 | head -5

# 6) 拉回 host 替换
adb pull "$DEV/ctxout/yolo26n-pose.bin" "D:/workspace/models/yolo/bin/yolo26n-pose.bin" 2>&1 | tail -2
echo ""
wc -c "D:/workspace/models/yolo/bin/yolo26n-pose.bin"
python -c "
p = r'D:\workspace\models\yolo\bin\yolo26n-pose.bin'
with open(p,'rb') as f: data = f.read()
i46 = data.find(b'v2.46.0.260424121129')
i47 = data.find(b'v2.47.1.260610072528')
i48 = data.find(b'v2.48.40.260702151143')
i49 = data.find(b'v2.49.40.260810104044')
print(f'new bin: len={len(data)} v2.46@{i46} v2.47@{i47} v2.48@{i48} v2.49@{i49}')
"

# 7) 同步替换到 test APK assets
SRC="D:/workspace/models/yolo/bin/yolo26n-pose.bin"
DST_APK="D:/workspace/AICode/qnn/QnnDemo/app/src/androidTest/assets/yolo26n-pose.bin"
cp "$SRC" "$DST_APK"
rm -f "D:/workspace/models/yolo/bin/yolo26n-pose-patched.bin"
rm -f "D:/workspace/models/yolo/bin/yolo26n-pose-sd247-vendor.bin" "D:/workspace/models/yolo/bin/yolo26n-pose-sd249-unsigned.bin"
wc -c "$DST_APK"

echo ""
echo "[OK] YOLO ctx binary 已用 SDK 2.47 + signed PD + vendor skel 重生"

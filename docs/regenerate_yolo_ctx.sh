#!/bin/bash
# =============================================================================
# regenerate_yolo_ctx.sh
#
# 在 8845 设备上用 SDK 2.48 的 qnn-context-binary-generator 重新生成
# yolo26n-pose.bin（unsigned PD + SDK 2.48 强配，与 LLM / NER 同栈）。
#
# 背景：
#   - setup_yolo.sh 原先推的 bin 是 SDK 2.47 + vendor signed PD 生成，build ID
#     = v2.46（设备 vendor skel 版本）。但 unified 栈的 libQnnSystem.so
#     /libQnnHtp.so /libQnnHtpV81Stub.so 全是 SDK 2.48（在 jniLibs 里），
#     libQnnHtpV81Skel.so 也是 SDK 2.48 unsigned 版。跨版本 stub/skel
#     FastRPC handshake 直接 AEE_EBADCLASS 0x80000600；即使过了 handshake，
#     ctx binary 的 build ID 与 skel 不符还会 0x1388 "newer binary on old SDK"。
#   - qairt_runtime.cpp 在 deviceCreate 时硬编码 useSignedProcessDomain=false
#     → FastRPC 用 unsigned skel（jniLibs 里的 SDK 2.48 libQnnHtpV81Skel.so）。
#   - 故 ctx binary 必须用 SDK 2.48 + unsigned PD 路径生成，让 bin 内 build ID
#     = v2.48.40.260702，与运行时加载的 skel 完全匹配 → 0x1388 消失。
#
# 用法：
#   bash docs/regenerate_yolo_ctx.sh
# =============================================================================
set -eu
export MSYS_NO_PATHCONV=1

SDK="D:/workspace/docs/qti/qairt/2.48.40.260702"
DEV=/data/local/tmp/yolo_ctxgen48
PKG="com.breeze.qnn"

# 1) 设备就绪
if ! timeout 5 adb devices | grep -q 'device$'; then
    echo "[ERR] 设备未连接"; exit 1
fi
adb root >/dev/null 2>&1 || echo "[WARN] adb root 未拿到"

# 2) mkdir + 推 binary generator + 依赖库
#    unsigned PD：$DEV/lib 必须含完整的 SDK 2.48 host + DSP 系列 .so
#    （libQnnHtp.so 调 libQnnHtpV81Stub.so 与 libQnnHtpPrepare.so，再 FastRPC
#    拉 libQnnHtpV81Skel.so。这条链缺一不可，少了 stub/prepare/skel 都会
#    Device Creation failure；vendor 2.46 的 stub 跨版本会 AEE_EBADCLASS）。
SKEL_SRC="D:/workspace/docs/qti/qairt/2.48.40.260702/lib/hexagon-v81/unsigned/libQnnHtpV81Skel.so"
adb shell "mkdir -p $DEV/bin $DEV/lib $DEV/dlc $DEV/cfg"
adb push "$SDK/bin/aarch64-android/qnn-context-binary-generator" "$DEV/qnn-ctx-gen"
for lib in libQnnHtp.so libQnnHtpV81Stub.so libQnnHtpPrepare.so \
           libQnnHtpV81CalculatorStub.so libQnnHtpNetRunExtensions.so \
           libQnnSystem.so libQnnModelDlc.so; do
    adb push "$SDK/lib/aarch64-android/$lib" "$DEV/lib/"
done
adb push "$SKEL_SRC" "$DEV/lib/"

# 3) 推 DLC + configs
adb push "D:/workspace/models/yolo/dlc_bin/yolo26n-pose.dlc" "$DEV/dlc/"
# ext config 用双层结构（跟 NER fp16 ctx binary 同 recipe）：
#   outer: backend_extensions.block → 指向 inner
#   inner: graphs + devices 真正控制 vtcm_mb / soc_model / cores
# ctx-gen 读 --config_file 时只接受 backend_extensions 块，raw graphs/devices
# 一律 Unknown Key → Device Creation failure。
adb push "D:/workspace/models/yolo/bin/htp_ext_outer.json" "$DEV/cfg/htp_ext.json"
adb push "D:/workspace/models/yolo/bin/htp_ext_inner.json" "$DEV/cfg/htp_ext_inner.json"

adb shell "chmod +x $DEV/qnn-ctx-gen"

# 4) 设备端执行 graph-prepare（V81 HTP + unsigned PD）
#    设备端 wrapper 脚本 cbg_run_yolo.sh 用 export 設 env 让 ADSP_LIBRARY_PATH
#    里的 ';' 在 /system/bin/sh 自己的上下文里当 PATH 分隔符（不被 adb shell
#    当成 command separator 拆散）。wrapper 自己 cd + exec ./qnn-ctx-gen，
#    所以这里只调 sh 跑 wrapper。
echo "[INFO] 设备端生成 fp16 ctx binary（约 1-3 分钟）..."
adb shell "sh /data/local/tmp/cbg_run_yolo.sh" 2>&1 | tail -25

# 5) 验证 bin 已生成 + build ID 是否匹配 SDK 2.48 skel
echo "--- bin 生成结果 ---"
adb shell "ls -la $DEV/ctxout/" 2>&1
if [ ! -f "$DEV/ctxout/yolo26n-pose.bin" ]; then
    echo "[FATAL] ctx binary 未生成，Device Creation failure，退出";
    adb shell "logcat -d -s QnnDemo:V QnnHtp:V QnnDsp:V 2>/dev/null | tail -40" || true
    exit 1
fi
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
assert i48 != -1, '期望 build ID v2.48，未找到'
assert i46 == -1 and i47 == -1 and i49 == -1, '混进了旧 build ID'
print('assert OK: bin build ID == v2.48')
"

# 7) 同步替换到 test APK assets
SRC="D:/workspace/models/yolo/bin/yolo26n-pose.bin"
DST_APK="D:/workspace/AICode/qnn/QnnDemo/app/src/androidTest/assets/yolo26n-pose.bin"
cp "$SRC" "$DST_APK"
wc -c "$DST_APK"

if [ ! -s "$DST_APK" ] || [ $(stat -c%s "$DST_APK") -lt 100000 ]; then
    echo "[FATAL] 测试 APK assets 复制失败"; exit 1
fi

echo ""
echo "[OK] YOLO ctx binary 已用 SDK 2.48 + unsigned PD 重生（build ID v2.48）"

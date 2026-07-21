#!/bin/bash
# =============================================================================
# 从设备 /vendor/lib64/ 拉取匹配版本的 QNN HTP 库并覆盖到 jniLibs
#
# 用途：SDK 版本（如 2.48）跟 vendor firmware（如 2.46）不匹配时，FastRPC
#      handshake 会 AEE_ENOSUCHMOD (0x80000406)。用 vendor 版覆盖即对齐。
#
# 用法：
#   bash docs/pull_vendor_htp_libs.sh
# =============================================================================
set -eu
export MSYS_NO_PATHCONV=1

PROJECT_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
JNI_DIR="$PROJECT_ROOT/app/src/main/jniLibs/arm64-v8a"

if ! timeout 5 adb devices | grep -q 'device$'; then
    echo "[ERR] 未检测到 adb 设备"
    exit 1
fi

echo "[INFO] 从 /vendor/lib64/ 拉取 QNN HTP 库到 $JNI_DIR/"
adb pull /vendor/lib64/libQnnHtp.so           "$JNI_DIR/"
adb pull /vendor/lib64/libQnnHtpPrepare.so    "$JNI_DIR/"
adb pull /vendor/lib64/libQnnHtpV81Stub.so    "$JNI_DIR/"

echo ""
echo "[INFO] 版本核对："
for f in libQnnHtp.so libQnnHtpPrepare.so libQnnHtpV81Stub.so libQnnSystem.so libQnnCpu.so; do
    if [ -f "$JNI_DIR/$f" ]; then
        v=$(grep -aoE 'AISW_VERSION: [0-9.]+' "$JNI_DIR/$f" | head -1)
        printf "  %-30s %s\n" "$f" "${v:-未标注}"
    fi
done

echo ""
echo "[OK] 完成。libQnnSystem/libQnnCpu 保留 SDK 版本（vendor 未提供 CPU、"
echo "     libQnnSystem 需要 composeGraphs 能力，SDK 版本才有）"

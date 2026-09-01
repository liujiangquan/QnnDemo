#!/bin/bash
# =============================================================================
# setup_yolo.sh
#
# 把 YOLO pose fp16 context binary push 到设备 app 私有目录。
#
# 用法：
#   bash docs/setup_yolo.sh
#
# 产物来源：D:/workspace/models/yolo/bin/yolo26n-pose.bin
#   由 qnn-context-binary-generator (SDK 2.49) 在设备端 HTP V81 生成，
#   fp16_relaxed_precision=1，soc_id=97 (8845)，dsp_arch=v81。
#   体积 ~7.5MB，与 SoC 绑死（只能 v81/soc 97），换机型要重新生成。
#
# context binary 生成方式：
#   1. ONNX → binary DLC（WSL，export_format=dlc）:
#        bash dlc_bin/convert_dlc_bin.sh
#   2. DLC → fp16 ctx binary（设备端）:
#        见下方 generate_ctx_bin() 注释
# =============================================================================
set -eu
export MSYS_NO_PATHCONV=1

PKG="com.breeze.qnn"
DST="/data/user/0/${PKG}/files/yolo"
SRC_BIN="D:/workspace/models/yolo/bin/yolo26n-pose.bin"

[ -f "$SRC_BIN" ] || { echo "[ERR] 模型不存在: $SRC_BIN"; exit 1; }

if ! adb shell "pm list packages ${PKG}" | grep -q "package:${PKG}"; then
    echo "[ERR] App ${PKG} 未安装，先 ./gradlew :app:installDebug"; exit 1
fi

adb root >/dev/null 2>&1 || echo "[WARN] adb root 未拿到"
adb shell "mkdir -p ${DST}"

size=$(stat -c%s "$SRC_BIN" 2>/dev/null || wc -c <"$SRC_BIN")
echo "推 fp16 context binary（${size} bytes）…"
adb push "$SRC_BIN" "${DST}/yolo26n-pose.bin" 2>&1 | tail -1
adb shell "chmod 644 ${DST}/yolo26n-pose.bin"

echo ""
echo "===== 部署结果 ====="
adb shell "ls -la ${DST}/"

echo ""
echo "[OK] 模型就绪。验证: adb install -r app/build/outputs/apk/debug/app-debug.apk"

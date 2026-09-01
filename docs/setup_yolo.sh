#!/bin/bash
# =============================================================================
# setup_yolo.sh
#
# 把 YOLO pose vendor 2.46 context binary push 到设备 app 私有目录。
#
# 用法：
#   bash docs/setup_yolo.sh
#
# 产物来源：D:/workspace/models/yolo/bin/yolo26n-pose.bin
#   由 QnnYolo 项目的 cbg_vendor.cpp 用 SDK 2.47 + vendor signed skel 在设备端
#   生成（参考 docs/regenerate_yolo_ctx.sh 复刻同一条 recipe）。
#   bin 内 embed build ID = v2.46.0.260424121129 与 vendor skel 完全匹配。
#   体积 ~4.1MB，走 vendor 2.46 stub + vendor 2.46 skel FastRPC handshake，
#   跨版本（SDK 2.48 stub + vendor 2.46 skel）会 0x80000600 AEE_EBADCLASS，
#   故 bin 必须 build ID == skel build ID。
#   与 SoC 绑死（v81/soc 97），换机型要重新生成。
#
# 真机必要条件：
#   - /system/priv-app/QnnDemo/QnnDemo.apk 必须已替换为新版（含 libqnndemo.so）。
#     priv-app 才能使用 vendor 2.46 signed skel（普通 untrusted_app 访问 /vendor
#     lib/rfsa/adsp/ 下 skel 会 OSError permission denied）。
#   - /system/priv-app/QnnDemo/lib/arm64/ 由 build.gradle.kts 的 stripSdkHtpJniLibs
#     preBuild 任务剔除 SDK 2.48 libQnnHtp*.so（SDK stub + vendor skel 跨版本
#     handshake 必 AEE_EBADCLASS），只保留 libQnnSystem/libQnnCpu/libQnnGpu/
#     libGenie/libQnnGenAiTransformer* 等 vendor 不提供的 lib。
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
echo "推 vendor 2.46 context binary（${size} bytes）…"
adb push "$SRC_BIN" "${DST}/yolo26n-pose.bin" 2>&1 | tail -1
adb shell "chmod 644 ${DST}/yolo26n-pose.bin"

echo ""
echo "===== 部署结果 ====="
adb shell "ls -la ${DST}/"

echo ""
echo "[OK] 模型就绪。接下来："
echo "  1. 把 app/build/outputs/apk/debug/app-debug.apk push 到 /system/priv-app/QnnDemo/QnnDemo.apk"
echo "     （adb root + mount -o rw,remount /system 后再 push，替换后 force-stop 重启 app）"
echo "  2. push libqnndemo.so 到 /system/priv-app/QnnDemo/lib/arm64/"
echo "  3. 启动 app → 切到 YOLO tab → 授权摄像头"
echo "  或：直接跑 E2E 测试：bash docs/run_e2e_tests.sh yolo"

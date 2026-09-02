#!/bin/bash
# =============================================================================
# setup_yolo.sh
#
# 把 YOLO pose context binary push 到设备 app 私有目录（runtime 用 unsigned PD
# + SDK 2.48 加载，与 LLM / NER 同栈）。
#
# 用法：
#   bash docs/setup_yolo.sh
#
# 产物来源：D:/workspace/models/yolo/bin/yolo26n-pose.bin
#   由 docs/regenerate_yolo_ctx.sh 用 SDK 2.48 + unsigned PD 在设备端生成，
#   bin 内 embed build ID = v2.48.40.260702，与 jniLibs 里的 SDK 2.48
#   libQnnHtpV81Skel.so 完全匹配。
#   qairt_runtime.cpp 在 deviceCreate 时硬编码 useSignedProcessDomain=false，
#   FastRPC 走 unsigned PD 加载 jniLibs 里的 unsigned skel → 0x1388 消失。
#   与 SoC 绑死（v81/soc 97），换机型要重新生成。
#
# 真机必要条件：
#   - com.breeze.qnn 已装（./gradlew :app:installDebug）。
#   - 平台签名 priv-app 模式下 APK 推送到 /system/priv-app/QnnDemo/；
#     普通 debug 安装模式下 APK 在 /data/app/~~<hash>/com.breeze.qnn-<variant>/。
#     两种安装方式 runtime 行为一致（sharedUserId=android.uid.system
#     只是给了 /vendor 读权限做兜底，unsigned PD 路径不依赖它）。
#   - jniLibs/arm64-v8a/ 已含 SDK 2.48 libQnnHtp.so / libQnnHtpV81Stub.so /
#     libQnnHtpPrepare.so / libQnnHtpV81CalculatorStub.so / libQnnHtpV81Skel.so
#     unsigned 版（build.gradle.kts 不再 strip 这些 lib）。
# =============================================================================
set -eu
export MSYS_NO_PATHCONV=1

PKG="com.breeze.qnn"
DST="/data/user/0/${PKG}/files/yolo"
# 必须用 vendor bin（buildId=v2.46, uint16 [1,56,8400] 原始 anchor）：
# SDK 2.48/2.49 ctx-gen 重生成的 bin 是 fp32 [1,300,57]（融合 NMS），体积翻倍且
# 实测检测质量严重退化——把椅子/显示器误检成 person、真人漏检。
# vendor bin 配合 yolo_pose.cpp 的 uint16 解码路径（+ nativeLibDir 优先的
# ADSP_LIBRARY_PATH，否则 v2.46 bin 会 0x80000600）。
SRC_BIN="D:/workspace/models/yolo/bin/yolo26n-pose-vendor.bin"

[ -f "$SRC_BIN" ] || { echo "[ERR] 模型不存在: $SRC_BIN"; exit 1; }

if ! adb shell "pm list packages ${PKG}" | grep -q "package:${PKG}"; then
    echo "[ERR] App ${PKG} 未安装，先 ./gradlew :app:installDebug"; exit 1
fi

adb root >/dev/null 2>&1 || echo "[WARN] adb root 未拿到（debug 安装不需要）"
adb shell "mkdir -p ${DST}"

size=$(stat -c%s "$SRC_BIN" 2>/dev/null || wc -c <"$SRC_BIN")
echo "推 SDK 2.48 context binary（${size} bytes）…"
adb push "$SRC_BIN" "${DST}/yolo26n-pose.bin" 2>&1 | tail -1
adb shell "chmod 644 ${DST}/yolo26n-pose.bin"

echo ""
echo "===== 部署结果 ====="
adb shell "ls -la ${DST}/"

echo ""
echo "[OK] 模型就绪。接下来："
echo "  1a. priv-app 模式：adb root + mount -o rw,remount /system 后 push APK 到"
echo "      /system/priv-app/QnnDemo/QnnDemo.apk，替换后 force-stop 重启 app"
echo "  1b. debug 模式：./gradlew :app:installDebug 直接装到 /data/app"
echo "  2. 启动 app → 切到 YOLO tab → 授权摄像头"
echo "  或：直接跑 E2E 测试：bash docs/run_e2e_tests.sh yolo"

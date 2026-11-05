#!/bin/bash
# =============================================================================
# cleanup_legacy_models.sh
#
# 模型改从公共目录 /data/models 加载后，删除 app 私有目录里的旧模型副本，
# 释放空间（本机实测约 4.2 GB：llm 2.8G + asr 1.3G + clip 147M）。
#
# 用法：
#   bash docs/cleanup_legacy_models.sh          # 预览要删的目录
#   bash docs/cleanup_legacy_models.sh --force  # 确认公共目录就绪后真正删除
#
# 依赖：adb 能连到设备
# =============================================================================
set -eu
export MSYS_NO_PATHCONV=1

PKG="com.breeze.qnn"
LEGACY=(
    "/data/user/0/${PKG}/files/llm"
    "/data/user/0/${PKG}/files/asr"
    "/data/user/0/${PKG}/files/clip"
)
SHARED=(
    "/data/models/chat"
    "/data/models/asr/voice_asr"
    "/data/models/clip/chineseclip"
)

if ! adb devices | grep -q 'device$'; then
    echo "[ERR] 设备未连接"
    exit 1
fi

echo "===== 检查公共目录 ====="
missing=0
for d in "${SHARED[@]}"; do
    if adb shell "[ -d ${d} ] && echo ok" | grep -q ok; then
        echo "[OK] ${d}"
    else
        echo "[ERR] 缺失: ${d}"
        missing=1
    fi
done
if [ "$missing" -ne 0 ]; then
    echo "[ABORT] 公共目录不完整，不删除私有目录模型"
    exit 1
fi

echo ""
echo "===== 私有目录占用 ====="
for d in "${LEGACY[@]}"; do
    adb shell "du -sh ${d} 2>/dev/null || echo '  (不存在)'"
done

if [ "${1:-}" != "--force" ]; then
    echo ""
    echo "[DRY-RUN] 加 --force 才会删除以下目录："
    printf '  %s\n' "${LEGACY[@]}"
    exit 0
fi

echo ""
echo "===== 删除私有目录模型 ====="
for d in "${LEGACY[@]}"; do
    adb shell "rm -rf ${d}"
    echo "[DEL] ${d}"
done

echo ""
echo "===== 删除后剩余 ====="
adb shell "du -sh /data/user/0/${PKG}/files/* 2>/dev/null"
echo ""
echo "[OK] 旧模型已清理。App 会从 /data/models 加载。"

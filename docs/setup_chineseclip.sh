#!/bin/bash
# =============================================================================
# setup_chineseclip.sh
#
# 把 Chinese-CLIP（MNN，文搜图）模型 push 到 8845 设备的 app private filesDir，
# 供 ChineseClipModel（nativeInit）加载。模型不进 APK。
#
# 用法：
#   bash docs/setup_chineseclip.sh <src_chineseclip_dir>
#
# <src_chineseclip_dir> 需包含：
#   ├── bundle_config.json      # properties 中给出图像/文本 .mnn 文件名
#   ├── rn50_img_fp16.mnn
#   └── rn50_txt_fp16.mnn
#
# 依赖：adb 能连到设备 + com.breeze.qnn 已装（adb install -r，保留 files）
# 注意：不要用 connectedDebugAndroidTest（会卸载重装清空 filesDir）。
# =============================================================================
set -eu
export MSYS_NO_PATHCONV=1

if [ "$#" -lt 1 ]; then
    echo "Usage: bash docs/setup_chineseclip.sh <src_chineseclip_dir>"
    exit 1
fi

SRC="$1"
PKG="com.breeze.qnn"
DST="/data/user/0/${PKG}/files/clip/chineseclip"

# 校验源文件（缺一即退出）
for f in \
    "$SRC/bundle_config.json" \
    "$SRC/rn50_img_fp16.mnn" \
    "$SRC/rn50_txt_fp16.mnn" ; do
    [ -f "$f" ] || { echo "[ERR] 缺失: $f"; exit 1; }
done

# 校验 app 已装
if ! adb shell "pm list packages ${PKG}" | grep -q "package:${PKG}"; then
    echo "[ERR] App ${PKG} 未安装，先 ./gradlew :app:assembleDebug 并 adb install -r"
    exit 1
fi

adb root >/dev/null 2>&1 || echo "[WARN] adb root 未拿到，若写入 files 失败请检查权限"

echo ""
echo "===== 创建目录 ====="
adb shell "mkdir -p ${DST}"

echo ""
echo "===== 推送模型（约 150 MB）====="
for f in bundle_config.json rn50_img_fp16.mnn rn50_txt_fp16.mnn; do
    adb push "$SRC/$f" "${DST}/$f" 2>&1 | tail -1
done

# 推送后确认大小与源文件一致，避免半截文件被加载
echo ""
echo "===== 校验文件大小 ====="
for f in rn50_img_fp16.mnn rn50_txt_fp16.mnn; do
    src_size=$(stat -c%s "$SRC/$f")
    dst_size=$(adb shell "stat -c%s ${DST}/$f" | tr -d '\r')
    if [ "$src_size" = "$dst_size" ]; then
        echo "[OK] $f ${dst_size} bytes"
    else
        echo "[ERR] $f 大小不一致: 源=${src_size} 设备=${dst_size}"
        exit 1
    fi
done

echo ""
echo "===== 部署结果 ====="
adb shell "ls -la ${DST} 2>&1"

echo ""
echo "[OK] chineseclip 就绪。打开 App 的「搜图」标签页，点击「刷新索引」建立图库索引。"

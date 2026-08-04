#!/bin/bash
# =============================================================================
# setup_bert_ner.sh
#
# 把 BERT NER 的 fp32 DLC push 到设备 app 私有目录。
#
# 用法：
#   bash docs/setup_bert_ner.sh [dlc_path]
#
# 默认从 D:/workspace/models/bert-base-chinese-ner/dlc/bert-ner-fp32.dlc 取。
# DLC 由 tools/convert 流程产出，见 docs/NER-使用指南.md。
#
# 为什么用 fp32 而不是量化版：实测 fp32 在 HTP V81 上余弦 0.99697、~34ms/句；
# 而 W8A8/W8A16/A16W16 量化后判别力全失（预测全塌成 O）且反而慢 1.6x。
# =============================================================================
set -eu
export MSYS_NO_PATHCONV=1

DEFAULT_DLC="D:/workspace/models/bert-base-chinese-ner/dlc/bert-ner-fp32.dlc"
SRC="${1:-$DEFAULT_DLC}"
PKG="com.breeze.qnn"
DST="/data/user/0/${PKG}/files/ner"

[ -f "$SRC" ] || { echo "[ERR] DLC 不存在: $SRC"; exit 1; }

if ! adb shell "pm list packages ${PKG}" | grep -q "package:${PKG}"; then
    echo "[ERR] App ${PKG} 未安装，先 ./gradlew :app:installDebug"; exit 1
fi

adb root >/dev/null 2>&1 || echo "[WARN] adb root 未拿到（priv-app 也许不需要）"
adb shell "mkdir -p ${DST}"

size=$(stat -c%s "$SRC" 2>/dev/null || wc -c <"$SRC")
echo "推 bert-ner-fp32.dlc (${size} bytes，约 388MB，需十几秒)…"
adb push "$SRC" "${DST}/bert-ner-fp32.dlc" 2>&1 | tail -1

adb shell "chmod 644 ${DST}/bert-ner-fp32.dlc 2>/dev/null || true"
echo ""
echo "===== 部署结果 ====="
adb shell "ls -la ${DST}/"

echo ""
echo "[OK] 模型就绪。验证："
echo "  bash docs/run_e2e_tests.sh ner          # 跑 NerE2ETest"
echo "  或启动 app 切到 NER tab 手动试"

#!/bin/bash
# =============================================================================
# setup_bert_ner.sh
#
# 把 BERT NER 模型 push 到设备 app 私有目录。
#
# 用法：
#   bash docs/setup_bert_ner.sh [ctx|dlc]
#
#   ctx（默认）  推 205MB fp16 context binary —— 加载 371ms、单句 17ms、只能 HTP
#   dlc          推 388MB fp32 DLC —— 加载 5.3s、单句 18ms、HTP/CPU 都行
#
# 两者精度相同（vs ONNX 原模型余弦 0.99697）。优先用 ctx；它与 SoC 绑死
# （只能 v81/soc_model 97），换机型要重新生成，那时用 dlc 兜底。
#
# context binary 生成方式见 docs/NER-使用指南.md。
# =============================================================================
set -eu
export MSYS_NO_PATHCONV=1

MODE="${1:-ctx}"
BASE="D:/workspace/models/bert-base-chinese-ner"
PKG="com.breeze.qnn"
DST="/data/user/0/${PKG}/files/ner"

case "$MODE" in
  ctx) SRC="$BASE/ctx/bert-ner-fp16.bin"; NAME="bert-ner-fp16.bin"; DESC="fp16 context binary (205MB)";;
  dlc) SRC="$BASE/dlc/bert-ner-fp32.dlc"; NAME="bert-ner-fp32.dlc"; DESC="fp32 DLC (388MB)";;
  *)   echo "[ERR] 未知模式: $MODE（用 ctx 或 dlc）"; exit 1;;
esac

[ -f "$SRC" ] || { echo "[ERR] 模型不存在: $SRC"; exit 1; }

if ! adb shell "pm list packages ${PKG}" | grep -q "package:${PKG}"; then
    echo "[ERR] App ${PKG} 未安装，先 ./gradlew :app:installDebug"; exit 1
fi

adb root >/dev/null 2>&1 || echo "[WARN] adb root 未拿到（priv-app 也许不需要）"
adb shell "mkdir -p ${DST}"

size=$(stat -c%s "$SRC" 2>/dev/null || wc -c <"$SRC")
echo "推 $DESC（${size} bytes）…"
adb push "$SRC" "${DST}/${NAME}" 2>&1 | tail -1
adb shell "chmod 644 ${DST}/${NAME} 2>/dev/null || true"

echo ""
echo "===== 部署结果 ====="
adb shell "ls -la ${DST}/"

echo ""
echo "[OK] 模型就绪。NerBackend 会优先用 .bin，没有才回退 .dlc。验证："
echo "  bash docs/run_e2e_tests.sh ner"

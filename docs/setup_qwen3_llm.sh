#!/bin/bash
# =============================================================================
# setup_qwen3_llm.sh
#
# 一键把 Qwen3-3B QNN Genie 权重 push 到 8845 设备的 app private filesDir。
#
# 用法：
#   bash docs/setup_qwen3_llm.sh <src_dir>
#
# <src_dir> 目录里必须包含以下 3 个文件：
#   - qwen3-3b-htp.json      (Genie config，从 SDK examples 改)
#   - qwen3-3b-context.bin   (~1.5GB QNN context binary，signed PD V81 编译)
#   - tokenizer.json         (Qwen tokenizer)
# =============================================================================
set -eu
export MSYS_NO_PATHCONV=1

if [ "$#" -lt 1 ]; then
    echo "Usage: bash docs/setup_qwen3_llm.sh <src_dir>"
    echo "src_dir 里必须有 qwen3-3b-htp.json + qwen3-3b-context.bin + tokenizer.json"
    exit 1
fi

SRC="$1"
PKG="com.breeze.qnn"
DST="/data/user/0/${PKG}/files/llm/qwen3-3b"

# 校验源文件
for f in "$SRC/qwen3-3b-htp.json" "$SRC/qwen3-3b-context.bin" "$SRC/tokenizer.json"; do
    [ -f "$f" ] || { echo "[ERR] 缺失: $f"; exit 1; }
done

# 校验 adb + app 已装
if ! adb shell "pm list packages ${PKG}" | grep -q "package:${PKG}"; then
    echo "[ERR] App ${PKG} 未安装，先 ./gradlew :app:installDebug"; exit 1
fi

# App 私有目录只有 app 自己能写，需要 root 或 run-as
if ! adb root >/dev/null 2>&1; then
    echo "[WARN] adb root 拿不到，试 run-as"
fi
adb shell "mkdir -p ${DST}"

for f in qwen3-3b-htp.json qwen3-3b-context.bin tokenizer.json; do
    size=$(stat -c%s "$SRC/$f" 2>/dev/null || wc -c <"$SRC/$f")
    echo "推送 $f (${size} bytes)..."
    adb push "$SRC/$f" "${DST}/$f"
done

adb shell "chmod -R 644 ${DST}"
adb shell "ls -la ${DST}"

echo ""
echo "[OK] 权重就绪。跑 LlmE2ETest 验证："
echo "  bash docs/run_e2e_tests.sh llmGenerate"

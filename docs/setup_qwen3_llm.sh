#!/bin/bash
# =============================================================================
# setup_qwen3_llm.sh
#
# 一键把 AIEngine Qwen3-4B QNN Genie bundle push 到 8845 设备的 app private
# filesDir，并在设备侧生成 Genie 1.19 config（rope 放到 model.positional-encoding
# 块，SDK 2.48 Genie 才认）。
#
# 用法：
#   bash docs/setup_qwen3_llm.sh <src_bundle_dir>
#
# <src_bundle_dir> 是 AIEngine chat.zip 解压目录，需要有：
#   ├── lib/                     # bundle 里的 Genie/Htp/... .so（这里不用，只做 sanity check）
#   ├── models/
#   │   ├── weight_sharing_model_1_of_4.serialized.bin
#   │   ├── weight_sharing_model_2_of_4.serialized.bin
#   │   ├── weight_sharing_model_3_of_4.serialized.bin
#   │   └── weight_sharing_model_4_of_4.serialized.bin
#   ├── tokenizer.json
#   ├── htp_backend_ext_config-v81.json
#   └── qwen3-4b-htp-v81.json    # 源 config（v1.13 schema，脚本会转成 v1.19）
#
# 依赖：adb 能连到设备 + com.breeze.qnn 已装（./gradlew :app:installDebug）
# =============================================================================
set -eu
export MSYS_NO_PATHCONV=1

if [ "$#" -lt 1 ]; then
    echo "Usage: bash docs/setup_qwen3_llm.sh <src_bundle_dir>"
    echo "  <src_bundle_dir> = AIEngine chat.zip 解压后的根目录"
    exit 1
fi

SRC="$1"
PKG="com.breeze.qnn"
# 目录名沿用 qwen3-3b（LlmE2ETest 里 MODEL_ID 常量），实际是 4B
DST="/data/user/0/${PKG}/files/llm/qwen3-3b"

# 校验源文件
for f in \
    "$SRC/tokenizer.json" \
    "$SRC/htp_backend_ext_config-v81.json" \
    "$SRC/models/weight_sharing_model_1_of_4.serialized.bin" \
    "$SRC/models/weight_sharing_model_2_of_4.serialized.bin" \
    "$SRC/models/weight_sharing_model_3_of_4.serialized.bin" \
    "$SRC/models/weight_sharing_model_4_of_4.serialized.bin" ; do
    [ -f "$f" ] || { echo "[ERR] 缺失: $f"; exit 1; }
done

# 校验 app 已装
if ! adb shell "pm list packages ${PKG}" | grep -q "package:${PKG}"; then
    echo "[ERR] App ${PKG} 未安装，先 ./gradlew :app:installDebug"; exit 1
fi

adb root >/dev/null 2>&1 || echo "[WARN] adb root 未拿到（priv-app 也许不需要）"
adb shell "mkdir -p ${DST}/models"

echo ""
echo "===== 推权重（4 shard 共 ~3.1GB，需要几十秒到几分钟）====="
for i in 1 2 3 4; do
    f="weight_sharing_model_${i}_of_4.serialized.bin"
    size=$(stat -c%s "$SRC/models/$f" 2>/dev/null || wc -c <"$SRC/models/$f")
    echo "推 $f (${size} bytes)..."
    adb push "$SRC/models/$f" "${DST}/models/$f" 2>&1 | tail -1
done
adb push "$SRC/tokenizer.json" "${DST}/tokenizer.json" 2>&1 | tail -1
adb push "$SRC/htp_backend_ext_config-v81.json" "${DST}/htp_backend_ext_config-v81.json" 2>&1 | tail -1

# 兼容 LlmBackend.weightsReady() 里 500MB size 检查：给 shard 1 建个 hardlink
adb shell "ln -f ${DST}/models/weight_sharing_model_1_of_4.serialized.bin ${DST}/qwen3-3b-context.bin"

echo ""
echo "===== 生成 v1.19 schema 的 Genie config ====="
# 用项目本地临时文件，避开 MSYS /tmp -> Windows 路径转换踩坑
mkdir -p "$SRC/.setup_tmp"
TMP_CONFIG="$SRC/.setup_tmp/qwen3-3b-htp.json"
cat > "$TMP_CONFIG" <<'JSON'
{
  "dialog" : {
    "version" : 1,
    "type" : "basic",
    "context" : {
      "version" : 1,
      "size": 2048,
      "n-vocab": 151936,
      "bos-token": 151643,
      "eos-token": 151645
    },
    "sampler" : {
      "version" : 1,
      "seed" : 42,
      "temp" : 0.6,
      "top-k" : 20,
      "top-p" : 0.95,
      "greedy" : false
    },
    "tokenizer" : {
      "version" : 1,
      "path" : "__DST__/tokenizer.json"
    },
    "engine" : {
      "version" : 1,
      "n-threads" : 3,
      "backend" : {
        "version" : 1,
        "type" : "QnnHtp",
        "QnnHtp" : {
          "version" : 1,
          "use-mmap" : true,
          "spill-fill-bufsize" : 0,
          "mmap-budget" : 0,
          "poll" : false,
          "cpu-mask" : "0xe0",
          "kv-dim" : 128
        },
        "extensions" : "__DST__/htp_backend_ext_config-v81.json"
      },
      "model" : {
        "version" : 1,
        "type" : "binary",
        "positional-encoding" : {
          "type" : "rope",
          "rope-dim" : 64,
          "rope-theta" : 1000000
        },
        "binary" : {
          "version" : 1,
          "ctx-bins" : [
            "__DST__/models/weight_sharing_model_1_of_4.serialized.bin",
            "__DST__/models/weight_sharing_model_2_of_4.serialized.bin",
            "__DST__/models/weight_sharing_model_3_of_4.serialized.bin",
            "__DST__/models/weight_sharing_model_4_of_4.serialized.bin"
          ]
        }
      }
    }
  }
}
JSON
# 替换绝对路径
sed -i.bak "s|__DST__|${DST}|g" "$TMP_CONFIG" && rm -f "$TMP_CONFIG.bak"
adb push "$TMP_CONFIG" "${DST}/qwen3-3b-htp.json" 2>&1 | tail -1
rm -rf "$SRC/.setup_tmp"

adb shell "chmod -R 644 ${DST} 2>/dev/null; chmod 755 ${DST} ${DST}/models"
echo ""
echo "===== 部署结果 ====="
adb shell "ls -la ${DST}/ ${DST}/models/"
adb shell "du -sh ${DST}"

echo ""
echo "[OK] 权重就绪。跑 LlmE2ETest 验证："
echo "  bash docs/run_e2e_tests.sh llm"

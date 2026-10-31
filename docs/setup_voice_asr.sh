#!/bin/bash
# =============================================================================
# setup_voice_asr.sh
#
# 把 voice_asr 流式语音识别包 push 到 8845 设备的 app private filesDir，供
# AsrBackend（ASREngine::init）加载。包不进 APK。
#
# 用法：
#   bash docs/setup_voice_asr.sh <src_voice_asr_dir> [sample_wav]
#
# <src_voice_asr_dir> 需包含（目录结构必须保持，model_config.json 用相对路径）：
#   ├── model_config.json
#   ├── htp_backend_ext_config-v81.json
#   ├── stream_model/
#   │   ├── asr_step2_0p6b_streaming_320ms_audio_fp16.serialized.bin
#   │   ├── embedding_fp32.bin
#   │   └── tokenizer.json
#   └── device_payload_w4a16_lpbq_kv16_no_seqmse/
#       ├── qwen3_base_weight_sharing_model_ar128_ar1_cl2048_1_of_2.serialized.bin
#       └── qwen3_base_weight_sharing_model_ar128_ar1_cl2048_2_of_2.serialized.bin
#
# [sample_wav] 可选：16 kHz 单声道 wav，推送为 files/asr/asr_example.wav，供「识别样例」按钮使用。
#
# 依赖：adb 能连到设备 + com.breeze.qnn 已装（./gradlew :app:assembleDebug 后 adb install -r）
# 注意：用 install -r 安装或直接推送，不要 connectedDebugAndroidTest（会卸载重装清空 filesDir）。
# =============================================================================
set -eu
export MSYS_NO_PATHCONV=1

if [ "$#" -lt 1 ]; then
    echo "Usage: bash docs/setup_voice_asr.sh <src_voice_asr_dir> [sample_wav]"
    exit 1
fi

SRC="$1"
SAMPLE="${2:-}"
PKG="com.breeze.qnn"
DST="/data/user/0/${PKG}/files/asr"
MODEL_DST="${DST}/voice_asr"

# 校验源文件（缺一即退出）
for f in \
    "$SRC/model_config.json" \
    "$SRC/htp_backend_ext_config-v81.json" \
    "$SRC/stream_model/asr_step2_0p6b_streaming_320ms_audio_fp16.serialized.bin" \
    "$SRC/stream_model/embedding_fp32.bin" \
    "$SRC/stream_model/tokenizer.json" \
    "$SRC/device_payload_w4a16_lpbq_kv16_no_seqmse/qwen3_base_weight_sharing_model_ar128_ar1_cl2048_1_of_2.serialized.bin" \
    "$SRC/device_payload_w4a16_lpbq_kv16_no_seqmse/qwen3_base_weight_sharing_model_ar128_ar1_cl2048_2_of_2.serialized.bin" ; do
    [ -f "$f" ] || { echo "[ERR] 缺失: $f"; exit 1; }
done
if [ -n "$SAMPLE" ] && [ ! -f "$SAMPLE" ]; then
    echo "[ERR] 样例不存在: $SAMPLE"; exit 1
fi

# 校验 app 已装
if ! adb shell "pm list packages ${PKG}" | grep -q "package:${PKG}"; then
    echo "[ERR] App ${PKG} 未安装，先 ./gradlew :app:assembleDebug 并 adb install -r"
    exit 1
fi

adb root >/dev/null 2>&1 || echo "[WARN] adb root 未拿到（若写入 files 失败请检查权限）"

echo ""
echo "===== 创建目录（保持相对路径结构）====="
adb shell "mkdir -p ${MODEL_DST}/stream_model ${MODEL_DST}/device_payload_w4a16_lpbq_kv16_no_seqmse"

echo ""
echo "===== 推送配置 ====="
adb push "$SRC/model_config.json" "${MODEL_DST}/model_config.json" 2>&1 | tail -1
adb push "$SRC/htp_backend_ext_config-v81.json" "${MODEL_DST}/htp_backend_ext_config-v81.json" 2>&1 | tail -1

echo ""
echo "===== 推送 stream_model（约 1.0 GB）====="
for f in asr_step2_0p6b_streaming_320ms_audio_fp16.serialized.bin embedding_fp32.bin tokenizer.json; do
    adb push "$SRC/stream_model/$f" "${MODEL_DST}/stream_model/$f" 2>&1 | tail -1
done

echo ""
echo "===== 推送 device_payload（约 0.4 GB）====="
for f in qwen3_base_weight_sharing_model_ar128_ar1_cl2048_1_of_2.serialized.bin qwen3_base_weight_sharing_model_ar128_ar1_cl2048_2_of_2.serialized.bin; do
    adb push "$SRC/device_payload_w4a16_lpbq_kv16_no_seqmse/$f" \
        "${MODEL_DST}/device_payload_w4a16_lpbq_kv16_no_seqmse/$f" 2>&1 | tail -1
done

if [ -n "$SAMPLE" ]; then
    echo ""
    echo "===== 推送样例 wav ====="
    adb push "$SAMPLE" "${DST}/asr_example.wav" 2>&1 | tail -1
fi

echo ""
echo "===== 部署结果 ====="
adb shell "ls -la ${MODEL_DST} ${MODEL_DST}/stream_model ${MODEL_DST}/device_payload_w4a16_lpbq_kv16_no_seqmse 2>&1"
adb shell "du -sh ${MODEL_DST} 2>&1"

echo ""
echo "[OK] voice_asr 就绪。打开 App 的「语音」标签页，点击「识别样例」验证。"

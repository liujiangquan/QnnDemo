#pragma once

#include <cstdint>

#include "common.h"

namespace step_infer {

enum class InferStageType {
    UNKNOWN = 0,

    MODEL_INFER_BEGIN = 1000,
    MODEL_INFER_END = 1001,

    VIT_PREPROCESS_BEGIN = 2000,
    VIT_PREPROCESS_TICK = 2001,
    VIT_PREPROCESS_END = 2002,
    VIT_INFER_BEGIN = 2003,
    VIT_INFER_END = 2004,

    AUDIO_PREPROCESS_BEGIN = 2100,
    AUDIO_PREPROCESS_TICK = 2101,
    AUDIO_PREPROCESS_END = 2102,

    LLM_PREFILL_BEGIN = 3000,
    LLM_PREFILL_TICK = 3001,
    LLM_PREFILL_END = 3002,
    LLM_DECODE_BEGIN = 3003,
    LLM_DECODE_TICK = 3004,
    LLM_DECODE_END = 3005,

    AUDIO_ENCODER_INFER_BEGIN = 4000,
    AUDIO_ENCODER_INFER_TICK = 4001,
    AUDIO_ENCODER_INFER_END = 4002,

    FLOW_MATCHING_BEGIN = 5000,
    FLOW_MATCHING_END = 5001,

    HIFT_BEGIN = 6000,
    HIFT_END = 6001,
};

class ViTPreprocessInfo {
public:
    int32_t m_total_size = 0; // 图片的总数量
    int32_t m_current_index = 0; // 当前处理图片的 index
    int64_t m_timecost_ms = 0; // 预处理的耗时，单位 ms，BEGIN 时为0，TICK 时为当前耗时数据，END 时为总耗时数据
};

class AudioPreprocessInfo {
public:
    int32_t m_mel_frames = 0; // 本次预处理产出的全部有效 mel 帧数（流式为对累积缓冲重算后的有效帧总数）
    int32_t m_dispatched_frames = 0; // 本次下发给推理的 mel 帧数
    int32_t m_cached_frames = 0; // 下发后仍缓存待处理的全部 mel 帧数
    int32_t m_sample_rate = 0; // 目标采样率
    int64_t m_timecost_ms = 0; // 预处理耗时，单位 ms，BEGIN 时为0，TICK 时为当前耗时数据，END 时为总耗时数据
};

class LLMPrefillInfo {
public:
    int32_t m_chunk_length = 0; // AR
    int32_t m_input_token_size = 0; // 输入的 token 数量，VLM 时若包括 image token 的数量
    int32_t m_chunk_index = 0; // 当前 prefill 的 chunk index
    int64_t m_timecost_ms = 0; // prefill 的耗时，单位 ms，BEGIN 时为0，TICK 时为当前耗时数据，END 时为总耗时数据
};

class LLMDecodeInfo {
public:
    int32_t m_max_token_size = 0; // 可生成 token 的最大数量
    int32_t m_generated_token_size = 0; // 已生成 token 的数量
    int64_t m_timecost_ms = 0; // decode 的耗时，单位 ms，BEGIN 时为0，TICK 时为当前耗时数据，END 时为总耗时数据
};

class AudioEncoderInferInfo {
public:
    int32_t m_chunk_size = 0; // 音频 chunk 的总数量
    int32_t m_chunk_index = 0; // 当前处理的 音频 chunk 的 index
    int64_t m_timecost_ms = 0; // 推理耗时，单位 ms，BEGIN 时为0，TICK 时为当前耗时数据，END 时为总耗时数据
};

class FlowMatchingInferInfo {
public:
    int32_t m_timecost_ms = 0; // 推理耗时，单位 ms，BEGIN 时为0，END 时为耗时数据
};

class HiFTInferInfo {
public:
    int32_t m_timecost_ms = 0; // 推理耗时，单位 ms，BEGIN 时为0，END 时为耗时数据
};

class InferStageInfo {
public:
    InferStageType m_infer_stage = InferStageType::UNKNOWN;
    ModelType m_model = ModelType::UNKNOWN;

    LLMPrefillInfo *m_llm_prefill_info = nullptr;
    LLMDecodeInfo *m_llm_decode_info = nullptr;

    ViTPreprocessInfo *m_vit_preprocess_info = nullptr;
    AudioPreprocessInfo *m_audio_preprocess_info = nullptr;
    AudioEncoderInferInfo *m_audio_encoder_infer_info = nullptr;
    FlowMatchingInferInfo *m_flow_matching_infer_info = nullptr;
    HiFTInferInfo *m_hift_infer_info = nullptr;
};

typedef int32_t (*InferStageCallback)(const InferStageInfo*, void*);

} // namespace step_infer
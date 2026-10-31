#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "common.h"
#include "stage.h"

namespace step_infer {

/**
 * Streaming ASR events converted from backend-specific callback codes.
 */
enum class ASRStreamCode {
    TextChunk = 0,
    End = 1,
    Abort = 2,
    LlmContextSizeExhausted = 3,
    Exception = 4,
    Error = 5,
};

using ASRTimedStreamCallback = std::function<bool(const char *text_chunk,
                                                  ASRStreamCode code,
                                                  uint64_t audio_start_ms,
                                                  uint64_t audio_end_ms)>;

enum class ASRErrorCode {
    UNKNOWN = -1,
    SUCCESS = 0,

    EXCEPTION = 1000,
    UNINIT = 1001,
    DUPLICATE_INIT = 1002,
    INVALID_PARAM = 1003,
    INVALID_FILE = 1004,
    INVALID_JSON = 1005,
    PARSE_PARAMS_FAILED = 1006,
    UNSUPPORTED_BACKEND = 1007,
    LOAD_MODEL_FAILED = 1008,
    GENERATE_ABORT = 1009,
    NO_IMPLEMENT = 1010,
    EXCEED_SEQ_LEN = 1011,
    AUDIO_PREPROCESS_FAILED = 1012,

    GENIE_ASR_INIT_FAILED = 2000,
    GENIE_ASR_AUDIO_ENCODE_FAILED = 2001,
    GENIE_ASR_DECODE_FAILED = 2002,
    GENIE_ASR_SET_PERFORMANCE_POLICY_FAILED = 2003,
    GENIE_ASR_STREAM_FAILED = 2004,
};

class ASREngineImpl;

class ASREngine {
public:
    ASREngine();
    explicit ASREngine(const std::string &model_config, bool enable_profile = false);
    ~ASREngine();

private:
    ASREngine(const ASREngine&) = delete;
    ASREngine(const ASREngine&&) = delete;
    ASREngine& operator=(const ASREngine&) = delete;
    ASREngine& operator=(ASREngine&&) = delete;

public:
    ASRErrorCode init(const std::string &model_config, bool enable_profile = false);
    void release();

    ASRErrorCode set_performance_policy(PerformancePolicy performance_policy);

    ASRErrorCode generate(const float *mel,
                          uint32_t mel_size,
                          bool is_final_chunk = true,
                          const ASRTimedStreamCallback &callback = nullptr);
    // Streaming ASR expects one externally chunked raw PCM block per call.
    // Container formats are intended for final one-shot segment-style ASR calls.
    ASRErrorCode generate(const uint8_t *audio_data,
                          uint64_t audio_data_size,
                          const std::string &format = "wav",
                          int32_t sample_rate = 16000,
                          int32_t channel = 1,
                          bool is_final_chunk = true,
                          const ASRTimedStreamCallback &callback = nullptr);

    // Text-input inference (SLA model type only): generate output text/function
    // call from an input text instruction. Overload of the audio generate calls.
    ASRErrorCode generate(const std::string &text,
                          const ASRTimedStreamCallback &callback = nullptr);

    const std::string &get_profile_json_data() const;

    ASRErrorCode set_infer_stage_callback(InferStageCallback call_back, void *user_data);

private:
    std::shared_ptr<ASREngineImpl> m_impl;
};

// ASR output postprocess:
// 1) remove "<...>" blocks
// 2) remove leading "language <name>" metadata
// 3) remove repeated tail segments
std::string postprocess_asr_text(const std::string &output_text,
                                 int32_t max_repeat_word_len = 30,
                                 int32_t min_repeat_times = 8);

} // namespace step_infer

#pragma once

namespace step_infer {

enum class PerformancePolicy {
    BURST = 10,
    SUSTAINED_HIGH_PERFORMANCE = 20,
    HIGH_PERFORMANCE = 30,
    BALANCED = 40,
    LOW_BALANCED = 50,
    HIGH_POWER_SAVER = 60,
    POWER_SAVER = 70,
    LOW_POWER_SAVER = 80,
    EXTREME_POWER_SAVER = 90
};

enum class ModelType {
    UNKNOWN = 0,
    LLM = 1,
    VLM = 2,
    ASR = 3,
    TTS = 4,
};

} // namespace step_infer
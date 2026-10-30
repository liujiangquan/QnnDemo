// app/src/main/cpp/llm_runtime.h
#ifndef QNN_DEMO_LLM_RUNTIME_H
#define QNN_DEMO_LLM_RUNTIME_H

#include "base_inference_runtime.h"
#include <atomic>
#include <functional>
#include <string>

namespace qnn_demo {

struct SamplingParams {
    float temperature = 0.7f;
    float topP        = 0.9f;
    int   topK        = 40;
    int   maxTokens   = 256;
    uint64_t seed     = 42;
};

struct GenerateResult {
    long long elapsedMs = 0;
    int  tokensGenerated = 0;
    double tokensPerSec = 0.0;
    std::string stoppedReason = "ERROR";
    std::string error;
};

// tokenCallback(tokenText) -- 每次 decode 一个 token 就回调一次
using TokenCallback = std::function<void(const std::string&)>;
using ErrorCallback = std::function<void(const std::string&)>;

class LlmRuntime : public BaseInferenceRuntime {
public:
    LlmRuntime();
    ~LlmRuntime() override;

    bool init(const std::string& libSearchPath = "") override;
    void cleanup() override;
    bool isReady() const override { return ready_; }

    bool loadModel(const std::string& configPath);
    bool isLoaded() const { return loaded_; }

    // 阻塞调用。tokenCb 在同一线程被回调。
    GenerateResult generate(const std::string& prompt,
                            const SamplingParams& params,
                            TokenCallback tokenCb,
                            ErrorCallback errorCb);

    // 从其它线程调，异步 flag 让 generate 返回。
    void stop();

private:
    struct Impl;
    Impl* impl_;
    bool ready_  = false;
    bool loaded_ = false;
    std::atomic<bool> stopRequested_{false};
};

}  // namespace qnn_demo

#endif  // QNN_DEMO_LLM_RUNTIME_H

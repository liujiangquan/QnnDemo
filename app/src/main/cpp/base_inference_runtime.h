// =============================================================================
// base_inference_runtime.h
//
// 所有 Runtime 的最小共同接口。
// CNN (QairtRuntime) 和 LLM (LlmRuntime) 都实现这三个方法。
// 具体的 loadDlc/generate/execute 等不在基类里——它们是各自领域的接口。
// =============================================================================

#ifndef QNN_DEMO_BASE_INFERENCE_RUNTIME_H
#define QNN_DEMO_BASE_INFERENCE_RUNTIME_H

#include <string>

namespace qnn_demo {

class BaseInferenceRuntime {
public:
    virtual ~BaseInferenceRuntime() = default;

    // 一次性 dlopen + setenv 类的初始化。
    // libSearchPath 是 app nativeLibraryDir 加自定义路径的分号拼接。
    virtual bool init(const std::string& libSearchPath = "") = 0;

    // 释放所有 native handle。
    virtual void cleanup() = 0;

    // 当前是否可用。
    virtual bool isReady() const = 0;
};

}  // namespace qnn_demo

#endif  // QNN_DEMO_BASE_INFERENCE_RUNTIME_H

// app/src/main/cpp/llm_runtime.cpp
#include "llm_runtime.h"

#include <android/log.h>
#include <dlfcn.h>

#define LOG_TAG "QnnDemoLlm"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

namespace qnn_demo {

struct LlmRuntime::Impl {
    void* genieLib = nullptr;
    // Genie handles / interface pointers 会在后续 task 加进来
    ~Impl() { if (genieLib) dlclose(genieLib); }
};

LlmRuntime::LlmRuntime() : impl_(new Impl()) {}
LlmRuntime::~LlmRuntime() { cleanup(); delete impl_; }

bool LlmRuntime::init(const std::string& /*libSearchPath*/) {
    LOGI("LlmRuntime::init 尚未实现（Task 6）");
    return false;
}

void LlmRuntime::cleanup() {
    // TODO(T6): 释放 impl_ 里的 Genie 句柄 —— configHandle / dialogHandle
    //   目前 Impl 只有 genieLib，析构函数会 dlclose；但 reinit 场景下需要在
    //   这里 dlclose + 置 nullptr，避免二次 init 泄漏。T6 dlopen 落地时补齐。
    ready_ = false;
    loaded_ = false;
}

bool LlmRuntime::loadModel(const std::string& configPath) {
    LOGI("LlmRuntime::loadModel(%s) 尚未实现（Task 8）", configPath.c_str());
    return false;
}

GenerateResult LlmRuntime::generate(const std::string&,
                                    const SamplingParams&,
                                    TokenCallback,
                                    ErrorCallback errorCb) {
    GenerateResult r;
    r.error = "generate 尚未实现（Task 9）";
    if (errorCb) errorCb(r.error);
    return r;
}

void LlmRuntime::stop() { stopRequested_ = true; }

}  // namespace qnn_demo

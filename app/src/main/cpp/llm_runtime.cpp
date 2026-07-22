// app/src/main/cpp/llm_runtime.cpp
#include "llm_runtime.h"

#include <android/log.h>
#include <chrono>
#include <dlfcn.h>
#include <cstdlib>  // setenv

#include "Genie/GenieCommon.h"
#include "Genie/GenieDialog.h"

#define LOG_TAG "QnnDemoLlm"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// Genie API function pointer typedefs — 从 libGenie.so dlsym 拿到。
// 类型签名严格对齐 SDK 2.48 头文件 GenieDialog.h。
using GenieDialogConfig_CreateFromJsonFn = Genie_Status_t (*)(
    const char* str, GenieDialogConfig_Handle_t* configHandle);
using GenieDialogConfig_FreeFn = Genie_Status_t (*)(
    const GenieDialogConfig_Handle_t configHandle);
using GenieDialog_CreateFn = Genie_Status_t (*)(
    const GenieDialogConfig_Handle_t configHandle, GenieDialog_Handle_t* dialogHandle);
using GenieDialog_FreeFn = Genie_Status_t (*)(
    const GenieDialog_Handle_t dialogHandle);
using GenieDialog_QueryFn = Genie_Status_t (*)(
    const GenieDialog_Handle_t dialogHandle,
    const char* queryStr,
    const GenieDialog_SentenceCode_t sentenceCode,
    const GenieDialog_QueryCallback_t callback,
    const void* userData);
using GenieDialog_SignalFn = Genie_Status_t (*)(
    const GenieDialog_Handle_t dialogHandle,
    const GenieDialog_Action_t action);

namespace qnn_demo {

struct LlmRuntime::Impl {
    void* genieLib = nullptr;

    // Dialog config API
    GenieDialogConfig_CreateFromJsonFn createConfigFromJson = nullptr;
    GenieDialogConfig_FreeFn           freeConfig           = nullptr;

    // Dialog API
    GenieDialog_CreateFn dialogCreate = nullptr;
    GenieDialog_FreeFn   dialogFree   = nullptr;
    GenieDialog_QueryFn  dialogQuery  = nullptr;
    GenieDialog_SignalFn  dialogSignal = nullptr;

    // 运行时 handles（Task 8/9 用）
    GenieDialogConfig_Handle_t configHandle = nullptr;
    GenieDialog_Handle_t       dialogHandle = nullptr;

    ~Impl() {
        if (dialogHandle && dialogFree) dialogFree(dialogHandle);
        if (configHandle && freeConfig) freeConfig(configHandle);
        if (genieLib) dlclose(genieLib);
    }
};

LlmRuntime::LlmRuntime() : impl_(new Impl()) {}
LlmRuntime::~LlmRuntime() { cleanup(); delete impl_; }

bool LlmRuntime::init(const std::string& libSearchPath) {
    if (ready_) return true;

    // 防御性清理：调用者若未 cleanup() 就重试 init()，之前那次的 genieLib 需先关
    // 否则第二次 dlopen 会覆盖旧 handle 导致泄漏。
    if (impl_->genieLib) {
        dlclose(impl_->genieLib);
        impl_->genieLib = nullptr;
    }

    // setenv ADSP_LIBRARY_PATH（跟 QairtRuntime init 保持一致的顺序）
    // Genie 内部会加载 libQnnHtp.so，进而通过 fastrpc 找 skel
    std::string adspPath = libSearchPath;
    if (!adspPath.empty()) adspPath += ";";
    adspPath += "/vendor/lib/rfsa/adsp;/vendor/dsp/cdsp;/vendor/dsp/adsp;"
                "/system/lib/rfsa/adsp;/dsp";
    setenv("ADSP_LIBRARY_PATH", adspPath.c_str(), 1);
    LOGI("ADSP_LIBRARY_PATH=%s", adspPath.c_str());

    // dlopen libGenie.so —— 先从 libSearchPath 第一段找（app nativeLibraryDir），
    // 找不到再走系统默认路径
    std::string genieLibPath;
    if (!libSearchPath.empty()) {
        size_t sep = libSearchPath.find(';');
        std::string firstDir = (sep == std::string::npos)
            ? libSearchPath
            : libSearchPath.substr(0, sep);
        genieLibPath = firstDir + "/libGenie.so";
        impl_->genieLib = dlopen(genieLibPath.c_str(), RTLD_NOW | RTLD_LOCAL);
    }
    if (!impl_->genieLib) {
        impl_->genieLib = dlopen("libGenie.so", RTLD_NOW | RTLD_LOCAL);
    }
    if (!impl_->genieLib) {
        LOGE("dlopen libGenie.so 失败: %s", dlerror());
        return false;
    }

    // dlsym 出所有需要的入口
    auto sym = [this](const char* name) -> void* {
        void* p = dlsym(impl_->genieLib, name);
        if (!p) LOGE("dlsym(%s) 失败: %s", name, dlerror());
        return p;
    };
    impl_->createConfigFromJson =
        reinterpret_cast<GenieDialogConfig_CreateFromJsonFn>(
            sym("GenieDialogConfig_createFromJson"));
    impl_->freeConfig =
        reinterpret_cast<GenieDialogConfig_FreeFn>(
            sym("GenieDialogConfig_free"));
    impl_->dialogCreate =
        reinterpret_cast<GenieDialog_CreateFn>(
            sym("GenieDialog_create"));
    impl_->dialogFree =
        reinterpret_cast<GenieDialog_FreeFn>(
            sym("GenieDialog_free"));
    impl_->dialogQuery =
        reinterpret_cast<GenieDialog_QueryFn>(
            sym("GenieDialog_query"));
    impl_->dialogSignal =
        reinterpret_cast<GenieDialog_SignalFn>(
            sym("GenieDialog_signal"));

    if (!impl_->createConfigFromJson || !impl_->dialogCreate) {
        LOGE("Genie API 必需入口缺失（createConfigFromJson=%p dialogCreate=%p）",
             (void*)impl_->createConfigFromJson, (void*)impl_->dialogCreate);
        return false;
    }

    LOGI("libGenie.so 加载完毕");
    ready_ = true;
    return true;
}

void LlmRuntime::cleanup() {
    if (impl_->dialogHandle && impl_->dialogFree) {
        impl_->dialogFree(impl_->dialogHandle);
        impl_->dialogHandle = nullptr;
    }
    if (impl_->configHandle && impl_->freeConfig) {
        impl_->freeConfig(impl_->configHandle);
        impl_->configHandle = nullptr;
    }
    if (impl_->genieLib) {
        dlclose(impl_->genieLib);
        impl_->genieLib = nullptr;
    }
    impl_->createConfigFromJson = nullptr;
    impl_->freeConfig           = nullptr;
    impl_->dialogCreate         = nullptr;
    impl_->dialogFree           = nullptr;
    impl_->dialogQuery          = nullptr;
    impl_->dialogSignal         = nullptr;
    ready_ = false;
    loaded_ = false;
}

bool LlmRuntime::loadModel(const std::string& configPath) {
    if (!ready_ || !impl_->createConfigFromJson) {
        LOGE("Runtime 未 init，请先 init()");
        return false;
    }
    if (loaded_) {
        LOGI("已加载模型，先释放旧的");
        if (impl_->dialogHandle) {
            impl_->dialogFree(impl_->dialogHandle);
            impl_->dialogHandle = nullptr;
        }
        if (impl_->configHandle) {
            impl_->freeConfig(impl_->configHandle);
            impl_->configHandle = nullptr;
        }
        loaded_ = false;
    }

    Genie_Status_t s = impl_->createConfigFromJson(
        configPath.c_str(), &impl_->configHandle);
    if (s != GENIE_STATUS_SUCCESS || !impl_->configHandle) {
        LOGE("GenieDialogConfig_createFromJson(%s) 失败: %d", configPath.c_str(), s);
        return false;
    }
    LOGI("Config 已加载: %s", configPath.c_str());

    s = impl_->dialogCreate(impl_->configHandle, &impl_->dialogHandle);
    if (s != GENIE_STATUS_SUCCESS || !impl_->dialogHandle) {
        LOGE("GenieDialog_create 失败: %d", s);
        impl_->freeConfig(impl_->configHandle);
        impl_->configHandle = nullptr;
        return false;
    }
    LOGI("GenieDialog 就绪");

    loaded_ = true;
    return true;
}

namespace {

// Genie tokenCallback 需要 C 函数入口，从 user_data 反查回 C++ TokenCallback
struct CallbackContext {
    TokenCallback tokenCb;
    ErrorCallback errorCb;
    int tokenCount = 0;
    std::atomic<bool>* stopFlag = nullptr;
    bool aborted = false;
};

// 签名对齐 GenieDialog_QueryCallback_t:
//   void (*)(const char* response, GenieDialog_SentenceCode_t sentenceCode, const void* userData)
void genieQueryCb(const char* response,
                  const GenieDialog_SentenceCode_t sentenceCode,
                  const void* userData) {
    auto* ctx = const_cast<CallbackContext*>(
        reinterpret_cast<const CallbackContext*>(userData));
    if (!ctx) return;

    if (sentenceCode == GENIE_DIALOG_SENTENCE_ABORT) {
        ctx->aborted = true;
        return;
    }

    if (response && ctx->tokenCb) {
        std::string tok(response);
        ctx->tokenCb(tok);
        ++ctx->tokenCount;
    }
}

}  // anonymous namespace

GenerateResult LlmRuntime::generate(const std::string& prompt,
                                    const SamplingParams& params,
                                    TokenCallback tokenCb,
                                    ErrorCallback errorCb) {
    GenerateResult r;
    if (!loaded_ || !impl_->dialogQuery) {
        r.error = "模型未加载";
        if (errorCb) errorCb(r.error);
        return r;
    }

    stopRequested_ = false;
    CallbackContext ctx{tokenCb, errorCb, 0, &stopRequested_, false};

    auto t0 = std::chrono::steady_clock::now();
    Genie_Status_t s = impl_->dialogQuery(
        impl_->dialogHandle,
        prompt.c_str(),
        GENIE_DIALOG_SENTENCE_COMPLETE,
        genieQueryCb,
        &ctx);
    auto t1 = std::chrono::steady_clock::now();

    r.elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
    r.tokensGenerated = ctx.tokenCount;
    r.tokensPerSec = r.elapsedMs > 0
        ? (1000.0 * ctx.tokenCount / r.elapsedMs)
        : 0.0;

    if (s != GENIE_STATUS_SUCCESS
        && s != GENIE_STATUS_WARNING_CONTEXT_EXCEEDED
        && s != GENIE_STATUS_WARNING_ABORTED) {
        r.stoppedReason = "ERROR";
        r.error = std::string("GenieDialog_query 失败: ") + std::to_string(s);
        if (errorCb) errorCb(r.error);
    } else if (ctx.aborted || stopRequested_) {
        r.stoppedReason = "USER_CANCEL";
    } else if (ctx.tokenCount >= params.maxTokens) {
        r.stoppedReason = "MAX_TOKENS";
    } else {
        r.stoppedReason = "EOS";
    }

    LOGI("generate 结束: tokens=%d elapsed=%lldms speed=%.1f tok/s reason=%s",
         r.tokensGenerated, (long long)r.elapsedMs, r.tokensPerSec,
         r.stoppedReason.c_str());
    return r;
}

void LlmRuntime::stop() {
    stopRequested_ = true;
    // 通知 Genie 主动中断生成（GenieDialog_signal ABORT）—— 让 dialogQuery
    // 尽快返回。stopRequested_ 是软 flag，dialogSignal 是硬 ABORT，两者
    // 组合让 stoppedReason 能正确判为 USER_CANCEL。
    if (impl_->dialogSignal && impl_->dialogHandle) {
        impl_->dialogSignal(impl_->dialogHandle, GENIE_DIALOG_ACTION_ABORT);
    }
}

}  // namespace qnn_demo

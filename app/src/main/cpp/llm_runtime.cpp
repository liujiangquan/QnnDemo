// app/src/main/cpp/llm_runtime.cpp
#include "llm_runtime.h"

#include <android/log.h>
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

namespace qnn_demo {

struct LlmRuntime::Impl {
    void* genieLib = nullptr;

    // Dialog config API
    GenieDialogConfig_CreateFromJsonFn createConfigFromJson = nullptr;
    GenieDialogConfig_FreeFn           freeConfig           = nullptr;

    // Dialog API
    GenieDialog_CreateFn dialogCreate = nullptr;
    GenieDialog_FreeFn   dialogFree   = nullptr;

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

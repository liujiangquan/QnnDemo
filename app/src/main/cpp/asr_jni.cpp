// =============================================================================
// asr_jni.cpp
//
// 语音识别（ASR）JNI 桥：封装 step_infer::ASREngine（libstep_infer.so）。
// - nativeCreate：chdir 到模型目录 → ASREngine::init(model_config.json) → 恢复 cwd
// - nativeGenerate：按 pcm_s16le / 16 kHz / 单声道 送入一段 PCM，流式文本经回调收集
// - nativeRelease：释放引擎
// 调用需由 Kotlin 层串行化（ASREngine 非线程安全）。
// =============================================================================

#include <jni.h>
#include <android/log.h>
#include <unistd.h>

#include <cstdint>
#include <string>

#include "asr.h"

#define ASR_LOG_TAG "AsrJni"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  ASR_LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, ASR_LOG_TAG, __VA_ARGS__)

namespace {

// 句柄：引擎 + 模型目录（generate 时需要切换到该目录）
struct AsrHandle {
    step_infer::ASREngine* engine = nullptr;
    std::string modelDir;
};

// RAII：临时切换工作目录，析构时恢复
class CwdGuard {
public:
    explicit CwdGuard(const std::string& dir) : ok_(false) {
        if (getcwd(before_, sizeof(before_)) == nullptr) before_[0] = '\0';
        ok_ = (chdir(dir.c_str()) == 0);
    }
    ~CwdGuard() {
        if (before_[0] != '\0' && chdir(before_) != 0) LOGE("restore cwd failed");
    }
    bool ok() const { return ok_; }
private:
    char before_[1024];
    bool ok_;
};

int g_lastInitErr = 0;

std::string toStdString(JNIEnv* env, jstring js) {
    if (js == nullptr) return {};
    const char* c = env->GetStringUTFChars(js, nullptr);
    std::string s(c ? c : "");
    if (c) env->ReleaseStringUTFChars(js, c);
    return s;
}

std::string dirOf(const std::string& path) {
    const auto pos = path.find_last_of('/');
    return pos == std::string::npos ? std::string(".") : path.substr(0, pos);
}

}  // namespace

extern "C" {

// 返回句柄（>0 成功）；失败时返回 0，并在 Kotlin 侧可通过 nativeLastError 取错误码
JNIEXPORT jlong JNICALL
Java_com_breeze_qnn_AsrNative_nativeCreate(JNIEnv* env, jobject, jstring jConfigPath) {
    const std::string configPath = toStdString(env, jConfigPath);
    const std::string modelDir = dirOf(configPath);

    CwdGuard guard(modelDir);
    if (!guard.ok()) {
        LOGE("chdir failed: %s", modelDir.c_str());
        return 0;
    }
    auto* engine = new step_infer::ASREngine();
    const step_infer::ASRErrorCode err = engine->init(configPath, false);
    g_lastInitErr = static_cast<int>(err);
    if (err != step_infer::ASRErrorCode::SUCCESS) {
        LOGE("ASREngine::init failed err=%d config=%s", static_cast<int>(err), configPath.c_str());
        engine->release();
        delete engine;
        return 0;
    }
    LOGI("ASREngine::init ok config=%s", configPath.c_str());
    auto* handle = new AsrHandle();
    handle->engine = engine;
    handle->modelDir = modelDir;
    return reinterpret_cast<jlong>(handle);
}

JNIEXPORT jint JNICALL
Java_com_breeze_qnn_AsrNative_nativeLastInitError(JNIEnv*, jobject) {
    return static_cast<jint>(g_lastInitErr);
}

// 送入一段 PCM（16-bit LE，16 kHz，单声道），返回本段识别出的文本；
// isFinal=true 表示最后一段，引擎会输出剩余文本。
JNIEXPORT jstring JNICALL
Java_com_breeze_qnn_AsrNative_nativeGenerate(JNIEnv* env, jobject, jlong jHandle,
                                             jbyteArray jPcm, jboolean isFinal) {
    auto* handle = reinterpret_cast<AsrHandle*>(jHandle);
    if (handle == nullptr || handle->engine == nullptr) {
        LOGE("generate: invalid handle");
        return env->NewStringUTF("");
    }
    const jsize len = env->GetArrayLength(jPcm);
    jbyte* bytes = env->GetByteArrayElements(jPcm, nullptr);
    if (bytes == nullptr) return env->NewStringUTF("");

    std::string text;
    const step_infer::ASRTimedStreamCallback cb =
        [&text](const char* chunk, step_infer::ASRStreamCode code, uint64_t, uint64_t) -> bool {
            if (code == step_infer::ASRStreamCode::TextChunk && chunk != nullptr) text += chunk;
            return true;
        };

    CwdGuard guard(handle->modelDir);
    const step_infer::ASRErrorCode err = handle->engine->generate(
        reinterpret_cast<const uint8_t*>(bytes), static_cast<uint64_t>(len),
        "pcm_s16le", 16000, 1, isFinal == JNI_TRUE, cb);

    env->ReleaseByteArrayElements(jPcm, bytes, JNI_ABORT);

    if (err != step_infer::ASRErrorCode::SUCCESS) {
        LOGE("generate failed err=%d bytes=%d final=%d", static_cast<int>(err),
             static_cast<int>(len), isFinal == JNI_TRUE ? 1 : 0);
        // 失败时不抛异常，返回空串；Kotlin 侧通过文本为空判断，错误码日志可查
        return env->NewStringUTF("");
    }
    return env->NewStringUTF(text.c_str());
}

JNIEXPORT void JNICALL
Java_com_breeze_qnn_AsrNative_nativeRelease(JNIEnv*, jobject, jlong jHandle) {
    auto* handle = reinterpret_cast<AsrHandle*>(jHandle);
    if (handle == nullptr) return;
    if (handle->engine != nullptr) {
        handle->engine->release();
        delete handle->engine;
    }
    delete handle;
    LOGI("ASR released");
}

}  // extern "C"

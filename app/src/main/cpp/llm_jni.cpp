// app/src/main/cpp/llm_jni.cpp
#include "llm_runtime.h"

#include <jni.h>
#include <android/log.h>
#include <sstream>

#define LOG_TAG "QnnDemoLlm"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

using qnn_demo::LlmRuntime;
using qnn_demo::SamplingParams;
using qnn_demo::GenerateResult;

namespace {

std::string jstr(JNIEnv* env, jstring s) {
    if (!s) return "";
    const char* c = env->GetStringUTFChars(s, nullptr);
    std::string r = c ? c : "";
    env->ReleaseStringUTFChars(s, c);
    return r;
}

std::string jsonEscape(const std::string& s) {
    std::string o;
    o.reserve(s.size() + 2);
    for (char c : s) switch (c) {
        case '"':  o += "\\\""; break;
        case '\\': o += "\\\\"; break;
        case '\n': o += "\\n";  break;
        default:   o += c;      break;
    }
    return o;
}

std::string serialize(const GenerateResult& r) {
    std::ostringstream os;
    os << "{\"elapsedMs\":" << r.elapsedMs
       << ",\"tokensGenerated\":" << r.tokensGenerated
       << ",\"tokensPerSec\":" << r.tokensPerSec
       << ",\"stoppedReason\":\"" << jsonEscape(r.stoppedReason) << "\""
       << ",\"error\":\"" << jsonEscape(r.error) << "\"}";
    return os.str();
}

}  // namespace

extern "C" {

JNIEXPORT jlong JNICALL
Java_com_breeze_qnn_LlmNative_nativeCreate(JNIEnv*, jobject) {
    return reinterpret_cast<jlong>(new LlmRuntime());
}

JNIEXPORT void JNICALL
Java_com_breeze_qnn_LlmNative_nativeDestroy(JNIEnv*, jobject, jlong h) {
    delete reinterpret_cast<LlmRuntime*>(h);
}

JNIEXPORT jboolean JNICALL
Java_com_breeze_qnn_LlmNative_nativeInit(JNIEnv* env, jobject, jlong h, jstring p) {
    auto* rt = reinterpret_cast<LlmRuntime*>(h);
    return rt && rt->init(jstr(env, p)) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_com_breeze_qnn_LlmNative_nativeLoadModel(JNIEnv* env, jobject, jlong h, jstring p) {
    auto* rt = reinterpret_cast<LlmRuntime*>(h);
    return rt && rt->loadModel(jstr(env, p)) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_com_breeze_qnn_LlmNative_nativeIsLoaded(JNIEnv*, jobject, jlong h) {
    auto* rt = reinterpret_cast<LlmRuntime*>(h);
    return rt && rt->isLoaded() ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jstring JNICALL
Java_com_breeze_qnn_LlmNative_nativeGenerate(JNIEnv* env, jobject, jlong h,
    jstring jPrompt, jfloat temp, jfloat topP, jint topK,
    jint maxTokens, jlong seed, jobject cb) {

    auto* rt = reinterpret_cast<LlmRuntime*>(h);
    if (!rt) return env->NewStringUTF("{\"error\":\"invalid handle\"}");

    // 缓存 tokenCallback / errorCallback method IDs
    jclass cbCls = env->GetObjectClass(cb);
    jmethodID onToken = env->GetMethodID(cbCls, "onToken", "(Ljava/lang/String;)V");
    jmethodID onError = env->GetMethodID(cbCls, "onError", "(Ljava/lang/String;)V");

    SamplingParams p;
    p.temperature = temp; p.topP = topP; p.topK = topK;
    p.maxTokens = maxTokens; p.seed = static_cast<uint64_t>(seed);

    GenerateResult r = rt->generate(jstr(env, jPrompt), p,
        [env, cb, onToken](const std::string& tok) {
            jstring js = env->NewStringUTF(tok.c_str());
            env->CallVoidMethod(cb, onToken, js);
            env->DeleteLocalRef(js);
            if (env->ExceptionCheck()) env->ExceptionClear();
        },
        [env, cb, onError](const std::string& msg) {
            jstring js = env->NewStringUTF(msg.c_str());
            env->CallVoidMethod(cb, onError, js);
            env->DeleteLocalRef(js);
            if (env->ExceptionCheck()) env->ExceptionClear();
        });

    return env->NewStringUTF(serialize(r).c_str());
}

JNIEXPORT void JNICALL
Java_com_breeze_qnn_LlmNative_nativeStop(JNIEnv*, jobject, jlong h) {
    auto* rt = reinterpret_cast<LlmRuntime*>(h);
    if (rt) rt->stop();
}

}  // extern "C"

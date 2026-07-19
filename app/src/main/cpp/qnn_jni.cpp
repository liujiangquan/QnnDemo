// =============================================================================
// qnn_jni.cpp
//
// JNI 桥接层：将 Kotlin 调用映射到 QairtRuntime C++ 类。
//
// 暴露的 native 方法（对应 com.breeze.qnn.QnnNative）：
//   - long nativeCreate()
//   - void nativeDestroy(long handle)
//   - boolean nativeInit(long handle, String libSearchPath)
//   - boolean nativeLoadDlc(long handle, String dlcPath, int backend)
//   - String[] nativeGetGraphNames(long handle)
//   - String nativeGetGraphInfoJson(long handle)
//   - String nativeExecute(long handle, String graphName, byte[][] inputs)
// =============================================================================

#include "qairt_runtime.h"

#include <jni.h>
#include <android/log.h>

#include <memory>
#include <sstream>
#include <string>

#define LOG_TAG "QnnDemo"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

using qnn_demo::QairtRuntime;
using qnn_demo::BackendType;
using qnn_demo::GraphInfo;
using qnn_demo::TensorInfo;
using qnn_demo::InferenceResult;

namespace {
// 将后端枚举值转换为 BackendType
BackendType toBackendType(jint b) {
    switch (b) {
        case 1:  return BackendType::GPU;
        case 3:  return BackendType::HTP;
        case 0:
        default: return BackendType::CPU;
    }
}

// 获取存储在 Java long 中的 C++ 指针
QairtRuntime* getRuntime(jlong handle) {
    return reinterpret_cast<QairtRuntime*>(handle);
}

// 简单 JSON 字符串转义
std::string jsonEscape(const std::string& s) {
    std::string out;
    out.reserve(s.size() + 2);
    for (char c : s) {
        switch (c) {
            case '"':  out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\n': out += "\\n";  break;
            case '\r': out += "\\r";  break;
            case '\t': out += "\\t";  break;
            default:   out += c;      break;
        }
    }
    return out;
}

// 数据类型枚举值 → 可读名称
const char* dataTypeName(int32_t dt) {
    switch (dt) {
        case 0x0232: return "FLOAT32";
        case 0x0216: return "FLOAT16";
        case 0x0108: return "UINT8";
        case 0x0008: return "INT8";
        case 0x0032: return "INT32";
        case 0x0064: return "INT64";
        case 0x0116: return "UINT16";
        case 0x0132: return "UINT32";
        default:    return "UNKNOWN";
    }
}

// 将图信息序列化为 JSON
std::string serializeGraphInfos(const std::vector<GraphInfo>& graphs) {
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < graphs.size(); ++i) {
        const auto& g = graphs[i];
        if (i > 0) oss << ",";
        oss << "{\"name\":\"" << jsonEscape(g.name) << "\",";
        oss << "\"inputs\":[";
        for (size_t j = 0; j < g.inputs.size(); ++j) {
            const auto& t = g.inputs[j];
            if (j > 0) oss << ",";
            oss << "{\"name\":\"" << jsonEscape(t.name) << "\","
                << "\"id\":" << t.id << ","
                << "\"rank\":" << t.rank << ","
                << "\"dataType\":\"" << dataTypeName(t.dataType) << "\","
                << "\"dataTypeValue\":" << t.dataType << ","
                << "\"elementBytes\":" << t.elementBytes << ","
                << "\"totalElements\":" << t.totalElements << ","
                << "\"totalBytes\":" << t.totalBytes << ","
                << "\"dimensions\":[";
            for (size_t k = 0; k < t.dimensions.size(); ++k) {
                if (k > 0) oss << ",";
                oss << t.dimensions[k];
            }
            oss << "]}";
        }
        oss << "],\"outputs\":[";
        for (size_t j = 0; j < g.outputs.size(); ++j) {
            const auto& t = g.outputs[j];
            if (j > 0) oss << ",";
            oss << "{\"name\":\"" << jsonEscape(t.name) << "\","
                << "\"id\":" << t.id << ","
                << "\"rank\":" << t.rank << ","
                << "\"dataType\":\"" << dataTypeName(t.dataType) << "\","
                << "\"dataTypeValue\":" << t.dataType << ","
                << "\"elementBytes\":" << t.elementBytes << ","
                << "\"totalElements\":" << t.totalElements << ","
                << "\"totalBytes\":" << t.totalBytes << ","
                << "\"dimensions\":[";
            for (size_t k = 0; k < t.dimensions.size(); ++k) {
                if (k > 0) oss << ",";
                oss << t.dimensions[k];
            }
            oss << "]}";
        }
        oss << "]}";
    }
    oss << "]";
    return oss.str();
}

// 将推理结果序列化为 JSON
std::string serializeResult(const InferenceResult& r) {
    std::ostringstream oss;
    oss << "{";
    oss << "\"graphName\":\"" << jsonEscape(r.graphName) << "\",";
    oss << "\"elapsedMs\":" << r.elapsedMs << ",";
    oss << "\"error\":\"" << jsonEscape(r.error) << "\",";
    oss << "\"outputs\":[";
    for (size_t i = 0; i < r.outputs.size(); ++i) {
        if (i > 0) oss << ",";
        oss << "{\"size\":" << r.outputs[i].size() << "}";
    }
    oss << "]}";
    return oss.str();
}
}  // namespace

// ---------------------------------------------------------------------------
// JNI 导出函数
// ---------------------------------------------------------------------------
extern "C" {

JNIEXPORT jlong JNICALL
Java_com_breeze_qnn_QnnNative_nativeCreate(JNIEnv*, jobject) {
    auto* rt = new QairtRuntime();
    return reinterpret_cast<jlong>(rt);
}

JNIEXPORT void JNICALL
Java_com_breeze_qnn_QnnNative_nativeDestroy(JNIEnv*, jobject, jlong handle) {
    auto* rt = getRuntime(handle);
    if (rt) {
        rt->cleanup();
        delete rt;
    }
}

JNIEXPORT jboolean JNICALL
Java_com_breeze_qnn_QnnNative_nativeInit(JNIEnv* env, jobject, jlong handle, jstring libSearchPath) {
    auto* rt = getRuntime(handle);
    if (!rt) return JNI_FALSE;
    const char* path = libSearchPath ? env->GetStringUTFChars(libSearchPath, nullptr) : nullptr;
    std::string s = path ? path : "";
    bool ok = rt->init(s);
    if (path) env->ReleaseStringUTFChars(libSearchPath, path);
    return ok ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_com_breeze_qnn_QnnNative_nativeLoadDlc(JNIEnv* env, jobject, jlong handle, jstring dlcPath, jint backend) {
    auto* rt = getRuntime(handle);
    if (!rt || !dlcPath) return JNI_FALSE;
    const char* p = env->GetStringUTFChars(dlcPath, nullptr);
    std::string s = p;
    env->ReleaseStringUTFChars(dlcPath, p);
    return rt->loadDlc(s, toBackendType(backend)) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jstring JNICALL
Java_com_breeze_qnn_QnnNative_nativeGetGraphInfoJson(JNIEnv* env, jobject, jlong handle) {
    auto* rt = getRuntime(handle);
    if (!rt) return env->NewStringUTF("[]");
    return env->NewStringUTF(serializeGraphInfos(rt->graphInfos()).c_str());
}

JNIEXPORT jstring JNICALL
Java_com_breeze_qnn_QnnNative_nativeExecute(JNIEnv* env, jobject, jlong handle,
                                              jstring graphName, jobjectArray inputs) {
    auto* rt = getRuntime(handle);
    if (!rt) return env->NewStringUTF("{\"error\":\"invalid handle\"}");

    const char* gn = graphName ? env->GetStringUTFChars(graphName, nullptr) : nullptr;
    std::string graphStr = gn ? gn : "";
    if (gn) env->ReleaseStringUTFChars(graphName, gn);

    // 将 Java byte[][] 转为 std::vector<std::vector<uint8_t>>
    std::vector<std::vector<uint8_t>> inVecs;
    if (inputs) {
        jsize n = env->GetArrayLength(inputs);
        inVecs.reserve(n);
        for (jsize i = 0; i < n; ++i) {
            jbyteArray arr = static_cast<jbyteArray>(env->GetObjectArrayElement(inputs, i));
            if (!arr) { inVecs.emplace_back(); continue; }
            jsize len = env->GetArrayLength(arr);
            jbyte* buf = env->GetByteArrayElements(arr, nullptr);
            std::vector<uint8_t> v(buf, buf + len);
            env->ReleaseByteArrayElements(arr, buf, JNI_ABORT);
            env->DeleteLocalRef(arr);
            inVecs.push_back(std::move(v));
        }
    }

    InferenceResult r = rt->execute(graphStr, inVecs);
    return env->NewStringUTF(serializeResult(r).c_str());
}

}  // extern "C"

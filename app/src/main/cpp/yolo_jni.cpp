/*
 * Copyright (c) 2026 智跃千里公司. All rights reserved.
 * File:        yolo_jni.cpp
 * Description: JNI bindings for YoloPoseSession (HTP YOLO26-pose).
 * Version:     v1.0
 * Created:     2026-09-01
 * Author:      g-liujiangquan
 */

#include <jni.h>

#include <memory>
#include <string>
#include <vector>

#include "yolo_pose.h"

namespace {

YoloPoseSession& session() {
    static YoloPoseSession instance;
    return instance;
}

jstring toJString(JNIEnv* env, const std::string& text) {
    return env->NewStringUTF(text.c_str());
}

}  // namespace

extern "C" JNIEXPORT jstring JNICALL
Java_com_breeze_qnn_yolo_YoloNative_nativeInit(
        JNIEnv* env,
        jobject /* thiz */,
        jstring nativeLibDir,
        jbyteArray contextBin) {
    if (nativeLibDir == nullptr || contextBin == nullptr) {
        return toJString(env, "nativeInit requires lib dir and context bytes");
    }
    const char* libDir = env->GetStringUTFChars(nativeLibDir, nullptr);
    const jsize bytes = env->GetArrayLength(contextBin);
    jbyte* data = env->GetByteArrayElements(contextBin, nullptr);
    std::string err = session().init(
            libDir,
            reinterpret_cast<const uint8_t*>(data),
            static_cast<size_t>(bytes));
    env->ReleaseByteArrayElements(contextBin, data, JNI_ABORT);
    env->ReleaseStringUTFChars(nativeLibDir, libDir);
    if (err.empty()) {
        return nullptr;
    }
    return toJString(env, err);
}

extern "C" JNIEXPORT jfloatArray JNICALL
Java_com_breeze_qnn_yolo_YoloNative_nativeInferRgb(
        JNIEnv* env,
        jobject /* thiz */,
        jint width,
        jint height,
        jbyteArray rgbBytes) {
    if (rgbBytes == nullptr) {
        return nullptr;
    }
    const jsize n = env->GetArrayLength(rgbBytes);
    jbyte* data = env->GetByteArrayElements(rgbBytes, nullptr);
    std::vector<float> detections = session().inferRgb(
            static_cast<int>(width),
            static_cast<int>(height),
            reinterpret_cast<const uint8_t*>(data));
    env->ReleaseByteArrayElements(rgbBytes, data, JNI_ABORT);
    if (detections.empty()) {
        return nullptr;
    }
    jfloatArray out = env->NewFloatArray(static_cast<jsize>(detections.size()));
    env->SetFloatArrayRegion(
            out, 0, static_cast<jsize>(detections.size()), detections.data());
    return out;
}

extern "C" JNIEXPORT void JNICALL
Java_com_breeze_qnn_yolo_YoloNative_nativeRelease(JNIEnv* /* env */, jobject /* thiz */) {
    session().release();
}

#include <android/bitmap.h>
#include <android/log.h>

#include <jni.h>
#include <cstdint>
#include <string>

#include "clipsolver.h"

/** UTF-8 → wchar_t（Linux/Android 上 wchar_t 为 UTF-32），避免 C++17 弃用的 codecvt/wstring_convert。 */
static std::wstring utf8_to_wstring(const std::string &utf8) {
    std::wstring out;
    out.reserve(utf8.size());
    const auto *data = reinterpret_cast<const unsigned char *>(utf8.data());
    size_t n = utf8.size();
    for (size_t i = 0; i < n;) {
        uint32_t cp = 0;
        size_t tail = 0;
        unsigned char c0 = data[i];
        if (c0 < 0x80u) {
            cp = c0;
        } else if ((c0 & 0xE0u) == 0xC0u && i + 1 < n && (data[i + 1] & 0xC0u) == 0x80u) {
            cp = (static_cast<uint32_t>(c0 & 0x1Fu) << 6) | (data[i + 1] & 0x3Fu);
            tail = 1;
        } else if ((c0 & 0xF0u) == 0xE0u && i + 2 < n &&
                   (data[i + 1] & 0xC0u) == 0x80u && (data[i + 2] & 0xC0u) == 0x80u) {
            cp = (static_cast<uint32_t>(c0 & 0x0Fu) << 12) |
                 (static_cast<uint32_t>(data[i + 1] & 0x3Fu) << 6) |
                 static_cast<uint32_t>(data[i + 2] & 0x3Fu);
            tail = 2;
        } else if ((c0 & 0xF8u) == 0xF0u && i + 3 < n &&
                   (data[i + 1] & 0xC0u) == 0x80u && (data[i + 2] & 0xC0u) == 0x80u &&
                   (data[i + 3] & 0xC0u) == 0x80u) {
            cp = (static_cast<uint32_t>(c0 & 0x07u) << 18) |
                 (static_cast<uint32_t>(data[i + 1] & 0x3Fu) << 12) |
                 (static_cast<uint32_t>(data[i + 2] & 0x3Fu) << 6) |
                 static_cast<uint32_t>(data[i + 3] & 0x3Fu);
            tail = 3;
        } else {
            ++i;
            continue;
        }
        if (cp <= 0x10FFFFu) {
            out += static_cast<wchar_t>(cp);
        }
        i += tail + 1;
    }
    return out;
}

cv::Mat bitmap_to_mat(JNIEnv *env, jobject bitmap) {
    AndroidBitmapInfo info;
    void *pixels = nullptr;
    if (AndroidBitmap_getInfo(env, bitmap, &info) < 0) {
        LOGD("AndroidBitmap_getInfo failed");
        return cv::Mat{};
    }
    if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
        LOGD("Bitmap format is not RGBA_8888");
        return cv::Mat{};
    }
    if (AndroidBitmap_lockPixels(env, bitmap, &pixels) < 0) {
        LOGD("AndroidBitmap_lockPixels failed");
        return cv::Mat{};
    }
    cv::Mat img(info.height, info.width, CV_8UC4, pixels);
    cv::Mat mat = img.clone();
    AndroidBitmap_unlockPixels(env, bitmap);
    return mat;
}

extern "C"
JNIEXPORT jlong JNICALL
Java_com_breeze_qnn_ChineseClipJni_nativeInit(JNIEnv *env, jobject thiz,
                                                                     jstring image_model_path,
                                                                     jstring text_model_path) {
    if (image_model_path == nullptr || text_model_path == nullptr) {
        return 0;
    }

    const char* image_model_path_str = env->GetStringUTFChars(image_model_path, nullptr);
    const char* text_model_path_str = env->GetStringUTFChars(text_model_path, nullptr);
    if (image_model_path_str == nullptr || text_model_path_str == nullptr) {
        return 0;
    }

    auto clip_solver = new ClipSolver();
    bool success = clip_solver->init(image_model_path_str, text_model_path_str);
    if (!success) {
        delete clip_solver;
        clip_solver = nullptr;
    }

    env->ReleaseStringUTFChars(image_model_path, image_model_path_str);
    env->ReleaseStringUTFChars(text_model_path, text_model_path_str);

    return (jlong)clip_solver;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_breeze_qnn_ChineseClipJni_nativeExtractImageFeature(JNIEnv *env,
                                                                                    jobject thiz,
                                                                                    jlong handle,
                                                                                    jobject bitmap,
                                                                                    jfloatArray result) {
    // bitmap转Mat
    cv::Mat image = bitmap_to_mat(env, bitmap);
    if (image.empty()) {
        return -1;
    }
    cv::Mat rgb(image.rows, image.cols, CV_8UC3);
    for (int y = 0; y < image.rows; ++y) {
        const uint8_t *src = image.ptr<uint8_t>(y);
        uint8_t *dst = rgb.ptr<uint8_t>(y);
        for (int x = 0; x < image.cols; ++x) {
            dst[3 * x] = src[4 * x];
            dst[3 * x + 1] = src[4 * x + 1];
            dst[3 * x + 2] = src[4 * x + 2];
        }
    }
    image = rgb;

    jsize size = env->GetArrayLength(result);
    if (size != 1024) {
        return -2;
    }

    jfloat* result_elems = env->GetFloatArrayElements(result, nullptr);
    if (result_elems == nullptr) {
        return -3;  // 获取 result 数组元素失败
    }

    // 提取图像特征
    auto clip_solver = (ClipSolver*)handle;
    if (clip_solver == nullptr) {
        return -4;
    }
    clip_solver->extract_image_feature(image, result_elems);

    env->ReleaseFloatArrayElements(result, result_elems, 0);

    return 0;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_breeze_qnn_ChineseClipJni_nativeExtractTextFeature(JNIEnv *env,
                                                                                   jobject thiz,
                                                                                   jlong handle,
                                                                                   jstring text,
                                                                                   jfloatArray result) {
    if (text == nullptr) {
        return -1;
    }

    const char* text_chars = env->GetStringUTFChars(text, nullptr);
    if (text_chars == nullptr) {
        return -1;
    }
    std::string text_str(text_chars);
    std::wstring keywords = utf8_to_wstring(text_str);

    env->ReleaseStringUTFChars(text, text_chars);

    jsize size = env->GetArrayLength(result);
    if (size != 1024) {
        return -2;
    }

    jfloat* result_elems = env->GetFloatArrayElements(result, nullptr);
    if (result_elems == nullptr) {
        return -3;  // 获取 result 数组元素失败
    }

    auto clip_solver = (ClipSolver*)handle;
    if (clip_solver == nullptr) {
        return -4;
    }
    clip_solver->extract_text_feature(keywords, result_elems);

    env->ReleaseFloatArrayElements(result, result_elems, 0);

    return 0;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_breeze_qnn_ChineseClipJni_nativeRelease(JNIEnv *env, jobject thiz, jlong handle) {
    auto clip_solver = (ClipSolver*)handle;
    if (clip_solver != nullptr) {
        clip_solver->release();
        delete clip_solver;
        clip_solver = nullptr;
    }
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_breeze_qnn_ChineseClipJni_nativeSelectBestImage(JNIEnv *env, jobject thiz,
                                                                   jlong handle,
                                                                   jfloatArray text_feature,
                                                                   jfloatArray images_feature,
                                                                   jint image_count,
                                                                   jfloatArray result) {
    auto clip_solver = (ClipSolver*)handle;
    if (clip_solver == nullptr) return -4;
    if (image_count <= 0) return -1;

    jsize dim       = env->GetArrayLength(text_feature);
    jsize imgs_len  = env->GetArrayLength(images_feature);
    jsize result_len = env->GetArrayLength(result);

    if (dim <= 0 || imgs_len != dim * image_count || result_len < image_count) return -2;

    jfloat* text_elems   = env->GetFloatArrayElements(text_feature,   nullptr);
    jfloat* images_elems = env->GetFloatArrayElements(images_feature, nullptr);
    jfloat* result_elems = env->GetFloatArrayElements(result,         nullptr);

    if (!text_elems || !images_elems || !result_elems) {
        if (text_elems)   env->ReleaseFloatArrayElements(text_feature,   text_elems,   JNI_ABORT);
        if (images_elems) env->ReleaseFloatArrayElements(images_feature, images_elems, JNI_ABORT);
        if (result_elems) env->ReleaseFloatArrayElements(result,         result_elems, JNI_ABORT);
        return -3;
    }

    int ret = clip_solver->select_best_image(text_elems, images_elems,
                                             (int)image_count, (int)dim, result_elems);

    env->ReleaseFloatArrayElements(text_feature,   text_elems,   JNI_ABORT);
    env->ReleaseFloatArrayElements(images_feature, images_elems, JNI_ABORT);
    env->ReleaseFloatArrayElements(result,         result_elems, 0);

    return ret;
}

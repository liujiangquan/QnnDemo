#include "clipsolver.h"
#include <cmath>
#ifdef __ARM_NEON
#include <arm_neon.h>
#endif

bool ClipSolver::init(const char* image_model_path, const char* text_model_path) {
    imageModel = std::make_unique<ImageModel>();
    textModel = std::make_unique<TextModel>();

    bool res = imageModel->init(image_model_path);
    if (!res) {
        LOGE("imageModel load fail!");
        return res;
    }

    res = textModel->init(text_model_path);
    if (!res) {
        LOGE("textModel load fail!");
        return res;
    }

    return res;
}

int ClipSolver::extract_image_feature(const cv::Mat& image, float* image_feature) {
    if (!imageModel) {
        return -1;
    }
    return imageModel->extract_image_feature(image, image_feature);
}

int ClipSolver::extract_text_feature(const std::wstring& text, float* text_feature) {
    if (!textModel) {
        return -1;
    }
    return textModel->extract_text_feature(text, text_feature);
}

static float cosine_similarity(const float* a, const float* b, int dim) {
    float dot = 0.0f, norm_a = 0.0f, norm_b = 0.0f;
#ifdef __ARM_NEON
    float32x4_t vdot   = vdupq_n_f32(0.0f);
    float32x4_t vnorm_a = vdupq_n_f32(0.0f);
    float32x4_t vnorm_b = vdupq_n_f32(0.0f);
    int i = 0;
    for (; i + 3 < dim; i += 4) {
        float32x4_t va = vld1q_f32(a + i);
        float32x4_t vb = vld1q_f32(b + i);
        vdot    = vmlaq_f32(vdot,    va, vb);
        vnorm_a = vmlaq_f32(vnorm_a, va, va);
        vnorm_b = vmlaq_f32(vnorm_b, vb, vb);
    }
    dot    = vaddvq_f32(vdot);
    norm_a = vaddvq_f32(vnorm_a);
    norm_b = vaddvq_f32(vnorm_b);
    for (; i < dim; ++i) {
        dot    += a[i] * b[i];
        norm_a += a[i] * a[i];
        norm_b += b[i] * b[i];
    }
#else
    for (int i = 0; i < dim; ++i) {
        dot    += a[i] * b[i];
        norm_a += a[i] * a[i];
        norm_b += b[i] * b[i];
    }
#endif
    if (norm_a <= 0.0f || norm_b <= 0.0f) return 0.0f;
    return dot / (sqrtf(norm_a) * sqrtf(norm_b));
}

int ClipSolver::select_best_image(const float* text_feature, const float* images_feature,
                                  int image_count, int dim, float* result) {
    // 置信度口径：logit = logit_scale(100) × cosine，再对候选图做 softmax 得每图概率。
    // 特征已 L2 归一化，cosine 即点积；与相册 0.492 阈值的标定口径一致。
    for (int i = 0; i < image_count; ++i) {
        result[i] = 100.0f * cosine_similarity(text_feature, images_feature + i * dim, dim);
    }
    float total = 0.0f;
    for (int i = 0; i < image_count; ++i) {
        total += expf(result[i]);
    }
    for (int i = 0; i < image_count; ++i) {
        result[i] = expf(result[i]) / total;
    }
    return 0;
}

void ClipSolver::release() {
    if (imageModel) {
        imageModel->release();
        imageModel.reset();
    }
    if (textModel) {
        textModel->release();
        textModel.reset();
    }
}
#ifndef COM_STEPOS_CLIP_CLIPSOLVER_H
#define COM_STEPOS_CLIP_CLIPSOLVER_H

#include "imagemodel.h"
#include "textmodel.h"

#include <android/log.h>

#define LOG_TAG "clip"

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

class ClipSolver {
public:
    bool init(const char* image_model_path, const char* text_model_path);

    int extract_image_feature(const cv::Mat& image, float* image_feature);

    int extract_text_feature(const std::wstring& text, float* text_feature);

    /**
     * 计算文本特征与多张图片特征的余弦相似度。
     * @param text_feature   文本特征向量，长度 dim
     * @param images_feature 所有图片特征平铺，长度 image_count * dim
     * @param image_count    候选图片数量
     * @param dim            单条特征向量维度
     * @param result         输出相似度分数，长度 image_count
     */
    int select_best_image(const float* text_feature, const float* images_feature,
                          int image_count, int dim, float* result);

    void release();

private:
    std::unique_ptr<ImageModel> imageModel;
    std::unique_ptr<TextModel> textModel;
};


#endif // COM_STEPOS_CLIP_CLIPSOLVER_H

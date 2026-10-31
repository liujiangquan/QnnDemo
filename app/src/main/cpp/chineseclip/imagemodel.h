#ifndef COM_STEPOS_CLIP_IMAGEMODEL_H
#define COM_STEPOS_CLIP_IMAGEMODEL_H

#include <iostream>
#include <vector>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include <MNN/Interpreter.hpp>
#include <MNN/ImageProcess.hpp>

class ImageModel {
public:
    bool init(const char* model_path);

    int extract_image_feature(const cv::Mat& image, float* image_feature);

    void release();

private:
    const int target_width = 224;
    const int target_height = 224;

    const float mean_vals[3] = {255.f * 0.48145466f, 255.f * 0.4578275f, 255.f * 0.40821073f};
    const float norm_vals[3] = {1.f / (255.f * 0.26862954f),
                                1.f / (255.f * 0.26130258f),
                                1.f / (255.f * 0.27577711f)};

    int m_precision = 2;
    int m_power = 0;
    int m_memory = 0;

    std::shared_ptr<MNN::Interpreter> image_net;
    MNN::Session* image_session;

    MNN::Tensor* input_tensor;
};


#endif // COM_STEPOS_CLIP_IMAGEMODEL_H

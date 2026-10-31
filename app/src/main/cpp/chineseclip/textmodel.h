#ifndef COM_STEPOS_CLIP_TEXTMODEL_H
#define COM_STEPOS_CLIP_TEXTMODEL_H

#include <iostream>
#include <vector>

#include <MNN/Interpreter.hpp>
#include <MNN/ImageProcess.hpp>

class TextModel {
public:
    bool init(const char* model_path);

    int extract_text_feature(const std::wstring& word, float* text_feature);

    void release();

private:
    int m_precision = 2;
    int m_power = 0;
    int m_memory = 0;

    std::shared_ptr<MNN::Interpreter> text_net;
    MNN::Session* text_session;

    MNN::Tensor* input_tensor;
};


#endif // COM_STEPOS_CLIP_TEXTMODEL_H

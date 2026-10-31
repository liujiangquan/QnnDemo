#include "textmodel.h"
#include "vocab.hpp"

bool TextModel::init(const char* model_path) {
    // create Interpreter
    text_net = std::shared_ptr<MNN::Interpreter>(MNN::Interpreter::createFromFile(model_path));
    if (text_net == nullptr) {
        return false;
    }

    // config
    MNN::ScheduleConfig config;
    config.numThread = 4;
    config.type = MNN_FORWARD_CPU;

    MNN::BackendConfig backend_config;
    backend_config.precision = (MNN::BackendConfig::PrecisionMode)m_precision;
    backend_config.power = (MNN::BackendConfig::PowerMode)m_power;
    backend_config.memory = (MNN::BackendConfig::MemoryMode)m_memory;
    config.backendConfig = &backend_config;

    // create session
    text_session = text_net->createSession(config);

    input_tensor = text_net->getSessionInput(text_session, nullptr);

    return true;
}

int TextModel::extract_text_feature(const std::wstring& word, float* text_feature) {
    // prepare input tensor
    MNN::Tensor host_input_tensor(input_tensor, input_tensor->getDimensionType());
    auto input_ptr = host_input_tensor.host<uint32_t>();

    const int max_length = 52;
    std::fill(input_ptr, input_ptr + max_length, 0);

    // 设置特殊标记
    int length = std::min(static_cast<int>(word.length()), max_length - 2);
    input_ptr[0] = 101;
    input_ptr[length + 1] = 102;

    // 填充词汇数据
    int index = 1;
    for (auto p: word) {
        if (index > max_length - 2) break; // 防止超出边界
        std::wstring oneWord(1, p);
        if (oneWord != L" ") {
            auto iter = vocab.find(oneWord);
            if (iter != vocab.end()) {
                input_ptr[index] = static_cast<uint32_t>(iter->second);
            }
            ++index;
        }
    }

    input_tensor->copyFromHostTensor(&host_input_tensor);

    // run session
    text_net->runSession(text_session);

    // process output tensor
    MNN::Tensor* output_tensor = text_net->getSessionOutput(text_session, nullptr);
    MNN::Tensor host_output_tensor(output_tensor, output_tensor->getDimensionType());
    output_tensor->copyToHostTensor(&host_output_tensor);

    auto output_ptr = host_output_tensor.host<float>();
    float sum = 0;
    for (int i = 0; i < 1024; i++) {
        sum += output_ptr[i] * output_ptr[i];
    }
    float norm = sqrt(sum);
    float epsilon = 1e-8; // 防止除以0
    for (int i = 0; i < 1024; i++) {
        output_ptr[i] /= (norm > epsilon ? norm : epsilon);
    }

    memcpy(text_feature, output_ptr, 1024 * sizeof(float));

    return 0;
}

void TextModel::release() {
    if (text_net != nullptr) {
        if (text_session != nullptr) {
            text_net->releaseSession(text_session);
            text_session = nullptr;
        }
        text_net->releaseModel();
        text_net.reset();
    }
}
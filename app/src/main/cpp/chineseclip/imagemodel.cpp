#include "imagemodel.h"

bool ImageModel::init(const char* model_path) {
    // create Interpreter
    image_net = std::shared_ptr<MNN::Interpreter>(MNN::Interpreter::createFromFile(model_path));
    if (image_net == nullptr) {
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
    image_session = image_net->createSession(config);

    input_tensor = image_net->getSessionInput(image_session, nullptr);

    return true;
}

int ImageModel::extract_image_feature(const cv::Mat& image, float* image_feature) {
    cv::Mat resize_image;
    cv::resize(image, resize_image, cv::Size(target_width, target_height), 0, 0, cv::INTER_CUBIC);

    // prepare input tensor
    MNN::CV::ImageProcess::Config config;
    config.sourceFormat = MNN::CV::ImageFormat::RGB;
    config.destFormat = MNN::CV::ImageFormat::RGB;
    memcpy(config.mean, mean_vals, sizeof(mean_vals));
    memcpy(config.normal, norm_vals, sizeof(norm_vals));
    std::shared_ptr<MNN::CV::ImageProcess> pretreat(MNN::CV::ImageProcess::create(config));
    pretreat->convert(resize_image.data, target_width, target_height, resize_image.step[0], input_tensor);

    // run session
    image_net->runSession(image_session);

    // process output tensor
    MNN::Tensor* output_tensor = image_net->getSessionOutput(image_session, nullptr);
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

    memcpy(image_feature, output_ptr, 1024 * sizeof(float));

    return 0;
}

void ImageModel::release() {
    if (image_net != nullptr) {
        if (image_session != nullptr) {
            image_net->releaseSession(image_session);
            image_session = nullptr;
        }
        image_net->releaseModel();
        image_net.reset();
    }
}
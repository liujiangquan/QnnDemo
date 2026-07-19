// =============================================================================
// qairt_runtime.h
//
// QAIRT (Qualcomm AI Runtime) Easy API 的轻量 C++ 封装。
//
// 设计目标：
//   - 仅通过 dlopen + 函数指针表使用 QAIRT C Easy API，不静态链接 QAIRT 库
//   - 提供完整的「加载 DLC → 构建运行时 → 执行推理 → 释放」生命周期
//   - 暴露图/张量元信息查询接口，便于上层 UI 展示
//
// 依赖的 QAIRT 共享库（运行时从 jniLibs 加载）：
//   - libQairtSystem.so  (System Builder / DLC / Context / Qairt V1 接口入口)
//   - libQairtCpu.so     (CPU 后端，默认)
//   - libQairtGpu.so     (GPU 后端，可选)
//   - libQairtHtp.so     (HTP/NPU 后端，可选)
// =============================================================================

#ifndef QNN_DEMO_QAIRT_RUNTIME_H
#define QNN_DEMO_QAIRT_RUNTIME_H

#include <cstdint>
#include <string>
#include <vector>

namespace qnn_demo {

// 后端类型（对应 QAIRT_System_BackendType_t 的枚举值）
enum class BackendType : int {
    CPU = 0,
    GPU = 1,
    HTP = 3,
};

// 张量元信息
struct TensorInfo {
    std::string name;
    uint64_t id = 0;
    uint32_t rank = 0;
    std::vector<uint32_t> dimensions;
    int32_t dataType = 0;  // Qairt_DataType_t 的原始值
    size_t elementBytes = 0;
    size_t totalElements = 0;
    size_t totalBytes = 0;
};

// 图元信息
struct GraphInfo {
    std::string name;
    std::vector<TensorInfo> inputs;
    std::vector<TensorInfo> outputs;
};

// 推理结果
struct InferenceResult {
    std::string graphName;
    // 每个输出张量：扁平化的字节缓冲
    std::vector<std::vector<uint8_t>> outputs;
    // 耗时（毫秒）
    double elapsedMs = 0.0;
    // 错误信息（成功时为空）
    std::string error;
};

class QairtRuntime {
public:
    QairtRuntime();
    ~QairtRuntime();

    QairtRuntime(const QairtRuntime&) = delete;
    QairtRuntime& operator=(const QairtRuntime&) = delete;

    // 初始化：dlopen libQairtSystem.so 并加载所需接口表
    // libSearchPath 为额外搜索路径（一般为 APP 的 nativeLibraryDir）
    bool init(const std::string& libSearchPath = "");

    // 加载 DLC 模型并构建运行时
    // dlcPath: DLC 文件绝对路径
    // backend: 后端类型
    bool loadDlc(const std::string& dlcPath, BackendType backend);

    // 释放运行时资源
    void cleanup();

    // 查询图信息
    const std::vector<GraphInfo>& graphInfos() const { return graphs_; }

    // 对指定图执行一次推理
    // inputs: 每个输入张量的扁平字节缓冲（顺序与 graphInfos().inputs 对应）
    // 返回推理结果；失败时 result.error 非空
    InferenceResult execute(const std::string& graphName,
                            const std::vector<std::vector<uint8_t>>& inputs);

    // QAIRT 后端库构建版本号
    std::string backendBuildId() const { return backendBuildId_; }

    bool isReady() const { return ready_; }

private:
    // PIMPL 风格：将 QAIRT 函数指针表与句柄隐藏到实现文件中
    struct Impl;
    Impl* impl_ = nullptr;

    bool ready_ = false;
    std::vector<GraphInfo> graphs_;
    std::string backendBuildId_;

    // 查询所有图与张量元信息
    bool queryGraphInfos();

    // 读取单个张量元信息（参数为 QAIRT 不透明句柄，用 void* 避免头文件依赖 QAIRT 类型）
    TensorInfo readTensorInfo(void* th);
};

}  // namespace qnn_demo

#endif  // QNN_DEMO_QAIRT_RUNTIME_H

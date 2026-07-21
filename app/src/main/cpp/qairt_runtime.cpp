// =============================================================================
// qairt_runtime.cpp
//
// QNN Direct API 的运行时实现（类名沿用 QairtRuntime 以避免 JNI 破坏）。
//
// 关键设计：绕开 QAIRT 上层，直接用 libQnnSystem.so + libQnnCpu/libQnnHtp.so
// 的稳定 QNN Core API。原因：vendor firmware 提供的是 QNN 2.46，签名 skel 也
// 是 2.46；而 QAIRT 2.48 上层跟 QNN 2.46 底层 FastRPC 协议对不上，会在
// DspTransport.openSession 时报 AEE_ENOSUCHMOD (0x80000406)。
//
// 库分工（见 jniLibs/arm64-v8a/）：
//   - libQnnSystem.so     (SDK 2.48)  纯 host，含 DLC 解析 + composeGraphs
//   - libQnnCpu.so        (SDK 2.48)  CPU 后端，vendor 不提供
//   - libQnnHtp.so        (vendor 2.46) HTP 后端，跟 vendor skel 匹配
//   - libQnnHtpV81Stub.so (vendor 2.46) FastRPC stub
//   - libQnnHtpPrepare.so (vendor 2.46) 图编译依赖
// =============================================================================

#include "qairt_runtime.h"

#include <dlfcn.h>
#include <android/log.h>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <sstream>

#include "QNN/QnnCommon.h"
#include "QNN/QnnInterface.h"
#include "QNN/QnnTypes.h"
#include "QNN/QnnBackend.h"
#include "QNN/QnnContext.h"
#include "QNN/QnnDevice.h"
#include "QNN/QnnGraph.h"
#include "QNN/QnnLog.h"
#include "QNN/QnnTensor.h"
#include "QNN/HTP/QnnHtpDevice.h"
#include "QNN/HTP/QnnHtpDeviceConfigShared.h"
#include "QNN/System/QnnSystemInterface.h"
#include "QNN/System/QnnSystemContext.h"

#define LOG_TAG "QnnDemo"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,  LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

namespace qnn_demo {

// Snapdragon 8845 SoC model = 685（canoe / SC8480XP）
static constexpr uint32_t kSocModel8845 = 685;

// Qnn_DataType_t → 字节宽度
static size_t dataTypeToBytes(Qnn_DataType_t dt) {
    switch (dt) {
        case QNN_DATATYPE_INT_8:
        case QNN_DATATYPE_UINT_8:
        case QNN_DATATYPE_SFIXED_POINT_8:
        case QNN_DATATYPE_UFIXED_POINT_8:
        case QNN_DATATYPE_BOOL_8:
            return 1;
        case QNN_DATATYPE_INT_16:
        case QNN_DATATYPE_UINT_16:
        case QNN_DATATYPE_SFIXED_POINT_16:
        case QNN_DATATYPE_UFIXED_POINT_16:
        case QNN_DATATYPE_FLOAT_16:
            return 2;
        case QNN_DATATYPE_INT_32:
        case QNN_DATATYPE_UINT_32:
        case QNN_DATATYPE_SFIXED_POINT_32:
        case QNN_DATATYPE_UFIXED_POINT_32:
        case QNN_DATATYPE_FLOAT_32:
            return 4;
        case QNN_DATATYPE_INT_64:
        case QNN_DATATYPE_UINT_64:
        case QNN_DATATYPE_FLOAT_64:
            return 8;
        default:
            return 0;
    }
}

// 便捷宏：从 Qnn_Tensor_t 取 V1/V2 字段
static inline const Qnn_TensorV1_t* asV1(const Qnn_Tensor_t& t) {
    return t.version == QNN_TENSOR_VERSION_1 ? &t.v1 : nullptr;
}
static inline const Qnn_TensorV2_t* asV2(const Qnn_Tensor_t& t) {
    return t.version == QNN_TENSOR_VERSION_2 ? &t.v2 : nullptr;
}
static inline Qnn_TensorV1_t* asV1Mut(Qnn_Tensor_t& t) {
    return t.version == QNN_TENSOR_VERSION_1 ? &t.v1 : nullptr;
}
static inline Qnn_TensorV2_t* asV2Mut(Qnn_Tensor_t& t) {
    return t.version == QNN_TENSOR_VERSION_2 ? &t.v2 : nullptr;
}

// 从 Qnn_Tensor_t 抽出通用信息填充到 TensorInfo
static TensorInfo makeTensorInfo(const Qnn_Tensor_t& t) {
    TensorInfo ti;
    const char* name = nullptr;
    uint32_t rank = 0;
    const uint32_t* dims = nullptr;
    Qnn_DataType_t dt = QNN_DATATYPE_UNDEFINED;
    uint32_t id = 0;
    if (auto* v = asV1(t)) {
        name = v->name; rank = v->rank; dims = v->dimensions; dt = v->dataType; id = v->id;
    } else if (auto* v = asV2(t)) {
        name = v->name; rank = v->rank; dims = v->dimensions; dt = v->dataType; id = v->id;
    }
    if (name) ti.name = name;
    ti.id = id;
    ti.rank = rank;
    ti.dataType = static_cast<int32_t>(dt);
    ti.elementBytes = dataTypeToBytes(dt);
    if (dims && rank > 0) {
        ti.dimensions.assign(dims, dims + rank);
        ti.totalElements = 1;
        for (uint32_t d : ti.dimensions) ti.totalElements *= d;
    } else {
        ti.totalElements = 0;
    }
    ti.totalBytes = ti.totalElements * ti.elementBytes;
    return ti;
}

// ---------------------------------------------------------------------------
// Impl：封装 dlopen 得到的接口表 + 运行时句柄
// ---------------------------------------------------------------------------
struct QairtRuntime::Impl {
    // dlopen handles
    void* systemLib = nullptr;      // libQnnSystem.so
    void* backendLib = nullptr;     // libQnnCpu.so / libQnnHtp.so

    // Interface tables
    const QnnInterface_t* qnnIface = nullptr;
    const QnnSystemInterface_t* sysIface = nullptr;

    // Runtime handles
    Qnn_LogHandle_t backendLog = nullptr;
    Qnn_LogHandle_t sysLog = nullptr;
    Qnn_BackendHandle_t backendHandle = nullptr;
    Qnn_DeviceHandle_t deviceHandle = nullptr;
    Qnn_ContextHandle_t contextHandle = nullptr;
    QnnSystemDlc_Handle_t dlcHandle = nullptr;

    BackendType currentBackend = BackendType::CPU;

    // 由 composeGraphs 返回，所有权属 sysIface（DLC 释放时一并释放）
    QnnSystemContext_GraphInfo_t* composedGraphs = nullptr;
    uint32_t numComposedGraphs = 0;

    // 每个 graph 的执行句柄 + tensor 数组（浅拷贝自 composedGraphs）
    struct GraphSlot {
        std::string name;
        Qnn_GraphHandle_t handle = nullptr;
        std::vector<Qnn_Tensor_t> inputs;
        std::vector<Qnn_Tensor_t> outputs;
    };
    std::vector<GraphSlot> graphSlots;

    ~Impl() { releaseAll(); }

    void releaseAll() {
        if (contextHandle && qnnIface) {
            qnnIface->QNN_INTERFACE_VER_NAME.contextFree(contextHandle, nullptr);
            contextHandle = nullptr;
        }
        if (deviceHandle && qnnIface) {
            qnnIface->QNN_INTERFACE_VER_NAME.deviceFree(deviceHandle);
            deviceHandle = nullptr;
        }
        if (backendHandle && qnnIface) {
            qnnIface->QNN_INTERFACE_VER_NAME.backendFree(backendHandle);
            backendHandle = nullptr;
        }
        if (backendLog && qnnIface && qnnIface->QNN_INTERFACE_VER_NAME.logFree) {
            qnnIface->QNN_INTERFACE_VER_NAME.logFree(backendLog);
            backendLog = nullptr;
        }
        if (dlcHandle && sysIface && sysIface->QNN_SYSTEM_INTERFACE_VER_NAME.systemDlcFree) {
            sysIface->QNN_SYSTEM_INTERFACE_VER_NAME.systemDlcFree(dlcHandle);
            dlcHandle = nullptr;
        }
        if (sysLog && sysIface && sysIface->QNN_SYSTEM_INTERFACE_VER_NAME.systemLogFree) {
            sysIface->QNN_SYSTEM_INTERFACE_VER_NAME.systemLogFree(sysLog);
            sysLog = nullptr;
        }
        composedGraphs = nullptr;
        numComposedGraphs = 0;
        graphSlots.clear();

        if (backendLib) {
            dlclose(backendLib);
            backendLib = nullptr;
            qnnIface = nullptr;
        }
        if (systemLib) {
            dlclose(systemLib);
            systemLib = nullptr;
            sysIface = nullptr;
        }
    }
};

// ---------------------------------------------------------------------------
QairtRuntime::QairtRuntime() : impl_(new Impl()) {}
QairtRuntime::~QairtRuntime() { cleanup(); delete impl_; }

// 简单的 QNN 日志回调
static void qnnLogCallback(const char* fmt,
                           QnnLog_Level_t level,
                           uint64_t /*timestamp*/,
                           va_list argp) {
    int prio = ANDROID_LOG_INFO;
    switch (level) {
        case QNN_LOG_LEVEL_ERROR: prio = ANDROID_LOG_ERROR; break;
        case QNN_LOG_LEVEL_WARN:  prio = ANDROID_LOG_WARN;  break;
        case QNN_LOG_LEVEL_INFO:  prio = ANDROID_LOG_INFO;  break;
        default:                  prio = ANDROID_LOG_DEBUG; break;
    }
    __android_log_vprint(prio, "QNN", fmt, argp);
}

// 加载指定共享库（先 libSearchPath，再默认系统路径）
static void* openLib(const char* name, const std::string& searchPath) {
    if (!searchPath.empty()) {
        std::string full = searchPath + "/" + name;
        void* h = dlopen(full.c_str(), RTLD_NOW | RTLD_LOCAL);
        if (h) return h;
    }
    return dlopen(name, RTLD_NOW | RTLD_LOCAL);
}

// ---------------------------------------------------------------------------
// init：加载 libQnnSystem.so 并抽出 SystemInterface 表（还没绑到具体后端）
// ---------------------------------------------------------------------------
bool QairtRuntime::init(const std::string& libSearchPath) {
    if (ready_) return true;

    // 关键：所有 HTP 后端启动前必须先设 ADSP_LIBRARY_PATH，否则 FastRPC
    // 找不到 vendor skel（跟 AIEngine 对齐）
    const char* adspPath =
        "/vendor/lib/rfsa/adsp;/vendor/dsp/cdsp;/vendor/dsp/adsp;/system/lib/rfsa/adsp;/dsp";
    setenv("ADSP_LIBRARY_PATH", adspPath, 1);
    LOGI("ADSP_LIBRARY_PATH=%s", adspPath);

    impl_->systemLib = openLib("libQnnSystem.so", libSearchPath);
    if (!impl_->systemLib) {
        LOGE("dlopen libQnnSystem.so 失败: %s", dlerror());
        return false;
    }

    using SysProvidersFn =
        Qnn_ErrorHandle_t (*)(const QnnSystemInterface_t***, uint32_t*);
    auto sysProviders = reinterpret_cast<SysProvidersFn>(
        dlsym(impl_->systemLib, "QnnSystemInterface_getProviders"));
    if (!sysProviders) {
        LOGE("dlsym QnnSystemInterface_getProviders 失败: %s", dlerror());
        return false;
    }
    const QnnSystemInterface_t** providers = nullptr;
    uint32_t numProviders = 0;
    Qnn_ErrorHandle_t s = sysProviders(&providers, &numProviders);
    if (s != QNN_SUCCESS || numProviders == 0 || !providers || !providers[0]) {
        LOGE("QnnSystemInterface_getProviders 返回失败: 0x%llx",
             (unsigned long long)s);
        return false;
    }
    impl_->sysIface = providers[0];
    LOGI("QnnSystem 接口就绪, api=%u.%u.%u",
         impl_->sysIface->systemApiVersion.major,
         impl_->sysIface->systemApiVersion.minor,
         impl_->sysIface->systemApiVersion.patch);

    // 单独给 Sys 建 log（DLC 解析用）
    auto sysLogCreate = impl_->sysIface->QNN_SYSTEM_INTERFACE_VER_NAME.systemLogCreate;
    if (sysLogCreate) {
        Qnn_ErrorHandle_t rs = sysLogCreate(qnnLogCallback, QNN_LOG_LEVEL_WARN,
                                            &impl_->sysLog);
        if (rs != QNN_SUCCESS) {
            LOGW("systemLogCreate 失败: 0x%llx（继续，无日志）",
                 (unsigned long long)rs);
            impl_->sysLog = nullptr;
        }
    }

    ready_ = true;
    return true;
}

// ---------------------------------------------------------------------------
// loadDlc：dlopen 后端 → 建 Log/Backend/Device/Context → DLC compose
// ---------------------------------------------------------------------------
bool QairtRuntime::loadDlc(const std::string& dlcPath, BackendType backend) {
    if (!ready_ || !impl_->sysIface) {
        LOGE("Runtime 未初始化，请先调用 init()");
        return false;
    }

    // 切后端时先清掉旧的 backend/context/device
    if (impl_->backendLib || impl_->backendHandle) {
        if (impl_->contextHandle) {
            impl_->qnnIface->QNN_INTERFACE_VER_NAME.contextFree(impl_->contextHandle, nullptr);
            impl_->contextHandle = nullptr;
        }
        if (impl_->deviceHandle) {
            impl_->qnnIface->QNN_INTERFACE_VER_NAME.deviceFree(impl_->deviceHandle);
            impl_->deviceHandle = nullptr;
        }
        if (impl_->backendHandle) {
            impl_->qnnIface->QNN_INTERFACE_VER_NAME.backendFree(impl_->backendHandle);
            impl_->backendHandle = nullptr;
        }
        if (impl_->backendLog) {
            impl_->qnnIface->QNN_INTERFACE_VER_NAME.logFree(impl_->backendLog);
            impl_->backendLog = nullptr;
        }
        if (impl_->backendLib) {
            dlclose(impl_->backendLib);
            impl_->backendLib = nullptr;
            impl_->qnnIface = nullptr;
        }
        impl_->graphSlots.clear();
        impl_->composedGraphs = nullptr;
        impl_->numComposedGraphs = 0;
    }
    impl_->currentBackend = backend;

    // 1) 挑对应后端库
    const char* backendLibName = nullptr;
    switch (backend) {
        case BackendType::CPU: backendLibName = "libQnnCpu.so"; break;
        case BackendType::GPU: backendLibName = "libQnnGpu.so"; break;
        case BackendType::HTP: backendLibName = "libQnnHtp.so"; break;
        default:               backendLibName = "libQnnCpu.so"; break;
    }
    impl_->backendLib = openLib(backendLibName, "");
    if (!impl_->backendLib) {
        LOGE("dlopen %s 失败: %s", backendLibName, dlerror());
        return false;
    }

    using BEProvidersFn = Qnn_ErrorHandle_t (*)(const QnnInterface_t***, uint32_t*);
    auto beProviders = reinterpret_cast<BEProvidersFn>(
        dlsym(impl_->backendLib, "QnnInterface_getProviders"));
    if (!beProviders) {
        LOGE("dlsym QnnInterface_getProviders 失败: %s", dlerror());
        return false;
    }
    const QnnInterface_t** providers = nullptr;
    uint32_t numProviders = 0;
    Qnn_ErrorHandle_t s = beProviders(&providers, &numProviders);
    if (s != QNN_SUCCESS || numProviders == 0 || !providers || !providers[0]) {
        LOGE("QnnInterface_getProviders 失败: 0x%llx", (unsigned long long)s);
        return false;
    }
    impl_->qnnIface = providers[0];
    LOGI("%s 接口就绪, api=%u.%u.%u backend=%u",
         backendLibName,
         impl_->qnnIface->apiVersion.coreApiVersion.major,
         impl_->qnnIface->apiVersion.coreApiVersion.minor,
         impl_->qnnIface->apiVersion.coreApiVersion.patch,
         impl_->qnnIface->backendId);

    const auto& iface = impl_->qnnIface->QNN_INTERFACE_VER_NAME;

    // 2) Log
    if (iface.logCreate) {
        Qnn_ErrorHandle_t rs = iface.logCreate(qnnLogCallback, QNN_LOG_LEVEL_WARN,
                                               &impl_->backendLog);
        if (rs != QNN_SUCCESS) {
            LOGW("logCreate 失败: 0x%llx", (unsigned long long)rs);
            impl_->backendLog = nullptr;
        }
    }

    // 3) Backend
    s = iface.backendCreate(impl_->backendLog, nullptr, &impl_->backendHandle);
    if (s != QNN_SUCCESS) {
        LOGE("backendCreate 失败: 0x%llx", (unsigned long long)s);
        return false;
    }
    LOGI("QnnBackend_create 成功");

    // 4) Device（HTP 需要显式配置 signed PD + SoC model）
    if (backend == BackendType::HTP) {
        QnnHtpDevice_CustomConfig_t socCfg{};
        socCfg.option = QNN_HTP_DEVICE_CONFIG_OPTION_SOC;
        socCfg.socModel = kSocModel8845;

        QnnHtpDevice_CustomConfig_t signedPdCfg{};
        signedPdCfg.option = QNN_HTP_DEVICE_CONFIG_OPTION_SIGNEDPD;
        signedPdCfg.useSignedProcessDomain.deviceId = 0;
        signedPdCfg.useSignedProcessDomain.useSignedProcessDomain = true;

        QnnDevice_Config_t socDevCfg{};
        socDevCfg.option = QNN_DEVICE_CONFIG_OPTION_CUSTOM;
        socDevCfg.customConfig = &socCfg;

        QnnDevice_Config_t signedPdDevCfg{};
        signedPdDevCfg.option = QNN_DEVICE_CONFIG_OPTION_CUSTOM;
        signedPdDevCfg.customConfig = &signedPdCfg;

        const QnnDevice_Config_t* deviceCfgs[] = {&socDevCfg, &signedPdDevCfg, nullptr};
        if (iface.deviceCreate) {
            s = iface.deviceCreate(impl_->backendLog, deviceCfgs, &impl_->deviceHandle);
            if (s != QNN_SUCCESS) {
                LOGE("HTP deviceCreate 失败: 0x%llx (socModel=%u signedPD=1)",
                     (unsigned long long)s, kSocModel8845);
                return false;
            }
            LOGI("HTP Device 创建成功 socModel=%u signedPD=1", kSocModel8845);
        }
    } else {
        // CPU / GPU 直接建默认 device（可以 NULL）
        if (iface.deviceCreate) {
            s = iface.deviceCreate(impl_->backendLog, nullptr, &impl_->deviceHandle);
            if (s != QNN_SUCCESS) {
                LOGW("非 HTP deviceCreate 失败: 0x%llx（继续用 NULL device）",
                     (unsigned long long)s);
                impl_->deviceHandle = nullptr;
            }
        }
    }

    // 5) Context
    s = iface.contextCreate(impl_->backendHandle,
                            impl_->deviceHandle,
                            nullptr, &impl_->contextHandle);
    if (s != QNN_SUCCESS) {
        LOGE("contextCreate 失败: 0x%llx", (unsigned long long)s);
        return false;
    }
    LOGI("QnnContext_create 成功");

    // 6) DLC 载入
    auto dlcCreate = impl_->sysIface->QNN_SYSTEM_INTERFACE_VER_NAME.systemDlcCreateFromFile;
    if (!dlcCreate) {
        LOGE("libQnnSystem.so 不含 systemDlcCreateFromFile（版本过老？）");
        return false;
    }
    s = dlcCreate(impl_->sysLog, dlcPath.c_str(), &impl_->dlcHandle);
    if (s != QNN_SUCCESS || !impl_->dlcHandle) {
        LOGE("systemDlcCreateFromFile(%s) 失败: 0x%llx",
             dlcPath.c_str(), (unsigned long long)s);
        return false;
    }
    LOGI("DLC 加载成功: %s", dlcPath.c_str());

    // 7) composeGraphs：把 DLC 里的图编排到 context 上
    auto composeGraphs = impl_->sysIface->QNN_SYSTEM_INTERFACE_VER_NAME.systemDlcComposeGraphs;
    if (!composeGraphs) {
        LOGE("libQnnSystem.so 不含 systemDlcComposeGraphs");
        return false;
    }
    QnnSystemContext_GraphInfo_t* infos = nullptr;
    uint32_t numGraphs = 0;
    s = composeGraphs(impl_->dlcHandle,
                      /*graphConfigs*/ nullptr,
                      /*numGraphConfigs*/ 0,
                      impl_->backendHandle,
                      impl_->contextHandle,
                      *impl_->qnnIface,   // 传整个 QnnInterface_t
                      QNN_SYSTEM_CONTEXT_GRAPH_INFO_VERSION_1,
                      &infos, &numGraphs);
    if (s != QNN_SUCCESS || !infos || numGraphs == 0) {
        LOGE("systemDlcComposeGraphs 失败: 0x%llx graphs=%p n=%u",
             (unsigned long long)s, infos, numGraphs);
        return false;
    }
    impl_->composedGraphs = infos;
    impl_->numComposedGraphs = numGraphs;
    LOGI("composeGraphs 成功: %u 个 graph", numGraphs);

    // 8) 建立 GraphSlot：从每个 composed graph 拿到句柄（graphRetrieve）
    graphs_.clear();
    impl_->graphSlots.clear();
    impl_->graphSlots.reserve(numGraphs);
    for (uint32_t i = 0; i < numGraphs; ++i) {
        const auto& info = infos[i];
        if (info.version != QNN_SYSTEM_CONTEXT_GRAPH_INFO_VERSION_1) {
            LOGW("graph[%u] 不是 V1 版本，跳过", i);
            continue;
        }
        const auto& v1 = info.graphInfoV1;

        Impl::GraphSlot slot;
        slot.name = v1.graphName ? v1.graphName : "";

        if (iface.graphRetrieve) {
            Qnn_ErrorHandle_t rs = iface.graphRetrieve(impl_->contextHandle,
                                                       slot.name.c_str(),
                                                       &slot.handle);
            if (rs != QNN_SUCCESS) {
                LOGW("graphRetrieve(%s) 失败: 0x%llx",
                     slot.name.c_str(), (unsigned long long)rs);
            }
        }

        // composeGraphs 只添加节点，未 finalize —— 必须 finalize 才能 execute
        if (slot.handle && iface.graphFinalize) {
            Qnn_ErrorHandle_t rs = iface.graphFinalize(slot.handle, nullptr, nullptr);
            if (rs != QNN_SUCCESS) {
                LOGE("graphFinalize(%s) 失败: 0x%llx",
                     slot.name.c_str(), (unsigned long long)rs);
                return false;
            }
            LOGI("graphFinalize(%s) 成功", slot.name.c_str());
        }

        // 拷贝 tensor 元数据（Qnn_Tensor_t 是浅结构，dim/name 指针指到 DLC 内部内存）
        slot.inputs.assign(v1.graphInputs,  v1.graphInputs  + v1.numGraphInputs);
        slot.outputs.assign(v1.graphOutputs, v1.graphOutputs + v1.numGraphOutputs);

        // 转成公开 GraphInfo（供 UI/JNI 使用）
        GraphInfo gi;
        gi.name = slot.name;
        gi.inputs.reserve(v1.numGraphInputs);
        for (uint32_t j = 0; j < v1.numGraphInputs; ++j) {
            gi.inputs.push_back(makeTensorInfo(v1.graphInputs[j]));
        }
        gi.outputs.reserve(v1.numGraphOutputs);
        for (uint32_t j = 0; j < v1.numGraphOutputs; ++j) {
            gi.outputs.push_back(makeTensorInfo(v1.graphOutputs[j]));
        }

        impl_->graphSlots.push_back(std::move(slot));
        graphs_.push_back(std::move(gi));
    }
    LOGI("图信息就绪: %zu 个", graphs_.size());
    return true;
}

// ---------------------------------------------------------------------------
// execute：绑定输入/输出 buffer，调用 graphExecute
// ---------------------------------------------------------------------------
InferenceResult QairtRuntime::execute(const std::string& graphName,
                                       const std::vector<std::vector<uint8_t>>& inputs) {
    InferenceResult result;
    result.graphName = graphName;
    if (!ready_ || !impl_->qnnIface || !impl_->contextHandle) {
        result.error = "Runtime 未加载模型";
        return result;
    }

    Impl::GraphSlot* slot = nullptr;
    for (auto& s : impl_->graphSlots) {
        if (s.name == graphName) { slot = &s; break; }
    }
    if (!slot) {
        result.error = "未找到图: " + graphName;
        return result;
    }
    if (!slot->handle) {
        result.error = "图句柄为空 (graphRetrieve 失败？)";
        return result;
    }
    if (inputs.size() != slot->inputs.size()) {
        std::ostringstream oss;
        oss << "输入数量不匹配: 期望 " << slot->inputs.size()
            << " 实际 " << inputs.size();
        result.error = oss.str();
        return result;
    }

    // 拷贝 tensor 数组（要修改 clientBuf 指向我们的 buffer，不能改原始 DLC 里的）
    std::vector<Qnn_Tensor_t> inTensors  = slot->inputs;
    std::vector<Qnn_Tensor_t> outTensors = slot->outputs;

    // 输入 buffer 大小校验 + 绑定
    for (size_t j = 0; j < inTensors.size(); ++j) {
        Qnn_ClientBuffer_t* cb = nullptr;
        if (auto* v1 = asV1Mut(inTensors[j])) cb = &v1->clientBuf;
        else if (auto* v2 = asV2Mut(inTensors[j])) cb = &v2->clientBuf;
        if (!cb) { result.error = "输入 tensor 版本不支持"; return result; }

        TensorInfo ti = makeTensorInfo(inTensors[j]);
        size_t exp = ti.totalBytes;
        size_t act = inputs[j].size();
        if (exp == 0 || act != exp) {
            std::ostringstream oss;
            oss << "输入[" << j << "] 大小不匹配 expected=" << exp
                << " actual=" << act;
            result.error = oss.str();
            return result;
        }
        cb->data = const_cast<uint8_t*>(inputs[j].data());
        cb->dataSize = static_cast<uint32_t>(act);
    }

    // 输出 buffer 分配 + 绑定
    result.outputs.resize(outTensors.size());
    for (size_t j = 0; j < outTensors.size(); ++j) {
        Qnn_ClientBuffer_t* cb = nullptr;
        if (auto* v1 = asV1Mut(outTensors[j])) cb = &v1->clientBuf;
        else if (auto* v2 = asV2Mut(outTensors[j])) cb = &v2->clientBuf;
        if (!cb) { result.error = "输出 tensor 版本不支持"; return result; }

        TensorInfo ti = makeTensorInfo(outTensors[j]);
        if (ti.totalBytes == 0) {
            std::ostringstream oss;
            oss << "输出[" << j << "] totalBytes=0";
            result.error = oss.str();
            return result;
        }
        result.outputs[j].assign(ti.totalBytes, 0);
        cb->data = result.outputs[j].data();
        cb->dataSize = static_cast<uint32_t>(ti.totalBytes);
    }

    const auto& iface = impl_->qnnIface->QNN_INTERFACE_VER_NAME;
    auto t0 = std::chrono::steady_clock::now();
    Qnn_ErrorHandle_t s = iface.graphExecute(slot->handle,
                                             inTensors.data(),
                                             static_cast<uint32_t>(inTensors.size()),
                                             outTensors.data(),
                                             static_cast<uint32_t>(outTensors.size()),
                                             /*profile*/ nullptr,
                                             /*signal*/ nullptr);
    auto t1 = std::chrono::steady_clock::now();
    result.elapsedMs = std::chrono::duration<double, std::milli>(t1 - t0).count();

    if (s != QNN_SUCCESS) {
        std::ostringstream oss;
        oss << "graphExecute 失败: 0x" << std::hex << s;
        result.error = oss.str();
        return result;
    }
    LOGI("推理成功: %s, 耗时 %.3f ms", graphName.c_str(), result.elapsedMs);
    return result;
}

// ---------------------------------------------------------------------------
void QairtRuntime::cleanup() {
    if (impl_) impl_->releaseAll();
    ready_ = false;
    graphs_.clear();
    backendBuildId_.clear();
}

// queryGraphInfos: 保留虚函数式接口签名（现在实际在 loadDlc 里完成）
bool QairtRuntime::queryGraphInfos() { return true; }

// readTensorInfo: 保留旧接口，兼容 header（当前实现不再需要）
TensorInfo QairtRuntime::readTensorInfo(void* /*th*/) { return {}; }

}  // namespace qnn_demo

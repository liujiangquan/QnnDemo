// =============================================================================
// qairt_runtime.cpp
//
// QAIRT Easy API 的运行时实现。通过 dlopen 加载 libQairtSystem.so，再通过
// QairtSystemInterface_getInterface 取出各 V1 接口表，完成 DLC 加载、运行时
// 构建与推理执行。
//
// 注意：QAIRT 头文件由 docs/setup_demo.ps1 从 SDK 拷贝到 ./qairt_include/ 目录下。
// =============================================================================

#include "qairt_runtime.h"

#include <dlfcn.h>
#include <android/log.h>
#include <chrono>
#include <cstring>
#include <memory>
#include <sstream>
#include <type_traits>

// QAIRT C API 头文件
#include "QairtCommon/QairtCommon.h"
#include "QairtLog/QairtLog.h"
#include "QairtSystem/QairtSystemCommon.h"
#include "QairtSystem/QairtSystemLog.h"
#include "QairtSystem/QairtSystemDlc.h"
#include "QairtSystem/QairtSystemBuilder.h"
#include "QairtSystem/Qairt.h"
#include "QairtSystem/QairtSystemContext.h"

#define LOG_TAG "QnnDemo"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,  LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

namespace qnn_demo {

// ---------------------------------------------------------------------------
// 工具函数：QAIRT 数据类型 → 字节宽度
// QAIRT 数据类型枚举的低字节采用 BCD 编码（0x16 表示 16 位，0x32 表示 32 位）
// ---------------------------------------------------------------------------
static size_t dataTypeToBytes(int32_t dt) {
    if (dt == 0x7FFFFFFF) return 0;
    int hi = (dt & 0xF0) >> 4;
    int lo = (dt & 0x0F);
    int bits = hi * 10 + lo;
    if (bits <= 0) return 0;
    return static_cast<size_t>((bits + 7) / 8);
}

// ---------------------------------------------------------------------------
// 运行时实现：持有所有 QAIRT 函数指针表与句柄
// ---------------------------------------------------------------------------
struct QairtRuntime::Impl {
    // dlopen 句柄
    void* sysLib = nullptr;

    // 入口函数
    Qairt_Status_t (*getInterface)(uint64_t id, void** out) = nullptr;

    // 各 V1 接口表
    const QairtSystemLog_V1_t*        logIface       = nullptr;
    const QairtSystem_DlcV1_t*        dlcIface       = nullptr;
    const QairtSystemBuilder_V1_t*    builderIface   = nullptr;
    const Qairt_V1_t*                 qairtIface     = nullptr;
    const QairtSystem_Context_GraphInfoV1_t* graphInfoIface = nullptr;
    const QairtSystem_Context_TensorInfoV1_t* tensorInfoIface = nullptr;

    // 运行时句柄
    QairtSystemLog_Handle_t       logHandle     = nullptr;
    QairtSystemDlc_Handle_t      dlcHandle     = nullptr;
    QairtSystemBuilder_Handle_t  builderHandle = nullptr;
    Qairt_Handle_t               qairtHandle   = nullptr;

    ~Impl() { releaseAll(); }

    void releaseAll() {
#define SAFE_CALL(fn, ...) if ((fn) != nullptr) (fn)(__VA_ARGS__)
        if (qairtHandle && qairtIface && qairtIface->free) {
            qairtIface->free(qairtHandle);
            qairtHandle = nullptr;
        }
        if (builderHandle && builderIface && builderIface->free) {
            builderIface->free(builderHandle);
            builderHandle = nullptr;
        }
        if (dlcHandle && dlcIface && dlcIface->free) {
            dlcIface->free(dlcHandle);
            dlcHandle = nullptr;
        }
        if (logHandle && logIface && logIface->free) {
            logIface->free(logHandle);
            logHandle = nullptr;
        }
#undef SAFE_CALL
    }
};

// ---------------------------------------------------------------------------
// 构造 / 析构
// ---------------------------------------------------------------------------
QairtRuntime::QairtRuntime() : impl_(new Impl()) {}
QairtRuntime::~QairtRuntime() { cleanup(); delete impl_; }

// ---------------------------------------------------------------------------
// init: dlopen libQairtSystem.so 并加载所有需要的 V1 接口表
// ---------------------------------------------------------------------------
bool QairtRuntime::init(const std::string& libSearchPath) {
    if (ready_) return true;

    const char* kSysLib = "libQairtSystem.so";
    // 依次尝试：搜索路径 / 默认系统路径
    if (!libSearchPath.empty()) {
        std::string full = libSearchPath + "/" + kSysLib;
        impl_->sysLib = dlopen(full.c_str(), RTLD_NOW | RTLD_LOCAL);
    }
    if (!impl_->sysLib) {
        impl_->sysLib = dlopen(kSysLib, RTLD_NOW | RTLD_LOCAL);
    }
    if (!impl_->sysLib) {
        LOGE("dlopen(%s) 失败: %s", kSysLib, dlerror());
        return false;
    }

    impl_->getInterface = reinterpret_cast<decltype(impl_->getInterface)>(
        dlsym(impl_->sysLib, "QairtSystemInterface_getInterface"));
    if (!impl_->getInterface) {
        LOGE("未找到符号 QairtSystemInterface_getInterface: %s", dlerror());
        return false;
    }

    auto loadIface = [&](uint64_t id, const char* name, void** out) -> bool {
        Qairt_Status_t s = impl_->getInterface(id, out);
        if (s != QAIRT_SUCCESS || !*out) {
            LOGE("getInterface(%s, id=%llu) 失败: status=%llu", name,
                 (unsigned long long)id, (unsigned long long)s);
            return false;
        }
        return true;
    };

    // 用临时 void* 接收接口指针，再赋值给 const 指针成员（避免 reinterpret_cast 丢弃 const）
    auto loadAndAssign = [&](uint64_t id, const char* name, auto& memberPtr) -> bool {
        void* p = nullptr;
        if (!loadIface(id, name, &p)) return false;
        memberPtr = static_cast<std::remove_reference_t<decltype(memberPtr)>>(p);
        return true;
    };

    bool ok = true;
    ok &= loadAndAssign(QAIRT_SYSTEM_LOG_V1_ID,                "SystemLog",     impl_->logIface);
    ok &= loadAndAssign(QAIRT_SYSTEM_DLC_V1_ID,                "SystemDlc",      impl_->dlcIface);
    ok &= loadAndAssign(QAIRT_SYSTEM_BUILDER_V1_ID,            "SystemBuilder", impl_->builderIface);
    ok &= loadAndAssign(QAIRT_V1_ID,                            "Qairt",         impl_->qairtIface);
    ok &= loadAndAssign(QAIRT_SYSTEM_CONTEXT_GRAPH_INFO_V1_ID, "GraphInfo",     impl_->graphInfoIface);
    ok &= loadAndAssign(QAIRT_SYSTEM_CONTEXT_TENSOR_INFO_V1_ID,"TensorInfo",    impl_->tensorInfoIface);
    if (!ok) return false;

    LOGI("QAIRT 接口表加载完成");
    return true;
}

// ---------------------------------------------------------------------------
// loadDlc: 创建日志 → 从文件加载 DLC → 构建 builder → 生成运行时
//
// 注意：QAIRT 内部实现是 C++ 代码，可能在 build() 等关键步骤抛出
//       internalqairt::Exception（如 PLATFORM_NOT_SUPPORTED），由于我们
//       通过 C 函数指针调用，必须用 try/catch 包裹，否则会触发 SIGABRT
//       导致整个进程崩溃。
// ---------------------------------------------------------------------------
bool QairtRuntime::loadDlc(const std::string& dlcPath, BackendType backend) {
    if (!impl_->getInterface) {
        LOGE("Runtime 未初始化，请先调用 init()");
        return false;
    }

    try {

    // 1) 创建 SystemLog（callback 传 NULL，日志走 Android logcat 默认流）
    //    注意：QairtLog_Level_t 枚举从 1 (ERROR) 开始，0 不是合法值，
    //    传 0 会触发 QAIRT_LOG_ERROR_INITIALIZATION (11002)。
    if (impl_->logIface && impl_->logIface->create) {
        Qairt_Status_t s = impl_->logIface->create(
            /*callback*/ nullptr, /*maxLogLevel*/ QAIRT_LOG_LEVEL_ERROR,
            /*userData*/ nullptr, &impl_->logHandle);
        if (s != QAIRT_SUCCESS) {
            LOGE("QairtSystemLog_create 失败: %llu", (unsigned long long)s);
            return false;
        }
    }

    // 2) 从文件创建 DLC 句柄
    if (!impl_->dlcIface || !impl_->dlcIface->createFromFile) {
        LOGE("SystemDlc 接口不可用");
        return false;
    }
    Qairt_Status_t s = impl_->dlcIface->createFromFile(
        impl_->logHandle, dlcPath.c_str(), &impl_->dlcHandle);
    if (s != QAIRT_SUCCESS || !impl_->dlcHandle) {
        LOGE("QairtSystemDlc_createFromFile(%s) 失败: %llu", dlcPath.c_str(), (unsigned long long)s);
        return false;
    }
    LOGI("DLC 加载成功: %s", dlcPath.c_str());

    // 3) 创建 Builder 并配置
    if (!impl_->builderIface || !impl_->builderIface->create) {
        LOGE("SystemBuilder 接口不可用");
        return false;
    }
    s = impl_->builderIface->create(&impl_->builderHandle);
    if (s != QAIRT_SUCCESS || !impl_->builderHandle) {
        LOGE("QairtSystemBuilder_create 失败: %llu", (unsigned long long)s);
        return false;
    }

    if (impl_->builderIface->setDlc) {
        impl_->builderIface->setDlc(impl_->builderHandle, impl_->dlcHandle);
    }
    if (impl_->builderIface->setBackendType) {
        impl_->builderIface->setBackendType(impl_->builderHandle,
            static_cast<QairtSystem_BackendType_t>(backend));
    }
    if (impl_->builderIface->setLogLevel) {
        impl_->builderIface->setLogLevel(impl_->builderHandle, QAIRT_LOG_LEVEL_ERROR);
    }

    // 4) 构建运行时（最可能抛 C++ 异常的步骤：平台检测、后端初始化）
    if (!impl_->builderIface->build) {
        LOGE("SystemBuilder.build 不可用");
        return false;
    }
    s = impl_->builderIface->build(impl_->builderHandle, &impl_->qairtHandle);
    if (s != QAIRT_SUCCESS || !impl_->qairtHandle) {
        LOGE("QairtSystemBuilder_build 失败: %llu", (unsigned long long)s);
        if (impl_->qairtIface && impl_->qairtIface->getLastError) {
            const char* err = nullptr;
            impl_->qairtIface->getLastError(impl_->qairtHandle, &err);
            if (err) LOGE("QAIRT 错误详情: %s", err);
        }
        return false;
    }
    LOGI("QAIRT 运行时构建成功 (backend=%d)", static_cast<int>(backend));

    // 5) 查询图信息
    if (!queryGraphInfos()) {
        LOGW("图信息查询失败，但运行时已构建");
    }

    ready_ = true;
    return true;

    } catch (const std::exception& e) {
        // 捕获 internalqairt::Exception（继承自 std::exception）等 C++ 异常，
        // 避免 SIGABRT 导致 App 崩溃，转为友好错误返回。
        LOGE("loadDlc 捕获 C++ 异常: %s", e.what());
        // 清理已分配的资源
        if (impl_) impl_->releaseAll();
        return false;
    } catch (...) {
        LOGE("loadDlc 捕获未知 C++ 异常");
        if (impl_) impl_->releaseAll();
        return false;
    }
}

// 读取单个张量元信息
TensorInfo QairtRuntime::readTensorInfo(void* th) {
    TensorInfo ti;
    if (!impl_->tensorInfoIface || !th) return ti;
    // 将 void* 还原为 QAIRT 不透明句柄类型
    QairtSystem_Context_TensorInfoHandle_t tensorHandle =
        reinterpret_cast<QairtSystem_Context_TensorInfoHandle_t>(th);

    if (impl_->tensorInfoIface->getName) {
        const char* name = nullptr;
        impl_->tensorInfoIface->getName(tensorHandle, &name);
        if (name) ti.name = name;
    }
    if (impl_->tensorInfoIface->getId) {
        impl_->tensorInfoIface->getId(tensorHandle, &ti.id);
    }
    if (impl_->tensorInfoIface->getRank) {
        impl_->tensorInfoIface->getRank(tensorHandle, &ti.rank);
    }
    if (impl_->tensorInfoIface->getDimensions) {
        const uint32_t* dims = nullptr;
        impl_->tensorInfoIface->getDimensions(tensorHandle, &dims);
        if (dims && ti.rank > 0) {
            ti.dimensions.assign(dims, dims + ti.rank);
        }
    }
    if (impl_->tensorInfoIface->getDatatype) {
        Qairt_DataType_t dt{};
        impl_->tensorInfoIface->getDatatype(tensorHandle, &dt);
        ti.dataType = static_cast<int32_t>(dt);
    }

    ti.elementBytes = dataTypeToBytes(ti.dataType);
    ti.totalElements = 1;
    for (uint32_t d : ti.dimensions) ti.totalElements *= d;
    ti.totalBytes = ti.totalElements * ti.elementBytes;
    return ti;
}

// ---------------------------------------------------------------------------
// 查询所有图与张量元信息
// ---------------------------------------------------------------------------
bool QairtRuntime::queryGraphInfos() {
    graphs_.clear();
    if (!impl_->qairtIface || !impl_->qairtIface->getNumGraphInfos) return false;

    uint32_t numGraphs = 0;
    Qairt_Status_t s = impl_->qairtIface->getNumGraphInfos(impl_->qairtHandle, &numGraphs);
    if (s != QAIRT_SUCCESS) return false;
    LOGI("模型包含 %u 个图", numGraphs);

    for (uint32_t i = 0; i < numGraphs; ++i) {
        QairtSystem_Context_GraphInfoHandle_t gh = nullptr;
        s = impl_->qairtIface->getGraphInfoAt(impl_->qairtHandle, i, &gh);
        if (s != QAIRT_SUCCESS || !gh) continue;

        GraphInfo gi;

        if (impl_->graphInfoIface && impl_->graphInfoIface->getName) {
            const char* name = nullptr;
            impl_->graphInfoIface->getName(gh, &name);
            if (name) gi.name = name;
        }

        if (impl_->graphInfoIface && impl_->graphInfoIface->getNumInputs) {
            uint32_t nIn = 0;
            impl_->graphInfoIface->getNumInputs(gh, &nIn);
            for (uint32_t j = 0; j < nIn; ++j) {
                QairtSystem_Context_TensorInfoHandle_t th = nullptr;
                impl_->graphInfoIface->getInputAt(gh, j, &th);
                if (th) gi.inputs.push_back(readTensorInfo(th));
            }
        }

        if (impl_->graphInfoIface && impl_->graphInfoIface->getNumOutputs) {
            uint32_t nOut = 0;
            impl_->graphInfoIface->getNumOutputs(gh, &nOut);
            for (uint32_t j = 0; j < nOut; ++j) {
                QairtSystem_Context_TensorInfoHandle_t th = nullptr;
                impl_->graphInfoIface->getOutputAt(gh, j, &th);
                if (th) gi.outputs.push_back(readTensorInfo(th));
            }
        }

        graphs_.push_back(std::move(gi));
    }
    return true;
}

// ---------------------------------------------------------------------------
// execute: 对指定图执行一次推理
// ---------------------------------------------------------------------------
InferenceResult QairtRuntime::execute(const std::string& graphName,
                                       const std::vector<std::vector<uint8_t>>& inputs) {
    InferenceResult result;
    result.graphName = graphName;
    if (!ready_ || !impl_->qairtIface) {
        result.error = "Runtime 未就绪";
        return result;
    }

    try {

    const GraphInfo* gi = nullptr;
    for (const auto& g : graphs_) {
        if (g.name == graphName) { gi = &g; break; }
    }
    if (!gi) {
        result.error = "未找到图: " + graphName;
        return result;
    }
    if (inputs.size() != gi->inputs.size()) {
        std::ostringstream oss;
        oss << "输入数量不匹配: 期望 " << gi->inputs.size() << " 实际 " << inputs.size();
        result.error = oss.str();
        return result;
    }

    // 重新获取图句柄
    QairtSystem_Context_GraphInfoHandle_t gh = nullptr;
    impl_->qairtIface->getGraphInfoAt(impl_->qairtHandle, 0, &gh);
    if (!gh) {
        result.error = "无法获取图句柄";
        return result;
    }

    // 准备输入
    std::vector<QairtSystem_Context_TensorInfoHandle_t> inHandles;
    std::vector<uint8_t*> inBufs;
    inHandles.reserve(gi->inputs.size());
    inBufs.reserve(gi->inputs.size());
    for (uint32_t j = 0; j < gi->inputs.size(); ++j) {
        QairtSystem_Context_TensorInfoHandle_t th = nullptr;
        impl_->graphInfoIface->getInputAt(gh, j, &th);
        inHandles.push_back(th);
        inBufs.push_back(const_cast<uint8_t*>(inputs[j].data()));
    }

    // 准备输出
    std::vector<QairtSystem_Context_TensorInfoHandle_t> outHandles;
    std::vector<uint8_t*> outBufs;
    std::vector<std::vector<uint8_t>> outStorage;
    outHandles.reserve(gi->outputs.size());
    outBufs.reserve(gi->outputs.size());
    outStorage.resize(gi->outputs.size());
    for (uint32_t j = 0; j < gi->outputs.size(); ++j) {
        QairtSystem_Context_TensorInfoHandle_t th = nullptr;
        impl_->graphInfoIface->getOutputAt(gh, j, &th);
        outHandles.push_back(th);
        outStorage[j].assign(gi->outputs[j].totalBytes, 0);
        outBufs.push_back(outStorage[j].data());
    }

    // 执行推理并计时
    auto t0 = std::chrono::steady_clock::now();
    Qairt_Status_t s = impl_->qairtIface->executeGraph(
        impl_->qairtHandle,
        graphName.c_str(),
        inHandles.data(), inBufs.data(), static_cast<uint32_t>(inHandles.size()),
        outHandles.data(), outBufs.data(), static_cast<uint32_t>(outHandles.size()));
    auto t1 = std::chrono::steady_clock::now();
    result.elapsedMs = std::chrono::duration<double, std::milli>(t1 - t0).count();

    if (s != QAIRT_SUCCESS) {
        std::ostringstream oss;
        oss << "executeGraph 失败: status=0x" << std::hex << s;
        if (impl_->qairtIface->getLastError) {
            const char* err = nullptr;
            impl_->qairtIface->getLastError(impl_->qairtHandle, &err);
            if (err) oss << " (" << err << ")";
        }
        result.error = oss.str();
        return result;
    }

    result.outputs = std::move(outStorage);
    LOGI("推理成功: %s, 耗时 %.3f ms", graphName.c_str(), result.elapsedMs);
    return result;

    } catch (const std::exception& e) {
        result.error = std::string("execute 捕获 C++ 异常: ") + e.what();
        LOGE("%s", result.error.c_str());
        return result;
    } catch (...) {
        result.error = "execute 捕获未知 C++ 异常";
        LOGE("%s", result.error.c_str());
        return result;
    }
}

// ---------------------------------------------------------------------------
// cleanup: 释放所有运行时资源
// ---------------------------------------------------------------------------
void QairtRuntime::cleanup() {
    if (impl_) impl_->releaseAll();
    ready_ = false;
    graphs_.clear();
    backendBuildId_.clear();
}

}  // namespace qnn_demo
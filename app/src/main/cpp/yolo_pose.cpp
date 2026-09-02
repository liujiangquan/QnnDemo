/*
 * Copyright (c) 2026 智跃千里公司. All rights reserved.
 * File:        yolo_pose.cpp
 * Description: QAIRT HTP session, letterbox, RGB quantize, YOLO26-pose decode.
 *              Ported from the working QnnYolo project.
 * Version:     v1.0
 * Created:     2026-09-01
 * Author:      g-liujiangquan
 */

#include "yolo_pose.h"

#include <android/log.h>
#include <dlfcn.h>
#include <sys/system_properties.h>
#include <algorithm>
#include <cmath>
#include <cstdarg>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <unistd.h>

#include "QnnBackend.h"
#include "QnnCommon.h"
#include "QnnContext.h"
#include "QnnDevice.h"
#include "QnnGraph.h"
#include "QnnLog.h"
#include "QnnTypes.h"
#include "System/QnnSystemContext.h"
#include "HTP/QnnHtpDevice.h"
#include "HTP/QnnHtpDeviceConfigShared.h"

namespace {

constexpr char kTag[] = "YoloPose";
constexpr uint32_t kSm8845SocModel = 97U;
constexpr float kDefaultOutputScale = 0.0124205472F;
constexpr float kDefaultOutputOffset = -1149.F;
constexpr float kLetterboxPad = 114.F / 255.F;

void logAndroid(int prio, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    __android_log_vprint(prio, kTag, fmt, args);
    va_end(args);
}

void qnnLogCallback(const char* fmt, QnnLog_Level_t level, uint64_t timestamp, va_list args) {
    (void)timestamp;
    int prio = ANDROID_LOG_INFO;
    if (level == QNN_LOG_LEVEL_ERROR) {
        prio = ANDROID_LOG_ERROR;
    } else if (level == QNN_LOG_LEVEL_WARN) {
        prio = ANDROID_LOG_WARN;
    } else if (level == QNN_LOG_LEVEL_VERBOSE || level == QNN_LOG_LEVEL_DEBUG) {
        prio = ANDROID_LOG_DEBUG;
    }
    __android_log_vprint(prio, kTag, fmt, args);
}

void* openLib(const char* nativeLibDir, const char* name) {
    std::string local;
    if (nativeLibDir != nullptr) {
        local = std::string(nativeLibDir) + "/" + name;
    }
    std::string vendor = std::string("/vendor/lib64/") + name;
    const char* candidates[] = {
            (!local.empty() && access(local.c_str(), R_OK) == 0) ? local.c_str() : nullptr,
            name,
            vendor.c_str(),
    };
    for (const char* path : candidates) {
        if (path == nullptr || path[0] == '\0') {
            continue;
        }
        void* handle = dlopen(path, RTLD_NOW | RTLD_GLOBAL);
        if (handle != nullptr) {
            logAndroid(ANDROID_LOG_INFO, "loaded %s", path);
            return handle;
        }
        logAndroid(ANDROID_LOG_WARN, "dlopen %s failed: %s", path, dlerror());
    }
    return nullptr;
}

bool deviceIsSm8845() {
    char soc[PROP_VALUE_MAX] = {};
    __system_property_get("ro.soc.model", soc);
    return std::strcmp(soc, "SM8845") == 0;
}

bool contextTargetsSm8850(uint32_t socModel, const char* socVersion) {
    if (socModel == static_cast<uint32_t>(QNN_SOC_MODEL_SM8850)) {
        return true;
    }
    if (socVersion != nullptr && std::strstr(socVersion, "8850") != nullptr) {
        return true;
    }
    return false;
}

size_t dtypeBytes(Qnn_DataType_t type) {
    switch (type) {
        case QNN_DATATYPE_FLOAT_32:
        case QNN_DATATYPE_UFIXED_POINT_32:
        case QNN_DATATYPE_UINT_32:
        case QNN_DATATYPE_INT_32:
            return 4;
        case QNN_DATATYPE_FLOAT_16:
        case QNN_DATATYPE_UFIXED_POINT_16:
        case QNN_DATATYPE_UINT_16:
        case QNN_DATATYPE_INT_16:
            return 2;
        default:
            return 1;
    }
}

float iouXyxy(const float* a, const float* b) {
    const float x1 = std::max(a[0], b[0]);
    const float y1 = std::max(a[1], b[1]);
    const float x2 = std::min(a[2], b[2]);
    const float y2 = std::min(a[3], b[3]);
    const float w = std::max(0.F, x2 - x1);
    const float h = std::max(0.F, y2 - y1);
    const float inter = w * h;
    const float areaA = std::max(0.F, a[2] - a[0]) * std::max(0.F, a[3] - a[1]);
    const float areaB = std::max(0.F, b[2] - b[0]) * std::max(0.F, b[3] - b[1]);
    const float denom = areaA + areaB - inter;
    if (denom <= 0.F) {
        return 0.F;
    }
    return inter / denom;
}

}  // namespace

YoloPoseSession::~YoloPoseSession() {
    release();
}

std::string YoloPoseSession::init(
        const char* nativeLibDir,
        const uint8_t* contextBin,
        size_t contextBytes) {
    release();
    if (nativeLibDir == nullptr || contextBin == nullptr || contextBytes == 0) {
        return "invalid init arguments";
    }
    std::string adsp = nativeLibDir;
    adsp += ";/vendor/lib/rfsa/adsp;/vendor/dsp/cdsp;/vendor/dsp/adsp;/system/lib/rfsa/adsp;/dsp";
    setenv("ADSP_LIBRARY_PATH", adsp.c_str(), 1);
    if (!loadInterfaces(nativeLibDir)) {
        return "failed to load QNN HTP/System interfaces";
    }
    if (!createSession(nativeLibDir, contextBin, contextBytes)) {
        if (mSocMismatch) {
            return "SOC_MISMATCH";
        }
        return "failed to create QNN HTP session from context binary";
    }
    if (!setupIoTensors()) {
        return "failed to allocate graph IO tensors";
    }
    mReady = true;
    logAndroid(ANDROID_LOG_INFO, "QAIRT pose session ready, graph=%s", mGraphName.c_str());
    return "";
}

bool YoloPoseSession::loadInterfaces(const char* nativeLibDir) {
    mSystemHandle = openLib(nativeLibDir, "libQnnSystem.so");
    if (mSystemHandle == nullptr) {
        return false;
    }
    auto getSysProviders =
            reinterpret_cast<Qnn_ErrorHandle_t (*)(const QnnSystemInterface_t***, uint32_t*)>(
                    dlsym(mSystemHandle, "QnnSystemInterface_getProviders"));
    if (getSysProviders == nullptr) {
        logAndroid(ANDROID_LOG_ERROR, "missing QNN System getProviders");
        return false;
    }
    const QnnSystemInterface_t** sysProviders = nullptr;
    uint32_t numSys = 0;
    if (getSysProviders(&sysProviders, &numSys) != QNN_SUCCESS || sysProviders == nullptr
            || numSys == 0) {
        return false;
    }
    mSystem = sysProviders[0]->QNN_SYSTEM_INTERFACE_VER_NAME;
    return true;
}

bool YoloPoseSession::loadHtpInterface(const char* nativeLibDir) {
    dlopen("libcdsprpc.so", RTLD_NOW | RTLD_GLOBAL);
    openLib(nativeLibDir, "libQnnHtpV81Stub.so");
    mHtpHandle = openLib(nativeLibDir, "libQnnHtp.so");
    if (mHtpHandle == nullptr) {
        return false;
    }
    auto getProviders = reinterpret_cast<Qnn_ErrorHandle_t (*)(const QnnInterface_t***, uint32_t*)>(
            dlsym(mHtpHandle, "QnnInterface_getProviders"));
    if (getProviders == nullptr) {
        logAndroid(ANDROID_LOG_ERROR, "missing QNN HTP getProviders");
        return false;
    }
    const QnnInterface_t** providers = nullptr;
    uint32_t numProviders = 0;
    if (getProviders(&providers, &numProviders) != QNN_SUCCESS || providers == nullptr
            || numProviders == 0) {
        return false;
    }
    bool found = false;
    for (uint32_t i = 0; i < numProviders; ++i) {
        if (providers[i]->apiVersion.coreApiVersion.major == QNN_API_VERSION_MAJOR
                && providers[i]->apiVersion.coreApiVersion.minor >= QNN_API_VERSION_MINOR) {
            mQnn = providers[i]->QNN_INTERFACE_VER_NAME;
            found = true;
            break;
        }
    }
    if (!found) {
        mQnn = providers[0]->QNN_INTERFACE_VER_NAME;
    }
    return true;
}

bool YoloPoseSession::createSession(
        const char* nativeLibDir,
        const uint8_t* contextBin,
        size_t contextBytes) {
    QnnSystemContext_Handle_t sysCtx = nullptr;
    if (mSystem.systemContextCreate(&sysCtx) != QNN_SUCCESS) {
        return false;
    }
    const QnnSystemContext_BinaryInfo_t* binaryInfo = nullptr;
    Qnn_ContextBinarySize_t infoSize = 0;
    if (mSystem.systemContextGetBinaryInfo(
                sysCtx,
                const_cast<uint8_t*>(contextBin),
                contextBytes,
                &binaryInfo,
                &infoSize) != QNN_SUCCESS
            || binaryInfo == nullptr) {
        mSystem.systemContextFree(sysCtx);
        return false;
    }
    uint32_t binSoc = 0;
    const char* socVersion = nullptr;
    const char* buildId = nullptr;
    const QnnSystemContext_GraphInfo_t* graphs = nullptr;
    uint32_t numGraphs = 0;
    if (binaryInfo->version == QNN_SYSTEM_CONTEXT_BINARY_INFO_VERSION_1) {
        graphs = binaryInfo->contextBinaryInfoV1.graphs;
        numGraphs = binaryInfo->contextBinaryInfoV1.numGraphs;
        buildId = binaryInfo->contextBinaryInfoV1.buildId;
        socVersion = binaryInfo->contextBinaryInfoV1.socVersion;
    } else if (binaryInfo->version == QNN_SYSTEM_CONTEXT_BINARY_INFO_VERSION_2) {
        graphs = binaryInfo->contextBinaryInfoV2.graphs;
        numGraphs = binaryInfo->contextBinaryInfoV2.numGraphs;
        socVersion = binaryInfo->contextBinaryInfoV2.socVersion;
        buildId = binaryInfo->contextBinaryInfoV2.buildId;
    } else if (binaryInfo->version == QNN_SYSTEM_CONTEXT_BINARY_INFO_VERSION_3) {
        graphs = binaryInfo->contextBinaryInfoV3.graphs;
        numGraphs = binaryInfo->contextBinaryInfoV3.numGraphs;
        socVersion = binaryInfo->contextBinaryInfoV3.socVersion;
        buildId = binaryInfo->contextBinaryInfoV3.buildId;
        binSoc = binaryInfo->contextBinaryInfoV3.socModel;
    }
    logAndroid(
            ANDROID_LOG_INFO,
            "context meta buildId=%s socModel=%u socVersion=%s",
            buildId != nullptr ? buildId : "?",
            binSoc,
            socVersion != nullptr ? socVersion : "?");
    if (deviceIsSm8845()
            && (contextTargetsSm8850(binSoc, socVersion) || binSoc != kSm8845SocModel)) {
        mSystem.systemContextFree(sysCtx);
        mSocMismatch = true;
        logAndroid(
                ANDROID_LOG_ERROR,
                "refusing HTP context on SM8845 (socModel=%u). CDSP dump risk",
                binSoc);
        return false;
    }
    if (graphs == nullptr || numGraphs == 0) {
        mSystem.systemContextFree(sysCtx);
        return false;
    }
    const QnnSystemContext_GraphInfo_t& graphInfo = graphs[0];
    const char* graphName = nullptr;
    const Qnn_Tensor_t* graphInputs = nullptr;
    const Qnn_Tensor_t* graphOutputs = nullptr;
    if (graphInfo.version == QNN_SYSTEM_CONTEXT_GRAPH_INFO_VERSION_1) {
        graphName = graphInfo.graphInfoV1.graphName;
        mNumInputs = graphInfo.graphInfoV1.numGraphInputs;
        mNumOutputs = graphInfo.graphInfoV1.numGraphOutputs;
        graphInputs = graphInfo.graphInfoV1.graphInputs;
        graphOutputs = graphInfo.graphInfoV1.graphOutputs;
    } else if (graphInfo.version == QNN_SYSTEM_CONTEXT_GRAPH_INFO_VERSION_2) {
        graphName = graphInfo.graphInfoV2.graphName;
        mNumInputs = graphInfo.graphInfoV2.numGraphInputs;
        mNumOutputs = graphInfo.graphInfoV2.numGraphOutputs;
        graphInputs = graphInfo.graphInfoV2.graphInputs;
        graphOutputs = graphInfo.graphInfoV2.graphOutputs;
    } else {
        graphName = graphInfo.graphInfoV3.graphName;
        mNumInputs = graphInfo.graphInfoV3.numGraphInputs;
        mNumOutputs = graphInfo.graphInfoV3.numGraphOutputs;
        graphInputs = graphInfo.graphInfoV3.graphInputs;
        graphOutputs = graphInfo.graphInfoV3.graphOutputs;
    }
    if (mNumInputs != 1 || mNumOutputs != 1 || graphInputs == nullptr || graphOutputs == nullptr) {
        mSystem.systemContextFree(sysCtx);
        return false;
    }
    mGraphName = graphName != nullptr ? graphName : "yolo26_pose";
    if (!copyTensorMeta(&mGraphInputs[0], &graphInputs[0])
            || !copyTensorMeta(&mGraphOutputs[0], &graphOutputs[0])) {
        mSystem.systemContextFree(sysCtx);
        return false;
    }
    mSystem.systemContextFree(sysCtx);
    if (!loadHtpInterface(nativeLibDir)) {
        logAndroid(ANDROID_LOG_ERROR, "failed to load vendor/libQnnHtp.so");
        return false;
    }
    if (mQnn.logCreate != nullptr) {
        mQnn.logCreate(qnnLogCallback, QNN_LOG_LEVEL_WARN, &mLogHandle);
    }
    if (mQnn.backendCreate(mLogHandle, nullptr, &mBackend) != QNN_BACKEND_NO_ERROR) {
        logAndroid(ANDROID_LOG_ERROR, "backendCreate failed");
        return false;
    }
    if (mQnn.deviceCreate != nullptr) {
        QnnHtpDevice_CustomConfig_t socCfg{};
        socCfg.option = QNN_HTP_DEVICE_CONFIG_OPTION_SOC;
        socCfg.socModel = 685U;
        QnnHtpDevice_CustomConfig_t pdCfg{};
        pdCfg.option = QNN_HTP_DEVICE_CONFIG_OPTION_SIGNEDPD;
        pdCfg.useSignedProcessDomain.deviceId = 0;
        pdCfg.useSignedProcessDomain.useSignedProcessDomain = false;
        QnnDevice_Config_t socDevCfg{};
        socDevCfg.option = QNN_DEVICE_CONFIG_OPTION_CUSTOM;
        socDevCfg.customConfig = &socCfg;
        QnnDevice_Config_t pdDevCfg{};
        pdDevCfg.option = QNN_DEVICE_CONFIG_OPTION_CUSTOM;
        pdDevCfg.customConfig = &pdCfg;
        const QnnDevice_Config_t* deviceCfgs[] = {&socDevCfg, &pdDevCfg, nullptr};
        if (mQnn.deviceCreate(mLogHandle, deviceCfgs, &mDevice) != QNN_SUCCESS) {
            logAndroid(ANDROID_LOG_WARN, "deviceCreate failed, continuing without device handle");
            mDevice = nullptr;
        }
    }
    if (mQnn.contextCreateFromBinary(
                mBackend,
                mDevice,
                nullptr,
                contextBin,
                contextBytes,
                &mContext,
                nullptr) != QNN_SUCCESS) {
        logAndroid(ANDROID_LOG_ERROR, "contextCreateFromBinary failed");
        return false;
    }
    if (mQnn.graphRetrieve(mContext, mGraphName.c_str(), &mGraph) != QNN_SUCCESS) {
        logAndroid(ANDROID_LOG_ERROR, "graphRetrieve failed for %s", mGraphName.c_str());
        return false;
    }
    return true;
}

bool YoloPoseSession::copyTensorMeta(Qnn_Tensor_t* dst, const Qnn_Tensor_t* src) {
    dst->version = src->version;
    const char* name = QNN_TENSOR_GET_NAME(src);
    if (name != nullptr) {
        const size_t len = std::strlen(name);
        char* copy = static_cast<char*>(std::malloc(len + 1));
        if (copy == nullptr) {
            return false;
        }
        std::memcpy(copy, name, len + 1);
        QNN_TENSOR_SET_NAME(dst, copy);
    } else {
        QNN_TENSOR_SET_NAME(dst, nullptr);
    }
    QNN_TENSOR_SET_ID(dst, QNN_TENSOR_GET_ID(src));
    QNN_TENSOR_SET_TYPE(dst, QNN_TENSOR_GET_TYPE(src));
    QNN_TENSOR_SET_DATA_FORMAT(dst, QNN_TENSOR_GET_DATA_FORMAT(src));
    QNN_TENSOR_SET_DATA_TYPE(dst, QNN_TENSOR_GET_DATA_TYPE(src));
    QNN_TENSOR_SET_QUANT_PARAMS(dst, QNN_TENSOR_GET_QUANT_PARAMS(src));
    const uint32_t rank = QNN_TENSOR_GET_RANK(src);
    QNN_TENSOR_SET_RANK(dst, rank);
    if (rank > 0) {
        auto* dims = static_cast<uint32_t*>(std::malloc(rank * sizeof(uint32_t)));
        if (dims == nullptr) {
            return false;
        }
        std::memcpy(dims, QNN_TENSOR_GET_DIMENSIONS(src), rank * sizeof(uint32_t));
        QNN_TENSOR_SET_DIMENSIONS(dst, dims);
    }
    QNN_TENSOR_SET_IS_DYNAMIC_DIMENSIONS(dst, nullptr);
    QNN_TENSOR_SET_SPARSE_PARAMS(dst, QNN_TENSOR_GET_SPARSE_PARAMS(src));
    return true;
}

size_t YoloPoseSession::tensorBytes(const Qnn_Tensor_t& tensor) const {
    const uint32_t rank = QNN_TENSOR_GET_RANK(tensor);
    uint32_t* dims = QNN_TENSOR_GET_DIMENSIONS(tensor);
    size_t count = 1;
    for (uint32_t i = 0; i < rank; ++i) {
        count *= dims[i];
    }
    return count * dtypeBytes(QNN_TENSOR_GET_DATA_TYPE(tensor));
}

bool YoloPoseSession::setupIoTensors() {
    if (!copyTensorMeta(&mInput, &mGraphInputs[0]) || !copyTensorMeta(&mOutput, &mGraphOutputs[0])) {
        return false;
    }
    mInputBuf.resize(tensorBytes(mInput));
    mOutputBuf.resize(tensorBytes(mOutput));
    QNN_TENSOR_SET_MEM_TYPE(&mInput, QNN_TENSORMEMTYPE_RAW);
    QNN_TENSOR_SET_MEM_TYPE(&mOutput, QNN_TENSORMEMTYPE_RAW);
    Qnn_ClientBuffer_t inBuf = QNN_CLIENT_BUFFER_INIT;
    inBuf.data = mInputBuf.data();
    inBuf.dataSize = mInputBuf.size();
    QNN_TENSOR_SET_CLIENT_BUF(&mInput, inBuf);
    Qnn_ClientBuffer_t outBuf = QNN_CLIENT_BUFFER_INIT;
    outBuf.data = mOutputBuf.data();
    outBuf.dataSize = mOutputBuf.size();
    QNN_TENSOR_SET_CLIENT_BUF(&mOutput, outBuf);
    return true;
}

void YoloPoseSession::letterboxRgbToInput(
        int srcWidth,
        int srcHeight,
        const uint8_t* rgbPixels) {
    mSrcWidth = srcWidth;
    mSrcHeight = srcHeight;
    const int originX = 0;
    const int originY = 0;
    mScale = std::min(
            static_cast<float>(kInputSize) / static_cast<float>(srcWidth),
            static_cast<float>(kInputSize) / static_cast<float>(srcHeight));
    const float newW = static_cast<float>(srcWidth) * mScale;
    const float newH = static_cast<float>(srcHeight) * mScale;
    mPadX = (static_cast<float>(kInputSize) - newW) * 0.5F;
    mPadY = (static_cast<float>(kInputSize) - newH) * 0.5F;
    auto* dst = reinterpret_cast<uint16_t*>(mInputBuf.data());
    const Qnn_QuantizeParams_t inQ = QNN_TENSOR_GET_QUANT_PARAMS(mInput);
    float inScale = 1.F / 65535.F;
    float inOffset = 0.F;
    if (inQ.quantizationEncoding == QNN_QUANTIZATION_ENCODING_SCALE_OFFSET) {
        inScale = inQ.scaleOffsetEncoding.scale;
        inOffset = static_cast<float>(inQ.scaleOffsetEncoding.offset);
    }
    auto quantize01 = [inScale, inOffset](float value) -> uint16_t {
        const float q = value / inScale - inOffset;
        const int iq = static_cast<int>(std::nearbyint(q));
        if (iq < 0) {
            return 0;
        }
        if (iq > 65535) {
            return 65535;
        }
        return static_cast<uint16_t>(iq);
    };
    const uint16_t padQ = quantize01(kLetterboxPad);
    std::fill(dst, dst + kInputSize * kInputSize * 3, padQ);
    const int channels = 3;
    for (int dy = 0; dy < kInputSize; ++dy) {
        const float syf = (static_cast<float>(dy) + 0.5F - mPadY) / mScale - 0.5F;
        int sy = static_cast<int>(std::floor(syf));
        if (sy < 0 || sy >= srcHeight) {
            continue;
        }
        for (int dx = 0; dx < kInputSize; ++dx) {
            const float sxf = (static_cast<float>(dx) + 0.5F - mPadX) / mScale - 0.5F;
            int sx = static_cast<int>(std::floor(sxf));
            if (sx < 0 || sx >= srcWidth) {
                continue;
            }
            const uint8_t r = rgbPixels[(sy * srcWidth + sx) * 4 + 0];
            const uint8_t g = rgbPixels[(sy * srcWidth + sx) * 4 + 1];
            const uint8_t b = rgbPixels[(sy * srcWidth + sx) * 4 + 2];
            const int base = (dy * kInputSize + dx) * channels;
            dst[base + 0] = quantize01(r / 255.F);
            dst[base + 1] = quantize01(g / 255.F);
            dst[base + 2] = quantize01(b / 255.F);
        }
    }
}

void YoloPoseSession::letterboxYuvToInput(
        int srcWidth,
        int srcHeight,
        int cropLeft,
        int cropTop,
        int yRowStride,
        int uRowStride,
        int vRowStride,
        int uPixelStride,
        int vPixelStride,
        const uint8_t* yPlane,
        const uint8_t* uPlane,
        const uint8_t* vPlane) {
    mSrcWidth = srcWidth;
    mSrcHeight = srcHeight;
    const int originX = cropLeft & ~1;
    const int originY = cropTop & ~1;
    mScale = std::min(
            static_cast<float>(kInputSize) / static_cast<float>(srcWidth),
            static_cast<float>(kInputSize) / static_cast<float>(srcHeight));
    const float newW = static_cast<float>(srcWidth) * mScale;
    const float newH = static_cast<float>(srcHeight) * mScale;
    mPadX = (static_cast<float>(kInputSize) - newW) * 0.5F;
    mPadY = (static_cast<float>(kInputSize) - newH) * 0.5F;
    auto* dst = reinterpret_cast<uint16_t*>(mInputBuf.data());
    const Qnn_QuantizeParams_t inQ = QNN_TENSOR_GET_QUANT_PARAMS(mInput);
    float inScale = 1.F / 65535.F;
    float inOffset = 0.F;
    if (inQ.quantizationEncoding == QNN_QUANTIZATION_ENCODING_SCALE_OFFSET) {
        inScale = inQ.scaleOffsetEncoding.scale;
        inOffset = static_cast<float>(inQ.scaleOffsetEncoding.offset);
    }
    auto quantize01 = [inScale, inOffset](float value) -> uint16_t {
        const float q = value / inScale - inOffset;
        const int iq = static_cast<int>(std::nearbyint(q));
        if (iq < 0) {
            return 0;
        }
        if (iq > 65535) {
            return 65535;
        }
        return static_cast<uint16_t>(iq);
    };
    const uint16_t padQ = quantize01(kLetterboxPad);
    std::fill(dst, dst + kInputSize * kInputSize * 3, padQ);
    for (int dy = 0; dy < kInputSize; ++dy) {
        const float syf = (static_cast<float>(dy) + 0.5F - mPadY) / mScale - 0.5F;
        int sy = static_cast<int>(std::floor(syf));
        if (sy < 0 || sy >= srcHeight) {
            continue;
        }
        const int srcY = originY + sy;
        const uint8_t* yRow = yPlane + srcY * yRowStride;
        const uint8_t* uRow = uPlane + (srcY / 2) * uRowStride;
        const uint8_t* vRow = vPlane + (srcY / 2) * vRowStride;
        for (int dx = 0; dx < kInputSize; ++dx) {
            const float sxf = (static_cast<float>(dx) + 0.5F - mPadX) / mScale - 0.5F;
            int sx = static_cast<int>(std::floor(sxf));
            if (sx < 0 || sx >= srcWidth) {
                continue;
            }
            const int srcX = originX + sx;
            const uint8_t y = yRow[srcX];
            const int uvIndex = (srcX / 2) * uPixelStride;
            const int vvIndex = (srcX / 2) * vPixelStride;
            float rgb[3];
            const float yf = static_cast<float>(y);
            const float uf = static_cast<float>(uRow[uvIndex]) - 128.F;
            const float vf = static_cast<float>(vRow[vvIndex]) - 128.F;
            rgb[0] = std::clamp((yf + 1.402F * vf) / 255.F, 0.F, 1.F);
            rgb[1] = std::clamp((yf - 0.344F * uf - 0.714F * vf) / 255.F, 0.F, 1.F);
            rgb[2] = std::clamp((yf + 1.772F * uf) / 255.F, 0.F, 1.F);
            const int base = (dy * kInputSize + dx) * 3;
            dst[base + 0] = quantize01(rgb[0]);
            dst[base + 1] = quantize01(rgb[1]);
            dst[base + 2] = quantize01(rgb[2]);
        }
    }
}

std::vector<float> YoloPoseSession::inferYuv(
        int srcWidth,
        int srcHeight,
        int cropLeft,
        int cropTop,
        int yRowStride,
        int uRowStride,
        int vRowStride,
        int uPixelStride,
        int vPixelStride,
        const uint8_t* yPlane,
        const uint8_t* uPlane,
        const uint8_t* vPlane) {
    if (!mReady) {
        logAndroid(ANDROID_LOG_ERROR, "session not ready");
        return {};
    }
    if (yPlane == nullptr || uPlane == nullptr || vPlane == nullptr) {
        logAndroid(ANDROID_LOG_ERROR, "null yuv planes");
        return {};
    }
    if (srcWidth <= 0 || srcHeight <= 0 || cropLeft < 0 || cropTop < 0) {
        logAndroid(ANDROID_LOG_ERROR, "invalid crop %dx%d @(%d,%d)",
                   srcWidth, srcHeight, cropLeft, cropTop);
        return {};
    }
    letterboxYuvToInput(
            srcWidth, srcHeight, cropLeft, cropTop,
            yRowStride, uRowStride, vRowStride, uPixelStride, vPixelStride,
            yPlane, uPlane, vPlane);
    const Qnn_ErrorHandle_t status = mQnn.graphExecute(
            mGraph, &mInput, 1, &mOutput, 1, nullptr, nullptr);
    if (status != QNN_GRAPH_NO_ERROR) {
        logAndroid(ANDROID_LOG_ERROR, "graphExecute failed: %llx",
                   (unsigned long long)status);
        return {};
    }
    std::vector<float> detections;
    decodePose(&detections);
    return detections;
}

void YoloPoseSession::decodePose(std::vector<float>* detections) {
    detections->clear();
    const Qnn_QuantizeParams_t q = QNN_TENSOR_GET_QUANT_PARAMS(mOutput);
    float scale = kDefaultOutputScale;
    float offset = kDefaultOutputOffset;
    if (q.quantizationEncoding == QNN_QUANTIZATION_ENCODING_SCALE_OFFSET) {
        scale = q.scaleOffsetEncoding.scale;
        offset = static_cast<float>(q.scaleOffsetEncoding.offset);
    }
    const auto* raw = reinterpret_cast<const uint16_t*>(mOutputBuf.data());
    struct Cand {
        float conf;
        float box[4];
        float kpts[kKeypoints * 3];
    };
    std::vector<Cand> cands;
    cands.reserve(64);
    auto at = [&](int ch, int anchor) -> float {
        const float qv = static_cast<float>(raw[ch * kAnchors + anchor]);
        return (qv + offset) * scale;
    };
    float maxConf = 0.F;
    for (int a = 0; a < kAnchors; ++a) {
        const float conf = at(4, a);
        if (conf > maxConf) {
            maxConf = conf;
        }
        if (conf < kConfThreshold) {
            continue;
        }
        const float cx = at(0, a);
        const float cy = at(1, a);
        const float w = at(2, a);
        const float h = at(3, a);
        Cand cand{};
        cand.conf = conf;
        cand.box[0] = (cx - w * 0.5F - mPadX) / mScale;
        cand.box[1] = (cy - h * 0.5F - mPadY) / mScale;
        cand.box[2] = (cx + w * 0.5F - mPadX) / mScale;
        cand.box[3] = (cy + h * 0.5F - mPadY) / mScale;
        for (int k = 0; k < kKeypoints; ++k) {
            const float kx = (at(5 + k * 3, a) - mPadX) / mScale;
            const float ky = (at(5 + k * 3 + 1, a) - mPadY) / mScale;
            const float kc = std::clamp(at(5 + k * 3 + 2, a), 0.F, 1.F);
            cand.kpts[k * 3] = kx;
            cand.kpts[k * 3 + 1] = ky;
            cand.kpts[k * 3 + 2] = kc;
        }
        cands.push_back(cand);
    }
    std::sort(cands.begin(), cands.end(), [](const Cand& a, const Cand& b) {
        return a.conf > b.conf;
    });
    std::vector<char> suppressed(cands.size(), 0);
    detections->push_back(0.F);
    int kept = 0;
    for (size_t i = 0; i < cands.size() && kept < kMaxDetections; ++i) {
        if (suppressed[i] != 0) {
            continue;
        }
        for (size_t j = i + 1; j < cands.size(); ++j) {
            if (suppressed[j] == 0 && iouXyxy(cands[i].box, cands[j].box) > kNmsIou) {
                suppressed[j] = 1;
            }
        }
        detections->push_back(cands[i].conf);
        detections->insert(detections->end(), cands[i].box, cands[i].box + 4);
        detections->insert(detections->end(), cands[i].kpts, cands[i].kpts + kKeypoints * 3);
        ++kept;
    }
    (*detections)[0] = static_cast<float>(kept);
    if ((mDecodeFrames++ % 30) == 0) {
        const float* box0 = kept > 0 ? cands.front().box : nullptr;
        logAndroid(
                ANDROID_LOG_INFO,
                "decode maxConf=%.3f preNms=%zu kept=%d src=%dx%d box0=[%.0f,%.0f,%.0f,%.0f]",
                maxConf,
                cands.size(),
                kept,
                mSrcWidth,
                mSrcHeight,
                box0 != nullptr ? box0[0] : -1.F,
                box0 != nullptr ? box0[1] : -1.F,
                box0 != nullptr ? box0[2] : -1.F,
                box0 != nullptr ? box0[3] : -1.F);
    }
}

std::vector<float> YoloPoseSession::inferRgb(
        int srcWidth,
        int srcHeight,
        const uint8_t* rgbPixels) {
    if (!mReady) {
        logAndroid(ANDROID_LOG_ERROR, "session not ready");
        return {};
    }
    if (rgbPixels == nullptr) {
        logAndroid(ANDROID_LOG_ERROR, "null rgb pixels");
        return {};
    }
    if (srcWidth <= 0 || srcHeight <= 0) {
        logAndroid(ANDROID_LOG_ERROR, "invalid src dims %dx%d", srcWidth, srcHeight);
        return {};
    }
    letterboxRgbToInput(srcWidth, srcHeight, rgbPixels);
    const Qnn_ErrorHandle_t status = mQnn.graphExecute(
            mGraph, &mInput, 1, &mOutput, 1, nullptr, nullptr);
    if (status != QNN_GRAPH_NO_ERROR) {
        logAndroid(ANDROID_LOG_ERROR, "graphExecute failed: %llx",
                   (unsigned long long)status);
        return {};
    }
    std::vector<float> detections;
    decodePose(&detections);
    return detections;
}

void YoloPoseSession::freeTensor(Qnn_Tensor_t* tensor) {
    char* name = const_cast<char*>(QNN_TENSOR_GET_NAME(tensor));
    if (name != nullptr) {
        std::free(name);
        QNN_TENSOR_SET_NAME(tensor, nullptr);
    }
    uint32_t* dims = QNN_TENSOR_GET_DIMENSIONS(tensor);
    if (dims != nullptr) {
        std::free(dims);
        QNN_TENSOR_SET_DIMENSIONS(tensor, nullptr);
    }
}

void YoloPoseSession::release() {
    mReady = false;
    mDecodeFrames = 0;
    mSocMismatch = false;
    freeTensor(&mInput);
    freeTensor(&mOutput);
    freeTensor(&mGraphInputs[0]);
    freeTensor(&mGraphOutputs[0]);
    mInput = QNN_TENSOR_INIT;
    mOutput = QNN_TENSOR_INIT;
    mGraphInputs[0] = QNN_TENSOR_INIT;
    mGraphOutputs[0] = QNN_TENSOR_INIT;
    if (mContext != nullptr && mQnn.contextFree != nullptr) {
        mQnn.contextFree(mContext, nullptr);
        mContext = nullptr;
    }
    if (mDevice != nullptr && mQnn.deviceFree != nullptr) {
        mQnn.deviceFree(mDevice);
        mDevice = nullptr;
    }
    if (mBackend != nullptr && mQnn.backendFree != nullptr) {
        mQnn.backendFree(mBackend);
        mBackend = nullptr;
    }
    if (mLogHandle != nullptr && mQnn.logFree != nullptr) {
        mQnn.logFree(mLogHandle);
        mLogHandle = nullptr;
    }
    mGraph = nullptr;
    if (mHtpHandle != nullptr) {
        dlclose(mHtpHandle);
        mHtpHandle = nullptr;
    }
    if (mSystemHandle != nullptr) {
        dlclose(mSystemHandle);
        mSystemHandle = nullptr;
    }
    mInputBuf.clear();
    mOutputBuf.clear();
}

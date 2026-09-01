/*
 * Copyright (c) 2026 智跃千里公司. All rights reserved.
 * File:        yolo_pose.h
 * Description: QAIRT HTP runner for YOLO26-pose context binaries.
 *              Ported from QnnYolo project (proven working on SM8845).
 * Version:     v1.0
 * Created:     2026-09-01
 * Author:      g-liujiangquan
 */

#ifndef QNN_DEMO_YOLO_POSE_H
#define QNN_DEMO_YOLO_POSE_H

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "QnnInterface.h"
#include "QnnTypeMacros.hpp"
#include "System/QnnSystemInterface.h"

/**
 * @brief Loads a QNN HTP context binary and runs YOLO26-pose.
 *
 * Reference implementation ported verbatim (modulo inferRgb vs inferYuv)
 * from the working QnnYolo project. Uses vendor libQnnHtp.so (build ID
 * v2.46 matching the vendor skel) + unsigned PD — the only configuration
 * that runs on SM8845 with the SDK 2.48 jniLibs we ship.
 *
 * 注意：QAIRT SDK 2.48 的 QNN 头里不定义 QNN_TENSOR_GET_* 宏（那些是 Easy API 的），
 * 这里移用 QnnYolo 项目附带的 QnnTypeMacros.hpp（同目录，提供同样的宏），让
 * Qnn_Tensor_t 的 v1/v2 字段访问走统一宏路径。
 */
class YoloPoseSession {
public:
    static constexpr int kInputSize = 640;
    static constexpr int kKeypoints = 17;
    static constexpr int kChannels = 56;
    static constexpr int kAnchors = 8400;
    static constexpr float kConfThreshold = 0.25F;
    static constexpr float kNmsIou = 0.45F;
    static constexpr int kMaxDetections = 10;
    static constexpr float kLetterboxPad = 114.F / 255.F;

    YoloPoseSession() = default;
    ~YoloPoseSession();

    YoloPoseSession(const YoloPoseSession&) = delete;
    YoloPoseSession& operator=(const YoloPoseSession&) = delete;

    /**
     * @brief Load HTP/System libs, create backend/context, allocate IO tensors.
     * @param nativeLibDir Absolute path of the app nativeLibraryDir.
     * @param contextBin Serialized QNN context binary (vendor bin, build ID v2.46).
     * @param contextBytes Size of contextBin in bytes.
     * @return Empty string on success, otherwise an English error.
     */
    std::string init(
            const char* nativeLibDir,
            const uint8_t* contextBin,
            size_t contextBytes);

    /**
     * @brief Run pose on an RGB bitmap (ARGB8888 pixels, width×height).
     *
     * The native side does letterbox + quantize-to-uint16 matching the bin's
     * input tensor encoding, then graphExecute + decode. Returns a flat
     * float array: [count, then count × (conf, x1, y1, x2, y2, 17×(x,y,vis))].
     */
    std::vector<float> inferRgb(
            int width,
            int height,
            const uint8_t* rgbPixels);

    /**
     * @brief Run pose on a YUV_420_888 image (camera frame).
     * @return Same flat float format as [inferRgb].
     */
    std::vector<float> inferYuv(
            int width,
            int height,
            int cropLeft,
            int cropTop,
            int yRowStride,
            int uRowStride,
            int vRowStride,
            int uPixelStride,
            int vPixelStride,
            const uint8_t* yPlane,
            const uint8_t* uPlane,
            const uint8_t* vPlane);

    void release();

private:
    bool loadInterfaces(const char* nativeLibDir);
    bool loadHtpInterface(const char* nativeLibDir);
    bool createSession(
            const char* nativeLibDir,
            const uint8_t* contextBin,
            size_t contextBytes);
    bool setupIoTensors();
    bool copyTensorMeta(Qnn_Tensor_t* dst, const Qnn_Tensor_t* src);
    void freeTensor(Qnn_Tensor_t* tensor);
    size_t tensorBytes(const Qnn_Tensor_t& tensor) const;
    void letterboxRgbToInput(
            int srcWidth,
            int srcHeight,
            const uint8_t* rgbPixels);
    void letterboxYuvToInput(
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
            const uint8_t* vPlane);
    void decodePose(std::vector<float>* detections);

    void* mHtpHandle = nullptr;
    void* mSystemHandle = nullptr;
    QNN_INTERFACE_VER_TYPE mQnn{};
    QNN_SYSTEM_INTERFACE_VER_TYPE mSystem{};
    Qnn_LogHandle_t mLogHandle = nullptr;
    Qnn_BackendHandle_t mBackend = nullptr;
    Qnn_DeviceHandle_t mDevice = nullptr;
    Qnn_ContextHandle_t mContext = nullptr;
    Qnn_GraphHandle_t mGraph = nullptr;
    Qnn_Tensor_t mGraphInputs[1]{};
    Qnn_Tensor_t mGraphOutputs[1]{};
    Qnn_Tensor_t mInput = QNN_TENSOR_INIT;
    Qnn_Tensor_t mOutput = QNN_TENSOR_INIT;
    uint32_t mNumInputs = 0;
    uint32_t mNumOutputs = 0;
    std::vector<uint8_t> mInputBuf;
    std::vector<uint8_t> mOutputBuf;
    std::string mGraphName;
    bool mSocMismatch = false;
    float mPadX = 0.F;
    float mPadY = 0.F;
    float mScale = 1.F;
    int mSrcWidth = 0;
    int mSrcHeight = 0;
    bool mReady = false;
    int mDecodeFrames = 0;
};

#endif

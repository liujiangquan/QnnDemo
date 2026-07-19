//=============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//=============================================================================

#ifndef QAIRT_HTP_GRAPH_H
#define QAIRT_HTP_GRAPH_H

#ifdef __cplusplus
#include <cstddef>
#include <cstdint>
#else
#include <stddef.h>
#include <stdint.h>
#endif

#include "QairtCommon/QairtCommon.h"
#include "QairtGraph/QairtGraphConfig.h"

//=============================================================================
// Macros
//=============================================================================
#define QAIRT_MIN_ID_HTP_GRAPH (QAIRT_MIN_ID_HTP + 100)

#define QAIRT_HTP_GRAPH_CUSTOM_CONFIG_ID       (QAIRT_MIN_ID_HTP_GRAPH + 0)
#define QAIRT_HTP_GRAPH_OPTIMIZATION_OPTION_ID (QAIRT_MIN_ID_HTP_GRAPH + 1)
#define QAIRT_HTP_GRAPH_HMX_BOUNDING_INFO_ID   (QAIRT_MIN_ID_HTP_GRAPH + 2)

// Define ranges for HTP graph-specific error codes
#define QAIRT_MIN_ERROR_HTP_GRAPH (QAIRT_MIN_ERROR_HTP + 1000)
#define QAIRT_MAX_ERROR_HTP_GRAPH (QAIRT_MIN_ERROR_HTP + 1999)

//=============================================================================
// Data Types
//=============================================================================
typedef QairtGraph_CustomConfigHandle_t QairtHtpGraph_CustomConfigHandle_t;

typedef struct _QairtHtpGraph_OptimizationOptionHandle_t*
    QairtHtpGraph_OptimizationOptionHandle_t;

typedef struct _QairtHtp_HmxBoundingInfoHandle_t* QairtHtp_HmxBoundingInfoHandle_t;

/**
 * @brief Enum of HTP graph optimization types, mirroring QnnHtpGraph_OptimizationType_t.
 */
typedef enum {
  QAIRT_HTP_GRAPH_OPTIMIZATION_TYPE_SCHEDULE_THRESHOLD                = 1,
  QAIRT_HTP_GRAPH_OPTIMIZATION_TYPE_FINALIZE_RETRIES                  = 2,
  QAIRT_HTP_GRAPH_OPTIMIZATION_TYPE_FINALIZE_OPTIMIZATION_FLAG        = 3,
  QAIRT_HTP_GRAPH_OPTIMIZATION_TYPE_ENABLE_DLBC                       = 4,
  QAIRT_HTP_GRAPH_OPTIMIZATION_TYPE_ENABLE_DLBC_WEIGHTS               = 5,
  QAIRT_HTP_GRAPH_OPTIMIZATION_TYPE_ENABLE_SPARSE_WEIGHTS_COMPRESSION = 6,
  QAIRT_HTP_GRAPH_OPTIMIZATION_TYPE_ENABLE_SLC_ALLOCATOR              = 7,
  QAIRT_HTP_GRAPH_OPTIMIZATION_TYPE_UNKNOWN                           = 0x7fffffff
} QairtHtpGraph_OptimizationType_t;

/**
 * @brief Precision type for HTP graph configuration, mirroring Qnn_Precision_t.
 */
typedef enum {
  QAIRT_HTP_GRAPH_PRECISION_FLOAT16   = 1,
  QAIRT_HTP_GRAPH_PRECISION_FLOAT32   = 2,
  QAIRT_HTP_GRAPH_PRECISION_DEFAULT   = QAIRT_HTP_GRAPH_PRECISION_FLOAT16,
  QAIRT_HTP_GRAPH_PRECISION_UNDEFINED = 0x7fffffff
} QairtHtpGraph_Precision_t;

/**
 * @brief QAIRT HTP Graph API result / error codes.
 */
typedef enum {
  QAIRT_HTP_GRAPH_MIN_ERROR = QAIRT_MIN_ERROR_HTP_GRAPH,
  ////////////////////////////////////////

  /// HTP graph operation succeeded.
  QAIRT_HTP_GRAPH_NO_ERROR = QAIRT_SUCCESS,
  /// General memory allocation failure.
  QAIRT_HTP_GRAPH_ERROR_MEM_ALLOC = QAIRT_COMMON_ERROR_MEM_ALLOC,
  /// Feature or API is not supported.
  QAIRT_HTP_GRAPH_ERROR_UNSUPPORTED_FEATURE = QAIRT_COMMON_ERROR_NOT_SUPPORTED,
  /// General error not covered by a more specific code.
  QAIRT_HTP_GRAPH_ERROR_GENERAL = QAIRT_COMMON_ERROR_GENERAL,
  /// An argument to the API is invalid.
  QAIRT_HTP_GRAPH_ERROR_INVALID_ARGUMENT = QAIRT_MIN_ERROR_HTP_GRAPH + 0,
  /// A handle argument is not valid.
  QAIRT_HTP_GRAPH_ERROR_INVALID_HANDLE = QAIRT_MIN_ERROR_HTP_GRAPH + 1,
  /// A configuration option mismatch was detected on get.
  QAIRT_HTP_GRAPH_ERROR_OPTION_MISMATCH = QAIRT_MIN_ERROR_HTP_GRAPH + 2,

  ////////////////////////////////////////
  QAIRT_HTP_GRAPH_MAX_ERROR = QAIRT_MAX_ERROR_HTP_GRAPH,
  // Unused, present to ensure 32 bits.
  QAIRT_HTP_GRAPH_ERROR_UNDEFINED = 0x7FFFFFFF
} QairtHtpGraph_Error_t;

//=============================================================================
// Public Functions
//=============================================================================

#ifdef __cplusplus
extern "C" {
#endif

/// HTP Graph Optimization Option

/**
 * @brief Create an optimization option handle.
 *
 * @param[out] optimizationOption A pointer to receive the created handle.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Handle was successfully created.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_ARGUMENT: optimizationOption is NULL.
 *         - QAIRT_HTP_GRAPH_ERROR_MEM_ALLOC: Memory allocation failed.
 *
 * @note Use corresponding API through QairtHtpGraph_OptimizationOptionV1_t.
 */
Qairt_Status_t QairtHtpGraph_OptimizationOption_create(
    QairtHtpGraph_OptimizationOptionHandle_t* optimizationOption);

/**
 * @brief Free a previously created optimization option handle.
 *
 * @param[in] optimizationOption The handle to free.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Handle was successfully freed.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_HANDLE: optimizationOption is not a valid handle.
 *
 * @note Use corresponding API through QairtHtpGraph_OptimizationOptionV1_t.
 */
Qairt_Status_t QairtHtpGraph_OptimizationOption_free(
    QairtHtpGraph_OptimizationOptionHandle_t optimizationOption);

/**
 * @brief Set the optimization type on an optimization option handle.
 *
 * @param[in] optimizationOption Handle to the optimization option object.
 * @param[in] type               The optimization type to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Value was successfully set.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_HANDLE: optimizationOption is not a valid handle.
 *
 * @note Use corresponding API through QairtHtpGraph_OptimizationOptionV1_t.
 */
Qairt_Status_t QairtHtpGraph_OptimizationOption_setType(
    QairtHtpGraph_OptimizationOptionHandle_t optimizationOption,
    QairtHtpGraph_OptimizationType_t type);

/**
 * @brief Get the optimization type from an optimization option handle.
 *
 * @param[in]  optimizationOption Handle to the optimization option object.
 * @param[out] type               Pointer to receive the optimization type.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Value was successfully retrieved.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_HANDLE: optimizationOption is not a valid handle.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_ARGUMENT: type is NULL.
 *
 * @note Use corresponding API through QairtHtpGraph_OptimizationOptionV1_t.
 */
Qairt_Status_t QairtHtpGraph_OptimizationOption_getType(
    QairtHtpGraph_OptimizationOptionHandle_t optimizationOption,
    QairtHtpGraph_OptimizationType_t* type);

/**
 * @brief Set the float value on an optimization option handle.
 *
 * @param[in] optimizationOption Handle to the optimization option object.
 * @param[in] floatValue         The float value to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Value was successfully set.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_HANDLE: optimizationOption is not a valid handle.
 *
 * @note Use corresponding API through QairtHtpGraph_OptimizationOptionV1_t.
 */
Qairt_Status_t QairtHtpGraph_OptimizationOption_setFloatValue(
    QairtHtpGraph_OptimizationOptionHandle_t optimizationOption, float floatValue);

/**
 * @brief Get the float value from an optimization option handle.
 *
 * @param[in]  optimizationOption Handle to the optimization option object.
 * @param[out] floatValue         Pointer to receive the float value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Value was successfully retrieved.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_HANDLE: optimizationOption is not a valid handle.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_ARGUMENT: floatValue is NULL.
 *
 * @note Use corresponding API through QairtHtpGraph_OptimizationOptionV1_t.
 */
Qairt_Status_t QairtHtpGraph_OptimizationOption_getFloatValue(
    QairtHtpGraph_OptimizationOptionHandle_t optimizationOption, float* floatValue);

/// HTP HMX Bounding Info

/**
 * @brief Create an HMX bounding info handle.
 *
 * @param[out] hmxBoundingInfo A pointer to receive the created handle.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Handle was successfully created.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_ARGUMENT: hmxBoundingInfo is NULL.
 *         - QAIRT_HTP_GRAPH_ERROR_MEM_ALLOC: Memory allocation failed.
 *
 * @note Only applicable for DSP architectures >= V81.
 * @note Use corresponding API through QairtHtp_HmxBoundingInfoV1_t.
 */
Qairt_Status_t QairtHtp_HmxBoundingInfo_create(
    QairtHtp_HmxBoundingInfoHandle_t* hmxBoundingInfo);

/**
 * @brief Free a previously created HMX bounding info handle.
 *
 * @param[in] hmxBoundingInfo The handle to free.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Handle was successfully freed.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_HANDLE: hmxBoundingInfo is not a valid handle.
 *
 * @note Use corresponding API through QairtHtp_HmxBoundingInfoV1_t.
 */
Qairt_Status_t QairtHtp_HmxBoundingInfo_free(
    QairtHtp_HmxBoundingInfoHandle_t hmxBoundingInfo);

/**
 * @brief Set the target HMX frequency (Hz) on an HMX bounding info handle.
 *
 * @param[in] hmxBoundingInfo Handle to the HMX bounding info object.
 * @param[in] targetHmxFreqHz The target HMX frequency in Hz.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Value was successfully set.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_HANDLE: hmxBoundingInfo is not a valid handle.
 *
 * @note Use corresponding API through QairtHtp_HmxBoundingInfoV1_t.
 */
Qairt_Status_t QairtHtp_HmxBoundingInfo_setTargetHmxFreqHz(
    QairtHtp_HmxBoundingInfoHandle_t hmxBoundingInfo, float targetHmxFreqHz);

/**
 * @brief Get the target HMX frequency (Hz) from an HMX bounding info handle.
 *
 * @param[in]  hmxBoundingInfo Handle to the HMX bounding info object.
 * @param[out] targetHmxFreqHz Pointer to receive the target HMX frequency in Hz.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Value was successfully retrieved.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_HANDLE: hmxBoundingInfo is not a valid handle.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_ARGUMENT: targetHmxFreqHz is NULL.
 *
 * @note Use corresponding API through QairtHtp_HmxBoundingInfoV1_t.
 */
Qairt_Status_t QairtHtp_HmxBoundingInfo_getTargetHmxFreqHz(
    QairtHtp_HmxBoundingInfoHandle_t hmxBoundingInfo, float* targetHmxFreqHz);

/**
 * @brief Set the target DSP core frequency on an HMX bounding info handle.
 *
 * @param[in] hmxBoundingInfo   Handle to the HMX bounding info object.
 * @param[in] targetDspCoreFreq The target DSP core frequency.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Value was successfully set.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_HANDLE: hmxBoundingInfo is not a valid handle.
 *
 * @note Use corresponding API through QairtHtp_HmxBoundingInfoV1_t.
 */
Qairt_Status_t QairtHtp_HmxBoundingInfo_setTargetDspCoreFreq(
    QairtHtp_HmxBoundingInfoHandle_t hmxBoundingInfo, float targetDspCoreFreq);

/**
 * @brief Get the target DSP core frequency from an HMX bounding info handle.
 *
 * @param[in]  hmxBoundingInfo   Handle to the HMX bounding info object.
 * @param[out] targetDspCoreFreq Pointer to receive the target DSP core frequency.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Value was successfully retrieved.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_HANDLE: hmxBoundingInfo is not a valid handle.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_ARGUMENT: targetDspCoreFreq is NULL.
 *
 * @note Use corresponding API through QairtHtp_HmxBoundingInfoV1_t.
 */
Qairt_Status_t QairtHtp_HmxBoundingInfo_getTargetDspCoreFreq(
    QairtHtp_HmxBoundingInfoHandle_t hmxBoundingInfo, float* targetDspCoreFreq);

/// HTP Graph Custom Configuration

/**
 * @brief Create a custom configuration handle for HTP graph operations.
 *
 * @param[out] customConfig A pointer to receive the created custom configuration handle.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Custom configuration handle was successfully created.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_ARGUMENT: customConfig is NULL.
 *         - QAIRT_HTP_GRAPH_ERROR_MEM_ALLOC: Memory allocation failed during creation.
 *
 * @note Use corresponding API through QairtHtpGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpGraph_CustomConfig_create(
    QairtHtpGraph_CustomConfigHandle_t* customConfig);

/**
 * @brief Free a previously created custom configuration handle and associated resources.
 *
 * @param[in] customConfig The custom configuration handle to free.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Custom configuration handle was successfully freed.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *
 * @note Use corresponding API through QairtHtpGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpGraph_CustomConfig_free(
    QairtHtpGraph_CustomConfigHandle_t customConfig);

/* Optimization Option */
/**
 * @brief Set the graph optimization option.
 *
 * @param[in] customConfig Handle to the custom configuration object.
 * @param[in] optimizationOption The optimization option to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Option was successfully set.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *
 * @note Use corresponding API through QairtHtpGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpGraph_CustomConfig_setOptimizationOption(
    QairtHtpGraph_CustomConfigHandle_t customConfig,
    QairtHtpGraph_OptimizationOptionHandle_t optimizationOption);

/**
 * @brief Get the graph optimization option.
 *
 * @param[in]  customConfig Handle to the custom configuration object.
 * @param[out] optimizationOption Pointer to receive the optimization option handle.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Value was successfully retrieved.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_ARGUMENT: optimizationOption is NULL or option mismatch.
 *
 * @note Use corresponding API through QairtHtpGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpGraph_CustomConfig_getOptimizationOption(
    QairtHtpGraph_CustomConfigHandle_t customConfig,
    QairtHtpGraph_OptimizationOptionHandle_t* optimizationOption);

/* Precision */
/**
 * @brief Set the graph precision.
 *
 * @param[in] customConfig Handle to the custom configuration object.
 * @param[in] precision The precision value to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Option was successfully set.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *
 * @note Use corresponding API through QairtHtpGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpGraph_CustomConfig_setPrecision(
    QairtHtpGraph_CustomConfigHandle_t customConfig, QairtHtpGraph_Precision_t precision);

/**
 * @brief Get the graph precision.
 *
 * @param[in]  customConfig Handle to the custom configuration object.
 * @param[out] precision Pointer to receive the precision value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Value was successfully retrieved.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_ARGUMENT: precision is NULL or option mismatch.
 *
 * @note Use corresponding API through QairtHtpGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpGraph_CustomConfig_getPrecision(
    QairtHtpGraph_CustomConfigHandle_t customConfig, QairtHtpGraph_Precision_t* precision);

/* VTCM Size (MB) */
/**
 * @brief Set the VTCM size in megabytes.
 *
 * @param[in] customConfig Handle to the custom configuration object.
 * @param[in] vtcmSizeInMB The VTCM size in MB. Use QNN_HTP_GRAPH_CONFIG_OPTION_MAX for maximum.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Option was successfully set.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *
 * @note Use corresponding API through QairtHtpGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpGraph_CustomConfig_setVtcmSizeInMB(
    QairtHtpGraph_CustomConfigHandle_t customConfig, uint32_t vtcmSizeInMB);

/**
 * @brief Get the VTCM size in megabytes.
 *
 * @param[in]  customConfig Handle to the custom configuration object.
 * @param[out] vtcmSizeInMB Pointer to receive the VTCM size in MB.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Value was successfully retrieved.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_ARGUMENT: vtcmSizeInMB is NULL or option mismatch.
 *
 * @note Use corresponding API through QairtHtpGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpGraph_CustomConfig_getVtcmSizeInMB(
    QairtHtpGraph_CustomConfigHandle_t customConfig, uint32_t* vtcmSizeInMB);

/* VTCM Size (Bytes) */
/**
 * @brief Set the VTCM size in bytes.
 *
 * @param[in] customConfig Handle to the custom configuration object.
 * @param[in] vtcmSizeInBytes The VTCM size in bytes.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Option was successfully set.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *
 * @note Use corresponding API through QairtHtpGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpGraph_CustomConfig_setVtcmSizeInBytes(
    QairtHtpGraph_CustomConfigHandle_t customConfig, uint32_t vtcmSizeInBytes);

/**
 * @brief Get the VTCM size in bytes.
 *
 * @param[in]  customConfig Handle to the custom configuration object.
 * @param[out] vtcmSizeInBytes Pointer to receive the VTCM size in bytes.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Value was successfully retrieved.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_ARGUMENT: vtcmSizeInBytes is NULL or option mismatch.
 *
 * @note Use corresponding API through QairtHtpGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpGraph_CustomConfig_getVtcmSizeInBytes(
    QairtHtpGraph_CustomConfigHandle_t customConfig, uint32_t* vtcmSizeInBytes);

/* Fold ReLU Activation Into Conv Off */
/**
 * @brief Disable folding of ReLU activation into convolution.
 *
 * @param[in] customConfig Handle to the custom configuration object.
 * @param[in] foldReluOff Non-zero to disable folding; zero to enable.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Option was successfully set.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *
 * @note Use corresponding API through QairtHtpGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpGraph_CustomConfig_setFoldReluActivationIntoConvOff(
    QairtHtpGraph_CustomConfigHandle_t customConfig, uint8_t foldReluOff);

/**
 * @brief Get the fold ReLU activation into convolution setting.
 *
 * @param[in]  customConfig Handle to the custom configuration object.
 * @param[out] foldReluOff Pointer to receive the setting.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Value was successfully retrieved.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_ARGUMENT: foldReluOff is NULL or option mismatch.
 *
 * @note Use corresponding API through QairtHtpGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpGraph_CustomConfig_getFoldReluActivationIntoConvOff(
    QairtHtpGraph_CustomConfigHandle_t customConfig, uint8_t* foldReluOff);

/* Short Depth Conv On HMX Off */
/**
 * @brief Disable short depth convolution on HMX.
 *
 * @param[in] customConfig Handle to the custom configuration object.
 * @param[in] shortDepthConvOff Non-zero to disable; zero to enable.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Option was successfully set.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *
 * @note Use corresponding API through QairtHtpGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpGraph_CustomConfig_setShortDepthConvOnHmxOff(
    QairtHtpGraph_CustomConfigHandle_t customConfig, uint8_t shortDepthConvOff);

/**
 * @brief Get the short depth convolution on HMX setting.
 *
 * @param[in]  customConfig Handle to the custom configuration object.
 * @param[out] shortDepthConvOff Pointer to receive the setting.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Value was successfully retrieved.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_ARGUMENT: shortDepthConvOff is NULL or option mismatch.
 *
 * @note Use corresponding API through QairtHtpGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpGraph_CustomConfig_getShortDepthConvOnHmxOff(
    QairtHtpGraph_CustomConfigHandle_t customConfig, uint8_t* shortDepthConvOff);

/* Num HVX Threads */
/**
 * @brief Set the number of HVX threads.
 *
 * @param[in] customConfig Handle to the custom configuration object.
 * @param[in] numHvxThreads The number of HVX threads to use.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Option was successfully set.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *
 * @note Use corresponding API through QairtHtpGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpGraph_CustomConfig_setNumHvxThreads(
    QairtHtpGraph_CustomConfigHandle_t customConfig, uint64_t numHvxThreads);

/**
 * @brief Get the number of HVX threads.
 *
 * @param[in]  customConfig Handle to the custom configuration object.
 * @param[out] numHvxThreads Pointer to receive the number of HVX threads.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Value was successfully retrieved.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_ARGUMENT: numHvxThreads is NULL or option mismatch.
 *
 * @note Use corresponding API through QairtHtpGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpGraph_CustomConfig_getNumHvxThreads(
    QairtHtpGraph_CustomConfigHandle_t customConfig, uint64_t* numHvxThreads);

/* Num Cores */
/**
 * @brief Set the number of cores.
 *
 * @param[in] customConfig Handle to the custom configuration object.
 * @param[in] numCores The number of cores to use.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Option was successfully set.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *
 * @note Use corresponding API through QairtHtpGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpGraph_CustomConfig_setNumCores(
    QairtHtpGraph_CustomConfigHandle_t customConfig, uint32_t numCores);

/**
 * @brief Get the number of cores.
 *
 * @param[in]  customConfig Handle to the custom configuration object.
 * @param[out] numCores Pointer to receive the number of cores.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Value was successfully retrieved.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_ARGUMENT: numCores is NULL or option mismatch.
 *
 * @note Use corresponding API through QairtHtpGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpGraph_CustomConfig_getNumCores(
    QairtHtpGraph_CustomConfigHandle_t customConfig, uint32_t* numCores);

/* HMX Bounding */
/**
 * @brief Set the HMX bounding info.
 *
 * @param[in] customConfig Handle to the custom configuration object.
 * @param[in] hmxBoundingInfo The HMX bounding info to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Option was successfully set.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *
 * @note Only applicable for DSP architectures >= V81.
 * @note Use corresponding API through QairtHtpGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpGraph_CustomConfig_setHmxBoundingInfo(
    QairtHtpGraph_CustomConfigHandle_t customConfig,
    QairtHtp_HmxBoundingInfoHandle_t hmxBoundingInfo);

/**
 * @brief Get the HMX bounding info.
 *
 * @param[in]  customConfig Handle to the custom configuration object.
 * @param[out] hmxBoundingInfo Pointer to receive the HMX bounding info handle.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Value was successfully retrieved.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_ARGUMENT: hmxBoundingInfo is NULL or option mismatch.
 *
 * @note Use corresponding API through QairtHtpGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpGraph_CustomConfig_getHmxBoundingInfo(
    QairtHtpGraph_CustomConfigHandle_t customConfig,
    QairtHtp_HmxBoundingInfoHandle_t* hmxBoundingInfo);

/* Weights Packing */
/**
 * @brief Enable or disable weights packing.
 *
 * @param[in] customConfig Handle to the custom configuration object.
 * @param[in] weightsPacking Non-zero to enable; zero to disable.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Option was successfully set.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *
 * @note Use corresponding API through QairtHtpGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpGraph_CustomConfig_setWeightsPacking(
    QairtHtpGraph_CustomConfigHandle_t customConfig, uint8_t weightsPacking);

/**
 * @brief Get the weights packing setting.
 *
 * @param[in]  customConfig Handle to the custom configuration object.
 * @param[out] weightsPacking Pointer to receive the setting.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Value was successfully retrieved.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_ARGUMENT: weightsPacking is NULL or option mismatch.
 *
 * @note Use corresponding API through QairtHtpGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpGraph_CustomConfig_getWeightsPacking(
    QairtHtpGraph_CustomConfigHandle_t customConfig, uint8_t* weightsPacking);

/* Assume Same Quant */
/**
 * @brief Enable or disable assume same quantization.
 *
 * @param[in] customConfig Handle to the custom configuration object.
 * @param[in] assumeSameQuant Non-zero to enable; zero to disable.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Option was successfully set.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *
 * @note Use corresponding API through QairtHtpGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpGraph_CustomConfig_setAssumeSameQuant(
    QairtHtpGraph_CustomConfigHandle_t customConfig, uint8_t assumeSameQuant);

/**
 * @brief Get the assume same quantization setting.
 *
 * @param[in]  customConfig Handle to the custom configuration object.
 * @param[out] assumeSameQuant Pointer to receive the setting.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Value was successfully retrieved.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_ARGUMENT: assumeSameQuant is NULL or option mismatch.
 *
 * @note Use corresponding API through QairtHtpGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpGraph_CustomConfig_getAssumeSameQuant(
    QairtHtpGraph_CustomConfigHandle_t customConfig, uint8_t* assumeSameQuant);

/* Share IO Buffer */
/**
 * @brief Enable or disable sharing of I/O buffers.
 *
 * @param[in] customConfig Handle to the custom configuration object.
 * @param[in] shareIOBuffer Non-zero to enable; zero to disable.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Option was successfully set.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *
 * @note Use corresponding API through QairtHtpGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpGraph_CustomConfig_setShareIOBuffer(
    QairtHtpGraph_CustomConfigHandle_t customConfig, uint8_t shareIOBuffer);

/**
 * @brief Get the share I/O buffer setting.
 *
 * @param[in]  customConfig Handle to the custom configuration object.
 * @param[out] shareIOBuffer Pointer to receive the setting.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Value was successfully retrieved.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_ARGUMENT: shareIOBuffer is NULL or option mismatch.
 *
 * @note Use corresponding API through QairtHtpGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpGraph_CustomConfig_getShareIOBuffer(
    QairtHtpGraph_CustomConfigHandle_t customConfig, uint8_t* shareIOBuffer);

/* Advanced Activation Fusion */
/**
 * @brief Enable or disable advanced activation fusion.
 *
 * @param[in] customConfig Handle to the custom configuration object.
 * @param[in] advancedActivationFusion Non-zero to enable; zero to disable.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Option was successfully set.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *
 * @note Use corresponding API through QairtHtpGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpGraph_CustomConfig_setAdvancedActivationFusion(
    QairtHtpGraph_CustomConfigHandle_t customConfig, uint8_t advancedActivationFusion);

/**
 * @brief Get the advanced activation fusion setting.
 *
 * @param[in]  customConfig Handle to the custom configuration object.
 * @param[out] advancedActivationFusion Pointer to receive the setting.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Value was successfully retrieved.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_ARGUMENT: advancedActivationFusion is NULL or option
 *           mismatch.
 *
 * @note Use corresponding API through QairtHtpGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpGraph_CustomConfig_getAdvancedActivationFusion(
    QairtHtpGraph_CustomConfigHandle_t customConfig, uint8_t* advancedActivationFusion);

/* High Precision Sigmoid */
/**
 * @brief Enable or disable high precision sigmoid.
 *
 * @param[in] customConfig Handle to the custom configuration object.
 * @param[in] highPrecisionSigmoid Non-zero to enable; zero to disable.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Option was successfully set.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *
 * @note Use corresponding API through QairtHtpGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpGraph_CustomConfig_setHighPrecisionSigmoid(
    QairtHtpGraph_CustomConfigHandle_t customConfig, uint8_t highPrecisionSigmoid);

/**
 * @brief Get the high precision sigmoid setting.
 *
 * @param[in]  customConfig Handle to the custom configuration object.
 * @param[out] highPrecisionSigmoid Pointer to receive the setting.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Value was successfully retrieved.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_ARGUMENT: highPrecisionSigmoid is NULL or option
 * mismatch.
 *
 * @note Use corresponding API through QairtHtpGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpGraph_CustomConfig_getHighPrecisionSigmoid(
    QairtHtpGraph_CustomConfigHandle_t customConfig, uint8_t* highPrecisionSigmoid);

/* Monolithic LSTM */
/**
 * @brief Enable or disable monolithic LSTM.
 *
 * @param[in] customConfig Handle to the custom configuration object.
 * @param[in] monolithicLstm Non-zero to enable; zero to disable.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Option was successfully set.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *
 * @note Use corresponding API through QairtHtpGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpGraph_CustomConfig_setMonolithicLstm(
    QairtHtpGraph_CustomConfigHandle_t customConfig, uint8_t monolithicLstm);

/**
 * @brief Get the monolithic LSTM setting.
 *
 * @param[in]  customConfig Handle to the custom configuration object.
 * @param[out] monolithicLstm Pointer to receive the setting.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Value was successfully retrieved.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *         - QAIRT_HTP_GRAPH_ERROR_INVALID_ARGUMENT: monolithicLstm is NULL or option mismatch.
 *
 * @note Use corresponding API through QairtHtpGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpGraph_CustomConfig_getMonolithicLstm(
    QairtHtpGraph_CustomConfigHandle_t customConfig, uint8_t* monolithicLstm);

//=============================================================================
// Function Pointer Typedefs
//=============================================================================

/* Optimization Option */
typedef Qairt_Status_t (*QairtHtpGraph_OptimizationOption_CreateFn_t)(
    QairtHtpGraph_OptimizationOptionHandle_t*);
typedef Qairt_Status_t (*QairtHtpGraph_OptimizationOption_FreeFn_t)(
    QairtHtpGraph_OptimizationOptionHandle_t);
typedef Qairt_Status_t (*QairtHtpGraph_OptimizationOption_SetTypeFn_t)(
    QairtHtpGraph_OptimizationOptionHandle_t, QairtHtpGraph_OptimizationType_t);
typedef Qairt_Status_t (*QairtHtpGraph_OptimizationOption_GetTypeFn_t)(
    QairtHtpGraph_OptimizationOptionHandle_t, QairtHtpGraph_OptimizationType_t*);
typedef Qairt_Status_t (*QairtHtpGraph_OptimizationOption_SetFloatValueFn_t)(
    QairtHtpGraph_OptimizationOptionHandle_t, float);
typedef Qairt_Status_t (*QairtHtpGraph_OptimizationOption_GetFloatValueFn_t)(
    QairtHtpGraph_OptimizationOptionHandle_t, float*);

/* HMX Bounding Info */
typedef Qairt_Status_t (*QairtHtp_HmxBoundingInfo_CreateFn_t)(
    QairtHtp_HmxBoundingInfoHandle_t*);
typedef Qairt_Status_t (*QairtHtp_HmxBoundingInfo_FreeFn_t)(
    QairtHtp_HmxBoundingInfoHandle_t);
typedef Qairt_Status_t (*QairtHtp_HmxBoundingInfo_SetTargetHmxFreqHzFn_t)(
    QairtHtp_HmxBoundingInfoHandle_t, float);
typedef Qairt_Status_t (*QairtHtp_HmxBoundingInfo_GetTargetHmxFreqHzFn_t)(
    QairtHtp_HmxBoundingInfoHandle_t, float*);
typedef Qairt_Status_t (*QairtHtp_HmxBoundingInfo_SetTargetDspCoreFreqFn_t)(
    QairtHtp_HmxBoundingInfoHandle_t, float);
typedef Qairt_Status_t (*QairtHtp_HmxBoundingInfo_GetTargetDspCoreFreqFn_t)(
    QairtHtp_HmxBoundingInfoHandle_t, float*);

/* Custom Config */
typedef Qairt_Status_t (*QairtHtpGraph_CustomConfig_CreateFn_t)(
    QairtHtpGraph_CustomConfigHandle_t*);
typedef Qairt_Status_t (*QairtHtpGraph_CustomConfig_FreeFn_t)(
    QairtHtpGraph_CustomConfigHandle_t);

typedef Qairt_Status_t (*QairtHtpGraph_CustomConfig_SetOptimizationOptionFn_t)(
    QairtHtpGraph_CustomConfigHandle_t, QairtHtpGraph_OptimizationOptionHandle_t);
typedef Qairt_Status_t (*QairtHtpGraph_CustomConfig_GetOptimizationOptionFn_t)(
    QairtHtpGraph_CustomConfigHandle_t, QairtHtpGraph_OptimizationOptionHandle_t*);

typedef Qairt_Status_t (*QairtHtpGraph_CustomConfig_SetPrecisionFn_t)(
    QairtHtpGraph_CustomConfigHandle_t, QairtHtpGraph_Precision_t);
typedef Qairt_Status_t (*QairtHtpGraph_CustomConfig_GetPrecisionFn_t)(
    QairtHtpGraph_CustomConfigHandle_t, QairtHtpGraph_Precision_t*);

typedef Qairt_Status_t (*QairtHtpGraph_CustomConfig_SetVtcmSizeInMBFn_t)(
    QairtHtpGraph_CustomConfigHandle_t, uint32_t);
typedef Qairt_Status_t (*QairtHtpGraph_CustomConfig_GetVtcmSizeInMBFn_t)(
    QairtHtpGraph_CustomConfigHandle_t, uint32_t*);

typedef Qairt_Status_t (*QairtHtpGraph_CustomConfig_SetVtcmSizeInBytesFn_t)(
    QairtHtpGraph_CustomConfigHandle_t, uint32_t);
typedef Qairt_Status_t (*QairtHtpGraph_CustomConfig_GetVtcmSizeInBytesFn_t)(
    QairtHtpGraph_CustomConfigHandle_t, uint32_t*);

typedef Qairt_Status_t (*QairtHtpGraph_CustomConfig_SetFoldReluActivationIntoConvOffFn_t)(
    QairtHtpGraph_CustomConfigHandle_t, uint8_t);
typedef Qairt_Status_t (*QairtHtpGraph_CustomConfig_GetFoldReluActivationIntoConvOffFn_t)(
    QairtHtpGraph_CustomConfigHandle_t, uint8_t*);

typedef Qairt_Status_t (*QairtHtpGraph_CustomConfig_SetShortDepthConvOnHmxOffFn_t)(
    QairtHtpGraph_CustomConfigHandle_t, uint8_t);
typedef Qairt_Status_t (*QairtHtpGraph_CustomConfig_GetShortDepthConvOnHmxOffFn_t)(
    QairtHtpGraph_CustomConfigHandle_t, uint8_t*);

typedef Qairt_Status_t (*QairtHtpGraph_CustomConfig_SetNumHvxThreadsFn_t)(
    QairtHtpGraph_CustomConfigHandle_t, uint64_t);
typedef Qairt_Status_t (*QairtHtpGraph_CustomConfig_GetNumHvxThreadsFn_t)(
    QairtHtpGraph_CustomConfigHandle_t, uint64_t*);

typedef Qairt_Status_t (*QairtHtpGraph_CustomConfig_SetNumCoresFn_t)(
    QairtHtpGraph_CustomConfigHandle_t, uint32_t);
typedef Qairt_Status_t (*QairtHtpGraph_CustomConfig_GetNumCoresFn_t)(
    QairtHtpGraph_CustomConfigHandle_t, uint32_t*);

typedef Qairt_Status_t (*QairtHtpGraph_CustomConfig_SetHmxBoundingInfoFn_t)(
    QairtHtpGraph_CustomConfigHandle_t, QairtHtp_HmxBoundingInfoHandle_t);
typedef Qairt_Status_t (*QairtHtpGraph_CustomConfig_GetHmxBoundingInfoFn_t)(
    QairtHtpGraph_CustomConfigHandle_t, QairtHtp_HmxBoundingInfoHandle_t*);

typedef Qairt_Status_t (*QairtHtpGraph_CustomConfig_SetWeightsPackingFn_t)(
    QairtHtpGraph_CustomConfigHandle_t, uint8_t);
typedef Qairt_Status_t (*QairtHtpGraph_CustomConfig_GetWeightsPackingFn_t)(
    QairtHtpGraph_CustomConfigHandle_t, uint8_t*);

typedef Qairt_Status_t (*QairtHtpGraph_CustomConfig_SetAssumeSameQuantFn_t)(
    QairtHtpGraph_CustomConfigHandle_t, uint8_t);
typedef Qairt_Status_t (*QairtHtpGraph_CustomConfig_GetAssumeSameQuantFn_t)(
    QairtHtpGraph_CustomConfigHandle_t, uint8_t*);

typedef Qairt_Status_t (*QairtHtpGraph_CustomConfig_SetShareIOBufferFn_t)(
    QairtHtpGraph_CustomConfigHandle_t, uint8_t);
typedef Qairt_Status_t (*QairtHtpGraph_CustomConfig_GetShareIOBufferFn_t)(
    QairtHtpGraph_CustomConfigHandle_t, uint8_t*);

typedef Qairt_Status_t (*QairtHtpGraph_CustomConfig_SetAdvancedActivationFusionFn_t)(
    QairtHtpGraph_CustomConfigHandle_t, uint8_t);
typedef Qairt_Status_t (*QairtHtpGraph_CustomConfig_GetAdvancedActivationFusionFn_t)(
    QairtHtpGraph_CustomConfigHandle_t, uint8_t*);

typedef Qairt_Status_t (*QairtHtpGraph_CustomConfig_SetHighPrecisionSigmoidFn_t)(
    QairtHtpGraph_CustomConfigHandle_t, uint8_t);
typedef Qairt_Status_t (*QairtHtpGraph_CustomConfig_GetHighPrecisionSigmoidFn_t)(
    QairtHtpGraph_CustomConfigHandle_t, uint8_t*);

typedef Qairt_Status_t (*QairtHtpGraph_CustomConfig_SetMonolithicLstmFn_t)(
    QairtHtpGraph_CustomConfigHandle_t, uint8_t);
typedef Qairt_Status_t (*QairtHtpGraph_CustomConfig_GetMonolithicLstmFn_t)(
    QairtHtpGraph_CustomConfigHandle_t, uint8_t*);

//=============================================================================
// Public Interfaces
//=============================================================================

typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtHtpGraph_OptimizationOption_CreateFn_t create;
  QairtHtpGraph_OptimizationOption_FreeFn_t free;
  QairtHtpGraph_OptimizationOption_SetTypeFn_t setType;
  QairtHtpGraph_OptimizationOption_GetTypeFn_t getType;
  QairtHtpGraph_OptimizationOption_SetFloatValueFn_t setFloatValue;
  QairtHtpGraph_OptimizationOption_GetFloatValueFn_t getFloatValue;
} QairtHtpGraph_OptimizationOptionV1_t;

typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtHtp_HmxBoundingInfo_CreateFn_t create;
  QairtHtp_HmxBoundingInfo_FreeFn_t free;
  QairtHtp_HmxBoundingInfo_SetTargetHmxFreqHzFn_t setTargetHmxFreqHz;
  QairtHtp_HmxBoundingInfo_GetTargetHmxFreqHzFn_t getTargetHmxFreqHz;
  QairtHtp_HmxBoundingInfo_SetTargetDspCoreFreqFn_t setTargetDspCoreFreq;
  QairtHtp_HmxBoundingInfo_GetTargetDspCoreFreqFn_t getTargetDspCoreFreq;
} QairtHtp_HmxBoundingInfoV1_t;

/**
 * @struct QairtHtpGraph_CustomConfigV1_t
 * @brief This struct contains the functions that define the V1 interface for
 *        QAIRT HTP Graph Custom Config. This interface can be retrieved through
 *        the id QAIRT_HTP_GRAPH_CUSTOM_CONFIG_ID.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtHtpGraph_CustomConfig_CreateFn_t create;
  QairtHtpGraph_CustomConfig_FreeFn_t free;
  QairtHtpGraph_CustomConfig_SetOptimizationOptionFn_t setOptimizationOption;
  QairtHtpGraph_CustomConfig_GetOptimizationOptionFn_t getOptimizationOption;
  QairtHtpGraph_CustomConfig_SetPrecisionFn_t setPrecision;
  QairtHtpGraph_CustomConfig_GetPrecisionFn_t getPrecision;
  QairtHtpGraph_CustomConfig_SetVtcmSizeInMBFn_t setVtcmSizeInMB;
  QairtHtpGraph_CustomConfig_GetVtcmSizeInMBFn_t getVtcmSizeInMB;
  QairtHtpGraph_CustomConfig_SetVtcmSizeInBytesFn_t setVtcmSizeInBytes;
  QairtHtpGraph_CustomConfig_GetVtcmSizeInBytesFn_t getVtcmSizeInBytes;
  QairtHtpGraph_CustomConfig_SetFoldReluActivationIntoConvOffFn_t setFoldReluActivationIntoConvOff;
  QairtHtpGraph_CustomConfig_GetFoldReluActivationIntoConvOffFn_t getFoldReluActivationIntoConvOff;
  QairtHtpGraph_CustomConfig_SetShortDepthConvOnHmxOffFn_t setShortDepthConvOnHmxOff;
  QairtHtpGraph_CustomConfig_GetShortDepthConvOnHmxOffFn_t getShortDepthConvOnHmxOff;
  QairtHtpGraph_CustomConfig_SetNumHvxThreadsFn_t setNumHvxThreads;
  QairtHtpGraph_CustomConfig_GetNumHvxThreadsFn_t getNumHvxThreads;
  QairtHtpGraph_CustomConfig_SetNumCoresFn_t setNumCores;
  QairtHtpGraph_CustomConfig_GetNumCoresFn_t getNumCores;
  QairtHtpGraph_CustomConfig_SetHmxBoundingInfoFn_t setHmxBoundingInfo;
  QairtHtpGraph_CustomConfig_GetHmxBoundingInfoFn_t getHmxBoundingInfo;
  QairtHtpGraph_CustomConfig_SetWeightsPackingFn_t setWeightsPacking;
  QairtHtpGraph_CustomConfig_GetWeightsPackingFn_t getWeightsPacking;
  QairtHtpGraph_CustomConfig_SetAssumeSameQuantFn_t setAssumeSameQuant;
  QairtHtpGraph_CustomConfig_GetAssumeSameQuantFn_t getAssumeSameQuant;
  QairtHtpGraph_CustomConfig_SetShareIOBufferFn_t setShareIOBuffer;
  QairtHtpGraph_CustomConfig_GetShareIOBufferFn_t getShareIOBuffer;
  QairtHtpGraph_CustomConfig_SetAdvancedActivationFusionFn_t setAdvancedActivationFusion;
  QairtHtpGraph_CustomConfig_GetAdvancedActivationFusionFn_t getAdvancedActivationFusion;
  QairtHtpGraph_CustomConfig_SetHighPrecisionSigmoidFn_t setHighPrecisionSigmoid;
  QairtHtpGraph_CustomConfig_GetHighPrecisionSigmoidFn_t getHighPrecisionSigmoid;
  QairtHtpGraph_CustomConfig_SetMonolithicLstmFn_t setMonolithicLstm;
  QairtHtpGraph_CustomConfig_GetMonolithicLstmFn_t getMonolithicLstm;
} QairtHtpGraph_CustomConfigV1_t;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // QAIRT_HTP_GRAPH_H

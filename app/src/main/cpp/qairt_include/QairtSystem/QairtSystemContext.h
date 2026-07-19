//==============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All Rights Reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//==============================================================================

/**
 * @file
 * @brief   System Context component API
 *
 *          A header which contains the QAIRT System Context
 *          components and the interfaces it implements.
 */

#ifndef QAISW_QAIRTSYSTEMCONTEXT_H
#define QAISW_QAIRTSYSTEMCONTEXT_H

#include "QairtDevice/QairtDevicePlatformInfo.h"
#include "QairtSystemCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
// Macros
//=============================================================================
#define QAIRT_SYSTEM_CONTEXT_V1_ID                                      QAIRT_MIN_ID_SYSTEM
#define QAIRT_SYSTEM_CONTEXT_BINARY_INFO_V1_ID                          QAIRT_MIN_ID_SYSTEM + 1
#define QAIRT_SYSTEM_CONTEXT_GRAPH_INFO_V1_ID                           QAIRT_MIN_ID_SYSTEM + 2
#define QAIRT_SYSTEM_CONTEXT_TENSOR_INFO_V1_ID                          QAIRT_MIN_ID_SYSTEM + 3
#define QAIRT_SYSTEM_CONTEXT_QUANT_INFO_V1_ID                           QAIRT_MIN_ID_SYSTEM + 4
#define QAIRT_SYSTEM_CONTEXT_GRAPH_INFO_SET_V1_ID                       QAIRT_MIN_ID_SYSTEM + 5
#define QAIRT_SYSTEM_CONTEXT_QUANTIZE_PARAMS_SCALE_OFFSET_V1_ID         QAIRT_MIN_ID_SYSTEM + 6
#define QAIRT_SYSTEM_CONTEXT_QUANTIZE_PARAMS_AXIS_SCALE_OFFSET_V1_ID    QAIRT_MIN_ID_SYSTEM + 7
#define QAIRT_SYSTEM_CONTEXT_QUANTIZE_PARAMS_BW_SCALE_OFFSET_V1_ID      QAIRT_MIN_ID_SYSTEM + 8
#define QAIRT_SYSTEM_CONTEXT_QUANTIZE_PARAMS_BW_AXIS_SCALE_OFFSET_V1_ID QAIRT_MIN_ID_SYSTEM + 9
#define QAIRT_SYSTEM_CONTEXT_QUANTIZE_PARAMS_BW_AXIS_SCALE_OFFSET_MAPPED_V1_ID \
  QAIRT_MIN_ID_SYSTEM + 10

//=============================================================================
// Data Types
//=============================================================================
/**
 * @brief QAIRT System Context API result / error codes.
 */
typedef enum {
  QAIRT_SYSTEM_CONTEXT_MINERROR = QAIRT_MIN_ERROR_SYSTEM,
  //////////////////////////////////////////

  /// QAIRT System Context success
  QAIRT_SYSTEM_CONTEXT_NO_ERROR = QAIRT_SYSTEM_COMMON_NO_ERROR,
  /// There is optional API component that is not supported yet.
  QAIRT_SYSTEM_CONTEXT_ERROR_UNSUPPORTED_FEATURE = QAIRT_SYSTEM_COMMON_ERROR_UNSUPPORTED_FEATURE,
  /// QAIRT System Context invalid handle
  QAIRT_SYSTEM_CONTEXT_ERROR_INVALID_HANDLE = QAIRT_SYSTEM_COMMON_ERROR_INVALID_HANDLE,
  /// One or more arguments to a System Context API is/are NULL/invalid.
  QAIRT_SYSTEM_CONTEXT_ERROR_INVALID_ARGUMENT = QAIRT_SYSTEM_COMMON_ERROR_INVALID_ARGUMENT,
  /// Generic Failure in achieving the objective of a System Context API
  QAIRT_SYSTEM_CONTEXT_ERROR_OPERATION_FAILED = 30002,

  // Errors related to context caching
  /// Malformed context binary
  QAIRT_SYSTEM_CONTEXT_ERROR_MALFORMED_BINARY = 30010,

  //////////////////////////////////////////
  QAIRT_SYSTEM_CONTEXT_MAXERROR = QAIRT_MAX_ERROR_SYSTEM
} QairtSystemContext_Error_t;

typedef enum {
  /// Indicates per-tensor scale-offset encoding type.
  QAIRT_QUANTIZATION_INFO_SCALE_OFFSET = 0,
  /// Indicates per-axis (e.g. per-channel) scale-offset encoding type.
  QAIRT_QUANTIZATION_INFO_AXIS_SCALE_OFFSET           = 1,
  QAIRT_QUANTIZATION_INFO_BW_SCALE_OFFSET             = 2,
  QAIRT_QUANTIZATION_INFO_BW_AXIS_SCALE_OFFSET        = 3,
  QAIRT_QUANTIZATION_INFO_BW_AXIS_SCALE_OFFSET_MAPPED = 8,
  QAIRT_QUANTIZATION_INFO_UNDEFINED                   = 0x7FFFFFFF
} Qairt_QuantizationInfoType_t;

/**
 * @brief An enum to specify quantized value mapping scheme
 */
typedef enum {
  /// Indicates standard symmetric 2's compliment mapping
  QAIRT_QUANTIZATION_INFO_ENCODING_MAPPING_STANDARD_SYMMETRIC = 0,
  /// Indicates 2's compliment mapping with a positive shift of one
  QAIRT_QUANTIZATION_INFO_ENCODING_MAPPING_ASYMMETRIC_PLUS_ONE = 1,
  /// Indicates linear mapping symmetric about zero, but excluding zero from the range
  QAIRT_QUANTIZATION_INFO_ENCODING_MAPPING_LINEAR_SYMMETRIC_EXCLUDE_ZERO = 2,
  // Unused, present to ensure 32 bits.
  QAIRT_QUANTIZATION_INFO_ENCODING_MAPPING_UNDEFINED = 0x7FFFFFFF
} Qairt_QuantizationInfoEncodingMapping_t;

typedef struct _QairtSystem_Context_Handle_t* QairtSystem_Context_Handle_t;
typedef struct _QairtSystem_Context_BinaryInfoHandle_t* QairtSystem_Context_BinaryInfoHandle_t;
typedef struct _QairtSystem_Context_GraphInfoHandle_t* QairtSystem_Context_GraphInfoHandle_t;
typedef struct _QairtSystem_Context_TensorInfoHandle_t* QairtSystem_Context_TensorInfoHandle_t;
typedef struct _QairtSystem_Context_QuantInfoHandle_t* QairtSystem_Context_QuantInfoHandle_t;
typedef struct _QairtSystem_Context_QuantizeParams_ScaleOffsetHandle_t*
    QairtSystem_Context_QuantizeParams_ScaleOffsetHandle_t;
typedef struct _QairtSystem_Context_QuantizeParams_AxisScaleOffsetHandle_t*
    QairtSystem_Context_QuantizeParams_AxisScaleOffsetHandle_t;
typedef struct _QairtSystem_Context_QuantizeParams_BwScaleOffsetHandle_t*
    QairtSystem_Context_QuantizeParams_BwScaleOffsetHandle_t;
typedef struct _QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetHandle_t*
    QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetHandle_t;
typedef struct _QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMappedHandle_t*
    QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMappedHandle_t;

/**
 * @brief A handle to a set of graph information.
 */
typedef struct _QairtSystem_Context_GraphInfoSetHandle_t* QairtSystem_Context_GraphInfoSetHandle_t;

//=============================================================================
// Public Functions
//=============================================================================

/// System Context

/**
 * @brief Create a system context handle.
 *
 * @param[out] sysContext A handle to the created system context.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _sysContext_ is NULL
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory allocation error while
 *           creating system context
 *
 * @note Use corresponding API through QairtSystem_ContextV1_t.
 */
Qairt_Status_t QairtSystem_Context_create(QairtSystem_Context_Handle_t* sysContext);

/**
 * @brief Get binary information from a context binary buffer.
 *
 * @param[in] sysContext A handle to the system context.
 *
 * @param[in] binaryBuffer A pointer to the context binary buffer.
 *
 * @param[in] bufferSize The size of the context binary buffer in bytes.
 *
 * @param[out] binaryInfo A handle to the retrieved binary information.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _sysContext_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _binaryBuffer_ or _binaryInfo_
 *           is NULL, or _bufferSize_ is 0
 *         - QAIRT_SYSTEM_CONTEXT_ERROR_MALFORMED_BINARY: _binaryBuffer_ does not
 *           contain a valid context binary
 *
 * @note Use corresponding API through QairtSystem_ContextV1_t.
 */
Qairt_Status_t QairtSystem_Context_getBinaryInfo(
    QairtSystem_Context_Handle_t sysContext,
    const void* binaryBuffer,
    uint64_t bufferSize,
    QairtSystem_Context_BinaryInfoHandle_t* binaryInfo);

/**
 * @brief Free a system context handle.
 *
 * @param[in] sysContext A handle to the system context to be freed.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _sysContext_ is not a valid handle
 *
 * @note Use corresponding API through QairtSystem_ContextV1_t.
 */
Qairt_Status_t QairtSystemContext_free(QairtSystem_Context_Handle_t sysContext);

/// Binary Context Info

/**
 * @brief Get the backend ID from binary information.
 *
 * @param[in] info A handle to the binary information.
 *
 * @param[out] backendId The retrieved backend ID.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _info_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _backendId_ is NULL
 *
 * @note Use corresponding API through QairtSystem_Context_BinaryInfoV1_t.
 */
Qairt_Status_t QairtSystem_Context_BinaryInfo_getBackendId(
    QairtSystem_Context_BinaryInfoHandle_t info, uint32_t* backendId);

/**
 * @brief Get the build ID string from binary information.
 *
 * @param[in] info A handle to the binary information.
 *
 * @param[out] buildId The retrieved build ID string. The string is owned by
 *             the binary info handle and must not be freed by the caller.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _info_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _buildId_ is NULL
 *
 * @note Use corresponding API through QairtSystem_Context_BinaryInfoV1_t.
 */
Qairt_Status_t QairtSystem_Context_BinaryInfo_getBuildId(
    QairtSystem_Context_BinaryInfoHandle_t info, const char** buildId);

/**
 * @brief Get the API version from binary information.
 *
 * @param[in] info A handle to the binary information.
 *
 * @param[out] major The retrieved major version number.
 *
 * @param[out] minor The retrieved minor version number.
 *
 * @param[out] patch The retrieved patch version number.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _info_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _major_, _minor_, or _patch_
 *           is NULL
 *
 * @note Use corresponding API through QairtSystem_Context_BinaryInfoV1_t.
 */
Qairt_Status_t QairtSystem_Context_BinaryInfo_getApiVersion(
    QairtSystem_Context_BinaryInfoHandle_t info, uint32_t* major, uint32_t* minor, uint32_t* patch);

/**
 * @brief Get the backend API version from binary information.
 *
 * @param[in] info A handle to the binary information.
 *
 * @param[out] major The retrieved major version number.
 *
 * @param[out] minor The retrieved minor version number.
 *
 * @param[out] patch The retrieved patch version number.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _info_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _major_, _minor_, or _patch_
 *           is NULL
 *
 * @note Use corresponding API through QairtSystem_Context_BinaryInfoV1_t.
 */
Qairt_Status_t QairtSystem_Context_BinaryInfo_getBackendApiVersion(
    QairtSystem_Context_BinaryInfoHandle_t info, uint32_t* major, uint32_t* minor, uint32_t* patch);

/**
 * @brief Get the number of SoC versions from binary information.
 *
 * @param[in] info A handle to the binary information.
 *
 * @param[out] numVersions The retrieved number of SoC versions.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _info_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _numVersions_ is NULL
 *
 * @note Use corresponding API through QairtSystem_Context_BinaryInfoV1_t.
 */
Qairt_Status_t QairtSystem_Context_BinaryInfo_getNumSocVersions(
    QairtSystem_Context_BinaryInfoHandle_t info, uint32_t* numVersions);

/**
 * @brief Get the SoC version string at an index from binary information.
 *
 * @param[in] info A handle to the binary information.
 *
 * @param[in] index Index of the SoC version to retrieve.
 *
 * @param[out] socVersion The retrieved SoC version string. The string is owned
 *             by the binary info handle and must not be freed by the caller.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _info_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _socVersion_ is NULL or
 *           _index_ is greater than or equal to the number of SoC versions
 *
 * @note Use corresponding API through QairtSystem_Context_BinaryInfoV1_t.
 */
Qairt_Status_t QairtSystem_Context_BinaryInfo_getSocVersionAt(
    QairtSystem_Context_BinaryInfoHandle_t info, uint32_t index, const char** socVersion);

/**
 * @brief Get the number of graph infos from binary information.
 *
 * @param[in] info A handle to the binary information.
 *
 * @param[out] numGraphInfos The retrieved number of graph infos.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _info_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _numGraphInfos_ is NULL
 *
 * @note Use corresponding API through QairtSystem_Context_BinaryInfoV1_t.
 */
Qairt_Status_t QairtSystem_Context_BinaryInfo_getNumGraphInfos(
    QairtSystem_Context_BinaryInfoHandle_t info, uint32_t* numGraphInfos);

/**
 * @brief Get the graph info at an index from binary information.
 *
 * @param[in] info A handle to the binary information.
 *
 * @param[in] index Index of the graph info to retrieve.
 *
 * @param[out] graphInfo A handle to the retrieved graph information.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _info_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _graphInfo_ is NULL or
 *           _index_ is greater than or equal to the number of graph infos
 *
 * @note Use corresponding API through QairtSystem_Context_BinaryInfoV1_t.
 */
Qairt_Status_t QairtSystem_Context_BinaryInfo_getGraphInfoAt(
    QairtSystem_Context_BinaryInfoHandle_t info,
    uint32_t index,
    QairtSystem_Context_GraphInfoHandle_t* graphInfo);

/**
 * @brief Get the device platform information from binary information.
 *
 * @param[in] info A handle to the binary information.
 *
 * @param[out] platform A handle to the retrieved device platform information.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _info_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _platform_ is NULL
 *
 * @note Use corresponding API through QairtSystem_Context_BinaryInfoV1_t.
 */
Qairt_Status_t QairtSystem_Context_BinaryInfo_getDevicePlatformInfo(
    QairtSystem_Context_BinaryInfoHandle_t info, QairtDevice_PlatformInfoHandle_t* platform);

/**
 * @brief Get the SoC model identifier from binary information.
 *
 * @param[in] info A handle to the binary information.
 *
 * @param[out] socModel The retrieved SoC model identifier.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _info_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _socModel_ is NULL
 *
 * @note Use corresponding API through QairtSystem_Context_BinaryInfoV1_t.
 */
Qairt_Status_t QairtSystem_Context_BinaryInfo_getSocModel(
    QairtSystem_Context_BinaryInfoHandle_t info, uint32_t* socModel);

/// Graph Info

/**
 * @brief Get the name of a graph.
 *
 * @param[in] graphInfo A handle to the graph information.
 *
 * @param[out] name The retrieved graph name string. The string is owned by
 *             the graph info handle and must not be freed by the caller.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _graphInfo_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _name_ is NULL
 *
 * @note Use corresponding API through QairtSystem_Context_GraphInfoV1_t.
 */
Qairt_Status_t QairtSystem_Context_GraphInfo_getName(
    QairtSystem_Context_GraphInfoHandle_t graphInfo, const char** name);

/**
 * @brief Get the number of inputs for a graph.
 *
 * @param[in] graphInfo A handle to the graph information.
 *
 * @param[out] numInputs The retrieved number of graph inputs.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _graphInfo_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _numInputs_ is NULL
 *
 * @note Use corresponding API through QairtSystem_Context_GraphInfoV1_t.
 */
Qairt_Status_t QairtSystem_Context_GraphInfo_getNumInputs(
    QairtSystem_Context_GraphInfoHandle_t graphInfo, uint32_t* numInputs);

/**
 * @brief Get the input tensor info at an index for a graph.
 *
 * @param[in] graphInfo A handle to the graph information.
 *
 * @param[in] index Index of the input tensor to retrieve.
 *
 * @param[out] inputs A handle to the retrieved input tensor information.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _graphInfo_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _inputs_ is NULL or _index_
 *           is greater than or equal to the number of inputs
 *
 * @note Use corresponding API through QairtSystem_Context_GraphInfoV1_t.
 */
Qairt_Status_t QairtSystem_Context_GraphInfo_getInputAt(
    QairtSystem_Context_GraphInfoHandle_t graphInfo,
    uint32_t index,
    QairtSystem_Context_TensorInfoHandle_t* inputs);

/**
 * @brief Get the number of outputs for a graph.
 *
 * @param[in] graphInfo A handle to the graph information.
 *
 * @param[out] numOutputs The retrieved number of graph outputs.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _graphInfo_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _numOutputs_ is NULL
 *
 * @note Use corresponding API through QairtSystem_Context_GraphInfoV1_t.
 */
Qairt_Status_t QairtSystem_Context_GraphInfo_getNumOutputs(
    QairtSystem_Context_GraphInfoHandle_t graphInfo, uint32_t* numOutputs);

/**
 * @brief Get the output tensor info at an index for a graph.
 *
 * @param[in] graphInfo A handle to the graph information.
 *
 * @param[in] index Index of the output tensor to retrieve.
 *
 * @param[out] output A handle to the retrieved output tensor information.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _graphInfo_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _output_ is NULL or _index_
 *           is greater than or equal to the number of outputs
 *
 * @note Use corresponding API through QairtSystem_Context_GraphInfoV1_t.
 */
Qairt_Status_t QairtSystem_Context_GraphInfo_getOutputAt(
    QairtSystem_Context_GraphInfoHandle_t graphInfo,
    uint32_t index,
    QairtSystem_Context_TensorInfoHandle_t* output);

/**
 * @brief Get the number of updateable tensors for a graph.
 *
 * @param[in] graphInfo A handle to the graph information.
 *
 * @param[out] numUpdateables The retrieved number of updateable tensors.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _graphInfo_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _numUpdateables_ is NULL
 *
 * @note Use corresponding API through QairtSystem_Context_GraphInfoV1_t.
 */
Qairt_Status_t QairtSystem_Context_GraphInfo_getNumUpdateables(
    QairtSystem_Context_GraphInfoHandle_t graphInfo, uint32_t* numUpdateables);

/**
 * @brief Get the updateable tensor info at an index for a graph.
 *
 * @param[in] graphInfo A handle to the graph information.
 *
 * @param[in] index Index of the updateable tensor to retrieve.
 *
 * @param[out] updateable A handle to the retrieved updateable tensor information.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _graphInfo_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _updateable_ is NULL or _index_
 *           is greater than or equal to the number of updateable tensors
 *
 * @note Use corresponding API through QairtSystem_Context_GraphInfoV1_t.
 */
Qairt_Status_t QairtSystem_Context_GraphInfo_getUpdateableAt(
    QairtSystem_Context_GraphInfoHandle_t graphInfo,
    uint32_t index,
    QairtSystem_Context_TensorInfoHandle_t* updateable);

/**
 * @brief Get the graph info blob size for a graph.
 *
 * @param[in] graphInfo A handle to the graph information.
 *
 * @param[out] size The retrieved graph info blob size in bytes.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _graphInfo_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _size_ is NULL
 *
 * @note Use corresponding API through QairtSystem_Context_GraphInfoV1_t.
 */
Qairt_Status_t QairtSystem_Context_GraphInfo_getGraphInfoBlobSize(
    QairtSystem_Context_GraphInfoHandle_t graphInfo, uint32_t* size);

/**
 * @brief Get the graph info blob for a graph.
 *
 * @param[in] graphInfo A handle to the graph information.
 *
 * @param[out] graphInfoBlob A pointer to the retrieved graph info blob. The
 *             memory is owned by the graph info handle and must not be freed
 *             by the caller.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _graphInfo_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _graphInfoBlob_ is NULL
 *
 * @note Use corresponding API through QairtSystem_Context_GraphInfoV1_t.
 */
Qairt_Status_t QairtSystem_Context_GraphInfo_getGraphInfoBlob(
    QairtSystem_Context_GraphInfoHandle_t graphInfo, void** graphInfoBlob);

/**
 * @brief Get the start operation index for a graph.
 *
 * @param[in] graphInfo A handle to the graph information.
 *
 * @param[out] start The retrieved start operation index.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _graphInfo_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _start_ is NULL
 *
 * @note Use corresponding API through QairtSystem_Context_GraphInfoV1_t.
 */
Qairt_Status_t QairtSystem_Context_GraphInfo_getStartOpIndex(
    QairtSystem_Context_GraphInfoHandle_t graphInfo, uint32_t* start);

/**
 * @brief Get the end operation index for a graph.
 *
 * @param[in] graphInfo A handle to the graph information.
 *
 * @param[out] end The retrieved end operation index.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _graphInfo_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _end_ is NULL
 *
 * @note Use corresponding API through QairtSystem_Context_GraphInfoV1_t.
 */
Qairt_Status_t QairtSystem_Context_GraphInfo_getEndOpIndex(
    QairtSystem_Context_GraphInfoHandle_t graphInfo, uint32_t* end);

/// Graph Info Set

/**
 * @brief Get the number of graph infos in a set.
 *
 * @param[in] graphInfoSetHandle  A handle to the graph info set.
 *
 * @param[out] numGraphInfos The retrieved number of graph infos.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _graphInfoSetHandle_ is
 *           not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _numGraphInfos_ is
 *           NULL
 *
 * @note Use corresponding API through QairtSystem_Context_GraphInfoSetHandle_t.
 */
Qairt_Status_t QairtSystem_Context_GraphInfoSet_getNumGraphInfos(
    QairtSystem_Context_GraphInfoSetHandle_t graphInfoSetHandle, uint32_t* numGraphInfos);

/**
 * @brief Get the graph info at an index.
 *
 * @param[in] graphInfoSetHandle  A handle to the graph info set.
 *
 * @param[in] index Index of graph info that should be retrieved.
 *
 * @param[out] graphInfoHandle The retrieved graph info handle.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _graphInfoSetHandle_ is
 *           not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _graphInfoHandle_ is
 *           NULL or _index_ is greater than number of graph infos in the set.
 *
 * @note Use corresponding API through QairtSystem_Context_GraphInfoSetHandle_t.
 */
Qairt_Status_t QairtSystem_Context_GraphInfoSet_getGraphInfoAt(
    QairtSystem_Context_GraphInfoSetHandle_t graphInfoSetHandle,
    uint32_t index,
    QairtSystem_Context_GraphInfoHandle_t* graphInfoHandle);

/// Tensor Info
/// Id, Name, Quant Params, Rank, Dimensions, isDynamicDimensions

/**
 * @brief Get the ID of a tensor.
 *
 * @param[in] tensor A handle to the tensor information.
 *
 * @param[out] id The retrieved tensor ID.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _tensor_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _id_ is NULL
 *
 * @note Use corresponding API through QairtSystem_Context_TensorInfoV1_t.
 */
Qairt_Status_t QairtSystem_Context_TensorInfo_getId(QairtSystem_Context_TensorInfoHandle_t tensor,
                                                    uint64_t* id);

/**
 * @brief Get the name of a tensor.
 *
 * @param[in] tensor A handle to the tensor information.
 *
 * @param[out] name The retrieved tensor name string. The string is owned by
 *             the tensor info handle and must not be freed by the caller.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _tensor_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _name_ is NULL
 *
 * @note Use corresponding API through QairtSystem_Context_TensorInfoV1_t.
 */
Qairt_Status_t QairtSystem_Context_TensorInfo_getName(QairtSystem_Context_TensorInfoHandle_t tensor,
                                                      const char** name);

/**
 * @brief Get the rank of a tensor.
 *
 * @param[in] tensor A handle to the tensor information.
 *
 * @param[out] rank The retrieved tensor rank (number of dimensions).
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _tensor_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _rank_ is NULL
 *
 * @note Use corresponding API through QairtSystem_Context_TensorInfoV1_t.
 */
Qairt_Status_t QairtSystem_Context_TensorInfo_getRank(QairtSystem_Context_TensorInfoHandle_t tensor,
                                                      uint32_t* rank);

/**
 * @brief Get the dimensions of a tensor.
 *
 * @param[in] tensor A handle to the tensor information.
 *
 * @param[out] dimensions A pointer to the retrieved array of dimension sizes.
 *             The array is owned by the tensor info handle and must not be
 *             freed by the caller. The number of elements equals the tensor rank.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _tensor_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _dimensions_ is NULL
 *
 * @note Use corresponding API through QairtSystem_Context_TensorInfoV1_t.
 */
Qairt_Status_t QairtSystem_Context_TensorInfo_getDimensions(
    QairtSystem_Context_TensorInfoHandle_t tensor, const uint32_t** dimensions);

/**
 * @brief Get the data type of a tensor.
 *
 * @param[in] tensor A handle to the tensor information.
 *
 * @param[out] datatype The retrieved tensor data type.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _tensor_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _datatype_ is NULL
 *
 * @note Use corresponding API through QairtSystem_Context_TensorInfoV1_t.
 */
Qairt_Status_t QairtSystem_Context_TensorInfo_getDatatype(
    QairtSystem_Context_TensorInfoHandle_t tensor, Qairt_DataType_t* datatype);

/**
 * @brief Get the quantization information for a tensor.
 *
 * @param[in] tensor A handle to the tensor information.
 *
 * @param[out] quantInfo A handle to the retrieved quantization information.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _tensor_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _quantInfo_ is NULL
 *
 * @note Use corresponding API through QairtSystem_Context_TensorInfoV1_t.
 */
Qairt_Status_t QairtSystem_Context_TensorInfo_getQuantInfo(
    QairtSystem_Context_TensorInfoHandle_t tensor,
    QairtSystem_Context_QuantInfoHandle_t* quantInfo);

/// Quant Info

/**
 * @brief Get the quantization type from quantization parameter information.
 *
 * @param[in] quantInfo A handle to the quantization parameter information.
 *
 * @param[out] type The retrieved quantization type.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _quantInfo_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _type_ is NULL
 *
 * @note Use corresponding API through QairtSystem_Context_QuantParamInfoV1_t.
 */
Qairt_Status_t QairtSystem_Context_QuantParamInfo_getQuantizationType(
    QairtSystem_Context_QuantInfoHandle_t quantInfo, Qairt_QuantizationInfoType_t* type);

/**
 * @brief Get the per-tensor scale-offset quantization parameters.
 *
 * @param[in] quantInfo A handle to the quantization parameter information.
 *
 * @param[out] scaleOffsetHandle A handle to the retrieved scale-offset
 *             quantization parameters.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _quantInfo_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _scaleOffsetHandle_ is NULL
 *
 * @note Use corresponding API through QairtSystem_Context_QuantParamInfoV1_t.
 */
Qairt_Status_t QairtSystem_Context_QuantParamInfo_getScaleOffset(
    QairtSystem_Context_QuantInfoHandle_t quantInfo,
    QairtSystem_Context_QuantizeParams_ScaleOffsetHandle_t* scaleOffsetHandle);

/**
 * @brief Get the per-axis scale-offset quantization parameters.
 *
 * @param[in] quantInfo A handle to the quantization parameter information.
 *
 * @param[out] axisScaleOffsetHandle A handle to the retrieved axis scale-offset
 *             quantization parameters.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _quantInfo_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _axisScaleOffsetHandle_ is NULL
 *
 * @note Use corresponding API through QairtSystem_Context_QuantParamInfoV1_t.
 */
Qairt_Status_t QairtSystem_Context_QuantParamInfo_getAxisScaleOffset(
    QairtSystem_Context_QuantInfoHandle_t quantInfo,
    QairtSystem_Context_QuantizeParams_AxisScaleOffsetHandle_t* axisScaleOffsetHandle);

/**
 * @brief Get the per-tensor bit-width scale-offset quantization parameters.
 *
 * @param[in] quantInfo A handle to the quantization parameter information.
 *
 * @param[out] BwScaleOffsetHandle A handle to the retrieved bit-width scale-offset
 *             quantization parameters.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _quantInfo_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _BwScaleOffsetHandle_ is NULL
 *
 * @note Use corresponding API through QairtSystem_Context_QuantParamInfoV1_t.
 */
Qairt_Status_t QairtSystem_Context_QuantParamInfo_getBwScaleOffset(
    QairtSystem_Context_QuantInfoHandle_t quantInfo,
    QairtSystem_Context_QuantizeParams_BwScaleOffsetHandle_t* BwScaleOffsetHandle);

/**
 * @brief Get the per-axis bit-width scale-offset quantization parameters.
 *
 * @param[in] quantInfo A handle to the quantization parameter information.
 *
 * @param[out] BwAxisScaleOffsetHandle A handle to the retrieved bit-width axis
 *             scale-offset quantization parameters.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _quantInfo_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _BwAxisScaleOffsetHandle_ is NULL
 *
 * @note Use corresponding API through QairtSystem_Context_QuantParamInfoV1_t.
 */
Qairt_Status_t QairtSystem_Context_QuantParamInfo_getBwAxisScaleOffset(
    QairtSystem_Context_QuantInfoHandle_t quantInfo,
    QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetHandle_t* BwAxisScaleOffsetHandle);

/**
 * @brief Get the per-axis bit-width scale-offset mapped quantization parameters.
 *
 * @param[in] quantInfo A handle to the quantization parameter information.
 *
 * @param[out] BwAxisScaleOffsetMappedHandle A handle to the retrieved bit-width
 *             axis scale-offset mapped quantization parameters.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _quantInfo_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _BwAxisScaleOffsetMappedHandle_
 *           is NULL
 *
 * @note Use corresponding API through QairtSystem_Context_QuantParamInfoV1_t.
 */
Qairt_Status_t QairtSystem_Context_QuantParamInfo_getBwAxisScaleOffsetMapped(
    QairtSystem_Context_QuantInfoHandle_t quantInfo,
    QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMappedHandle_t*
        BwAxisScaleOffsetMappedHandle);

// ScaleOffset Functions

/**
 * @brief Create a scale-offset handle for per-tensor quantization.
 *
 * @param[out] scaleOffsetHandle A handle to the created scale-offset.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _scaleOffsetHandle_ is
 *           NULL
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory allocation error while
 *           creating scale-offset
 *
 * @note Use corresponding API through QairtQuantizeParams_ScaleOffsetV1_t.
 */
Qairt_Status_t QairtSystem_Context_QuantizeParams_ScaleOffset_create(
    QairtSystem_Context_QuantizeParams_ScaleOffsetHandle_t* scaleOffsetHandle);

/**
 * @brief Free a scale-offset handle.
 *
 * @param[in] scaleOffsetHandle A handle to the scale-offset to be freed.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _scaleOffsetHandle_ is not
 *           a valid handle
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory deallocation error
 *
 * @note Use corresponding API through QairtQuantizeParams_ScaleOffsetV1_t.
 */
Qairt_Status_t QairtSystem_Context_QuantizeParams_ScaleOffset_free(
    QairtSystem_Context_QuantizeParams_ScaleOffsetHandle_t scaleOffsetHandle);

/**
 * @brief Get the scale value from a scale-offset.
 *
 * @param[in] scaleOffsetHandle A handle to the scale-offset.
 *
 * @param[out] scale The retrieved scale value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _scaleOffsetHandle_ is not
 *           a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _scale_ is NULL
 *
 * @note Use corresponding API through QairtQuantizeParams_ScaleOffsetV1_t.
 */
Qairt_Status_t QairtSystem_Context_QuantizeParams_ScaleOffset_getScale(
    QairtSystem_Context_QuantizeParams_ScaleOffsetHandle_t scaleOffsetHandle, float* scale);

/**
 * @brief Get the offset value from a scale-offset.
 *
 * @param[in] scaleOffsetHandle A handle to the scale-offset.
 *
 * @param[out] offset The retrieved offset value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _scaleOffsetHandle_ is not
 *           a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _offset_ is NULL
 *
 * @note Use corresponding API through QairtQuantizeParams_ScaleOffsetV1_t.
 */
Qairt_Status_t QairtSystem_Context_QuantizeParams_ScaleOffset_getOffset(
    QairtSystem_Context_QuantizeParams_ScaleOffsetHandle_t scaleOffsetHandle, int32_t* offset);

// AxisScaleOffset Functions

/**
 * @brief Create an axis scale-offset handle for per-axis quantization.
 *
 * @param[out] axisScaleOffsetHandle A handle to the created axis scale-offset.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _axisScaleOffsetHandle_
 *           is NULL
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory allocation error while
 *           creating axis scale-offset
 *
 * @note Use corresponding API through QairtQuantizeParams_AxisScaleOffsetV1_t.
 */
Qairt_Status_t QairtSystem_Context_QuantizeParams_AxisScaleOffset_create(
    QairtSystem_Context_QuantizeParams_AxisScaleOffsetHandle_t* axisScaleOffsetHandle);

/**
 * @brief Free an axis scale-offset handle.
 *
 * @param[in] axisScaleOffsetHandle A handle to the axis scale-offset to be freed.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _axisScaleOffsetHandle_ is not
 *           a valid handle
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory deallocation error
 *
 * @note Use corresponding API through QairtQuantizeParams_AxisScaleOffsetV1_t.
 */
Qairt_Status_t QairtSystem_Context_QuantizeParams_AxisScaleOffset_free(
    QairtSystem_Context_QuantizeParams_AxisScaleOffsetHandle_t axisScaleOffsetHandle);

/**
 * @brief Get the number of scale-offsets in an axis scale-offset.
 *
 * @param[in] axisScaleOffsetHandle A handle to the axis scale-offset.
 *
 * @param[out] numScaleOffsets The retrieved number of scale-offsets.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _axisScaleOffsetHandle_ is not
 *           a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _numScaleOffsets_ is NULL
 *
 * @note Use corresponding API through QairtQuantizeParams_AxisScaleOffsetV1_t.
 */
Qairt_Status_t QairtSystem_Context_QuantizeParams_AxisScaleOffset_getNumScaleOffsets(
    QairtSystem_Context_QuantizeParams_AxisScaleOffsetHandle_t axisScaleOffsetHandle,
    uint32_t* numScaleOffsets);

/**
 * @brief Get the scale-offset at an index from an axis scale-offset.
 *
 * @param[in] axisScaleOffsetHandle A handle to the axis scale-offset.
 *
 * @param[in] index Index of the scale-offset to retrieve.
 *
 * @param[out] scaleOffsetHandle A handle to the retrieved scale-offset.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _axisScaleOffsetHandle_ is not
 *           a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _scaleOffsetHandle_ is NULL or
 *           _index_ is greater than or equal to the number of scale-offsets
 *
 * @note Use corresponding API through QairtQuantizeParams_AxisScaleOffsetV1_t.
 */
Qairt_Status_t QairtSystem_Context_QuantizeParams_AxisScaleOffset_getScaleOffsetAt(
    QairtSystem_Context_QuantizeParams_AxisScaleOffsetHandle_t axisScaleOffsetHandle,
    uint32_t index,
    QairtSystem_Context_QuantizeParams_ScaleOffsetHandle_t* scaleOffsetHandle);

/**
 * @brief Get the axis value from an axis scale-offset.
 *
 * @param[in] axisScaleOffsetHandle A handle to the axis scale-offset.
 *
 * @param[out] axis The retrieved axis value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _axisScaleOffsetHandle_ is not
 *           a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _axis_ is NULL
 *
 * @note Use corresponding API through QairtQuantizeParams_AxisScaleOffsetV1_t.
 */
Qairt_Status_t QairtSystem_Context_QuantizeParams_AxisScaleOffset_getAxis(
    QairtSystem_Context_QuantizeParams_AxisScaleOffsetHandle_t axisScaleOffsetHandle,
    int32_t* axis);

// BwScaleOffset Functions

/**
 * @brief Create a bit-width scale-offset handle for per-tensor quantization.
 *
 * @param[out] bwScaleOffsetHandle A handle to the created bit-width scale-offset.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _bwScaleOffsetHandle_ is NULL
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory allocation error while
 *           creating bit-width scale-offset
 *
 * @note Use corresponding API through QairtSystem_Context_QuantizeParams_BwScaleOffsetV1_t.
 */
Qairt_Status_t QairtSystem_Context_QuantizeParams_BwScaleOffset_create(
    QairtSystem_Context_QuantizeParams_BwScaleOffsetHandle_t* bwScaleOffsetHandle);

/**
 * @brief Free a bit-width scale-offset handle.
 *
 * @param[in] bwScaleOffsetHandle A handle to the bit-width scale-offset to be freed.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwScaleOffsetHandle_ is not
 *           a valid handle
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory deallocation error
 *
 * @note Use corresponding API through QairtSystem_Context_QuantizeParams_BwScaleOffsetV1_t.
 */
Qairt_Status_t QairtSystem_Context_QuantizeParams_BwScaleOffset_free(
    QairtSystem_Context_QuantizeParams_BwScaleOffsetHandle_t bwScaleOffsetHandle);

/**
 * @brief Get the scale value from a bit-width scale-offset.
 *
 * @param[in] bwScaleOffsetHandle A handle to the bit-width scale-offset.
 *
 * @param[out] scale The retrieved scale value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwScaleOffsetHandle_ is not
 *           a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _scale_ is NULL
 *
 * @note Use corresponding API through QairtSystem_Context_QuantizeParams_BwScaleOffsetV1_t.
 */
Qairt_Status_t QairtSystem_Context_QuantizeParams_BwScaleOffset_getScale(
    QairtSystem_Context_QuantizeParams_BwScaleOffsetHandle_t bwScaleOffsetHandle, float* scale);

/**
 * @brief Get the offset value from a bit-width scale-offset.
 *
 * @param[in] bwScaleOffsetHandle A handle to the bit-width scale-offset.
 *
 * @param[out] offset The retrieved offset value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwScaleOffsetHandle_ is not
 *           a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _offset_ is NULL
 *
 * @note Use corresponding API through QairtSystem_Context_QuantizeParams_BwScaleOffsetV1_t.
 */
Qairt_Status_t QairtSystem_Context_QuantizeParams_BwScaleOffset_getOffset(
    QairtSystem_Context_QuantizeParams_BwScaleOffsetHandle_t bwScaleOffsetHandle, int32_t* offset);

/**
 * @brief Get the bit-width value from a bit-width scale-offset.
 *
 * @param[in] bwScaleOffsetHandle A handle to the bit-width scale-offset.
 *
 * @param[out] bw The retrieved bit-width value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwScaleOffsetHandle_ is not
 *           a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _bw_ is NULL
 *
 * @note Use corresponding API through QairtSystem_Context_QuantizeParams_BwScaleOffsetV1_t.
 */
Qairt_Status_t QairtSystem_Context_QuantizeParams_BwScaleOffset_getBw(
    QairtSystem_Context_QuantizeParams_BwScaleOffsetHandle_t bwScaleOffsetHandle, uint32_t* bw);

// BwAxisScaleOffset Functions

/**
 * @brief Create a bit-width axis scale-offset handle for per-axis quantization.
 *
 * @param[out] bwAxisScaleOffsetHandle A handle to the created bit-width axis
 *             scale-offset.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _bwAxisScaleOffsetHandle_ is NULL
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory allocation error while
 *           creating bit-width axis scale-offset
 *
 * @note Use corresponding API through QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetV1_t.
 */
Qairt_Status_t QairtSystem_Context_QuantizeParams_BwAxisScaleOffset_create(
    QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetHandle_t* bwAxisScaleOffsetHandle);

/**
 * @brief Free a bit-width axis scale-offset handle.
 *
 * @param[in] bwAxisScaleOffsetHandle A handle to the bit-width axis scale-offset
 *            to be freed.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwAxisScaleOffsetHandle_ is not
 *           a valid handle
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory deallocation error
 *
 * @note Use corresponding API through QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetV1_t.
 */
Qairt_Status_t QairtSystem_Context_QuantizeParams_BwAxisScaleOffset_free(
    QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetHandle_t bwAxisScaleOffsetHandle);

/**
 * @brief Get the number of scales in a bit-width axis scale-offset.
 *
 * @param[in] bwAxisScaleOffsetHandle A handle to the bit-width axis scale-offset.
 *
 * @param[out] numScales The retrieved number of scales.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwAxisScaleOffsetHandle_ is not
 *           a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _numScales_ is NULL
 *
 * @note Use corresponding API through QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetV1_t.
 */
Qairt_Status_t QairtSystem_Context_QuantizeParams_BwAxisScaleOffset_getNumScales(
    QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetHandle_t bwAxisScaleOffsetHandle,
    uint32_t* numScales);

/**
 * @brief Get the scale at an index from a bit-width axis scale-offset.
 *
 * @param[in] bwAxisScaleOffsetHandle A handle to the bit-width axis scale-offset.
 *
 * @param[in] index Index of the scale to retrieve.
 *
 * @param[out] scale The retrieved scale value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwAxisScaleOffsetHandle_ is not
 *           a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _scale_ is NULL or _index_ is
 *           greater than or equal to the number of scales
 *
 * @note Use corresponding API through QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetV1_t.
 */
Qairt_Status_t QairtSystem_Context_QuantizeParams_BwAxisScaleOffset_getScaleAt(
    QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetHandle_t bwAxisScaleOffsetHandle,
    uint32_t index,
    float* scale);

/**
 * @brief Get the number of offsets in a bit-width axis scale-offset.
 *
 * @param[in] bwAxisScaleOffsetHandle A handle to the bit-width axis scale-offset.
 *
 * @param[out] numOffsets The retrieved number of offsets.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwAxisScaleOffsetHandle_ is not
 *           a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _numOffsets_ is NULL
 *
 * @note Use corresponding API through QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetV1_t.
 */
Qairt_Status_t QairtSystem_Context_QuantizeParams_BwAxisScaleOffset_getNumOffsets(
    QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetHandle_t bwAxisScaleOffsetHandle,
    uint32_t* numOffsets);

/**
 * @brief Get the offset at an index from a bit-width axis scale-offset.
 *
 * @param[in] bwAxisScaleOffsetHandle A handle to the bit-width axis scale-offset.
 *
 * @param[in] index Index of the offset to retrieve.
 *
 * @param[out] offset The retrieved offset value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwAxisScaleOffsetHandle_ is not
 *           a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _offset_ is NULL or _index_ is
 *           greater than or equal to the number of offsets
 *
 * @note Use corresponding API through QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetV1_t.
 */
Qairt_Status_t QairtSystem_Context_QuantizeParams_BwAxisScaleOffset_getOffsetAt(
    QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetHandle_t bwAxisScaleOffsetHandle,
    uint32_t index,
    int32_t* offset);

/**
 * @brief Get the axis value from a bit-width axis scale-offset.
 *
 * @param[in] bwAxisScaleOffsetHandle A handle to the bit-width axis scale-offset.
 *
 * @param[out] axis The retrieved axis value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwAxisScaleOffsetHandle_ is not
 *           a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _axis_ is NULL
 *
 * @note Use corresponding API through QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetV1_t.
 */
Qairt_Status_t QairtSystem_Context_QuantizeParams_BwAxisScaleOffset_getAxis(
    QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetHandle_t bwAxisScaleOffsetHandle,
    int32_t* axis);

/**
 * @brief Get the bit-width value from a bit-width axis scale-offset.
 *
 * @param[in] bwAxisScaleOffsetHandle A handle to the bit-width axis scale-offset.
 *
 * @param[out] bw The retrieved bit-width value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwAxisScaleOffsetHandle_ is not
 *           a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _bw_ is NULL
 *
 * @note Use corresponding API through QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetV1_t.
 */
Qairt_Status_t QairtSystem_Context_QuantizeParams_BwAxisScaleOffset_getBw(
    QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetHandle_t bwAxisScaleOffsetHandle,
    uint32_t* bw);

/**
 * @brief Create a bit-width axis scale-offset mapped handle.
 *
 * @param[out] bwAxisScaleOffsetMappedHandle A handle to the created bit-width axis
 *                                           scale-offset mapped.
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT:
 *           _bwAxisScaleOffsetMappedHandle_ is NULL
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory allocation error while
 *           creating bit-width axis scale-offset mapped
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_BwAxisScaleOffsetMappedV1_t.
 */
Qairt_Status_t QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMapped_create(
    QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMappedHandle_t*
        bwAxisScaleOffsetMappedHandle);

/**
 * @brief Free a bit-width axis scale-offset mapped handle.
 *
 * @param[in] bwAxisScaleOffsetMappedHandle A handle to the bit-width axis
 *                                          scale-offset mapped to be freed.
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwAxisScaleOffsetMappedHandle_
 *           is not a valid handle
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory deallocation error
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_BwAxisScaleOffsetMappedV1_t.
 */
Qairt_Status_t QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMapped_free(
    QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMappedHandle_t
        bwAxisScaleOffsetMappedHandle);

/**
 * @brief Get the number of offsets in a bit-width axis scale-offset mapped.
 *
 * @param[in] bwAxisScaleOffsetMappedHandle A handle to the bit-width axis
 *                                          scale-offset mapped.
 * @param[out] numOffsets The retrieved number of offsets.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwAxisScaleOffsetMappedHandle_
 *           is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _numOffsets_ is NULL
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_BwAxisScaleOffsetMappedV1_t.
 */
Qairt_Status_t QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMapped_getNumOffsets(
    QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMappedHandle_t
        bwAxisScaleOffsetMappedHandle,
    uint32_t* numOffsets);

/**
 * @brief Get the number of scales in a bit-width axis scale-offset mapped.
 *
 * @param[in] bwAxisScaleOffsetMappedHandle A handle to the bit-width axis
 *                                          scale-offset mapped.
 * @param[out] numScales The retrieved number of scales.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwAxisScaleOffsetMappedHandle_
 *           is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _numScales_ is NULL
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_BwAxisScaleOffsetMappedV1_t.
 */
Qairt_Status_t QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMapped_getNumScales(
    QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMappedHandle_t
        bwAxisScaleOffsetMappedHandle,
    uint32_t* numScales);

/**
 * @brief Get the scale at an index from a bit-width axis scale-offset mapped.
 *
 * @param[in] bwAxisScaleOffsetMappedHandle A handle to the bit-width axis
 *                                          scale-offset mapped.
 *
 * @param[in] index The index of the scale to retrieve.
 *
 * @param[out] scale The retrieved scale value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwAxisScaleOffsetMappedHandle_
 *           is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _scale_ is NULL or
 *           _index_ >= total number of scales.
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_BwAxisScaleOffsetMappedV1_t.
 */
Qairt_Status_t QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMapped_getScaleAt(
    QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMappedHandle_t
        bwAxisScaleOffsetMappedHandle,
    uint32_t index,
    float* scale);

/**
 * @brief Get the offset array from a bit-width axis scale-offset mapped.
 *
 * @param[in] bwAxisScaleOffsetMappedHandle A handle to the bit-width axis
 *                                          scale-offset mapped.
 *
 * @param[in] index Index of the offset to retrieve.
 *
 * @param[out] offset The retrieved offset value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwAxisScaleOffsetMappedHandle_
 *           is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _offset_ is NULL or
 *           _index_ >= the total number of offsets
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_BwAxisScaleOffsetMappedV1_t.
 */
Qairt_Status_t QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMapped_getOffsetAt(
    QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMappedHandle_t
        bwAxisScaleOffsetMappedHandle,
    uint32_t index,
    int32_t* offset);

/**
 * @brief Get the axis from a bit-width axis scale-offset mapped.
 *
 * @param[in] bwAxisScaleOffsetMappedHandle A handle to the bit-width axis
 *                                          scale-offset mapped.
 * @param[out] axis The retrieved axis value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwAxisScaleOffsetMappedHandle_
 *           is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _axis_ is NULL
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_BwAxisScaleOffsetMappedV1_t.
 */
Qairt_Status_t QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMapped_getAxis(
    QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMappedHandle_t
        bwAxisScaleOffsetMappedHandle,
    int32_t* axis);

/**
 * @brief Get the bit-width from a bit-width axis scale-offset mapped.
 *
 * @param[in] bwAxisScaleOffsetMappedHandle A handle to the bit-width axis
 *                                          scale-offset mapped.
 * @param[out] bw The retrieved bit-width value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwAxisScaleOffsetMappedHandle_
 *           is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _bw_ is NULL
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_BwAxisScaleOffsetMappedV1_t.
 */
Qairt_Status_t QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMapped_getBw(
    QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMappedHandle_t
        bwAxisScaleOffsetMappedHandle,
    uint32_t* bw);

/**
 * @brief Get the mapping from a bit-width axis scale-offset mapped.
 *
 * @param[in] bwAxisScaleOffsetMappedHandle A handle to the bit-width axis
 *                                          scale-offset mapped.
 * @param[out] mapping The retrieved mapping value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwAxisScaleOffsetMappedHandle_
 *           is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _mapping_ is NULL
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_BwAxisScaleOffsetMappedV1_t.
 */
Qairt_Status_t QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMapped_getMapping(
    QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMappedHandle_t
        bwAxisScaleOffsetMappedHandle,
    Qairt_QuantizationInfoEncodingMapping_t* mapping);

/// QuantInfo
/// PCQ, PTQ

typedef Qairt_Status_t (*QairtSystem_Context_CreateFn_t)(QairtSystem_Context_Handle_t*);

/// Binary Info
typedef Qairt_Status_t (*QairtSystem_Context_GetBinaryInfoFn_t)(
    QairtSystem_Context_Handle_t, const void*, uint64_t, QairtSystem_Context_BinaryInfoHandle_t*);
typedef Qairt_Status_t (*QairtSystemContext_FreeFn_t)(QairtSystem_Context_Handle_t);

typedef Qairt_Status_t (*QairtSystem_Context_BinaryInfo_GetBackendIdFn_t)(
    QairtSystem_Context_BinaryInfoHandle_t, uint32_t*);

typedef Qairt_Status_t (*QairtSystem_Context_BinaryInfo_GetBuildIdFn_t)(
    QairtSystem_Context_BinaryInfoHandle_t, const char**);

typedef Qairt_Status_t (*QairtSystem_Context_BinaryInfo_GetApiVersionFn_t)(
    QairtSystem_Context_BinaryInfoHandle_t, uint32_t*, uint32_t*, uint32_t*);

typedef Qairt_Status_t (*QairtSystem_Context_BinaryInfo_GetBackendApiVersionFn_t)(
    QairtSystem_Context_BinaryInfoHandle_t, uint32_t*, uint32_t*, uint32_t*);

typedef Qairt_Status_t (*QairtSystem_Context_BinaryInfo_GetNumSocVersionsFn_t)(
    QairtSystem_Context_BinaryInfoHandle_t, uint32_t*);

typedef Qairt_Status_t (*QairtSystem_Context_BinaryInfo_GetSocVersionAtFn_t)(
    QairtSystem_Context_BinaryInfoHandle_t, uint32_t, const char**);

typedef Qairt_Status_t (*QairtSystem_Context_BinaryInfo_GetNumGraphInfosFn_t)(
    QairtSystem_Context_BinaryInfoHandle_t, uint32_t*);

typedef Qairt_Status_t (*QairtSystem_Context_BinaryInfo_GetGraphInfoAtFn_t)(
    QairtSystem_Context_BinaryInfoHandle_t, uint32_t, QairtSystem_Context_GraphInfoHandle_t*);

typedef Qairt_Status_t (*QairtSystem_Context_BinaryInfo_GetDevicePlatformInfoFn_t)(
    QairtSystem_Context_BinaryInfoHandle_t, QairtDevice_PlatformInfoHandle_t*);

typedef Qairt_Status_t (*QairtSystem_Context_BinaryInfo_GetSocModelFn_t)(
    QairtSystem_Context_BinaryInfoHandle_t, uint32_t*);

/// Graph Info
typedef Qairt_Status_t (*QairtSystem_Context_GraphInfo_GetNameFn_t)(
    QairtSystem_Context_GraphInfoHandle_t, const char**);

typedef Qairt_Status_t (*QairtSystem_Context_GraphInfo_GetNumInputsFn_t)(
    QairtSystem_Context_GraphInfoHandle_t, uint32_t*);

typedef Qairt_Status_t (*QairtSystem_Context_GraphInfo_GetInputAtFn_t)(
    QairtSystem_Context_GraphInfoHandle_t, uint32_t, QairtSystem_Context_TensorInfoHandle_t*);

typedef Qairt_Status_t (*QairtSystem_Context_GraphInfo_GetNumOutputsFn_t)(
    QairtSystem_Context_GraphInfoHandle_t, uint32_t*);

typedef Qairt_Status_t (*QairtSystem_Context_GraphInfo_GetOutputAtFn_t)(
    QairtSystem_Context_GraphInfoHandle_t, uint32_t, QairtSystem_Context_TensorInfoHandle_t*);

typedef Qairt_Status_t (*QairtSystem_Context_GraphInfo_GetNumUpdateablesFn_t)(
    QairtSystem_Context_GraphInfoHandle_t, uint32_t*);

typedef Qairt_Status_t (*QairtSystem_Context_GraphInfo_GetUpdateableAtFn_t)(
    QairtSystem_Context_GraphInfoHandle_t, uint32_t, QairtSystem_Context_TensorInfoHandle_t*);

typedef Qairt_Status_t (*QairtSystem_Context_GraphInfo_GetGraphInfoBlobSizeFn_t)(
    QairtSystem_Context_GraphInfoHandle_t, uint32_t*);

typedef Qairt_Status_t (*QairtSystem_Context_GraphInfo_GetGraphInfoBlobFn_t)(
    QairtSystem_Context_GraphInfoHandle_t, void**);

typedef Qairt_Status_t (*QairtSystem_Context_GraphInfo_GetStartOpIndexFn_t)(
    QairtSystem_Context_GraphInfoHandle_t, uint32_t*);

typedef Qairt_Status_t (*QairtSystem_Context_GraphInfo_GetEndOpIndexFn_t)(
    QairtSystem_Context_GraphInfoHandle_t, uint32_t*);

/// Graph Info Set
typedef Qairt_Status_t (*QairtSystem_Context_GraphInfoSet_GetNumGraphInfosFn_t)(
    QairtSystem_Context_GraphInfoSetHandle_t, uint32_t*);
typedef Qairt_Status_t (*QairtSystem_Context_GraphInfoSet_GetGraphInfoAtFn_t)(
    QairtSystem_Context_GraphInfoSetHandle_t, uint32_t, QairtSystem_Context_GraphInfoHandle_t*);

/// Tensor Info
typedef Qairt_Status_t (*QairtSystem_Context_TensorInfo_GetIdFn_t)(
    QairtSystem_Context_TensorInfoHandle_t tensor, uint64_t* id);

typedef Qairt_Status_t (*QairtSystem_Context_TensorInfo_GetNameFn_t)(
    QairtSystem_Context_TensorInfoHandle_t tensor, const char** name);

typedef Qairt_Status_t (*QairtSystem_Context_TensorInfo_GetRankFn_t)(
    QairtSystem_Context_TensorInfoHandle_t tensor, uint32_t* rank);

typedef Qairt_Status_t (*QairtSystem_Context_TensorInfo_GetDimensionsFn_t)(
    QairtSystem_Context_TensorInfoHandle_t tensor, const uint32_t** dimensions);

typedef Qairt_Status_t (*QairtSystem_Context_TensorInfo_GetDatatypeFn_t)(
    QairtSystem_Context_TensorInfoHandle_t tensor, Qairt_DataType_t* datatype);
typedef Qairt_Status_t (*QairtSystem_Context_TensorInfo_GetQuantInfoFn_t)(
    QairtSystem_Context_TensorInfoHandle_t tensor,
    QairtSystem_Context_QuantInfoHandle_t* quantInfo);

/// Quant Info
typedef Qairt_Status_t (*QairtSystem_Context_QuantParamInfo_GetQuantizationTypeFn_t)(
    QairtSystem_Context_QuantInfoHandle_t quantInfo, Qairt_QuantizationInfoType_t* type);

typedef Qairt_Status_t (*QairtSystem_Context_QuantParamInfo_GetScaleOffsetFn_t)(
    QairtSystem_Context_QuantInfoHandle_t quantInfoHandle,
    QairtSystem_Context_QuantizeParams_ScaleOffsetHandle_t* scaleOffsetHandle);

typedef Qairt_Status_t (*QairtSystem_Context_QuantParamInfo_GetAxisScaleOffsetFn_t)(
    QairtSystem_Context_QuantInfoHandle_t quantInfoHandle,
    QairtSystem_Context_QuantizeParams_AxisScaleOffsetHandle_t* axisScaleOffsetHandle);

typedef Qairt_Status_t (*QairtSystem_Context_QuantParamInfo_GetBwScaleOffsetFn_t)(
    QairtSystem_Context_QuantInfoHandle_t quantInfoHandle,
    QairtSystem_Context_QuantizeParams_BwScaleOffsetHandle_t* bwScaleOffsetHandle);

typedef Qairt_Status_t (*QairtSystem_Context_QuantParamInfo_GetBwAxisScaleOffsetFn_t)(
    QairtSystem_Context_QuantInfoHandle_t quantInfoHandle,
    QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetHandle_t* bwAxisScaleOffsetHandle);

// ScaleOffset Function Pointer Typedefs

typedef Qairt_Status_t (*QairtSystem_Context_QuantizeParams_ScaleOffset_CreateFn_t)(
    QairtSystem_Context_QuantizeParams_ScaleOffsetHandle_t* scaleOffsetHandle);
typedef Qairt_Status_t (*QairtSystem_Context_QuantizeParams_ScaleOffset_FreeFn_t)(
    QairtSystem_Context_QuantizeParams_ScaleOffsetHandle_t scaleOffsetHandle);
typedef Qairt_Status_t (*QairtSystem_Context_QuantizeParams_ScaleOffset_GetScaleFn_t)(
    QairtSystem_Context_QuantizeParams_ScaleOffsetHandle_t scaleOffsetHandle, float* scale);
typedef Qairt_Status_t (*QairtSystem_Context_QuantizeParams_ScaleOffset_GetOffsetFn_t)(
    QairtSystem_Context_QuantizeParams_ScaleOffsetHandle_t scaleOffsetHandle, int32_t* offset);

// AxisScaleOffset Function Pointer Typedefs
typedef Qairt_Status_t (*QairtSystem_Context_QuantizeParams_AxisScaleOffset_CreateFn_t)(
    QairtSystem_Context_QuantizeParams_AxisScaleOffsetHandle_t* axisScaleOffsetHandle);
typedef Qairt_Status_t (*QairtSystem_Context_QuantizeParams_AxisScaleOffset_FreeFn_t)(
    QairtSystem_Context_QuantizeParams_AxisScaleOffsetHandle_t axisScaleOffsetHandle);

typedef Qairt_Status_t (*QairtSystem_Context_QuantizeParams_AxisScaleOffset_GetNumScaleOffsetsFn_t)(
    QairtSystem_Context_QuantizeParams_AxisScaleOffsetHandle_t axisScaleOffsetHandle,
    uint32_t* numScaleOffsets);
typedef Qairt_Status_t (*QairtSystem_Context_QuantizeParams_AxisScaleOffset_GetScaleOffsetAtFn_t)(
    QairtSystem_Context_QuantizeParams_AxisScaleOffsetHandle_t axisScaleOffsetHandle,
    uint32_t index,
    QairtSystem_Context_QuantizeParams_ScaleOffsetHandle_t* scaleOffsetHandle);
typedef Qairt_Status_t (*QairtSystem_Context_QuantizeParams_AxisScaleOffset_GetAxisFn_t)(
    QairtSystem_Context_QuantizeParams_AxisScaleOffsetHandle_t axisScaleOffsetHandle,
    int32_t* axis);

// BwScaleOffset Function Pointer Typedefs
typedef Qairt_Status_t (*QairtSystem_Context_QuantizeParams_BwScaleOffset_CreateFn_t)(
    QairtSystem_Context_QuantizeParams_BwScaleOffsetHandle_t* bwScaleOffsetHandle);
typedef Qairt_Status_t (*QairtSystem_Context_QuantizeParams_BwScaleOffset_FreeFn_t)(
    QairtSystem_Context_QuantizeParams_BwScaleOffsetHandle_t bwScaleOffsetHandle);
typedef Qairt_Status_t (*QairtSystem_Context_QuantizeParams_BwScaleOffset_GetScaleFn_t)(
    QairtSystem_Context_QuantizeParams_BwScaleOffsetHandle_t bwScaleOffsetHandle, float* scale);
typedef Qairt_Status_t (*QairtSystem_Context_QuantizeParams_BwScaleOffset_GetOffsetFn_t)(
    QairtSystem_Context_QuantizeParams_BwScaleOffsetHandle_t bwScaleOffsetHandle, int32_t* offset);
typedef Qairt_Status_t (*QairtSystem_Context_QuantizeParams_BwScaleOffset_GetBwFn_t)(
    QairtSystem_Context_QuantizeParams_BwScaleOffsetHandle_t bwScaleOffsetHandle, uint32_t* bw);

// BwAxisScaleOffset Function Pointer Typedefs
typedef Qairt_Status_t (*QairtSystem_Context_QuantizeParams_BwAxisScaleOffset_CreateFn_t)(
    QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetHandle_t* bwAxisScaleOffsetHandle);
typedef Qairt_Status_t (*QairtSystem_Context_QuantizeParams_BwAxisScaleOffset_FreeFn_t)(
    QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetHandle_t bwAxisScaleOffsetHandle);
typedef Qairt_Status_t (*QairtSystem_Context_QuantizeParams_BwAxisScaleOffset_GetNumScalesFn_t)(
    QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetHandle_t bwAxisScaleOffsetHandle,
    uint32_t* numScales);
typedef Qairt_Status_t (*QairtSystem_Context_QuantizeParams_BwAxisScaleOffset_GetScaleAtFn_t)(
    QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetHandle_t bwAxisScaleOffsetHandle,
    uint32_t index,
    float* scale);
typedef Qairt_Status_t (*QairtSystem_Context_QuantizeParams_BwAxisScaleOffset_GetNumOffsetsFn_t)(
    QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetHandle_t bwAxisScaleOffsetHandle,
    uint32_t* numOffsets);
typedef Qairt_Status_t (*QairtSystem_Context_QuantizeParams_BwAxisScaleOffset_GetOffsetAtFn_t)(
    QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetHandle_t bwAxisScaleOffsetHandle,
    uint32_t index,
    int32_t* offset);
typedef Qairt_Status_t (*QairtSystem_Context_QuantizeParams_BwAxisScaleOffset_GetAxisFn_t)(
    QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetHandle_t bwAxisScaleOffsetHandle,
    int32_t* axis);
typedef Qairt_Status_t (*QairtSystem_Context_QuantizeParams_BwAxisScaleOffset_GetBwFn_t)(
    QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetHandle_t bwAxisScaleOffsetHandle,
    uint32_t* bw);

// BwAxisScaleOffsetMapped Function Pointer Typedefs

typedef Qairt_Status_t (*QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMapped_CreateFn_t)(
    QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMappedHandle_t*
        bwAxisScaleOffsetMappedHandle);
typedef Qairt_Status_t (*QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMapped_FreeFn_t)(
    QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMappedHandle_t
        bwAxisScaleOffsetMappedHandle);
typedef Qairt_Status_t (
    *QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMapped_GetNumScalesFn_t)(
    QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMappedHandle_t
        bwAxisScaleOffsetMappedHandle,
    uint32_t* numScales);
typedef Qairt_Status_t (*QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMapped_GetScaleAtFn_t)(
    QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMappedHandle_t
        bwAxisScaleOffsetMappedHandle,
    uint32_t index,
    float* scale);
typedef Qairt_Status_t (
    *QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMapped_GetNumOffsetsFn_t)(
    QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMappedHandle_t
        bwAxisScaleOffsetMappedHandle,
    uint32_t* numOffsets);
typedef Qairt_Status_t (
    *QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMapped_GetOffsetAtFn_t)(
    QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMappedHandle_t
        bwAxisScaleOffsetMappedHandle,
    uint32_t index,
    int32_t* offset);
typedef Qairt_Status_t (*QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMapped_GetAxisFn_t)(
    QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMappedHandle_t
        bwAxisScaleOffsetMappedHandle,
    int32_t* axis);
typedef Qairt_Status_t (*QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMapped_GetBwFn_t)(
    QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMappedHandle_t
        bwAxisScaleOffsetMappedHandle,
    uint32_t* bw);
typedef Qairt_Status_t (*QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMapped_GetMappingFn_t)(
    QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMappedHandle_t
        bwAxisScaleOffsetMappedHandle,
    Qairt_QuantizationInfoEncodingMapping_t* mapping);

typedef Qairt_Status_t (*QairtSystem_Context_QuantParamInfo_GetBwAxisScaleOffsetMappedFn_t)(
    QairtSystem_Context_QuantInfoHandle_t quantInfoHandle,
    QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMappedHandle_t*
        bwAxisScaleOffsetMappedHandle);

//=============================================================================
// Public Interfaces
//=============================================================================

typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtSystem_Context_CreateFn_t create;
  QairtSystem_Context_GetBinaryInfoFn_t getBinaryInfo;
  QairtSystemContext_FreeFn_t free;
} QairtSystem_ContextV1_t;

typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtSystem_Context_BinaryInfo_GetBackendIdFn_t getBackendId;
  QairtSystem_Context_BinaryInfo_GetBuildIdFn_t getBuildId;
  QairtSystem_Context_BinaryInfo_GetApiVersionFn_t getApiVersion;
  QairtSystem_Context_BinaryInfo_GetBackendApiVersionFn_t getBackendApiVersion;
  QairtSystem_Context_BinaryInfo_GetNumSocVersionsFn_t getNumSocVersions;
  QairtSystem_Context_BinaryInfo_GetSocVersionAtFn_t getSocVersionAt;
  QairtSystem_Context_BinaryInfo_GetNumGraphInfosFn_t getNumGraphInfos;
  QairtSystem_Context_BinaryInfo_GetGraphInfoAtFn_t getGraphInfoAt;
  QairtSystem_Context_BinaryInfo_GetDevicePlatformInfoFn_t getDevicePlatformInfo;
  QairtSystem_Context_BinaryInfo_GetSocModelFn_t getSocModel;
} QairtSystem_Context_BinaryInfoV1_t;

typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtSystem_Context_GraphInfo_GetNameFn_t getName;
  QairtSystem_Context_GraphInfo_GetNumInputsFn_t getNumInputs;
  QairtSystem_Context_GraphInfo_GetInputAtFn_t getInputAt;
  QairtSystem_Context_GraphInfo_GetNumOutputsFn_t getNumOutputs;
  QairtSystem_Context_GraphInfo_GetOutputAtFn_t getOutputAt;
  QairtSystem_Context_GraphInfo_GetNumUpdateablesFn_t getNumUpdateables;
  QairtSystem_Context_GraphInfo_GetUpdateableAtFn_t getUpdateableAt;
  QairtSystem_Context_GraphInfo_GetGraphInfoBlobSizeFn_t getGraphInfoBlobSize;
  QairtSystem_Context_GraphInfo_GetGraphInfoBlobFn_t getGraphInfoBlob;
  QairtSystem_Context_GraphInfo_GetStartOpIndexFn_t getStartOpIndex;
  QairtSystem_Context_GraphInfo_GetEndOpIndexFn_t getEndOpIndex;
} QairtSystem_Context_GraphInfoV1_t;

typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtSystem_Context_GraphInfoSet_GetNumGraphInfosFn_t getNumGraphInfos;
  QairtSystem_Context_GraphInfoSet_GetGraphInfoAtFn_t getGraphInfoAt;
} QairtSystem_Context_GraphInfoSetV1_t;

typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtSystem_Context_TensorInfo_GetIdFn_t getId;
  QairtSystem_Context_TensorInfo_GetNameFn_t getName;
  QairtSystem_Context_TensorInfo_GetRankFn_t getRank;
  QairtSystem_Context_TensorInfo_GetDimensionsFn_t getDimensions;
  QairtSystem_Context_TensorInfo_GetDatatypeFn_t getDatatype;
  QairtSystem_Context_TensorInfo_GetQuantInfoFn_t getQuantInfo;
} QairtSystem_Context_TensorInfoV1_t;

typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtSystem_Context_QuantParamInfo_GetQuantizationTypeFn_t getQuantizationType;
  QairtSystem_Context_QuantParamInfo_GetScaleOffsetFn_t getScaleOffset;
  QairtSystem_Context_QuantParamInfo_GetAxisScaleOffsetFn_t getAxisScaleOffset;
  QairtSystem_Context_QuantParamInfo_GetBwScaleOffsetFn_t getBwScaleOffset;
  QairtSystem_Context_QuantParamInfo_GetBwAxisScaleOffsetFn_t getBwAxisScaleOffset;
  QairtSystem_Context_QuantParamInfo_GetBwAxisScaleOffsetMappedFn_t getBwAxisScaleOffsetMapped;
} QairtSystem_Context_QuantParamInfoV1_t;

/**
 * @brief Scale-offset interface structure containing function pointers for
 *        scale-offset operations.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtSystem_Context_QuantizeParams_ScaleOffset_CreateFn_t create;
  QairtSystem_Context_QuantizeParams_ScaleOffset_FreeFn_t free;
  QairtSystem_Context_QuantizeParams_ScaleOffset_GetScaleFn_t getScale;
  QairtSystem_Context_QuantizeParams_ScaleOffset_GetOffsetFn_t getOffset;
} QairtSystem_Context_QuantizeParams_ScaleOffsetV1_t;

/**
 * @brief Axis scale-offset interface structure containing function pointers for
 *        axis scale-offset operations.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtSystem_Context_QuantizeParams_AxisScaleOffset_CreateFn_t create;
  QairtSystem_Context_QuantizeParams_AxisScaleOffset_FreeFn_t free;
  QairtSystem_Context_QuantizeParams_AxisScaleOffset_GetNumScaleOffsetsFn_t getNumScaleOffsets;
  QairtSystem_Context_QuantizeParams_AxisScaleOffset_GetScaleOffsetAtFn_t getScaleOffsetAt;
  QairtSystem_Context_QuantizeParams_AxisScaleOffset_GetAxisFn_t getAxis;
} QairtSystem_Context_QuantizeParams_AxisScaleOffsetV1_t;

/**
 * @struct QairtSystem_Context_QuantizeParams_BwScaleOffsetV1_t
 * @brief This struct contains the functions that define the V1 interface for
 *        QAIRT System Context BwScaleOffset.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtSystem_Context_QuantizeParams_BwScaleOffset_CreateFn_t create;
  QairtSystem_Context_QuantizeParams_BwScaleOffset_FreeFn_t free;
  QairtSystem_Context_QuantizeParams_BwScaleOffset_GetScaleFn_t getScale;
  QairtSystem_Context_QuantizeParams_BwScaleOffset_GetOffsetFn_t getOffset;
  QairtSystem_Context_QuantizeParams_BwScaleOffset_GetBwFn_t getBw;
} QairtSystem_Context_QuantizeParams_BwScaleOffsetV1_t;

/**
 * @struct QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetV1_t
 * @brief This struct contains the functions that define the V1 interface for
 *        QAIRT System Context BwAxisScaleOffset.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtSystem_Context_QuantizeParams_BwAxisScaleOffset_CreateFn_t create;
  QairtSystem_Context_QuantizeParams_BwAxisScaleOffset_FreeFn_t free;
  QairtSystem_Context_QuantizeParams_BwAxisScaleOffset_GetNumScalesFn_t getNumScales;
  QairtSystem_Context_QuantizeParams_BwAxisScaleOffset_GetScaleAtFn_t getScaleAt;
  QairtSystem_Context_QuantizeParams_BwAxisScaleOffset_GetNumOffsetsFn_t getNumOffsets;
  QairtSystem_Context_QuantizeParams_BwAxisScaleOffset_GetOffsetAtFn_t getOffsetAt;
  QairtSystem_Context_QuantizeParams_BwAxisScaleOffset_GetAxisFn_t getAxis;
  QairtSystem_Context_QuantizeParams_BwAxisScaleOffset_GetBwFn_t getBw;
} QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetV1_t;

/**
 * @struct QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMappedV1_t
 * @brief This struct contains the functions that define the V1 interface for
 *        QAIRT System Context BwAxisScaleOffsetMapped.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMapped_CreateFn_t create;
  QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMapped_FreeFn_t free;
  QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMapped_GetNumScalesFn_t getNumScales;
  QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMapped_GetScaleAtFn_t getScaleAt;
  QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMapped_GetNumOffsetsFn_t getNumOffsets;
  QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMapped_GetOffsetAtFn_t getOffsetAt;
  QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMapped_GetAxisFn_t getAxis;
  QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMapped_GetBwFn_t getBw;
  QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMapped_GetMappingFn_t getMapping;
} QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMappedV1_t;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // QAISW_QAIRTSYSTEMCONTEXT_H

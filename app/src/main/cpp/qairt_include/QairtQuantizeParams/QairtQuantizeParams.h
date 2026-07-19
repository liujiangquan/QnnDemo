//==============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//==============================================================================

/**
 * @file
 * @brief Quantization Parameters component API
 *
 *        A header which contains the QAIRT Quantization Parameters component
 *        and the interfaces it implements. This component provides various
 *        quantization encoding types for tensor quantization including scale-offset,
 *        axis-based, bit-width, block-based, and vector encoding schemes.
 */

#ifndef QAISW_QAIRTQUANTIZEPARAMS_H
#define QAISW_QAIRTQUANTIZEPARAMS_H
#include "QairtCommon/QairtCommon.h"

#ifdef __cplusplus
extern "C" {
#include <cstdint>
#else
#include <stdint.h>
#endif

//=============================================================================
// Macros
//=============================================================================
#define QAIRT_QUANTIZE_PARAMS_V1_ID QAIRT_MIN_ID_QUANTIZE_PARAMS

#define QAIRT_SCALE_OFFSET_V1_ID QAIRT_MIN_ID_QUANTIZE_PARAMS + 1

#define QAIRT_AXIS_SCALE_OFFSET_V1_ID QAIRT_MIN_ID_QUANTIZE_PARAMS + 2

#define QAIRT_BW_SCALE_OFFSET_V1_ID QAIRT_MIN_ID_QUANTIZE_PARAMS + 3

#define QAIRT_BW_AXIS_SCALE_OFFSET_V1_ID QAIRT_MIN_ID_QUANTIZE_PARAMS + 4

#define QAIRT_BLOCK_ENCODING_V1_ID QAIRT_MIN_ID_QUANTIZE_PARAMS + 5

#define QAIRT_BLOCKWISE_EXPANSION_V1_ID QAIRT_MIN_ID_QUANTIZE_PARAMS + 6

#define QAIRT_VECTOR_ENCODING_V1_ID QAIRT_MIN_ID_QUANTIZE_PARAMS + 7

#define QAIRT_BW_AXIS_SCALE_OFFSET_MAPPED_V1_ID QAIRT_MIN_ID_QUANTIZE_PARAMS + 8

#define QAIRT_MICROSCALING_ENCODING_V1_ID QAIRT_MIN_ID_QUANTIZE_PARAMS + 9

#define QAIRT_FLOAT_SCALE_OFFSET_V1_ID QAIRT_MIN_ID_QUANTIZE_PARAMS + 10

#define QAIRT_BW_FLOAT_BLOCK_ENCODING_V1_ID QAIRT_MIN_ID_QUANTIZE_PARAMS + 11

#define QAIRT_BW_BLOCK_MAPPED_V1_ID QAIRT_MIN_ID_QUANTIZE_PARAMS + 12

#define QAIRT_BW_BLOCKWISE_EXPANSION_MAPPED_V1_ID QAIRT_MIN_ID_QUANTIZE_PARAMS + 13

#define QAIRT_FLOAT_BLOCK_ENCODING_V1_ID QAIRT_MIN_ID_QUANTIZE_PARAMS + 14

//=============================================================================
// Data Types
//=============================================================================

/**
 * @brief Quantization parameters handle used to interface with quantization
 *        parameter APIs
 *
 *        The handle itself is created and freed by the client.
 */
typedef struct _QairtQuantizeParams_Handle_t* QairtQuantizeParams_Handle_t;

/**
 * @brief Scale-offset handle for per-tensor quantization encoding
 *
 *        The handle itself is created and freed by the client.
 */
typedef struct _QairtQuantizeParams_ScaleOffsetHandle_t* QairtQuantizeParams_ScaleOffsetHandle_t;

/**
 * @brief Axis scale-offset handle for per-axis (e.g., per-channel)
 *        quantization encoding
 *
 *        The handle itself is created and freed by the client.
 */
typedef struct _QairtQuantizeParams_AxisScaleOffsetHandle_t*
    QairtQuantizeParams_AxisScaleOffsetHandle_t;

/**
 * @brief Bit-width scale-offset handle for bit-width quantization encoding
 *
 *        The handle itself is created and freed by the client.
 */
typedef struct _QairtQuantizeParams_BwScaleOffsetHandle_t*
    QairtQuantizeParams_BwScaleOffsetHandle_t;

/**
 * @brief Bit-width axis scale-offset handle for bit-width per-axis
 *        quantization encoding
 *
 *        The handle itself is created and freed by the client.
 */
typedef struct _QairtQuantizeParams_BwAxisScaleOffsetHandle_t*
    QairtQuantizeParams_BwAxisScaleOffsetHandle_t;

/**
 * @brief Block encoding handle for per-block quantization encoding
 *
 *        The handle itself is created and freed by the client.
 */
typedef struct _QairtQuantizeParams_BlockEncodingHandle_t*
    QairtQuantizeParams_BlockEncodingHandle_t;

/**
 * @brief Blockwise expansion handle for blockwise expansion quantization
 *        encoding
 *
 *        The handle itself is created and freed by the client.
 */
typedef struct _QairtQuantizeParams_BlockwiseExpansionHandle_t*
    QairtQuantizeParams_BlockwiseExpansionHandle_t;

/**
 * @brief Vector encoding handle for vector quantization compression encoding
 *
 *        The handle itself is created and freed by the client.
 */
typedef struct _QairtQuantizeParams_VectorEncodingHandle_t*
    QairtQuantizeParams_VectorEncodingHandle_t;

/**
 * @brief Bit-width axis scale-offset mapped handle for bit-width per-axis
 *        quantization encoding with mapping
 *
 *        The handle itself is created and freed by the client.
 */
typedef struct _QairtQuantizeParams_BwAxisScaleOffsetMappedHandle_t*
    QairtQuantizeParams_BwAxisScaleOffsetMappedHandle_t;

/**
 * @brief Microscaling encoding handle for Microscaling (MX) quantization
 *        encoding
 *
 *        The handle itself is created and freed by the client.
 */
typedef struct _QairtQuantizeParams_MicroscalingEncodingHandle_t*
    QairtQuantizeParams_MicroscalingEncodingHandle_t;

/**
 * @brief Float scale-offset handle for per-tensor float quantization encoding
 *
 *        The handle itself is created and freed by the client.
 */
typedef struct _QairtQuantizeParams_FloatScaleOffsetHandle_t*
    QairtQuantizeParams_FloatScaleOffsetHandle_t;

/**
 * @brief Bit-width float scale-offset block handle for bit-width per-block
 *        quantization encoding
 *
 *        The handle itself is created and freed by the client.
 */
typedef struct _QairtQuantizeParams_BwFloatBlockEncodingHandle_t*
    QairtQuantizeParams_BwFloatBlockEncodingHandle_t;

/**
 * @brief Float block encoding handle
 *
 *        The handle itself is created and freed by the client.
 */
typedef struct _QairtQuantizeParams_FloatBlockEncodingHandle_t*
    QairtQuantizeParams_FloatBlockEncodingHandle_t;

/**
 * @brief Bit-width block mapped encoding handle
 *
 *        The handle itself is created and freed by the client.
 */
typedef struct _QairtQuantizeParams_BwBlockMappedHandle_t*
    QairtQuantizeParams_BwBlockMappedHandle_t;

/**
 * @brief Bit-width blockwise expansion mapped encoding handle
 *
 *        The handle itself is created and freed by the client.
 */
typedef struct _QairtQuantizeParams_BwBlockwiseExpansionMappedHandle_t*
    QairtQuantizeParams_BwBlockwiseExpansionMappedHandle_t;

/**
 * @brief An enum which defines various floating point encodings used in
 *        Microscaling (MX) quantization.
 */
typedef enum {
  /// Data with microscaling MXFP8 (E5M2) encoding. Compatible with
  /// QAIRT_DATATYPE_FLOAT_8
  QAIRT_FLOAT_ENCODING_MXFP8_E5M2 = 0,
  /// Data with microscaling MXFP8 (E4M3) encoding. Compatible with
  /// QAIRT_DATATYPE_FLOAT_8
  QAIRT_FLOAT_ENCODING_MXFP8_E4M3 = 1,
  /// Data with microscaling MXFP6 (E3M2) encoding.
  QAIRT_FLOAT_ENCODING_MXFP6_E3M2 = 2,
  /// Data with microscaling MXFP6 (E2M3) encoding.
  QAIRT_FLOAT_ENCODING_MXFP6_E2M3 = 3,
  /// Data with microscaling MXFP4 (E2M1) encoding.
  QAIRT_FLOAT_ENCODING_MXFP4_E2M1 = 4,
  // Unused, present to ensure 32 bits.
  QAIRT_FLOAT_ENCODING_UNDEFINED = 0x7FFFFFFF
} Qairt_FloatEncoding_t;

/**
 * @brief An enum to specify quantized value mapping scheme
 */
typedef enum {
  /// Indicates standard symmetric 2's compliment mapping
  QAIRT_QUANTIZATION_ENCODING_MAPPING_STANDARD_SYMMETRIC = 0,
  /// Indicates 2's compliment mapping with a positive shift of one
  QAIRT_QUANTIZATION_ENCODING_MAPPING_ASYMMETRIC_PLUS_ONE = 1,
  /// Indicates linear mapping symmetric about zero, but excluding zero from the range
  QAIRT_QUANTIZATION_ENCODING_MAPPING_LINEAR_SYMMETRIC_EXCLUDE_ZERO = 2,
  // Unused, present to ensure 32 bits.
  QAIRT_QUANTIZATION_ENCODING_MAPPING_UNDEFINED = 0x7FFFFFFF
} Qairt_QuantizationEncodingMapping_t;

/**
 * @brief An enum to specify the storage type for blockwise expansion block
 *        scales
 */
typedef enum {
  /// 8-bit storage for block scales
  QAIRT_BLOCKWISE_EXPANSION_BITWIDTH_SCALE_STORAGE_8 = 0,
  /// 16-bit storage for block scales
  QAIRT_BLOCKWISE_EXPANSION_BITWIDTH_SCALE_STORAGE_16 = 1,
  // Unused, present to ensure 32 bits.
  QAIRT_BLOCKWISE_EXPANSION_BITWIDTH_SCALE_STORAGE_UNDEFINED = 0x7FFFFFFF
} Qairt_BlockwiseExpansionBlockScaleStorageType_t;

/**
 * @brief An enum to specify quantization encoding types
 */
typedef enum {
  /// Indicates per-tensor scale-offset encoding type. See Qairt_ScaleOffset_t.
  /// Support
  /// can be
  /// checked via
  /// QAIRT_CAPABILITY_TENSOR_SUPPORT_QUANTIZATION_ENCODING_SCALE_OFFSET.
  QAIRT_QUANTIZATION_ENCODING_SCALE_OFFSET = 0,
  /// Indicates per-axis (e.g. per-channel) scale-offset encoding type. See
  /// Qairt_AxisScaleOffset_t.
  /// Support can be checked via
  /// QAIRT_CAPABILITY_TENSOR_SUPPORT_QUANTIZATION_ENCODING_AXIS_SCALE_OFFSET.
  QAIRT_QUANTIZATION_ENCODING_AXIS_SCALE_OFFSET = 1,
  /// Indicates bit-width scale-offset encoding type. See Qairt_BwScaleOffset_t.
  /// Support
  /// can be
  /// checked via
  /// QAIRT_CAPABILITY_TENSOR_SUPPORT_QUANTIZATION_ENCODING_BW_SCALE_OFFSET.
  QAIRT_QUANTIZATION_ENCODING_BW_SCALE_OFFSET = 2,
  /// Indicates bit-width per-axis scale-offset encoding type. See
  /// Qairt_BwAxisScaleOffset_t.
  /// Support can be checked via
  /// QAIRT_CAPABILITY_TENSOR_SUPPORT_QUANTIZATION_ENCODING_BW_AXIS_SCALE_OFFSET.
  QAIRT_QUANTIZATION_ENCODING_BW_AXIS_SCALE_OFFSET = 3,
  /// Indicates per-block scale-offset encoding type. See
  /// Qairt_BlockScaleOffset_t.
  /// Support can be
  /// checked via
  /// QAIRT_CAPABILITY_TENSOR_SUPPORT_QUANTIZATION_ENCODING_BLOCK.
  QAIRT_QUANTIZATION_ENCODING_BLOCK = 4,
  /// Indicates blockwise expansion encoding type. See
  /// Qairt_BlockwiseExpansion_t. Support
  /// can be
  /// checked via
  /// QAIRT_CAPABILITY_TENSOR_SUPPORT_QUANTIZATION_ENCODING_BLOCKWISE_EXPANSION.
  QAIRT_QUANTIZATION_ENCODING_BLOCKWISE_EXPANSION = 5,
  /// Indicates VQ compression encoding type. See
  /// Qairt_VectorQuantCompression_t. Support
  /// can be
  /// checked via
  /// QAIRT_CAPABILITY_TENSOR_SUPPORT_QUANTIZATION_ENCODING_VECTOR.
  QAIRT_QUANTIZATION_ENCODING_VECTOR = 6,
  /// Indicates bit-width per-axis scale-offset mapped encoding type. See
  /// Qairt_BwAxisScaleOffsetMapped_t.
  /// Support can be checked via
  /// QAIRT_CAPABILITY_TENSOR_SUPPORT_QUANTIZATION_ENCODING_BW_AXIS_SCALE_OFFSET_MAPPED.
  QAIRT_QUANTIZATION_ENCODING_BW_AXIS_SCALE_OFFSET_MAPPED = 8,
  /// Indicates bit-width per-block scale-offset mapped encoding type. See
  /// QairtQuantizeParams_BwBlockMappedHandle_t.
  /// Support can be checked via
  /// QAIRT_CAPABILITY_TENSOR_SUPPORT_QUANTIZATION_ENCODING_BW_BLOCK_SCALE_OFFSET_MAPPED.
  QAIRT_QUANTIZATION_ENCODING_BW_BLOCK_MAPPED = 9,
  /// Indicates bit-width blockwise expansion mapped encoding type. See
  /// QairtQuantizeParams_BwBlockwiseExpansionMappedHandle_t.
  /// Support can be checked via
  /// QAIRT_CAPABILITY_TENSOR_SUPPORT_QUANTIZATION_ENCODING_BW_BLOCKWISE_EXPANSION_MAPPED.
  QAIRT_QUANTIZATION_ENCODING_BW_BLOCKWISE_EXPANSION_MAPPED = 10,
  /// Indicates per-block float scale-offset encoding type. See
  /// QairtQuantizeParams_FloatBlockEncodingHandle_t.
  /// Support can be checked via
  /// QAIRT_CAPABILITY_TENSOR_SUPPORT_QUANTIZATION_ENCODING_FLOAT_BLOCK.
  QAIRT_QUANTIZATION_ENCODING_FLOAT_BLOCK = 7,
  /// Indicates bit-width per-block float scale-offset encoding type. See
  /// QairtQuantizeParams_BwFloatBlockEncodingHandle_t.
  /// Support can be checked via
  /// QAIRT_CAPABILITY_TENSOR_SUPPORT_QUANTIZATION_ENCODING_BW_FLOAT_BLOCK.
  QAIRT_QUANTIZATION_ENCODING_BW_FLOAT_BLOCK = 11,
  /// Indicates Microscaling (MX) encoding type. See
  /// QairtQuantizeParams_MicroscalingEncodingHandle_t.
  /// Support can be checked via
  /// QAIRT_CAPABILITY_TENSOR_SUPPORT_QUANTIZATION_ENCODING_MICROSCALING.
  QAIRT_QUANTIZATION_ENCODING_MICROSCALING = 12,
  // Unused, present to ensure 32 bits.
  QAIRT_QUANTIZATION_ENCODING_UNDEFINED = 0x7FFFFFFF
} Qairt_QuantizationEncoding_t;

//=============================================================================
// Public Functions
//=============================================================================

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
Qairt_Status_t QairtQuantizeParams_ScaleOffset_create(
    QairtQuantizeParams_ScaleOffsetHandle_t* scaleOffsetHandle);

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
Qairt_Status_t QairtQuantizeParams_ScaleOffset_free(
    QairtQuantizeParams_ScaleOffsetHandle_t scaleOffsetHandle);

/**
 * @brief Set the scale value of a scale-offset.
 *
 * @param[in] scaleOffsetHandle A handle to the scale-offset.
 *
 * @param[in] scale The scale value to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _scaleOffsetHandle_ is not
 *           a valid handle
 *
 * @note Use corresponding API through QairtQuantizeParams_ScaleOffsetV1_t.
 */
Qairt_Status_t QairtQuantizeParams_ScaleOffset_setScale(
    QairtQuantizeParams_ScaleOffsetHandle_t scaleOffsetHandle, float scale);

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
Qairt_Status_t QairtQuantizeParams_ScaleOffset_getScale(
    QairtQuantizeParams_ScaleOffsetHandle_t scaleOffsetHandle, float* scale);

/**
 * @brief Set the offset value of a scale-offset.
 *
 * @param[in] scaleOffsetHandle A handle to the scale-offset.
 *
 * @param[in] offset The offset value to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _scaleOffsetHandle_ is not
 *           a valid handle
 *
 * @note Use corresponding API through QairtQuantizeParams_ScaleOffsetV1_t.
 */
Qairt_Status_t QairtQuantizeParams_ScaleOffset_setOffset(
    QairtQuantizeParams_ScaleOffsetHandle_t scaleOffsetHandle, int32_t offset);

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
Qairt_Status_t QairtQuantizeParams_ScaleOffset_getOffset(
    QairtQuantizeParams_ScaleOffsetHandle_t scaleOffsetHandle, int32_t* offset);

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
Qairt_Status_t QairtQuantizeParams_AxisScaleOffset_create(
    QairtQuantizeParams_AxisScaleOffsetHandle_t* axisScaleOffsetHandle);

/**
 * @brief Free an axis scale-offset handle.
 *
 * @param[in] axisScaleOffsetHandle A handle to the axis scale-offset to be
 *                                  freed.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _axisScaleOffsetHandle_ is
 *           not a valid handle
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory deallocation error
 *
 * @note Use corresponding API through QairtQuantizeParams_AxisScaleOffsetV1_t.
 */
Qairt_Status_t QairtQuantizeParams_AxisScaleOffset_free(
    QairtQuantizeParams_AxisScaleOffsetHandle_t axisScaleOffsetHandle);

/**
 * @brief Set the scale-offset array for an axis scale-offset.
 *
 * @param[in] axisScaleOffsetHandle A handle to the axis scale-offset.
 *
 * @param[in] scaleOffsetsHandle Array of scale-offset handles.
 *
 * @param[in] numScaleOffsets Number of scale-offset handles in the array.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _axisScaleOffsetHandle_ or a
 *           handle in _scaleOffsetsHandle_ array is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _scaleOffsetsHandle_ is
 *           NULL or _numScaleOffsets_ is 0
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory allocation error while
 *           setting scale-offsets
 *
 * @note Use corresponding API through QairtQuantizeParams_AxisScaleOffsetV1_t.
 */
Qairt_Status_t QairtQuantizeParams_AxisScaleOffset_setScaleOffsets(
    QairtQuantizeParams_AxisScaleOffsetHandle_t axisScaleOffsetHandle,
    QairtQuantizeParams_ScaleOffsetHandle_t* scaleOffsetsHandle,
    uint32_t numScaleOffsets);

/**
 * @brief Get the number of scale-offsets in an axis scale-offset.
 *
 * @param[in] axisScaleOffsetHandle A handle to the axis scale-offset.
 *
 * @param[out] numScaleOffsets The retrieved number of scale-offsets.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _axisScaleOffsetHandle_ is
 *           not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _numScaleOffsets_ is
 *           NULL
 *
 * @note Use corresponding API through QairtQuantizeParams_AxisScaleOffsetV1_t.
 */
Qairt_Status_t QairtQuantizeParams_AxisScaleOffset_getNumScaleOffsets(
    QairtQuantizeParams_AxisScaleOffsetHandle_t axisScaleOffsetHandle, uint32_t* numScaleOffsets);

/**
 * @brief Get the scale-offset at index along the axis.
 *
 * @param[in] axisScaleOffsetHandle A handle to the axis scale-offset.
 *
 * @param[in] index Index of scale-offset that should be retrieved.
 *
 * @param[out] scaleOffsetsHandle The retrieved scale-offset handle.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _axisScaleOffsetHandle_ is
 *           not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _scaleOffsetsHandle_ is
 *           NULL or _numScaleOffsets_ is 0
 *
 * @note Use corresponding API through QairtQuantizeParams_AxisScaleOffsetV1_t.
 */
Qairt_Status_t QairtQuantizeParams_AxisScaleOffset_getScaleOffsetAt(
    QairtQuantizeParams_AxisScaleOffsetHandle_t axisScaleOffsetHandle,
    uint32_t index,
    QairtQuantizeParams_ScaleOffsetHandle_t* scaleOffsetsHandle);

/**
 * @brief Set the axis for an axis scale-offset.
 *
 * @param[in] axisScaleOffsetHandle A handle to the axis scale-offset.
 *
 * @param[in] axis The axis value to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _axisScaleOffsetHandle_ is
 *           not a valid handle
 *
 * @note Use corresponding API through QairtQuantizeParams_AxisScaleOffsetV1_t.
 */
Qairt_Status_t QairtQuantizeParams_AxisScaleOffset_setAxis(
    QairtQuantizeParams_AxisScaleOffsetHandle_t axisScaleOffsetHandle, int32_t axis);

/**
 * @brief Get the axis from an axis scale-offset.
 *
 * @param[in] axisScaleOffsetHandle A handle to the axis scale-offset.
 *
 * @param[out] axis The retrieved axis value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _axisScaleOffsetHandle_ is
 *           not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _axis_ is NULL
 *
 * @note Use corresponding API through QairtQuantizeParams_AxisScaleOffsetV1_t.
 */
Qairt_Status_t QairtQuantizeParams_AxisScaleOffset_getAxis(
    QairtQuantizeParams_AxisScaleOffsetHandle_t axisScaleOffsetHandle, int32_t* axis);

// BwScaleOffset Functions

/**
 * @brief Create a bit-width scale-offset handle.
 *
 * @param[out] bwScaleOffsetHandle A handle to the created bit-width
 *                                 scale-offset.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _bwScaleOffsetHandle_ is
 *           NULL
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory allocation error while
 *           creating bit-width scale-offset
 *
 * @note Use corresponding API through QairtQuantizeParams_BwScaleOffsetV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwScaleOffset_create(
    QairtQuantizeParams_BwScaleOffsetHandle_t* bwScaleOffsetHandle);

/**
 * @brief Free a bit-width scale-offset handle.
 *
 * @param[in] bwScaleOffsetHandle A handle to the bit-width scale-offset to be
 *                                freed.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwScaleOffsetHandle_ is
 *           not a valid handle
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory deallocation error
 *
 * @note Use corresponding API through QairtQuantizeParams_BwScaleOffsetV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwScaleOffset_free(
    QairtQuantizeParams_BwScaleOffsetHandle_t bwScaleOffsetHandle);

/**
 * @brief Set the scale value of a bit-width scale-offset.
 *
 * @param[in] bwScaleOffsetHandle A handle to the bit-width scale-offset.
 *
 * @param[in] scale The scale value to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwScaleOffsetHandle_ is
 *           not a valid handle
 *
 * @note Use corresponding API through QairtQuantizeParams_BwScaleOffsetV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwScaleOffset_setScale(
    QairtQuantizeParams_BwScaleOffsetHandle_t bwScaleOffsetHandle, float scale);

/**
 * @brief Get the scale value from a bit-width scale-offset.
 *
 * @param[in] bwScaleOffsetHandle A handle to the bit-width scale-offset.
 *
 * @param[out] scale The retrieved scale value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwScaleOffsetHandle_ is
 *           not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _scale_ is NULL
 *
 * @note Use corresponding API through QairtQuantizeParams_BwScaleOffsetV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwScaleOffset_getScale(
    QairtQuantizeParams_BwScaleOffsetHandle_t bwScaleOffsetHandle, float* scale);

/**
 * @brief Set the offset value of a bit-width scale-offset.
 *
 * @param[in] bwScaleOffsetHandle A handle to the bit-width scale-offset.
 *
 * @param[in] offset The offset value to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwScaleOffsetHandle_ is
 *           not a valid handle
 *
 * @note Use corresponding API through QairtQuantizeParams_BwScaleOffsetV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwScaleOffset_setOffset(
    QairtQuantizeParams_BwScaleOffsetHandle_t bwScaleOffsetHandle, int32_t offset);

/**
 * @brief Get the offset value from a bit-width scale-offset.
 *
 * @param[in] bwScaleOffsetHandle A handle to the bit-width scale-offset.
 *
 * @param[out] offset The retrieved offset value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwScaleOffsetHandle_ is
 *           not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _offset_ is NULL
 *
 * @note Use corresponding API through QairtQuantizeParams_BwScaleOffsetV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwScaleOffset_getOffset(
    QairtQuantizeParams_BwScaleOffsetHandle_t bwScaleOffsetHandle, int32_t* offset);

/**
 * @brief Set the bit-width value of a bit-width scale-offset.
 *
 * @param[in] bwScaleOffsetHandle A handle to the bit-width scale-offset.
 *
 * @param[in] bw The bit-width value to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwScaleOffsetHandle_ is
 *           not a valid handle
 *
 * @note Use corresponding API through QairtQuantizeParams_BwScaleOffsetV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwScaleOffset_setBw(
    QairtQuantizeParams_BwScaleOffsetHandle_t bwScaleOffsetHandle, uint32_t bw);

/**
 * @brief Get the bit-width value from a bit-width scale-offset.
 *
 * @param[in] bwScaleOffsetHandle A handle to the bit-width scale-offset.
 *
 * @param[out] bw The retrieved bit-width value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwScaleOffsetHandle_ is
 *           not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _bw_ is NULL
 *
 * @note Use corresponding API through QairtQuantizeParams_BwScaleOffsetV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwScaleOffset_getBw(
    QairtQuantizeParams_BwScaleOffsetHandle_t bwScaleOffsetHandle, uint32_t* bw);

// BwAxisScaleOffset Functions

/**
 * @brief Create a bit-width axis scale-offset handle.
 *
 * @param[out] bwAxisScaleOffsetHandle A handle to the created bit-width axis
 *                                     scale-offset.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT:
 *           _bwAxisScaleOffsetHandle_ is NULL
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory allocation error while
 *           creating bit-width axis scale-offset
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_BwAxisScaleOffsetV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwAxisScaleOffset_create(
    QairtQuantizeParams_BwAxisScaleOffsetHandle_t* bwAxisScaleOffsetHandle);

/**
 * @brief Free a bit-width axis scale-offset handle.
 *
 * @param[in] bwAxisScaleOffsetHandle A handle to the bit-width axis
 *                                    scale-offset to be freed.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwAxisScaleOffsetHandle_
 *           is not a valid handle
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory deallocation error
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_BwAxisScaleOffsetV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwAxisScaleOffset_free(
    QairtQuantizeParams_BwAxisScaleOffsetHandle_t bwAxisScaleOffsetHandle);

/**
 * @brief Get the number of offsets in a bit-width axis scale-offset.
 *
 * @param[in] bwAxisScaleOffsetHandle A handle to the bit-width axis
 *                                    scale-offset.
 *
 * @param[out] numOffsets The retrieved number of offsets.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwAxisScaleOffsetHandle_
 *           is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _numOffsets_ is NULL
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_BwAxisScaleOffsetV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwAxisScaleOffset_getNumOffsets(
    QairtQuantizeParams_BwAxisScaleOffsetHandle_t bwAxisScaleOffsetHandle, uint32_t* numOffsets);

/**
 * @brief Get the number of scales in a bit-width axis scale-offset.
 *
 * @param[in] bwAxisScaleOffsetHandle A handle to the bit-width axis
 *                                    scale-offset.
 *
 * @param[out] numScales The retrieved number of scales.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwAxisScaleOffsetHandle_
 *           is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _numScales_ is NULL
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_BwAxisScaleOffsetV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwAxisScaleOffset_getNumScales(
    QairtQuantizeParams_BwAxisScaleOffsetHandle_t bwAxisScaleOffsetHandle, uint32_t* numScales);

/**
 * @brief Set the scale array for a bit-width axis scale-offset.
 *
 * @param[in] bwAxisScaleOffsetHandle A handle to the bit-width axis
 *                                    scale-offset.
 *
 * @param[in] scale Array of scale values.
 *
 * @param[in] numScales Number of scale values in the array.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwAxisScaleOffsetHandle_
 *           is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _scale_ is NULL or
 *           _numScales_ is 0
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory allocation error while
 *           setting scales
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_BwAxisScaleOffsetV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwAxisScaleOffset_setScales(
    QairtQuantizeParams_BwAxisScaleOffsetHandle_t bwAxisScaleOffsetHandle,
    float* scale,
    uint32_t numScales);

/**
 * @brief Get the scale array from a bit-width axis scale-offset.
 *
 * @param[in] bwAxisScaleOffsetHandle A handle to the bit-width axis
 *                                    scale-offset.
 *
 * @param[in] index Index of scale that should be retrieved.
 *
 * @param[out] scale The retrieved scale value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwAxisScaleOffsetHandle_
 *           is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _scale_ is NULL or
 *           _numScales_ is 0
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_BwAxisScaleOffsetV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwAxisScaleOffset_getScaleAt(
    QairtQuantizeParams_BwAxisScaleOffsetHandle_t bwAxisScaleOffsetHandle,
    uint32_t index,
    float* scale);

/**
 * @brief Set the offset array for a bit-width axis scale-offset.
 *
 * @param[in] bwAxisScaleOffsetHandle A handle to the bit-width axis
 *                                    scale-offset.
 *
 * @param[in] offsets Array of offset values.
 *
 * @param[in] numOffsets Number of offset values in the array.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwAxisScaleOffsetHandle_
 *           is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _offsets_ is NULL or
 *           _numOffsets_ is 0
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory allocation error while
 *           setting offsets
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_BwAxisScaleOffsetV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwAxisScaleOffset_setOffsets(
    QairtQuantizeParams_BwAxisScaleOffsetHandle_t bwAxisScaleOffsetHandle,
    int32_t* offsets,
    uint32_t numOffsets);

/**
 * @brief Get the offset array from a bit-width axis scale-offset.
 *
 * @param[in] bwAxisScaleOffsetHandle A handle to the bit-width axis
 *                                    scale-offset.
 *
 * @param[in] index Index of offset that should be retrieved.
 *
 * @param[out] offset The retrieved offset value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwAxisScaleOffsetHandle_
 *           is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _offset_ is NULL or
 *           _numOffsets_ is 0
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_BwAxisScaleOffsetV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwAxisScaleOffset_getOffsetAt(
    QairtQuantizeParams_BwAxisScaleOffsetHandle_t bwAxisScaleOffsetHandle,
    uint32_t index,
    int32_t* offset);

/**
 * @brief Set the axis for a bit-width axis scale-offset.
 *
 * @param[in] bwAxisScaleOffsetHandle A handle to the bit-width axis
 *                                    scale-offset.
 *
 * @param[in] axis The axis value to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwAxisScaleOffsetHandle_
 *           is not a valid handle
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_BwAxisScaleOffsetV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwAxisScaleOffset_setAxis(
    QairtQuantizeParams_BwAxisScaleOffsetHandle_t bwAxisScaleOffsetHandle, int32_t axis);

/**
 * @brief Get the axis from a bit-width axis scale-offset.
 *
 * @param[in] bwAxisScaleOffsetHandle A handle to the bit-width axis
 *                                    scale-offset.
 *
 * @param[out] axis The retrieved axis value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwAxisScaleOffsetHandle_
 *           is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _axis_ is NULL
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_BwAxisScaleOffsetV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwAxisScaleOffset_getAxis(
    QairtQuantizeParams_BwAxisScaleOffsetHandle_t bwAxisScaleOffsetHandle, int32_t* axis);

/**
 * @brief Set the bit-width for a bit-width axis scale-offset.
 *
 * @param[in] bwAxisScaleOffsetHandle A handle to the bit-width axis
 *                                    scale-offset.
 *
 * @param[in] bw The bit-width value to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwAxisScaleOffsetHandle_
 *           is not a valid handle
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_BwAxisScaleOffsetV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwAxisScaleOffset_setBw(
    QairtQuantizeParams_BwAxisScaleOffsetHandle_t bwAxisScaleOffsetHandle, uint32_t bw);

/**
 * @brief Get the bit-width from a bit-width axis scale-offset.
 *
 * @param[in] bwAxisScaleOffsetHandle A handle to the bit-width axis
 *                                    scale-offset.
 *
 * @param[out] bw The retrieved bit-width value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwAxisScaleOffsetHandle_
 *           is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _bw_ is NULL
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_BwAxisScaleOffsetV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwAxisScaleOffset_getBw(
    QairtQuantizeParams_BwAxisScaleOffsetHandle_t bwAxisScaleOffsetHandle, uint32_t* bw);

// BwAxisScaleOffsetMapped Functions

/**
 * @brief Create a bit-width axis scale-offset mapped handle.
 *
 * @param[out] bwAxisScaleOffsetMappedHandle A handle to the created bit-width
 *                                           axis scale-offset mapped.
 *
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
Qairt_Status_t QairtQuantizeParams_BwAxisScaleOffsetMapped_create(
    QairtQuantizeParams_BwAxisScaleOffsetMappedHandle_t* bwAxisScaleOffsetMappedHandle);

/**
 * @brief Free a bit-width axis scale-offset mapped handle.
 *
 * @param[in] bwAxisScaleOffsetMappedHandle A handle to the bit-width axis
 *                                          scale-offset mapped to be freed.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE:
 *           _bwAxisScaleOffsetMappedHandle_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory deallocation error
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_BwAxisScaleOffsetMappedV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwAxisScaleOffsetMapped_free(
    QairtQuantizeParams_BwAxisScaleOffsetMappedHandle_t bwAxisScaleOffsetMappedHandle);

/**
 * @brief Get the number of offsets in a bit-width axis scale-offset mapped.
 *
 * @param[in] bwAxisScaleOffsetMappedHandle A handle to the bit-width axis
 *                                          scale-offset mapped.
 *
 * @param[out] numOffsets The retrieved number of offsets.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE:
 *           _bwAxisScaleOffsetMappedHandle_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _numOffsets_ is NULL
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_BwAxisScaleOffsetMappedV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwAxisScaleOffsetMapped_getNumOffsets(
    QairtQuantizeParams_BwAxisScaleOffsetMappedHandle_t bwAxisScaleOffsetMappedHandle,
    uint32_t* numOffsets);

/**
 * @brief Get the number of scales in a bit-width axis scale-offset mapped.
 *
 * @param[in] bwAxisScaleOffsetMappedHandle A handle to the bit-width axis
 *                                          scale-offset mapped.
 *
 * @param[out] numScales The retrieved number of scales.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE:
 *           _bwAxisScaleOffsetMappedHandle_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _numScales_ is NULL
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_BwAxisScaleOffsetMappedV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwAxisScaleOffsetMapped_getNumScales(
    QairtQuantizeParams_BwAxisScaleOffsetMappedHandle_t bwAxisScaleOffsetMappedHandle,
    uint32_t* numScales);

/**
 * @brief Set the scale array for a bit-width axis scale-offset mapped.
 *
 * @param[in] bwAxisScaleOffsetMappedHandle A handle to the bit-width axis
 *                                          scale-offset mapped.
 *
 * @param[in] scale Array of scale values.
 *
 * @param[in] numScales Number of scale values in the array.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE:
 *           _bwAxisScaleOffsetMappedHandle_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _scale_ is NULL or
 *           _numScales_ is 0
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory allocation error while
 *           setting scales
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_BwAxisScaleOffsetMappedV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwAxisScaleOffsetMapped_setScales(
    QairtQuantizeParams_BwAxisScaleOffsetMappedHandle_t bwAxisScaleOffsetMappedHandle,
    float* scale,
    uint32_t numScales);

/**
 * @brief Get the scale array from a bit-width axis scale-offset mapped.
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
Qairt_Status_t QairtQuantizeParams_BwAxisScaleOffsetMapped_getScaleAt(
    QairtQuantizeParams_BwAxisScaleOffsetMappedHandle_t bwAxisScaleOffsetMappedHandle,
    uint32_t index,
    float* scale);

/**
 * @brief Set the offset array for a bit-width axis scale-offset mapped.
 *
 * @param[in] bwAxisScaleOffsetMappedHandle A handle to the bit-width axis
 *                                          scale-offset mapped.
 *
 * @param[in] offsets Array of offset values.
 *
 * @param[in] numOffsets Number of offset values in the array.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE:
 *           _bwAxisScaleOffsetMappedHandle_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _offsets_ is NULL or
 *           _numOffsets_ is 0
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory allocation error while
 *           setting offsets
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_BwAxisScaleOffsetMappedV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwAxisScaleOffsetMapped_setOffsets(
    QairtQuantizeParams_BwAxisScaleOffsetMappedHandle_t bwAxisScaleOffsetMappedHandle,
    int32_t* offsets,
    uint32_t numOffsets);

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
Qairt_Status_t QairtQuantizeParams_BwAxisScaleOffsetMapped_getOffsetAt(
    QairtQuantizeParams_BwAxisScaleOffsetMappedHandle_t bwAxisScaleOffsetMappedHandle,
    uint32_t index,
    int32_t* offset);

/**
 * @brief Set the axis for a bit-width axis scale-offset mapped.
 *
 * @param[in] bwAxisScaleOffsetMappedHandle A handle to the bit-width axis
 *                                          scale-offset mapped.
 *
 * @param[in] axis The axis value to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE:
 *           _bwAxisScaleOffsetMappedHandle_ is not a valid handle
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_BwAxisScaleOffsetMappedV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwAxisScaleOffsetMapped_setAxis(
    QairtQuantizeParams_BwAxisScaleOffsetMappedHandle_t bwAxisScaleOffsetMappedHandle,
    int32_t axis);

/**
 * @brief Get the axis from a bit-width axis scale-offset mapped.
 *
 * @param[in] bwAxisScaleOffsetMappedHandle A handle to the bit-width axis
 *                                          scale-offset mapped.
 *
 * @param[out] axis The retrieved axis value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE:
 *           _bwAxisScaleOffsetMappedHandle_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _axis_ is NULL
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_BwAxisScaleOffsetMappedV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwAxisScaleOffsetMapped_getAxis(
    QairtQuantizeParams_BwAxisScaleOffsetMappedHandle_t bwAxisScaleOffsetMappedHandle,
    int32_t* axis);

/**
 * @brief Set the bit-width for a bit-width axis scale-offset mapped.
 *
 * @param[in] bwAxisScaleOffsetMappedHandle A handle to the bit-width axis
 *                                          scale-offset mapped.
 *
 * @param[in] bw The bit-width value to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE:
 *           _bwAxisScaleOffsetMappedHandle_ is not a valid handle
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_BwAxisScaleOffsetMappedV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwAxisScaleOffsetMapped_setBw(
    QairtQuantizeParams_BwAxisScaleOffsetMappedHandle_t bwAxisScaleOffsetMappedHandle, uint32_t bw);

/**
 * @brief Get the bit-width from a bit-width axis scale-offset mapped.
 *
 * @param[in] bwAxisScaleOffsetMappedHandle A handle to the bit-width axis
 *                                          scale-offset mapped.
 *
 * @param[out] bw The retrieved bit-width value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE:
 *           _bwAxisScaleOffsetMappedHandle_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _bw_ is NULL
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_BwAxisScaleOffsetMappedV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwAxisScaleOffsetMapped_getBw(
    QairtQuantizeParams_BwAxisScaleOffsetMappedHandle_t bwAxisScaleOffsetMappedHandle,
    uint32_t* bw);

/**
 * @brief Set the mapping for a bit-width axis scale-offset mapped.
 *
 * @param[in] bwAxisScaleOffsetMappedHandle A handle to the bit-width axis
 *                                          scale-offset mapped.
 *
 * @param[in] mapping The mapping value to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE:
 *           _bwAxisScaleOffsetMappedHandle_ is not a valid handle
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_BwAxisScaleOffsetMappedV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwAxisScaleOffsetMapped_setMapping(
    QairtQuantizeParams_BwAxisScaleOffsetMappedHandle_t bwAxisScaleOffsetMappedHandle,
    Qairt_QuantizationEncodingMapping_t mapping);

/**
 * @brief Get the mapping from a bit-width axis scale-offset mapped.
 *
 * @param[in] bwAxisScaleOffsetMappedHandle A handle to the bit-width axis
 *                                          scale-offset mapped.
 *
 * @param[out] mapping The retrieved mapping value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE:
 *           _bwAxisScaleOffsetMappedHandle_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _mapping_ is NULL
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_BwAxisScaleOffsetMappedV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwAxisScaleOffsetMapped_getMapping(
    QairtQuantizeParams_BwAxisScaleOffsetMappedHandle_t bwAxisScaleOffsetMappedHandle,
    Qairt_QuantizationEncodingMapping_t* mapping);

// BlockEncoding Functions

/**
 * @brief Create a block encoding handle.
 *
 * @param[out] blockEncodingHandle A handle to the created block encoding.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _blockEncodingHandle_ is
 *           NULL
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory allocation error while
 *           creating block encoding
 *
 * @note Use corresponding API through QairtQuantizeParams_BlockEncodingV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BlockEncoding_create(
    QairtQuantizeParams_BlockEncodingHandle_t* blockEncodingHandle);

/**
 * @brief Free a block encoding handle.
 *
 * @param[in] blockEncodingHandle A handle to the block encoding to be freed.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _blockEncodingHandle_ is
 *           not a valid handle
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory deallocation error
 *
 * @note Use corresponding API through QairtQuantizeParams_BlockEncodingV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BlockEncoding_free(
    QairtQuantizeParams_BlockEncodingHandle_t blockEncodingHandle);

/**
 * @brief Set the block size array for a block encoding.
 *
 * @param[in] blockEncodingHandle A handle to the block encoding.
 *
 * @param[in] blockSizes Array of block size values.
 *
 * @param[in] numBlocks Number of block sizes in the array.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _blockEncodingHandle_ is
 *           not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _blockSizes_ is NULL or
 *           _numBlocks_ is 0
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory allocation error while
 *           setting block sizes
 *
 * @note Use corresponding API through QairtQuantizeParams_BlockEncodingV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BlockEncoding_setBlockSize(
    QairtQuantizeParams_BlockEncodingHandle_t blockEncodingHandle,
    uint32_t* blockSizes,
    uint32_t numBlocks);

/**
 * @brief Get the number of block sizes from a block encoding
 *
 * @param[in] blockEncodingHandle A handle to the block encoding.
 *
 * @param[out] numBlockSize The retrieved number of block sizes.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _blockEncodingHandle_ is
 *           not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _numBlockSize_ is
 *           NULL
 *
 * @note Use corresponding API through QairtQuantizeParams_BlockEncodingV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BlockEncoding_getNumBlockSize(
    QairtQuantizeParams_BlockEncodingHandle_t blockEncodingHandle, uint32_t* numBlockSize);

/**
 * @brief Get the block size array from a block encoding.
 *
 * @param[in] blockEncodingHandle A handle to the block encoding.
 *
 * @param[in] index Index of the blockSize to retrieve.
 *
 * @param[out] blockSize The retrieved blockSize .
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _blockEncodingHandle_ is
 *           not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _blockSizes_ is NULL or
 *           _numBlocks_ is NULL
 *
 * @note Use corresponding API through QairtQuantizeParams_BlockEncodingV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BlockEncoding_getBlockSizeAt(
    QairtQuantizeParams_BlockEncodingHandle_t blockEncodingHandle,
    uint32_t index,
    uint32_t* blockSize);

/**
 * @brief Set the scale-offset array for a block encoding.
 *
 * @param[in] blockEncodingHandle A handle to the block encoding.
 *
 * @param[in] scaleOffsetsHandle Array of scale-offset handles.
 *
 * @param[in] numScaleOffsets Number of scale-offset handles in the array.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _blockEncodingHandle_ or a
 *           handle in _scaleOffsetsHandle_ array is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _scaleOffsetsHandle_ is
 *           NULL or _numScaleOffsets_ is 0
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory allocation error while
 *           setting scale-offsets
 *
 * @note Use corresponding API through QairtQuantizeParams_BlockEncodingV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BlockEncoding_setScaleOffsets(
    QairtQuantizeParams_BlockEncodingHandle_t blockEncodingHandle,
    QairtQuantizeParams_ScaleOffsetHandle_t* scaleOffsetsHandle,
    uint32_t numScaleOffsets);

/**
 * @brief Get the number of block sizes from a block encoding
 *
 * @param[in] blockEncodingHandle A handle to the block encoding.
 *
 * @param[out] numScaleOffsets The retrieved number of scale offsets.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _blockEncodingHandle_ is
 *           not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _numScaleOffsets_ is
 *           NULL
 *
 * @note Use corresponding API through QairtQuantizeParams_BlockEncodingV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BlockEncoding_getNumScaleOffsets(
    QairtQuantizeParams_BlockEncodingHandle_t blockEncodingHandle, uint32_t* numScaleOffsets);

/**
 * @brief Get the scale-offset array from a block encoding.
 *
 * @param[in] blockEncodingHandle A handle to the block encoding.
 *
 * @param[in] index Index of the scaleOffsets to retrieve.
 *
 * @param[out] scaleOffsetsHandle The retrieved scaleOffset Handle .
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _blockEncodingHandle_ is
 *           not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _scaleOffsetsHandle_ is
 *           NULL or _numScaleOffset_ is NULL
 *
 * @note Use corresponding API through QairtQuantizeParams_BlockEncodingV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BlockEncoding_getScaleOffsetAt(
    QairtQuantizeParams_BlockEncodingHandle_t blockEncodingHandle,
    uint32_t index,
    QairtQuantizeParams_ScaleOffsetHandle_t* scaleOffsetsHandle);

// BlockwiseExpansion Functions

/**
 * @brief Create a blockwise expansion handle.
 *
 * @param[out] blockwiseExpansionHandle A handle to the created blockwise
 *                                      expansion.
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT:
 *           _blockwiseExpansionHandle_ is NULL
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory allocation error while
 *           creating blockwise expansion
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_BlockwiseExpansionV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BlockwiseExp_create(
    QairtQuantizeParams_BlockwiseExpansionHandle_t* blockwiseExpansionHandle);

/**
 * @brief Free a blockwise expansion handle.
 *
 * @param[in] blockwiseExpansionHandle A handle to the blockwise expansion to be
 *                                     freed.
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _blockwiseExpansionHandle_
 *           is not a valid handle
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory deallocation error
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_BlockwiseExpansionV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BlockwiseExp_free(
    QairtQuantizeParams_BlockwiseExpansionHandle_t blockwiseExpansionHandle);

/**
 * @brief Set the axis for a blockwise expansion.
 *
 * @param[in] blockwiseExpansionHandle A handle to the blockwise expansion.
 *
 * @param[in] axis The axis value to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _blockwiseExpansionHandle_
 *           is not a valid handle
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_BlockwiseExpansionV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BlockwiseExp_setAxis(
    QairtQuantizeParams_BlockwiseExpansionHandle_t blockwiseExpansionHandle, int32_t axis);

/**
 * @brief Get the axis from a blockwise expansion.
 *
 * @param[in] blockwiseExpansionHandle A handle to the blockwise expansion.
 *
 * @param[out] axis The retrieved axis value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _blockwiseExpansionHandle_
 *           is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _axis_ is NULL
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_BlockwiseExpansionV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BlockwiseExp_getAxis(
    QairtQuantizeParams_BlockwiseExpansionHandle_t blockwiseExpansionHandle, int32_t* axis);

/**
 * @brief Set the scale-offset array for a blockwise expansion.
 *
 * @param[in] blockwiseExpansionHandle A handle to the blockwise expansion.
 *
 * @param[in] scaleOffsetsHandle Array of scale-offset handles.
 *
 * @param[in] numScaleOffsets Number of scale-offset handles in the array.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _blockwiseExpansionHandle_
 *           or a handle in _scaleOffsetsHandle_ array is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _scaleOffsetsHandle_ is
 *           NULL or _numScaleOffsets_ is 0
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory allocation error while
 *           setting scale-offsets
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_BlockwiseExpansionV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BlockwiseExp_setScaleOffsets(
    QairtQuantizeParams_BlockwiseExpansionHandle_t blockwiseExpansionHandle,
    QairtQuantizeParams_ScaleOffsetHandle_t* scaleOffsetsHandle,
    uint32_t numScaleOffsets);

/**
 * @brief Get the storage type from a blockwise expansion.
 *
 * @param[in] blockwiseExpansionHandle A handle to the blockwise expansion.
 *
 * @param[out] storageType The retrieved storage type.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _blockwiseExpansionHandle_
 *           is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _storageType_ is NULL
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_BlockwiseExpansionV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BlockwiseExp_getStorageType(
    QairtQuantizeParams_BlockwiseExpansionHandle_t blockwiseExpansionHandle,
    Qairt_BlockwiseExpansionBlockScaleStorageType_t* storageType);

/**
 * @brief Get the number of scale offsets from a blockwise expansion.
 *
 * @param[in] blockwiseExpansionHandle A handle to the blockwise expansion.
 *
 * @param[out] numScaleOffsets The retrieved number of scale offsets.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _blockwiseExpansionHandle_
 *           is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _numScaleOffsets_ is
 *           NULL
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_BlockwiseExpansionV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BlockwiseExp_getNumScaleOffsets(
    QairtQuantizeParams_BlockwiseExpansionHandle_t blockwiseExpansionHandle,
    uint32_t* numScaleOffsets);

/**
 * @brief Get the scale-offset array from a blockwise expansion.
 *
 * @param[in] blockwiseExpansionHandle A handle to the blockwise expansion.
 *
 * @param[in] index Index of the scaleOffsets to retrieve.
 *
 * @param[out] scaleOffsetHandle The retrieved scaleOffset Handle.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _blockwiseExpansionHandle_
 *           is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _scaleOffsetHandle_ is
 *           NULL or _index_ is out of range
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_BlockwiseExpansionV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BlockwiseExp_getScaleOffsetAt(
    QairtQuantizeParams_BlockwiseExpansionHandle_t blockwiseExpansionHandle,
    uint32_t index,
    QairtQuantizeParams_ScaleOffsetHandle_t* scaleOffsetHandle);

/**
 * @brief Set the number of blocks per axis for a blockwise expansion.
 *
 * @param[in] blockwiseExpansionHandle A handle to the blockwise expansion.
 *
 * @param[in] numBlocks The number of blocks per axis to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _blockwiseExpansionHandle_
 *           is not a valid handle
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_BlockwiseExpansionV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BlockwiseExp_setNumBlocksPerAxis(
    QairtQuantizeParams_BlockwiseExpansionHandle_t blockwiseExpansionHandle, uint32_t numBlocks);

/**
 * @brief Get the number of blocks per axis from a blockwise expansion.
 *
 * @param[in] blockwiseExpansionHandle A handle to the blockwise expansion.
 *
 * @param[out] numBlocks The retrieved number of blocks per axis.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _blockwiseExpansionHandle_
 *           is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _numBlocks_ is NULL
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_BlockwiseExpansionV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BlockwiseExp_getNumBlocksPerAxis(
    QairtQuantizeParams_BlockwiseExpansionHandle_t blockwiseExpansionHandle, uint32_t* numBlocks);

/**
 * @brief Set the block scale bit-width for a blockwise expansion.
 *
 * @param[in] blockwiseExpansionHandle A handle to the blockwise expansion.
 *
 * @param[in] bitwidth The bit-width value to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _blockwiseExpansionHandle_
 *           is not a valid handle
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_BlockwiseExpansionV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BlockwiseExp_setBlockScaleBw(
    QairtQuantizeParams_BlockwiseExpansionHandle_t blockwiseExpansionHandle, uint32_t bitwidth);

/**
 * @brief Get the block scale bit-width from a blockwise expansion.
 *
 * @param[in] blockwiseExpansionHandle A handle to the blockwise expansion.
 *
 * @param[out] bitwidth The retrieved bit-width value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _blockwiseExpansionHandle_
 *           is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _bitwidth_ is NULL
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_BlockwiseExpansionV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BlockwiseExp_getBlockScaleBw(
    QairtQuantizeParams_BlockwiseExpansionHandle_t blockwiseExpansionHandle, uint32_t* bitwidth);

/**
 * @brief Set the 8-bit block scale array for a blockwise expansion.
 *
 * @param[in] blockwiseExpansionHandle A handle to the blockwise expansion.
 *
 * @param[in] blockScale8 The 8-bit scale value array to set
 *
 * @param[in] numBlocks Size of the block Scale 8 array
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _blockwiseExpansionHandle_
 *           is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _blockScale8_ is NULL or
 *           _numBlocks_ is 0
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory allocation error while
 *           setting block scales
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_BlockwiseExpansionV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BlockwiseExp_setBlockScale8(
    QairtQuantizeParams_BlockwiseExpansionHandle_t blockwiseExpansionHandle,
    uint8_t* blockScale8,
    uint32_t numBlocks);

/**
 * @brief Get the number of 8-bit block scales from a blockwise expansion.
 *
 * @param[in] blockwiseExpansionHandle A handle to the blockwise expansion.
 *
 * @param[out] numBlockScale8 The retrieved number of block scale 8.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _blockwiseExpansionHandle_ is
 *           not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _numBlockScale8_ is
 *           NULL
 *
 * @note Use corresponding API through QairtQuantizeParams_BlockwiseExpansionV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BlockwiseExp_getNumBlockScale8(
    QairtQuantizeParams_BlockwiseExpansionHandle_t blockwiseExpansionHandle,
    uint32_t* numBlockScale8);

/**
 * @brief Get the 8-bit block scale from a blockwise expansion.
 *
 * @param[in] blockwiseExpansionHandle A handle to the blockwise expansion.
 *
 * @param[in] index Index of the blockScale8 to retrieve.
 *
 * @param[out] blockScale8 The retrieved blockScale8  .
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _blockwiseExpansionHandle_
 *           is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _blockScale8_ is NULL
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_BlockwiseExpansionV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BlockwiseExp_getBlockScale8At(
    QairtQuantizeParams_BlockwiseExpansionHandle_t blockwiseExpansionHandle,
    uint32_t index,
    uint8_t* blockScale8);

/**
 * @brief Set the 16-bit block scale array for a blockwise expansion.
 *
 * @param[in] blockwiseExpansionHandle A handle to the blockwise expansion.
 *
 * @param[in] blockScale16 The 16-bit scale value array to set
 *
 * @param[in] numBlocks Size of the block Scale 16 array
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _blockwiseExpansionHandle_
 *           is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _blockScale16_ is NULL or
 *           _numBlocks_ is 0
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory allocation error while
 *           setting block scales
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_BlockwiseExpansionV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BlockwiseExp_setBlockScale16(
    QairtQuantizeParams_BlockwiseExpansionHandle_t blockwiseExpansionHandle,
    uint16_t* blockScale16,
    uint32_t numBlocks);

/**
 * @brief Get the number of scale offsets from a blockwise expansion
 *
 * @param[in] blockwiseExpansionHandle A handle to the blockwise expansion.
 *
 * @param[out] numBlockScale16 The retrieved number of block scale 16.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _blockwiseExpansionHandle_ is
 *           not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _numBlockScale16_ is
 *           NULL
 *
 * @note Use corresponding API through QairtQuantizeParams_BlockwiseExpansionV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BlockwiseExp_getNumBlockScale16(
    QairtQuantizeParams_BlockwiseExpansionHandle_t blockwiseExpansionHandle,
    uint32_t* numBlockScale16);

/**
 * @brief Get the 16-bit block scale from a blockwise expansion.
 *
 * @param[in] blockwiseExpansionHandle A handle to the blockwise expansion.
 *
 * @param[in] index Index of the blockScale16 to retrieve.
 *
 * @param[out] blockScale16 The retrieved blockScale16.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _blockwiseExpansionHandle_
 *           is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _blockSize16_ is NULL
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_BlockwiseExpansionV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BlockwiseExp_getBlockScale16At(
    QairtQuantizeParams_BlockwiseExpansionHandle_t blockwiseExpansionHandle,
    uint32_t index,
    uint16_t* blockScale16);

// VectorEncoding Functions

/**
 * @brief Create a vector encoding handle.
 *
 * @param[out] vectorEncodingHandle A handle to the created vector encoding.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _vectorEncodingHandle_
 *           is NULL
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory allocation error while
 *           creating vector encoding
 *
 * @note Use corresponding API through QairtQuantizeParams_VectorEncodingV1_t.
 */
Qairt_Status_t QairtQuantizeParams_VectorEncoding_create(
    QairtQuantizeParams_VectorEncodingHandle_t* vectorEncodingHandle);

/**
 * @brief Free a vector encoding handle.
 *
 * @param[in] vectorEncodingHandle A handle to the vector encoding to be freed.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _vectorEncodingHandle_ is
 *           not a valid handle
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory deallocation error
 *
 * @note Use corresponding API through QairtQuantizeParams_VectorEncodingV1_t.
 */
Qairt_Status_t QairtQuantizeParams_VectorEncoding_free(
    QairtQuantizeParams_VectorEncodingHandle_t vectorEncodingHandle);

/**
 * @brief Set the bit-width scale-offset array for a vector encoding.
 *
 * @param[in] vectorEncodingHandle A handle to the vector encoding.
 *
 * @param[in] bwAxisScaleOffsetHandle bit-width axis scale-offset handle
 *
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _vectorEncodingHandle_ or
 *           _bwAxisScaleOffsetHandle_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory allocation error while
 *           setting scale-offsets
 *
 * @note Use corresponding API through QairtQuantizeParams_VectorEncodingV1_t.
 */
Qairt_Status_t QairtQuantizeParams_VectorEncoding_setBwAxisScaleOffset(
    QairtQuantizeParams_VectorEncodingHandle_t vectorEncodingHandle,
    QairtQuantizeParams_BwAxisScaleOffsetHandle_t bwAxisScaleOffsetHandle);

/**
 * @brief Get the bit-width scale-offset array from a vector encoding.
 *
 * @param[in] vectorEncodingHandle A handle to the vector encoding.
 *
 * @param[out] bwAxisScaleOffsetHandle The retrieved bit-width axis
 *                              scale-offset handle.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _vectorEncodingHandle_ is
 *           not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _bwAxisScaleOffsetHandle_ is
 *           NULL
 *
 * @note Use corresponding API through QairtQuantizeParams_VectorEncodingV1_t.
 */
Qairt_Status_t QairtQuantizeParams_VectorEncoding_getBwAxisScaleOffset(
    QairtQuantizeParams_VectorEncodingHandle_t vectorEncodingHandle,
    QairtQuantizeParams_BwAxisScaleOffsetHandle_t* bwAxisScaleOffsetHandle);

/**
 * @brief Set the rows per block for a vector encoding.
 *
 * @param[in] vectorEncodingHandle A handle to the vector encoding.
 *
 * @param[in] rows The number of rows per block to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _vectorEncodingHandle_ is
 *           not a valid handle
 *
 * @note Use corresponding API through QairtQuantizeParams_VectorEncodingV1_t.
 */
Qairt_Status_t QairtQuantizeParams_VectorEncoding_setRowsPerBlock(
    QairtQuantizeParams_VectorEncodingHandle_t vectorEncodingHandle, uint32_t rows);

/**
 * @brief Get the rows per block from a vector encoding.
 *
 * @param[in] vectorEncodingHandle A handle to the vector encoding.
 *
 * @param[out] rows The retrieved number of rows per block.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _vectorEncodingHandle_ is
 *           not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _rows_ is NULL
 *
 * @note Use corresponding API through QairtQuantizeParams_VectorEncodingV1_t.
 */
Qairt_Status_t QairtQuantizeParams_VectorEncoding_getRowsPerBlock(
    QairtQuantizeParams_VectorEncodingHandle_t vectorEncodingHandle, uint32_t* rows);

/**
 * @brief Set the columns per block for a vector encoding.
 *
 * @param[in] vectorEncodingHandle A handle to the vector encoding.
 *
 * @param[in] cols The number of columns per block to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _vectorEncodingHandle_ is
 *           not a valid handle
 *
 * @note Use corresponding API through QairtQuantizeParams_VectorEncodingV1_t.
 */
Qairt_Status_t QairtQuantizeParams_VectorEncoding_setColsPerBlock(
    QairtQuantizeParams_VectorEncodingHandle_t vectorEncodingHandle, uint32_t cols);

/**
 * @brief Get Columns per block from a vector encoding.
 *
 * @param[in] vectorEncodingHandle A handle to the vector encoding.
 *
 * @param[out] cols The retrieved columns per block.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _vectorEncodingHandle_ is
 *           not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _cols_ is NULL
 *
 * @note Use corresponding API through QairtQuantizeParams_VectorEncodingV1_t.
 */
Qairt_Status_t QairtQuantizeParams_VectorEncoding_getColsPerBlock(
    QairtQuantizeParams_VectorEncodingHandle_t vectorEncodingHandle, uint32_t* cols);
/**
 * @brief Set vector stride from a vector encoding.
 *
 * @param[in] vectorEncodingHandle A handle to the vector encoding.
 *
 * @param[in] vectorStride The vector stride to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _vectorEncodingHandle_ is
 *           not a valid handle
 *
 * @note Use corresponding API through QairtQuantizeParams_VectorEncodingV1_t.
 */
Qairt_Status_t QairtQuantizeParams_VectorEncoding_setVectorStride(
    QairtQuantizeParams_VectorEncodingHandle_t vectorEncodingHandle, uint32_t vectorStride);

/**
 * @brief Get vector stride from a vector encoding.
 *
 * @param[in] vectorEncodingHandle A handle to the vector encoding.
 *
 * @param[out] vectorStride The retrieved vector stride.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _vectorEncodingHandle_ is
 *           not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _vectorStride_ is NULL
 *
 * @note Use corresponding API through QairtQuantizeParams_VectorEncodingV1_t.
 */
Qairt_Status_t QairtQuantizeParams_VectorEncoding_getVectorStride(
    QairtQuantizeParams_VectorEncodingHandle_t vectorEncodingHandle, uint32_t* vectorStride);
/**
 * @brief Set vector dimension from a vector encoding.
 *
 * @param[in] vectorEncodingHandle A handle to the vector encoding.
 *
 * @param[in] vectorDimension The vector dimension to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _vectorEncodingHandle_ is
 *           not a valid handle
 *
 * @note Use corresponding API through QairtQuantizeParams_VectorEncodingV1_t.
 */
Qairt_Status_t QairtQuantizeParams_VectorEncoding_setVectorDimension(
    QairtQuantizeParams_VectorEncodingHandle_t vectorEncodingHandle, uint32_t vectorDimension);

/**
 * @brief Get the vector dimension from a vector encoding.
 *
 * @param[in] vectorEncodingHandle A handle to the vector encoding.
 *
 * @param[out] vectorDimension The retrieved vector dimension.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _vectorEncodingHandle_ is
 *           not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _vectorDimension_ is NULL
 *
 * @note Use corresponding API through QairtQuantizeParams_VectorEncodingV1_t.
 */
Qairt_Status_t QairtQuantizeParams_VectorEncoding_getVectorDimension(
    QairtQuantizeParams_VectorEncodingHandle_t vectorEncodingHandle, uint32_t* vectorDimension);

/**
 * @brief Set index bit-width for a vector encoding.
 *
 * @param[in] vectorEncodingHandle A handle to the vector encoding.
 *
 * @param[in] indexBw The index bit-width to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _vectorEncodingHandle_ is
 *           not a valid handle
 *
 * @note Use corresponding API through QairtQuantizeParams_VectorEncodingV1_t.
 */
Qairt_Status_t QairtQuantizeParams_VectorEncoding_setIndexBw(
    QairtQuantizeParams_VectorEncodingHandle_t vectorEncodingHandle, uint32_t indexBw);

/**
 * @brief Get the index bit-width from a vector encoding.
 *
 * @param[in] vectorEncodingHandle A handle to the vector encoding.
 *
 * @param[out] indexBw The retrieved index bit-width.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _vectorEncodingHandle_ is
 *           not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _indexBw_ is NULL
 *
 * @note Use corresponding API through QairtQuantizeParams_VectorEncodingV1_t.
 */
Qairt_Status_t QairtQuantizeParams_VectorEncoding_getIndexBw(
    QairtQuantizeParams_VectorEncodingHandle_t vectorEncodingHandle, uint32_t* indexBw);

// MicroscalingEncoding Functions

/**
 * @brief Create a Microscaling encoding handle.
 *
 * @param[out] microscalingEncodingHandle A handle to the created Microscaling
 *                                        encoding.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT:
 *           _microscalingEncodingHandle_ is NULL
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory allocation error while
 *           creating Microscaling encoding
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_MicroscalingEncodingV1_t.
 */
Qairt_Status_t QairtQuantizeParams_MicroscalingEncoding_create(
    QairtQuantizeParams_MicroscalingEncodingHandle_t* microscalingEncodingHandle);

/**
 * @brief Free a Microscaling encoding handle.
 *
 * @param[in] microscalingEncodingHandle A handle to the Microscaling encoding
 *                                       to be freed.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _microscalingEncodingHandle_
 *           is not a valid handle
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory deallocation error
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_MicroscalingEncodingV1_t.
 */
Qairt_Status_t QairtQuantizeParams_MicroscalingEncoding_free(
    QairtQuantizeParams_MicroscalingEncodingHandle_t microscalingEncodingHandle);

/**
 * @brief Set the floating point value encoding for a Microscaling encoding.
 *
 * @param[in] microscalingEncodingHandle A handle to the Microscaling encoding.
 *
 * @param[in] valueEncoding The floating point encoding to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _microscalingEncodingHandle_
 *           is not a valid handle
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_MicroscalingEncodingV1_t.
 */
Qairt_Status_t QairtQuantizeParams_MicroscalingEncoding_setValueEncoding(
    QairtQuantizeParams_MicroscalingEncodingHandle_t microscalingEncodingHandle,
    Qairt_FloatEncoding_t valueEncoding);

/**
 * @brief Get the floating point value encoding from a Microscaling encoding.
 *
 * @param[in] microscalingEncodingHandle A handle to the Microscaling encoding.
 *
 * @param[out] valueEncoding The retrieved floating point encoding.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _microscalingEncodingHandle_
 *           is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _valueEncoding_ is NULL
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_MicroscalingEncodingV1_t.
 */
Qairt_Status_t QairtQuantizeParams_MicroscalingEncoding_getValueEncoding(
    QairtQuantizeParams_MicroscalingEncodingHandle_t microscalingEncodingHandle,
    Qairt_FloatEncoding_t* valueEncoding);

/**
 * @brief Set the block dimensions for a Microscaling encoding.
 *
 *        The number of elements in _blockDimensions_ must equal the rank of
 *        the associated tensor. Each element specifies the size of a block
 *        along the corresponding tensor dimension.
 *
 * @param[in] microscalingEncodingHandle A handle to the Microscaling encoding.
 *
 * @param[in] blockDimensions Array of block dimension values.
 *
 * @param[in] blockRank Number of elements in _blockDimensions_. Must equal
 *                      the rank of the associated tensor.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _microscalingEncodingHandle_
 *           is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _blockDimensions_ is NULL
 *           or _blockRank_ is 0
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory allocation error while
 *           setting block dimensions
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_MicroscalingEncodingV1_t.
 */
Qairt_Status_t QairtQuantizeParams_MicroscalingEncoding_setBlockDimensions(
    QairtQuantizeParams_MicroscalingEncodingHandle_t microscalingEncodingHandle,
    uint32_t* blockDimensions,
    uint32_t blockRank);

/**
 * @brief Get the number of block dimensions from a Microscaling encoding.
 *
 * @param[in] microscalingEncodingHandle A handle to the Microscaling encoding.
 *
 * @param[out] blockRank The retrieved number of block dimensions.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _microscalingEncodingHandle_
 *           is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _blockRank_ is NULL
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_MicroscalingEncodingV1_t.
 */
Qairt_Status_t QairtQuantizeParams_MicroscalingEncoding_getNumBlockDimensions(
    QairtQuantizeParams_MicroscalingEncodingHandle_t microscalingEncodingHandle,
    uint32_t* blockRank);

/**
 * @brief Get the block dimension value at a given index from a Microscaling encoding.
 *
 * @param[in] microscalingEncodingHandle A handle to the Microscaling encoding.
 *
 * @param[in] index The index of the block dimension to retrieve.
 *
 * @param[out] blockDimension The retrieved block dimension value at _index_.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _microscalingEncodingHandle_
 *           is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _blockDimension_ is NULL or
 *           _index_ is out of range
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_MicroscalingEncodingV1_t.
 */
Qairt_Status_t QairtQuantizeParams_MicroscalingEncoding_getBlockDimensionAt(
    QairtQuantizeParams_MicroscalingEncodingHandle_t microscalingEncodingHandle,
    uint32_t index,
    uint32_t* blockDimension);

/**
 * @brief Get the block count from a Microscaling encoding.
 *
 * @param[in] microscalingEncodingHandle A handle to the Microscaling encoding.
 *
 * @param[out] numBlockScales The retrieved number of block scales.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _microscalingEncodingHandle_
 *           is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _blockCount_ is NULL
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_MicroscalingEncodingV1_t.
 */
Qairt_Status_t QairtQuantizeParams_MicroscalingEncoding_getNumBlockScales(
    QairtQuantizeParams_MicroscalingEncodingHandle_t microscalingEncodingHandle,
    uint32_t* numBlockScales);

/**
 * @brief Set the scale data type for a Microscaling encoding.
 *
 * @param[in] microscalingEncodingHandle A handle to the Microscaling encoding.
 *
 * @param[in] scaleDataType The data type of the block scale factors to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _microscalingEncodingHandle_
 *           is not a valid handle
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_MicroscalingEncodingV1_t.
 */
Qairt_Status_t QairtQuantizeParams_MicroscalingEncoding_setScaleDataType(
    QairtQuantizeParams_MicroscalingEncodingHandle_t microscalingEncodingHandle,
    Qairt_DataType_t scaleDataType);

/**
 * @brief Get the scale data type from a Microscaling encoding.
 *
 * @param[in] microscalingEncodingHandle A handle to the Microscaling encoding.
 *
 * @param[out] scaleDataType The retrieved data type of the block scale factors.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _microscalingEncodingHandle_
 *           is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _scaleDataType_ is NULL
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_MicroscalingEncodingV1_t.
 */
Qairt_Status_t QairtQuantizeParams_MicroscalingEncoding_getScaleDataType(
    QairtQuantizeParams_MicroscalingEncodingHandle_t microscalingEncodingHandle,
    Qairt_DataType_t* scaleDataType);

/**
 * @brief Set the 8-bit block scale array for a Microscaling encoding.
 *
 *        Used when the scale data type is QAIRT_DATATYPE_FLOAT_8. The scale
 *        factor for block i is pow(2, blockScales8[i] - 127). The array must
 *        contain exactly blockCount elements.
 *
 * @param[in] microscalingEncodingHandle A handle to the Microscaling encoding.
 *
 * @param[in] blockScales8 Array of 8-bit block scale values.
 *
 * @param[in] numScales Number of scale values in the array. Must equal the
 *                      block count.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _microscalingEncodingHandle_
 *           is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _blockScales8_ is NULL or
 *           _numScales_ is 0
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory allocation error while
 *           setting block scales
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_MicroscalingEncodingV1_t.
 */
Qairt_Status_t QairtQuantizeParams_MicroscalingEncoding_setBlockScales8(
    QairtQuantizeParams_MicroscalingEncodingHandle_t microscalingEncodingHandle,
    uint8_t* blockScales8,
    uint32_t numScales);

/**
 * @brief Get the 8-bit block scale value at a given index from a Microscaling encoding.
 *
 *        Used when the scale data type is QAIRT_DATATYPE_FLOAT_8.
 *
 * @param[in] microscalingEncodingHandle A handle to the Microscaling encoding.
 *
 * @param[in] index The index of the block scale value to retrieve.
 *
 * @param[out] blockScale8 The retrieved 8-bit block scale value at _index_.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _microscalingEncodingHandle_
 *           is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _blockScale8_ is NULL or
 *           _index_ is out of range
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_MicroscalingEncodingV1_t.
 */
Qairt_Status_t QairtQuantizeParams_MicroscalingEncoding_getBlockScale8At(
    QairtQuantizeParams_MicroscalingEncodingHandle_t microscalingEncodingHandle,
    uint32_t index,
    uint8_t* blockScale8);

/**
 * @brief Set the float block scale array for a Microscaling encoding.
 *
 *        Used when the scale data type is QAIRT_DATATYPE_FLOAT_16 or
 *        QAIRT_DATATYPE_FLOAT_32. The array must contain exactly blockCount
 *        elements.
 *
 * @param[in] microscalingEncodingHandle A handle to the Microscaling encoding.
 *
 * @param[in] blockScalesFloat Array of float block scale values.
 *
 * @param[in] numScales Number of scale values in the array. Must equal the
 *                      block count.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _microscalingEncodingHandle_
 *           is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _blockScalesFloat_ is NULL
 *           or _numScales_ is 0
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory allocation error while
 *           setting block scales
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_MicroscalingEncodingV1_t.
 */
Qairt_Status_t QairtQuantizeParams_MicroscalingEncoding_setBlockScalesFloat(
    QairtQuantizeParams_MicroscalingEncodingHandle_t microscalingEncodingHandle,
    float* blockScalesFloat,
    uint32_t numScales);

/**
 * @brief Get the float block scale value at a given index from a Microscaling encoding.
 *
 *        Used when the scale data type is QAIRT_DATATYPE_FLOAT_16 or
 *        QAIRT_DATATYPE_FLOAT_32.
 *
 * @param[in] microscalingEncodingHandle A handle to the Microscaling encoding.
 *
 * @param[in] index The index of the block scale value to retrieve.
 *
 * @param[out] blockScaleFloat The retrieved float block scale value at _index_.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _microscalingEncodingHandle_
 *           is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _blockScaleFloat_ is NULL or
 *           _index_ is out of range
 *
 * @note Use corresponding API through
 *       QairtQuantizeParams_MicroscalingEncodingV1_t.
 */
Qairt_Status_t QairtQuantizeParams_MicroscalingEncoding_getBlockScaleFloatAt(
    QairtQuantizeParams_MicroscalingEncodingHandle_t microscalingEncodingHandle,
    uint32_t index,
    float* blockScaleFloat);
/**
 * @brief Create a float scale-offset handle for per-tensor float quantization.
 *
 * @param[out] floatScaleOffsetHandle A handle to the created float scale-offset.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _floatScaleOffsetHandle_ is NULL
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory allocation error while creating float scale-offset
 *
 * @note Use corresponding API through QairtQuantizeParams_FloatScaleOffsetV1_t.
 */
Qairt_Status_t QairtQuantizeParams_FloatScaleOffset_create(
    QairtQuantizeParams_FloatScaleOffsetHandle_t* floatScaleOffsetHandle);

/**
 * @brief Free a float scale-offset handle.
 *
 * @param[in] floatScaleOffsetHandle A handle to the float scale-offset to be freed.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _floatScaleOffsetHandle_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory deallocation error
 *
 * @note Use corresponding API through QairtQuantizeParams_FloatScaleOffsetV1_t.
 */
Qairt_Status_t QairtQuantizeParams_FloatScaleOffset_free(
    QairtQuantizeParams_FloatScaleOffsetHandle_t floatScaleOffsetHandle);

/**
 * @brief Set the scale value of a float scale-offset.
 *
 * @param[in] floatScaleOffsetHandle A handle to the float scale-offset.
 * @param[in] scale The scale value to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _floatScaleOffsetHandle_ is not a valid handle
 *
 * @note Use corresponding API through QairtQuantizeParams_FloatScaleOffsetV1_t.
 */
Qairt_Status_t QairtQuantizeParams_FloatScaleOffset_setScale(
    QairtQuantizeParams_FloatScaleOffsetHandle_t floatScaleOffsetHandle, float scale);

/**
 * @brief Get the scale value from a float scale-offset.
 *
 * @param[in] floatScaleOffsetHandle A handle to the float scale-offset.
 * @param[out] scale The retrieved scale value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _floatScaleOffsetHandle_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _scale_ is NULL
 *
 * @note Use corresponding API through QairtQuantizeParams_FloatScaleOffsetV1_t.
 */
Qairt_Status_t QairtQuantizeParams_FloatScaleOffset_getScale(
    QairtQuantizeParams_FloatScaleOffsetHandle_t floatScaleOffsetHandle, float* scale);

/**
 * @brief Set the offset value of a float scale-offset.
 *
 * @param[in] floatScaleOffsetHandle A handle to the float scale-offset.
 * @param[in] offset The offset value to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _floatScaleOffsetHandle_ is not a valid handle
 *
 * @note Use corresponding API through QairtQuantizeParams_FloatScaleOffsetV1_t.
 */
Qairt_Status_t QairtQuantizeParams_FloatScaleOffset_setOffset(
    QairtQuantizeParams_FloatScaleOffsetHandle_t floatScaleOffsetHandle, float offset);

/**
 * @brief Get the offset value from a float scale-offset.
 *
 * @param[in] floatScaleOffsetHandle A handle to the float scale-offset.
 * @param[out] offset The retrieved offset value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _floatScaleOffsetHandle_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _offset_ is NULL
 *
 * @note Use corresponding API through QairtQuantizeParams_FloatScaleOffsetV1_t.
 */
Qairt_Status_t QairtQuantizeParams_FloatScaleOffset_getOffset(
    QairtQuantizeParams_FloatScaleOffsetHandle_t floatScaleOffsetHandle, float* offset);

// BwFloatBlockEncoding Functions

/**
 * @brief Create a bit-width float scale-offset block encoding handle.
 *
 * @param[out] bwFloatBlockEncodingHandle A handle to the created bit-width float scale-offset block
 * encoding.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _bwFloatBlockEncodingHandle_ is
 *           NULL
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory allocation error while
 *           creating bwFloatBlock encoding
 *
 * @note Use corresponding API through QairtQuantizeParams_BwFloatBlockEncodingV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwFloatBlockEncoding_create(
    QairtQuantizeParams_BwFloatBlockEncodingHandle_t* bwFloatBlockEncodingHandle);

/**
 * @brief Free a bit-width float scale-offset block encoding handle.
 *
 * @param[in] bwFloatBlockEncodingHandle A handle to the bit-width float scale-offset block encoding
 * to be freed.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwFloatBlockEncodingHandle_
 *           is not a valid handle
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory deallocation error
 *
 * @note Use corresponding API through QairtQuantizeParams_BwFloatBlockEncodingV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwFloatBlockEncoding_free(
    QairtQuantizeParams_BwFloatBlockEncodingHandle_t bwFloatBlockEncodingHandle);

/**
 * @brief Set the bit-width for a bit-width float scale-offset block encoding.
 *
 * @param[in] bwFloatBlockEncodingHandle A handle to the bit-width axis
 *                                    scale-offset.
 * @param[in] bw The bit-width value to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwFloatBlockEncodingHandle_
 *           is not a valid handle
 *
 * @note Use corresponding API through QairtQuantizeParams_BwFloatBlockEncodingV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwFloatBlockEncoding_setBw(
    QairtQuantizeParams_BwFloatBlockEncodingHandle_t bwFloatBlockEncodingHandle, uint32_t bw);

/**
 * @brief Get the bit-width from a bit-width float scale-offset block encoding.
 *
 * @param[in] bwFloatBlockEncodingHandle A handle to the bit-width float
 *                                       scale-offset block encoding.
 * @param[out] bw The retrieved bit-width value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwFloatBlockEncodingHandle_
 *           is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _bw_ is NULL
 *
 * @note Use corresponding API through QairtQuantizeParams_BwFloatBlockEncodingV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwFloatBlockEncoding_getBw(
    QairtQuantizeParams_BwFloatBlockEncodingHandle_t bwFloatBlockEncodingHandle, uint32_t* bw);

/**
 * @brief Set the block size array for a bit-width float scale-offset block encoding.
 *
 * @param[in] bwFloatBlockEncodingHandle A handle to the bit-width float scale-offset block
 * encoding.
 *
 * @param[in] blockSizes Array of block size values.
 *
 * @param[in] numBlocks Number of block sizes in the array.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwFloatBlockEncodingHandle_ is
 *           not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _blockSizes_ is NULL or
 *           _numBlocks_ is 0
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory allocation error while
 *           setting block sizes
 *
 * @note Use corresponding API through QairtQuantizeParams_BwFloatBlockEncodingV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwFloatBlockEncoding_setBlockSize(
    QairtQuantizeParams_BwFloatBlockEncodingHandle_t bwFloatBlockEncodingHandle,
    uint32_t* blockSizes,
    uint32_t numBlocks);

/**
 * @brief Get the number of block sizes from a bit-width float scale-offset block encoding.
 *
 * @param[in] bwFloatBlockEncodingHandle A handle to the bit-width float scale-offset block
 * encoding.
 *
 * @param[out] numBlocks The retrieved number of block sizes.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwFloatBlockEncodingHandle_ is
 *           not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _numBlocks_ is NULL
 *
 * @note Use corresponding API through QairtQuantizeParams_BwFloatBlockEncodingV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwFloatBlockEncoding_getNumBlockSizes(
    QairtQuantizeParams_BwFloatBlockEncodingHandle_t bwFloatBlockEncodingHandle,
    uint32_t* numBlocks);

/**
 * @brief Get the block size value at a given index from a bit-width float scale-offset block
 * encoding.
 *
 * @param[in] bwFloatBlockEncodingHandle A handle to the bit-width float scale-offset block
 * encoding.
 *
 * @param[in] index The index of the block size to retrieve.
 *
 * @param[out] blockSize The retrieved block size value at _index_.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwFloatBlockEncodingHandle_ is
 *           not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _blockSize_ is NULL or
 *           _index_ is out of range
 *
 * @note Use corresponding API through QairtQuantizeParams_BwFloatBlockEncodingV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwFloatBlockEncoding_getBlockSizeAt(
    QairtQuantizeParams_BwFloatBlockEncodingHandle_t bwFloatBlockEncodingHandle,
    uint32_t index,
    uint32_t* blockSize);

/**
 * @brief Set the float scale-offset array for a bit-width float scale-offset block encoding.
 *
 * @param[in] bwFloatBlockEncodingHandle A handle to the bit-width float scale-offset block
 * encoding.
 *
 * @param[in] floatScaleOffsetsHandle Array of float scale-offset handles.
 *
 * @param[in] numFloatScaleOffsets Number of float scale-offset handles in the array.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwFloatBlockEncodingHandle_ or a
 *           handle in _floatScaleOffsetsHandle_ array is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _floatScaleOffsetsHandle_ is
 *           NULL or _numFloatScaleOffsets_ is 0
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory allocation error while
 *           setting float scale-offsets
 *
 * @note Use corresponding API through QairtQuantizeParams_BwFloatBlockEncodingV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwFloatBlockEncoding_setFloatScaleOffsets(
    QairtQuantizeParams_BwFloatBlockEncodingHandle_t bwFloatBlockEncodingHandle,
    QairtQuantizeParams_FloatScaleOffsetHandle_t* floatScaleOffsetsHandle,
    uint32_t numFloatScaleOffsets);

/**
 * @brief Get the number of float scale-offsets in a bit-width float scale-offset block encoding.
 *
 * @param[in] bwFloatBlockEncodingHandle A handle to the bit-width float scale-offset block
 * encoding.
 *
 * @param[out] numFloatScaleOffsets The retrieved number of float scale-offsets.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwFloatBlockEncodingHandle_ is
 *           not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _numFloatScaleOffsets_ is NULL
 *
 * @note Use corresponding API through QairtQuantizeParams_BwFloatBlockEncodingV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwFloatBlockEncoding_getNumFloatScaleOffsets(
    QairtQuantizeParams_BwFloatBlockEncodingHandle_t bwFloatBlockEncodingHandle,
    uint32_t* numFloatScaleOffsets);

/**
 * @brief Get the float scale-offset at index from a bit-width float scale-offset block encoding.
 *
 * @param[in] bwFloatBlockEncodingHandle A handle to the bit-width float scale-offset block
 * encoding.
 *
 * @param[in] index Index of float scale-offset that should be retrieved.
 *
 * @param[out] floatScaleOffsetHandle The retrieved float scale-offset handle.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwFloatBlockEncodingHandle_ is
 *           not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _floatScaleOffsetHandle_ is NULL
 *           or _index_ is out of bounds
 *
 * @note Use corresponding API through QairtQuantizeParams_BwFloatBlockEncodingV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwFloatBlockEncoding_getFloatScaleOffsetAt(
    QairtQuantizeParams_BwFloatBlockEncodingHandle_t bwFloatBlockEncodingHandle,
    uint32_t index,
    QairtQuantizeParams_FloatScaleOffsetHandle_t* floatScaleOffsetHandle);

// FloatBlockEncoding Functions

/**
 * @brief Create a float scale-offset block encoding handle.
 *
 * @param[out] floatBlockEncodingHandle A handle to the created float scale-offset block encoding.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _floatBlockEncodingHandle_ is NULL
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory allocation error while
 *           creating floatBlock encoding
 *
 * @note Use corresponding API through QairtQuantizeParams_FloatBlockEncodingV1_t.
 */
Qairt_Status_t QairtQuantizeParams_FloatBlockEncoding_create(
    QairtQuantizeParams_FloatBlockEncodingHandle_t* floatBlockEncodingHandle);

/**
 * @brief Free a float scale-offset block encoding handle.
 *
 * @param[in] floatBlockEncodingHandle A handle to the float scale-offset block encoding to be
 * freed.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _floatBlockEncodingHandle_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory deallocation error
 *
 * @note Use corresponding API through QairtQuantizeParams_FloatBlockEncodingV1_t.
 */
Qairt_Status_t QairtQuantizeParams_FloatBlockEncoding_free(
    QairtQuantizeParams_FloatBlockEncodingHandle_t floatBlockEncodingHandle);

/**
 * @brief Set the block size array for a float scale-offset block encoding.
 *
 * @param[in] floatBlockEncodingHandle A handle to the float scale-offset block encoding.
 * @param[in] blockSizes Array of block size values.
 * @param[in] numBlocks Number of block sizes in the array.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _floatBlockEncodingHandle_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _blockSizes_ is NULL or _numBlocks_ is 0
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory allocation error while setting block sizes
 *
 * @note Use corresponding API through QairtQuantizeParams_FloatBlockEncodingV1_t.
 */
Qairt_Status_t QairtQuantizeParams_FloatBlockEncoding_setBlockSize(
    QairtQuantizeParams_FloatBlockEncodingHandle_t floatBlockEncodingHandle,
    uint32_t* blockSizes,
    uint32_t numBlocks);

/**
 * @brief Get the number of block sizes from a float scale-offset block encoding.
 *
 * @param[in] floatBlockEncodingHandle A handle to the float scale-offset block encoding.
 * @param[out] numBlocks The retrieved number of block sizes.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _floatBlockEncodingHandle_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _numBlocks_ is NULL
 *
 * @note Use corresponding API through QairtQuantizeParams_FloatBlockEncodingV1_t.
 */
Qairt_Status_t QairtQuantizeParams_FloatBlockEncoding_getNumBlockSizes(
    QairtQuantizeParams_FloatBlockEncodingHandle_t floatBlockEncodingHandle, uint32_t* numBlocks);

/**
 * @brief Get the block size value at a given index from a float scale-offset block encoding.
 *
 * @param[in] floatBlockEncodingHandle A handle to the float scale-offset block encoding.
 * @param[in] index The index of the block size to retrieve.
 * @param[out] blockSize The retrieved block size value at _index_.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _floatBlockEncodingHandle_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _blockSize_ is NULL or _index_ is out of range
 *
 * @note Use corresponding API through QairtQuantizeParams_FloatBlockEncodingV1_t.
 */
Qairt_Status_t QairtQuantizeParams_FloatBlockEncoding_getBlockSizeAt(
    QairtQuantizeParams_FloatBlockEncodingHandle_t floatBlockEncodingHandle,
    uint32_t index,
    uint32_t* blockSize);

/**
 * @brief Set the float scale-offset array for a float scale-offset block encoding.
 *
 * @param[in] floatBlockEncodingHandle A handle to the float scale-offset block encoding.
 * @param[in] floatScaleOffsetsHandle Array of float scale-offset handles.
 * @param[in] numFloatScaleOffsets Number of float scale-offset handles in the array.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _floatBlockEncodingHandle_ or a handle in
 *           _floatScaleOffsetsHandle_ array is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _floatScaleOffsetsHandle_ is NULL or
 *           _numFloatScaleOffsets_ is 0
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory allocation error while setting float scale-offsets
 *
 * @note Use corresponding API through QairtQuantizeParams_FloatBlockEncodingV1_t.
 */
Qairt_Status_t QairtQuantizeParams_FloatBlockEncoding_setFloatScaleOffsets(
    QairtQuantizeParams_FloatBlockEncodingHandle_t floatBlockEncodingHandle,
    QairtQuantizeParams_FloatScaleOffsetHandle_t* floatScaleOffsetsHandle,
    uint32_t numFloatScaleOffsets);

/**
 * @brief Get the number of float scale-offsets in a float scale-offset block encoding.
 *
 * @param[in] floatBlockEncodingHandle A handle to the float scale-offset block encoding.
 * @param[out] numFloatScaleOffsets The retrieved number of float scale-offsets.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _floatBlockEncodingHandle_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _numFloatScaleOffsets_ is NULL
 *
 * @note Use corresponding API through QairtQuantizeParams_FloatBlockEncodingV1_t.
 */
Qairt_Status_t QairtQuantizeParams_FloatBlockEncoding_getNumFloatScaleOffsets(
    QairtQuantizeParams_FloatBlockEncodingHandle_t floatBlockEncodingHandle,
    uint32_t* numFloatScaleOffsets);

/**
 * @brief Get the float scale-offset at index from a float scale-offset block encoding.
 *
 * @param[in] floatBlockEncodingHandle A handle to the float scale-offset block encoding.
 * @param[in] index Index of float scale-offset that should be retrieved.
 * @param[out] floatScaleOffsetHandle The retrieved float scale-offset handle.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _floatBlockEncodingHandle_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _floatScaleOffsetHandle_ is NULL or
 *           _index_ is out of bounds
 *
 * @note Use corresponding API through QairtQuantizeParams_FloatBlockEncodingV1_t.
 */
Qairt_Status_t QairtQuantizeParams_FloatBlockEncoding_getFloatScaleOffsetAt(
    QairtQuantizeParams_FloatBlockEncodingHandle_t floatBlockEncodingHandle,
    uint32_t index,
    QairtQuantizeParams_FloatScaleOffsetHandle_t* floatScaleOffsetHandle);

// BwBlockMapped Functions

/**
 * @brief Create a bit-width block mapped encoding handle.
 *
 * @param[out] bwBlockMappedHandle A handle to the created bit-width block mapped encoding.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _bwBlockMappedHandle_ is NULL
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory allocation error while creating bwBlockMapped
 * encoding
 *
 * @note Use corresponding API through QairtQuantizeParams_BwBlockMappedV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwBlockMapped_create(
    QairtQuantizeParams_BwBlockMappedHandle_t* bwBlockMappedHandle);

/**
 * @brief Free a bit-width block mapped encoding handle.
 *
 * @param[in] bwBlockMappedHandle A handle to the bit-width block mapped encoding to be freed.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwBlockMappedHandle_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory deallocation error
 *
 * @note Use corresponding API through QairtQuantizeParams_BwBlockMappedV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwBlockMapped_free(
    QairtQuantizeParams_BwBlockMappedHandle_t bwBlockMappedHandle);

/**
 * @brief Set the bit-width for a bit-width block mapped encoding.
 *
 * @param[in] bwBlockMappedHandle A handle to the bit-width block mapped encoding.
 * @param[in] bw The bit-width value to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwBlockMappedHandle_ is not a valid handle
 *
 * @note Use corresponding API through QairtQuantizeParams_BwBlockMappedV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwBlockMapped_setBw(
    QairtQuantizeParams_BwBlockMappedHandle_t bwBlockMappedHandle, uint32_t bw);

/**
 * @brief Get the bit-width from a bit-width block mapped encoding.
 *
 * @param[in] bwBlockMappedHandle A handle to the bit-width block mapped encoding.
 * @param[out] bw The retrieved bit-width value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwBlockMappedHandle_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _bw_ is NULL
 *
 * @note Use corresponding API through QairtQuantizeParams_BwBlockMappedV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwBlockMapped_getBw(
    QairtQuantizeParams_BwBlockMappedHandle_t bwBlockMappedHandle, uint32_t* bw);

/**
 * @brief Set the mapping for a bit-width block mapped encoding.
 *
 * @param[in] bwBlockMappedHandle A handle to the bit-width block mapped encoding.
 * @param[in] mapping The mapping value to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwBlockMappedHandle_ is not a valid handle
 *
 * @note Use corresponding API through QairtQuantizeParams_BwBlockMappedV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwBlockMapped_setMapping(
    QairtQuantizeParams_BwBlockMappedHandle_t bwBlockMappedHandle,
    Qairt_QuantizationEncodingMapping_t mapping);

/**
 * @brief Get the mapping from a bit-width block mapped encoding.
 *
 * @param[in] bwBlockMappedHandle A handle to the bit-width block mapped encoding.
 * @param[out] mapping The retrieved mapping value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwBlockMappedHandle_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _mapping_ is NULL
 *
 * @note Use corresponding API through QairtQuantizeParams_BwBlockMappedV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwBlockMapped_getMapping(
    QairtQuantizeParams_BwBlockMappedHandle_t bwBlockMappedHandle,
    Qairt_QuantizationEncodingMapping_t* mapping);

/**
 * @brief Set the block size array for a bit-width block mapped encoding.
 *
 * @param[in] bwBlockMappedHandle A handle to the bit-width block mapped encoding.
 * @param[in] blockSize Array of block size values.
 * @param[in] numBlockSizes Number of block sizes in the array.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwBlockMappedHandle_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _blockSize_ is NULL or _numBlockSizes_ is 0
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory allocation error while setting block sizes
 *
 * @note Use corresponding API through QairtQuantizeParams_BwBlockMappedV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwBlockMapped_setBlockSize(
    QairtQuantizeParams_BwBlockMappedHandle_t bwBlockMappedHandle,
    uint32_t* blockSize,
    uint32_t numBlockSizes);

/**
 * @brief Get the number of block sizes from a bit-width block mapped encoding.
 *
 * @param[in] bwBlockMappedHandle A handle to the bit-width block mapped encoding.
 * @param[out] numBlockSizes The retrieved number of block sizes.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwBlockMappedHandle_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _numBlockSizes_ is NULL
 *
 * @note Use corresponding API through QairtQuantizeParams_BwBlockMappedV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwBlockMapped_getNumBlockSizes(
    QairtQuantizeParams_BwBlockMappedHandle_t bwBlockMappedHandle, uint32_t* numBlockSizes);

/**
 * @brief Get the block size value at a given index from a bit-width block mapped encoding.
 *
 * @param[in] bwBlockMappedHandle A handle to the bit-width block mapped encoding.
 * @param[in] index The index of the block size to retrieve.
 * @param[out] blockSize The retrieved block size value at _index_.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwBlockMappedHandle_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _blockSize_ is NULL or _index_ is out of range
 *
 * @note Use corresponding API through QairtQuantizeParams_BwBlockMappedV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwBlockMapped_getBlockSizeAt(
    QairtQuantizeParams_BwBlockMappedHandle_t bwBlockMappedHandle,
    uint32_t index,
    uint32_t* blockSize);

/**
 * @brief Set the scale-offset array for a bit-width block mapped encoding.
 *
 * @param[in] bwBlockMappedHandle A handle to the bit-width block mapped encoding.
 * @param[in] scaleOffsetsHandle Array of scale-offset handles.
 * @param[in] numScaleOffsets Number of scale-offset handles in the array.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwBlockMappedHandle_ or a handle in
 *           _scaleOffsetsHandle_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _scaleOffsetsHandle_ is NULL or
 *           _numScaleOffsets_ is 0
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory allocation error while setting scale-offsets
 *
 * @note Use corresponding API through QairtQuantizeParams_BwBlockMappedV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwBlockMapped_setScaleOffsets(
    QairtQuantizeParams_BwBlockMappedHandle_t bwBlockMappedHandle,
    QairtQuantizeParams_ScaleOffsetHandle_t* scaleOffsetsHandle,
    uint32_t numScaleOffsets);

/**
 * @brief Get the number of scale-offsets in a bit-width block mapped encoding.
 *
 * @param[in] bwBlockMappedHandle A handle to the bit-width block mapped encoding.
 * @param[out] numScaleOffsets The retrieved number of scale-offsets.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwBlockMappedHandle_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _numScaleOffsets_ is NULL
 *
 * @note Use corresponding API through QairtQuantizeParams_BwBlockMappedV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwBlockMapped_getNumScaleOffsets(
    QairtQuantizeParams_BwBlockMappedHandle_t bwBlockMappedHandle, uint32_t* numScaleOffsets);

/**
 * @brief Get the scale-offset at index from a bit-width block mapped encoding.
 *
 * @param[in] bwBlockMappedHandle A handle to the bit-width block mapped encoding.
 * @param[in] index Index of scale-offset that should be retrieved.
 * @param[out] scaleOffsetHandle The retrieved scale-offset handle.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwBlockMappedHandle_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _scaleOffsetHandle_ is NULL or _index_ is
 *           out of bounds
 *
 * @note Use corresponding API through QairtQuantizeParams_BwBlockMappedV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwBlockMapped_getScaleOffsetAt(
    QairtQuantizeParams_BwBlockMappedHandle_t bwBlockMappedHandle,
    uint32_t index,
    QairtQuantizeParams_ScaleOffsetHandle_t* scaleOffsetHandle);

// BwBlockwiseExpansionMapped Functions

/**
 * @brief Create a bit-width blockwise expansion mapped encoding handle.
 *
 * @param[out] bwBlockwiseExpansionMappedHandle A handle to the created encoding.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _bwBlockwiseExpansionMappedHandle_ is NULL
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory allocation error
 *
 * @note Use corresponding API through QairtQuantizeParams_BwBlockwiseExpansionMappedV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwBlockwiseExpansionMapped_create(
    QairtQuantizeParams_BwBlockwiseExpansionMappedHandle_t* bwBlockwiseExpansionMappedHandle);

/**
 * @brief Free a bit-width blockwise expansion mapped encoding handle.
 *
 * @param[in] bwBlockwiseExpansionMappedHandle A handle to the encoding to be freed.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwBlockwiseExpansionMappedHandle_ is not a valid
 * handle
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory deallocation error
 *
 * @note Use corresponding API through QairtQuantizeParams_BwBlockwiseExpansionMappedV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwBlockwiseExpansionMapped_free(
    QairtQuantizeParams_BwBlockwiseExpansionMappedHandle_t bwBlockwiseExpansionMappedHandle);

/**
 * @brief Set the bit-width for a bit-width blockwise expansion mapped encoding.
 *
 * @param[in] bwBlockwiseExpansionMappedHandle A handle to the encoding.
 * @param[in] bw The bit-width value to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwBlockwiseExpansionMappedHandle_ is not a valid
 * handle
 *
 * @note Use corresponding API through QairtQuantizeParams_BwBlockwiseExpansionMappedV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwBlockwiseExpansionMapped_setBw(
    QairtQuantizeParams_BwBlockwiseExpansionMappedHandle_t bwBlockwiseExpansionMappedHandle,
    uint32_t bw);

/**
 * @brief Get the bit-width from a bit-width blockwise expansion mapped encoding.
 *
 * @param[in] bwBlockwiseExpansionMappedHandle A handle to the encoding.
 * @param[out] bw The retrieved bit-width value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwBlockwiseExpansionMappedHandle_ is not a valid
 * handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _bw_ is NULL
 *
 * @note Use corresponding API through QairtQuantizeParams_BwBlockwiseExpansionMappedV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwBlockwiseExpansionMapped_getBw(
    QairtQuantizeParams_BwBlockwiseExpansionMappedHandle_t bwBlockwiseExpansionMappedHandle,
    uint32_t* bw);

/**
 * @brief Set the mapping for a bit-width blockwise expansion mapped encoding.
 *
 * @param[in] bwBlockwiseExpansionMappedHandle A handle to the encoding.
 * @param[in] mapping The mapping value to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwBlockwiseExpansionMappedHandle_ is not a valid
 * handle
 *
 * @note Use corresponding API through QairtQuantizeParams_BwBlockwiseExpansionMappedV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwBlockwiseExpansionMapped_setMapping(
    QairtQuantizeParams_BwBlockwiseExpansionMappedHandle_t bwBlockwiseExpansionMappedHandle,
    Qairt_QuantizationEncodingMapping_t mapping);

/**
 * @brief Get the mapping from a bit-width blockwise expansion mapped encoding.
 *
 * @param[in] bwBlockwiseExpansionMappedHandle A handle to the encoding.
 * @param[out] mapping The retrieved mapping value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwBlockwiseExpansionMappedHandle_ is not a valid
 * handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _mapping_ is NULL
 *
 * @note Use corresponding API through QairtQuantizeParams_BwBlockwiseExpansionMappedV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwBlockwiseExpansionMapped_getMapping(
    QairtQuantizeParams_BwBlockwiseExpansionMappedHandle_t bwBlockwiseExpansionMappedHandle,
    Qairt_QuantizationEncodingMapping_t* mapping);

/**
 * @brief Set the axis for a bit-width blockwise expansion mapped encoding.
 *
 * @param[in] bwBlockwiseExpansionMappedHandle A handle to the encoding.
 * @param[in] axis The axis value to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwBlockwiseExpansionMappedHandle_ is not a valid
 * handle
 *
 * @note Use corresponding API through QairtQuantizeParams_BwBlockwiseExpansionMappedV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwBlockwiseExpansionMapped_setAxis(
    QairtQuantizeParams_BwBlockwiseExpansionMappedHandle_t bwBlockwiseExpansionMappedHandle,
    int32_t axis);

/**
 * @brief Get the axis from a bit-width blockwise expansion mapped encoding.
 *
 * @param[in] bwBlockwiseExpansionMappedHandle A handle to the encoding.
 * @param[out] axis The retrieved axis value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwBlockwiseExpansionMappedHandle_ is not a valid
 * handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _axis_ is NULL
 *
 * @note Use corresponding API through QairtQuantizeParams_BwBlockwiseExpansionMappedV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwBlockwiseExpansionMapped_getAxis(
    QairtQuantizeParams_BwBlockwiseExpansionMappedHandle_t bwBlockwiseExpansionMappedHandle,
    int32_t* axis);

/**
 * @brief Set the scale-offset array for a bit-width blockwise expansion mapped encoding.
 *
 * @param[in] bwBlockwiseExpansionMappedHandle A handle to the encoding.
 * @param[in] scaleOffsetsHandle Array of scale-offset handles.
 * @param[in] numScaleOffsets Number of scale-offset handles in the array.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwBlockwiseExpansionMappedHandle_ or a handle in
 *           _scaleOffsetsHandle_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _scaleOffsetsHandle_ is NULL or
 *           _numScaleOffsets_ is 0
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory allocation error while setting scale-offsets
 *
 * @note Use corresponding API through QairtQuantizeParams_BwBlockwiseExpansionMappedV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwBlockwiseExpansionMapped_setScaleOffsets(
    QairtQuantizeParams_BwBlockwiseExpansionMappedHandle_t bwBlockwiseExpansionMappedHandle,
    QairtQuantizeParams_ScaleOffsetHandle_t* scaleOffsetsHandle,
    uint32_t numScaleOffsets);

/**
 * @brief Get the number of scale-offsets in a bit-width blockwise expansion mapped encoding.
 *
 * @param[in] bwBlockwiseExpansionMappedHandle A handle to the encoding.
 * @param[out] numScaleOffsets The retrieved number of scale-offsets.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwBlockwiseExpansionMappedHandle_ is not a valid
 * handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _numScaleOffsets_ is NULL
 *
 * @note Use corresponding API through QairtQuantizeParams_BwBlockwiseExpansionMappedV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwBlockwiseExpansionMapped_getNumScaleOffsets(
    QairtQuantizeParams_BwBlockwiseExpansionMappedHandle_t bwBlockwiseExpansionMappedHandle,
    uint32_t* numScaleOffsets);

/**
 * @brief Get the scale-offset at index from a bit-width blockwise expansion mapped encoding.
 *
 * @param[in] bwBlockwiseExpansionMappedHandle A handle to the encoding.
 * @param[in] index Index of scale-offset that should be retrieved.
 * @param[out] scaleOffsetHandle The retrieved scale-offset handle.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwBlockwiseExpansionMappedHandle_ is not a valid
 * handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _scaleOffsetHandle_ is NULL or _index_ is
 *           out of bounds
 *
 * @note Use corresponding API through QairtQuantizeParams_BwBlockwiseExpansionMappedV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwBlockwiseExpansionMapped_getScaleOffsetAt(
    QairtQuantizeParams_BwBlockwiseExpansionMappedHandle_t bwBlockwiseExpansionMappedHandle,
    uint32_t index,
    QairtQuantizeParams_ScaleOffsetHandle_t* scaleOffsetHandle);

/**
 * @brief Set the number of blocks per axis for a bit-width blockwise expansion mapped encoding.
 *
 * @param[in] bwBlockwiseExpansionMappedHandle A handle to the encoding.
 * @param[in] numBlocksPerAxis The number of blocks per axis to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwBlockwiseExpansionMappedHandle_ is not a valid
 * handle
 *
 * @note Use corresponding API through QairtQuantizeParams_BwBlockwiseExpansionMappedV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwBlockwiseExpansionMapped_setNumBlocksPerAxis(
    QairtQuantizeParams_BwBlockwiseExpansionMappedHandle_t bwBlockwiseExpansionMappedHandle,
    uint32_t numBlocksPerAxis);

/**
 * @brief Get the number of blocks per axis from a bit-width blockwise expansion mapped encoding.
 *
 * @param[in] bwBlockwiseExpansionMappedHandle A handle to the encoding.
 * @param[out] numBlocksPerAxis The retrieved number of blocks per axis.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwBlockwiseExpansionMappedHandle_ is not a valid
 * handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _numBlocksPerAxis_ is NULL
 *
 * @note Use corresponding API through QairtQuantizeParams_BwBlockwiseExpansionMappedV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwBlockwiseExpansionMapped_getNumBlocksPerAxis(
    QairtQuantizeParams_BwBlockwiseExpansionMappedHandle_t bwBlockwiseExpansionMappedHandle,
    uint32_t* numBlocksPerAxis);

/**
 * @brief Set the block scale bitwidth for a bit-width blockwise expansion mapped encoding.
 *
 * @param[in] bwBlockwiseExpansionMappedHandle A handle to the encoding.
 * @param[in] blockScaleBitwidth The block scale bitwidth to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwBlockwiseExpansionMappedHandle_ is not a valid
 * handle
 *
 * @note Use corresponding API through QairtQuantizeParams_BwBlockwiseExpansionMappedV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwBlockwiseExpansionMapped_setBlockScaleBitwidth(
    QairtQuantizeParams_BwBlockwiseExpansionMappedHandle_t bwBlockwiseExpansionMappedHandle,
    uint32_t blockScaleBitwidth);

/**
 * @brief Get the block scale bitwidth from a bit-width blockwise expansion mapped encoding.
 *
 * @param[in] bwBlockwiseExpansionMappedHandle A handle to the encoding.
 * @param[out] blockScaleBitwidth The retrieved block scale bitwidth.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwBlockwiseExpansionMappedHandle_ is not a valid
 * handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _blockScaleBitwidth_ is NULL
 *
 * @note Use corresponding API through QairtQuantizeParams_BwBlockwiseExpansionMappedV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwBlockwiseExpansionMapped_getBlockScaleBitwidth(
    QairtQuantizeParams_BwBlockwiseExpansionMappedHandle_t bwBlockwiseExpansionMappedHandle,
    uint32_t* blockScaleBitwidth);

/**
 * @brief Set the 8-bit block scales for a bit-width blockwise expansion mapped encoding.
 *
 * @param[in] bwBlockwiseExpansionMappedHandle A handle to the encoding.
 * @param[in] blocksScale8 Array of 8-bit block scale values.
 * @param[in] numBlocksScale8 Number of 8-bit block scale values in the array.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwBlockwiseExpansionMappedHandle_ is not a valid
 * handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _blocksScale8_ is NULL or _numBlocksScale8_ is 0
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory allocation error
 *
 * @note Use corresponding API through QairtQuantizeParams_BwBlockwiseExpansionMappedV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwBlockwiseExpansionMapped_setBlocksScale8(
    QairtQuantizeParams_BwBlockwiseExpansionMappedHandle_t bwBlockwiseExpansionMappedHandle,
    uint8_t* blocksScale8,
    uint32_t numBlocksScale8);

/**
 * @brief Get the number of 8-bit block scales from a bit-width blockwise expansion mapped encoding.
 *
 * @param[in] bwBlockwiseExpansionMappedHandle A handle to the encoding.
 * @param[out] numBlocksScale8 The retrieved number of 8-bit block scales.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwBlockwiseExpansionMappedHandle_ is not a valid
 * handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _numBlocksScale8_ is NULL
 *
 * @note Use corresponding API through QairtQuantizeParams_BwBlockwiseExpansionMappedV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwBlockwiseExpansionMapped_getNumBlocksScale8(
    QairtQuantizeParams_BwBlockwiseExpansionMappedHandle_t bwBlockwiseExpansionMappedHandle,
    uint32_t* numBlocksScale8);

/**
 * @brief Get the 8-bit block scale at index from a bit-width blockwise expansion mapped encoding.
 *
 * @param[in] bwBlockwiseExpansionMappedHandle A handle to the encoding.
 * @param[in] index Index of the block scale to retrieve.
 * @param[out] blocksScale8 The retrieved 8-bit block scale value at _index_.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwBlockwiseExpansionMappedHandle_ is not a valid
 * handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _blocksScale8_ is NULL or _index_ is out of bounds
 *
 * @note Use corresponding API through QairtQuantizeParams_BwBlockwiseExpansionMappedV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwBlockwiseExpansionMapped_getBlocksScale8At(
    QairtQuantizeParams_BwBlockwiseExpansionMappedHandle_t bwBlockwiseExpansionMappedHandle,
    uint32_t index,
    uint8_t* blocksScale8);

/**
 * @brief Set the 16-bit block scales for a bit-width blockwise expansion mapped encoding.
 *
 * @param[in] bwBlockwiseExpansionMappedHandle A handle to the encoding.
 * @param[in] blocksScale16 Array of 16-bit block scale values.
 * @param[in] numBlocksScale16 Number of 16-bit block scale values in the array.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwBlockwiseExpansionMappedHandle_ is not a valid
 * handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _blocksScale16_ is NULL or _numBlocksScale16_ is 0
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory allocation error
 *
 * @note Use corresponding API through QairtQuantizeParams_BwBlockwiseExpansionMappedV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwBlockwiseExpansionMapped_setBlocksScale16(
    QairtQuantizeParams_BwBlockwiseExpansionMappedHandle_t bwBlockwiseExpansionMappedHandle,
    uint16_t* blocksScale16,
    uint32_t numBlocksScale16);

/**
 * @brief Get the number of 16-bit block scales from a bit-width blockwise expansion mapped
 * encoding.
 *
 * @param[in] bwBlockwiseExpansionMappedHandle A handle to the encoding.
 * @param[out] numBlocksScale16 The retrieved number of 16-bit block scales.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwBlockwiseExpansionMappedHandle_ is not a valid
 * handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _numBlocksScale16_ is NULL
 *
 * @note Use corresponding API through QairtQuantizeParams_BwBlockwiseExpansionMappedV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwBlockwiseExpansionMapped_getNumBlocksScale16(
    QairtQuantizeParams_BwBlockwiseExpansionMappedHandle_t bwBlockwiseExpansionMappedHandle,
    uint32_t* numBlocksScale16);

/**
 * @brief Get the 16-bit block scale at index from a bit-width blockwise expansion mapped encoding.
 *
 * @param[in] bwBlockwiseExpansionMappedHandle A handle to the encoding.
 * @param[in] index Index of the block scale to retrieve.
 * @param[out] blocksScale16 The retrieved 16-bit block scale value at _index_.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _bwBlockwiseExpansionMappedHandle_ is not a valid
 * handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _blocksScale16_ is NULL or _index_ is out of
 * bounds
 *
 * @note Use corresponding API through QairtQuantizeParams_BwBlockwiseExpansionMappedV1_t.
 */
Qairt_Status_t QairtQuantizeParams_BwBlockwiseExpansionMapped_getBlocksScale16At(
    QairtQuantizeParams_BwBlockwiseExpansionMappedHandle_t bwBlockwiseExpansionMappedHandle,
    uint32_t index,
    uint16_t* blocksScale16);

// QuantizeParams functions
/**
 * @brief Create a quantization parameter handle.
 *
 * @param[out] quantizeParamsHandle A handle to the created quantization
 *                                   parameters.
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT:
 *           _quantizeParamsHandle_ is NULL
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory allocation error while
 *           creating quantization parameter
 *
 * @note Use corresponding API through QairtQuantizeParams_Handle_t.
 */
Qairt_Status_t QairtQuantizeParams_create(QairtQuantizeParams_Handle_t* quantizeParamsHandle);

/**
 * @brief Free a quantization parameter handle.
 *
 * @param[in] quantizeParamsHandle A handle to the quantization parameter to be
 *                                     freed.
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _quantizeParamsHandle_
 *           is not a valid handle
 *         - QAIRT_COMMON_ERROR_MEM_ALLOC: Memory deallocation error
 *
 * @note Use corresponding API through QairtQuantizeParams_Handle_t.
 */
Qairt_Status_t QairtQuantizeParams_free(QairtQuantizeParams_Handle_t quantizeParamsHandle);

/**
 * @brief Set scale-offset on the quantization parameter
 *
 * @param[in] quantParamsHandle A handle to the quantization parameter
 *
 * @param[in] scaleOffsetHandle A handle to the scale-offset.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _quantParamsHandle_ or
 *           _scaleOffsetHandle_ is not a valid handle
 *
 * @note Use corresponding API through QairtQuantizeParams_Handle_t.
 */
Qairt_Status_t QairtQuantizeParams_setScaleOffset(
    QairtQuantizeParams_Handle_t quantParamsHandle,
    QairtQuantizeParams_ScaleOffsetHandle_t scaleOffsetHandle);

/**
 * @brief Get scale-offset on the quantization parameter
 *
 * @param[in] quantParamsHandle A handle to the quantization parameter
 *
 * @param[out] scaleOffsetHandle A handle to the retrieved scale-offset.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _quantParamsHandle_ is
 *           not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _scaleOffsetHandle_ is NULL
 *
 * @note Use corresponding API through QairtQuantizeParams_Handle_t.
 */
Qairt_Status_t QairtQuantizeParams_getScaleOffset(
    QairtQuantizeParams_Handle_t quantParamsHandle,
    QairtQuantizeParams_ScaleOffsetHandle_t* scaleOffsetHandle);

/**
 * @brief Set axis-scale-offset on the quantization parameter
 *
 * @param[in] quantParamsHandle A handle to the quantization parameter
 *
 * @param[out] axisScaleOffsetHandle A handle to the retrieved
 *                                   axis-scale-offset.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _quantParamsHandle_ or
 *           _axisScaleOffsetHandle_ is not a valid handle
 *
 * @note Use corresponding API through QairtQuantizeParams_Handle_t.
 */
Qairt_Status_t QairtQuantizeParams_setAxisScaleOffset(
    QairtQuantizeParams_Handle_t quantParamsHandle,
    QairtQuantizeParams_AxisScaleOffsetHandle_t axisScaleOffsetHandle);

/**
 * @brief Get axis-scale-offset on the quantization parameter
 *
 * @param[in] quantParamsHandle A handle to the quantization parameter
 *
 * @param[out] scaleOffsetHandle A handle to the retrieved axis-scale-offset.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _quantParamsHandle_ is
 *           not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _scaleOffsetHandle_ is NULL
 *
 * @note Use corresponding API through QairtQuantizeParams_Handle_t.
 */
Qairt_Status_t QairtQuantizeParams_getAxisScaleOffset(
    QairtQuantizeParams_Handle_t quantParamsHandle,
    QairtQuantizeParams_AxisScaleOffsetHandle_t* axisScaleOffsetHandle);

/**
 * @brief Set bitwidth-scale-offset on the quantization parameter
 *
 * @param[in] quantParamsHandle A handle to the quantization parameter
 *
 * @param[out] bwScaleOffsetHandle A handle to the retrieved
 *                                 bitwidth-scale-offset.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _quantParamsHandle_ or
 *           _bwScaleOffsetHandle_ is not a valid handle
 *
 * @note Use corresponding API through QairtQuantizeParams_Handle_t.
 */
Qairt_Status_t QairtQuantizeParams_setBwScaleOffset(
    QairtQuantizeParams_Handle_t quantParamsHandle,
    QairtQuantizeParams_BwScaleOffsetHandle_t bwScaleOffsetHandle);

/**
 * @brief Get bitwidth-scale-offset on the quantization parameter
 *
 * @param[in] quantParamsHandle A handle to the quantization parameter
 *
 * @param[out] bwScaleOffsetHandle A handle to the retrieved
 *                                 bitwidth-scale-offset.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _quantParamsHandle_ is
 *           not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _bwScaleOffsetHandle_ is NULL
 *
 * @note Use corresponding API through QairtQuantizeParams_Handle_t.
 */
Qairt_Status_t QairtQuantizeParams_getBwScaleOffset(
    QairtQuantizeParams_Handle_t quantParamsHandle,
    QairtQuantizeParams_BwScaleOffsetHandle_t* bwScaleOffsetHandle);

/**
 * @brief Set bitwidth-axis-scale-offset on the quantization parameter
 *
 * @param[in] quantParamsHandle A handle to the quantization parameter
 *
 * @param[out] bwAxisScaleOffsetHandle A handle to the retrieved
 *                                     bitwidth-axis-scale-offset.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _quantParamsHandle_ or
 *           _bwAxisScaleOffsetHandle_ is not a valid handle
 *
 * @note Use corresponding API through QairtQuantizeParams_Handle_t.
 */
Qairt_Status_t QairtQuantizeParams_setBwAxisScaleOffset(
    QairtQuantizeParams_Handle_t quantParamsHandle,
    QairtQuantizeParams_BwAxisScaleOffsetHandle_t bwAxisScaleOffsetHandle);

/**
 * @brief Get bitwidth-axis-scale-offset on the quantization parameter
 *
 * @param[in] quantParamsHandle A handle to the quantization parameter
 *
 * @param[out] bwAxisScaleOffsetHandle A handle to the retrieved
 *                                     bitwidth-axis-scale-offset.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _quantParamsHandle_ is
 *           not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _bwAxisScaleOffsetHandle_ is
 *           NULL
 *
 * @note Use corresponding API through QairtQuantizeParams_Handle_t.
 */
Qairt_Status_t QairtQuantizeParams_getBwAxisScaleOffset(
    QairtQuantizeParams_Handle_t quantParamsHandle,
    QairtQuantizeParams_BwAxisScaleOffsetHandle_t* bwAxisScaleOffsetHandle);

/**
 * @brief Set bitwidth-axis-scale-offset-mapped on the quantization parameter
 *
 * @param[in] quantParamsHandle A handle to the quantization parameter
 *
 * @param[out] bwAxisScaleOffsetMappedHandle A handle to the retrieved
 *                                           bitwidth-axis-scale-offset-mapped.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _quantParamsHandle_ or
 *           _bwAxisScaleOffsetMappedHandle_ is not a valid handle
 *
 * @note Use corresponding API through QairtQuantizeParams_Handle_t.
 */
Qairt_Status_t QairtQuantizeParams_setBwAxisScaleOffsetMapped(
    QairtQuantizeParams_Handle_t quantParamsHandle,
    QairtQuantizeParams_BwAxisScaleOffsetMappedHandle_t bwAxisScaleOffsetMappedHandle);

/**
 * @brief Get bitwidth-axis-scale-offset-mapped on the quantization parameter
 *
 * @param[in] quantParamsHandle A handle to the quantization parameter
 *
 * @param[out] bwAxisScaleOffsetMappedHandle A handle to the retrieved
 *                                           bitwidth-axis-scale-offset-mapped.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _quantParamsHandle_ is
 *           not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _bwAxisScaleOffsetMappedHandle_ is
 *           NULL
 *
 * @note Use corresponding API through QairtQuantizeParams_Handle_t.
 */
Qairt_Status_t QairtQuantizeParams_getBwAxisScaleOffsetMapped(
    QairtQuantizeParams_Handle_t quantParamsHandle,
    QairtQuantizeParams_BwAxisScaleOffsetMappedHandle_t* bwAxisScaleOffsetMappedHandle);

/**
 * @brief Set block encoding on the quantization parameter
 *
 * @param[in] quantParamsHandle A handle to the quantization parameter
 *
 * @param[out] blockEncodingHandle A handle to the retrieved block encoding
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _quantParamsHandle_ or
 *           _blockEncodingHandle_ is not a valid handle
 *
 * @note Use corresponding API through QairtQuantizeParams_Handle_t.
 */
Qairt_Status_t QairtQuantizeParams_setBlockEncoding(
    QairtQuantizeParams_Handle_t quantParamsHandle,
    QairtQuantizeParams_BlockEncodingHandle_t blockEncodingHandle);

/**
 * @brief Get block encoding on the quantization parameter
 *
 * @param[in] quantParamsHandle A handle to the quantization parameter
 *
 * @param[out] blockEncodingHandle A handle to the retrieved block encoding
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _quantParamsHandle_ is
 *           not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _blockEncodingHandle_ is
 *           NULL
 *
 * @note Use corresponding API through QairtQuantizeParams_Handle_t.
 */
Qairt_Status_t QairtQuantizeParams_getBlockEncoding(
    QairtQuantizeParams_Handle_t quantParamsHandle,
    QairtQuantizeParams_BlockEncodingHandle_t* blockEncodingHandle);

/**
 * @brief Set blockwise expansion on the quantization parameter
 *
 * @param[in] quantParamsHandle A handle to the quantization parameter
 *
 * @param[out] blockwiseExpansionHandle A handle to the retrieved blockwise
 *                                      expansion
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _quantParamsHandle_ or
 *           _blockwiseExpansionHandle_ is not a valid handle
 *
 * @note Use corresponding API through QairtQuantizeParams_Handle_t.
 */
Qairt_Status_t QairtQuantizeParams_setBlockwiseExpansion(
    QairtQuantizeParams_Handle_t quantParamsHandle,
    QairtQuantizeParams_BlockwiseExpansionHandle_t blockwiseExpansionHandle);

/**
 * @brief Get blockwise expansion on the quantization parameter
 *
 * @param[in] quantParamsHandle A handle to the quantization parameter
 *
 * @param[out] blockwiseExpansionHandle A handle to the retrieved blockwise
 *                                      expansion
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _quantParamsHandle_ is
 *           not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _blockwiseExpansionHandle_ is
 *           NULL
 *
 * @note Use corresponding API through QairtQuantizeParams_Handle_t.
 */
Qairt_Status_t QairtQuantizeParams_getBlockwiseExpansion(
    QairtQuantizeParams_Handle_t quantParamsHandle,
    QairtQuantizeParams_BlockwiseExpansionHandle_t* blockwiseExpansionHandle);

/**
 * @brief Set vector encoding on the quantization parameter
 *
 * @param[in] quantParamsHandle A handle to the quantization parameter
 *
 * @param[out] vectorEncodingHandle A handle to the retrieved block encoding
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _quantParamsHandle_ or
 *           _vectorEncodingHandle_ is not a valid handle
 *
 * @note Use corresponding API through QairtQuantizeParams_Handle_t.
 */
Qairt_Status_t QairtQuantizeParams_setVectorEncoding(
    QairtQuantizeParams_Handle_t quantParamsHandle,
    QairtQuantizeParams_VectorEncodingHandle_t vectorEncodingHandle);

/**
 * @brief Get vector encoding on the quantization parameter
 *
 * @param[in] quantParamsHandle A handle to the quantization parameter
 *
 * @param[out] vectorEncodingHandle A handle to the retrieved vector encoding
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _quantParamsHandle_ is
 *           not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _vectorEncodingHandle_ is
 *           NULL
 *
 * @note Use corresponding API through QairtQuantizeParams_Handle_t.
 */
Qairt_Status_t QairtQuantizeParams_getVectorEncoding(
    QairtQuantizeParams_Handle_t quantParamsHandle,
    QairtQuantizeParams_VectorEncodingHandle_t* vectorEncodingHandle);

/**
 * @brief Set Microscaling encoding on the quantization parameter
 *
 * @param[in] quantParamsHandle A handle to the quantization parameter
 *
 * @param[in] microscalingEncodingHandle A handle to the Microscaling encoding
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _quantParamsHandle_ or
 *           _microscalingEncodingHandle_ is not a valid handle
 *
 * @note Use corresponding API through QairtQuantizeParams_Handle_t.
 */
Qairt_Status_t QairtQuantizeParams_setMicroscalingEncoding(
    QairtQuantizeParams_Handle_t quantParamsHandle,
    QairtQuantizeParams_MicroscalingEncodingHandle_t microscalingEncodingHandle);

/**
 * @brief Get Microscaling encoding from the quantization parameter
 *
 * @param[in] quantParamsHandle A handle to the quantization parameter
 *
 * @param[out] microscalingEncodingHandle A handle to the retrieved Microscaling
 *                                        encoding
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _quantParamsHandle_ is
 *           not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT:
 *           _microscalingEncodingHandle_ is NULL
 *
 * @note Use corresponding API through QairtQuantizeParams_Handle_t.
 */
Qairt_Status_t QairtQuantizeParams_getMicroscalingEncoding(
    QairtQuantizeParams_Handle_t quantParamsHandle,
    QairtQuantizeParams_MicroscalingEncodingHandle_t* microscalingEncodingHandle);

/**
 * @brief Set bitwidth-float-scale-offset block on the quantization parameter
 *
 * @param[in] quantParamsHandle A handle to the quantization parameter
 * @param[in] bwFloatBlockEncodingHandle A handle to the bitwidth-float-scale-offset
 *                               block.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _quantParamsHandle_ or
 *           _bwFloatBlockEncodingHandle_ is not a valid handle
 *
 * @note Use corresponding API through QairtQuantizeParams_Handle_t.
 **/
Qairt_Status_t QairtQuantizeParams_setBwFloatBlockEncoding(
    QairtQuantizeParams_Handle_t quantParamsHandle,
    QairtQuantizeParams_BwFloatBlockEncodingHandle_t bwFloatBlockEncodingHandle);

/**
 * @brief Get bitwidth-float-scale-offset block from the quantization parameter
 *
 * @param[in] quantParamsHandle A handle to the quantization parameter
 * @param[out] bwFloatBlockEncodingHandle A handle to the retrieved bitwidth-float-scale-offset
 *                                block.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _quantParamsHandle_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _bwFloatBlockEncodingHandle_ is NULL
 *
 * @note Use corresponding API through QairtQuantizeParams_Handle_t.
 */
Qairt_Status_t QairtQuantizeParams_getBwFloatBlockEncoding(
    QairtQuantizeParams_Handle_t quantParamsHandle,
    QairtQuantizeParams_BwFloatBlockEncodingHandle_t* bwFloatBlockEncodingHandle);

/**
 * @brief Set float-scale-offset block on the quantization parameter
 *
 * @param[in] quantParamsHandle A handle to the quantization parameter
 * @param[in] floatBlockEncodingHandle A handle to the float-scale-offset block.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _quantParamsHandle_ or
 *           _floatBlockEncodingHandle_ is not a valid handle
 *
 * @note Use corresponding API through QairtQuantizeParams_Handle_t.
 **/
Qairt_Status_t QairtQuantizeParams_setFloatBlockEncoding(
    QairtQuantizeParams_Handle_t quantParamsHandle,
    QairtQuantizeParams_FloatBlockEncodingHandle_t floatBlockEncodingHandle);

/**
 * @brief Get float-scale-offset block from the quantization parameter
 *
 * @param[in] quantParamsHandle A handle to the quantization parameter
 * @param[out] floatBlockEncodingHandle A handle to the retrieved float-scale-offset block.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _quantParamsHandle_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _floatBlockEncodingHandle_ is NULL
 *
 * @note Use corresponding API through QairtQuantizeParams_Handle_t.
 */
Qairt_Status_t QairtQuantizeParams_getFloatBlockEncoding(
    QairtQuantizeParams_Handle_t quantParamsHandle,
    QairtQuantizeParams_FloatBlockEncodingHandle_t* floatBlockEncodingHandle);

/**
 * @brief Set bit-width block mapped encoding on the quantization parameter
 *
 * @param[in] quantParamsHandle A handle to the quantization parameter
 * @param[in] bwBlockMappedHandle A handle to the bit-width block mapped encoding.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _quantParamsHandle_ or
 *           _bwBlockMappedHandle_ is not a valid handle
 *
 * @note Use corresponding API through QairtQuantizeParams_Handle_t.
 **/
Qairt_Status_t QairtQuantizeParams_setBwBlockMapped(
    QairtQuantizeParams_Handle_t quantParamsHandle,
    QairtQuantizeParams_BwBlockMappedHandle_t bwBlockMappedHandle);

/**
 * @brief Get bit-width block mapped encoding from the quantization parameter
 *
 * @param[in] quantParamsHandle A handle to the quantization parameter
 * @param[out] bwBlockMappedHandle A handle to the retrieved bit-width block mapped encoding.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _quantParamsHandle_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _bwBlockMappedHandle_ is NULL
 *
 * @note Use corresponding API through QairtQuantizeParams_Handle_t.
 */
Qairt_Status_t QairtQuantizeParams_getBwBlockMapped(
    QairtQuantizeParams_Handle_t quantParamsHandle,
    QairtQuantizeParams_BwBlockMappedHandle_t* bwBlockMappedHandle);

/**
 * @brief Set bit-width blockwise expansion mapped encoding on the quantization parameter
 *
 * @param[in] quantParamsHandle A handle to the quantization parameter
 * @param[in] bwBlockwiseExpansionMappedHandle A handle to the bit-width blockwise expansion
 *                                             mapped encoding.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _quantParamsHandle_ or
 *           _bwBlockwiseExpansionMappedHandle_ is not a valid handle
 *
 * @note Use corresponding API through QairtQuantizeParams_Handle_t.
 **/
Qairt_Status_t QairtQuantizeParams_setBwBlockwiseExpansionMapped(
    QairtQuantizeParams_Handle_t quantParamsHandle,
    QairtQuantizeParams_BwBlockwiseExpansionMappedHandle_t bwBlockwiseExpansionMappedHandle);

/**
 * @brief Get bit-width blockwise expansion mapped encoding from the quantization parameter
 *
 * @param[in] quantParamsHandle A handle to the quantization parameter
 * @param[out] bwBlockwiseExpansionMappedHandle A handle to the retrieved encoding.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _quantParamsHandle_ is not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _bwBlockwiseExpansionMappedHandle_ is NULL
 *
 * @note Use corresponding API through QairtQuantizeParams_Handle_t.
 */
Qairt_Status_t QairtQuantizeParams_getBwBlockwiseExpansionMapped(
    QairtQuantizeParams_Handle_t quantParamsHandle,
    QairtQuantizeParams_BwBlockwiseExpansionMappedHandle_t* bwBlockwiseExpansionMappedHandle);

/**
 * @brief Get quantization encoding of quantization parameter
 *
 * @param[in] quantParamsHandle A handle to the quantization parameter
 *
 * @param[out] encoding Retrieved quantization encoding
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_INVALID_HANDLE: _quantParamsHandle_ is
 *           not a valid handle
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _encoding_ is
 *           NULL
 *
 * @note Use corresponding API through QairtQuantizeParams_Handle_t.
 */
Qairt_Status_t QairtQuantizeParams_getQuantEncoding(QairtQuantizeParams_Handle_t quantParamsHandle,
                                                    Qairt_QuantizationEncoding_t* encoding);

//=============================================================================
// Function Pointer Typedefs
//=============================================================================

// ScaleOffset Function Pointer Typedefs

typedef Qairt_Status_t (*QairtQuantizeParams_ScaleOffset_CreateFn_t)(
    QairtQuantizeParams_ScaleOffsetHandle_t* scaleOffsetHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_ScaleOffset_FreeFn_t)(
    QairtQuantizeParams_ScaleOffsetHandle_t scaleOffsetHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_ScaleOffset_SetScaleFn_t)(
    QairtQuantizeParams_ScaleOffsetHandle_t scaleOffsetHandle, float scale);
typedef Qairt_Status_t (*QairtQuantizeParams_ScaleOffset_GetScaleFn_t)(
    QairtQuantizeParams_ScaleOffsetHandle_t scaleOffsetHandle, float* scale);
typedef Qairt_Status_t (*QairtQuantizeParams_ScaleOffset_SetOffsetFn_t)(
    QairtQuantizeParams_ScaleOffsetHandle_t scaleOffsetHandle, int32_t offset);
typedef Qairt_Status_t (*QairtQuantizeParams_ScaleOffset_GetOffsetFn_t)(
    QairtQuantizeParams_ScaleOffsetHandle_t scaleOffsetHandle, int32_t* offset);

// AxisScaleOffset Function Pointer Typedefs

typedef Qairt_Status_t (*QairtQuantizeParams_AxisScaleOffset_CreateFn_t)(
    QairtQuantizeParams_AxisScaleOffsetHandle_t* axisScaleOffsetHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_AxisScaleOffset_FreeFn_t)(
    QairtQuantizeParams_AxisScaleOffsetHandle_t axisScaleOffsetHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_AxisScaleOffset_SetScaleOffsetsFn_t)(
    QairtQuantizeParams_AxisScaleOffsetHandle_t axisScaleOffsetHandle,
    QairtQuantizeParams_ScaleOffsetHandle_t* scaleOffsetsHandle,
    uint32_t numScaleOffsets);
typedef Qairt_Status_t (*QairtQuantizeParams_AxisScaleOffset_GetNumScaleOffsetsFn_t)(
    QairtQuantizeParams_AxisScaleOffsetHandle_t axisScaleOffsetHandle, uint32_t* numScaleOffsets);
typedef Qairt_Status_t (*QairtQuantizeParams_AxisScaleOffset_GetScaleOffsetAtFn_t)(
    QairtQuantizeParams_AxisScaleOffsetHandle_t axisScaleOffsetHandle,
    uint32_t index,
    QairtQuantizeParams_ScaleOffsetHandle_t* scaleOffsetHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_AxisScaleOffset_SetAxisFn_t)(
    QairtQuantizeParams_AxisScaleOffsetHandle_t axisScaleOffsetHandle, int32_t axis);
typedef Qairt_Status_t (*QairtQuantizeParams_AxisScaleOffset_GetAxisFn_t)(
    QairtQuantizeParams_AxisScaleOffsetHandle_t axisScaleOffsetHandle, int32_t* axis);

// BwScaleOffset Function Pointer Typedefs

typedef Qairt_Status_t (*QairtQuantizeParams_BwScaleOffset_CreateFn_t)(
    QairtQuantizeParams_BwScaleOffsetHandle_t* bwScaleOffsetHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_BwScaleOffset_FreeFn_t)(
    QairtQuantizeParams_BwScaleOffsetHandle_t bwScaleOffsetHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_BwScaleOffset_SetScaleFn_t)(
    QairtQuantizeParams_BwScaleOffsetHandle_t bwScaleOffsetHandle, float scale);
typedef Qairt_Status_t (*QairtQuantizeParams_BwScaleOffset_GetScaleFn_t)(
    QairtQuantizeParams_BwScaleOffsetHandle_t bwScaleOffsetHandle, float* scale);
typedef Qairt_Status_t (*QairtQuantizeParams_BwScaleOffset_SetOffsetFn_t)(
    QairtQuantizeParams_BwScaleOffsetHandle_t bwScaleOffsetHandle, int32_t offset);
typedef Qairt_Status_t (*QairtQuantizeParams_BwScaleOffset_GetOffsetFn_t)(
    QairtQuantizeParams_BwScaleOffsetHandle_t bwScaleOffsetHandle, int32_t* offset);
typedef Qairt_Status_t (*QairtQuantizeParams_BwScaleOffset_SetBwFn_t)(
    QairtQuantizeParams_BwScaleOffsetHandle_t bwScaleOffsetHandle, uint32_t bw);
typedef Qairt_Status_t (*QairtQuantizeParams_BwScaleOffset_GetBwFn_t)(
    QairtQuantizeParams_BwScaleOffsetHandle_t bwScaleOffsetHandle, uint32_t* bw);

// BwAxisScaleOffset Function Pointer Typedefs

typedef Qairt_Status_t (*QairtQuantizeParams_BwAxisScaleOffset_CreateFn_t)(
    QairtQuantizeParams_BwAxisScaleOffsetHandle_t* bwAxisScaleOffsetHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_BwAxisScaleOffset_FreeFn_t)(
    QairtQuantizeParams_BwAxisScaleOffsetHandle_t bwAxisScaleOffsetHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_BwAxisScaleOffset_SetScalesFn_t)(
    QairtQuantizeParams_BwAxisScaleOffsetHandle_t bwAxisScaleOffsetHandle,
    float* scale,
    uint32_t numScales);
typedef Qairt_Status_t (*QairtQuantizeParams_BwAxisScaleOffset_GetNumScalesFn_t)(
    QairtQuantizeParams_BwAxisScaleOffsetHandle_t bwAxisScaleOffsetHandle, uint32_t* numScales);
typedef Qairt_Status_t (*QairtQuantizeParams_BwAxisScaleOffset_GetScaleAtFn_t)(
    QairtQuantizeParams_BwAxisScaleOffsetHandle_t bwAxisScaleOffsetHandle,
    uint32_t index,
    float* scale);
typedef Qairt_Status_t (*QairtQuantizeParams_BwAxisScaleOffset_SetOffsetsFn_t)(
    QairtQuantizeParams_BwAxisScaleOffsetHandle_t bwAxisScaleOffsetHandle,
    int32_t* offsets,
    uint32_t numOffsets);
typedef Qairt_Status_t (*QairtQuantizeParams_BwAxisScaleOffset_GetNumOffsetsFn_t)(
    QairtQuantizeParams_BwAxisScaleOffsetHandle_t bwAxisScaleOffsetHandle, uint32_t* numOffsets);
typedef Qairt_Status_t (*QairtQuantizeParams_BwAxisScaleOffset_GetOffsetAtFn_t)(
    QairtQuantizeParams_BwAxisScaleOffsetHandle_t bwAxisScaleOffsetHandle,
    uint32_t index,
    int32_t* offset);
typedef Qairt_Status_t (*QairtQuantizeParams_BwAxisScaleOffset_SetAxisFn_t)(
    QairtQuantizeParams_BwAxisScaleOffsetHandle_t bwAxisScaleOffsetHandle, int32_t axis);
typedef Qairt_Status_t (*QairtQuantizeParams_BwAxisScaleOffset_GetAxisFn_t)(
    QairtQuantizeParams_BwAxisScaleOffsetHandle_t bwAxisScaleOffsetHandle, int32_t* axis);
typedef Qairt_Status_t (*QairtQuantizeParams_BwAxisScaleOffset_SetBwFn_t)(
    QairtQuantizeParams_BwAxisScaleOffsetHandle_t bwAxisScaleOffsetHandle, uint32_t bw);
typedef Qairt_Status_t (*QairtQuantizeParams_BwAxisScaleOffset_GetBwFn_t)(
    QairtQuantizeParams_BwAxisScaleOffsetHandle_t bwAxisScaleOffsetHandle, uint32_t* bw);

// BwAxisScaleOffsetMapped Function Pointer Typedefs

typedef Qairt_Status_t (*QairtQuantizeParams_BwAxisScaleOffsetMapped_CreateFn_t)(
    QairtQuantizeParams_BwAxisScaleOffsetMappedHandle_t* bwAxisScaleOffsetMappedHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_BwAxisScaleOffsetMapped_FreeFn_t)(
    QairtQuantizeParams_BwAxisScaleOffsetMappedHandle_t bwAxisScaleOffsetMappedHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_BwAxisScaleOffsetMapped_SetScalesFn_t)(
    QairtQuantizeParams_BwAxisScaleOffsetMappedHandle_t bwAxisScaleOffsetMappedHandle,
    float* scale,
    uint32_t numScales);
typedef Qairt_Status_t (*QairtQuantizeParams_BwAxisScaleOffsetMapped_GetNumScalesFn_t)(
    QairtQuantizeParams_BwAxisScaleOffsetMappedHandle_t bwAxisScaleOffsetMappedHandle,
    uint32_t* numScales);
typedef Qairt_Status_t (*QairtQuantizeParams_BwAxisScaleOffsetMapped_GetScaleAtFn_t)(
    QairtQuantizeParams_BwAxisScaleOffsetMappedHandle_t bwAxisScaleOffsetMappedHandle,
    uint32_t index,
    float* scale);
typedef Qairt_Status_t (*QairtQuantizeParams_BwAxisScaleOffsetMapped_SetOffsetsFn_t)(
    QairtQuantizeParams_BwAxisScaleOffsetMappedHandle_t bwAxisScaleOffsetMappedHandle,
    int32_t* offsets,
    uint32_t numOffsets);
typedef Qairt_Status_t (*QairtQuantizeParams_BwAxisScaleOffsetMapped_GetNumOffsetsFn_t)(
    QairtQuantizeParams_BwAxisScaleOffsetMappedHandle_t bwAxisScaleOffsetMappedHandle,
    uint32_t* numOffsets);
typedef Qairt_Status_t (*QairtQuantizeParams_BwAxisScaleOffsetMapped_GetOffsetAtFn_t)(
    QairtQuantizeParams_BwAxisScaleOffsetMappedHandle_t bwAxisScaleOffsetMappedHandle,
    uint32_t index,
    int32_t* offset);
typedef Qairt_Status_t (*QairtQuantizeParams_BwAxisScaleOffsetMapped_SetAxisFn_t)(
    QairtQuantizeParams_BwAxisScaleOffsetMappedHandle_t bwAxisScaleOffsetMappedHandle,
    int32_t axis);
typedef Qairt_Status_t (*QairtQuantizeParams_BwAxisScaleOffsetMapped_GetAxisFn_t)(
    QairtQuantizeParams_BwAxisScaleOffsetMappedHandle_t bwAxisScaleOffsetMappedHandle,
    int32_t* axis);
typedef Qairt_Status_t (*QairtQuantizeParams_BwAxisScaleOffsetMapped_SetBwFn_t)(
    QairtQuantizeParams_BwAxisScaleOffsetMappedHandle_t bwAxisScaleOffsetMappedHandle, uint32_t bw);
typedef Qairt_Status_t (*QairtQuantizeParams_BwAxisScaleOffsetMapped_GetBwFn_t)(
    QairtQuantizeParams_BwAxisScaleOffsetMappedHandle_t bwAxisScaleOffsetMappedHandle,
    uint32_t* bw);
typedef Qairt_Status_t (*QairtQuantizeParams_BwAxisScaleOffsetMapped_SetMappingFn_t)(
    QairtQuantizeParams_BwAxisScaleOffsetMappedHandle_t bwAxisScaleOffsetMappedHandle,
    Qairt_QuantizationEncodingMapping_t mapping);
typedef Qairt_Status_t (*QairtQuantizeParams_BwAxisScaleOffsetMapped_GetMappingFn_t)(
    QairtQuantizeParams_BwAxisScaleOffsetMappedHandle_t bwAxisScaleOffsetMappedHandle,
    Qairt_QuantizationEncodingMapping_t* mapping);

// BlockEncoding Function Pointer Typedefs

typedef Qairt_Status_t (*QairtQuantizeParams_BlockEncoding_CreateFn_t)(
    QairtQuantizeParams_BlockEncodingHandle_t* blockEncodingHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_BlockEncoding_FreeFn_t)(
    QairtQuantizeParams_BlockEncodingHandle_t blockEncodingHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_BlockEncoding_SetBlockSizeFn_t)(
    QairtQuantizeParams_BlockEncodingHandle_t blockEncodingHandle,
    uint32_t* blockSizes,
    uint32_t numBlocks);
typedef Qairt_Status_t (*QairtQuantizeParams_BlockEncoding_GetNumBlockSizeFn_t)(
    QairtQuantizeParams_BlockEncodingHandle_t blockEncodingHandle, uint32_t* numBlocks);
typedef Qairt_Status_t (*QairtQuantizeParams_BlockEncoding_GetBlockSizeAtFn_t)(
    QairtQuantizeParams_BlockEncodingHandle_t blockEncodingHandle,
    uint32_t index,
    uint32_t* BlockSize);
typedef Qairt_Status_t (*QairtQuantizeParams_BlockEncoding_SetScaleOffsetsFn_t)(
    QairtQuantizeParams_BlockEncodingHandle_t blockEncodingHandle,
    QairtQuantizeParams_ScaleOffsetHandle_t* scaleOffsetsHandle,
    uint32_t numScaleOffsets);
typedef Qairt_Status_t (*QairtQuantizeParams_BlockEncoding_GetNumScaleOffsetsFn_t)(
    QairtQuantizeParams_BlockEncodingHandle_t blockEncodingHandle, uint32_t* numScaleOffsets);
typedef Qairt_Status_t (*QairtQuantizeParams_BlockEncoding_GetScaleOffsetAtFn_t)(
    QairtQuantizeParams_BlockEncodingHandle_t blockEncodingHandle,
    uint32_t index,
    QairtQuantizeParams_ScaleOffsetHandle_t* scaleOffsetHandle);

// VectorEncoding Function Pointer Typedefs

typedef Qairt_Status_t (*QairtQuantizeParams_VectorEncoding_CreateFn_t)(
    QairtQuantizeParams_VectorEncodingHandle_t* vectorEncodingHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_VectorEncoding_FreeFn_t)(
    QairtQuantizeParams_VectorEncodingHandle_t vectorEncodingHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_VectorEncoding_SetBwAxisScaleOffsetFn_t)(
    QairtQuantizeParams_VectorEncodingHandle_t vectorEncodingHandle,
    QairtQuantizeParams_BwAxisScaleOffsetHandle_t bwAxisScaleOffsetHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_VectorEncoding_GetBwAxisScaleOffsetFn_t)(
    QairtQuantizeParams_VectorEncodingHandle_t vectorEncodingHandle,
    QairtQuantizeParams_BwAxisScaleOffsetHandle_t* bwAxisScaleOffsetHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_VectorEncoding_SetRowsPerBlockFn_t)(
    QairtQuantizeParams_VectorEncodingHandle_t vectorEncodingHandle, uint32_t rows);
typedef Qairt_Status_t (*QairtQuantizeParams_VectorEncoding_GetRowsPerBlockFn_t)(
    QairtQuantizeParams_VectorEncodingHandle_t vectorEncodingHandle, uint32_t* rows);
typedef Qairt_Status_t (*QairtQuantizeParams_VectorEncoding_SetColsPerBlockFn_t)(
    QairtQuantizeParams_VectorEncodingHandle_t vectorEncodingHandle, uint32_t cols);
typedef Qairt_Status_t (*QairtQuantizeParams_VectorEncoding_GetColsPerBlockFn_t)(
    QairtQuantizeParams_VectorEncodingHandle_t vectorEncodingHandle, uint32_t* cols);
typedef Qairt_Status_t (*QairtQuantizeParams_VectorEncoding_SetVectorStrideFn_t)(
    QairtQuantizeParams_VectorEncodingHandle_t vectorEncodingHandle, uint32_t stride);
typedef Qairt_Status_t (*QairtQuantizeParams_VectorEncoding_GetVectorStrideFn_t)(
    QairtQuantizeParams_VectorEncodingHandle_t vectorEncodingHandle, uint32_t* stride);
typedef Qairt_Status_t (*QairtQuantizeParams_VectorEncoding_SetVectorDimensionFn_t)(
    QairtQuantizeParams_VectorEncodingHandle_t vectorEncodingHandle, uint32_t vectorDimension);
typedef Qairt_Status_t (*QairtQuantizeParams_VectorEncoding_GetVectorDimensionFn_t)(
    QairtQuantizeParams_VectorEncodingHandle_t vectorEncodingHandle, uint32_t* vectorDimension);
typedef Qairt_Status_t (*QairtQuantizeParams_VectorEncoding_SetIndexBwFn_t)(
    QairtQuantizeParams_VectorEncodingHandle_t vectorEncodingHandle, uint32_t bw);
typedef Qairt_Status_t (*QairtQuantizeParams_VectorEncoding_GetIndexBwFn_t)(
    QairtQuantizeParams_VectorEncodingHandle_t vectorEncodingHandle, uint32_t* bw);

// MicroscalingEncoding Function Pointer Typedefs

typedef Qairt_Status_t (*QairtQuantizeParams_MicroscalingEncoding_CreateFn_t)(
    QairtQuantizeParams_MicroscalingEncodingHandle_t* microscalingEncodingHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_MicroscalingEncoding_FreeFn_t)(
    QairtQuantizeParams_MicroscalingEncodingHandle_t microscalingEncodingHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_MicroscalingEncoding_SetValueEncodingFn_t)(
    QairtQuantizeParams_MicroscalingEncodingHandle_t microscalingEncodingHandle,
    Qairt_FloatEncoding_t valueEncoding);
typedef Qairt_Status_t (*QairtQuantizeParams_MicroscalingEncoding_GetValueEncodingFn_t)(
    QairtQuantizeParams_MicroscalingEncodingHandle_t microscalingEncodingHandle,
    Qairt_FloatEncoding_t* valueEncoding);
typedef Qairt_Status_t (*QairtQuantizeParams_MicroscalingEncoding_SetBlockDimensionsFn_t)(
    QairtQuantizeParams_MicroscalingEncodingHandle_t microscalingEncodingHandle,
    uint32_t* blockDimensions,
    uint32_t blockRank);
typedef Qairt_Status_t (*QairtQuantizeParams_MicroscalingEncoding_GetNumBlockDimensionsFn_t)(
    QairtQuantizeParams_MicroscalingEncodingHandle_t microscalingEncodingHandle,
    uint32_t* blockRank);
typedef Qairt_Status_t (*QairtQuantizeParams_MicroscalingEncoding_GetBlockDimensionAtFn_t)(
    QairtQuantizeParams_MicroscalingEncodingHandle_t microscalingEncodingHandle,
    uint32_t index,
    uint32_t* blockDimension);
typedef Qairt_Status_t (*QairtQuantizeParams_MicroscalingEncoding_GetNumBlockScalesFn_t)(
    QairtQuantizeParams_MicroscalingEncodingHandle_t microscalingEncodingHandle,
    uint32_t* numBlockScales);
typedef Qairt_Status_t (*QairtQuantizeParams_MicroscalingEncoding_SetScaleDataTypeFn_t)(
    QairtQuantizeParams_MicroscalingEncodingHandle_t microscalingEncodingHandle,
    Qairt_DataType_t scaleDataType);
typedef Qairt_Status_t (*QairtQuantizeParams_MicroscalingEncoding_GetScaleDataTypeFn_t)(
    QairtQuantizeParams_MicroscalingEncodingHandle_t microscalingEncodingHandle,
    Qairt_DataType_t* scaleDataType);
typedef Qairt_Status_t (*QairtQuantizeParams_MicroscalingEncoding_SetBlockScales8Fn_t)(
    QairtQuantizeParams_MicroscalingEncodingHandle_t microscalingEncodingHandle,
    uint8_t* blockScales8,
    uint32_t numScales);
typedef Qairt_Status_t (*QairtQuantizeParams_MicroscalingEncoding_GetBlockScale8AtFn_t)(
    QairtQuantizeParams_MicroscalingEncodingHandle_t microscalingEncodingHandle,
    uint32_t index,
    uint8_t* blockScale8);
typedef Qairt_Status_t (*QairtQuantizeParams_MicroscalingEncoding_SetBlockScalesFloatFn_t)(
    QairtQuantizeParams_MicroscalingEncodingHandle_t microscalingEncodingHandle,
    float* blockScalesFloat,
    uint32_t numScales);
typedef Qairt_Status_t (*QairtQuantizeParams_MicroscalingEncoding_GetBlockScaleFloatAtFn_t)(
    QairtQuantizeParams_MicroscalingEncodingHandle_t microscalingEncodingHandle,
    uint32_t index,
    float* blockScaleFloat);

// BlockwiseExpansion Function Pointer Typedefs

typedef Qairt_Status_t (*QairtQuantizeParams_BlockwiseExp_CreateFn_t)(
    QairtQuantizeParams_BlockwiseExpansionHandle_t* blockwiseExpansionHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_BlockwiseExp_FreeFn_t)(
    QairtQuantizeParams_BlockwiseExpansionHandle_t blockwiseExpansionHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_BlockwiseExp_SetAxisFn_t)(
    QairtQuantizeParams_BlockwiseExpansionHandle_t blockwiseExpansionHandle, int32_t axis);
typedef Qairt_Status_t (*QairtQuantizeParams_BlockwiseExp_GetAxisFn_t)(
    QairtQuantizeParams_BlockwiseExpansionHandle_t blockwiseExpansionHandle, int32_t* axis);
typedef Qairt_Status_t (*QairtQuantizeParams_BlockwiseExp_SetScaleOffsetsFn_t)(
    QairtQuantizeParams_BlockwiseExpansionHandle_t blockwiseExpansionHandle,
    QairtQuantizeParams_ScaleOffsetHandle_t* scaleOffsetsHandle,
    uint32_t numScaleOffsets);
typedef Qairt_Status_t (*QairtQuantizeParams_BlockwiseExp_GetNumScaleOffsetsFn_t)(
    QairtQuantizeParams_BlockwiseExpansionHandle_t blockwiseExpansionHandle,
    uint32_t* numScaleOffsets);
typedef Qairt_Status_t (*QairtQuantizeParams_BlockwiseExp_GetScaleOffsetAtFn_t)(
    QairtQuantizeParams_BlockwiseExpansionHandle_t blockwiseExpansionHandle,
    uint32_t index,
    QairtQuantizeParams_ScaleOffsetHandle_t* scaleOffsetHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_BlockwiseExp_SetNumBlocksPerAxisFn_t)(
    QairtQuantizeParams_BlockwiseExpansionHandle_t blockwiseExpansionHandle, uint32_t numBlocks);
typedef Qairt_Status_t (*QairtQuantizeParams_BlockwiseExp_GetNumBlocksPerAxisFn_t)(
    QairtQuantizeParams_BlockwiseExpansionHandle_t blockwiseExpansionHandle, uint32_t* numBlocks);
typedef Qairt_Status_t (*QairtQuantizeParams_BlockwiseExp_SetBlockScaleBwFn_t)(
    QairtQuantizeParams_BlockwiseExpansionHandle_t blockwiseExpansionHandle, uint32_t bitwidth);
typedef Qairt_Status_t (*QairtQuantizeParams_BlockwiseExp_GetBlockScaleBwFn_t)(
    QairtQuantizeParams_BlockwiseExpansionHandle_t blockwiseExpansionHandle, uint32_t* bitwidth);
typedef Qairt_Status_t (*QairtQuantizeParams_BlockwiseExp_SetBlockScale8Fn_t)(
    QairtQuantizeParams_BlockwiseExpansionHandle_t blockwiseExpansionHandle,
    uint8_t* scales,
    uint32_t numScales);
typedef Qairt_Status_t (*QairtQuantizeParams_BlockwiseExp_GetNumBlockScale8Fn_t)(
    QairtQuantizeParams_BlockwiseExpansionHandle_t blockwiseExpansionHandle, uint32_t* numScales);
typedef Qairt_Status_t (*QairtQuantizeParams_BlockwiseExp_GetBlockScale8AtFn_t)(
    QairtQuantizeParams_BlockwiseExpansionHandle_t blockwiseExpansionHandle,
    uint32_t index,
    uint8_t* scales);
typedef Qairt_Status_t (*QairtQuantizeParams_BlockwiseExp_SetBlockScale16Fn_t)(
    QairtQuantizeParams_BlockwiseExpansionHandle_t blockwiseExpansionHandle,
    uint16_t* scales,
    uint32_t numScales);
typedef Qairt_Status_t (*QairtQuantizeParams_BlockwiseExp_GetNumBlockScale16Fn_t)(
    QairtQuantizeParams_BlockwiseExpansionHandle_t blockwiseExpansionHandle, uint32_t* numScales);
typedef Qairt_Status_t (*QairtQuantizeParams_BlockwiseExp_GetBlockScale16AtFn_t)(
    QairtQuantizeParams_BlockwiseExpansionHandle_t blockwiseExpansionHandle,
    uint32_t index,
    uint16_t* scales);
typedef Qairt_Status_t (*QairtQuantizeParams_BlockwiseExp_GetStorageTypeFn_t)(
    QairtQuantizeParams_BlockwiseExpansionHandle_t blockwiseExpansionHandle,
    Qairt_BlockwiseExpansionBlockScaleStorageType_t* storageType);

// FloatScaleOffset Function Pointer Typedefs

typedef Qairt_Status_t (*QairtQuantizeParams_FloatScaleOffset_CreateFn_t)(
    QairtQuantizeParams_FloatScaleOffsetHandle_t* floatScaleOffsetHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_FloatScaleOffset_FreeFn_t)(
    QairtQuantizeParams_FloatScaleOffsetHandle_t floatScaleOffsetHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_FloatScaleOffset_SetScaleFn_t)(
    QairtQuantizeParams_FloatScaleOffsetHandle_t floatScaleOffsetHandle, float scale);
typedef Qairt_Status_t (*QairtQuantizeParams_FloatScaleOffset_GetScaleFn_t)(
    QairtQuantizeParams_FloatScaleOffsetHandle_t floatScaleOffsetHandle, float* scale);
typedef Qairt_Status_t (*QairtQuantizeParams_FloatScaleOffset_SetOffsetFn_t)(
    QairtQuantizeParams_FloatScaleOffsetHandle_t floatScaleOffsetHandle, float offset);
typedef Qairt_Status_t (*QairtQuantizeParams_FloatScaleOffset_GetOffsetFn_t)(
    QairtQuantizeParams_FloatScaleOffsetHandle_t floatScaleOffsetHandle, float* offset);

// BwFloatBlockEncoding Function Pointer Typedefs

typedef Qairt_Status_t (*QairtQuantizeParams_BwFloatBlockEncoding_CreateFn_t)(
    QairtQuantizeParams_BwFloatBlockEncodingHandle_t* bwFloatBlockEncodingHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_BwFloatBlockEncoding_FreeFn_t)(
    QairtQuantizeParams_BwFloatBlockEncodingHandle_t bwFloatBlockEncodingHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_BwFloatBlockEncoding_SetBwFn_t)(
    QairtQuantizeParams_BwFloatBlockEncodingHandle_t bwFloatBlockEncodingHandle, uint32_t bw);
typedef Qairt_Status_t (*QairtQuantizeParams_BwFloatBlockEncoding_GetBwFn_t)(
    QairtQuantizeParams_BwFloatBlockEncodingHandle_t bwFloatBlockEncodingHandle, uint32_t* bw);
typedef Qairt_Status_t (*QairtQuantizeParams_BwFloatBlockEncoding_SetBlockSizeFn_t)(
    QairtQuantizeParams_BwFloatBlockEncodingHandle_t bwFloatBlockEncodingHandle,
    uint32_t* blockSizes,
    uint32_t numBlocks);
typedef Qairt_Status_t (*QairtQuantizeParams_BwFloatBlockEncoding_GetNumBlockSizesFn_t)(
    QairtQuantizeParams_BwFloatBlockEncodingHandle_t bwFloatBlockEncodingHandle,
    uint32_t* numBlocks);
typedef Qairt_Status_t (*QairtQuantizeParams_BwFloatBlockEncoding_GetBlockSizeAtFn_t)(
    QairtQuantizeParams_BwFloatBlockEncodingHandle_t bwFloatBlockEncodingHandle,
    uint32_t index,
    uint32_t* blockSize);
typedef Qairt_Status_t (*QairtQuantizeParams_BwFloatBlockEncoding_SetFloatScaleOffsetsFn_t)(
    QairtQuantizeParams_BwFloatBlockEncodingHandle_t bwFloatBlockEncodingHandle,
    QairtQuantizeParams_FloatScaleOffsetHandle_t* floatScaleOffsetsHandle,
    uint32_t numFloatScaleOffsets);
typedef Qairt_Status_t (*QairtQuantizeParams_BwFloatBlockEncoding_GetNumFloatScaleOffsetsFn_t)(
    QairtQuantizeParams_BwFloatBlockEncodingHandle_t bwFloatBlockEncodingHandle,
    uint32_t* numFloatScaleOffsets);
typedef Qairt_Status_t (*QairtQuantizeParams_BwFloatBlockEncoding_GetFloatScaleOffsetAtFn_t)(
    QairtQuantizeParams_BwFloatBlockEncodingHandle_t bwFloatBlockEncodingHandle,
    uint32_t index,
    QairtQuantizeParams_FloatScaleOffsetHandle_t* floatScaleOffsetHandle);

// FloatBlockEncoding Function Pointer Typedefs

typedef Qairt_Status_t (*QairtQuantizeParams_FloatBlockEncoding_CreateFn_t)(
    QairtQuantizeParams_FloatBlockEncodingHandle_t* floatBlockEncodingHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_FloatBlockEncoding_FreeFn_t)(
    QairtQuantizeParams_FloatBlockEncodingHandle_t floatBlockEncodingHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_FloatBlockEncoding_SetBlockSizeFn_t)(
    QairtQuantizeParams_FloatBlockEncodingHandle_t floatBlockEncodingHandle,
    uint32_t* blockSizes,
    uint32_t numBlocks);
typedef Qairt_Status_t (*QairtQuantizeParams_FloatBlockEncoding_GetNumBlockSizesFn_t)(
    QairtQuantizeParams_FloatBlockEncodingHandle_t floatBlockEncodingHandle, uint32_t* numBlocks);
typedef Qairt_Status_t (*QairtQuantizeParams_FloatBlockEncoding_GetBlockSizeAtFn_t)(
    QairtQuantizeParams_FloatBlockEncodingHandle_t floatBlockEncodingHandle,
    uint32_t index,
    uint32_t* blockSize);
typedef Qairt_Status_t (*QairtQuantizeParams_FloatBlockEncoding_SetFloatScaleOffsetsFn_t)(
    QairtQuantizeParams_FloatBlockEncodingHandle_t floatBlockEncodingHandle,
    QairtQuantizeParams_FloatScaleOffsetHandle_t* floatScaleOffsetsHandle,
    uint32_t numFloatScaleOffsets);
typedef Qairt_Status_t (*QairtQuantizeParams_FloatBlockEncoding_GetNumFloatScaleOffsetsFn_t)(
    QairtQuantizeParams_FloatBlockEncodingHandle_t floatBlockEncodingHandle,
    uint32_t* numFloatScaleOffsets);
typedef Qairt_Status_t (*QairtQuantizeParams_FloatBlockEncoding_GetFloatScaleOffsetAtFn_t)(
    QairtQuantizeParams_FloatBlockEncodingHandle_t floatBlockEncodingHandle,
    uint32_t index,
    QairtQuantizeParams_FloatScaleOffsetHandle_t* floatScaleOffsetHandle);

// BwBlockMapped Function Pointer Typedefs

typedef Qairt_Status_t (*QairtQuantizeParams_BwBlockMapped_CreateFn_t)(
    QairtQuantizeParams_BwBlockMappedHandle_t* bwBlockMappedHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_BwBlockMapped_FreeFn_t)(
    QairtQuantizeParams_BwBlockMappedHandle_t bwBlockMappedHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_BwBlockMapped_SetBwFn_t)(
    QairtQuantizeParams_BwBlockMappedHandle_t bwBlockMappedHandle, uint32_t bw);
typedef Qairt_Status_t (*QairtQuantizeParams_BwBlockMapped_GetBwFn_t)(
    QairtQuantizeParams_BwBlockMappedHandle_t bwBlockMappedHandle, uint32_t* bw);
typedef Qairt_Status_t (*QairtQuantizeParams_BwBlockMapped_SetMappingFn_t)(
    QairtQuantizeParams_BwBlockMappedHandle_t bwBlockMappedHandle,
    Qairt_QuantizationEncodingMapping_t mapping);
typedef Qairt_Status_t (*QairtQuantizeParams_BwBlockMapped_GetMappingFn_t)(
    QairtQuantizeParams_BwBlockMappedHandle_t bwBlockMappedHandle,
    Qairt_QuantizationEncodingMapping_t* mapping);
typedef Qairt_Status_t (*QairtQuantizeParams_BwBlockMapped_SetBlockSizeFn_t)(
    QairtQuantizeParams_BwBlockMappedHandle_t bwBlockMappedHandle,
    uint32_t* blockSize,
    uint32_t numBlockSizes);
typedef Qairt_Status_t (*QairtQuantizeParams_BwBlockMapped_GetNumBlockSizesFn_t)(
    QairtQuantizeParams_BwBlockMappedHandle_t bwBlockMappedHandle, uint32_t* numBlockSizes);
typedef Qairt_Status_t (*QairtQuantizeParams_BwBlockMapped_GetBlockSizeAtFn_t)(
    QairtQuantizeParams_BwBlockMappedHandle_t bwBlockMappedHandle,
    uint32_t index,
    uint32_t* blockSize);
typedef Qairt_Status_t (*QairtQuantizeParams_BwBlockMapped_SetScaleOffsetsFn_t)(
    QairtQuantizeParams_BwBlockMappedHandle_t bwBlockMappedHandle,
    QairtQuantizeParams_ScaleOffsetHandle_t* scaleOffsetsHandle,
    uint32_t numScaleOffsets);
typedef Qairt_Status_t (*QairtQuantizeParams_BwBlockMapped_GetNumScaleOffsetsFn_t)(
    QairtQuantizeParams_BwBlockMappedHandle_t bwBlockMappedHandle, uint32_t* numScaleOffsets);
typedef Qairt_Status_t (*QairtQuantizeParams_BwBlockMapped_GetScaleOffsetAtFn_t)(
    QairtQuantizeParams_BwBlockMappedHandle_t bwBlockMappedHandle,
    uint32_t index,
    QairtQuantizeParams_ScaleOffsetHandle_t* scaleOffsetHandle);

// BwBlockwiseExpansionMapped Function Pointer Typedefs

typedef Qairt_Status_t (*QairtQuantizeParams_BwBlockwiseExpansionMapped_CreateFn_t)(
    QairtQuantizeParams_BwBlockwiseExpansionMappedHandle_t* bwBlockwiseExpansionMappedHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_BwBlockwiseExpansionMapped_FreeFn_t)(
    QairtQuantizeParams_BwBlockwiseExpansionMappedHandle_t bwBlockwiseExpansionMappedHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_BwBlockwiseExpansionMapped_SetBwFn_t)(
    QairtQuantizeParams_BwBlockwiseExpansionMappedHandle_t bwBlockwiseExpansionMappedHandle,
    uint32_t bw);
typedef Qairt_Status_t (*QairtQuantizeParams_BwBlockwiseExpansionMapped_GetBwFn_t)(
    QairtQuantizeParams_BwBlockwiseExpansionMappedHandle_t bwBlockwiseExpansionMappedHandle,
    uint32_t* bw);
typedef Qairt_Status_t (*QairtQuantizeParams_BwBlockwiseExpansionMapped_SetMappingFn_t)(
    QairtQuantizeParams_BwBlockwiseExpansionMappedHandle_t bwBlockwiseExpansionMappedHandle,
    Qairt_QuantizationEncodingMapping_t mapping);
typedef Qairt_Status_t (*QairtQuantizeParams_BwBlockwiseExpansionMapped_GetMappingFn_t)(
    QairtQuantizeParams_BwBlockwiseExpansionMappedHandle_t bwBlockwiseExpansionMappedHandle,
    Qairt_QuantizationEncodingMapping_t* mapping);
typedef Qairt_Status_t (*QairtQuantizeParams_BwBlockwiseExpansionMapped_SetAxisFn_t)(
    QairtQuantizeParams_BwBlockwiseExpansionMappedHandle_t bwBlockwiseExpansionMappedHandle,
    int32_t axis);
typedef Qairt_Status_t (*QairtQuantizeParams_BwBlockwiseExpansionMapped_GetAxisFn_t)(
    QairtQuantizeParams_BwBlockwiseExpansionMappedHandle_t bwBlockwiseExpansionMappedHandle,
    int32_t* axis);
typedef Qairt_Status_t (*QairtQuantizeParams_BwBlockwiseExpansionMapped_SetScaleOffsetsFn_t)(
    QairtQuantizeParams_BwBlockwiseExpansionMappedHandle_t bwBlockwiseExpansionMappedHandle,
    QairtQuantizeParams_ScaleOffsetHandle_t* scaleOffsetsHandle,
    uint32_t numScaleOffsets);
typedef Qairt_Status_t (*QairtQuantizeParams_BwBlockwiseExpansionMapped_GetNumScaleOffsetsFn_t)(
    QairtQuantizeParams_BwBlockwiseExpansionMappedHandle_t bwBlockwiseExpansionMappedHandle,
    uint32_t* numScaleOffsets);
typedef Qairt_Status_t (*QairtQuantizeParams_BwBlockwiseExpansionMapped_GetScaleOffsetAtFn_t)(
    QairtQuantizeParams_BwBlockwiseExpansionMappedHandle_t bwBlockwiseExpansionMappedHandle,
    uint32_t index,
    QairtQuantizeParams_ScaleOffsetHandle_t* scaleOffsetHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_BwBlockwiseExpansionMapped_SetNumBlocksPerAxisFn_t)(
    QairtQuantizeParams_BwBlockwiseExpansionMappedHandle_t bwBlockwiseExpansionMappedHandle,
    uint32_t numBlocksPerAxis);
typedef Qairt_Status_t (*QairtQuantizeParams_BwBlockwiseExpansionMapped_GetNumBlocksPerAxisFn_t)(
    QairtQuantizeParams_BwBlockwiseExpansionMappedHandle_t bwBlockwiseExpansionMappedHandle,
    uint32_t* numBlocksPerAxis);
typedef Qairt_Status_t (*QairtQuantizeParams_BwBlockwiseExpansionMapped_SetBlockScaleBitwidthFn_t)(
    QairtQuantizeParams_BwBlockwiseExpansionMappedHandle_t bwBlockwiseExpansionMappedHandle,
    uint32_t blockScaleBitwidth);
typedef Qairt_Status_t (*QairtQuantizeParams_BwBlockwiseExpansionMapped_GetBlockScaleBitwidthFn_t)(
    QairtQuantizeParams_BwBlockwiseExpansionMappedHandle_t bwBlockwiseExpansionMappedHandle,
    uint32_t* blockScaleBitwidth);
typedef Qairt_Status_t (*QairtQuantizeParams_BwBlockwiseExpansionMapped_SetBlocksScale8Fn_t)(
    QairtQuantizeParams_BwBlockwiseExpansionMappedHandle_t bwBlockwiseExpansionMappedHandle,
    uint8_t* blocksScale8,
    uint32_t numBlocksScale8);
typedef Qairt_Status_t (*QairtQuantizeParams_BwBlockwiseExpansionMapped_GetNumBlocksScale8Fn_t)(
    QairtQuantizeParams_BwBlockwiseExpansionMappedHandle_t bwBlockwiseExpansionMappedHandle,
    uint32_t* numBlocksScale8);
typedef Qairt_Status_t (*QairtQuantizeParams_BwBlockwiseExpansionMapped_GetBlocksScale8AtFn_t)(
    QairtQuantizeParams_BwBlockwiseExpansionMappedHandle_t bwBlockwiseExpansionMappedHandle,
    uint32_t index,
    uint8_t* blocksScale8);
typedef Qairt_Status_t (*QairtQuantizeParams_BwBlockwiseExpansionMapped_SetBlocksScale16Fn_t)(
    QairtQuantizeParams_BwBlockwiseExpansionMappedHandle_t bwBlockwiseExpansionMappedHandle,
    uint16_t* blocksScale16,
    uint32_t numBlocksScale16);
typedef Qairt_Status_t (*QairtQuantizeParams_BwBlockwiseExpansionMapped_GetNumBlocksScale16Fn_t)(
    QairtQuantizeParams_BwBlockwiseExpansionMappedHandle_t bwBlockwiseExpansionMappedHandle,
    uint32_t* numBlocksScale16);
typedef Qairt_Status_t (*QairtQuantizeParams_BwBlockwiseExpansionMapped_GetBlocksScale16AtFn_t)(
    QairtQuantizeParams_BwBlockwiseExpansionMappedHandle_t bwBlockwiseExpansionMappedHandle,
    uint32_t index,
    uint16_t* blocksScale16);

// QuantizeParams (Top-level) Function Pointer Typedefs
typedef Qairt_Status_t (*QairtQuantizeParams_QuantizeParams_CreateFn_t)(
    QairtQuantizeParams_Handle_t* quantizeParamsHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_QuantizeParams_FreeFn_t)(
    QairtQuantizeParams_Handle_t quantizeParamsHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_QuantizeParams_SetScaleOffsetFn_t)(
    QairtQuantizeParams_Handle_t quantParamsHandle,
    QairtQuantizeParams_ScaleOffsetHandle_t scaleOffsetHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_QuantizeParams_GetScaleOffsetFn_t)(
    QairtQuantizeParams_Handle_t quantParamsHandle,
    QairtQuantizeParams_ScaleOffsetHandle_t* scaleOffsetHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_QuantizeParams_SetAxisScaleOffsetFn_t)(
    QairtQuantizeParams_Handle_t quantParamsHandle,
    QairtQuantizeParams_AxisScaleOffsetHandle_t axisScaleOffsetHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_QuantizeParams_GetAxisScaleOffsetFn_t)(
    QairtQuantizeParams_Handle_t quantParamsHandle,
    QairtQuantizeParams_AxisScaleOffsetHandle_t* axisScaleOffsetHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_QuantizeParams_SetBwScaleOffsetFn_t)(
    QairtQuantizeParams_Handle_t quantParamsHandle,
    QairtQuantizeParams_BwScaleOffsetHandle_t bwScaleOffsetHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_QuantizeParams_GetBwScaleOffsetFn_t)(
    QairtQuantizeParams_Handle_t quantParamsHandle,
    QairtQuantizeParams_BwScaleOffsetHandle_t* bwScaleOffsetHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_QuantizeParams_SetBwAxisScaleOffsetFn_t)(
    QairtQuantizeParams_Handle_t quantParamsHandle,
    QairtQuantizeParams_BwAxisScaleOffsetHandle_t bwAxisScaleOffsetHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_QuantizeParams_GetBwAxisScaleOffsetFn_t)(
    QairtQuantizeParams_Handle_t quantParamsHandle,
    QairtQuantizeParams_BwAxisScaleOffsetHandle_t* bwAxisScaleOffsetHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_QuantizeParams_SetBwAxisScaleOffsetMappedFn_t)(
    QairtQuantizeParams_Handle_t quantParamsHandle,
    QairtQuantizeParams_BwAxisScaleOffsetMappedHandle_t bwAxisScaleOffsetMappedHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_QuantizeParams_GetBwAxisScaleOffsetMappedFn_t)(
    QairtQuantizeParams_Handle_t quantParamsHandle,
    QairtQuantizeParams_BwAxisScaleOffsetMappedHandle_t* bwAxisScaleOffsetMappedHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_QuantizeParams_SetBlockEncodingFn_t)(
    QairtQuantizeParams_Handle_t quantParamsHandle,
    QairtQuantizeParams_BlockEncodingHandle_t blockEncodingHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_QuantizeParams_GetBlockEncodingFn_t)(
    QairtQuantizeParams_Handle_t quantParamsHandle,
    QairtQuantizeParams_BlockEncodingHandle_t* blockEncodingHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_QuantizeParams_SetBlockwiseExpansionFn_t)(
    QairtQuantizeParams_Handle_t quantParamsHandle,
    QairtQuantizeParams_BlockwiseExpansionHandle_t blockwiseExpansionHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_QuantizeParams_GetBlockwiseExpansionFn_t)(
    QairtQuantizeParams_Handle_t quantParamsHandle,
    QairtQuantizeParams_BlockwiseExpansionHandle_t* blockwiseExpansionHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_QuantizeParams_SetVectorEncodingFn_t)(
    QairtQuantizeParams_Handle_t quantParamsHandle,
    QairtQuantizeParams_VectorEncodingHandle_t vectorEncodingHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_QuantizeParams_GetVectorEncodingFn_t)(
    QairtQuantizeParams_Handle_t quantParamsHandle,
    QairtQuantizeParams_VectorEncodingHandle_t* vectorEncodingHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_QuantizeParams_SetBwFloatBlockEncodingFn_t)(
    QairtQuantizeParams_Handle_t quantParamsHandle,
    QairtQuantizeParams_BwFloatBlockEncodingHandle_t bwFloatBlockEncodingHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_QuantizeParams_GetBwFloatBlockEncodingFn_t)(
    QairtQuantizeParams_Handle_t quantParamsHandle,
    QairtQuantizeParams_BwFloatBlockEncodingHandle_t* bwFloatBlockEncodingHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_QuantizeParams_GetQuantizeEncodingFn_t)(
    QairtQuantizeParams_Handle_t quantParamsHandle, Qairt_QuantizationEncoding_t* encoding);
typedef Qairt_Status_t (*QairtQuantizeParams_QuantizeParams_SetMicroscalingEncodingFn_t)(
    QairtQuantizeParams_Handle_t quantParamsHandle,
    QairtQuantizeParams_MicroscalingEncodingHandle_t microscalingEncodingHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_QuantizeParams_GetMicroscalingEncodingFn_t)(
    QairtQuantizeParams_Handle_t quantParamsHandle,
    QairtQuantizeParams_MicroscalingEncodingHandle_t* microscalingEncodingHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_QuantizeParams_SetBwBlockMappedFn_t)(
    QairtQuantizeParams_Handle_t quantParamsHandle,
    QairtQuantizeParams_BwBlockMappedHandle_t bwBlockMappedHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_QuantizeParams_GetBwBlockMappedFn_t)(
    QairtQuantizeParams_Handle_t quantParamsHandle,
    QairtQuantizeParams_BwBlockMappedHandle_t* bwBlockMappedHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_QuantizeParams_SetBwBlockwiseExpansionMappedFn_t)(
    QairtQuantizeParams_Handle_t quantParamsHandle,
    QairtQuantizeParams_BwBlockwiseExpansionMappedHandle_t bwBlockwiseExpansionMappedHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_QuantizeParams_GetBwBlockwiseExpansionMappedFn_t)(
    QairtQuantizeParams_Handle_t quantParamsHandle,
    QairtQuantizeParams_BwBlockwiseExpansionMappedHandle_t* bwBlockwiseExpansionMappedHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_QuantizeParams_SetFloatBlockEncodingFn_t)(
    QairtQuantizeParams_Handle_t quantParamsHandle,
    QairtQuantizeParams_FloatBlockEncodingHandle_t floatBlockEncodingHandle);
typedef Qairt_Status_t (*QairtQuantizeParams_QuantizeParams_GetFloatBlockEncodingFn_t)(
    QairtQuantizeParams_Handle_t quantParamsHandle,
    QairtQuantizeParams_FloatBlockEncodingHandle_t* floatBlockEncodingHandle);

//=============================================================================
// Interface Structures
//=============================================================================

/**
 * @brief Scale-offset interface structure containing function pointers for
 *        scale-offset operations.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtQuantizeParams_ScaleOffset_CreateFn_t create;
  QairtQuantizeParams_ScaleOffset_FreeFn_t free;
  QairtQuantizeParams_ScaleOffset_SetScaleFn_t setScale;
  QairtQuantizeParams_ScaleOffset_GetScaleFn_t getScale;
  QairtQuantizeParams_ScaleOffset_SetOffsetFn_t setOffset;
  QairtQuantizeParams_ScaleOffset_GetOffsetFn_t getOffset;
} QairtQuantizeParams_ScaleOffsetV1_t;

/**
 * @brief Axis scale-offset interface structure containing function pointers for
 *        axis scale-offset operations.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtQuantizeParams_AxisScaleOffset_CreateFn_t create;
  QairtQuantizeParams_AxisScaleOffset_FreeFn_t free;
  QairtQuantizeParams_AxisScaleOffset_SetScaleOffsetsFn_t setScaleOffsets;
  QairtQuantizeParams_AxisScaleOffset_GetNumScaleOffsetsFn_t getNumScaleOffsets;
  QairtQuantizeParams_AxisScaleOffset_GetScaleOffsetAtFn_t getScaleOffsetAt;
  QairtQuantizeParams_AxisScaleOffset_SetAxisFn_t setAxis;
  QairtQuantizeParams_AxisScaleOffset_GetAxisFn_t getAxis;
} QairtQuantizeParams_AxisScaleOffsetV1_t;

/**
 * @struct QairtQuantizeParams_BwScaleOffsetV1_t
 * @brief This struct contains the functions that define the V1 interface for
 *        QAIRT BwScaleOffset.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtQuantizeParams_BwScaleOffset_CreateFn_t create;
  QairtQuantizeParams_BwScaleOffset_FreeFn_t free;
  QairtQuantizeParams_BwScaleOffset_SetScaleFn_t setScale;
  QairtQuantizeParams_BwScaleOffset_GetScaleFn_t getScale;
  QairtQuantizeParams_BwScaleOffset_SetOffsetFn_t setOffset;
  QairtQuantizeParams_BwScaleOffset_GetOffsetFn_t getOffset;
  QairtQuantizeParams_BwScaleOffset_SetBwFn_t setBw;
  QairtQuantizeParams_BwScaleOffset_GetBwFn_t getBw;
} QairtQuantizeParams_BwScaleOffsetV1_t;

/**
 * @struct QairtQuantizeParams_BwAxisScaleOffsetV1_t
 * @brief This struct contains the functions that define the V1 interface for
 *        QAIRT BwAxisScaleOffset.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtQuantizeParams_BwAxisScaleOffset_CreateFn_t create;
  QairtQuantizeParams_BwAxisScaleOffset_FreeFn_t free;
  QairtQuantizeParams_BwAxisScaleOffset_SetScalesFn_t setScales;
  QairtQuantizeParams_BwAxisScaleOffset_GetNumScalesFn_t getNumScales;
  QairtQuantizeParams_BwAxisScaleOffset_GetScaleAtFn_t getScaleAt;
  QairtQuantizeParams_BwAxisScaleOffset_SetOffsetsFn_t setOffsets;
  QairtQuantizeParams_BwAxisScaleOffset_GetNumOffsetsFn_t getNumOffsets;
  QairtQuantizeParams_BwAxisScaleOffset_GetOffsetAtFn_t getOffsetAt;
  QairtQuantizeParams_BwAxisScaleOffset_SetAxisFn_t setAxis;
  QairtQuantizeParams_BwAxisScaleOffset_GetAxisFn_t getAxis;
  QairtQuantizeParams_BwAxisScaleOffset_SetBwFn_t setBw;
  QairtQuantizeParams_BwAxisScaleOffset_GetBwFn_t getBw;
} QairtQuantizeParams_BwAxisScaleOffsetV1_t;

/**
 * @struct QairtQuantizeParams_BwAxisScaleOffsetMappedV1_t
 * @brief This struct contains the functions that define the V1 interface for
 *        QAIRT BwAxisScaleOffsetMapped.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtQuantizeParams_BwAxisScaleOffsetMapped_CreateFn_t create;
  QairtQuantizeParams_BwAxisScaleOffsetMapped_FreeFn_t free;
  QairtQuantizeParams_BwAxisScaleOffsetMapped_SetScalesFn_t setScales;
  QairtQuantizeParams_BwAxisScaleOffsetMapped_GetNumScalesFn_t getNumScales;
  QairtQuantizeParams_BwAxisScaleOffsetMapped_GetScaleAtFn_t getScaleAt;
  QairtQuantizeParams_BwAxisScaleOffsetMapped_SetOffsetsFn_t setOffsets;
  QairtQuantizeParams_BwAxisScaleOffsetMapped_GetNumOffsetsFn_t getNumOffsets;
  QairtQuantizeParams_BwAxisScaleOffsetMapped_GetOffsetAtFn_t getOffsetAt;
  QairtQuantizeParams_BwAxisScaleOffsetMapped_SetAxisFn_t setAxis;
  QairtQuantizeParams_BwAxisScaleOffsetMapped_GetAxisFn_t getAxis;
  QairtQuantizeParams_BwAxisScaleOffsetMapped_SetBwFn_t setBw;
  QairtQuantizeParams_BwAxisScaleOffsetMapped_GetBwFn_t getBw;
  QairtQuantizeParams_BwAxisScaleOffsetMapped_SetMappingFn_t setMapping;
  QairtQuantizeParams_BwAxisScaleOffsetMapped_GetMappingFn_t getMapping;
} QairtQuantizeParams_BwAxisScaleOffsetMappedV1_t;

/**
 * @struct QairtQuantizeParams_BlockEncodingV1_t
 * @brief This struct contains the functions that define the V1 interface for
 *        QAIRT BlockEncoding.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtQuantizeParams_BlockEncoding_CreateFn_t create;
  QairtQuantizeParams_BlockEncoding_FreeFn_t free;
  QairtQuantizeParams_BlockEncoding_SetBlockSizeFn_t setBlockSize;
  QairtQuantizeParams_BlockEncoding_GetNumBlockSizeFn_t getNumBlockSize;
  QairtQuantizeParams_BlockEncoding_GetBlockSizeAtFn_t getBlockSizeAt;
  QairtQuantizeParams_BlockEncoding_SetScaleOffsetsFn_t setScaleOffsets;
  QairtQuantizeParams_BlockEncoding_GetNumScaleOffsetsFn_t getNumScaleOffsets;
  QairtQuantizeParams_BlockEncoding_GetScaleOffsetAtFn_t getScaleOffsetAt;
} QairtQuantizeParams_BlockEncodingV1_t;

/**
 * @struct QairtQuantizeParams_VectorEncodingV1_t
 * @brief This struct contains the functions that define the V1 interface for
 *        QAIRT VectorEncoding.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtQuantizeParams_VectorEncoding_CreateFn_t create;
  QairtQuantizeParams_VectorEncoding_FreeFn_t free;
  QairtQuantizeParams_VectorEncoding_SetBwAxisScaleOffsetFn_t setBwAxisScaleOffset;
  QairtQuantizeParams_VectorEncoding_GetBwAxisScaleOffsetFn_t getBwAxisScaleOffset;
  QairtQuantizeParams_VectorEncoding_SetRowsPerBlockFn_t setRowsPerBlock;
  QairtQuantizeParams_VectorEncoding_GetRowsPerBlockFn_t getRowsPerBlock;
  QairtQuantizeParams_VectorEncoding_SetColsPerBlockFn_t setColsPerBlock;
  QairtQuantizeParams_VectorEncoding_GetColsPerBlockFn_t getColsPerBlock;
  QairtQuantizeParams_VectorEncoding_SetVectorStrideFn_t setVectorStride;
  QairtQuantizeParams_VectorEncoding_GetVectorStrideFn_t getVectorStride;
  QairtQuantizeParams_VectorEncoding_SetVectorDimensionFn_t setVectorDimension;
  QairtQuantizeParams_VectorEncoding_GetVectorDimensionFn_t getVectorDimension;
  QairtQuantizeParams_VectorEncoding_SetIndexBwFn_t setIndexBw;
  QairtQuantizeParams_VectorEncoding_GetIndexBwFn_t getIndexBw;
} QairtQuantizeParams_VectorEncodingV1_t;

/**
 * @struct QairtQuantizeParams_FloatScaleOffsetV1_t
 * @brief This struct contains the functions that define the V1 interface for
 *        QAIRT FloatScaleOffset.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtQuantizeParams_FloatScaleOffset_CreateFn_t create;
  QairtQuantizeParams_FloatScaleOffset_FreeFn_t free;
  QairtQuantizeParams_FloatScaleOffset_SetScaleFn_t setScale;
  QairtQuantizeParams_FloatScaleOffset_GetScaleFn_t getScale;
  QairtQuantizeParams_FloatScaleOffset_SetOffsetFn_t setOffset;
  QairtQuantizeParams_FloatScaleOffset_GetOffsetFn_t getOffset;
} QairtQuantizeParams_FloatScaleOffsetV1_t;

/**
 * @struct QairtQuantizeParams_BwFloatBlockEncodingV1_t
 * @brief This struct contains the functions that define the V1 interface for
 *        QAIRT BwFloatBlockEncoding.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtQuantizeParams_BwFloatBlockEncoding_CreateFn_t create;
  QairtQuantizeParams_BwFloatBlockEncoding_FreeFn_t free;
  QairtQuantizeParams_BwFloatBlockEncoding_SetBwFn_t setBw;
  QairtQuantizeParams_BwFloatBlockEncoding_GetBwFn_t getBw;
  QairtQuantizeParams_BwFloatBlockEncoding_SetBlockSizeFn_t setBlockSize;
  QairtQuantizeParams_BwFloatBlockEncoding_GetNumBlockSizesFn_t getNumBlockSizes;
  QairtQuantizeParams_BwFloatBlockEncoding_GetBlockSizeAtFn_t getBlockSizeAt;
  QairtQuantizeParams_BwFloatBlockEncoding_SetFloatScaleOffsetsFn_t setFloatScaleOffsets;
  QairtQuantizeParams_BwFloatBlockEncoding_GetNumFloatScaleOffsetsFn_t getNumFloatScaleOffsets;
  QairtQuantizeParams_BwFloatBlockEncoding_GetFloatScaleOffsetAtFn_t getFloatScaleOffsetAt;
} QairtQuantizeParams_BwFloatBlockEncodingV1_t;

/**
 * @struct QairtQuantizeParams_FloatBlockEncodingV1_t
 * @brief This struct contains the functions that define the V1 interface for
 *        QAIRT FloatBlockEncoding.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtQuantizeParams_FloatBlockEncoding_CreateFn_t create;
  QairtQuantizeParams_FloatBlockEncoding_FreeFn_t free;
  QairtQuantizeParams_FloatBlockEncoding_SetBlockSizeFn_t setBlockSize;
  QairtQuantizeParams_FloatBlockEncoding_GetNumBlockSizesFn_t getNumBlockSizes;
  QairtQuantizeParams_FloatBlockEncoding_GetBlockSizeAtFn_t getBlockSizeAt;
  QairtQuantizeParams_FloatBlockEncoding_SetFloatScaleOffsetsFn_t setFloatScaleOffsets;
  QairtQuantizeParams_FloatBlockEncoding_GetNumFloatScaleOffsetsFn_t getNumFloatScaleOffsets;
  QairtQuantizeParams_FloatBlockEncoding_GetFloatScaleOffsetAtFn_t getFloatScaleOffsetAt;
} QairtQuantizeParams_FloatBlockEncodingV1_t;

/**
 * @struct QairtQuantizeParams_BlockwiseExpansionV1_t
 * @brief This struct contains the functions that define the V1 interface for
 *        QAIRT BlockwiseExpansion.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtQuantizeParams_BlockwiseExp_CreateFn_t create;
  QairtQuantizeParams_BlockwiseExp_FreeFn_t free;
  QairtQuantizeParams_BlockwiseExp_SetAxisFn_t setAxis;
  QairtQuantizeParams_BlockwiseExp_GetAxisFn_t getAxis;
  QairtQuantizeParams_BlockwiseExp_SetScaleOffsetsFn_t setScaleOffsets;
  QairtQuantizeParams_BlockwiseExp_GetNumScaleOffsetsFn_t getNumScaleOffsets;
  QairtQuantizeParams_BlockwiseExp_GetScaleOffsetAtFn_t getScaleOffsetAt;
  QairtQuantizeParams_BlockwiseExp_SetNumBlocksPerAxisFn_t setNumBlocksPerAxis;
  QairtQuantizeParams_BlockwiseExp_GetNumBlocksPerAxisFn_t getNumBlocksPerAxis;
  QairtQuantizeParams_BlockwiseExp_SetBlockScaleBwFn_t setBlockScaleBw;
  QairtQuantizeParams_BlockwiseExp_GetBlockScaleBwFn_t getBlockScaleBw;
  QairtQuantizeParams_BlockwiseExp_SetBlockScale8Fn_t setBlockScale8;
  QairtQuantizeParams_BlockwiseExp_GetNumBlockScale8Fn_t getNumBlockScale8;
  QairtQuantizeParams_BlockwiseExp_GetBlockScale8AtFn_t getBlockScale8At;
  QairtQuantizeParams_BlockwiseExp_SetBlockScale16Fn_t setBlockScale16;
  QairtQuantizeParams_BlockwiseExp_GetNumBlockScale16Fn_t getNumBlockScale16;
  QairtQuantizeParams_BlockwiseExp_GetBlockScale16AtFn_t getBlockScale16At;
  QairtQuantizeParams_BlockwiseExp_GetStorageTypeFn_t getStorageType;
} QairtQuantizeParams_BlockwiseExpansionV1_t;

//
/**
 * @struct QairtQuantizeParams_MicroscalingEncodingV1_t
 * @brief This struct contains the functions that define the V1 interface for
 *        QAIRT MicroscalingEncoding.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtQuantizeParams_MicroscalingEncoding_CreateFn_t create;
  QairtQuantizeParams_MicroscalingEncoding_FreeFn_t free;
  QairtQuantizeParams_MicroscalingEncoding_SetValueEncodingFn_t setValueEncoding;
  QairtQuantizeParams_MicroscalingEncoding_GetValueEncodingFn_t getValueEncoding;
  QairtQuantizeParams_MicroscalingEncoding_SetBlockDimensionsFn_t setBlockDimensions;
  QairtQuantizeParams_MicroscalingEncoding_GetNumBlockDimensionsFn_t getNumBlockDimensions;
  QairtQuantizeParams_MicroscalingEncoding_GetBlockDimensionAtFn_t getBlockDimensionAt;
  QairtQuantizeParams_MicroscalingEncoding_GetNumBlockScalesFn_t getNumBlockScales;
  QairtQuantizeParams_MicroscalingEncoding_SetScaleDataTypeFn_t setScaleDataType;
  QairtQuantizeParams_MicroscalingEncoding_GetScaleDataTypeFn_t getScaleDataType;
  QairtQuantizeParams_MicroscalingEncoding_SetBlockScales8Fn_t setBlockScales8;
  QairtQuantizeParams_MicroscalingEncoding_GetBlockScale8AtFn_t getBlockScale8At;
  QairtQuantizeParams_MicroscalingEncoding_SetBlockScalesFloatFn_t setBlockScalesFloat;
  QairtQuantizeParams_MicroscalingEncoding_GetBlockScaleFloatAtFn_t getBlockScaleFloatAt;
} QairtQuantizeParams_MicroscalingEncodingV1_t;

/**
 * @struct QairtQuantizeParams_BwBlockMappedV1_t
 * @brief This struct contains the functions that define the V1 interface for
 *        QAIRT BwBlockMapped.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtQuantizeParams_BwBlockMapped_CreateFn_t create;
  QairtQuantizeParams_BwBlockMapped_FreeFn_t free;
  QairtQuantizeParams_BwBlockMapped_SetBwFn_t setBw;
  QairtQuantizeParams_BwBlockMapped_GetBwFn_t getBw;
  QairtQuantizeParams_BwBlockMapped_SetMappingFn_t setMapping;
  QairtQuantizeParams_BwBlockMapped_GetMappingFn_t getMapping;
  QairtQuantizeParams_BwBlockMapped_SetBlockSizeFn_t setBlockSize;
  QairtQuantizeParams_BwBlockMapped_GetNumBlockSizesFn_t getNumBlockSizes;
  QairtQuantizeParams_BwBlockMapped_GetBlockSizeAtFn_t getBlockSizeAt;
  QairtQuantizeParams_BwBlockMapped_SetScaleOffsetsFn_t setScaleOffsets;
  QairtQuantizeParams_BwBlockMapped_GetNumScaleOffsetsFn_t getNumScaleOffsets;
  QairtQuantizeParams_BwBlockMapped_GetScaleOffsetAtFn_t getScaleOffsetAt;
} QairtQuantizeParams_BwBlockMappedV1_t;

/**
 * @struct QairtQuantizeParams_BwBlockwiseExpansionMappedV1_t
 * @brief This struct contains the functions that define the V1 interface for
 *        QAIRT BwBlockwiseExpansionMapped.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtQuantizeParams_BwBlockwiseExpansionMapped_CreateFn_t create;
  QairtQuantizeParams_BwBlockwiseExpansionMapped_FreeFn_t free;
  QairtQuantizeParams_BwBlockwiseExpansionMapped_SetBwFn_t setBw;
  QairtQuantizeParams_BwBlockwiseExpansionMapped_GetBwFn_t getBw;
  QairtQuantizeParams_BwBlockwiseExpansionMapped_SetMappingFn_t setMapping;
  QairtQuantizeParams_BwBlockwiseExpansionMapped_GetMappingFn_t getMapping;
  QairtQuantizeParams_BwBlockwiseExpansionMapped_SetAxisFn_t setAxis;
  QairtQuantizeParams_BwBlockwiseExpansionMapped_GetAxisFn_t getAxis;
  QairtQuantizeParams_BwBlockwiseExpansionMapped_SetScaleOffsetsFn_t setScaleOffsets;
  QairtQuantizeParams_BwBlockwiseExpansionMapped_GetNumScaleOffsetsFn_t getNumScaleOffsets;
  QairtQuantizeParams_BwBlockwiseExpansionMapped_GetScaleOffsetAtFn_t getScaleOffsetAt;
  QairtQuantizeParams_BwBlockwiseExpansionMapped_SetNumBlocksPerAxisFn_t setNumBlocksPerAxis;
  QairtQuantizeParams_BwBlockwiseExpansionMapped_GetNumBlocksPerAxisFn_t getNumBlocksPerAxis;
  QairtQuantizeParams_BwBlockwiseExpansionMapped_SetBlockScaleBitwidthFn_t setBlockScaleBitwidth;
  QairtQuantizeParams_BwBlockwiseExpansionMapped_GetBlockScaleBitwidthFn_t getBlockScaleBitwidth;
  QairtQuantizeParams_BwBlockwiseExpansionMapped_SetBlocksScale8Fn_t setBlocksScale8;
  QairtQuantizeParams_BwBlockwiseExpansionMapped_GetNumBlocksScale8Fn_t getNumBlocksScale8;
  QairtQuantizeParams_BwBlockwiseExpansionMapped_GetBlocksScale8AtFn_t getBlocksScale8At;
  QairtQuantizeParams_BwBlockwiseExpansionMapped_SetBlocksScale16Fn_t setBlocksScale16;
  QairtQuantizeParams_BwBlockwiseExpansionMapped_GetNumBlocksScale16Fn_t getNumBlocksScale16;
  QairtQuantizeParams_BwBlockwiseExpansionMapped_GetBlocksScale16AtFn_t getBlocksScale16At;
} QairtQuantizeParams_BwBlockwiseExpansionMappedV1_t;

/**
 * @struct QairtQuantizeParams_V1_t
 * @brief This struct contains the functions that define the V1 interface for
 *        QAIRT QuantizeParams.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtQuantizeParams_QuantizeParams_CreateFn_t create;
  QairtQuantizeParams_QuantizeParams_FreeFn_t free;
  QairtQuantizeParams_QuantizeParams_SetScaleOffsetFn_t setScaleOffset;
  QairtQuantizeParams_QuantizeParams_GetScaleOffsetFn_t getScaleOffset;
  QairtQuantizeParams_QuantizeParams_SetAxisScaleOffsetFn_t setAxisScaleOffset;
  QairtQuantizeParams_QuantizeParams_GetAxisScaleOffsetFn_t getAxisScaleOffset;
  QairtQuantizeParams_QuantizeParams_SetBwScaleOffsetFn_t setBwScaleOffset;
  QairtQuantizeParams_QuantizeParams_GetBwScaleOffsetFn_t getBwScaleOffset;
  QairtQuantizeParams_QuantizeParams_SetBwAxisScaleOffsetFn_t setBwAxisScaleOffset;
  QairtQuantizeParams_QuantizeParams_GetBwAxisScaleOffsetFn_t getBwAxisScaleOffset;
  QairtQuantizeParams_QuantizeParams_SetBlockEncodingFn_t setBlockEncoding;
  QairtQuantizeParams_QuantizeParams_GetBlockEncodingFn_t getBlockEncoding;
  QairtQuantizeParams_QuantizeParams_SetBlockwiseExpansionFn_t setBlockwiseExpansion;
  QairtQuantizeParams_QuantizeParams_GetBlockwiseExpansionFn_t getBlockwiseExpansion;
  QairtQuantizeParams_QuantizeParams_SetVectorEncodingFn_t setVectorEncoding;
  QairtQuantizeParams_QuantizeParams_GetVectorEncodingFn_t getVectorEncoding;
  QairtQuantizeParams_QuantizeParams_GetQuantizeEncodingFn_t getQuantEncoding;
  QairtQuantizeParams_QuantizeParams_SetBwAxisScaleOffsetMappedFn_t setBwAxisScaleOffsetMapped;
  QairtQuantizeParams_QuantizeParams_GetBwAxisScaleOffsetMappedFn_t getBwAxisScaleOffsetMapped;
  QairtQuantizeParams_QuantizeParams_SetMicroscalingEncodingFn_t setMicroscalingEncoding;
  QairtQuantizeParams_QuantizeParams_GetMicroscalingEncodingFn_t getMicroscalingEncoding;
  QairtQuantizeParams_QuantizeParams_SetBwFloatBlockEncodingFn_t setBwFloatBlockEncoding;
  QairtQuantizeParams_QuantizeParams_GetBwFloatBlockEncodingFn_t getBwFloatBlockEncoding;
  QairtQuantizeParams_QuantizeParams_SetBwBlockMappedFn_t setBwBlockMapped;
  QairtQuantizeParams_QuantizeParams_GetBwBlockMappedFn_t getBwBlockMapped;
  QairtQuantizeParams_QuantizeParams_SetBwBlockwiseExpansionMappedFn_t
      setBwBlockwiseExpansionMapped;
  QairtQuantizeParams_QuantizeParams_GetBwBlockwiseExpansionMappedFn_t
      getBwBlockwiseExpansionMapped;
  QairtQuantizeParams_QuantizeParams_SetFloatBlockEncodingFn_t setFloatBlockEncoding;
  QairtQuantizeParams_QuantizeParams_GetFloatBlockEncodingFn_t getFloatBlockEncoding;
} QairtQuantizeParams_V1_t;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // QAISW_QAIRTQUANTIZEPARAMS_H

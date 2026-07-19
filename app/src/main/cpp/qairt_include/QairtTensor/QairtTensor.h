//=============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//=============================================================================

/**
 * @file
 * @brief   Tensor component API
 *
 *          A header which contains the QAIRT Tensor Properties component and
 *          the interfaces it implements.
 */

#ifndef QAISW_QAIRTTENSOR_H
#define QAISW_QAIRTTENSOR_H

#include "QairtCommon/QairtScalar.h"
#include "QairtQuantizeParams/QairtQuantizeParams.h"
#include "QairtTensorCommon.h"
#include "QairtTensorMemory.h"
#include "QairtTensorProperties.h"

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
// Macros
//=============================================================================
#define QAIRT_TENSOR_V1_ID QAIRT_MIN_ID_TENSOR

#define QAIRT_TENSOR_MEMORY_V1_ID QAIRT_MIN_ID_TENSOR + 1

#define QAIRT_TENSOR_PROPERTIES_V1_ID QAIRT_MIN_ID_TENSOR + 2

#define QAIRT_TENSOR_CLIENT_BUFFER_V1_ID QAIRT_MIN_ID_TENSOR + 3

//=============================================================================
// Data Types
//=============================================================================
typedef struct _QairtTensor_Handle_t* QairtTensor_Handle_t;

//=============================================================================
// Public Functions
//=============================================================================
/**
 * @brief Create a tensor handle.
 *
 * @param[out] tensorHandle A handle to the created tensor.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Tensor was successfully created.
 *         - QAIRT_TENSOR_ERROR_INVALID_ARGUMENT: _tensorHandle_ is NULL.
 *         - QAIRT_TENSOR_ERROR_MEM_ALLOC: Memory allocation error.
 *
 * @note Use corresponding API through QairtTensor_V1_t.
 */
Qairt_Status_t QairtTensor_create(QairtTensor_Handle_t* tensorHandle);

/**
 * @brief Free a tensor handle.
 *
 * @param[in] tensorHandle A handle to a created tensor.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Tensor was successfully freed.
 *         - QAIRT_TENSOR_ERROR_INVALID_HANDLE: _tensorHandle_ is not a
 *           valid handle.
 *         - QAIRT_TENSOR_ERROR_MEM_ALLOC: Memory deallocation error.
 *
 * @note Use corresponding API through QairtTensor_V1_t.
 */
Qairt_Status_t QairtTensor_free(QairtTensor_Handle_t tensorHandle);

/**
 * @brief Set the unique identifier of a tensor.
 *
 * @param[in] tensorHandle A handle to a created tensor.
 * @param[in] id The unique identifier to set on the tensor.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: ID was successfully set.
 *         - QAIRT_TENSOR_ERROR_INVALID_HANDLE: _tensorHandle_ is not a
 *           valid handle.
 *
 * @note Use corresponding API through QairtTensor_V1_t.
 */
Qairt_Status_t QairtTensor_setId(QairtTensor_Handle_t tensorHandle, uint64_t id);

/**
 * @brief Get the unique identifier of a tensor.
 *
 * @param[in] tensorHandle A handle to a created tensor.
 * @param[out] id The unique identifier of the tensor.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: ID was successfully retrieved.
 *         - QAIRT_TENSOR_ERROR_INVALID_HANDLE: _tensorHandle_ is not a
 *           valid handle.
 *         - QAIRT_TENSOR_ERROR_INVALID_ARGUMENT: _id_ is NULL.
 *
 * @note Use corresponding API through QairtTensor_V1_t.
 */
Qairt_Status_t QairtTensor_getId(QairtTensor_Handle_t tensorHandle, uint64_t* id);

/**
 * @brief Check if a tensor is produced (output) or consumed (input).
 *
 * @param[in] tensorHandle A handle to a created tensor.
 *
 * @param[out] isProduced Set to 1 if the tensor is produced (output), 0 if
 *                        consumed (input).
 * @return Error code:
 *         - QAIRT_SUCCESS: Status was successfully retrieved.
 *         - QAIRT_TENSOR_ERROR_INVALID_HANDLE: _tensorHandle_ is not a
 *           valid handle.
 *         - QAIRT_TENSOR_ERROR_INVALID_ARGUMENT: _isProduced_ is NULL.
 *
 * @note Use corresponding API through QairtTensor_V1_t.
 */
Qairt_Status_t QairtTensor_getIsProduced(QairtTensor_Handle_t tensorHandle,
                                              uint8_t* isProduced);

/**
 * @brief Set the name of a tensor.
 *
 * @param[in] tensorHandle A handle to a created tensor.
 *
 * @param[in] name The name to assign to the tensor. Must be a valid
 *                 null-terminated string.
 * @return Error code:
 *         - QAIRT_SUCCESS: Name was successfully set.
 *         - QAIRT_TENSOR_ERROR_INVALID_HANDLE: _tensorHandle_ is not a
 *           valid handle.
 *         - QAIRT_TENSOR_ERROR_INVALID_ARGUMENT: _name_ is NULL.
 *
 * @note Use corresponding API through QairtTensor_V1_t.
 */
Qairt_Status_t QairtTensor_setName(QairtTensor_Handle_t tensorHandle, const char* name);

/**
 * @brief Get the name of a tensor.
 *
 * @param[in] tensorHandle A handle to a created tensor.
 *
 * @param[out] name Pointer to the tensor name string. The string is owned by
 *                  the tensor and should not be freed by the caller.
 * @return Error code:
 *         - QAIRT_SUCCESS: Name was successfully retrieved.
 *         - QAIRT_TENSOR_ERROR_INVALID_HANDLE: _tensorHandle_ is not a
 *           valid handle.
 *         - QAIRT_TENSOR_ERROR_INVALID_ARGUMENT: _name_ is NULL.
 *
 * @note Use corresponding API through QairtTensor_V1_t.
 */
Qairt_Status_t QairtTensor_getName(QairtTensor_Handle_t tensorHandle, const char** name);

/**
 * @brief Set tensor properties on a tensor.
 *
 * @param[in] tensorHandle A handle to a created tensor.
 *
 * @param[in] propertiesHandle A handle to tensor properties.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Properties were successfully set.
 *         - QAIRT_TENSOR_ERROR_INVALID_HANDLE: _tensorHandle_ or
 *           _propertiesHandle_ is not a valid handle.
 *         - QAIRT_TENSOR_ERROR_INVALID_ARGUMENT: One or more property
 *           values are invalid.
 *
 * @note Use corresponding API through QairtTensor_V1_t.
 */
Qairt_Status_t QairtTensor_setTensorProperties(
    QairtTensor_Handle_t tensorHandle, QairtTensor_PropertiesHandle_t propertiesHandle);

/**
 * @brief Get tensor properties from a tensor.
 *
 * @param[in] tensorHandle A handle to a created tensor.
 *
 * @param[out] propertiesHandle A referenced tensor properties handle. Reference
 *                              handles should not be freed. Referenced handles
 *                              are invalidated if modification operations are
 *                              performed from the same component API.
 * @return Error code:
 *         - QAIRT_SUCCESS: Properties were successfully retrieved.
 *         - QAIRT_TENSOR_ERROR_INVALID_HANDLE: _tensorHandle_ is not a
 *           valid handle.
 *         - QAIRT_TENSOR_ERROR_INVALID_ARGUMENT: _propertiesHandle_ is
 *           NULL.
 *
 * @note Use corresponding API through QairtTensor_V1_t.
 */
Qairt_Status_t QairtTensor_getTensorProperties(
    QairtTensor_Handle_t tensorHandle, QairtTensor_PropertiesHandle_t* propertiesHandle);

/**
 * @brief Set the data format of a tensor.
 *
 * @param[in] tensorHandle A handle to a created tensor.
 *
 * @param[in] dataFormat The data format value to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Data format was successfully set.
 *         - QAIRT_TENSOR_ERROR_INVALID_HANDLE: _tensorHandle_ is not a
 *           valid handle.
 *         - QAIRT_TENSOR_ERROR_INVALID_ARGUMENT: _dataFormat_ is invalid.
 *
 * @note Use corresponding API through QairtTensor_V1_t.
 */
Qairt_Status_t QairtTensor_setDataFormat(QairtTensor_Handle_t tensorHandle,
                                              uint32_t dataFormat);

/**
 * @brief Get the data format of a tensor.
 *
 * @param[in] tensorHandle A handle to a created tensor.
 *
 * @param[out] dataFormat The data format value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Data format was successfully retrieved.
 *         - QAIRT_TENSOR_ERROR_INVALID_HANDLE: _tensorHandle_ is not a
 *           valid handle.
 *         - QAIRT_TENSOR_ERROR_INVALID_ARGUMENT: _dataFormat_ is NULL.
 *
 * @note Use corresponding API through QairtTensor_V1_t.
 */
Qairt_Status_t QairtTensor_getDataFormat(QairtTensor_Handle_t tensorHandle,
                                              uint32_t* dataFormat);

/**
 * @brief Set the data type of a tensor.
 *
 * @param[in] tensorHandle A handle to a created tensor.
 *
 * @param[in] dataType The data type to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Data type was successfully set.
 *         - QAIRT_TENSOR_ERROR_INVALID_HANDLE: _tensorHandle_ is not a
 *           valid handle.
 *         - QAIRT_TENSOR_ERROR_INVALID_ARGUMENT: _dataType_ is invalid.
 *
 * @note Use corresponding API through QairtTensor_V1_t.
 */
Qairt_Status_t QairtTensor_setDataType(QairtTensor_Handle_t tensorHandle,
                                            Qairt_DataType_t dataType);

/**
 * @brief Get the data type of a tensor.
 *
 * @param[in] tensorHandle A handle to a created tensor.
 *
 * @param[out] dataType The data type value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Data type was successfully retrieved.
 *         - QAIRT_TENSOR_ERROR_INVALID_HANDLE: _tensorHandle_ is not a
 *           valid handle.
 *         - QAIRT_TENSOR_ERROR_INVALID_ARGUMENT: _dataType_ is NULL.
 *
 * @note Use corresponding API through QairtTensor_V1_t.
 */
Qairt_Status_t QairtTensor_getDataType(QairtTensor_Handle_t tensorHandle,
                                            Qairt_DataType_t* dataType);

/**
 * @brief Get the rank (number of dimensions) of a tensor.
 *
 * @param[in] tensorHandle A handle to a created tensor.
 *
 * @param[out] rank The rank value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Rank was successfully retrieved.
 *         - QAIRT_TENSOR_ERROR_INVALID_HANDLE: _tensorHandle_ is not a
 *           valid handle.
 *         - QAIRT_TENSOR_ERROR_INVALID_ARGUMENT: _rank_ is NULL.
 *
 * @note Use corresponding API through QairtTensor_V1_t.
 */
Qairt_Status_t QairtTensor_getRank(QairtTensor_Handle_t tensorHandle, uint32_t* rank);

/**
 * @brief Set the dimensions of a tensor.
 *
 * @param[in] tensorHandle A handle to a created tensor.
 *
 * @param[in] dims An array of dimension values.
 *
 * @param[in] size The number of dimensions (rank).
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Dimensions were successfully set.
 *         - QAIRT_TENSOR_ERROR_INVALID_HANDLE: _tensorHandle_ is not a
 *           valid handle.
 *         - QAIRT_TENSOR_ERROR_INVALID_ARGUMENT: _dims_ is NULL or _size_
 *           is invalid.
 *         - QAIRT_TENSOR_ERROR_MEM_ALLOC: Memory allocation error while
 *           setting dimensions.
 *
 * @note Use corresponding API through QairtTensor_V1_t.
 */
Qairt_Status_t QairtTensor_setDims(QairtTensor_Handle_t tensorHandle,
                                        uint32_t* dims,
                                        uint32_t size);

/**
 * @brief Get the dimensions of a tensor.
 *
 * @param[in] tensorHandle A handle to a created tensor.
 *
 * @param[out] dims An array to store the dimension values. Must be
 *                      pre-allocated with size equal to the tensor's rank.
 * @return Error code:
 *         - QAIRT_SUCCESS: Dimensions were successfully retrieved.
 *         - QAIRT_TENSOR_ERROR_INVALID_HANDLE: _tensorHandle_ is not a
 *           valid handle.
 *         - QAIRT_TENSOR_ERROR_INVALID_ARGUMENT: _dims_ is NULL.
 *
 * @note Use corresponding API through QairtTensor_V1_t.
 */
Qairt_Status_t QairtTensor_getDims(QairtTensor_Handle_t tensorHandle, uint32_t* dims);

/**
 * @brief Set tensor memory configuration.
 *
 * @param[in] tensorHandle A handle to a created tensor.
 *
 * @param[in] memoryHandle A handle to tensor memory configuration.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Memory configuration was successfully set.
 *         - QAIRT_TENSOR_ERROR_INVALID_HANDLE: _tensorHandle_ or
 *           _memoryHandle_ is not a valid handle.
 *         - QAIRT_TENSOR_ERROR_INVALID_ARGUMENT: Memory configuration is
 *           invalid.
 *
 * @note Use corresponding API through QairtTensor_V1_t.
 */
Qairt_Status_t QairtTensor_setTensorMemory(QairtTensor_Handle_t tensorHandle,
                                                QairtTensor_MemoryHandle_t memoryHandle);

/**
 * @brief Get tensor memory configuration.
 *
 * @param[in] tensorHandle A handle to a created tensor.
 *
 * @param[out] memoryHandle A referenced tensor memory handle. Reference
 *                          handles should not be freed. Referenced handles are
 *                          invalidated if modification operations are performed
 *                          from the same component API.
 * @return Error code:
 *         - QAIRT_SUCCESS: Memory configuration was successfully retrieved.
 *         - QAIRT_TENSOR_ERROR_INVALID_HANDLE: _tensorHandle_ is not a
 *           valid handle.
 *         - QAIRT_TENSOR_ERROR_INVALID_ARGUMENT: _memoryHandle_ is NULL.
 *
 * @note Use corresponding API through QairtTensor_V1_t.
 */
Qairt_Status_t QairtTensor_getTensorMemory(QairtTensor_Handle_t tensorHandle,
                                                QairtTensor_MemoryHandle_t* memoryHandle);

/**
 * @brief Set which dimensions are dynamic (can change at runtime).
 *
 * @param[in] tensorHandle A handle to a created tensor.
 *
 * @param[in] isDynamic An array of boolean values (0 or 1) indicating which
 *                      dimensions are dynamic. Array length must equal the
 *                      tensor's rank.
 * @param[in] size The number of elements in the isDynamic array (must equal
 *                 rank).
 * @return Error code:
 *         - QAIRT_SUCCESS: Dynamic dimensions were successfully set.
 *         - QAIRT_TENSOR_ERROR_INVALID_HANDLE: _tensorHandle_ is not a
 *           valid handle.
 *         - QAIRT_TENSOR_ERROR_INVALID_ARGUMENT: _isDynamic_ is NULL or
 *           _size_ doesn't match rank.
 *
 * @note Use corresponding API through QairtTensor_V1_t.
 */
Qairt_Status_t QairtTensor_setIsDynamicDimensions(QairtTensor_Handle_t tensorHandle,
                                                       const uint8_t* isDynamic,
                                                       uint32_t size);

/**
 * @brief Get which dimensions are dynamic.
 *
 * @param[in] tensorHandle A handle to a created tensor.
 *
 * @param[out] isDynamic An array to store boolean values indicating which
 *                       dimensions are dynamic. Must be pre-allocated with size
 *                       equal to the tensor's rank.
 * @return Error code:
 *         - QAIRT_SUCCESS: Dynamic dimensions were successfully retrieved.
 *         - QAIRT_TENSOR_ERROR_INVALID_HANDLE: _tensorHandle_ is not a
 *           valid handle.
 *         - QAIRT_TENSOR_ERROR_INVALID_ARGUMENT: _isDynamic_ is NULL.
 *
 * @note Use corresponding API through QairtTensor_V1_t.
 */
Qairt_Status_t QairtTensor_getIsDynamicDimensions(QairtTensor_Handle_t tensorHandle,
                                                       uint8_t* isDynamic);

/**
 * @brief Set sparse parameters for a tensor.
 *
 * @param[in] tensorHandle A handle to a created tensor.
 *
 * @param[in] sparseParamsHandle A handle to sparse parameters configuration.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Sparse parameters were successfully set.
 *         - QAIRT_TENSOR_ERROR_INVALID_HANDLE: _tensorHandle_ or
 *           _sparseParamsHandle_ is not a valid handle.
 *         - QAIRT_TENSOR_ERROR_INVALID_ARGUMENT: Sparse parameters are
 *           invalid.
 *         - QAIRT_TENSOR_ERROR_UNSUPPORTED_FEATURE: Sparse tensors are
 *           not supported.
 *
 * @note Use corresponding API through QairtTensor_V1_t.
 */
Qairt_Status_t QairtTensor_setSparseParams(QairtTensor_Handle_t tensorHandle,
                                                QairtSparseParams_Handle_t sparseParamsHandle);

/**
 * @brief Get sparse parameters from a tensor.
 *
 * @param[in] tensorHandle A handle to a created tensor.
 *
 * @param[out] sparseParamsHandle A referenced sparse parameters handle.
 *                                Reference handles should not be freed.
 *                                Referenced handles are invalidated if
 *                                modification operations are performed from
 *                                the same component API.
 * @return Error code:
 *         - QAIRT_SUCCESS: Sparse parameters were successfully retrieved.
 *         - QAIRT_TENSOR_ERROR_INVALID_HANDLE: _tensorHandle_ is not a
 *           valid handle.
 *         - QAIRT_TENSOR_ERROR_INVALID_ARGUMENT: _sparseParamsHandle_ is
 *           NULL.
 *
 * @note Use corresponding API through QairtTensor_V1_t.
 */
Qairt_Status_t QairtTensor_getSparseParams(QairtTensor_Handle_t tensorHandle,
                                                QairtSparseParams_Handle_t* sparseParamsHandle);

/**
 * @brief Set quantization parameters for a tensor.
 *
 * @param[in] tensorHandle A handle to a created tensor.
 *
 * @param[in] quantParamsHandle A handle to quantization parameters
 *                              configuration.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Quantization parameters were successfully set.
 *         - QAIRT_TENSOR_ERROR_INVALID_HANDLE: _tensorHandle_ or
 *           _quantParamsHandle_ is not a valid handle.
 *         - QAIRT_TENSOR_ERROR_INVALID_ARGUMENT: Quantization parameters are
 *           invalid.
 *         - QAIRT_TENSOR_ERROR_UNSUPPORTED_FEATURE: Quantized tensors are
 *           not supported.
 *
 * @note Use corresponding API through QairtTensor_V1_t.
 */
Qairt_Status_t QairtTensor_setQuantizeParams(QairtTensor_Handle_t tensorHandle,
                                                  QairtQuantizeParams_Handle_t quantParamsHandle);

/**
 * @brief Get quantization parameters from a tensor.
 *
 * @param[in] tensorHandle A handle to a created tensor.
 *
 * @param[out] quantParamsHandle A referenced quantization parameters handle.
 *                               Reference handles should not be freed.
 *                               Referenced handles are invalidated if
 *                               modification operations are performed from
 *                               the same component API.
 * @return Error code:
 *         - QAIRT_SUCCESS: Quantization parameters were successfully retrieved.
 *         - QAIRT_TENSOR_ERROR_INVALID_HANDLE: _tensorHandle_ is not a
 *           valid handle.
 *         - QAIRT_TENSOR_ERROR_INVALID_ARGUMENT: _quantParamsHandle_ is
 *           NULL.
 *
 * @note Use corresponding API through QairtTensor_V1_t.
 */
Qairt_Status_t QairtTensor_getQuantizeParams(
    QairtTensor_Handle_t tensorHandle, QairtQuantizeParams_Handle_t* quantParamsHandle);

typedef Qairt_Status_t (*QairtTensor_CreateFn_t)(QairtTensor_Handle_t*);

typedef Qairt_Status_t (*QairtTensor_FreeFn_t)(QairtTensor_Handle_t);

typedef Qairt_Status_t (*QairtTensor_SetIdFn_t)(QairtTensor_Handle_t, uint64_t);

typedef Qairt_Status_t (*QairtTensor_GetIdFn_t)(QairtTensor_Handle_t, uint64_t*);

typedef Qairt_Status_t (*QairtTensor_GetIsProducedFn_t)(QairtTensor_Handle_t, uint8_t*);

typedef Qairt_Status_t (*QairtTensor_SetNameFn_t)(QairtTensor_Handle_t, const char*);
typedef Qairt_Status_t (*QairtTensor_GetNameFn_t)(QairtTensor_Handle_t, const char**);

typedef Qairt_Status_t (*QairtTensor_SetTensorPropertiesFn_t)(
    QairtTensor_Handle_t, QairtTensor_PropertiesHandle_t);
typedef Qairt_Status_t (*QairtTensor_GetTensorPropertiesFn_t)(
    QairtTensor_Handle_t, QairtTensor_PropertiesHandle_t*);

typedef Qairt_Status_t (*QairtTensor_SetDataFormatFn_t)(QairtTensor_Handle_t, uint32_t);
typedef Qairt_Status_t (*QairtTensor_GetDataFormatFn_t)(QairtTensor_Handle_t, uint32_t*);

typedef Qairt_Status_t (*QairtTensor_SetDataTypeFn_t)(QairtTensor_Handle_t,
                                                           Qairt_DataType_t);
typedef Qairt_Status_t (*QairtTensor_GetDataTypeFn_t)(QairtTensor_Handle_t,
                                                           Qairt_DataType_t*);

typedef Qairt_Status_t (*QairtTensor_GetRankFn_t)(QairtTensor_Handle_t, uint32_t*);

typedef Qairt_Status_t (*QairtTensor_SetDimsFn_t)(QairtTensor_Handle_t, uint32_t*, uint32_t);
typedef Qairt_Status_t (*QairtTensor_GetDimsFn_t)(QairtTensor_Handle_t, uint32_t*);

typedef Qairt_Status_t (*QairtTensor_SetTensorMemoryFn_t)(QairtTensor_Handle_t,
                                                               QairtTensor_MemoryHandle_t);
typedef Qairt_Status_t (*QairtTensor_GetTensorMemoryFn_t)(QairtTensor_Handle_t,
                                                               QairtTensor_MemoryHandle_t*);

typedef Qairt_Status_t (*QairtTensor_SetIsDynamicDimensionsFn_t)(QairtTensor_Handle_t,
                                                                      const uint8_t*,
                                                                      uint32_t);
typedef Qairt_Status_t (*QairtTensor_GetIsDynamicDimensionsFn_t)(QairtTensor_Handle_t,
                                                                      uint8_t*);

typedef Qairt_Status_t (*QairtTensor_SetSparseParamsFn_t)(QairtTensor_Handle_t,
                                                               QairtSparseParams_Handle_t);
typedef Qairt_Status_t (*QairtTensor_GetSparseParamsFn_t)(QairtTensor_Handle_t,
                                                               QairtSparseParams_Handle_t*);

typedef Qairt_Status_t (*QairtTensor_SetQuantizeParamsFn_t)(QairtTensor_Handle_t,
                                                                 QairtQuantizeParams_Handle_t);
typedef Qairt_Status_t (*QairtTensor_GetQuantizeParamsFn_t)(QairtTensor_Handle_t,
                                                                 QairtQuantizeParams_Handle_t*);

//=============================================================================
// Public Interfaces
//=============================================================================
/**
 * @struct QairtTensor_V1_t
 * @brief This struct contains the functions that define the V1 interface for
 *        QAIRT Tensor. This interface can be retrieved through the id
 *        QAIRT_TENSOR_V1_ID.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtTensor_CreateFn_t create;
  QairtTensor_FreeFn_t free;
  QairtTensor_SetIdFn_t setId;
  QairtTensor_GetIdFn_t getId;
  QairtTensor_GetIsProducedFn_t getIsProduced;
  QairtTensor_SetNameFn_t setName;
  QairtTensor_GetNameFn_t getName;
  QairtTensor_SetTensorPropertiesFn_t setTensorProperties;
  QairtTensor_GetTensorPropertiesFn_t getTensorProperties;
  QairtTensor_SetDataFormatFn_t setDataFormat;
  QairtTensor_GetDataFormatFn_t getDataFormat;
  QairtTensor_SetDataTypeFn_t setDataType;
  QairtTensor_GetDataTypeFn_t getDataType;
  QairtTensor_GetRankFn_t getRank;
  QairtTensor_SetDimsFn_t setDims;
  QairtTensor_GetDimsFn_t getDims;
  QairtTensor_SetTensorMemoryFn_t setTensorMemory;
  QairtTensor_GetTensorMemoryFn_t getTensorMemory;
  QairtTensor_SetIsDynamicDimensionsFn_t setIsDynamicDimensions;
  QairtTensor_GetIsDynamicDimensionsFn_t getIsDynamicDimensions;
  QairtTensor_SetSparseParamsFn_t setSparseParams;
  QairtTensor_GetSparseParamsFn_t getSparseParams;
  QairtTensor_SetQuantizeParamsFn_t setQuantizeParams;
  QairtTensor_GetQuantizeParamsFn_t getQuantizeParams;
} QairtTensor_V1_t;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // QAISW_QAIRTTENSOR_H

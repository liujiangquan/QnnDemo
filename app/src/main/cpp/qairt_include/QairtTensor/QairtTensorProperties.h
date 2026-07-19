//=============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//=============================================================================

/**
 * @file
 * @brief   Tensor Properties component API
 *
 *          A header which contains the QAIRT Tensor Properties component and
 *          the interfaces it implements.
 */

#ifndef QAISW_QAIRTTENSORPROPERTIES_H
#define QAISW_QAIRTTENSORPROPERTIES_H

#include "QairtTensorCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
// Macros
//=============================================================================

//=============================================================================
// Data Types
//=============================================================================
typedef struct _QairtTensor_PropertiesHandle_t* QairtTensor_PropertiesHandle_t;

//=============================================================================
// Public Functions
//=============================================================================

/**
 * @brief Create a tensor properties handle.
 *
 * @param[out] tensorPropertiesHandle A handle to the created tensor properties.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Tensor properties was successfully created.
 *         - QAIRT_TENSOR_ERROR_INVALID_ARGUMENT: _tensorPropertiesHandle_
 *           is NULL.
 *         - QAIRT_TENSOR_ERROR_MEM_ALLOC: Memory allocation error while
 *           creating tensor properties.
 *
 * @note Use corresponding API through QairtTensor_PropertiesV1_t.
 */
Qairt_Status_t QairtTensor_Properties_create(
    QairtTensor_PropertiesHandle_t* tensorPropertiesHandle);

/**
 * @brief Free a tensor properties handle and associated resources.
 *
 * @param[in] tensorPropertiesHandle A handle to a created tensor properties.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Tensor properties was successfully freed.
 *         - QAIRT_TENSOR_ERROR_INVALID_HANDLE: _tensorPropertiesHandle_
 *           is not a valid handle.
 *         - QAIRT_TENSOR_ERROR_MEM_ALLOC: Memory deallocation error.
 *
 * @note Use corresponding API through QairtTensor_PropertiesV1_t.
 */
Qairt_Status_t QairtTensor_Properties_free(
    QairtTensor_PropertiesHandle_t tensorPropertiesHandle);

/**
 * @brief Set the input property of tensor properties.
 *
 * @param[in] tensorPropertiesHandle A handle to a created tensor properties.
 *
 * @param[in] isInput Flag indicating whether tensor is an input (1) or not (0).
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Input property was successfully set.
 *         - QAIRT_TENSOR_ERROR_INVALID_HANDLE: _tensorPropertiesHandle_
 *           is not a valid handle.
 *
 * @note Use corresponding API through QairtTensor_PropertiesV1_t.
 */
Qairt_Status_t QairtTensor_Properties_setIsInput(
    QairtTensor_PropertiesHandle_t tensorPropertiesHandle, uint8_t isInput);

/**
 * @brief Get the input property of tensor properties.
 *
 * @param[in] tensorPropertiesHandle A handle to a created tensor properties.
 *
 * @param[out] isInput Flag indicating whether tensor is an input (1) or not
 *                     (0).
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Input property was successfully retrieved.
 *         - QAIRT_TENSOR_ERROR_INVALID_HANDLE: _tensorPropertiesHandle_
 *           is not a valid handle.
 *         - QAIRT_TENSOR_ERROR_INVALID_ARGUMENT: _isInput_ is NULL.
 *
 * @note Use corresponding API through QairtTensor_PropertiesV1_t.
 */
Qairt_Status_t QairtTensor_Properties_getIsInput(
    QairtTensor_PropertiesHandle_t tensorPropertiesHandle, uint8_t* isInput);

/**
 * @brief Set the output property of tensor properties.
 *
 * @param[in] tensorPropertiesHandle A handle to a created tensor properties.
 *
 * @param[in] isOutput Flag indicating whether tensor is an output (1) or not
 *                     (0).
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Output property was successfully set.
 *         - QAIRT_TENSOR_ERROR_INVALID_HANDLE: _tensorPropertiesHandle_
 *           is not a valid handle.
 *
 * @note Use corresponding API through QairtTensor_PropertiesV1_t.
 */
Qairt_Status_t QairtTensor_Properties_setIsOutput(
    QairtTensor_PropertiesHandle_t tensorPropertiesHandle, uint8_t isOutput);

/**
 * @brief Get the output property of tensor properties.
 *
 * @param[in] tensorPropertiesHandle A handle to a created tensor properties.
 *
 * @param[out] isOutput Flag indicating whether tensor is an output (1) or not
 *                      (0).
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Output property was successfully retrieved.
 *         - QAIRT_TENSOR_ERROR_INVALID_HANDLE: _tensorPropertiesHandle_
 *           is not a valid handle.
 *         - QAIRT_TENSOR_ERROR_INVALID_ARGUMENT: _isOutput_ is NULL.
 *
 * @note Use corresponding API through QairtTensor_PropertiesV1_t.
 */
Qairt_Status_t QairtTensor_Properties_getIsOutput(
    QairtTensor_PropertiesHandle_t tensorPropertiesHandle, uint8_t* isOutput);

/**
 * @brief Set the native property of tensor properties.
 *
 * @param[in] tensorPropertiesHandle A handle to a created tensor properties.
 *
 * @param[in] isNative Flag indicating whether tensor is native (1) or not (0).
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Native property was successfully set.
 *         - QAIRT_TENSOR_ERROR_INVALID_HANDLE: _tensorPropertiesHandle_
 *           is not a valid handle.
 *
 * @note Use corresponding API through QairtTensor_PropertiesV1_t.
 */
Qairt_Status_t QairtTensor_Properties_setIsNative(
    QairtTensor_PropertiesHandle_t tensorPropertiesHandle, uint8_t isNative);

/**
 * @brief Get the native property of tensor properties.
 *
 * @param[in] tensorPropertiesHandle A handle to a created tensor properties.
 *
 * @param[out] isNative Flag indicating whether tensor is native (1) or not (0).
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Native property was successfully retrieved.
 *         - QAIRT_TENSOR_ERROR_INVALID_HANDLE: _tensorPropertiesHandle_
 *           is not a valid handle.
 *         - QAIRT_TENSOR_ERROR_INVALID_ARGUMENT: _isNative_ is NULL.
 *
 * @note Use corresponding API through QairtTensor_PropertiesV1_t.
 */
Qairt_Status_t QairtTensor_Properties_getIsNative(
    QairtTensor_PropertiesHandle_t tensorPropertiesHandle, uint8_t* isNative);

/**
 * @brief Set the null property of tensor properties.
 *
 * @param[in] tensorPropertiesHandle A handle to a created tensor properties.
 *
 * @param[in] isNull Flag indicating whether tensor is null (1) or not (0).
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Null property was successfully set.
 *         - QAIRT_TENSOR_ERROR_INVALID_HANDLE: _tensorPropertiesHandle_
 *           is not a valid handle.
 *
 * @note Use corresponding API through QairtTensor_PropertiesV1_t.
 */
Qairt_Status_t QairtTensor_Properties_setIsNull(
    QairtTensor_PropertiesHandle_t tensorPropertiesHandle, uint8_t isNull);

/**
 * @brief Get the null property of tensor properties.
 *
 * @param[in] tensorPropertiesHandle A handle to a created tensor properties.
 *
 * @param[out] isNull Flag indicating whether tensor is null (1) or not (0).
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Null property was successfully retrieved.
 *         - QAIRT_TENSOR_ERROR_INVALID_HANDLE: _tensorPropertiesHandle_
 *           is not a valid handle.
 *         - QAIRT_TENSOR_ERROR_INVALID_ARGUMENT: _isNull_ is NULL.
 *
 * @note Use corresponding API through QairtTensor_PropertiesV1_t.
 */
Qairt_Status_t QairtTensor_Properties_getIsNull(
    QairtTensor_PropertiesHandle_t tensorPropertiesHandle, uint8_t* isNull);

/**
 * @brief Set the optional property of tensor properties.
 *
 * @param[in] tensorPropertiesHandle A handle to a created tensor properties.
 *
 * @param[in] isOptional Flag indicating whether tensor is optional (1) or not
 *                       (0).
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Optional property was successfully set.
 *         - QAIRT_TENSOR_ERROR_INVALID_HANDLE: _tensorPropertiesHandle_
 *           is not a valid handle.
 *
 * @note Use corresponding API through QairtTensor_PropertiesV1_t.
 */
Qairt_Status_t QairtTensor_Properties_setIsOptional(
    QairtTensor_PropertiesHandle_t tensorPropertiesHandle, uint8_t isOptional);

/**
 * @brief Get the optional property of tensor properties.
 *
 * @param[in] tensorPropertiesHandle A handle to a created tensor properties.
 *
 * @param[out] isOptional Flag indicating whether tensor is optional (1) or not
 *                        (0).
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Optional property was successfully retrieved.
 *         - QAIRT_TENSOR_ERROR_INVALID_HANDLE: _tensorPropertiesHandle_
 *           is not a valid handle.
 *         - QAIRT_TENSOR_ERROR_INVALID_ARGUMENT: _isOptional_ is NULL.
 *
 * @note Use corresponding API through QairtTensor_PropertiesV1_t.
 */
Qairt_Status_t QairtTensor_Properties_getIsOptional(
    QairtTensor_PropertiesHandle_t tensorPropertiesHandle, uint8_t* isOptional);

/**
 * @brief Set the static property of tensor properties.
 *
 * @param[in] tensorPropertiesHandle A handle to a created tensor properties.
 *
 * @param[in] isStatic Flag indicating whether tensor is static (1) or not (0).
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Static property was successfully set.
 *         - QAIRT_TENSOR_ERROR_INVALID_HANDLE: _tensorPropertiesHandle_
 *           is not a valid handle.
 *
 * @note Use corresponding API through QairtTensor_PropertiesV1_t.
 */
Qairt_Status_t QairtTensor_Properties_setIsStatic(
    QairtTensor_PropertiesHandle_t tensorPropertiesHandle, uint8_t isStatic);

/**
 * @brief Get the static property of tensor properties.
 *
 * @param[in] tensorPropertiesHandle A handle to a created tensor properties.
 *
 * @param[out] isStatic Flag indicating whether tensor is static (1) or not (0).
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Static property was successfully retrieved.
 *         - QAIRT_TENSOR_ERROR_INVALID_HANDLE: _tensorPropertiesHandle_
 *           is not a valid handle.
 *         - QAIRT_TENSOR_ERROR_INVALID_ARGUMENT: _isStatic_ is NULL.
 *
 * @note Use corresponding API through QairtTensor_PropertiesV1_t.
 */
Qairt_Status_t QairtTensor_Properties_getIsStatic(
    QairtTensor_PropertiesHandle_t tensorPropertiesHandle, uint8_t* isStatic);

/**
 * @brief Set the updatable property of tensor properties.
 *
 * @param[in] tensorPropertiesHandle A handle to a created tensor properties.
 *
 * @param[in] isUpdatable Flag indicating whether tensor is updatable (1) or not
 *                        (0).
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Updatable property was successfully set.
 *         - QAIRT_TENSOR_ERROR_INVALID_HANDLE: _tensorPropertiesHandle_
 *           is not a valid handle.
 *
 * @note Use corresponding API through QairtTensor_PropertiesV1_t.
 */
Qairt_Status_t QairtTensor_Properties_setIsUpdatable(
    QairtTensor_PropertiesHandle_t tensorPropertiesHandle, uint8_t isUpdatable);

/**
 * @brief Get the updatable property of tensor properties.
 *
 * @param[in] tensorPropertiesHandle A handle to a created tensor properties.
 *
 * @param[out] isUpdatable Flag indicating whether tensor is updatable (1) or
 *                         not (0).
 * @return Error code:
 *         - QAIRT_SUCCESS: Updatable property was successfully retrieved.
 *         - QAIRT_TENSOR_ERROR_INVALID_HANDLE: _tensorPropertiesHandle_
 *           is not a valid handle.
 *         - QAIRT_TENSOR_ERROR_INVALID_ARGUMENT: _isUpdatable_ is NULL.
 *
 * @note Use corresponding API through QairtTensor_PropertiesV1_t.
 */
Qairt_Status_t QairtTensor_Properties_getIsUpdatable(
    QairtTensor_PropertiesHandle_t tensorPropertiesHandle, uint8_t* isUpdatable);

//=============================================================================
// Function Pointer Typedefs
//=============================================================================

/// Create/Free
typedef Qairt_Status_t (*QairtTensor_Properties_CreateFn_t)(QairtTensor_PropertiesHandle_t*);
typedef Qairt_Status_t (*QairtTensor_Properties_FreeFn_t)(QairtTensor_PropertiesHandle_t);

/// Input Tensor
typedef Qairt_Status_t (*QairtTensor_Properties_SetIsInputFn_t)(
    QairtTensor_PropertiesHandle_t, uint8_t);
typedef Qairt_Status_t (*QairtTensor_Properties_GetIsInputFn_t)(
    QairtTensor_PropertiesHandle_t, uint8_t*);

/// Output Tensor
typedef Qairt_Status_t (*QairtTensor_Properties_SetIsOutputFn_t)(
    QairtTensor_PropertiesHandle_t, uint8_t);
typedef Qairt_Status_t (*QairtTensor_Properties_GetIsOutputFn_t)(
    QairtTensor_PropertiesHandle_t, uint8_t*);

/// Native Tensor
typedef Qairt_Status_t (*QairtTensor_Properties_SetIsNativeFn_t)(
    QairtTensor_PropertiesHandle_t, uint8_t);
typedef Qairt_Status_t (*QairtTensor_Properties_GetIsNativeFn_t)(
    QairtTensor_PropertiesHandle_t, uint8_t*);

/// Null Tensor
typedef Qairt_Status_t (*QairtTensor_Properties_SetIsNullFn_t)(
    QairtTensor_PropertiesHandle_t, uint8_t);
typedef Qairt_Status_t (*QairtTensor_Properties_GetIsNullFn_t)(
    QairtTensor_PropertiesHandle_t, uint8_t*);

/// Static Tensor
typedef Qairt_Status_t (*QairtTensor_Properties_SetIsStaticFn_t)(
    QairtTensor_PropertiesHandle_t, uint8_t);
typedef Qairt_Status_t (*QairtTensor_Properties_GetIsStaticFn_t)(
    QairtTensor_PropertiesHandle_t, uint8_t*);

/// Optional Tensor
typedef Qairt_Status_t (*QairtTensor_Properties_SetIsOptionalFn_t)(
    QairtTensor_PropertiesHandle_t, uint8_t);
typedef Qairt_Status_t (*QairtTensor_Properties_GetIsOptionalFn_t)(
    QairtTensor_PropertiesHandle_t, uint8_t*);

/// Updatable Tensor
typedef Qairt_Status_t (*QairtTensor_Properties_SetIsUpdatableFn_t)(
    QairtTensor_PropertiesHandle_t, uint8_t);
typedef Qairt_Status_t (*QairtTensor_Properties_GetIsUpdatableFn_t)(
    QairtTensor_PropertiesHandle_t, uint8_t*);

//=============================================================================
// Public Interfaces
//=============================================================================
/**
 * @struct QairtTensor_PropertiesV1_t
 * @brief This struct contains the functions that define the V1 interface for
 *        QAIRT Tensor Properties. This interface can be retrieved through
 *        the id QAIRT_TENSOR_PROPERTIES_V1_ID.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtTensor_Properties_CreateFn_t create;
  QairtTensor_Properties_FreeFn_t free;
  QairtTensor_Properties_SetIsInputFn_t setIsInput;
  QairtTensor_Properties_GetIsInputFn_t getIsInput;
  QairtTensor_Properties_SetIsOutputFn_t setIsOutput;
  QairtTensor_Properties_GetIsOutputFn_t getIsOutput;
  QairtTensor_Properties_SetIsNativeFn_t setIsNative;
  QairtTensor_Properties_GetIsNativeFn_t getIsNative;
  QairtTensor_Properties_SetIsNullFn_t setIsNull;
  QairtTensor_Properties_GetIsNullFn_t getIsNull;
  QairtTensor_Properties_SetIsStaticFn_t setIsStatic;
  QairtTensor_Properties_GetIsStaticFn_t getIsStatic;
  QairtTensor_Properties_SetIsOptionalFn_t setIsOptional;
  QairtTensor_Properties_GetIsOptionalFn_t getIsOptional;
  QairtTensor_Properties_SetIsUpdatableFn_t setIsUpdatable;
  QairtTensor_Properties_GetIsUpdatableFn_t getIsUpdatable;
} QairtTensor_PropertiesV1_t;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // QAISW_QAIRTTENSORPROPERTIES_H

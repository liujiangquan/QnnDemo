//==============================================================================
//
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// All rights reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//==============================================================================

/**
 * @file
 * @brief   OpConfig component API
 *
 *          A header which contains the QAIRT OpConfig component and the
 *          interfaces it implements.
 */

#ifndef QAISW_QAIRTOPCONFIG_H
#define QAISW_QAIRTOPCONFIG_H

#include "QairtCommon/QairtScalar.h"
#include "QairtParam.h"
#include "QairtTensor/QairtTensor.h"

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
// Macros
//=============================================================================
#define QAIRT_OPCONFIG_V1_ID QAIRT_MIN_ID_OP_PACKAGE
#define QAIRT_SCALAR_V1_ID   QAIRT_MIN_ID_OP_PACKAGE + 1
#define QAIRT_PARAM_V1_ID    QAIRT_MIN_ID_OP_PACKAGE + 2

//=============================================================================
// Datatypes
//=============================================================================
/**
 * @brief QAIRT OpConfig API result / error codes.
 */
typedef enum {
  QAIRT_OPCONFIG_MIN_ERROR = QAIRT_MIN_ERROR_OP_PACKAGE,
  ////////////////////////////////////////////
  /// Qairt OpConfig success
  QAIRT_OPCONFIG_NO_ERROR = QAIRT_SUCCESS,
  /// General error relating to memory allocation in OpConfig API
  QAIRT_OPCONFIG_ERROR_MEM_ALLOC = QAIRT_COMMON_ERROR_MEM_ALLOC,
  /// Invalid function argument
  QAIRT_OPCONFIG_ERROR_INVALID_ARGUMENT = 3004,
  /// Invalid OpConfig handle
  QAIRT_OPCONFIG_ERROR_INVALID_HANDLE = 3005,
  ////////////////////////////////////////////
  QAIRT_OPCONFIG_MAX_ERROR = QAIRT_MAX_ERROR_OP_PACKAGE,
  // Unused, present to ensure 32 bits.
  QAIRT_OPCONFIG_ERROR_UNDEFINED = 0x7FFFFFFF
} QairtOpConfig_Error_t;

/**
 * @brief   OpConfig handle used to interface with OpConfig APIs
 *
 *          The handle itself is created and freed by the client.
 */
typedef struct _QairtOpConfig_Handle_t* QairtOpConfig_Handle_t;

//=============================================================================
// Public Functions
//=============================================================================
/**
 * @brief Create an OpConfig handle.
 *
 * @param[out] opConfigHandle A handle to the created OpConfig.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_OPCONFIG_ERROR_INVALID_ARGUMENT: _opConfigHandle_ is
 *           NULL
 *         - QAIRT_OPCONFIG_ERROR_MEM_ALLOC: Memory allocation error while
 *           creating OpConfig
 *
 * @note Use corresponding API through QairtOpConfig_V1_t.
 */
Qairt_Status_t QairtOpConfig_create(QairtOpConfig_Handle_t* opConfigHandle);

/**
 * @brief Free an OpConfig handle.
 *
 * @param[in] opConfigHandle A handle to the OpConfig to be freed.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_OPCONFIG_ERROR_INVALID_HANDLE: _opConfigHandle_ is not
 *           a valid handle
 *         - QAIRT_OPCONFIG_ERROR_MEM_ALLOC: Memory deallocation error
 *
 * @note Use corresponding API through QairtOpConfig_V1_t.
 */
Qairt_Status_t QairtOpConfig_free(QairtOpConfig_Handle_t opConfigHandle);

/**
 * @brief Set the name of an OpConfig.
 *
 * @param[in] opConfigHandle A handle to the OpConfig.
 *
 * @param[in] name The name to set for the OpConfig.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_OPCONFIG_ERROR_INVALID_HANDLE: _opConfigHandle_ is not
 *           a valid handle
 *         - QAIRT_OPCONFIG_ERROR_INVALID_ARGUMENT: _name_ is NULL
 *
 * @note Use corresponding API through QairtOpConfig_V1_t.
 */
Qairt_Status_t QairtOpConfig_setName(QairtOpConfig_Handle_t opConfigHandle,
                                          const char* name);

/**
 * @brief Get the name of an OpConfig.
 *
 * @param[in] opConfigHandle A handle to the OpConfig.
 *
 * @param[out] name The retrieved name.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_OPCONFIG_ERROR_INVALID_HANDLE: _opConfigHandle_ is not
 *           a valid handle
 *         - QAIRT_OPCONFIG_ERROR_INVALID_ARGUMENT: _name_ is NULL
 *
 * @note Use corresponding API through QairtOpConfig_V1_t.
 */
Qairt_Status_t QairtOpConfig_getName(QairtOpConfig_Handle_t opConfigHandle,
                                          const char** name);

/**
 * @brief Set the package name of an OpConfig.
 *
 * @param[in] opConfigHandle A handle to the OpConfig.
 *
 * @param[in] pkgName The package name to set for the OpConfig.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_OPCONFIG_ERROR_INVALID_HANDLE: _opConfigHandle_ is not
 *           a valid handle
 *         - QAIRT_OPCONFIG_ERROR_INVALID_ARGUMENT: _pkgName_ is NULL
 *
 * @note Use corresponding API through QairtOpConfig_V1_t.
 */
Qairt_Status_t QairtOpConfig_setPackageName(QairtOpConfig_Handle_t opConfigHandle,
                                                 const char* pkgName);

/**
 * @brief Get the package name of an OpConfig.
 *
 * @param[in] opConfigHandle A handle to the OpConfig.
 *
 * @param[out] pkgName The retrieved package name.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_OPCONFIG_ERROR_INVALID_HANDLE: _opConfigHandle_ is not
 *           a valid handle
 *         - QAIRT_OPCONFIG_ERROR_INVALID_ARGUMENT: _pkgName_ is NULL
 *
 * @note Use corresponding API through QairtOpConfig_V1_t.
 */
Qairt_Status_t QairtOpConfig_getPackageName(QairtOpConfig_Handle_t opConfigHandle,
                                                 const char** pkgName);

/**
 * @brief Set the type name of an OpConfig.
 *
 * @param[in] opConfigHandle A handle to the OpConfig.
 *
 * @param[in] typeName The type name to set for the OpConfig.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_OPCONFIG_ERROR_INVALID_HANDLE: _opConfigHandle_ is not
 *           a valid handle
 *         - QAIRT_OPCONFIG_ERROR_INVALID_ARGUMENT: _typeName_ is NULL
 *
 * @note Use corresponding API through QairtOpConfig_V1_t.
 */
Qairt_Status_t QairtOpConfig_setTypeName(QairtOpConfig_Handle_t opConfigHandle,
                                              const char* typeName);

/**
 * @brief Get the type name of an OpConfig.
 *
 * @param[in] opConfigHandle A handle to the OpConfig.
 *
 * @param[out] typeName The retrieved type name.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_OPCONFIG_ERROR_INVALID_HANDLE: _opConfigHandle_ is not
 *           a valid handle
 *         - QAIRT_OPCONFIG_ERROR_INVALID_ARGUMENT: _typeName_ is NULL
 *
 * @note Use corresponding API through QairtOpConfig_V1_t.
 */
Qairt_Status_t QairtOpConfig_getTypeName(QairtOpConfig_Handle_t opConfigHandle,
                                              const char** typeName);

/**
 * @brief Set the parameters of an OpConfig.
 * @param[in] opConfigHandle A handle to the OpConfig.
 *
 * @param[in] paramHandle Array of parameter handles to set.
 *
 * @param[in] numParams Number of parameters in the array.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_OPCONFIG_ERROR_INVALID_HANDLE: _opConfigHandle_ is not
 *           a valid handle
 *         - QAIRT_OPCONFIG_ERROR_INVALID_ARGUMENT: _paramHandle_ is NULL
 *           when _numParams_ > 0
 *         - QAIRT_OPCONFIG_ERROR_MEM_ALLOC: Memory allocation error while
 *           setting parameters
 *
 * @note Use corresponding API through QairtOpConfig_V1_t.
 */
Qairt_Status_t QairtOpConfig_setParams(QairtOpConfig_Handle_t opConfigHandle,
                                            QairtParam_Handle_t* paramHandle,
                                            const uint32_t numParams);

/**
 * @brief Get the parameter of an OpConfig at a specific index.
 *
 * @param[in] opConfigHandle A handle to the OpConfig.
 *
 * @param[in] index The index of the parameter to be retrieved
 *
 * @param[out] paramHandle The parameter handle.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_OPCONFIG_ERROR_INVALID_HANDLE: _opConfigHandle_ is not
 *           a valid handle
 *         - QAIRT_OPCONFIG_ERROR_INVALID_ARGUMENT: _paramHandle_ is NULL or _index_ is invalid
 *
 * @note Use corresponding API through QairtOpConfig_V1_t.
 */
Qairt_Status_t QairtOpConfig_getParamAt(QairtOpConfig_Handle_t opConfigHandle,
                                             uint32_t index,
                                             QairtParam_Handle_t* paramHandle);

/**
 * @brief Get the number of parameters in an OpConfig.
 *
 * @param[in] opConfigHandle A handle to the OpConfig.
 *
 * @param[out] numParams The number of parameters.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_OPCONFIG_ERROR_INVALID_HANDLE: _opConfigHandle_ is not
 *           a valid handle
 *         - QAIRT_OPCONFIG_ERROR_INVALID_ARGUMENT: _numParams_ is NULL
 *
 * @note Use corresponding API through QairtOpConfig_V1_t.
 */
Qairt_Status_t QairtOpConfig_getNumParams(QairtOpConfig_Handle_t opConfigHandle,
                                               uint32_t* numParams);

/**
 * @brief Set the input tensors of an OpConfig.
 *
 * @param[in] opConfigHandle A handle to the OpConfig.
 *
 * @param[in] tensorHandlesHandle Array of input tensor handles to set.
 *
 * @param[in] numTensors Number of input tensors in the array.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_OPCONFIG_ERROR_INVALID_HANDLE: _opConfigHandle_ is not
 *           a valid handle
 *         - QAIRT_OPCONFIG_ERROR_INVALID_ARGUMENT: _tensorHandlesHandle_
 *           is NULL when _numTensors_ > 0
 *         - QAIRT_OPCONFIG_ERROR_MEM_ALLOC: Memory allocation error while
 *           setting input tensors
 *
 * @note Use corresponding API through QairtOpConfig_V1_t.
 */
Qairt_Status_t QairtOpConfig_setInputTensors(QairtOpConfig_Handle_t opConfigHandle,
                                                  QairtTensor_Handle_t* tensorHandles,
                                                  const uint32_t numTensors);

/**
 * @brief Get the input tensor of an OpConfig at a specific index.
 *
 * @param[in] opConfigHandle A handle to the OpConfig.
 *
 * @param[in] index The index of the tensor to be retrieved
 *
 * @param[out] tensorHandle The input tensor handle.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_OPCONFIG_ERROR_INVALID_HANDLE: _opConfigHandle_ is not
 *           a valid handle
 *         - QAIRT_OPCONFIG_ERROR_INVALID_ARGUMENT: _tensorHandle_ is NULL or _index_ is invalid
 *
 * @note Use corresponding API through QairtOpConfig_V1_t.
 */
Qairt_Status_t QairtOpConfig_getInputTensorAt(QairtOpConfig_Handle_t opConfigHandle,
                                                   uint32_t index,
                                                   QairtTensor_Handle_t* tensorHandle);

/**
 * @brief Get the number of input tensors in an OpConfig.
 *
 * @param[in] opConfigHandle A handle to the OpConfig.
 *
 * @param[out] numTensors The number of input tensors.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_OPCONFIG_ERROR_INVALID_HANDLE: _opConfigHandle_ is not
 *           a valid handle
 *         - QAIRT_OPCONFIG_ERROR_INVALID_ARGUMENT: _numTensors_ is NULL
 *
 * @note Use corresponding API through QairtOpConfig_V1_t.
 */
Qairt_Status_t QairtOpConfig_getNumInputTensors(QairtOpConfig_Handle_t opConfigHandle,
                                                     uint32_t* numTensors);

/**
 * @brief Set the output tensors of an OpConfig.
 *
 * @param[in] opConfigHandle A handle to the OpConfig.
 *
 * @param[in] tensorHandlesHandle Array of output tensor handles to set.
 *
 * @param[in] numTensors Number of output tensors in the array.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_OPCONFIG_ERROR_INVALID_HANDLE: _opConfigHandle_ is not
 *           a valid handle
 *         - QAIRT_OPCONFIG_ERROR_INVALID_ARGUMENT: _tensorHandles_ is
 *           NULL when _numTensors_ > 0
 *         - QAIRT_OPCONFIG_ERROR_MEM_ALLOC: Memory allocation error while
 *           setting output tensors
 *
 * @note Use corresponding API through QairtOpConfig_V1_t.
 */
Qairt_Status_t QairtOpConfig_setOutputTensors(QairtOpConfig_Handle_t opConfigHandle,
                                                   QairtTensor_Handle_t* tensorHandles,
                                                   const uint32_t numTensors);

/**
 * @brief Get the number of output tensors in an OpConfig.
 *
 * @param[in] opConfigHandle A handle to the OpConfig.
 *
 * @param[out] numTensors The number of output tensors.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_OPCONFIG_ERROR_INVALID_HANDLE: _opConfigHandle_ is not a valid handle
 *         - QAIRT_OPCONFIG_ERROR_INVALID_ARGUMENT: _numTensors_ is NULL
 *
 * @note Use corresponding API through QairtOpConfig_V1_t.
 */
Qairt_Status_t QairtOpConfig_getNumOutputTensors(QairtOpConfig_Handle_t opConfigHandle,
                                                      uint32_t* numTensors);

/**
 * @brief Get the output tensor of an OpConfig at a specific index.
 *
 * @param[in] opConfigHandle A handle to the OpConfig.
 *
 * @param[in] index The index of the tensor to be retrieved
 *
 * @param[out] tensorHandle The output tensor handles.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_OPCONFIG_ERROR_INVALID_HANDLE: _opConfigHandle_ is not
 *           a valid handle
 *         - QAIRT_OPCONFIG_ERROR_INVALID_ARGUMENT: _tensorHandles_ is NULL or _index_ is invalid
 *
 * @note Use corresponding API through QairtOpConfig_V1_t.
 */
Qairt_Status_t QairtOpConfig_getOutputTensorAt(QairtOpConfig_Handle_t opConfigHandle,
                                                    uint32_t index,
                                                    QairtTensor_Handle_t* tensorHandle);

typedef Qairt_Status_t (*QairtOpConfig_CreateFn_t)(QairtOpConfig_Handle_t*);
typedef Qairt_Status_t (*QairtOpConfig_FreeFn_t)(QairtOpConfig_Handle_t);

typedef Qairt_Status_t (*QairtOpConfig_SetNameFn_t)(QairtOpConfig_Handle_t, const char*);
typedef Qairt_Status_t (*QairtOpConfig_GetNameFn_t)(QairtOpConfig_Handle_t, const char**);

typedef Qairt_Status_t (*QairtOpConfig_SetPackageNameFn_t)(QairtOpConfig_Handle_t,
                                                                const char*);
typedef Qairt_Status_t (*QairtOpConfig_GetPackageNameFn_t)(QairtOpConfig_Handle_t,
                                                                const char**);

typedef Qairt_Status_t (*QairtOpConfig_SetTypeNameFn_t)(QairtOpConfig_Handle_t, const char*);
typedef Qairt_Status_t (*QairtOpConfig_GetTypeNameFn_t)(QairtOpConfig_Handle_t,
                                                             const char**);

typedef Qairt_Status_t (*QairtOpConfig_SetParamsFn_t)(QairtOpConfig_Handle_t,
                                                           QairtParam_Handle_t*,
                                                           const uint32_t);
typedef Qairt_Status_t (*QairtOpConfig_GetParamAtFn_t)(QairtOpConfig_Handle_t,
                                                            uint32_t,
                                                            QairtParam_Handle_t*);
typedef Qairt_Status_t (*QairtOpConfig_GetNumParamsFn_t)(QairtOpConfig_Handle_t, uint32_t*);

typedef Qairt_Status_t (*QairtOpConfig_SetInputTensorsFn_t)(QairtOpConfig_Handle_t,
                                                                 QairtTensor_Handle_t*,
                                                                 const uint32_t);
typedef Qairt_Status_t (*QairtOpConfig_GetInputTensorAtFn_t)(QairtOpConfig_Handle_t,
                                                                  uint32_t,
                                                                  QairtTensor_Handle_t*);
typedef Qairt_Status_t (*QairtOpConfig_GetNumInputTensorsFn_t)(QairtOpConfig_Handle_t,
                                                                    uint32_t*);

typedef Qairt_Status_t (*QairtOpConfig_SetOutputTensorsFn_t)(QairtOpConfig_Handle_t,
                                                                  QairtTensor_Handle_t*,
                                                                  const uint32_t);
typedef Qairt_Status_t (*QairtOpConfig_GetNumOutputTensorsFn_t)(QairtOpConfig_Handle_t,
                                                                     uint32_t*);
typedef Qairt_Status_t (*QairtOpConfig_GetOutputTensorAtFn_t)(QairtOpConfig_Handle_t,
                                                                   uint32_t,
                                                                   QairtTensor_Handle_t*);

//=============================================================================
// Public Interfaces
//=============================================================================
/**
 * @struct QairtOpConfig_V1_t
 * @brief This struct contains the functions that define the V1 interface for
 *        QAIRT OpConfig. This interface can be retrieved through the id
 *        QAIRT_OPCONFIG_V1_ID.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtOpConfig_CreateFn_t create;
  QairtOpConfig_FreeFn_t free;
  QairtOpConfig_SetNameFn_t setName;
  QairtOpConfig_GetNameFn_t getName;
  QairtOpConfig_SetPackageNameFn_t setPackageName;
  QairtOpConfig_GetPackageNameFn_t getPackageName;
  QairtOpConfig_SetTypeNameFn_t setTypeName;
  QairtOpConfig_GetTypeNameFn_t getTypeName;
  QairtOpConfig_SetParamsFn_t setParams;
  QairtOpConfig_GetNumParamsFn_t getNumParams;
  QairtOpConfig_GetParamAtFn_t getParamAt;
  QairtOpConfig_SetInputTensorsFn_t setInputTensors;
  QairtOpConfig_GetNumInputTensorsFn_t getNumInputTensors;
  QairtOpConfig_GetInputTensorAtFn_t getInputTensorAt;
  QairtOpConfig_SetOutputTensorsFn_t setOutputTensors;
  QairtOpConfig_GetNumOutputTensorsFn_t getNumOutputTensors;
  QairtOpConfig_GetOutputTensorAtFn_t getOutputTensorAt;
} QairtOpConfig_V1_t;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // QAISW_QAIRTOPCONFIG_H

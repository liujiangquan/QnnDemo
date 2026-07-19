//==============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//==============================================================================

/**
 * @file
 * @brief   Param component API
 *
 *          A header which contains the QAIRT Param component and the interfaces
 *          it implements.
 */

#ifndef QAISW_QAIRTPARAM_H
#define QAISW_QAIRTPARAM_H

#include "QairtCommon/QairtScalar.h"
#include "QairtTensor/QairtTensor.h"

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
// Datatypes
//=============================================================================
/**
 * @brief   Param handle used to interface with Param APIs
 *
 *          The handle itself is created and freed by the client.
 */
typedef struct _QairtParam_Handle_t* QairtParam_Handle_t;

/**
 * @brief An enum to specify the parameter type : Scalar or Tensor
 */
typedef enum {
  QAIRT_PARAMTYPE_SCALAR = 0,
  QAIRT_PARAMTYPE_TENSOR = 1,
  // Unused, present to ensure 32 bits.
  QAIRT_PARAMTYPE_UNDEFINED = 0xFFFFFFFF
} Qairt_ParamType_t;

//=============================================================================
// Public Functions
//=============================================================================
/**
 * @brief Create a parameter handle.
 *
 * @param[out] paramHandle A handle to the created parameter.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_OPCONFIG_ERROR_INVALID_ARGUMENT: _paramHandle_ is NULL
 *         - QAIRT_OPCONFIG_ERROR_MEM_ALLOC: Memory allocation error while
 *           creating parameter
 *
 * @note Use corresponding API through QairtOpConfig_ParamV1_t.
 */
Qairt_Status_t QairtOpConfig_Param_create(QairtParam_Handle_t* paramHandle);

/**
 * @brief Free a parameter handle.
 *
 * @param[in] paramHandle A handle to the parameter to be freed.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_OPCONFIG_ERROR_INVALID_HANDLE: _paramHandle_ is not a
 *           valid handle
 *         - QAIRT_OPCONFIG_ERROR_MEM_ALLOC: Memory deallocation error
 *
 * @note Use corresponding API through QairtOpConfig_ParamV1_t.
 */
Qairt_Status_t QairtOpConfig_Param_free(QairtParam_Handle_t paramHandle);

/**
 * @brief Get the parameter type of a parameter.
 *
 * @param[in] paramHandle A handle to the parameter.
 * @param[out] paramType The parameter type (scalar or tensor).
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_OPCONFIG_ERROR_INVALID_HANDLE: _paramHandle_ is not a
 *           valid handle
 *         - QAIRT_OPCONFIG_ERROR_INVALID_ARGUMENT: _paramType_ is NULL
 *
 * @note Use corresponding API through QairtOpConfig_ParamV1_t.
 */
Qairt_Status_t QairtOpConfig_Param_getParamType(QairtParam_Handle_t paramHandle,
                                                     Qairt_ParamType_t* paramType);

/**
 * @brief Get the name of a parameter.
 *
 * @param[in] paramHandle A handle to the parameter.
 *
 * @param[out] name The retrieved parameter name.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_OPCONFIG_ERROR_INVALID_HANDLE: _paramHandle_ is not a
 *           valid handle
 *         - QAIRT_OPCONFIG_ERROR_INVALID_ARGUMENT: _name_ is NULL
 *
 * @note Use corresponding API through QairtOpConfig_ParamV1_t.
 */
Qairt_Status_t QairtOpConfig_Param_getName(QairtParam_Handle_t paramHandle,
                                                const char** name);

/**
 * @brief Get the scalar parameter from a parameter handle.
 *
 * @param[in] paramHandle A handle to the parameter.
 *
 * @param[out] scalarHandle The retrieved scalar handle.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_OPCONFIG_ERROR_INVALID_HANDLE: _paramHandle_ is not a
 *           valid handle
 *         - QAIRT_OPCONFIG_ERROR_INVALID_ARGUMENT: _scalarHandle_ is NULL
 *           or parameter is not scalar type
 *
 * @note Use corresponding API through QairtOpConfig_ParamV1_t.
 */
Qairt_Status_t QairtOpConfig_Param_getScalarParam(QairtParam_Handle_t paramHandle,
                                                       QairtScalar_Handle_t* scalarHandle);

/**
 * @brief Get the tensor parameter from a parameter handle.
 *
 * @param[in] paramHandle A handle to the parameter.
 *
 * @param[out] tensorHandle The retrieved tensor handle.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_OPCONFIG_ERROR_INVALID_HANDLE: _paramHandle_ is not a
 *           valid handle
 *         - QAIRT_OPCONFIG_ERROR_INVALID_ARGUMENT: _tensorHandle_ is NULL
 *           or parameter is not tensor type
 *
 * @note Use corresponding API through QairtOpConfig_ParamV1_t.
 */
Qairt_Status_t QairtOpConfig_Param_getTensorParam(QairtParam_Handle_t paramHandle,
                                                       QairtTensor_Handle_t* tensorHandle);

/**
 * @brief Set the name of a parameter.
 *
 * @param[in] paramHandle A handle to the parameter.
 *
 * @param[in] name The name to set for the parameter.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_OPCONFIG_ERROR_INVALID_HANDLE: _paramHandle_ is not a
 *           valid handle
 *         - QAIRT_OPCONFIG_ERROR_INVALID_ARGUMENT: _name_ is NULL
 *
 * @note Use corresponding API through QairtOpConfig_ParamV1_t.
 */
Qairt_Status_t QairtOpConfig_Param_setName(QairtParam_Handle_t paramHandle,
                                                const char* name);

/**
 * @brief Set a scalar parameter.
 *
 * @param[in] paramHandle A handle to the parameter.
 *
 * @param[in] scalarHandle The scalar handle to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_OPCONFIG_ERROR_INVALID_HANDLE: _paramHandle_ or
 *           _scalarHandle_ is not a valid handle
 *
 * @note Use corresponding API through QairtOpConfig_ParamV1_t.
 */
Qairt_Status_t QairtOpConfig_Param_setScalarParam(QairtParam_Handle_t paramHandle,
                                                       QairtScalar_Handle_t scalarHandle);

/**
 * @brief Set a tensor parameter.
 *
 * @param[in] paramHandle A handle to the parameter.
 *
 * @param[in] tensorHandle The tensor handle to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_OPCONFIG_ERROR_INVALID_HANDLE: _paramHandle_ or
 *           _tensorHandle_ is not a valid handle
 *
 * @note Use corresponding API through QairtOpConfig_ParamV1_t.
 */
Qairt_Status_t QairtOpConfig_Param_setTensorParam(QairtParam_Handle_t paramHandle,
                                                       QairtTensor_Handle_t tensorHandle);

typedef Qairt_Status_t (*QairtOpConfig_Param_CreateFn_t)(QairtParam_Handle_t*);
typedef Qairt_Status_t (*QairtOpConfig_Param_FreeFn_t)(QairtParam_Handle_t);

typedef Qairt_Status_t (*QairtOpConfig_Param_GetParamTypeFn_t)(QairtParam_Handle_t,
                                                                    Qairt_ParamType_t*);
typedef Qairt_Status_t (*QairtOpConfig_Param_GetNameFn_t)(QairtParam_Handle_t, const char**);
typedef Qairt_Status_t (*QairtOpConfig_Param_GetScalarParamFn_t)(QairtParam_Handle_t,
                                                                      QairtScalar_Handle_t*);
typedef Qairt_Status_t (*QairtOpConfig_Param_GetTensorParamFn_t)(QairtParam_Handle_t,
                                                                      QairtTensor_Handle_t*);

typedef Qairt_Status_t (*QairtOpConfig_Param_SetNameFn_t)(QairtParam_Handle_t, const char*);
typedef Qairt_Status_t (*QairtOpConfig_Param_SetScalarParamFn_t)(QairtParam_Handle_t,
                                                                      QairtScalar_Handle_t);
typedef Qairt_Status_t (*QairtOpConfig_Param_SetTensorParamFn_t)(QairtParam_Handle_t,
                                                                      QairtTensor_Handle_t);

//=============================================================================
// Public Interfaces
//=============================================================================
/**
 * @struct QairtOpConfig_ParamV1_t
 * @brief This struct contains the functions that define the V1 interface for
 *        QAIRT OpConfig Param. This interface can be retrieved through the
 *        id QAIRT_PARAM_V1_ID.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtOpConfig_Param_CreateFn_t create;
  QairtOpConfig_Param_FreeFn_t free;
  QairtOpConfig_Param_GetParamTypeFn_t getParamType;
  QairtOpConfig_Param_GetNameFn_t getName;
  QairtOpConfig_Param_GetScalarParamFn_t getScalarParam;
  QairtOpConfig_Param_GetTensorParamFn_t getTensorParam;
  QairtOpConfig_Param_SetNameFn_t setName;
  QairtOpConfig_Param_SetScalarParamFn_t setScalarParam;
  QairtOpConfig_Param_SetTensorParamFn_t setTensorParam;
} QairtOpConfig_ParamV1_t;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // QAISW_QAIRTPARAM_H

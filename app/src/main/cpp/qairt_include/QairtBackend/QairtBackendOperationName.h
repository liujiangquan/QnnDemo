//==============================================================================
//
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// All rights reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//==============================================================================

/**
 * @file
 * @brief   Backend OperationName component API
 *
 *          A header which contains the QAIRT Backend OperationName component
 *          and the interfaces it implements.
 */

#ifndef QAISW_QAIRTBACKENDOPERATIONNAME_H
#define QAISW_QAIRTBACKENDOPERATIONNAME_H

#include "QairtCommon/QairtCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
// Datatypes
//=============================================================================
/**
 * @brief   Backend OperationName handle used to interface with Backend
 *          OperationName APIs
 *
 * The handle itself is created and freed by the client.
 */
typedef struct _QairtBackend_OperationNameHandle_t* QairtBackend_OperationNameHandle_t;

//=============================================================================
// Public Functions
//=============================================================================
/**
 * @brief   Gets a package name associated with a Backend Operation Name Handle.
 *
 * @param[in] beOpNameHandle A Backend Operation Name Handle.
 *
 * @param[out] packageName A package name associated with the Backend Operation
 *                         Name handle. Describes the op package to which the
 *                         operation belongs.
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_BACKEND_ERROR_INVALID_HANDLE: _beOpNameHandle_ is not a
 *           valid handle
 *         - QAIRT_BACKEND_ERROR_INVALID_ARGUMENT: _packageName_ is NULL
 *
 * @note Use corresponding API through QairtBackend_OperationNameV1_t.
 */
Qairt_Status_t QairtBackend_OperationName_getPackageName(
    QairtBackend_OperationNameHandle_t beOpNameHandle, const char** packageName);

/**
 * @brief   Gets an operation name associated with a Backend Operation Name
 *          Handle.
 *
 * @param[in] beOpNameHandle A Backend Operation Name Handle.
 *
 * @param[out] name A name associated with the Backend Operation Name handle.
 *                  Describes the operation name within the op package.
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_BACKEND_ERROR_INVALID_HANDLE: _beOpNameHandle_ is not a
 *           valid handle
 *         - QAIRT_BACKEND_ERROR_INVALID_ARGUMENT: _name_ is NULL
 *
 * @note Use corresponding API through QairtBackend_OperationNameV1_t.
 */
Qairt_Status_t QairtBackend_OperationName_getName(
    QairtBackend_OperationNameHandle_t beOpNameHandle, const char** name);

/**
 * @brief   Gets the intended target platform for the combination of domain and
 *          operation name associated with a Backend Operation Name Handle.
 *
 * @param[in] beOpNameHandle A Backend Operation Name Handle.
 *
 * @param[out] target The intended target platform. May be unused (NULL) by some
 *                    backends.
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_BACKEND_ERROR_INVALID_HANDLE: _beOpNameHandle_ is not a
 *           valid handle
 *         - QAIRT_BACKEND_ERROR_INVALID_ARGUMENT: _target_ is NULL
 *
 * @note Use corresponding API through QairtBackend_OperationNameV1_t.
 */
Qairt_Status_t QairtBackend_OperationName_getTarget(
    QairtBackend_OperationNameHandle_t beOpNameHandle, const char** target);

typedef Qairt_Status_t (*QairtBackend_OperationName_GetPackageNameFn_t)(
    QairtBackend_OperationNameHandle_t, const char**);
typedef Qairt_Status_t (*QairtBackend_OperationName_GetNameFn_t)(
    QairtBackend_OperationNameHandle_t, const char**);
typedef Qairt_Status_t (*QairtBackend_OperationName_GetTargetFn_t)(
    QairtBackend_OperationNameHandle_t, const char**);

//=============================================================================
// Public Interfaces
//=============================================================================
/**
 * @struct QairtBackend_OperationNameV1_t
 * @brief This struct contains the functions that define the V1 interface for
 *        QAIRT Backend OperationName. This interface can be retrieved
 *        through the id QAIRT_BACKEND_OPERATION_NAME_V1_ID.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtBackend_OperationName_GetPackageNameFn_t getPackageName;
  QairtBackend_OperationName_GetNameFn_t getName;
  QairtBackend_OperationName_GetTargetFn_t getTarget;
} QairtBackend_OperationNameV1_t;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // QAISW_QAIRTBACKENDOPERATIONNAME_H

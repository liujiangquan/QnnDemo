//=============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//=============================================================================

#ifndef QAIRT_PROFILEBACKENDOPAQUEOBJECT_H
#define QAIRT_PROFILEBACKENDOPAQUEOBJECT_H

#include "QairtCommon/QairtCommon.h"
#include "QairtTensor/QairtTensorMemory.h"

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
// Data Types
//=============================================================================
typedef struct _QairtProfile_BackendOpaqueObjectHandle_t*
    QairtProfile_BackendOpaqueObjectHandle_t;

//=============================================================================
// Public Functions
//=============================================================================

/**
 * @brief Create a profile backend opqaue object
 *
 * @param[out] opaqueObjectHandle A handle to the created profile backend opqaue object.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Config was successfully created.
 *         - QAIRT_PROFILE_ERROR_INVALID_HANDLE: Config handle is invalid.
 *         - QAIRT_PROFILE_ERROR_MEM_ALLOC: Config creation failed due to memory/resource allocation
 *                                          issues.
 *
 * @note Use corresponding API through QairtProfile_BackendOpaqueObjectV1_t.
 */

Qairt_Status_t QairtProfile_BackendOpaqueObject_create(
    QairtProfile_BackendOpaqueObjectHandle_t* opaqueObjectHandle);

/**
 * @brief Free a profile backend opqaue object handle and associated resources.
 *
 * @param[in] opaqueObjectHandle A handle to a created profile backend opqaue object.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Profile was successfully freed.
 *         - QAIRT_PROFILE_ERROR_INVALID_HANDLE: _opaqueObjectHandle_ is not a valid handle.
 *         - QAIRT_PROFILE_ERROR_MEM_ALLOC: Memory deallocation error.
 *
 * @note Use corresponding API through QairtProfile_BackendOpaqueObjectV1_t.
 */
Qairt_Status_t QairtProfile_BackendOpaqueObject_free(
    QairtProfile_BackendOpaqueObjectHandle_t opaqueObjectHandle);

/**
 * @brief Set the opaque object related to the backend opaque object
 *
 * @param[in] opaqueObjectHandle A profile backend opaque object handle.
 *
 * @param[in] data Buffer containing the opaque object.
 *
 * @param[in] size Size of the buffer, in bytes, containting the opaque object.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_PROFILE_ERROR_INVALID_HANDLE: _opaqueObjectHandle_ or _bufferHandle_ is not a
 * valid handle
 *
 * @note Use corresponding API through QairtProfile_BackendOpaqueObjectV1_t.
 */
Qairt_Status_t QairtProfile_BackendOpaqueObject_setData(
    QairtProfile_BackendOpaqueObjectHandle_t opaqueObjectHandle, void* data, uint64_t size);

/**
 * @brief Get the underlying opaque object from the backend opaque object
 *
 * @param[in] opaqueObjectHandle A profile backend opaque object handle.
 *
 * @param[out] data Buffer containing the opaque object.
 *
 * @param[out] size Size of the buffer, in bytes, containting the opaque object.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_PROFILE_ERROR_INVALID_HANDLE: _opaqueObjectHandle_ is not a valid handle
 *         - QAIRT_PROFILE_ERROR_INVALID_ARGUMENT: _bufferHandle_ is NULL
 *
 * @note Use corresponding API through QairtProfile_BackendOpaqueObjectV1_t.
 */
Qairt_Status_t QairtProfile_BackendOpaqueObject_getData(
    QairtProfile_BackendOpaqueObjectHandle_t opaqueObjectHandle, void** data, uint64_t* size);

/**
 * @brief This config sets the maximum number of profiling events that can be stored in the profile
 *        handle.
 *
 * @param[in] opaqueObjectHandle A profile backend opaque object handle.
 *
 * @param[in] fileName The corresponding file name for the profile backend opaque object
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_PROFILE_ERROR_INVALID_HANDLE: _opaqueObjectHandle_ is not a valid handle
 *
 * @note Use corresponding API through QairtProfile_BackendOpaqueObjectV1_t.
 */
Qairt_Status_t QairtProfile_BackendOpaqueObject_setFileName(
    QairtProfile_BackendOpaqueObjectHandle_t opaqueObjectHandle, const char* fileName);

/**
 * @brief Get the maximum number of profiling events that can be stored in the profile handle.
 *
 * @param[in] opaqueObjectHandle A profile backend opaque object handle.
 *
 * @param[out] fileName The corresponding file name for the profile backend opaque object
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_PROFILE_ERROR_INVALID_HANDLE: _opaqueObjectHandle_ is not a valid handle
 *         - QAIRT_PROFILE_ERROR_INVALID_ARGUMENT: _maxEvents_ is NULL
 *
 * @note Use corresponding API through QairtProfile_BackendOpaqueObjectV1_t.
 */
Qairt_Status_t QairtProfile_BackendOpaqueObject_getFileName(
    QairtProfile_BackendOpaqueObjectHandle_t opaqueObjectHandle, const char** fileName);

typedef Qairt_Status_t (*QairtProfile_BackendOpaqueObject_CreateFn_t)(
    QairtProfile_BackendOpaqueObjectHandle_t*);

typedef Qairt_Status_t (*QairtProfile_BackendOpaqueObject_FreeFn_t)(
    QairtProfile_BackendOpaqueObjectHandle_t);

typedef Qairt_Status_t (*QairtProfile_BackendOpaqueObject_SetOpaqueObjectFn_t)(
    QairtProfile_BackendOpaqueObjectHandle_t, void*, uint64_t);

typedef Qairt_Status_t (*QairtProfile_BackendOpaqueObject_GetOpaqueObjectFn_t)(
    QairtProfile_BackendOpaqueObjectHandle_t, void**, uint64_t*);

typedef Qairt_Status_t (*QairtProfile_BackendOpaqueObject_SetFileNameFn_t)(
    QairtProfile_BackendOpaqueObjectHandle_t, const char*);

typedef Qairt_Status_t (*QairtProfile_BackendOpaqueObject_GetFileNameFn_t)(
    QairtProfile_BackendOpaqueObjectHandle_t, const char**);

//=============================================================================
// Public Interfaces
//=============================================================================

typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtProfile_BackendOpaqueObject_CreateFn_t create;
  QairtProfile_BackendOpaqueObject_FreeFn_t free;
  QairtProfile_BackendOpaqueObject_SetOpaqueObjectFn_t setData;
  QairtProfile_BackendOpaqueObject_GetOpaqueObjectFn_t getData;
  QairtProfile_BackendOpaqueObject_SetFileNameFn_t setFileName;
  QairtProfile_BackendOpaqueObject_GetFileNameFn_t getFileName;
} QairtProfile_BackendOpaqueObjectV1_t;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // QAIRT_PROFILEBACKENDOPAQUEOBJECT_H

//=============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//=============================================================================

#ifndef QAIRT_DSP_CONTEXT_H
#define QAIRT_DSP_CONTEXT_H

#ifdef __cplusplus
#include <cstddef>
#include <cstdint>
#else
#include <stddef.h>
#include <stdint.h>
#endif

#include "QairtContext/QairtContextConfig.h"

//=============================================================================
// Macros
//=============================================================================
#define QAIRT_MIN_ID_DSP_CONTEXT (QAIRT_MIN_ID_DSP + 100)

#define QAIRT_DSP_CONTEXT_GROUP_REGISTRATION_ID (QAIRT_MIN_ID_DSP_CONTEXT + 0)
#define QAIRT_DSP_CONTEXT_CUSTOM_CONFIG_ID      (QAIRT_MIN_ID_DSP_CONTEXT + 1)

//=============================================================================
// Data Types
//=============================================================================

typedef QairtContext_CustomConfigHandle_t QairtDspContext_CustomConfigHandle_t;
//=============================================================================
// Public Functions
//=============================================================================

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create a custom configuration handle for DSP context operations.
 *
 * Allocates and initializes a custom configuration object that can be used to
 * override default context settings for DSP programs.
 *
 * @param[out] customConfig A pointer to receive the created custom configuration handle.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Custom configuration handle was successfully created.
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: At least one argument is invalid (e.g.,
 * customConfig is NULL).
 *         - QAIRT_CONTEXT_ERROR_MEM_ALLOC: Memory allocation failed during creation.
 *         - QAIRT_CONTEXT_ERROR_UNSUPPORTED_FEATURE: The requested operation is not supported.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence (successful recovery).
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence (unsuccessful recovery).
 *
 * @note Use corresponding API through QairtDspContext_CustomConfigV1_t.
 */
Qairt_Status_t QairtDspContext_CustomConfig_create(
    QairtDspContext_CustomConfigHandle_t* customConfig);

/**
 * @brief Free a previously created custom configuration handle and associated resources.
 *
 * Releases all resources associated with the custom configuration handle. On success,
 * the handle pointed to by customConfig will be set to NULL.
 *
 * @param[in] customConfig The custom configuration handle to free.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Custom configuration handle was successfully freed.
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: At least one argument is invalid (e.g.,
 * customConfig is NULL).
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: The handle referenced by customConfig is not valid
 * or already freed.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence (successful recovery).
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence (unsuccessful recovery).
 *
 * @note Use corresponding API through QairtDspContext_CustomConfigV1_t.
 */
Qairt_Status_t QairtDspContext_CustomConfig_free(
    QairtDspContext_CustomConfigHandle_t customConfig);

/**
 * @brief Set the file read memory budget (in MB) for deserializing context binaries.
 *
 * Provides a hint to the backend to read and copy constant sections of the context
 * binary in chunks up to the specified number of megabytes, potentially reducing
 * peak host memory usage during deserialization.
 *
 * @param[in] customConfig Handle to the custom configuration object.
 * @param[in] fileReadMemoryBudgetInMb The read budget in megabytes. Implementations may
 *                                     interpret 0 as disabled and enforce upper bounds.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Budget was successfully set.
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: At least one argument is invalid.
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *         - QAIRT_CONTEXT_ERROR_UNSUPPORTED_FEATURE: Operation is not supported on the target.
 *         - QAIRT_CONTEXT_ERROR_INVALID_CONFIG: Provided budget value is out of allowed range.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence (successful recovery).
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence (unsuccessful recovery).
 *
 * @note Use corresponding API through QairtDspContext_CustomConfigV1_t.
 */
Qairt_Status_t QairtDspContext_CustomConfig_setFileReadMemoryBudget(
    QairtDspContext_CustomConfigHandle_t customConfig, uint32_t fileReadMemoryBudgetInMb);
/**
 * @brief Get the file read memory budget (in MB) used for context binary deserialization.
 *
 * Retrieves the currently configured read budget that guides chunked loading of
 * constant sections during deserialization.
 *
 * @param[in]  customConfig Handle to the custom configuration object.
 * @param[out] fileReadMemoryBudgetInMb Pointer to receive the budget in megabytes.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Budget was successfully retrieved.
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: At least one argument is invalid (e.g.,
 * fileReadMemoryBudgetInMb is NULL).
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *         - QAIRT_CONTEXT_ERROR_UNSUPPORTED_FEATURE: Operation is not supported on the target.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence (successful recovery).
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence (unsuccessful recovery).
 *
 * @note Use corresponding API through QairtDspContext_CustomConfigV1_t.
 */
Qairt_Status_t QairtDspContext_CustomConfig_getFileReadMemoryBudget(
    QairtDspContext_CustomConfigHandle_t customConfig, uint32_t* fileReadMemoryBudgetInMb);

typedef Qairt_Status_t (*QairtDspContext_CustomConfig_CreateFn_t)(
    QairtDspContext_CustomConfigHandle_t*);

typedef Qairt_Status_t (*QairtDspContext_CustomConfig_FreeFn_t)(
    QairtDspContext_CustomConfigHandle_t);

typedef Qairt_Status_t (*QairtDspContext_CustomConfig_SetFileReadMemoryBudgetFn_t)(
    QairtDspContext_CustomConfigHandle_t, uint32_t);

typedef Qairt_Status_t (*QairtDspContext_CustomConfig_GetFileReadMemoryBudgetFn_t)(
    QairtDspContext_CustomConfigHandle_t, uint32_t*);

//=============================================================================
// Public Interfaces
//=============================================================================

typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtDspContext_CustomConfig_CreateFn_t create;
  QairtDspContext_CustomConfig_FreeFn_t free;
  QairtDspContext_CustomConfig_SetFileReadMemoryBudgetFn_t setFileReadMemoryBudget;
  QairtDspContext_CustomConfig_GetFileReadMemoryBudgetFn_t getFileReadMemoryBudget;
} QairtDspContext_CustomConfigV1_t;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif
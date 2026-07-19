//=============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//=============================================================================

#ifndef QAIRT_HTP_CONTEXT_H
#define QAIRT_HTP_CONTEXT_H

#ifdef __cplusplus
#include <cstddef>
#include <cstdint>
#else
#include <stddef.h>
#include <stdint.h>
#endif

#include "QairtContext/QairtContext.h"
#include "QairtContext/QairtContextConfig.h"

//=============================================================================
// Macros
//=============================================================================
#define QAIRT_MIN_ID_HTP_CONTEXT (QAIRT_MIN_ID_HTP + 0)

#define QAIRT_HTP_CONTEXT_GROUP_REGISTRATION_ID (QAIRT_MIN_ID_HTP_CONTEXT + 0)
#define QAIRT_HTP_CONTEXT_CUSTOM_CONFIG_ID      (QAIRT_MIN_ID_HTP_CONTEXT + 1)

// Define ranges for HTP context-specific error codes
#define QAIRT_MIN_ERROR_HTP_CONTEXT (QAIRT_MIN_ERROR_HTP + 0)
#define QAIRT_MAX_ERROR_HTP_CONTEXT (QAIRT_MIN_ERROR_HTP + 999)

//=============================================================================
// Data Types
//=============================================================================
typedef struct _QairtHtpContext_GroupRegistrationHandle_t*
    QairtHtpContext_GroupRegistrationHandle_t;
typedef QairtContext_CustomConfigHandle_t QairtHtpContext_CustomConfigHandle_t;

// This enum is supported only with the QairtContext_createFromBinaryListAsync API, when
// shareResources is true; otherwise, it is ignored. This enumeration allows users to specify how
// graphs are going to be executed, providing QAIRT with hints for optimizing memory.
typedef enum {
  // Default value if no user input is provided.
  // This type is used for sequential graph execution, optimizing both VA and memory.
  QAIRT_HTP_CONTEXT_RESOURCE_OPTIMIZATION_SEQUENTIAL_WITH_VA_OPTIMIZATION =
      0,  // SEQUENTIAL_WITH_VA_OPTIMIZATION,
  // This type is used for sequential graph execution, optimizing memory.
  QAIRT_HTP_CONTEXT_RESOURCE_OPTIMIZATION_SEQUENTIAL_WITHOUT_VA_OPTIMIZATION =
      1,  // SEQUENTIAL_WITHOUT_VA_OPTIMIZATION,
  // This type is used for concurrent resource sharing, optimizing memory by sharing
  // resources across contexts with the same priority level.
  QAIRT_HTP_CONTEXT_RESOURCE_OPTIMIZATION_CONCURRENT_OPTIMIZATION = 2,  // CONCURRENT_OPTIMIZATION,
  QAIRT_HTP_CONTEXT_RESOURCE_OPTIMIZATION_UNDEFINED               = 0x7fffffff
} QairtHtpContext_ShareResourcesOptimizationType_t;

/**
 * @brief QAIRT HTP Context API result / error codes.
 */
typedef enum {
  QAIRT_HTP_CONTEXT_MIN_ERROR = QAIRT_MIN_ERROR_HTP_CONTEXT,
  ////////////////////////////////////////

  /// HTP context operation succeeded.
  QAIRT_HTP_CONTEXT_NO_ERROR = QAIRT_SUCCESS,
  /// General memory allocation failure.
  QAIRT_HTP_CONTEXT_ERROR_MEM_ALLOC = QAIRT_COMMON_ERROR_MEM_ALLOC,
  /// Feature or API is not supported.
  QAIRT_HTP_CONTEXT_ERROR_UNSUPPORTED_FEATURE = QAIRT_COMMON_ERROR_NOT_SUPPORTED,
  /// General error not covered by a more specific code.
  QAIRT_HTP_CONTEXT_ERROR_GENERAL = QAIRT_COMMON_ERROR_GENERAL,
  /// An argument to the API is invalid.
  QAIRT_HTP_CONTEXT_ERROR_INVALID_ARGUMENT = QAIRT_MIN_ERROR_HTP_CONTEXT + 0,
  /// A handle argument is not valid.
  QAIRT_HTP_CONTEXT_ERROR_INVALID_HANDLE = QAIRT_MIN_ERROR_HTP_CONTEXT + 1,
  /// A configuration value is invalid or incompatible.
  QAIRT_HTP_CONTEXT_ERROR_INVALID_CONFIG = QAIRT_MIN_ERROR_HTP_CONTEXT + 2,

  ////////////////////////////////////////
  QAIRT_HTP_CONTEXT_MAX_ERROR = QAIRT_MAX_ERROR_HTP_CONTEXT,
  // Unused, present to ensure 32 bits.
  QAIRT_HTP_CONTEXT_ERROR_UNDEFINED = 0x7FFFFFFF
} QairtHtpContext_Error_t;

//=============================================================================
// Public Functions
//=============================================================================

#ifdef __cplusplus
extern "C" {
#endif

/// HTP Group Registration
/**
 * @brief Create a group registration handle used to manage HTP program groups.
 *
 * @param[out] groupRegistration A handle to the created group registration object.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Group registration was successfully created.
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: At least one argument is invalid (e.g.,
 * groupRegistration is NULL).
 *         - QAIRT_CONTEXT_ERROR_MEM_ALLOC: Failure in allocating memory when creating the group
 * registration.
 *         - QAIRT_CONTEXT_ERROR_UNSUPPORTED_FEATURE: The optional group registration feature is not
 * supported.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence (successful recovery).
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence (unsuccessful recovery).
 *
 * @note Use corresponding API through QairtHtpContext_GroupRegistrationV1_t.
 */
Qairt_Status_t QairtHtpContext_GroupRegistration_create(
    QairtHtpContext_GroupRegistrationHandle_t* groupRegistration);
/**
 * @brief Free a previously created group registration handle and associated resources.
 *
 * @param[in] groupRegistration A handle to a group registration. Passing an invalid handle
 *                              results in an error.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Group registration was successfully freed.
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: At least one argument is invalid (e.g., NULL
 * handle).
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: groupRegistration is not a valid handle.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence (successful recovery).
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence (unsuccessful recovery).
 *
 * @note Use corresponding API through QairtHtpContext_GroupRegistrationV1_t.
 */
Qairt_Status_t QairtHtpContext_GroupRegistration_free(
    QairtHtpContext_GroupRegistrationHandle_t groupRegistration);

/**
 * @brief Set the first group handle for an HTP group registration.
 *
 * Associates the first group in the registration sequence with the provided context handle.
 *
 * @param[in] groupRegistration A handle to a group registration.
 *
 * @param[in] context A handle to a context to associate as the first group in the registration.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: First group handle was successfully set.
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: At least one argument is invalid.
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: groupRegistration or context is not a valid handle.
 *         - QAIRT_CONTEXT_ERROR_UNSUPPORTED_FEATURE: The requested operation is not supported.
 *         - QAIRT_CONTEXT_ERROR_INVALID_CONFIG: One or more configuration values are invalid.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence (successful recovery).
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence (unsuccessful recovery).
 *
 * @note Use corresponding API through QairtHtpContext_GroupRegistrationV1_t.
 */
Qairt_Status_t QairtHtpContext_GroupRegistration_setFirstGroupHandle(
    QairtHtpContext_GroupRegistrationHandle_t groupRegistration, QairtContext_Handle_t context);

/**
 * @brief Get the first group handle associated with an HTP group registration.
 *
 * Retrieves the context handle that was previously set as the first group for the specified
 * group registration.
 *
 * @param[in]  groupRegistration A handle to a group registration.
 *
 * @param[out] context A pointer to receive the context handle associated as the first group.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: First group handle was successfully retrieved.
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: At least one argument is invalid (e.g., context
 * is NULL).
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: groupRegistration is not a valid handle.
 *         - QAIRT_CONTEXT_ERROR_UNSUPPORTED_FEATURE: The requested operation is not supported.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence (successful recovery).
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence (unsuccessful recovery).
 *
 * @note Use corresponding API through QairtHtpContext_GroupRegistrationV1_t.
 */
Qairt_Status_t QairtHtpContext_GroupRegistration_getFirstGroupHandle(
    QairtHtpContext_GroupRegistrationHandle_t groupRegistration, QairtContext_Handle_t* context);

/**
 * @brief Set the maximum spill-fill buffer size for an HTP group registration.
 *
 * Configures the maximum spill-fill buffer size (in bytes) that may be required by
 * programs within the registered group.
 *
 * @param[in] groupRegistration A handle to a group registration.
 *
 * @param[in] spillFill The maximum spill-fill buffer size in bytes.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Value was successfully set.
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: At least one argument is invalid.
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: groupRegistration is not a valid handle.
 *         - QAIRT_CONTEXT_ERROR_UNSUPPORTED_FEATURE: The requested operation is not supported.
 *         - QAIRT_CONTEXT_ERROR_INVALID_CONFIG: The provided spillFill value is not valid.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence (successful recovery).
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence (unsuccessful recovery).
 *
 * @note Use corresponding API through QairtHtpContext_GroupRegistrationV1_t.
 */
Qairt_Status_t QairtHtpContext_GroupRegistration_setMaxSpillFill(
    QairtHtpContext_GroupRegistrationHandle_t groupRegistration, uint64_t spillFill);
/**
 * @brief Get the maximum spill-fill buffer size for an HTP group registration.
 *
 * Retrieves the currently configured maximum spill-fill buffer size (in bytes) for
 * the specified group registration.
 *
 * @param[in]  groupRegistration A handle to a group registration.
 *
 * @param[out] spillFill A pointer to receive the maximum spill-fill buffer size in bytes.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Value was successfully retrieved.
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: At least one argument is invalid (e.g., spillFill
 * is NULL).
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: groupRegistration is not a valid handle.
 *         - QAIRT_CONTEXT_ERROR_UNSUPPORTED_FEATURE: The requested operation is not supported.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence (successful recovery).
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence (unsuccessful recovery).
 *
 * @note Use corresponding API through QairtHtpContext_GroupRegistrationV1_t.
 */
Qairt_Status_t QairtHtpContext_GroupRegistration_getMaxSpillFill(
    QairtHtpContext_GroupRegistrationHandle_t groupRegistration, uint64_t* spillFill);

/// HTP Context Configuration

/**
 * @brief Create a custom configuration handle for HTP context operations.
 *
 * Allocates and initializes a custom configuration object that can be used to
 * override default context settings for HTP programs.
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
 * @note Use corresponding API through QairtHtpContext_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpContext_CustomConfig_create(
    QairtHtpContext_CustomConfigHandle_t* customConfig);

/**
 * @brief Free a previously created custom configuration handle and associated resources.
 *
 * Releases all resources associated with the custom configuration handle. On success,
 * the handle pointed to by customConfig will be set to NULL.
 *
 * @param[in,out] customConfig A pointer to the custom configuration handle to free.
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
 * @note Use corresponding API through QairtHtpContext_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpContext_CustomConfig_free(QairtHtpContext_CustomConfigHandle_t customConfig);

// Weight Sharing
/**
 * @brief Enable or disable weight sharing for the HTP context custom configuration.
 *
 * Configures whether weight sharing is enabled when preparing or executing graphs
 * within the context. When enabled, compatible graphs can share constant sections
 * (weights) to reduce memory footprint.
 *
 * @param[in] customConfig Handle to the custom configuration object.
 * @param[in] enableWeightSharing Non-zero to enable weight sharing; zero to disable.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Option was successfully set.
 *         - QAIRT_HTP_CONTEXT_ERROR_INVALID_ARGUMENT: At least one argument is invalid.
 *         - QAIRT_HTP_CONTEXT_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *         - QAIRT_HTP_CONTEXT_ERROR_UNSUPPORTED_FEATURE: Operation is not supported on the target.
 *         - QAIRT_HTP_CONTEXT_ERROR_INVALID_CONFIG: Provided value is not valid for this option.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence (successful recovery).
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence (unsuccessful recovery).
 *
 * @note Use corresponding API through QairtHtpContext_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpContext_CustomConfig_setEnableWeightSharing(
    QairtHtpContext_CustomConfigHandle_t customConfig, uint8_t enableWeightSharing);
/**
 * @brief Get the weight sharing enablement setting from the HTP context custom configuration.
 *
 * Retrieves whether weight sharing is currently enabled for the context configuration.
 *
 * @param[in]  customConfig Handle to the custom configuration object.
 * @param[out] enableWeightSharing Pointer to receive the non-zero/zero value indicating
 *                                 whether weight sharing is enabled.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Value was successfully retrieved.
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: At least one argument is invalid (e.g.,
 * enableWeightSharing is NULL).
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *         - QAIRT_CONTEXT_ERROR_UNSUPPORTED_FEATURE: Operation is not supported on the target.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence (successful recovery).
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence (unsuccessful recovery).
 *
 * @note Use corresponding API through QairtHtpContext_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpContext_CustomConfig_getEnableWeightSharing(
    QairtHtpContext_CustomConfigHandle_t customConfig, uint8_t* enableWeightSharing);

// Group Registration
/**
 * @brief Set the group registration to associate with this custom configuration.
 *
 * Associates a group registration handle used to register multiple contexts into
 * a group for shared spill-fill buffer management.
 *
 * @param[in] customConfig Handle to the custom configuration object.
 * @param[in] groupRegistration A valid group registration handle.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Handle was successfully set.
 *         - QAIRT_HTP_CONTEXT_ERROR_INVALID_ARGUMENT: At least one argument is invalid.
 *         - QAIRT_HTP_CONTEXT_ERROR_INVALID_HANDLE: customConfig or groupRegistration is not a
 * valid handle.
 *         - QAIRT_HTP_CONTEXT_ERROR_UNSUPPORTED_FEATURE: Operation is not supported on the target.
 *         - QAIRT_HTP_CONTEXT_ERROR_INVALID_CONFIG: Provided handle is incompatible with current
 * configuration.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence (successful recovery).
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence (unsuccessful recovery).
 *
 * @note Use corresponding API through QairtHtpContext_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpContext_CustomConfig_setGroupRegistration(
    QairtHtpContext_CustomConfigHandle_t customConfig,
    QairtHtpContext_GroupRegistrationHandle_t groupRegistration);
/**
 * @brief Get the group registration associated with this custom configuration.
 *
 * Retrieves the currently associated group registration handle, if any.
 *
 * @param[in]  customConfig Handle to the custom configuration object.
 * @param[out] groupRegistration Pointer to receive the group registration handle.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Handle was successfully retrieved.
 *         - QAIRT_HTP_CONTEXT_ERROR_INVALID_ARGUMENT: At least one argument is invalid (e.g.,
 * groupRegistration is NULL).
 *         - QAIRT_HTP_CONTEXT_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *         - QAIRT_HTP_CONTEXT_ERROR_UNSUPPORTED_FEATURE: Operation is not supported on the target.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence (successful recovery).
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence (unsuccessful recovery).
 *
 * @note Use corresponding API through QairtHtpContext_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpContext_CustomConfig_getGroupRegistration(
    QairtHtpContext_CustomConfigHandle_t customConfig,
    QairtHtpContext_GroupRegistrationHandle_t* groupRegistration);

// FileReadMemoryBudget
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
 * @note Use corresponding API through QairtHtpContext_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpContext_CustomConfig_setFileReadMemoryBudget(
    QairtHtpContext_CustomConfigHandle_t customConfig, uint64_t fileReadMemoryBudgetInMb);
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
 * @note Use corresponding API through QairtHtpContext_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpContext_CustomConfig_getFileReadMemoryBudget(
    QairtHtpContext_CustomConfigHandle_t customConfig, uint64_t* fileReadMemoryBudgetInMb);

// DSP Memory Profiling Enabled
/**
 * @brief Enable or disable DSP memory profiling.
 *
 * Controls whether DSP memory profiling is enabled for the session, which may
 * incur additional overhead to collect memory usage metrics.
 *
 * @param[in] customConfig Handle to the custom configuration object.
 * @param[in] enableDspMemoryProfiling Non-zero to enable profiling; zero to disable.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Option was successfully set.
 *         - QAIRT_HTP_CONTEXT_ERROR_INVALID_ARGUMENT: At least one argument is invalid.
 *         - QAIRT_HTP_CONTEXT_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *         - QAIRT_HTP_CONTEXT_ERROR_UNSUPPORTED_FEATURE: Operation is not supported on the target.
 *         - QAIRT_HTP_CONTEXT_ERROR_INVALID_CONFIG: Provided value is not valid for this option.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence (successful recovery).
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence (unsuccessful recovery).
 *
 * @note Use corresponding API through QairtHtpContext_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpContext_CustomConfig_setEnableDspMemoryProfiling(
    QairtHtpContext_CustomConfigHandle_t customConfig, uint8_t enableDspMemoryProfiling);
/**
 * @brief Get the DSP memory profiling enablement setting.
 *
 * Retrieves whether DSP memory profiling is currently enabled for the context configuration.
 *
 * @param[in]  customConfig Handle to the custom configuration object.
 * @param[out] enableDspMemoryProfiling Pointer to receive the non-zero/zero value indicating
 *                                      whether profiling is enabled.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Value was successfully retrieved.
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: At least one argument is invalid (e.g.,
 * enableDspMemoryProfiling is NULL).
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *         - QAIRT_CONTEXT_ERROR_UNSUPPORTED_FEATURE: Operation is not supported on the target.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence (successful recovery).
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence (unsuccessful recovery).
 *
 * @note Use corresponding API through QairtHtpContext_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpContext_CustomConfig_getEnableDspMemoryProfiling(
    QairtHtpContext_CustomConfigHandle_t customConfig, uint8_t* enableDspMemoryProfiling);

// Share Resources
/**
 * @brief Enable or disable resource sharing across eligible graphs/contexts.
 *
 * Configures whether resources (e.g., buffers) may be shared where supported to
 * optimize memory usage under certain execution models.
 *
 * @param[in] customConfig Handle to the custom configuration object.
 * @param[in] shareResources Non-zero to enable sharing; zero to disable.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Option was successfully set.
 *         - QAIRT_HTP_CONTEXT_ERROR_INVALID_ARGUMENT: At least one argument is invalid.
 *         - QAIRT_HTP_CONTEXT_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *         - QAIRT_HTP_CONTEXT_ERROR_UNSUPPORTED_FEATURE: Operation is not supported on the target.
 *         - QAIRT_HTP_CONTEXT_ERROR_INVALID_CONFIG: Provided value is not valid for this option.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence (successful recovery).
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence (unsuccessful recovery).
 *
 * @note Use corresponding API through QairtHtpContext_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpContext_CustomConfig_setShareResources(
    QairtHtpContext_CustomConfigHandle_t customConfig, uint8_t shareResources);
/**
 * @brief Get the resource sharing enablement setting.
 *
 * Retrieves whether resource sharing is currently enabled for the context configuration.
 *
 * @param[in]  customConfig Handle to the custom configuration object.
 * @param[out] shareResources Pointer to receive the non-zero/zero value indicating
 *                            whether sharing is enabled.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Value was successfully retrieved.
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: At least one argument is invalid (e.g.,
 * shareResources is NULL).
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *         - QAIRT_CONTEXT_ERROR_UNSUPPORTED_FEATURE: Operation is not supported on the target.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence (successful recovery).
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence (unsuccessful recovery).
 *
 * @note Use corresponding API through QairtHtpContext_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpContext_CustomConfig_getShareResources(
    QairtHtpContext_CustomConfigHandle_t customConfig, uint8_t* shareResources);

// IO Mem Estimation
/**
 * @brief Set the I/O memory estimation mode.
 *
 * Configures whether I/O memory estimation heuristics are enabled to help predict
 * memory requirements prior to execution.
 *
 * @param[in] customConfig Handle to the custom configuration object.
 * @param[in] ioMemEstimation Non-zero to enable estimation; zero to disable.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Option was successfully set.
 *         - QAIRT_HTP_CONTEXT_ERROR_INVALID_ARGUMENT: At least one argument is invalid.
 *         - QAIRT_HTP_CONTEXT_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *         - QAIRT_HTP_CONTEXT_ERROR_UNSUPPORTED_FEATURE: Operation is not supported on the target.
 *         - QAIRT_HTP_CONTEXT_ERROR_INVALID_CONFIG: Provided value is not valid for this option.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence (successful recovery).
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence (unsuccessful recovery).
 *
 * @note Use corresponding API through QairtHtpContext_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpContext_CustomConfig_setIoMemEstimation(
    QairtHtpContext_CustomConfigHandle_t customConfig, uint8_t ioMemEstimation);
/**
 * @brief Get the I/O memory estimation mode.
 *
 * Retrieves whether I/O memory estimation is currently enabled for the context configuration.
 *
 * @param[in]  customConfig Handle to the custom configuration object.
 * @param[out] ioMemEstimation Pointer to receive the non-zero/zero value indicating
 *                             whether estimation is enabled.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Value was successfully retrieved.
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: At least one argument is invalid (e.g.,
 * ioMemEstimation is NULL).
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *         - QAIRT_CONTEXT_ERROR_UNSUPPORTED_FEATURE: Operation is not supported on the target.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence (successful recovery).
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence (unsuccessful recovery).
 *
 * @note Use corresponding API through QairtHtpContext_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpContext_CustomConfig_getIoMemEstimation(
    QairtHtpContext_CustomConfigHandle_t customConfig, uint8_t* ioMemEstimation);

// Is Prepare Only
/**
 * @brief Set prepare-only mode for context operations.
 *
 * Configures whether the context should only perform preparation (without executing),
 * which may be useful for ahead-of-time preparation workflows.
 *
 * @param[in] customConfig Handle to the custom configuration object.
 * @param[in] isPrepareOnly Non-zero to enable prepare-only; zero to disable.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Option was successfully set.
 *         - QAIRT_HTP_CONTEXT_ERROR_INVALID_ARGUMENT: At least one argument is invalid.
 *         - QAIRT_HTP_CONTEXT_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *         - QAIRT_HTP_CONTEXT_ERROR_UNSUPPORTED_FEATURE: Operation is not supported on the target.
 *         - QAIRT_HTP_CONTEXT_ERROR_INVALID_CONFIG: Provided value is not valid for this option.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence (successful recovery).
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence (unsuccessful recovery).
 *
 * @note Use corresponding API through QairtHtpContext_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpContext_CustomConfig_setIsPrepareOnly(
    QairtHtpContext_CustomConfigHandle_t customConfig, uint8_t isPrepareOnly);
/**
 * @brief Get prepare-only mode for context operations.
 *
 * Retrieves whether prepare-only mode is currently enabled for the context configuration.
 *
 * @param[in]  customConfig Handle to the custom configuration object.
 * @param[out] isPrepareOnly Pointer to receive the non-zero/zero value indicating
 *                           whether prepare-only is enabled.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Value was successfully retrieved.
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: At least one argument is invalid (e.g.,
 * isPrepareOnly is NULL).
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *         - QAIRT_CONTEXT_ERROR_UNSUPPORTED_FEATURE: Operation is not supported on the target.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence (successful recovery).
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence (unsuccessful recovery).
 *
 * @note Use corresponding API through QairtHtpContext_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpContext_CustomConfig_getIsPrepareOnly(
    QairtHtpContext_CustomConfigHandle_t customConfig, uint8_t* isPrepareOnly);

// Init Acceleration
/**
 * @brief Enable or disable initialization acceleration.
 *
 * Controls whether an optimization path is used during initialization to accelerate
 * setup at potential trade-offs depending on backend support.
 *
 * @param[in] customConfig Handle to the custom configuration object.
 * @param[in] initAcceleration Non-zero to enable; zero to disable.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Option was successfully set.
 *         - QAIRT_HTP_CONTEXT_ERROR_INVALID_ARGUMENT: At least one argument is invalid.
 *         - QAIRT_HTP_CONTEXT_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *         - QAIRT_HTP_CONTEXT_ERROR_UNSUPPORTED_FEATURE: Operation is not supported on the target.
 *         - QAIRT_HTP_CONTEXT_ERROR_INVALID_CONFIG: Provided value is not valid for this option.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence (successful recovery).
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence (unsuccessful recovery).
 *
 * @note Use corresponding API through QairtHtpContext_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpContext_CustomConfig_setInitAcceleration(
    QairtHtpContext_CustomConfigHandle_t customConfig, uint8_t initAcceleration);
/**
 * @brief Get the initialization acceleration setting.
 *
 * Retrieves whether initialization acceleration is currently enabled.
 *
 * @param[in]  customConfig Handle to the custom configuration object.
 * @param[out] initAcceleration Pointer to receive the non-zero/zero value indicating
 *                              whether the feature is enabled.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Value was successfully retrieved.
 *         - QAIRT_HTP_CONTEXT_ERROR_INVALID_ARGUMENT: At least one argument is invalid (e.g.,
 * initAcceleration is NULL).
 *         - QAIRT_HTP_CONTEXT_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *         - QAIRT_HTP_CONTEXT_ERROR_UNSUPPORTED_FEATURE: Operation is not supported on the target.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence (successful recovery).
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence (unsuccessful recovery).
 *
 * @note The function name indicates "set", but the signature returns a value via pointer.
 *       This API retrieves the current setting.
 * @note Use corresponding API through QairtHtpContext_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpContext_CustomConfig_getInitAcceleration(
    QairtHtpContext_CustomConfigHandle_t customConfig, uint8_t* initAcceleration);

// Skip Binary Section Validation
/**
 * @brief Skip validation on binary sections during context operations.
 *
 * Configures whether certain binary section validation steps should be skipped.
 * Disabling validation may reduce initialization time but should be used with caution.
 *
 * @param[in] customConfig Handle to the custom configuration object.
 * @param[in] skipValidationOnBinarySection Non-zero to skip validation; zero to perform validation.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Option was successfully set.
 *         - QAIRT_HTP_CONTEXT_ERROR_INVALID_ARGUMENT: At least one argument is invalid.
 *         - QAIRT_HTP_CONTEXT_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *         - QAIRT_HTP_CONTEXT_ERROR_UNSUPPORTED_FEATURE: Operation is not supported on the target.
 *         - QAIRT_HTP_CONTEXT_ERROR_INVALID_CONFIG: Provided value is not valid for this option.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence (successful recovery).
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence (unsuccessful recovery).
 *
 * @note Use corresponding API through QairtHtpContext_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpContext_CustomConfig_setSkipBinarySectionValidation(
    QairtHtpContext_CustomConfigHandle_t customConfig, uint8_t skipValidationOnBinarySection);
/**
 * @brief Get the binary section validation skip setting.
 *
 * Retrieves whether binary section validation is currently being skipped.
 *
 * @param[in]  customConfig Handle to the custom configuration object.
 * @param[out] skipValidationOnBinarySection Pointer to receive the non-zero/zero value indicating
 *                                           whether validation is skipped.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Value was successfully retrieved.
 *         - QAIRT_HTP_CONTEXT_ERROR_INVALID_ARGUMENT: At least one argument is invalid (e.g.,
 * skipValidationOnBinarySection is NULL).
 *         - QAIRT_HTP_CONTEXT_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *         - QAIRT_HTP_CONTEXT_ERROR_UNSUPPORTED_FEATURE: Operation is not supported on the target.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence (successful recovery).
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence (unsuccessful recovery).
 *
 * @note Use corresponding API through QairtHtpContext_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpContext_CustomConfig_getSkipBinarySectionValidation(
    QairtHtpContext_CustomConfigHandle_t customConfig, uint8_t* skipValidationOnBinarySection);

// Share Resource Optimization Type
/**
 * @brief Set the optimization type for resource sharing.
 *
 * Selects the optimization strategy for sharing resources across eligible graphs/contexts.
 *
 * @param[in] customConfig Handle to the custom configuration object.
 * @param[in] shareResourcesOptimizationType Optimization type selector.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Option was successfully set.
 *         - QAIRT_HTP_CONTEXT_ERROR_INVALID_ARGUMENT: At least one argument is invalid.
 *         - QAIRT_HTP_CONTEXT_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *         - QAIRT_HTP_CONTEXT_ERROR_UNSUPPORTED_FEATURE: Operation is not supported on the target.
 *         - QAIRT_HTP_CONTEXT_ERROR_INVALID_CONFIG: Provided type is not supported or out of range.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence (successful recovery).
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence (unsuccessful recovery).
 *
 * @note Use corresponding API through QairtHtpContext_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpContext_CustomConfig_setShareResourcesOptimizationType(
    QairtHtpContext_CustomConfigHandle_t customConfig,
    QairtHtpContext_ShareResourcesOptimizationType_t shareResourcesOptimizationType);
/**
 * @brief Get the optimization type used for resource sharing.
 *
 * Retrieves the currently configured resource sharing optimization strategy.
 *
 * @param[in]  customConfig Handle to the custom configuration object.
 * @param[out] shareResourcesOptimizationType Pointer to receive the optimization type.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Value was successfully retrieved.
 *         - QAIRT_HTP_CONTEXT_ERROR_INVALID_ARGUMENT: At least one argument is invalid (e.g.,
 * shareResourcesOptimizationType is NULL).
 *         - QAIRT_HTP_CONTEXT_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *         - QAIRT_HTP_CONTEXT_ERROR_UNSUPPORTED_FEATURE: Operation is not supported on the target.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence (successful recovery).
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence (unsuccessful recovery).
 *
 * @note Use corresponding API through QairtHtpContext_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpContext_CustomConfig_getShareResourcesOptimizationType(
    QairtHtpContext_CustomConfigHandle_t customConfig,
    QairtHtpContext_ShareResourcesOptimizationType_t* shareResourcesOptimizationType);

// Use Extended UDMA
/**
 * @brief Enable or disable use of extended UDMA.
 *
 * Configures whether extended UDMA capabilities are used when supported by the target.
 *
 * @param[in] customConfig Handle to the custom configuration object.
 * @param[in] useExtendedUdma Non-zero to enable; zero to disable.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Option was successfully set.
 *         - QAIRT_HTP_CONTEXT_ERROR_INVALID_ARGUMENT: At least one argument is invalid.
 *         - QAIRT_HTP_CONTEXT_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *         - QAIRT_HTP_CONTEXT_ERROR_UNSUPPORTED_FEATURE: Operation is not supported on the target.
 *         - QAIRT_HTP_CONTEXT_ERROR_INVALID_CONFIG: Provided value is not valid for this option.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence (successful recovery).
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence (unsuccessful recovery).
 *
 * @note Use corresponding API through QairtHtpContext_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpContext_CustomConfig_setUseExtendedUdma(
    QairtHtpContext_CustomConfigHandle_t customConfig, uint8_t useExtendedUdma);
/**
 * @brief Get the extended UDMA setting.
 *
 * Retrieves whether extended UDMA is currently enabled.
 *
 * @param[in]  customConfig Handle to the custom configuration object.
 * @param[out] useExtendedUdma Pointer to receive the non-zero/zero value indicating
 *                             whether extended UDMA is enabled.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Value was successfully retrieved.
 *         - QAIRT_HTP_CONTEXT_ERROR_INVALID_ARGUMENT: At least one argument is invalid (e.g.,
 * useExtendedUdma is NULL).
 *         - QAIRT_HTP_CONTEXT_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *         - QAIRT_HTP_CONTEXT_ERROR_UNSUPPORTED_FEATURE: Operation is not supported on the target.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence (successful recovery).
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence (unsuccessful recovery).
 *
 * @note Use corresponding API through QairtHtpContext_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpContext_CustomConfig_getUseExtendedUdma(
    QairtHtpContext_CustomConfigHandle_t customConfig, uint8_t* useExtendedUdma);

// ConcurrentGroupRegistration
/**
 * @brief Set the concurrent group registration to associate with this custom configuration.
 *
 * Associates a group registration handle used to coordinate shared resources or
 * sequencing across multiple contexts/graphs as supported.
 *
 * @param[in] customConfig Handle to the custom configuration object.
 * @param[in] groupRegistration A valid group registration handle.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Handle was successfully set.
 *         - QAIRT_HTP_CONTEXT_ERROR_INVALID_ARGUMENT: At least one argument is invalid.
 *         - QAIRT_HTP_CONTEXT_ERROR_INVALID_HANDLE: customConfig or groupRegistration is not a
 * valid handle.
 *         - QAIRT_HTP_CONTEXT_ERROR_UNSUPPORTED_FEATURE: Operation is not supported on the target.
 *         - QAIRT_HTP_CONTEXT_ERROR_INVALID_CONFIG: Provided handle is incompatible with current
 * configuration.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence (successful recovery).
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence (unsuccessful recovery).
 *
 * @note Use corresponding API through QairtHtpContext_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpContext_CustomConfig_setConcurrentGroupRegistration(
    QairtHtpContext_CustomConfigHandle_t customConfig,
    QairtHtpContext_GroupRegistrationHandle_t groupRegistration);
/**
 * @brief Get the concurrent group registration associated with this custom configuration.
 *
 * Retrieves the currently associated group registration handle, if any.
 *
 * @param[in]  customConfig Handle to the custom configuration object.
 * @param[out] groupRegistration Pointer to receive the group registration handle.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Handle was successfully retrieved.
 *         - QAIRT_HTP_CONTEXT_ERROR_INVALID_ARGUMENT: At least one argument is invalid (e.g.,
 * groupRegistration is NULL).
 *         - QAIRT_HTP_CONTEXT_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *         - QAIRT_HTP_CONTEXT_ERROR_UNSUPPORTED_FEATURE: Operation is not supported on the target.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence (successful recovery).
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence (unsuccessful recovery).
 *
 * @note Use corresponding API through QairtHtpContext_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpContext_CustomConfig_getConcurrentGroupRegistration(
    QairtHtpContext_CustomConfigHandle_t customConfig,
    QairtHtpContext_GroupRegistrationHandle_t* groupRegistration);

// LoRA Weight Sharing Enabled
/**
 * @brief Enable or disable LoRA-specific weight sharing behavior.
 *
 * Configures whether LoRA (Low-Rank Adaptation) weight sharing is enabled when supported,
 * allowing additional memory savings for LoRA-based models.
 *
 * @param[in] customConfig Handle to the custom configuration object.
 * @param[in] loraWeightSharingEnabled Non-zero to enable; zero to disable.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Option was successfully set.
 *         - QAIRT_HTP_CONTEXT_ERROR_INVALID_ARGUMENT: At least one argument is invalid.
 *         - QAIRT_HTP_CONTEXT_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *         - QAIRT_HTP_CONTEXT_ERROR_UNSUPPORTED_FEATURE: Operation is not supported on the target.
 *         - QAIRT_HTP_CONTEXT_ERROR_INVALID_CONFIG: Provided value is not valid for this option or
 * unsupported with current config.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence (successful recovery).
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence (unsuccessful recovery).
 *
 * @note Use corresponding API through QairtHtpContext_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpContext_CustomConfig_setLoraWeightSharingEnabled(
    QairtHtpContext_CustomConfigHandle_t customConfig, uint8_t loraWeightSharingEnabled);
/**
 * @brief Get the LoRA weight sharing enablement setting.
 *
 * Retrieves whether LoRA-specific weight sharing is currently enabled.
 *
 * @param[in]  customConfig Handle to the custom configuration object.
 * @param[out] loraWeightSharingEnabled Pointer to receive the non-zero/zero value indicating
 *                                      whether LoRA weight sharing is enabled.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Value was successfully retrieved.
 *         - QAIRT_HTP_CONTEXT_ERROR_INVALID_ARGUMENT: At least one argument is invalid (e.g.,
 * loraWeightSharingEnabled is NULL).
 *         - QAIRT_HTP_CONTEXT_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *         - QAIRT_HTP_CONTEXT_ERROR_UNSUPPORTED_FEATURE: Operation is not supported on the target.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence (successful recovery).
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence (unsuccessful recovery).
 *
 * @note Use corresponding API through QairtHtpContext_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpContext_CustomConfig_getLoraWeightSharingEnabled(
    QairtHtpContext_CustomConfigHandle_t customConfig, uint8_t* loraWeightSharingEnabled);

typedef Qairt_Status_t (*QairtHtpContext_GroupRegistration_CreateFn_t)(
    QairtHtpContext_GroupRegistrationHandle_t*);
typedef Qairt_Status_t (*QairtHtpContext_GroupRegistration_FreeFn_t)(
    QairtHtpContext_GroupRegistrationHandle_t);
typedef Qairt_Status_t (*QairtHtpContext_CustomConfig_CreateFn_t)(
    QairtHtpContext_CustomConfigHandle_t*);
typedef Qairt_Status_t (*QairtHtpContext_GroupRegistration_SetFirstGroupHandleFn_t)(
    QairtHtpContext_GroupRegistrationHandle_t, QairtContext_Handle_t);

typedef Qairt_Status_t (*QairtHtpContext_GroupRegistration_GetFirstGroupHandleFn_t)(
    QairtHtpContext_GroupRegistrationHandle_t, QairtContext_Handle_t*);

typedef Qairt_Status_t (*QairtHtpContext_GroupRegistration_SetMaxSpillFillFn_t)(
    QairtHtpContext_GroupRegistrationHandle_t, uint64_t);

typedef Qairt_Status_t (*QairtHtpContext_GroupRegistration_GetMaxSpillFillFn_t)(
    QairtHtpContext_GroupRegistrationHandle_t, uint64_t*);

typedef Qairt_Status_t (*QairtHtpContext_CustomConfig_FreeFn_t)(
    QairtHtpContext_CustomConfigHandle_t);

/* Weight Sharing */
typedef Qairt_Status_t (*QairtHtpContext_CustomConfig_SetEnableWeightSharingFn_t)(
    QairtHtpContext_CustomConfigHandle_t, uint8_t);

typedef Qairt_Status_t (*QairtHtpContext_CustomConfig_GetEnableWeightSharingFn_t)(
    QairtHtpContext_CustomConfigHandle_t, uint8_t*);

/* Group Registration */
typedef Qairt_Status_t (*QairtHtpContext_CustomConfig_SetGroupRegistrationFn_t)(
    QairtHtpContext_CustomConfigHandle_t, QairtHtpContext_GroupRegistrationHandle_t);

typedef Qairt_Status_t (*QairtHtpContext_CustomConfig_GetGroupRegistrationFn_t)(
    QairtHtpContext_CustomConfigHandle_t, QairtHtpContext_GroupRegistrationHandle_t*);

/* File Read Memory Budget */
typedef Qairt_Status_t (*QairtHtpContext_CustomConfig_SetFileReadMemoryBudgetFn_t)(
    QairtHtpContext_CustomConfigHandle_t, uint64_t);

typedef Qairt_Status_t (*QairtHtpContext_CustomConfig_GetFileReadMemoryBudgetFn_t)(
    QairtHtpContext_CustomConfigHandle_t, uint64_t*);

/* DSP Memory Profiling Enabled */
typedef Qairt_Status_t (*QairtHtpContext_CustomConfig_SetEnableDspMemoryProfilingFn_t)(
    QairtHtpContext_CustomConfigHandle_t, uint8_t);

typedef Qairt_Status_t (*QairtHtpContext_CustomConfig_GetEnableDspMemoryProfilingFn_t)(
    QairtHtpContext_CustomConfigHandle_t, uint8_t*);

/* Share Resources */
typedef Qairt_Status_t (*QairtHtpContext_CustomConfig_SetShareResourcesFn_t)(
    QairtHtpContext_CustomConfigHandle_t, uint8_t);

typedef Qairt_Status_t (*QairtHtpContext_CustomConfig_GetShareResourcesFn_t)(
    QairtHtpContext_CustomConfigHandle_t, uint8_t*);

/* IO Mem Estimation */
typedef Qairt_Status_t (*QairtHtpContext_CustomConfig_SetIoMemEstimationFn_t)(
    QairtHtpContext_CustomConfigHandle_t, uint8_t);

typedef Qairt_Status_t (*QairtHtpContext_CustomConfig_GetIoMemEstimationFn_t)(
    QairtHtpContext_CustomConfigHandle_t, uint8_t*);

/* Is Prepare Only */
typedef Qairt_Status_t (*QairtHtpContext_CustomConfig_SetIsPrepareOnlyFn_t)(
    QairtHtpContext_CustomConfigHandle_t, uint8_t);

typedef Qairt_Status_t (*QairtHtpContext_CustomConfig_GetIsPrepareOnlyFn_t)(
    QairtHtpContext_CustomConfigHandle_t, uint8_t*);

/* Init Acceleration */
typedef Qairt_Status_t (*QairtHtpContext_CustomConfig_SetInitAccelerationFn_t)(
    QairtHtpContext_CustomConfigHandle_t, uint8_t);

typedef Qairt_Status_t (*QairtHtpContext_CustomConfig_GetInitAccelerationFn_t)(
    QairtHtpContext_CustomConfigHandle_t, uint8_t*);

/* Skip Binary Section Validation */
typedef Qairt_Status_t (*QairtHtpContext_CustomConfig_SetSkipBinarySectionValidationFn_t)(
    QairtHtpContext_CustomConfigHandle_t, uint8_t);

typedef Qairt_Status_t (*QairtHtpContext_CustomConfig_GetSkipBinarySectionValidationFn_t)(
    QairtHtpContext_CustomConfigHandle_t, uint8_t*);

/* Share Resource Optimization Type */
typedef Qairt_Status_t (*QairtHtpContext_CustomConfig_SetShareResourcesOptimizationTypeFn_t)(
    QairtHtpContext_CustomConfigHandle_t, QairtHtpContext_ShareResourcesOptimizationType_t);

typedef Qairt_Status_t (*QairtHtpContext_CustomConfig_GetShareResourcesOptimizationTypeFn_t)(
    QairtHtpContext_CustomConfigHandle_t, QairtHtpContext_ShareResourcesOptimizationType_t*);

/* Use Extended UDMA */
typedef Qairt_Status_t (*QairtHtpContext_CustomConfig_SetUseExtendedUdmaFn_t)(
    QairtHtpContext_CustomConfigHandle_t, uint8_t);

typedef Qairt_Status_t (*QairtHtpContext_CustomConfig_GetUseExtendedUdmaFn_t)(
    QairtHtpContext_CustomConfigHandle_t, uint8_t*);

/* Concurrent Group Registration */
typedef Qairt_Status_t (*QairtHtpContext_CustomConfig_SetConcurrentGroupRegistrationFn_t)(
    QairtHtpContext_CustomConfigHandle_t, QairtHtpContext_GroupRegistrationHandle_t);

typedef Qairt_Status_t (*QairtHtpContext_CustomConfig_GetConcurrentGroupRegistrationFn_t)(
    QairtHtpContext_CustomConfigHandle_t, QairtHtpContext_GroupRegistrationHandle_t*);

/* LoRA Weight Sharing Enabled */
typedef Qairt_Status_t (*QairtHtpContext_CustomConfig_SetLoraWeightSharingEnabledFn_t)(
    QairtHtpContext_CustomConfigHandle_t, uint8_t);

typedef Qairt_Status_t (*QairtHtpContext_CustomConfig_GetLoraWeightSharingEnabledFn_t)(
    QairtHtpContext_CustomConfigHandle_t, uint8_t*);

//=============================================================================
// Public Interfaces
//=============================================================================

typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtHtpContext_GroupRegistration_CreateFn_t create;
  QairtHtpContext_GroupRegistration_FreeFn_t free;
  QairtHtpContext_GroupRegistration_SetFirstGroupHandleFn_t setFirstGroupHandle;
  QairtHtpContext_GroupRegistration_GetFirstGroupHandleFn_t getFirstGroupHandle;
  QairtHtpContext_GroupRegistration_SetMaxSpillFillFn_t setMaxSpillFill;
  QairtHtpContext_GroupRegistration_GetMaxSpillFillFn_t getMaxSpillFill;
} QairtHtpContext_GroupRegistrationV1_t;

typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtHtpContext_CustomConfig_CreateFn_t create;
  QairtHtpContext_CustomConfig_FreeFn_t free;
  QairtHtpContext_CustomConfig_SetEnableWeightSharingFn_t setEnableWeightSharing;
  QairtHtpContext_CustomConfig_GetEnableWeightSharingFn_t getEnableWeightSharing;
  QairtHtpContext_CustomConfig_SetGroupRegistrationFn_t setGroupRegistration;
  QairtHtpContext_CustomConfig_GetGroupRegistrationFn_t getGroupRegistration;
  QairtHtpContext_CustomConfig_SetFileReadMemoryBudgetFn_t setFileReadMemoryBudget;
  QairtHtpContext_CustomConfig_GetFileReadMemoryBudgetFn_t getFileReadMemoryBudget;
  QairtHtpContext_CustomConfig_SetEnableDspMemoryProfilingFn_t setEnableDspMemoryProfiling;
  QairtHtpContext_CustomConfig_GetEnableDspMemoryProfilingFn_t getEnableDspMemoryProfiling;
  QairtHtpContext_CustomConfig_SetShareResourcesFn_t setShareResources;
  QairtHtpContext_CustomConfig_GetShareResourcesFn_t getShareResources;
  QairtHtpContext_CustomConfig_SetIoMemEstimationFn_t setIoMemEstimation;
  QairtHtpContext_CustomConfig_GetIoMemEstimationFn_t getIoMemEstimation;
  QairtHtpContext_CustomConfig_SetIsPrepareOnlyFn_t setIsPrepareOnly;
  QairtHtpContext_CustomConfig_GetIsPrepareOnlyFn_t getIsPrepareOnly;
  QairtHtpContext_CustomConfig_SetInitAccelerationFn_t setInitAcceleration;
  QairtHtpContext_CustomConfig_GetInitAccelerationFn_t getInitAcceleration;
  QairtHtpContext_CustomConfig_SetSkipBinarySectionValidationFn_t setSkipBinarySectionValidation;
  QairtHtpContext_CustomConfig_GetSkipBinarySectionValidationFn_t getSkipBinarySectionValidation;
  QairtHtpContext_CustomConfig_SetShareResourcesOptimizationTypeFn_t
      setShareResourcesOptimizationType;
  QairtHtpContext_CustomConfig_GetShareResourcesOptimizationTypeFn_t
      getShareResourcesOptimizationType;
  QairtHtpContext_CustomConfig_SetUseExtendedUdmaFn_t setUseExtendedUdma;
  QairtHtpContext_CustomConfig_GetUseExtendedUdmaFn_t getUseExtendedUdma;
  QairtHtpContext_CustomConfig_SetConcurrentGroupRegistrationFn_t setConcurrentGroupRegistration;
  QairtHtpContext_CustomConfig_GetConcurrentGroupRegistrationFn_t getConcurrentGroupRegistration;
  QairtHtpContext_CustomConfig_SetLoraWeightSharingEnabledFn_t setLoraWeightSharingEnabled;
  QairtHtpContext_CustomConfig_GetLoraWeightSharingEnabledFn_t getLoraWeightSharingEnabled;
} QairtHtpContext_CustomConfigV1_t;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif
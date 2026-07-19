//=============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//=============================================================================

#ifndef QAIRT_DSP_BACKEND_H
#define QAIRT_DSP_BACKEND_H

#ifdef __cplusplus
#include <cstddef>
#include <cstdint>
#else
#include <stddef.h>
#include <stdint.h>
#endif

#include "QairtBackend/QairtBackendConfig.h"

//=============================================================================
// Macros
//=============================================================================
#define QAIRT_MIN_ID_DSP_BACKEND (QAIRT_MIN_ID_DSP + 0)

#define QAIRT_DSP_BACKEND_GROUP_REGISTRATION_ID (QAIRT_MIN_ID_DSP_BACKEND + 0)
#define QAIRT_DSP_BACKEND_CUSTOM_CONFIG_ID      (QAIRT_MIN_ID_DSP_BACKEND + 1)

//=============================================================================
// Data Types
//=============================================================================
typedef enum {
  QAIRT_DSP_BACKEND_DSP_ARCH_NONE    = 0,
  QAIRT_DSP_BACKEND_DSP_ARCH_V65     = 65,
  QAIRT_DSP_BACKEND_DSP_ARCH_V66     = 66,
  QAIRT_DSP_BACKEND_DSP_ARCH_V68     = 68,
  QAIRT_DSP_BACKEND_DSP_ARCH_V69     = 69,
  QAIRT_DSP_BACKEND_DSP_ARCH_V73     = 73,
  QAIRT_DSP_BACKEND_DSP_ARCH_UNKNOWN = 0x7fffffff
} QairtDspBackend_DspArch_t;

typedef QairtBackend_CustomConfigHandle_t QairtDspBackend_CustomConfigHandle_t;
//=============================================================================
// Public Functions
//=============================================================================

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create a custom configuration handle for DSP backend operations.
 *
 * Allocates and initializes a custom configuration object that can be used to
 * override default backend settings for DSP programs.
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
 * @note Use corresponding API through QairtDspBackend_CustomConfigV1_t.
 */
Qairt_Status_t QairtDspBackend_CustomConfig_create(
    QairtDspBackend_CustomConfigHandle_t* customConfig);

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
 * @note Use corresponding API through QairtDspBackend_CustomConfigV1_t.
 */
Qairt_Status_t QairtDspBackend_CustomConfig_free(
    QairtDspBackend_CustomConfigHandle_t customConfig);

/**
 * @brief Set whether relu activation folding into convolution is disabled.
 *
 * The accelerator will always attempt to fold relu activation into the immediate preceding
 * convolution operation. This optimization is correct when quantization ranges for convolution
 * are equal or a subset of the Relu operation. For graphs where this cannot be guaranteed,
 * set this option to true to disable the optimization.
 *
 * @param[in] customConfig          Handle to the custom configuration object.
 * @param[in] foldReluActivationIntoConvOff  Set to 1 (true) to disable relu-into-conv folding,
 *                                           or 0 (false) to allow it.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Option was successfully set.
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: At least one argument is invalid.
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *         - QAIRT_CONTEXT_ERROR_UNSUPPORTED_FEATURE: Operation is not supported on the target.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence (successful recovery).
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence (unsuccessful recovery).
 *
 * @note Use corresponding API through QairtDspBackend_CustomConfigV1_t.
 */
Qairt_Status_t QairtDspBackend_CustomConfig_setFoldReluActivationIntoConvOff(
    QairtDspBackend_CustomConfigHandle_t customConfig, uint8_t foldReluActivationIntoConvOff);

/**
 * @brief Get whether relu activation folding into convolution is disabled.
 *
 * Retrieves the currently configured setting for relu-into-conv folding optimization.
 *
 * @param[in]  customConfig                   Handle to the custom configuration object.
 * @param[out] foldReluActivationIntoConvOff  Pointer to receive the current setting.
 *                                            1 means folding is disabled, 0 means it is allowed.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Option was successfully retrieved.
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: At least one argument is invalid (e.g.,
 * foldReluActivationIntoConvOff is NULL).
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *         - QAIRT_CONTEXT_ERROR_UNSUPPORTED_FEATURE: Operation is not supported on the target.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence (successful recovery).
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence (unsuccessful recovery).
 *
 * @note Use corresponding API through QairtDspBackend_CustomConfigV1_t.
 */
Qairt_Status_t QairtDspBackend_CustomConfig_getFoldReluActivationIntoConvOff(
    QairtDspBackend_CustomConfigHandle_t customConfig, uint8_t* foldReluActivationIntoConvOff);

/**
 * @brief Set whether short-depth convolution on HMX is disabled.
 *
 * The accelerator will always attempt to execute all convolution operations using HMX
 * instructions. Convolutions that have short depth and/or weights that are not symmetric
 * could exhibit inaccurate results. In such cases, set this option to true to guarantee
 * correctness.
 *
 * @param[in] customConfig             Handle to the custom configuration object.
 * @param[in] shortDepthConvOnHmxOff   Set to 1 (true) to disable HMX for short-depth convolutions,
 *                                     or 0 (false) to allow it.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Option was successfully set.
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: At least one argument is invalid.
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *         - QAIRT_CONTEXT_ERROR_UNSUPPORTED_FEATURE: Operation is not supported on the target.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence (successful recovery).
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence (unsuccessful recovery).
 *
 * @note Use corresponding API through QairtDspBackend_CustomConfigV1_t.
 */
Qairt_Status_t QairtDspBackend_CustomConfig_setShortDepthConvOnHmxOff(
    QairtDspBackend_CustomConfigHandle_t customConfig, uint8_t shortDepthConvOnHmxOff);

/**
 * @brief Get whether short-depth convolution on HMX is disabled.
 *
 * Retrieves the currently configured setting for HMX usage with short-depth convolutions.
 *
 * @param[in]  customConfig            Handle to the custom configuration object.
 * @param[out] shortDepthConvOnHmxOff  Pointer to receive the current setting.
 *                                     1 means HMX is disabled for short-depth conv, 0 means
 * allowed.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Option was successfully retrieved.
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: At least one argument is invalid (e.g.,
 * shortDepthConvOnHmxOff is NULL).
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *         - QAIRT_CONTEXT_ERROR_UNSUPPORTED_FEATURE: Operation is not supported on the target.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence (successful recovery).
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence (unsuccessful recovery).
 *
 * @note Use corresponding API through QairtDspBackend_CustomConfigV1_t.
 */
Qairt_Status_t QairtDspBackend_CustomConfig_getShortDepthConvOnHmxOff(
    QairtDspBackend_CustomConfigHandle_t customConfig, uint8_t* shortDepthConvOnHmxOff);

/**
 * @brief Set whether to use a signed process domain for the DSP RPC session.
 *
 * Every app-side user process that uses a DSP via FastRPC has a corresponding dynamic user
 * process domain on the DSP side. QNN by default opens the RPC session as an unsigned PD.
 * Set this option to true to open the session as a signed PD (requires a signed .so).
 *
 * @param[in] customConfig           Handle to the custom configuration object.
 * @param[in] useSignedProcessDomain Set to 1 (true) to use a signed process domain,
 *                                   or 0 (false) to use an unsigned process domain (default).
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Option was successfully set.
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: At least one argument is invalid.
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *         - QAIRT_CONTEXT_ERROR_UNSUPPORTED_FEATURE: Operation is not supported on the target.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence (successful recovery).
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence (unsuccessful recovery).
 *
 * @note Use corresponding API through QairtDspBackend_CustomConfigV1_t.
 */
Qairt_Status_t QairtDspBackend_CustomConfig_setUseSignedProcessDomain(
    QairtDspBackend_CustomConfigHandle_t customConfig, uint8_t useSignedProcessDomain);

/**
 * @brief Get whether a signed process domain is configured for the DSP RPC session.
 *
 * Retrieves the currently configured process domain setting.
 *
 * @param[in]  customConfig           Handle to the custom configuration object.
 * @param[out] useSignedProcessDomain Pointer to receive the current setting.
 *                                    1 means signed PD, 0 means unsigned PD.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Option was successfully retrieved.
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: At least one argument is invalid (e.g.,
 * useSignedProcessDomain is NULL).
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *         - QAIRT_CONTEXT_ERROR_UNSUPPORTED_FEATURE: Operation is not supported on the target.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence (successful recovery).
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence (unsuccessful recovery).
 *
 * @note Use corresponding API through QairtDspBackend_CustomConfigV1_t.
 */
Qairt_Status_t QairtDspBackend_CustomConfig_getUseSignedProcessDomain(
    QairtDspBackend_CustomConfigHandle_t customConfig, uint8_t* useSignedProcessDomain);

/**
 * @brief Set the DSP architecture for offline prepare mode.
 *
 * Specifies the target DSP architecture when preparing context binaries offline
 * (i.e., without a physical device present).
 *
 * @param[in] customConfig Handle to the custom configuration object.
 * @param[in] arch         The target DSP architecture (QairtDspBackend_DspArch_t).
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Architecture was successfully set.
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: At least one argument is invalid.
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *         - QAIRT_CONTEXT_ERROR_UNSUPPORTED_FEATURE: Operation is not supported on the target.
 *         - QAIRT_CONTEXT_ERROR_INVALID_CONFIG: Provided arch value is not recognized.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence (successful recovery).
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence (unsuccessful recovery).
 *
 * @note Use corresponding API through QairtDspBackend_CustomConfigV1_t.
 */
Qairt_Status_t QairtDspBackend_CustomConfig_setArch(
    QairtDspBackend_CustomConfigHandle_t customConfig, QairtDspBackend_DspArch_t arch);

/**
 * @brief Get the configured DSP architecture for offline prepare mode.
 *
 * Retrieves the target DSP architecture currently set in the custom configuration.
 *
 * @param[in]  customConfig Handle to the custom configuration object.
 * @param[out] arch         Pointer to receive the configured DSP architecture.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Architecture was successfully retrieved.
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: At least one argument is invalid (e.g.,
 * arch is NULL).
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *         - QAIRT_CONTEXT_ERROR_UNSUPPORTED_FEATURE: Operation is not supported on the target.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence (successful recovery).
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence (unsuccessful recovery).
 *
 * @note Use corresponding API through QairtDspBackend_CustomConfigV1_t.
 */
Qairt_Status_t QairtDspBackend_CustomConfig_getArch(
    QairtDspBackend_CustomConfigHandle_t customConfig, QairtDspBackend_DspArch_t* arch);

typedef Qairt_Status_t (*QairtDspBackend_CustomConfig_CreateFn_t)(
    QairtDspBackend_CustomConfigHandle_t*);

typedef Qairt_Status_t (*QairtDspBackend_CustomConfig_FreeFn_t)(
    QairtDspBackend_CustomConfigHandle_t);

typedef Qairt_Status_t (*QairtDspBackend_CustomConfig_SetFoldReluActivationIntoConvOffFn_t)(
    QairtDspBackend_CustomConfigHandle_t, uint8_t);

typedef Qairt_Status_t (*QairtDspBackend_CustomConfig_GetFoldReluActivationIntoConvOffFn_t)(
    QairtDspBackend_CustomConfigHandle_t, uint8_t*);

typedef Qairt_Status_t (*QairtDspBackend_CustomConfig_SetShortDepthConvOnHmxOffFn_t)(
    QairtDspBackend_CustomConfigHandle_t, uint8_t);

typedef Qairt_Status_t (*QairtDspBackend_CustomConfig_GetShortDepthConvOnHmxOffFn_t)(
    QairtDspBackend_CustomConfigHandle_t, uint8_t*);

typedef Qairt_Status_t (*QairtDspBackend_CustomConfig_SetUseSignedProcessDomainFn_t)(
    QairtDspBackend_CustomConfigHandle_t, uint8_t);

typedef Qairt_Status_t (*QairtDspBackend_CustomConfig_GetUseSignedProcessDomainFn_t)(
    QairtDspBackend_CustomConfigHandle_t, uint8_t*);

typedef Qairt_Status_t (*QairtDspBackend_CustomConfig_SetArchFn_t)(
    QairtDspBackend_CustomConfigHandle_t, QairtDspBackend_DspArch_t);

typedef Qairt_Status_t (*QairtDspBackend_CustomConfig_GetArchFn_t)(
    QairtDspBackend_CustomConfigHandle_t, QairtDspBackend_DspArch_t*);

//=============================================================================
// Public Interfaces
//=============================================================================

typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtDspBackend_CustomConfig_CreateFn_t create;
  QairtDspBackend_CustomConfig_FreeFn_t free;
  QairtDspBackend_CustomConfig_SetFoldReluActivationIntoConvOffFn_t
      setFoldReluActivationIntoConvOff;
  QairtDspBackend_CustomConfig_GetFoldReluActivationIntoConvOffFn_t
      getFoldReluActivationIntoConvOff;
  QairtDspBackend_CustomConfig_SetShortDepthConvOnHmxOffFn_t setShortDepthConvOnHmxOff;
  QairtDspBackend_CustomConfig_GetShortDepthConvOnHmxOffFn_t getShortDepthConvOnHmxOff;
  QairtDspBackend_CustomConfig_SetUseSignedProcessDomainFn_t setUseSignedProcessDomain;
  QairtDspBackend_CustomConfig_GetUseSignedProcessDomainFn_t getUseSignedProcessDomain;
  QairtDspBackend_CustomConfig_SetArchFn_t setArch;
  QairtDspBackend_CustomConfig_GetArchFn_t getArch;
} QairtDspBackend_CustomConfigV1_t;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif

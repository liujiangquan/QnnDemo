//==============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//==============================================================================

/**
 * @file
 * @brief   Signal Config component API
 *
 *          A header which contains the QAIRT signal config component
 *          and the interfaces it implements.
 */

#ifndef QAISW_QAIRTSIGNALCONFIG_H
#define QAISW_QAIRTSIGNALCONFIG_H

#ifdef __cplusplus
extern "C" {
#include <cstddef>
#include <cstdint>
#else
#include <stddef.h>
#include <stdint.h>
#endif

#include "QairtCommon/QairtCommon.h"

//=============================================================================
// Macros
//=============================================================================

//=============================================================================
// Data Types
//=============================================================================

/**
 * @brief Custom configuration for Signal object
 *
 * Please refer to documentation provided by the backend for usage information
 */
typedef void* QairtSignal_CustomConfigHandle_t;

/**
 * @brief Signal configuration handle used to interface with Signal Config APIs
 *
 *        The handle itself is created and freed by the client.
 */
typedef void* QairtSignal_ConfigHandle_t;

/**
 * @brief This enum defines signal config options.
 */
typedef enum {
  /// Sets signal custom options via QairtSignal_CustomConfigHandle_t
  QAIRT_SIGNAL_CONFIG_OPTION_CUSTOM = 0,
  /// Sets abort on API calls invoked with a signal object.
  /// Abort and Timeout signals are mutually exclusive and
  /// cannot be used together.
  QAIRT_SIGNAL_CONFIG_OPTION_ABORT = 1,
  /// Sets timeout interval on API calls invoked with a signal
  /// object. Timeout and Abort signals are mutually exclusive
  /// and cannot be used together.
  QAIRT_SIGNAL_CONFIG_OPTION_TIMEOUT = 2,
  // Unused, present to ensure 32 bits.
  QAIRT_SIGNAL_CONFIG_OPTION_UNDEFINED = 0x7FFFFFFF
} QairtSignal_ConfigOption_t;

//=============================================================================
// Public Functions
//=============================================================================

/**
 * @brief Create a signal configuration handle.
 *
 * @param[out] configHandle A handle to the created signal configuration.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Signal configuration was successfully created.
 *         - QAIRT_SIGNAL_ERROR_INVALID_ARGUMENT: _configHandle_ is NULL.
 *         - QAIRT_SIGNAL_ERROR_MEM_ALLOC: Memory allocation error while
 *           creating signal configuration.
 *
 * @note Use corresponding API through QairtSignal_ConfigV1_t.
 */
Qairt_Status_t QairtSignal_Config_create(QairtSignal_ConfigHandle_t* configHandle);

/**
 * @brief Free a signal configuration handle and associated resources.
 *
 * @param[in] configHandle A handle to a created signal configuration.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Signal configuration was successfully freed.
 *         - QAIRT_SIGNAL_ERROR_INVALID_HANDLE: _configHandle_ is not a
 *           valid handle.
 *         - QAIRT_SIGNAL_ERROR_MEM_ALLOC: Memory deallocation error.
 *
 * @note Use corresponding API through QairtSignal_ConfigV1_t.
 */
Qairt_Status_t QairtSignal_Config_free(QairtSignal_ConfigHandle_t configHandle);

/**
 * @brief Set the custom config for a signal configuration.
 *
 * @param[in] configHandle A handle to a created signal configuration.
 *
 * @param[in] customConfigHandle A handle to a custom configuration.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Custom config was successfully set.
 *         - QAIRT_SIGNAL_ERROR_INVALID_HANDLE: _configHandle_ or
 *           _customConfigHandle_ is not a valid handle.
 *
 * @note Use corresponding API through QairtSignal_ConfigV1_t.
 */
Qairt_Status_t QairtSignal_Config_setCustomConfig(
    QairtSignal_ConfigHandle_t configHandle,
    QairtSignal_CustomConfigHandle_t customConfigHandle);

/**
 * @brief Get the custom config from a signal configuration.
 *
 * @param[in] configHandle A handle to a created signal configuration.
 *
 * @param[out] customConfigHandle Pointer to retrieve the custom config handle.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Custom config was successfully retrieved.
 *         - QAIRT_SIGNAL_ERROR_INVALID_HANDLE: _configHandle_ is not a
 *           valid handle.
 *         - QAIRT_SIGNAL_ERROR_INVALID_ARGUMENT: _customConfigHandle_ is
 *           NULL.
 *
 * @note Use corresponding API through QairtSignal_ConfigV1_t.
 */
Qairt_Status_t QairtSignal_Config_getCustomConfig(
    QairtSignal_ConfigHandle_t configHandle,
    QairtSignal_CustomConfigHandle_t* customConfigHandle);

/**
 * @brief Set the timeout duration for a signal configuration.
 *        Timeout interval is represented in microseconds.
 *        Tolerance for the Timeout is platform dependent and cannot be
 *        guaranteed.
 *
 * @param[in] configHandle A handle to a created signal configuration.
 *
 * @param[in] timeoutDurationUs The timeout duration in microseconds.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Timeout duration was successfully set.
 *         - QAIRT_SIGNAL_ERROR_INVALID_HANDLE: _configHandle_ is not a
 *           valid handle.
 *         - QAIRT_SIGNAL_ERROR_INVALID_ARGUMENT: _timeoutDurationUs_ is
 *           invalid.
 *
 * @note Use corresponding API through QairtSignal_ConfigV1_t.
 */
Qairt_Status_t QairtSignal_Config_setTimeoutDuration(QairtSignal_ConfigHandle_t configHandle,
                                                   uint64_t timeoutDurationUs);

/**
 * @brief Get the timeout duration from a signal configuration.
 *
 * @param[in] configHandle A handle to a created signal configuration.
 *
 * @param[out] timeoutDurationUs Pointer to retrieve the timeout duration in
 *                               microseconds.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Timeout duration was successfully retrieved.
 *         - QAIRT_SIGNAL_ERROR_INVALID_HANDLE: _configHandle_ is not a
 *           valid handle.
 *         - QAIRT_SIGNAL_ERROR_INVALID_ARGUMENT: _timeoutDurationUs_ is
 *           NULL.
 *
 * @note Use corresponding API through QairtSignal_ConfigV1_t.
 */
Qairt_Status_t QairtSignal_Config_getTimeoutDuration(QairtSignal_ConfigHandle_t configHandle,
                                                   uint64_t* timeoutDurationUs);

//=============================================================================
// Function Pointer Types
//=============================================================================

typedef Qairt_Status_t (*QairtSignal_Config_CreateFn_t)(QairtSignal_ConfigHandle_t*);

typedef Qairt_Status_t (*QairtSignal_Config_FreeFn_t)(QairtSignal_ConfigHandle_t);

typedef Qairt_Status_t (*QairtSignal_Config_SetCustomConfigFn_t)(QairtSignal_ConfigHandle_t,
                                                               QairtSignal_CustomConfigHandle_t);

typedef Qairt_Status_t (*QairtSignal_Config_GetCustomConfigFn_t)(QairtSignal_ConfigHandle_t,
                                                               QairtSignal_CustomConfigHandle_t*);

typedef Qairt_Status_t (*QairtSignal_Config_SetTimeoutDurationFn_t)(QairtSignal_ConfigHandle_t,
                                                                  uint64_t);

typedef Qairt_Status_t (*QairtSignal_Config_GetTimeoutDurationFn_t)(QairtSignal_ConfigHandle_t,
                                                                  uint64_t*);

//=============================================================================
// Public Interfaces
//=============================================================================

/**
 * @struct QairtSignal_ConfigV1_t
 * @brief This struct contains the functions that define the V1 interface for
 *        QAIRT Signal Config. This interface can be retrieved through the id
 *        QAIRT_SIGNAL_CONFIG_V1_ID.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtSignal_Config_CreateFn_t create;
  QairtSignal_Config_FreeFn_t free;
  QairtSignal_Config_SetCustomConfigFn_t setCustomConfig;
  QairtSignal_Config_GetCustomConfigFn_t getCustomConfig;
  QairtSignal_Config_SetTimeoutDurationFn_t setTimeoutDuration;
  QairtSignal_Config_GetTimeoutDurationFn_t getTimeoutDuration;
} QairtSignal_ConfigV1_t;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // QAISW_QAIRTSIGNALCONFIG_H

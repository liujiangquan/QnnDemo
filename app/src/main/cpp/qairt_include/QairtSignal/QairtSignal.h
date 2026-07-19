//==============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//==============================================================================

/**
 * @file
 * @brief   Signal component API
 *
 *          Requires Backend to be initialized.
 *          Provides means to manage Signal objects.
 *          Signal objects are used to control execution of other components.
 */

#ifndef QAISW_QAIRTSIGNAL_H
#define QAISW_QAIRTSIGNAL_H

#include "QairtBackend/QairtBackend.h"
#include "QairtCommon/QairtCommon.h"
#include "QairtSignalConfig.h"

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
// Macros
//=============================================================================

#define QAIRT_SIGNAL_V1_ID QAIRT_MIN_ID_SIGNAL

#define QAIRT_SIGNAL_CONFIG_V1_ID QAIRT_MIN_ID_SIGNAL + 1

//=============================================================================
// Data Types
//=============================================================================

/**
 * @brief QAIRT Signal API result / error codes.
 */
typedef enum {
  QAIRT_SIGNAL_MIN_ERROR = QAIRT_MIN_ERROR_SIGNAL,
  //////////////////////////////////////////

  QAIRT_SIGNAL_NO_ERROR = QAIRT_SUCCESS,
  /// Backend does not support the requested functionality
  QAIRT_SIGNAL_ERROR_UNSUPPORTED = QAIRT_COMMON_ERROR_NOT_SUPPORTED,
  /// Attempt to reconfigure, free, or supply to a second QAIRT function call
  /// a signal object that is already in use.
  QAIRT_SIGNAL_ERROR_SIGNAL_IN_USE = QAIRT_MIN_ERROR_SIGNAL + 0,
  /// Signal object is idle and not being used by an outstanding function
  /// call.
  QAIRT_SIGNAL_ERROR_SIGNAL_IDLE = QAIRT_MIN_ERROR_SIGNAL + 1,
  /// Invalid configuration error
  QAIRT_SIGNAL_ERROR_INVALID_ARGUMENT = QAIRT_MIN_ERROR_SIGNAL + 2,
  /// NULL or unrecognized signal handle error
  QAIRT_SIGNAL_ERROR_INVALID_HANDLE = QAIRT_MIN_ERROR_SIGNAL + 3,
  /// Timeout error
  QAIRT_SIGNAL_ERROR_TIMEOUT = QAIRT_MIN_ERROR_SIGNAL + 4,
  /// API supplied with incompatible signal type
  QAIRT_SIGNAL_ERROR_INCOMPATIBLE_SIGNAL_TYPE = QAIRT_MIN_ERROR_SIGNAL + 5,
  /// Memory allocation error
  QAIRT_SIGNAL_ERROR_MEM_ALLOC = QAIRT_COMMON_ERROR_MEM_ALLOC,

  //////////////////////////////////////////
  QAIRT_SIGNAL_MAX_ERROR = QAIRT_MAX_ERROR_SIGNAL,
  // Unused, present to ensure 32 bits.
  QAIRT_SIGNAL_ERROR_UNDEFINED = 0x7FFFFFFF
} QairtSignal_Error_t;

/**
 * @brief Signal handle used to interface with Signal APIs
 *
 *        The handle itself is created and freed by the client.
 */
typedef struct _QairtSignal_Handle_t* QairtSignal_Handle_t;

//=============================================================================
// Public Functions
//=============================================================================

/**
 * @brief Create a new signal object. The object will be configured with desired
 *        behavior and is idle and available for usage.
 *
 * @param[in] backendHandle A backend handle
 *
 * @param[in] configHandle A handle to a signal configuration. NULL is allowed,
 *                         indicates no config options are provided, and signal
 *                         will not be configured to do anything. All config options have default
 *                         value, in case not provided.
 *
 * @param[out] signalHandle Handle to newly created signal object.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: if the signal is created successfully
 *         - QAIRT_SIGNAL_ERROR_INVALID_ARGUMENT: at least one argument or
 *           config option invalid
 *         - QAIRT_SIGNAL_ERROR_INVALID_HANDLE: _backendHandle_ is not a
 *           valid handle
 *         - QAIRT_SIGNAL_ERROR_UNSUPPORTED: if QairtSignal API is not
 *           supported on the backend
 *         - QAIRT_SIGNAL_ERROR_MEM_ALLOC: Memory allocation error
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence
 *           (successful recovery)
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence
 *           (unsuccessful recovery)
 *
 * @note Use corresponding API through QairtSignal_V1_t.
 */
Qairt_Status_t QairtSignal_create(QairtBackend_Handle_t backendHandle,
                                  QairtSignal_ConfigHandle_t configHandle,
                                  QairtSignal_Handle_t* signalHandle);

/**
 * @brief Set/change a configuration on an existing signal
 *
 * @param[in] signalHandle Signal object whose configuration needs to be set
 *
 * @param[in] configHandle A handle to a signal configuration. NULL is allowed
 *                         and may be used to reset any previously set
 *                         configuration. No default values are assumed for config options that are
 *                         not set. If a backend cannot support all provided configs it will fail.
 *
 * @return Error Code:
 *         - QAIRT_SUCCESS: if the config is set successfully
 *         - QAIRT_SIGNAL_ERROR_INVALID_HANDLE: signal handle is null or
 *           invalid
 *         - QAIRT_SIGNAL_ERROR_INVALID_ARGUMENT: one or more config
 *           values is invalid
 *         - QAIRT_SIGNAL_ERROR_SIGNAL_IN_USE: when attempting to
 *           reconfigure a signal that is active and in-use.
 *         - QAIRT_SIGNAL_ERROR_UNSUPPORTED: if QairtSignal API is not
 *           supported on the backend
 *
 * @note Use corresponding API through QairtSignal_V1_t.
 */
Qairt_Status_t QairtSignal_setConfig(QairtSignal_Handle_t signalHandle,
                                     QairtSignal_ConfigHandle_t configHandle);

/**
 * @brief Triggers the signal action during the associated API call. For abort
 *        config signals, it causes the associated API call to gracefully cease
 *        execution at the earliest opportunity. This function will block until the
 *        targeted call has released associated resources and is ready to return in
 *        it's own calling context. When the associated API call is initiated, the
 *        signal object will be in-use and not available to another call. When the
 *        associated API call returns, the associated signal object will be available
 *        and can safely be passed to another call.
 *
 * @param[in] signalHandle Signal handle used by the associated API call
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: if the trigger is successful.
 *         - QAIRT_SIGNAL_ERROR_INVALID_HANDLE: signal handle is null or
 *           invalid
 *         - QAIRT_SIGNAL_ERROR_INCOMPATIBLE_SIGNAL_TYPE: API does not
 *           support the signal type
 *         - QAIRT_SIGNAL_ERROR_SIGNAL_IDLE: if the signal is not
 *           currently in-use, and hence can not be triggered.
 *         - QAIRT_SIGNAL_ERROR_UNSUPPORTED: if QairtSignal API is not
 *           supported on the backend
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence
 *           (successful recovery)
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence
 *           (unsuccessful recovery)
 *
 * @note Use corresponding API through QairtSignal_V1_t.
 */
Qairt_Status_t QairtSignal_trigger(QairtSignal_Handle_t signalHandle);

/**
 * @brief Free memory and resources associated with an available signal object.
 *
 * @param[in] signalHandle The signal object to free.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: if the signal object is successfully freed
 *         - QAIRT_SIGNAL_ERROR_INVALID_HANDLE: signal handle is null or
 *           invalid
 *         - QAIRT_SIGNAL_ERROR_SIGNAL_IN_USE: if the signal object is
 *           currently in-use
 *         - QAIRT_SIGNAL_ERROR_MEM_ALLOC: an error is encountered with
 *           de-allocation of associated memory
 *         - QAIRT_SIGNAL_ERROR_UNSUPPORTED: if QairtSignal API is not
 *           supported on the backend
 *
 * @note Use corresponding API through QairtSignal_V1_t.
 */
Qairt_Status_t QairtSignal_free(QairtSignal_Handle_t signalHandle);

typedef Qairt_Status_t (*QairtSignal_CreateFn_t)(QairtBackend_Handle_t,
                                                 QairtSignal_ConfigHandle_t,
                                                 QairtSignal_Handle_t*);

typedef Qairt_Status_t (*QairtSignal_SetConfigFn_t)(QairtSignal_Handle_t,
                                                    QairtSignal_ConfigHandle_t);

typedef Qairt_Status_t (*QairtSignal_TriggerFn_t)(QairtSignal_Handle_t);

typedef Qairt_Status_t (*QairtSignal_FreeFn_t)(QairtSignal_Handle_t);

//=============================================================================
// Public Interfaces
//=============================================================================

/**
 * @struct QairtSignal_V1_t
 * @brief This struct contains the functions that define the V1 interface for
 *        QAIRT Signal. This interface can be retrieved through the id
 *        QAIRT_SIGNAL_V1_ID.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtSignal_CreateFn_t create;
  QairtSignal_SetConfigFn_t setConfig;
  QairtSignal_TriggerFn_t trigger;
  QairtSignal_FreeFn_t free;
} QairtSignal_V1_t;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // QAISW_QAIRTSIGNAL_H

//==============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All Rights Reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//==============================================================================

/**
 * @file
 * @brief   QAIRT System Log API component.
 *
 *          Provides means for QAIRT System to output logging data.
 */

#ifndef QAIRT_SYSTEM_LOG_H
#define QAIRT_SYSTEM_LOG_H

#include "QairtCommon/QairtCommon.h"
#include "QairtLog/QairtLog.h"
#include "QairtSystem/QairtSystemCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
// Macros
//=============================================================================

#define QAIRT_SYSTEM_LOG_V1_ID QAIRT_MIN_ID_SYSTEM_LOG

//=============================================================================
// Data Types
//=============================================================================

/**
 * @brief A handle to a system log instance.
 */
typedef struct _QairtSystemLog_Handle_t* QairtSystemLog_Handle_t;

//=============================================================================
// Public Functions
//=============================================================================

/**
 * @brief Create a handle to a logger object.
 *
 * @param[in] callback Callback handle to handle system library generated logging messages.
 *
 * @param[in] maxLogLevel Maximum level of messages which the system library will generate.
 *
 * @param[in] userData Opaque user data pointer that will be forwarded to the callback supplied in
 *                     _callbackHandle_ on every invocation. May be NULL. The same callback
 *                     function may be registered with multiple loggers using different userData
 *                     values.
 *
 * @param[out] logger The created log handle.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: if logging is successfully initialized.
 *         - QAIRT_COMMON_ERROR_NOT_SUPPORTED: logging is not supported.
 *         - QAIRT_LOG_ERROR_INVALID_ARGUMENT: if one or more arguments is invalid.
 *         - QAIRT_LOG_ERROR_MEM_ALLOC: for memory allocation errors.
 *         - QAIRT_LOG_ERROR_INITIALIZATION: log init failed.
 *
 * @warning The callback function pointer and userData are currently global
 *          and will be overwritten by each subsequent call to QairtSystemLog_create that supplies
 *          a non-null callback. Only the most recently registered (callback, userData) pair will
 *          be active at any given time. Loggers created with a NULL (default) callback will still use
 *          the default callback regardless of non-null callback registration.
 *          This limitation will be removed in a future release.
 *
 * @note Use corresponding API through QairtSystemLog_V1_t.
 */
Qairt_Status_t QairtSystemLog_create(QairtLog_CallbackHandle_t callback,
                                          QairtLog_Level_t maxLogLevel,
                                          void* userData,
                                          QairtSystemLog_Handle_t* logger);

/**
 * @brief A function to change the log level for the supplied log handle.
 *
 * @param[in] logger A log handle.
 *
 * @param[in] maxLogLevel New maximum log level.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: if the level is changed successfully.
 *         - QAIRT_LOG_ERROR_INVALID_ARGUMENT: if maxLogLevel is not a valid QairtLog_Level_t level.
 *         - QAIRT_LOG_ERROR_INVALID_HANDLE: logger is not a valid handle
 *
 * @note Use corresponding API through QairtSystemLog_V1_t.
 */
Qairt_Status_t QairtSystemLog_setLogLevel(QairtSystemLog_Handle_t logger,
                                               QairtLog_Level_t maxLogLevel);

/**
 * @brief A function to free the memory associated with the log handle.
 *
 * @param[in] logger A log handle.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: indicates logging is terminated.
 *         - QAIRT_LOG_ERROR_MEM_ALLOC: for memory de-allocation errors.
 *         - QAIRT_LOG_ERROR_INVALID_HANDLE: logger is not a valid handle
 *
 * @note Use corresponding API through QairtSystemLog_V1_t.
 */
Qairt_Status_t QairtSystemLog_free(QairtSystemLog_Handle_t logger);

//=============================================================================
// Function Pointer Types
//=============================================================================

typedef Qairt_Status_t (*QairtSystemLog_CreateFn_t)(QairtLog_CallbackHandle_t,
                                                         QairtLog_Level_t,
                                                         void*,
                                                         QairtSystemLog_Handle_t*);

typedef Qairt_Status_t (*QairtSystemLog_SetLogLevelFn_t)(QairtSystemLog_Handle_t,
                                                              QairtLog_Level_t);

typedef Qairt_Status_t (*QairtSystemLog_FreeFn_t)(QairtSystemLog_Handle_t);

//=============================================================================
// Public Interfaces
//=============================================================================

/**
 * @struct QairtSystemLog_V1_t
 * @brief This struct contains the functions that define the V1 interface for
 *        QAIRT System Log.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtSystemLog_CreateFn_t create;
  QairtSystemLog_SetLogLevelFn_t setLogLevel;
  QairtSystemLog_FreeFn_t free;
} QairtSystemLog_V1_t;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // QAIRT_SYSTEM_LOG_H

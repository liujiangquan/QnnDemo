//==============================================================================
//
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// All rights reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//==============================================================================

/**
 * @file
 * @brief   Log component API
 *
 *          A header which contains the QAIRT Log component and the interfaces
 *          it implements.
 */

#ifndef QAISW_QAIRTLOG_H
#define QAISW_QAIRTLOG_H

#include "QairtCommon/QairtCommon.h"

#ifdef __cplusplus
extern "C" {
#include <cstdarg>
#else
#include <stdarg.h>
#endif
//=============================================================================
// Macros
//=============================================================================
#define QAIRT_LOG_V1_ID          QAIRT_MIN_ID_LOG
#define QAIRT_LOG_CALLBACK_V1_ID QAIRT_MIN_ID_LOG + 1

//=============================================================================
// Datatypes
//=============================================================================
/**
 * @brief QAIRT Log API result / error codes.
 */
typedef enum {
  QAIRT_LOG_MIN_ERROR = QAIRT_MIN_ERROR_LOG,
  ////////////////////////////////////

  /// Qairt Log success
  QAIRT_LOG_NO_ERROR = QAIRT_SUCCESS,
  /// General error relating to memory allocation in Log API
  QAIRT_LOG_ERROR_MEM_ALLOC = QAIRT_COMMON_ERROR_MEM_ALLOC,
  /// Unable to initialize logging
  QAIRT_LOG_ERROR_INITIALIZATION = 11002,
  /// Invalid argument passed
  QAIRT_LOG_ERROR_INVALID_ARGUMENT = 11003,
  /// Invalid log handle passed
  QAIRT_LOG_ERROR_INVALID_HANDLE = 11004,
  ////////////////////////////////////
  QAIRT_LOG_MAX_ERROR = QAIRT_MAX_ERROR_LOG,
  // Unused, present to ensure 32 bits.
  QAIRT_LOG_ERROR_UNDEFINED = 0x7FFFFFFF
} QairtLog_Error_t;


typedef enum {
  // Enum Levels must be in ascending order, so that the enum value
  // can be compared with the "maximum" set in QairtLog_create().
  QAIRT_LOG_LEVEL_ERROR   = 1,
  QAIRT_LOG_LEVEL_WARN    = 2,
  QAIRT_LOG_LEVEL_INFO    = 3,
  QAIRT_LOG_LEVEL_VERBOSE = 4,
  /// Reserved for developer debugging
  QAIRT_LOG_LEVEL_DEBUG = 5,
  // Present to ensure 32 bits
  QAIRT_LOG_LEVEL_MAX = 0x7fffffff
} QairtLog_Level_t;

/**
 * @brief   Log handle used to interface with Log APIs
 *
 *          The handle itself is created and freed by the client.
 */
typedef struct _QairtLog_Handle_t* QairtLog_Handle_t;

/**
 * @brief   Log Callback handle used to interface with Log Callback APIs
 *
 *          The handle itself is created and freed by the client.
 */
typedef struct _QairtLog_CallbackHandle_t* QairtLog_CallbackHandle_t;

/**
 * @brief Signature for user-supplied logging callback.
 *
 * @warning The backend may call this callback from multiple threads, and
 *              expects that it is re-entrant.
 * @param[in] fmt Printf-style message format specifier.
 *
 * @param[in] level Log level for the message. Will not be higher than the
 *                  maximum specified in QairtLog_create.
 * @param[in] timestamp Backend-generated timestamp which is monotonically
 *                      increasing, but otherwise meaningless.
 * @param[in] userData User-supplied opaque pointer passed through from QairtLog_create.
 *                     May be NULL.
 * @param[in] args Message-specific parameters, to be used with fmt.
 */

typedef void (*QairtLog_CallbackFn_t)(const char* fmt,
                                      QairtLog_Level_t level,
                                      uint64_t timestamp,
                                      void* userData,
                                      va_list args);
//=============================================================================
// Public Functions
//=============================================================================
/**
 * @brief Create a handle to a logger object. This function can be
 *        called before QairtBackend_create().
 *
 * @warning With different logging level enabled, the inference time may vary.
 *
 * @param[in] callbackHandle Callback handle for backend-generated logging
 *                           messages. NULL indicates backend may direct log
 *                           messages to the default log stream on the target platform when possible
 * (e.g. to logcat in case of Android). Otherwise, manage the callback handle via the Log Callback
 * APIs.
 * @param[in] maxLogLevel Maximum level of messages which the backend will
 *                        generate.
 * @param[in] userData Opaque user data pointer that will be forwarded to the callback supplied in
 *                     _callbackHandle_ on every invocation. May be NULL. The same callback
 *                     function may be registered with multiple loggers using different userData
 *                     values.
 * @param[out] logHandle The created log handle.
 *
 * @warning The callback function pointer and userData are currently global
 *          and will be overwritten by each subsequent call to QairtLog_create that supplies
 *          a non-null callback. Only the most recently registered (callback, userData) pair will
 *          be active at any given time. Loggers created with a NULL (default) callback will still use
 *          the default callback regardless of non-null callback registration.
 *          This limitation will be removed in a future release.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: if logging is successfully initialized.
 *         - QAIRT_COMMON_ERROR_NOT_SUPPORTED: logging is not supported.
 *         - QAIRT_LOG_ERROR_INVALID_ARGUMENT: if one or more arguments is
 *           invalid.
 *         - QAIRT_LOG_ERROR_MEM_ALLOC: for memory allocation errors.
 *         - QAIRT_LOG_ERROR_INITIALIZATION: log init failed.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurence
 *           (successful recovery)
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurence
 *           (unsuccessful recovery)
 *
 * @note Use corresponding API through QairtLog_V1_t.
 */
Qairt_Status_t QairtLog_create(QairtLog_CallbackHandle_t callbackHandle,
                                    const QairtLog_Level_t maxLogLevel,
                                    void* userData,
                                    QairtLog_Handle_t* logHandle);

/**
 * @brief Frees a log handle
 *
 * @param[in] logHandle A handle to a created logger.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered, logging is terminated.
 *         - QAIRT_LOG_ERROR_MEM_ALLOC: error related to memory
 *           deallocation
 *         - QAIRT_LOG_ERROR_INVALID_HANDLE: _logHandle_ is not a valid
 *           handle
 *
 * @note Use corresponding API through QairtLog_V1_t.
 */
Qairt_Status_t QairtLog_free(QairtLog_Handle_t logHandle);

/**
 * @brief A function to change the log level for the supplied log handle.
 *
 * @warning With different logging level enabled, the inference time may vary.
 *
 * @param[in] logHandle A created log handle.
 *
 * @param[in] maxLogLevel New maximum log level.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: if the level is changed successfully.
 *         - QAIRT_LOG_ERROR_INVALID_ARGUMENT: if maxLogLevel is not a
 *           valid QairtLog_Level_t level.
 *         - QAIRT_LOG_ERROR_INVALID_HANDLE: _logHandle_ is not a valid
 *           handle
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurence
 *           (successful recovery)
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurence
 *           (unsuccessful recovery)
 *
 * @note Use corresponding API through QairtLog_V1_t.
 */
Qairt_Status_t QairtLog_setLogLevel(QairtLog_Handle_t logHandle,
                                         const QairtLog_Level_t maxLogLevel);

/**
 * @brief Create a handle to a logging callback object. This function can be
 *        called before QairtBackend_create().
 *
 * @warning Callback implementation may cause inference time to vary.
 *
 * @param[in] callback User supplied callback for backend-generated logging
 *                     messages.
 * @param[out] callbackHandle The created callback handle.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: if logging is successfully initialized.
 *         - QAIRT_LOG_ERROR_INVALID_ARGUMENT: if one or more arguments is
 *           invalid.
 *         - QAIRT_LOG_ERROR_MEM_ALLOC: for memory allocation errors.
 *
 * @note Use corresponding API through QairtLog_CallbackV1_t.
 */
Qairt_Status_t QairtLog_Callback_create(QairtLog_CallbackFn_t callback,
                                             QairtLog_CallbackHandle_t* callbackHandle);

/**
 * @brief A function to free the callback handle.
 *
 * @param[in] callbackHandle User supplied callback for backend-generated
 *                           logging messages.
 * @return Error code:
 *         - QAIRT_SUCCESS: the handle was freed.
 *         - QAIRT_LOG_ERROR_INVALID_ARGUMENT: if one or more arguments is
 *           invalid.
 *         - QAIRT_LOG_ERROR_MEM_ALLOC: for memory allocation errors.
 *
 * @note Use corresponding API through QairtLog_CallbackV1_t.
 */
Qairt_Status_t QairtLog_Callback_free(QairtLog_CallbackHandle_t callbackHandle);

typedef Qairt_Status_t (*QairtLog_Callback_CreateFn_t)(QairtLog_CallbackFn_t,
                                                            QairtLog_CallbackHandle_t*);
typedef Qairt_Status_t (*QairtLog_Callback_FreeFn_t)(QairtLog_CallbackHandle_t);

typedef Qairt_Status_t (*QairtLog_CreateFn_t)(QairtLog_CallbackHandle_t,
                                                   const QairtLog_Level_t,
                                                   void*,
                                                   QairtLog_Handle_t*);

typedef Qairt_Status_t (*QairtLog_FreeFn_t)(QairtLog_Handle_t);

typedef Qairt_Status_t (*QairtLog_SetLogLevelFn_t)(QairtLog_Handle_t,
                                                        const QairtLog_Level_t);
//=============================================================================
// Public Interfaces
//=============================================================================
/**
 * @struct QairtLog_CallbackV1_t
 * @brief This struct contains the functions that define the V1 interface for
 *        QAIRT Log Callback. This interface can be retrieved through the
 *        id QAIRT_LOG_CALLBACK_V1_ID.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtLog_Callback_CreateFn_t create;
  QairtLog_Callback_FreeFn_t free;
} QairtLog_CallbackV1_t;

/**
 * @struct QairtLog_V1_t
 * @brief This struct contains the functions that define the V1 interface for
 *        QAIRT Log. This interface can be retrieved through the id
 *        QAIRT_LOG_V1_ID.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtLog_CreateFn_t create;
  QairtLog_FreeFn_t free;
  QairtLog_SetLogLevelFn_t setLogLevel;
} QairtLog_V1_t;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // QAISW_QAIRTLOG_H

//==============================================================================
//
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// All rights reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//==============================================================================

/**
 * @file
 * @brief   Backend config component API
 *
 *          A header which contains the QAIRT Backend config component and the
 *          interfaces it implements.
 */

#ifndef QAISW_QAIRTBACKENDCONFIG_H
#define QAISW_QAIRTBACKENDCONFIG_H

#include "QairtCommon/QairtCommon.h"
#include "QairtCommon/QairtErrorReportingConfig.h"

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
// Datatypes
//=============================================================================

/**
 * @brief   Backend Config handle used to interface with Backend Config APIs
 *
 *          The handle itself is created and freed by the client.
 */
typedef struct _QairtBackend_ConfigHandle_t* QairtBackend_ConfigHandle_t;

/**
 * @brief   Backend Custom Config handle used to interface with Backend Custom
 *          Config APIs, which are specific to each backend.
 *
 *          The handle itself is created and freed by the client.
 */
typedef struct _QairtBackend_CustomConfigHandle_t* QairtBackend_CustomConfigHandle_t;

//=============================================================================
// Public Functions
//=============================================================================
/**
 * @brief Creates a backend config handle.
 *
 * @param[out] cfgHandle A handle to the created config.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_BACKEND_ERROR_INVALID_ARGUMENT: _cfgHandle_ is NULL
 *
 * @note Use corresponding API through QairtBackend_ConfigV1_t.
 */
Qairt_Status_t QairtBackend_Config_create(QairtBackend_ConfigHandle_t* cfgHandle);

/**
 * @brief Frees a backend config handle.
 *
 * @param[in] cfgHandle A handle to the created config.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_BACKEND_ERROR_INVALID_HANDLE: _cfgHandle_ is not a
 *           valid handle
 *         - QAIRT_BACKEND_ERROR_MEM_ALLOC: Memory deallocation error
 *
 * @note Use corresponding API through QairtBackend_ConfigV1_t.
 */
Qairt_Status_t QairtBackend_Config_free(QairtBackend_ConfigHandle_t cfgHandle);

/**
 * @brief Applies an array of custom configs onto a Backend Config.
 *
 * @param[in] cfgHandle A handle to a created backend config.
 *
 * @param[in] customConfigHandles An array of custom configuration handles.
 *
 * @param[in] numCustomConfigs The number of handles in the array.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_BACKEND_ERROR_INVALID_HANDLE: _cfgHandle_ or one or more
 *           handles in _customConfigHandles_ are not valid handles
 *         - QAIRT_BACKEND_ERROR_INVALID_ARGUMENT: _customConfigHandles_ is
 *           NULL when _numCustomConfigs_ is greater than zero
 *
 * @note Use corresponding API through QairtBackend_ConfigV1_t.
 */
Qairt_Status_t QairtBackend_Config_setCustomConfigs(
    QairtBackend_ConfigHandle_t cfgHandle,
    const QairtBackend_CustomConfigHandle_t* customConfigHandles,
    uint32_t numCustomConfigs);

/**
 * @brief Gets the number of custom configurations associated with the Backend
 *        Config handle.
 *
 * @param[in] cfgHandle A handle to the created config.
 *
 * @param[out] numConfigs Pointer to retrieve the number of custom configs.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_BACKEND_ERROR_INVALID_HANDLE: _cfgHandle_ is not a
 *           valid handle
 *         - QAIRT_BACKEND_ERROR_INVALID_ARGUMENT: _numConfigs_ is NULL
 *
 * @note Use corresponding API through QairtBackend_ConfigV1_t.
 */
Qairt_Status_t QairtBackend_Config_getNumCustomConfigs(QairtBackend_ConfigHandle_t cfgHandle,
                                                            uint32_t* numConfigs);

/**
 * @brief Gets a custom config at a particular index. Index must be less than
 *        getNumCustomConfigs.
 *
 * @param[in] cfgHandle A handle to the created config.
 *
 * @param[in] index Index of the custom config
 *
 * @param[out] customConfigHandle A referenced backend custom config handle.
 *                                Reference handles should not be freed.
 *                                Referenced handles are invalidated if modification operations are
 * performed from the same component API.
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_BACKEND_ERROR_INVALID_HANDLE: _cfgHandle_ is not a
 *           valid handle
 *         - QAIRT_BACKEND_ERROR_INVALID_ARGUMENT: _customConfigHandle_ is
 *           NULL or _index_ is invalid
 *
 * @note Use corresponding API through QairtBackend_ConfigV1_t.
 */
Qairt_Status_t QairtBackend_Config_getCustomConfigAt(
    QairtBackend_ConfigHandle_t cfgHandle,
    uint32_t index,
    QairtBackend_CustomConfigHandle_t* customConfigHandle);

/**
 * @brief Sets an Error Reporting Config onto a Backend Config.
 *
 * @param[in] cfgHandle A handle to the created config.
 *
 * @param[in] errorReportingConfigHandle A handle to an error reporting config,
 *                                       which is managed by the Error Reporting
 *                                       Config component APIs.
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_BACKEND_ERROR_INVALID_HANDLE: _cfgHandle_ or
 *           _errorReportingConfig_ is not a valid handle
 *         - QAIRT_BACKEND_ERROR_INVALID_ARGUMENT: at least one argument
 *           is invalid
 *
 * @note Use corresponding API through QairtBackend_ConfigV1_t.
 */
Qairt_Status_t QairtBackend_Config_setErrorReportingConfig(
    QairtBackend_ConfigHandle_t cfgHandle,
    const QairtErrorReporting_ConfigHandle_t errorReportingConfigHandle);

/**
 * @brief Gets an Error Reporting Config associated with a Backend Config.
 *
 * @param[in] cfgHandle A handle to the created config.
 *
 * @param[out] errorReportingConfigHandle A handle to a referenced error
 *                                        reporting config, which is managed by
 *                                        the Error Reporting Config component APIs. Referenced
 * handles should not be freed. Referenced handles are invalidated if modification operations are
 *                                        performed from the same component API.
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_BACKEND_ERROR_INVALID_HANDLE: _cfgHandle_ is not a
 *           valid handle
 *         - QAIRT_BACKEND_ERROR_INVALID_ARGUMENT: _errorReportingConfig_
 *           is NULL
 *
 * @note Use corresponding API through QairtBackend_ConfigV1_t.
 */
Qairt_Status_t QairtBackend_Config_getErrorReportingConfig(
    QairtBackend_ConfigHandle_t cfgHandle,
    QairtErrorReporting_ConfigHandle_t* errorReportingConfigHandle);

/**
 * @brief Sets an array of Platform options onto a Backend Config.
 *
 * @param[in] cfgHandle A handle to the created config.
 *
 * @param[in] platformOptions An array of null-terminated platform option
 *                            key-value pair strings. Max length per option
 *                            is 1024.
 *
 * @param[in] numPlatformOptions The number of platform options in the array.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_BACKEND_ERROR_INVALID_HANDLE: _cfgHandle_ is not a
 *           valid handle
 *         - QAIRT_BACKEND_ERROR_INVALID_ARGUMENT: _platformOptions_ is
 *           NULL when _numPlatformOptions_ is greater than zero, or one
 *           or more options are invalid
 *
 * @note Use corresponding API through QairtBackend_ConfigV1_t.
 */
Qairt_Status_t QairtBackend_Config_setPlatformOptions(QairtBackend_ConfigHandle_t cfgHandle,
                                                           const char* const* platformOptions,
                                                           uint32_t numPlatformOptions);

/**
 * @brief Gets the number of platform options associated with a Backend Config
 *        handle.
 *
 * @param[in] cfgHandle A handle to the created config.
 *
 * @param[out] numPlatformOptions Pointer to retrieve the number of platform
 *                                options associated with the Backend Config
 *                                handle.
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_BACKEND_ERROR_INVALID_HANDLE: _cfgHandle_ is not a
 *           valid handle
 *         - QAIRT_BACKEND_ERROR_INVALID_ARGUMENT: _numPlatformOptions_ is
 *           NULL
 *
 * @note Use corresponding API through QairtBackend_ConfigV1_t.
 */
Qairt_Status_t QairtBackend_Config_getNumPlatformOptions(
    QairtBackend_ConfigHandle_t cfgHandle, uint32_t* numPlatformOptions);

/**
 * @brief Gets a Platform option at a particular index associated with the
 *        Backend Config Handle.
 *
 * @param[in] cfgHandle A handle to the created config.
 *
 * @param[in] index Index of the platform option. Value must be less than
 *                  getNumPlatformOptions.
 * @param[out] platformOption The null-terminated platform option key-value pair
 *                            with a max length of 1024.
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_BACKEND_ERROR_INVALID_HANDLE: _cfgHandle_ is not a
 *           valid handle
 *         - QAIRT_BACKEND_ERROR_INVALID_ARGUMENT: _platformOption_ is
 *           NULL or _index_ is invalid
 *
 * @note Use corresponding API through QairtBackend_ConfigV1_t.
 */
Qairt_Status_t QairtBackend_Config_getPlatformOptionAt(QairtBackend_ConfigHandle_t cfgHandle,
                                                            uint32_t index,
                                                            const char** platformOption);

typedef Qairt_Status_t (*QairtBackend_Config_CreateFn_t)(QairtBackend_ConfigHandle_t*);
typedef Qairt_Status_t (*QairtBackend_Config_FreeFn_t)(QairtBackend_ConfigHandle_t);

typedef Qairt_Status_t (*QairtBackend_Config_SetCustomConfigsFn_t)(
    QairtBackend_ConfigHandle_t, const QairtBackend_CustomConfigHandle_t*, uint32_t);
typedef Qairt_Status_t (*QairtBackend_Config_GetNumCustomConfigsFn_t)(
    QairtBackend_ConfigHandle_t, uint32_t*);
typedef Qairt_Status_t (*QairtBackend_Config_GetCustomConfigAtFn_t)(
    QairtBackend_ConfigHandle_t, uint32_t, QairtBackend_CustomConfigHandle_t*);

typedef Qairt_Status_t (*QairtBackend_Config_SetErrorReportingConfigFn_t)(
    QairtBackend_ConfigHandle_t, const QairtErrorReporting_ConfigHandle_t);
typedef Qairt_Status_t (*QairtBackend_Config_GetErrorReportingConfigFn_t)(
    QairtBackend_ConfigHandle_t, QairtErrorReporting_ConfigHandle_t*);

typedef Qairt_Status_t (*QairtBackend_Config_SetPlatformOptionsFn_t)(
    QairtBackend_ConfigHandle_t, const char* const*, uint32_t);
typedef Qairt_Status_t (*QairtBackend_Config_GetNumPlatformOptionsFn_t)(
    QairtBackend_ConfigHandle_t, uint32_t*);
typedef Qairt_Status_t (*QairtBackend_Config_GetPlatformOptionAtFn_t)(
    QairtBackend_ConfigHandle_t, uint32_t, const char**);
//=============================================================================
// Public Interfaces
//=============================================================================
/**
 * @struct QairtBackend_ConfigV1_t
 * @brief This struct contains the functions that define the V1 interface for
 *        QAIRT Backend Config. This interface can be retrieved through the
 *        id QAIRT_BACKEND_CONFIG_V1_ID.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtBackend_Config_CreateFn_t create;
  QairtBackend_Config_FreeFn_t free;
  QairtBackend_Config_SetCustomConfigsFn_t setCustomConfigs;
  QairtBackend_Config_GetNumCustomConfigsFn_t getNumCustomConfigs;
  QairtBackend_Config_GetCustomConfigAtFn_t getCustomConfigAt;
  QairtBackend_Config_SetErrorReportingConfigFn_t setErrorReportingConfig;
  QairtBackend_Config_GetErrorReportingConfigFn_t getErrorReportingConfig;
  QairtBackend_Config_SetPlatformOptionsFn_t setPlatformOptions;
  QairtBackend_Config_GetNumPlatformOptionsFn_t getNumPlatformOptions;
  QairtBackend_Config_GetPlatformOptionAtFn_t getPlatformOptionAt;
} QairtBackend_ConfigV1_t;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // QAISW_QAIRTBACKENDCONFIG_H

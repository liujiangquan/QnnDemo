//=============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//=============================================================================

#ifndef QAIRT_PROFILECONFIG_H
#define QAIRT_PROFILECONFIG_H

#include "QairtCommon/QairtCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
// Data Types
//=============================================================================
typedef struct _QairtProfile_ConfigHandle_t* QairtProfile_ConfigHandle_t;
typedef struct _QairtProfile_CustomConfigHandle_t* QairtProfile_CustomConfigHandle_t;

//=============================================================================
// Public Functions
//=============================================================================

/**
 * @brief Create a profile config
 *
 * @param[out] configHandle A handle to the created profile config.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Config was successfully created.
 *         - QAIRT_PROFILE_ERROR_INVALID_HANDLE: Config handle is invalid.
 *         - QAIRT_PROFILE_ERROR_MEM_ALLOC: Config creation failed due to memory/resource allocation
 *                                          issues.
 *
 * @note Use corresponding API through QairtProfile_ConfigV1_t.
 */

Qairt_Status_t QairtProfile_Config_create(QairtProfile_ConfigHandle_t* configHandle);

/**
 * @brief Free a profile config handle and associated resources.
 *
 * @param[in] configHandle A handle to a created profile config.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Profile was successfully freed.
 *         - QAIRT_PROFILE_ERROR_INVALID_HANDLE: _configHandle_ is not a valid handle.
 *         - QAIRT_PROFILE_ERROR_MEM_ALLOC: Memory deallocation error.
 *
 * @note Use corresponding API through QairtProfile_ConfigV1_t.
 */
Qairt_Status_t QairtProfile_Config_free(QairtProfile_ConfigHandle_t configHandle);

/**
 * @brief Set optrace profiling support via enableOptrace flag
 *
 * @param[in] configHandle A profile config handle.
 *
 * @param[in] enableOpTrace Whether to enable optrace or not.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_PROFILE_ERROR_INVALID_HANDLE: _configHandle_ is not a valid handle
 *
 * @note Use corresponding API through QairtProfile_ConfigV1_t.
 */
Qairt_Status_t QairtProfile_Config_setEnableOpTrace(QairtProfile_ConfigHandle_t configHandle,
                                                         uint8_t enableOpTrace);

/**
 * @brief Get optrace profiling support flag
 *
 * @param[in] configHandle A profile config handle.
 *
 * @param[out] enableOpTrace Whether optrace is enabled or not.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_PROFILE_ERROR_INVALID_HANDLE: _configHandle_ is not a valid handle
 *         - QAIRT_PROFILE_ERROR_INVALID_ARGUMENT: _enableOpTrace_ is NULL
 *
 * @note Use corresponding API through QairtProfile_ConfigV1_t.
 */
Qairt_Status_t QairtProfile_Config_getEnableOpTrace(QairtProfile_ConfigHandle_t configHandle,
                                                         uint8_t* enableOpTrace);

/**
 * @brief This config sets the maximum number of profiling events that can be stored in the profile
 *        handle.
 *
 * @param[in] configHandle A profile config handle.
 *
 * @param[in] maxEvents Max number of events. Once the maximum number of events is reached, no more
 *                      events will be stored. The absolute maximum number of events is subject to
 *                      a maximum limit determined by the backend and available system resources.
 *                      The default maximum number of events is backend-specific, refer to SDK
 *                      documentation.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_PROFILE_ERROR_INVALID_HANDLE: _configHandle_ is not a valid handle
 *
 * @note Use corresponding API through QairtProfile_ConfigV1_t.
 */
Qairt_Status_t QairtProfile_Config_setMaxEvents(QairtProfile_ConfigHandle_t configHandle,
                                                     uint64_t maxEvents);

/**
 * @brief Get the maximum number of profiling events that can be stored in the profile handle.
 *
 * @param[in] configHandle A profile config handle.
 *
 * @param[out] maxEvents Max number of events. The absolute maximum number of events is subject to
 *                       a maximum limit determined by the backend and available system resources.
 *                       The default maximum number of events is backend-specific, refer to SDK
 *                       documentation.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_PROFILE_ERROR_INVALID_HANDLE: _configHandle_ is not a valid handle
 *         - QAIRT_PROFILE_ERROR_INVALID_ARGUMENT: _maxEvents_ is NULL
 *
 * @note Use corresponding API through QairtProfile_ConfigV1_t.
 */
Qairt_Status_t QairtProfile_Config_getMaxEvents(QairtProfile_ConfigHandle_t configHandle,
                                                     uint64_t* maxEvents);

/**
 * @brief Sets backend custom configs, see backend specific documentation.
 *
 * @param[in] configHandle A profile config handle.
 *
 * @param[in] customConfig Backend specific custom config.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_PROFILE_ERROR_INVALID_HANDLE: either _configHandle_ or
 * _QairtProfile_CustomConfigHandle_t_ is not a valid handle
 *
 * @note Use corresponding API through QairtProfile_ConfigV1_t.
 */
Qairt_Status_t QairtProfile_Config_setCustomConfig(
    QairtProfile_ConfigHandle_t configHandle, QairtProfile_CustomConfigHandle_t customConfig);

/**
 * @brief Gets custom config from the profile config object.
 *
 * @param[in] configHandle A profile config handle.
 *
 * @param[out] customConfig A referenced backend custom config handle. Reference handles should not
 *                          be freed. Referenced handles are invalidated if modification operations
 *                          are performed from the same component API.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_PROFILE_ERROR_INVALID_HANDLE: _configHandle_ is not a valid handle
 *         - QAIRT_PROFILE_ERROR_INVALID_ARGUMENT: _customConfig_ is NULL
 *
 * @note Use corresponding API through QairtProfile_ConfigV1_t.
 */
Qairt_Status_t QairtProfile_Config_getCustomConfig(
    QairtProfile_ConfigHandle_t configHandle, QairtProfile_CustomConfigHandle_t* customConfig);

typedef Qairt_Status_t (*QairtProfile_Config_CreateFn_t)(QairtProfile_ConfigHandle_t*);

typedef Qairt_Status_t (*QairtProfile_Config_FreeFn_t)(QairtProfile_ConfigHandle_t);

typedef Qairt_Status_t (*QairtProfile_Config_SetEnableOpTraceFn_t)(
    QairtProfile_ConfigHandle_t, uint8_t);

typedef Qairt_Status_t (*QairtProfile_Config_GetEnableOpTraceFn_t)(
    QairtProfile_ConfigHandle_t, uint8_t*);

typedef Qairt_Status_t (*QairtProfile_Config_SetMaxEventsFn_t)(QairtProfile_ConfigHandle_t,
                                                                    uint64_t);

typedef Qairt_Status_t (*QairtProfile_Config_GetMaxEventsFn_t)(QairtProfile_ConfigHandle_t,
                                                                    uint64_t*);

typedef Qairt_Status_t (*QairtProfile_Config_SetCustomConfigFn_t)(
    QairtProfile_ConfigHandle_t, QairtProfile_CustomConfigHandle_t);

typedef Qairt_Status_t (*QairtProfile_Config_GetCustomConfigFn_t)(
    QairtProfile_ConfigHandle_t, QairtProfile_CustomConfigHandle_t*);

//=============================================================================
// Public Interfaces
//=============================================================================

typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtProfile_Config_CreateFn_t create;
  QairtProfile_Config_FreeFn_t free;
  QairtProfile_Config_SetEnableOpTraceFn_t setEnableOpTrace;
  QairtProfile_Config_GetEnableOpTraceFn_t getEnableOpTrace;
  QairtProfile_Config_SetMaxEventsFn_t setMaxEvents;
  QairtProfile_Config_GetMaxEventsFn_t getMaxEvents;
  QairtProfile_Config_SetCustomConfigFn_t setCustomConfig;
  QairtProfile_Config_GetCustomConfigFn_t getCustomConfig;
} QairtProfile_ConfigV1_t;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // QAIRT_PROFILECONFIG_H

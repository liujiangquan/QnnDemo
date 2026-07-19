//==============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//==============================================================================

/**
 * @file
 * @brief   Device Config component API
 *
 *          Provides interfaces for managing device configuration options.
 */

#ifndef QAISW_QAIRTDEVICECONFIG_H
#define QAISW_QAIRTDEVICECONFIG_H

#include "QairtCommon/QairtCommon.h"
#include "QairtDeviceCommon.h"
#include "QairtDevicePlatformInfo.h"
#include "QairtLog/QairtLog.h"

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
// Macros
//=============================================================================

//=============================================================================
// Datatypes
//=============================================================================

typedef struct _QairtDevice_ConfigHandle_t* QairtDevice_ConfigHandle_t;
typedef struct _QairtDevice_CustomConfigHandle_t* QairtDevice_CustomConfigHandle_t;

//=============================================================================
// Public Functions
//=============================================================================

/**
 * @brief Create a device configuration handle.
 *
 * @param[out] configHandle A handle to the created device configuration.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_DEVICE_ERROR_INVALID_ARGUMENT: _configHandle_ is NULL
 *         - QAIRT_DEVICE_ERROR_MEM_ALLOC: Failure in allocating memory
 *
 * @note Use corresponding API through QairtDevice_ConfigV1_t.
 */
Qairt_Status_t QairtDevice_Config_create(QairtDevice_ConfigHandle_t* configHandle);

/**
 * @brief Free a device configuration handle and associated resources.
 *
 * @param[in] configHandle A handle to the device configuration.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_DEVICE_ERROR_INVALID_HANDLE: _configHandle_ is not a
 *           valid handle
 *         - QAIRT_DEVICE_ERROR_MEM_ALLOC: Error in de-allocating memory
 *
 * @note Use corresponding API through QairtDevice_ConfigV1_t.
 */
Qairt_Status_t QairtDevice_Config_free(QairtDevice_ConfigHandle_t configHandle);

/**
 * @brief Set platform info in the device configuration.
 *
 * @param[in] configHandle A handle to the device configuration.
 *
 * @param[in] platformInfoHandle A handle to the platform info to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_DEVICE_ERROR_INVALID_HANDLE: _configHandle_ is not a
 *           valid handle
 *         - QAIRT_DEVICE_ERROR_INVALID_ARGUMENT: _platformInfoHandle_ is
 *           NULL
 *
 * @note Use corresponding API through QairtDevice_ConfigV1_t.
 */
Qairt_Status_t QairtDevice_Config_setPlatformInfo(
    QairtDevice_ConfigHandle_t configHandle,
    QairtDevice_PlatformInfoHandle_t platformInfoHandle);

/**
 * @brief Get platform info from the device configuration.
 *
 * @param[in] configHandle A handle to the device configuration.
 *
 * @param[out] platformInfoHandle A referenced platform info handle. Reference
 *                                handles should not be freed. Referenced
 *                                handles are invalidated if modification operations are performed
 * from the same component API.
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_DEVICE_ERROR_INVALID_HANDLE: _configHandle_ is not a
 *           valid handle
 *         - QAIRT_DEVICE_ERROR_INVALID_ARGUMENT: _platformInfoHandle_ is
 *           NULL
 *
 * @note Use corresponding API through QairtDevice_ConfigV1_t.
 */
Qairt_Status_t QairtDevice_Config_getPlatformInfo(
    QairtDevice_ConfigHandle_t configHandle,
    QairtDevice_PlatformInfoHandle_t* platformInfoHandle);

/**
 * @brief Set custom configuration in the device configuration.
 *
 * @param[in] configHandle A handle to the device configuration.
 *
 * @param[in] customConfigHandle A handle to the custom configuration to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_DEVICE_ERROR_INVALID_HANDLE: _configHandle_ is not a
 *           valid handle
 *         - QAIRT_DEVICE_ERROR_INVALID_ARGUMENT: _customConfigHandle_ is
 *           NULL
 *
 * @note Use corresponding API through QairtDevice_ConfigV1_t.
 */
Qairt_Status_t QairtDevice_Config_setCustomConfig(
    QairtDevice_ConfigHandle_t configHandle,
    QairtDevice_CustomConfigHandle_t customConfigHandle);

/**
 * @brief Get the number of custom configurations in the device configuration.
 *
 * @param[in] configHandle A handle to the device configuration.
 *
 * @param[out] numCustomConfigs Number of custom configurations.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_DEVICE_ERROR_INVALID_HANDLE: _configHandle_ is not a
 *           valid handle
 *         - QAIRT_DEVICE_ERROR_INVALID_ARGUMENT: _numCustomConfigs_ is
 *           NULL
 *
 * @note Use corresponding API through QairtDevice_ConfigV1_t.
 */
Qairt_Status_t QairtDevice_Config_getNumCustomConfigs(
    QairtDevice_ConfigHandle_t configHandle, uint32_t* numCustomConfigs);

/**
 * @brief Get a custom configuration handle at the specified index.
 *
 * @param[in] configHandle A handle to the device configuration.
 *
 * @param[in] index Index of the custom configuration to retrieve.
 *
 * @param[out] customConfigHandle A referenced custom configuration handle at
 *                                the specified index. Reference handles should
 *                                not be freed. Referenced handles are invalidated if modification
 * operations are performed from the same component API.
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_DEVICE_ERROR_INVALID_HANDLE: _configHandle_ is not a
 *           valid handle
 *         - QAIRT_DEVICE_ERROR_INVALID_ARGUMENT: _customConfigHandle_ is
 *           NULL
 *         - QAIRT_DEVICE_ERROR_OUT_OF_RANGE: _index_ is out of range
 *
 * @note Use corresponding API through QairtDevice_ConfigV1_t.
 */
Qairt_Status_t QairtDevice_Config_getCustomConfigAt(
    QairtDevice_ConfigHandle_t configHandle,
    uint32_t index,
    QairtDevice_CustomConfigHandle_t* customConfigHandle);

/**
 * @brief Clear all custom configurations from the device configuration.
 *
 * @param[in] configHandle A handle to the device configuration.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_DEVICE_ERROR_INVALID_HANDLE: _configHandle_ is not a
 *           valid handle
 *
 * @note Use corresponding API through QairtDevice_ConfigV1_t.
 */
Qairt_Status_t QairtDevice_Config_clearCustomConfigs(
    QairtDevice_ConfigHandle_t configHandle);

typedef Qairt_Status_t (*QairtDevice_Config_CreateFn_t)(QairtDevice_ConfigHandle_t*);
typedef Qairt_Status_t (*QairtDevice_Config_FreeFn_t)(QairtDevice_ConfigHandle_t);
typedef Qairt_Status_t (*QairtDevice_Config_SetPlatformInfoFn_t)(
    QairtDevice_ConfigHandle_t, QairtDevice_PlatformInfoHandle_t);
typedef Qairt_Status_t (*QairtDevice_Config_GetPlatformInfoFn_t)(
    QairtDevice_ConfigHandle_t, QairtDevice_PlatformInfoHandle_t*);
typedef Qairt_Status_t (*QairtDevice_Config_SetCustomConfigFn_t)(
    QairtDevice_ConfigHandle_t, QairtDevice_CustomConfigHandle_t);
typedef Qairt_Status_t (*QairtDevice_Config_GetNumCustomConfigsFn_t)(
    QairtDevice_ConfigHandle_t, uint32_t*);
typedef Qairt_Status_t (*QairtDevice_Config_GetCustomConfigAtFn_t)(
    QairtDevice_ConfigHandle_t, uint32_t, QairtDevice_CustomConfigHandle_t*);
typedef Qairt_Status_t (*QairtDevice_Config_ClearCustomConfigsFn_t)(
    QairtDevice_ConfigHandle_t);
//=============================================================================
// Public Interfaces
//=============================================================================
/**
 * @struct QairtDevice_ConfigV1_t
 * @brief This struct contains the functions that define the V1 interface for
 *        QAIRT Device Config. This interface can be retrieved through the
 *        id QAIRT_DEVICE_CONFIG_V1_ID.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtDevice_Config_CreateFn_t create;
  QairtDevice_Config_FreeFn_t free;
  QairtDevice_Config_SetPlatformInfoFn_t setPlatformInfo;
  QairtDevice_Config_GetPlatformInfoFn_t getPlatformInfo;
  QairtDevice_Config_SetCustomConfigFn_t setCustomConfig;
  QairtDevice_Config_GetNumCustomConfigsFn_t getNumCustomConfig;
  QairtDevice_Config_GetCustomConfigAtFn_t getCustomConfigAt;
  QairtDevice_Config_ClearCustomConfigsFn_t clearCustomConfigs;
} QairtDevice_ConfigV1_t;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // QAISW_QAIRTDEVICECONFIG_H

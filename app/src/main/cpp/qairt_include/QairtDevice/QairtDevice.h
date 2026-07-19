//==============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//==============================================================================

/**
 * @file
 * @brief   Device component API
 *
 *          This is the top level QAIRT API component for hardware resource
 * management.
 */

#ifndef QAISW_QAIRTDEVICE_H
#define QAISW_QAIRTDEVICE_H

#include "QairtCommon/QairtCommon.h"
#include "QairtDeviceCommon.h"
#include "QairtDeviceConfig.h"
#include "QairtDeviceInfrastructure.h"
#include "QairtDevicePlatformInfo.h"
#include "QairtLog/QairtLog.h"

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
// Macros
//=============================================================================

#define QAIRT_DEVICE_V1_ID                        QAIRT_MIN_ID_DEVICE
#define QAIRT_DEVICE_CONFIG_V1_ID                 QAIRT_MIN_ID_DEVICE + 1
#define QAIRT_DEVICE_INRFA_PROVIDER_V1_ID         QAIRT_MIN_ID_DEVICE + 2
#define QAIRT_DEVICE_INRFA_V1_ID                  QAIRT_MIN_ID_DEVICE + 3
#define QAIRT_DEVICE_PLATFORM_INFO_PROVIDER_V1_ID QAIRT_MIN_ID_DEVICE + 4
#define QAIRT_DEVICE_PLATFORM_INFO_V1_ID          QAIRT_MIN_ID_DEVICE + 5
#define QAIRT_DEVICE_HW_DEVICE_INFO_V1_ID         QAIRT_MIN_ID_DEVICE + 6
#define QAIRT_DEVICE_HW_CORE_INFO_V1_ID           QAIRT_MIN_ID_DEVICE + 7
#define QAIRT_DEVICE_HW_EXT_DEVICE_INFO_V1_ID     QAIRT_MIN_ID_DEVICE + 8
#define QAIRT_DEVICE_HW_EXT_CORE_INFO_V1_ID       QAIRT_MIN_ID_DEVICE + 9

/// Reserved value to select a default device
#define QAIRT_DEVICE_DEFAULT_DEVICE_ID 0xFFFFFFFF

/// Reserved value to select a default core
#define QAIRT_DEVICE_DEFAULT_CORE_ID 0xFFFFFFFF

//=============================================================================
// Datatypes
//=============================================================================

/**
 * @brief   Device handle used to interface with Device APIs
 *
 *          The handle itself is created and freed by the client.
 */
typedef struct _QairtDevice_Handle_t* QairtDevice_Handle_t;

//=============================================================================
// Public Functions
//=============================================================================

/**
 * @brief Create a logical device handle to a subset of hardware resources
 *        available on the platform.
 *
 * @param[in] logHandle A handle to the logger. Use a NULL handle to disable
 *                      logging. QairtDevice does not manage the lifecycle of
 *                      the logger and must be freed by using QairtLog_free().
 * @param[in] configHandle A handle to a device configuration, which is managed
 *                         from the DeviceConfig component APIs. NULL is allowed
 *                         and indicates no config options are provided. This handle may be freed
 once
 *                         the device is created.
 * @note NULL value for _configHandle_ creates a device handle with default
         configuration. Unless mentioned in backend specific headers,
         default configuration would enable all the devices and cores present on a
         platform for which a backend can control.
 *
 * @param[out] deviceHandle A handle to the created device.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_DEVICE_ERROR_INVALID_ARGUMENT: _deviceHandle_ is NULL
 *         - QAIRT_DEVICE_ERROR_INVALID_HANDLE: _logHandle_ is not a valid
 *           handle
 *         - QAIRT_DEVICE_ERROR_INVALID_CONFIG: One or more configuration
 *           values is invalid
 *         - QAIRT_DEVICE_ERROR_MEM_ALLOC: Failure in allocating memory
 *           when creating device
 *         - QAIRT_DEVICE_ERROR_HARDWARE_UNAVAILABLE: Requested hardware
 *           resources are unavailable
 *         - QAIRT_DEVICE_ERROR_UNSUPPORTED_FEATURE: API is not supported
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurence
 *           (successful recovery)
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurence
 *           (unsuccessful recovery)
 *
 * @note Use corresponding API through QairtDevice_V1_t.
 */
Qairt_Status_t QairtDevice_create(QairtLog_Handle_t logHandle,
                                       QairtDevice_ConfigHandle_t configHandle,
                                       QairtDevice_Handle_t* deviceHandle);

/**
 * @brief Free the created device and perform any deallocation of the resources
 *        allocated during device create.
 *
 * @param[in] deviceHandle A device handle.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_DEVICE_ERROR_INVALID_HANDLE: _deviceHandle_ is not a
 *           valid handle
 *         - QAIRT_DEVICE_ERROR_MEM_ALLOC: An error is encountered with
 *           de-allocation of associated memory, failure to invalidate
 *           handles or other allocated resources
 *         - QAIRT_DEVICE_ERROR_ASSOCIATED_TO_CONTEXT: One or more
 *           contexts associated with the device handle is not freed
 *         - QAIRT_DEVICE_ERROR_UNSUPPORTED_FEATURE: API is not supported
 *
 * @note Use corresponding API through QairtDevice_V1_t.
 */
Qairt_Status_t QairtDevice_free(QairtDevice_Handle_t deviceHandle);

/**
 * @brief A function to set/modify configuration options on an already created
 *        device. Backends are not required to support this API.
 *
 * @param[in] deviceHandle A device handle.
 *
 * @param[in] configHandle A handle to a device configuration, which is managed
 *                         from the DeviceConfig component APIs. NULL is allowed
 *                         and indicates no config options are provided. This handle may be freed
 * once the device is created.
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_DEVICE_ERROR_INVALID_HANDLE: _deviceHandle_ is not a
 *           valid handle
 *         - QAIRT_DEVICE_ERROR_INVALID_ARGUMENT: At least one argument is
 *           invalid
 *         - QAIRT_DEVICE_ERROR_INVALID_CONFIG: One or more configuration
 *           values is invalid
 *         - QAIRT_DEVICE_ERROR_UNSUPPORTED_FEATURE: API is not supported
 *         - QAIRT_DEVICE_ERROR_ASSOCIATED_TO_CONTEXT: _deviceHandle_ has
 *           associated contexts. Free the associations before attempting to
 *           change the config.
 *
 * @note Use corresponding API through QairtDevice_V1_t.
 */
Qairt_Status_t QairtDevice_setConfig(QairtDevice_Handle_t deviceHandle,
                                          QairtDevice_ConfigHandle_t configHandle);

/**
 * @brief A function to get configuration associated with a device handle.
 *
 * @param[in] deviceHandle A device handle.
 *
 * @param[out] configHandle A referenced handle device configuration handle.
 *                          Reference handles should not be freed. Referenced
 *                          handles are invalidated if modification operations are performed from
 * the same component API.
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_DEVICE_ERROR_INVALID_HANDLE: _deviceHandle_ is not a
 *           valid handle
 *         - QAIRT_DEVICE_ERROR_INVALID_ARGUMENT: _configHandle_ is NULL
 *         - QAIRT_DEVICE_ERROR_UNSUPPORTED_FEATURE: API is not supported
 *
 * @note Use corresponding API through QairtDevice_V1_t.
 */
Qairt_Status_t QairtDevice_getConfig(QairtDevice_Handle_t deviceHandle,
                                          QairtDevice_ConfigHandle_t* configHandle);

typedef Qairt_Status_t (*QairtDevice_CreateFn_t)(QairtLog_Handle_t,
                                                      QairtDevice_ConfigHandle_t,
                                                      QairtDevice_Handle_t*);

typedef Qairt_Status_t (*QairtDevice_FreeFn_t)(QairtDevice_Handle_t);

typedef Qairt_Status_t (*QairtDevice_SetConfigFn_t)(QairtDevice_Handle_t,
                                                         QairtDevice_ConfigHandle_t);

typedef Qairt_Status_t (*QairtDevice_GetConfigFn_t)(QairtDevice_Handle_t,
                                                         QairtDevice_ConfigHandle_t*);
//=============================================================================
// Public Interfaces
//=============================================================================
/**
 * @struct QairtDevice_V1_t
 * @brief This struct contains the functions that define the V1 interface for
 *        QAIRT Device. This interface can be retrieved through the id
 *        QAIRT_DEVICE_V1_ID.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtDevice_CreateFn_t create;
  QairtDevice_FreeFn_t free;
  QairtDevice_SetConfigFn_t setConfig;
  QairtDevice_GetConfigFn_t getConfig;
} QairtDevice_V1_t;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // QAISW_QAIRTDEVICE_H

//==============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//==============================================================================

/**
 * @file
 * @brief   Platform Info component API
 *
 *          Provides interfaces for querying hardware device and core
 *          information.
 */

#ifndef QAISW_QAIRTDEVICEPLATFORMINFO_H
#define QAISW_QAIRTDEVICEPLATFORMINFO_H

#include "QairtCommon/QairtCommon.h"
#include "QairtDeviceCommon.h"
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

typedef struct _QairtDevice_PlatformInfoProviderHandle_t*
    QairtDevice_PlatformInfoProviderHandle_t;
typedef struct _QairtDevice_PlatformInfoHandle_t* QairtDevice_PlatformInfoHandle_t;
typedef struct _QairtDevice_HardwareDeviceInfoHandle_t* QairtDevice_HardwareDeviceInfoHandle_t;
typedef struct _QairtDevice_HardwareCoreInfoHandle_t* QairtDevice_HardwareCoreInfoHandle_t;
typedef struct _QairtDevice_HardwareDeviceExtendedInfoHandle_t*
    QairtDevice_HardwareDeviceExtendedInfoHandle_t;
typedef struct _QairtDevice_HardwareCoreExtendedInfoHandle_t*
    QairtDevice_HardwareCoreExtendedInfoHandle_t;

//=============================================================================
// Public Functions
//=============================================================================
/**
 * @brief Create a platform info provider handle to query platform information.
 *
 * @param[in] logHandle A handle to the logger. Use a NULL handle to disable
 *                      logging. QairtDevice does not manage the lifecycle of
 *                      the logger and must be freed by using QairtLog_free().
 * @param[out] platformInfoProviderHandle A handle to the created platform info
 *                                        provider.
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_DEVICE_ERROR_INVALID_ARGUMENT:
 *           _platformInfoProviderHandle_ is NULL
 *         - QAIRT_DEVICE_ERROR_MEM_ALLOC: Failure in allocating memory
 *
 * @note Use corresponding API through QairtDevice_PlatformInfoProviderV1_t.
 */
Qairt_Status_t QairtDevice_PlatformInfoProvider_create(
    QairtLog_Handle_t logHandle,
    QairtDevice_PlatformInfoProviderHandle_t* platformInfoProviderHandle);

/**
 * @brief Get a platform info handle.
 *
 * @param[in] platformInfoProviderHandle A handle to the platform info provider.
 *
 * @param[in] index Index of the platform info to retrieve.
 *
 * @param[out] platformInfoHandle A handle to the platform info at the specified
 *                                index.
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_DEVICE_ERROR_INVALID_ARGUMENT: _platformInfoHandle_ is
 *           NULL
 *         - QAIRT_DEVICE_ERROR_INVALID_HANDLE:
 *           _platformInfoProviderHandle_ is not a valid handle
 *         - QAIRT_DEVICE_ERROR_OUT_OF_RANGE: _index_ is out of range
 *
 * @note Use corresponding API through QairtDevice_PlatformInfoProviderV1_t.
 */
Qairt_Status_t QairtDevice_PlatformInfoProvider_getPlatformInfo(
    QairtDevice_PlatformInfoProviderHandle_t platformInfoProviderHandle,
    QairtDevice_PlatformInfoHandle_t* platformInfoHandle);

/**
 * @brief Free the platform info provider handle and associated resources.
 *
 * @param[in] platformInfoProviderHandle A handle to the platform info provider.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_DEVICE_ERROR_INVALID_HANDLE:
 *           _platformInfoProviderHandle_ is not a valid handle
 *         - QAIRT_DEVICE_ERROR_MEM_ALLOC: Error in de-allocating memory
 *
 * @note Use corresponding API through QairtDevice_PlatformInfoProviderV1_t.
 */
Qairt_Status_t QairtDevice_PlatformInfoProvider_free(
    QairtDevice_PlatformInfoProviderHandle_t platformInfoProviderHandle);

/**
 * @brief Get the number of hardware device info entries in the platform info.
 *
 * @param[in] platformInfoHandle A handle to the platform info.
 *
 * @param[out] numHwDeviceInfo Number of hardware device info entries.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_DEVICE_ERROR_INVALID_ARGUMENT: _numHwDeviceInfo_ is
 *           NULL
 *         - QAIRT_DEVICE_ERROR_INVALID_HANDLE: _platformInfoHandle_ is
 *           not a valid handle
 *
 * @note Use corresponding API through QairtDevice_PlatformInfoV1_t.
 */
Qairt_Status_t QairtDevice_PlatformInfo_getNumHardwareDeviceInfo(
    QairtDevice_PlatformInfoHandle_t platformInfoHandle, uint32_t* numHwDeviceInfo);

/**
 * @brief Get a hardware device info handle at the specified index.
 *
 * @param[in] platformInfoHandle A handle to the platform info.
 *
 * @param[in] index Index of the hardware device info to retrieve.
 *
 * @param[out] hwDeviceInfoHandle A handle to the hardware device info at the
 *                                specified index.
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_DEVICE_ERROR_INVALID_ARGUMENT: _hwDeviceInfoHandle_ is
 *           NULL
 *         - QAIRT_DEVICE_ERROR_INVALID_HANDLE: _platformInfoHandle_ is
 *           not a valid handle
 *         - QAIRT_DEVICE_ERROR_OUT_OF_RANGE: _index_ is out of range
 *
 * @note Use corresponding API through QairtDevice_PlatformInfoV1_t.
 */
Qairt_Status_t QairtDevice_PlatformInfo_getHardwareDeviceInfoAt(
    QairtDevice_PlatformInfoHandle_t platformInfoHandle,
    uint32_t index,
    QairtDevice_HardwareDeviceInfoHandle_t* hwDeviceInfoHandle);

/**
 * @brief Get the device ID from hardware device info.
 *
 * @param[in] hwDeviceInfoHandle A handle to the hardware device info.
 *
 * @param[out] deviceId The device ID.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_DEVICE_ERROR_INVALID_ARGUMENT: _deviceId_ is NULL
 *         - QAIRT_DEVICE_ERROR_INVALID_HANDLE: _hwDeviceInfoHandle_ is
 *           not a valid handle
 *
 * @note Use corresponding API through QairtDevice_HardwareDeviceInfoV1_t.
 */
Qairt_Status_t QairtDevice_HardwareDeviceInfo_getDeviceId(
    QairtDevice_HardwareDeviceInfoHandle_t hwDeviceInfoHandle, uint32_t* deviceId);

/**
 * @brief Get the device type from hardware device info.
 *
 * @param[in] hwDeviceInfoHandle A handle to the hardware device info.
 *
 * @param[out] deviceType The device type.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_DEVICE_ERROR_INVALID_ARGUMENT: _deviceType_ is NULL
 *         - QAIRT_DEVICE_ERROR_INVALID_HANDLE: _hwDeviceInfoHandle_ is
 *           not a valid handle
 *
 * @note Use corresponding API through QairtDevice_HardwareDeviceInfoV1_t.
 */
Qairt_Status_t QairtDevice_HardwareDeviceInfo_getDeviceType(
    QairtDevice_HardwareDeviceInfoHandle_t hwDeviceInfoHandle, uint32_t* deviceType);

/**
 * @brief Get the number of core info entries in the hardware device info.
 *
 * @param[in] hwDeviceInfoHandle A handle to the hardware device info.
 *
 * @param[out] numCores Number of core info entries.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_DEVICE_ERROR_INVALID_ARGUMENT: _numCores_ is NULL
 *         - QAIRT_DEVICE_ERROR_INVALID_HANDLE: _hwDeviceInfoHandle_ is
 *           not a valid handle
 *
 * @note Use corresponding API through QairtDevice_HardwareDeviceInfoV1_t.
 */
Qairt_Status_t QairtDevice_HardwareDeviceInfo_getNumCoreInfos(
    QairtDevice_HardwareDeviceInfoHandle_t hwDeviceInfoHandle, uint32_t* numCores);

/**
 * @brief Get a hardware core info handle at the specified index.
 *
 * @param[in] hwDeviceInfoHandle A handle to the hardware device info.
 *
 * @param[in] index Index of the hardware core info to retrieve.
 *
 * @param[out] hwCoreInfoHandle A referenced hardware core info handle at the
 *                              specified index. Reference handles should not be
 *                              freed. Referenced handles are invalidated if modification operations
 * are performed from the same component API.
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_DEVICE_ERROR_INVALID_ARGUMENT: _hwCoreInfoHandle_ is
 *           NULL
 *         - QAIRT_DEVICE_ERROR_INVALID_HANDLE: _hwDeviceInfoHandle_ is
 *           not a valid handle
 *         - QAIRT_DEVICE_ERROR_OUT_OF_RANGE: _index_ is out of range
 *
 * @note Use corresponding API through QairtDevice_HardwareDeviceInfoV1_t.
 */
Qairt_Status_t QairtDevice_HardwareDeviceInfo_getHardwareCoreInfoAt(
    QairtDevice_HardwareDeviceInfoHandle_t hwDeviceInfoHandle,
    uint32_t index,
    QairtDevice_HardwareCoreInfoHandle_t* hwCoreInfoHandle);

/**
 * @brief Get the hardware device extended info handle.
 *
 * @param[in] hwDeviceInfoHandle A handle to the hardware device info.
 *
 * @param[out] extHwDeviceInfoHandle A referenced hardware device extended info
 *                                   handle. Reference handles should not be
 *                                   freed. Referenced handles are invalidated if modification
 * operations are performed from the same component API.
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_DEVICE_ERROR_INVALID_ARGUMENT: _extHwDeviceInfoHandle_
 *           is NULL
 *         - QAIRT_DEVICE_ERROR_INVALID_HANDLE: _hwDeviceInfoHandle_ is
 *           not a valid handle
 *         - QAIRT_DEVICE_ERROR_UNSUPPORTED_FEATURE: Extended info is not
 *           supported
 *
 * @note Use corresponding API through QairtDevice_HardwareDeviceInfoV1_t.
 */
Qairt_Status_t QairtDevice_HardwareDeviceInfo_getHardwareDeviceExtendedInfo(
    QairtDevice_HardwareDeviceInfoHandle_t hwDeviceInfoHandle,
    QairtDevice_HardwareDeviceExtendedInfoHandle_t* extHwDeviceInfoHandle);

/**
 * @brief Get the core ID from hardware core info.
 *
 * @param[in] hwCoreInfoHandle A handle to the hardware core info.
 *
 * @param[out] coreId The core ID.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_DEVICE_ERROR_INVALID_ARGUMENT: _coreId_ is NULL
 *         - QAIRT_DEVICE_ERROR_INVALID_HANDLE: _hwCoreInfoHandle_ is not
 *           a valid handle
 *
 * @note Use corresponding API through QairtDevice_HardwareCoreInfoV1_t.
 */
Qairt_Status_t QairtDevice_HardwareCoreInfo_getCoreId(
    QairtDevice_HardwareCoreInfoHandle_t hwCoreInfoHandle, uint32_t* coreId);

/**
 * @brief Get the core type from hardware core info.
 *
 * @param[in] hwCoreInfoHandle A handle to the hardware core info.
 *
 * @param[out] coreType The core type.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_DEVICE_ERROR_INVALID_ARGUMENT: _coreType_ is NULL
 *         - QAIRT_DEVICE_ERROR_INVALID_HANDLE: _hwCoreInfoHandle_ is not
 *           a valid handle
 *
 * @note Use corresponding API through QairtDevice_HardwareCoreInfoV1_t.
 */
Qairt_Status_t QairtDevice_HardwareCoreInfo_getCoreType(
    QairtDevice_HardwareCoreInfoHandle_t hwCoreInfoHandle, uint32_t* coreType);

/**
 * @brief Get the hardware core extended info handle.
 *
 * @param[in] hwCoreInfoHandle A handle to the hardware core info.
 *
 * @param[out] extHwCoreInfoHandle A referenced extended hardware info handle.
 *                                 Reference handles should not be freed.
 *                                 Referenced handles are invalidated if
 *                                 modification operations are performed from
 *                                 the same component API.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_DEVICE_ERROR_INVALID_ARGUMENT: _extHwCoreInfoHandle_ is
 *           NULL
 *         - QAIRT_DEVICE_ERROR_INVALID_HANDLE: _hwCoreInfoHandle_ is not
 *           a valid handle
 *         - QAIRT_DEVICE_ERROR_UNSUPPORTED_FEATURE: Extended info is not
 *           supported
 *
 * @note Use corresponding API through QairtDevice_HardwareCoreInfoV1_t.
 */
Qairt_Status_t QairtDevice_HardwareCoreInfo_getHardwareCoreExtendedInfo(
    QairtDevice_HardwareCoreInfoHandle_t hwCoreInfoHandle,
    QairtDevice_HardwareDeviceExtendedInfoHandle_t* extHwCoreInfoHandle);

// PlatformInfoProvider
typedef Qairt_Status_t (*QairtDevice_PlatformInfoProvider_CreateFn_t)(
    QairtLog_Handle_t, QairtDevice_PlatformInfoProviderHandle_t*);
typedef Qairt_Status_t (*QairtDevice_PlatformInfoProvider_GetPlatformInfoFn_t)(
    QairtDevice_PlatformInfoProviderHandle_t, QairtDevice_PlatformInfoHandle_t*);
typedef Qairt_Status_t (*QairtDevice_PlatformInfoProvider_FreeFn_t)(
    QairtDevice_PlatformInfoProviderHandle_t);

typedef Qairt_Status_t (*QairtDevice_PlatformInfo_GetNumHardwareDeviceInfoFn_t)(
    QairtDevice_PlatformInfoHandle_t, uint32_t*);
typedef Qairt_Status_t (*QairtDevice_PlatformInfo_GetNumHardwareDeviceInfoAtFn_t)(
    QairtDevice_PlatformInfoHandle_t, uint32_t, QairtDevice_HardwareDeviceInfoHandle_t*);

typedef Qairt_Status_t (*QairtDevice_HardwareDeviceInfo_GetDeviceIdFn_t)(
    QairtDevice_HardwareDeviceInfoHandle_t, uint32_t*);
typedef Qairt_Status_t (*QairtDevice_HardwareDeviceInfo_GetDeviceTypeFn_t)(
    QairtDevice_HardwareDeviceInfoHandle_t, uint32_t*);
typedef Qairt_Status_t (*QairtDevice_HardwareDeviceInfo_GetNumCoreInfosFn_t)(
    QairtDevice_HardwareDeviceInfoHandle_t, uint32_t*);
typedef Qairt_Status_t (*QairtDevice_HardwareDeviceInfo_GetHardwareCoreInfoAtFn_t)(
    QairtDevice_HardwareDeviceInfoHandle_t, uint32_t, QairtDevice_HardwareCoreInfoHandle_t*);
typedef Qairt_Status_t (*QairtDevice_HardwareDeviceInfo_GetHardwareDeviceExtendedInfoFn_t)(
    QairtDevice_HardwareDeviceInfoHandle_t, QairtDevice_HardwareDeviceExtendedInfoHandle_t*);

typedef Qairt_Status_t (*QairtDevice_HardwareCoreInfo_GetCoreIdFn_t)(
    QairtDevice_HardwareCoreInfoHandle_t, uint32_t*);
typedef Qairt_Status_t (*QairtDevice_HardwareCoreInfo_GetCoreTypeFn_t)(
    QairtDevice_HardwareCoreInfoHandle_t, uint32_t*);
typedef Qairt_Status_t (*QairtDevice_HardwareCoreInfo_GetHardwareCoreExtendedInfoFn_t)(
    QairtDevice_HardwareCoreInfoHandle_t, QairtDevice_HardwareDeviceExtendedInfoHandle_t*);

//=============================================================================
// Public Interfaces
//=============================================================================
/**
 * @struct QairtDevice_PlatformInfoProviderV1_t
 * @brief This struct contains the functions that define the V1 interface for
 *        QAIRT Platform Info Provider. This interface can be retrieved
 *        through the id QAIRT_DEVICE_PLATFORM_INFO_PROVIDER_V1_ID.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtDevice_PlatformInfoProvider_CreateFn_t create;
  QairtDevice_PlatformInfoProvider_GetPlatformInfoFn_t getPlatformInfo;
  QairtDevice_PlatformInfoProvider_FreeFn_t free;
} QairtDevice_PlatformInfoProviderV1_t;

/**
 * @struct QairtDevice_PlatformInfoV1_t
 * @brief This struct contains the functions that define the V1 interface for
 *        QAIRT Platform Info. This interface can be retrieved through the
 *        id QAIRT_DEVICE_PLATFORM_INFO_V1_ID.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtDevice_PlatformInfo_GetNumHardwareDeviceInfoFn_t getNumHwDeviceInfo;
  QairtDevice_PlatformInfo_GetNumHardwareDeviceInfoAtFn_t getHwDeviceInfoAt;
} QairtDevice_PlatformInfoV1_t;

/**
 * @struct QairtDevice_HardwareDeviceInfoV1_t
 * @brief This struct contains the functions that define the V1 interface for
 *        QAIRT Hardware Device Info. This interface can be retrieved
 *        through the id QAIRT_DEVICE_HW_DEVICE_INFO_V1_ID.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtDevice_HardwareDeviceInfo_GetDeviceIdFn_t getDeviceId;
  QairtDevice_HardwareDeviceInfo_GetDeviceTypeFn_t getDeviceType;
  QairtDevice_HardwareDeviceInfo_GetNumCoreInfosFn_t getNumCoreInfos;
  QairtDevice_HardwareDeviceInfo_GetHardwareCoreInfoAtFn_t getHardwareCoreInfoAt;
  QairtDevice_HardwareDeviceInfo_GetHardwareDeviceExtendedInfoFn_t getHardwareDeviceExtendedInfo;
} QairtDevice_HardwareDeviceInfoV1_t;

/**
 * @struct QairtDevice_HardwareCoreInfoV1_t
 * @brief This struct contains the functions that define the V1 interface for
 *        QAIRT Hardware Core Info. This interface can be retrieved through
 *        the id QAIRT_DEVICE_HW_CORE_INFO_V1_ID.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtDevice_HardwareCoreInfo_GetCoreIdFn_t getCoreId;
  QairtDevice_HardwareCoreInfo_GetCoreTypeFn_t getCoreType;
  QairtDevice_HardwareCoreInfo_GetHardwareCoreExtendedInfoFn_t getHardwareCoreExtendedInfo;
} QairtDevice_HardwareCoreInfoV1_t;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // QAISW_QAIRTDEVICEPLATFORMINFO_H

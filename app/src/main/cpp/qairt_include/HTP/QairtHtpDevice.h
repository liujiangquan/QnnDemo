//=============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//=============================================================================

#ifndef QAIRT_HTP_DEVICE_H
#define QAIRT_HTP_DEVICE_H

#ifdef __cplusplus
#include <cstdint>
#else
#include <stdint.h>
#endif

#include "QairtBackend/QairtBackendConfig.h"
#include "QairtCommon/QairtCommon.h"

//=============================================================================
// Macros
//=============================================================================
#define QAIRT_MIN_ID_HTP_DEVICE (QAIRT_MIN_ID_HTP + 200)

#define QAIRT_HTP_DEVICE_CUSTOM_CONFIG_ID (QAIRT_MIN_ID_HTP_DEVICE + 0)

// Define ranges for HTP device-specific error codes
#define QAIRT_MIN_ERROR_HTP_DEVICE (QAIRT_MIN_ERROR_HTP + 2000)
#define QAIRT_MAX_ERROR_HTP_DEVICE (QAIRT_MIN_ERROR_HTP + 2999)

//=============================================================================
// Data Types
//=============================================================================

typedef QairtBackend_CustomConfigHandle_t QairtHtpDevice_CustomConfigHandle_t;

/**
 * @brief HTP device architecture enum, mirroring QnnHtpDevice_Arch_t.
 */
typedef enum {
  QAIRT_HTP_DEVICE_ARCH_NONE    = 0,
  QAIRT_HTP_DEVICE_ARCH_V68     = 68,
  QAIRT_HTP_DEVICE_ARCH_V69     = 69,
  QAIRT_HTP_DEVICE_ARCH_V73     = 73,
  QAIRT_HTP_DEVICE_ARCH_V75     = 75,
  QAIRT_HTP_DEVICE_ARCH_V79     = 79,
  QAIRT_HTP_DEVICE_ARCH_V81     = 81,
  QAIRT_HTP_DEVICE_ARCH_V85     = 85,
  QAIRT_HTP_DEVICE_ARCH_V89     = 89,
  QAIRT_HTP_DEVICE_ARCH_UNKNOWN = 0x7fffffff
} QairtHtpDevice_Arch_t;

/**
 * @brief QAIRT HTP Device API result / error codes.
 */
typedef enum {
  QAIRT_HTP_DEVICE_MIN_ERROR = QAIRT_MIN_ERROR_HTP_DEVICE,
  ////////////////////////////////////////

  /// HTP device operation succeeded.
  QAIRT_HTP_DEVICE_NO_ERROR = QAIRT_SUCCESS,
  /// General memory allocation failure.
  QAIRT_HTP_DEVICE_ERROR_MEM_ALLOC = QAIRT_COMMON_ERROR_MEM_ALLOC,
  /// Feature or API is not supported.
  QAIRT_HTP_DEVICE_ERROR_UNSUPPORTED_FEATURE = QAIRT_COMMON_ERROR_NOT_SUPPORTED,
  /// General error not covered by a more specific code.
  QAIRT_HTP_DEVICE_ERROR_GENERAL = QAIRT_COMMON_ERROR_GENERAL,
  /// An argument to the API is invalid.
  QAIRT_HTP_DEVICE_ERROR_INVALID_ARGUMENT = QAIRT_MIN_ERROR_HTP_DEVICE + 0,
  /// A handle argument is not valid.
  QAIRT_HTP_DEVICE_ERROR_INVALID_HANDLE = QAIRT_MIN_ERROR_HTP_DEVICE + 1,
  /// A configuration option mismatch was detected on get.
  QAIRT_HTP_DEVICE_ERROR_OPTION_MISMATCH = QAIRT_MIN_ERROR_HTP_DEVICE + 2,

  ////////////////////////////////////////
  QAIRT_HTP_DEVICE_MAX_ERROR = QAIRT_MAX_ERROR_HTP_DEVICE,
  // Unused, present to ensure 32 bits.
  QAIRT_HTP_DEVICE_ERROR_UNDEFINED = 0x7FFFFFFF
} QairtHtpDevice_Error_t;

//=============================================================================
// Public Functions
//=============================================================================

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create a HTP device custom configuration handle.
 *
 * @param[out] customConfig A pointer to receive the created handle.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Handle was successfully created.
 *         - QAIRT_HTP_DEVICE_ERROR_INVALID_ARGUMENT: customConfig is NULL.
 *         - QAIRT_HTP_DEVICE_ERROR_MEM_ALLOC: Memory allocation failed.
 *
 * @note Use corresponding API through QairtHtpDevice_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpDevice_CustomConfig_create(
    QairtHtpDevice_CustomConfigHandle_t* customConfig);

/**
 * @brief Free a HTP device custom configuration handle.
 *
 * @param[in] customConfig The handle to free.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Handle was successfully freed.
 *         - QAIRT_HTP_DEVICE_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *
 * @note Use corresponding API through QairtHtpDevice_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpDevice_CustomConfig_free(
    QairtHtpDevice_CustomConfigHandle_t customConfig);

// SoC Model
/**
 * @brief Set the SoC model for the HTP device.
 *
 * @param[in] customConfig Handle to the device custom config object.
 * @param[in] socModel     The SoC model identifier to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Value was successfully set.
 *         - QAIRT_HTP_DEVICE_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *
 * @note Use corresponding API through QairtHtpDevice_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpDevice_CustomConfig_setSocModel(
    QairtHtpDevice_CustomConfigHandle_t customConfig, uint32_t socModel);

/**
 * @brief Get the SoC model from the HTP device custom config.
 *
 * @param[in]  customConfig Handle to the device custom config object.
 * @param[out] socModel     Pointer to receive the SoC model identifier.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Value was successfully retrieved.
 *         - QAIRT_HTP_DEVICE_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *         - QAIRT_HTP_DEVICE_ERROR_INVALID_ARGUMENT: socModel is NULL or option mismatch.
 *
 * @note Use corresponding API through QairtHtpDevice_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpDevice_CustomConfig_getSocModel(
    QairtHtpDevice_CustomConfigHandle_t customConfig, uint32_t* socModel);

// Minimum Architecture
/**
 * @brief Set the minimum required HTP architecture for a given device.
 *
 * @param[in] customConfig The handle to the device custom config.
 * @param[in] deviceId     The device ID to associate with the minimum arch requirement.
 * @param[in] arch         The minimum required architecture.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Value was successfully set.
 *         - QAIRT_HTP_DEVICE_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *
 * @note Use corresponding API through QairtHtpDevice_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpDevice_CustomConfig_setArch(
    QairtHtpDevice_CustomConfigHandle_t customConfig,
    uint32_t deviceId,
    QairtHtpDevice_Arch_t arch);

/**
 * @brief Get the minimum required HTP architecture.
 *
 * @param[in]  customConfig The handle to the device custom config.
 * @param[out] deviceId     Pointer to receive the device ID.
 * @param[out] arch         Pointer to receive the minimum required architecture.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Value was successfully retrieved.
 *         - QAIRT_HTP_DEVICE_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *         - QAIRT_HTP_DEVICE_ERROR_INVALID_ARGUMENT: deviceId or arch is NULL, or option mismatch.
 *
 * @note Use corresponding API through QairtHtpDevice_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpDevice_CustomConfig_getArch(
    QairtHtpDevice_CustomConfigHandle_t customConfig,
    uint32_t* deviceId,
    QairtHtpDevice_Arch_t* arch);

// Use Signed Process Domain
/**
 * @brief Set whether to use the signed process domain for a given device.
 *
 * @param[in] customConfig           The handle to the device custom config.
 * @param[in] deviceId               The device ID.
 * @param[in] useSignedProcessDomain Non-zero to enable; zero to disable.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Value was successfully set.
 *         - QAIRT_HTP_DEVICE_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *
 * @note Use corresponding API through QairtHtpDevice_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpDevice_CustomConfig_setUseSignedProcessDomain(
    QairtHtpDevice_CustomConfigHandle_t customConfig,
    uint32_t deviceId,
    uint8_t useSignedProcessDomain);

/**
 * @brief Get the signed process domain setting.
 *
 * @param[in]  customConfig           The handle to the device custom config.
 * @param[out] deviceId               Pointer to receive the device ID.
 * @param[out] useSignedProcessDomain Pointer to receive the setting.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Value was successfully retrieved.
 *         - QAIRT_HTP_DEVICE_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *         - QAIRT_HTP_DEVICE_ERROR_INVALID_ARGUMENT: deviceId or useSignedProcessDomain is NULL,
 *           or option mismatch.
 *
 * @note Use corresponding API through QairtHtpDevice_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpDevice_CustomConfig_getUseSignedProcessDomain(
    QairtHtpDevice_CustomConfigHandle_t customConfig,
    uint32_t* deviceId,
    uint8_t* useSignedProcessDomain);

// Use Secure Process Domain
/**
 * @brief Set whether to use the secure process domain for a given device.
 *
 * @param[in] customConfig            The handle to the device custom config.
 * @param[in] deviceId                The device ID.
 * @param[in] useSecureProcessDomain  Non-zero to enable; zero to disable.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Value was successfully set.
 *         - QAIRT_HTP_DEVICE_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *
 * @note Use corresponding API through QairtHtpDevice_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpDevice_CustomConfig_setUseSecureProcessDomain(
    QairtHtpDevice_CustomConfigHandle_t customConfig,
    uint32_t deviceId,
    uint8_t useSecureProcessDomain);

/**
 * @brief Get the secure process domain setting.
 *
 * @param[in]  customConfig            The handle to the device custom config.
 * @param[out] deviceId                Pointer to receive the device ID.
 * @param[out] useSecureProcessDomain  Pointer to receive the setting.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Value was successfully retrieved.
 *         - QAIRT_HTP_DEVICE_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *         - QAIRT_HTP_DEVICE_ERROR_INVALID_ARGUMENT: deviceId or useSecureProcessDomain is NULL,
 *           or option mismatch.
 *
 * @note Use corresponding API through QairtHtpDevice_CustomConfigV1_t.
 */
Qairt_Status_t QairtHtpDevice_CustomConfig_getUseSecureProcessDomain(
    QairtHtpDevice_CustomConfigHandle_t customConfig,
    uint32_t* deviceId,
    uint8_t* useSecureProcessDomain);

//=============================================================================
// Function Pointer Typedefs
//=============================================================================

typedef Qairt_Status_t (*QairtHtpDevice_CustomConfig_CreateFn_t)(
    QairtHtpDevice_CustomConfigHandle_t*);
typedef Qairt_Status_t (*QairtHtpDevice_CustomConfig_FreeFn_t)(
    QairtHtpDevice_CustomConfigHandle_t);

/* SoC Model */
typedef Qairt_Status_t (*QairtHtpDevice_CustomConfig_SetSocModelFn_t)(
    QairtHtpDevice_CustomConfigHandle_t, uint32_t);
typedef Qairt_Status_t (*QairtHtpDevice_CustomConfig_GetSocModelFn_t)(
    QairtHtpDevice_CustomConfigHandle_t, uint32_t*);

/* Minimum Architecture */
typedef Qairt_Status_t (*QairtHtpDevice_CustomConfig_SetArchFn_t)(
    QairtHtpDevice_CustomConfigHandle_t, uint32_t, QairtHtpDevice_Arch_t);
typedef Qairt_Status_t (*QairtHtpDevice_CustomConfig_GetArchFn_t)(
    QairtHtpDevice_CustomConfigHandle_t, uint32_t*, QairtHtpDevice_Arch_t*);

/* Use Signed Process Domain */
typedef Qairt_Status_t (*QairtHtpDevice_CustomConfig_SetUseSignedProcessDomainFn_t)(
    QairtHtpDevice_CustomConfigHandle_t, uint32_t, uint8_t);
typedef Qairt_Status_t (*QairtHtpDevice_CustomConfig_GetUseSignedProcessDomainFn_t)(
    QairtHtpDevice_CustomConfigHandle_t, uint32_t*, uint8_t*);

/* Use Secure Process Domain */
typedef Qairt_Status_t (*QairtHtpDevice_CustomConfig_SetUseSecureProcessDomainFn_t)(
    QairtHtpDevice_CustomConfigHandle_t, uint32_t, uint8_t);
typedef Qairt_Status_t (*QairtHtpDevice_CustomConfig_GetUseSecureProcessDomainFn_t)(
    QairtHtpDevice_CustomConfigHandle_t, uint32_t*, uint8_t*);

//=============================================================================
// Public Interfaces
//=============================================================================

/**
 * @struct QairtHtpDevice_CustomConfigV1_t
 * @brief Interface struct for HTP device custom configuration (V1).
 *        Retrieved via interface ID QAIRT_HTP_DEVICE_CUSTOM_CONFIG_ID.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtHtpDevice_CustomConfig_CreateFn_t create;
  QairtHtpDevice_CustomConfig_FreeFn_t free;
  QairtHtpDevice_CustomConfig_SetSocModelFn_t setSocModel;
  QairtHtpDevice_CustomConfig_GetSocModelFn_t getSocModel;
  QairtHtpDevice_CustomConfig_SetArchFn_t setArch;
  QairtHtpDevice_CustomConfig_GetArchFn_t getArch;
  QairtHtpDevice_CustomConfig_SetUseSignedProcessDomainFn_t setUseSignedProcessDomain;
  QairtHtpDevice_CustomConfig_GetUseSignedProcessDomainFn_t getUseSignedProcessDomain;
  QairtHtpDevice_CustomConfig_SetUseSecureProcessDomainFn_t setUseSecureProcessDomain;
  QairtHtpDevice_CustomConfig_GetUseSecureProcessDomainFn_t getUseSecureProcessDomain;
} QairtHtpDevice_CustomConfigV1_t;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // QAIRT_HTP_DEVICE_H

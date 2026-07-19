//=============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//=============================================================================

/** @file
 *  @brief QAIRT LPAI Backend API specialization.

 */

#ifndef QAIRT_LPAI_BACKEND_H
#define QAIRT_LPAI_BACKEND_H

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

/// Base ID for all LPAI backend interface IDs.
#define QAIRT_MIN_ID_LPAI_BACKEND (QAIRT_MIN_ID_LPAI + 1000)

/// Interface ID for QairtLpaiBackend_HwInfoV1_t.
#define QAIRT_LPAI_BACKEND_HW_INFO_ID (QAIRT_MIN_ID_LPAI_BACKEND + 0)

/// Interface ID for QairtLpaiBackend_CustomConfigV1_t.
#define QAIRT_LPAI_BACKEND_CUSTOM_CONFIG_ID (QAIRT_MIN_ID_LPAI_BACKEND + 1)

//=============================================================================
// Data Types
//=============================================================================

/**
 * @brief Handle for an LPAI backend HW info configuration object.
 */
typedef struct _QairtLpaiBackend_HwInfoHandle_t* QairtLpaiBackend_HwInfoHandle_t;

/**
 * @brief Handle for an LPAI backend custom configuration object.
 */
typedef QairtBackend_CustomConfigHandle_t QairtLpaiBackend_CustomConfigHandle_t;

/**
 * @brief Compilation target for the LPAI backend.
 */
typedef enum {
  /// LPAI model will be compiled for x86
  QAIRT_LPAI_BACKEND_TARGET_X86 = 0,
  /// LPAI model will be compiled for ARM
  QAIRT_LPAI_BACKEND_TARGET_ARM = 1,
  /// LPAI model will be compiled for ADSP
  QAIRT_LPAI_BACKEND_TARGET_ADSP = 2,
  /// LPAI model will be compiled for TENSILICA
  QAIRT_LPAI_BACKEND_TARGET_TENSILICA = 3,
  // Unused, present to ensure 32 bits.
  QAIRT_LPAI_BACKEND_TARGET_UNDEFINED = 0x7FFFFFFF
} QairtLpaiBackend_Target_t;

/**
 * @brief LPAI hardware version.
 */
typedef enum {
  /// No LPAI HW will be used
  QAIRT_LPAI_BACKEND_HW_VERSION_NA = 0,
  /// LPAI HW version v1
  QAIRT_LPAI_BACKEND_HW_VERSION_V1 = 0x00000001,
  /// LPAI HW version v2
  QAIRT_LPAI_BACKEND_HW_VERSION_V2 = 0x00000002,
  /// LPAI HW version v3
  QAIRT_LPAI_BACKEND_HW_VERSION_V3 = 0x00000003,
  /// LPAI HW version v4
  QAIRT_LPAI_BACKEND_HW_VERSION_V4 = 0x00000004,
  /// LPAI HW version v5
  QAIRT_LPAI_BACKEND_HW_VERSION_V5 = 0x00000005,
  /// LPAI HW version v5.1
  QAIRT_LPAI_BACKEND_HW_VERSION_V5_1 = 0x00010005,
  /// LPAI HW version v6
  QAIRT_LPAI_BACKEND_HW_VERSION_V6 = 0x00000006,
  /// LPAI HW version v7
  QAIRT_LPAI_BACKEND_HW_VERSION_V7 = 0x00000007,
  /// LPAI HW default version v7
  QAIRT_LPAI_BACKEND_HW_VERSION_DEFAULT = QAIRT_LPAI_BACKEND_HW_VERSION_V7,
  // Unused, present to ensure 32 bits.
  QAIRT_LPAI_BACKEND_HW_VERSION_UNDEFINED = 0x7FFFFFFF
} QairtLpaiBackend_HwVersion_t;

//=============================================================================
// Public Functions — HW Info
//=============================================================================

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create an LPAI backend HW info handle.
 *
 * @param[out] hwInfo Receives the newly created handle on success.
 *                    Must not be NULL.
 *
 * @return QAIRT_SUCCESS on success.
 *         QAIRT_COMMON_ERROR_INVALID_ARGUMENT if hwInfo is NULL.
 *         QAIRT_COMMON_ERROR_MEM_ALLOC if allocation fails.
 *
 * @note Use corresponding API through QairtLpaiBackend_HwInfoV1_t.
 */
Qairt_Status_t QairtLpaiBackend_HwInfo_create(QairtLpaiBackend_HwInfoHandle_t* hwInfo);

/**
 * @brief Free an LPAI backend HW info handle.
 *
 * @param[in] hwInfo The handle to free.
 *
 * @return QAIRT_SUCCESS on success.
 *         QAIRT_COMMON_ERROR_INVALID_HANDLE if hwInfo is NULL or not valid.
 *
 * @note Use corresponding API through QairtLpaiBackend_HwInfoV1_t.
 */
Qairt_Status_t QairtLpaiBackend_HwInfo_free(QairtLpaiBackend_HwInfoHandle_t hwInfo);

/**
 * @brief Set the compilation target on an HW info handle.
 *
 * @param[in] hwInfo The handle to set the configuration on.
 *
 * @param[in] target The desired compilation target.
 *
 * @return QAIRT_SUCCESS on success.
 *         QAIRT_COMMON_ERROR_INVALID_HANDLE if hwInfo is not valid.
 *
 * @note Use corresponding API through QairtLpaiBackend_HwInfoV1_t.
 */
Qairt_Status_t QairtLpaiBackend_HwInfo_setTarget(QairtLpaiBackend_HwInfoHandle_t hwInfo,
                                                      QairtLpaiBackend_Target_t target);

/**
 * @brief Get the compilation target from an HW info handle.
 *
 * @param[in] hwInfo The handle to get the configuration from.
 *
 * @param[out] target Receives the currently configured target.
 *                    Must not be NULL.
 *
 * @return QAIRT_SUCCESS on success.
 *         QAIRT_COMMON_ERROR_INVALID_HANDLE if hwInfo is not valid.
 *         QAIRT_COMMON_ERROR_INVALID_ARGUMENT if target is NULL.
 *
 * @note Use corresponding API through QairtLpaiBackend_HwInfoV1_t.
 */
Qairt_Status_t QairtLpaiBackend_HwInfo_getTarget(QairtLpaiBackend_HwInfoHandle_t hwInfo,
                                                      QairtLpaiBackend_Target_t* target);

/**
 * @brief Set the hardware version on an HW info handle.
 *
 * @param[in] hwInfo The handle to set the configuration on.
 *
 * @param[in] hwVersion The desired hardware version.
 *
 * @return QAIRT_SUCCESS on success.
 *         QAIRT_COMMON_ERROR_INVALID_HANDLE if hwInfo is not valid.
 *
 * @note Use corresponding API through QairtLpaiBackend_HwInfoV1_t.
 */
Qairt_Status_t QairtLpaiBackend_HwInfo_setHwVersion(QairtLpaiBackend_HwInfoHandle_t hwInfo,
                                                         QairtLpaiBackend_HwVersion_t hwVersion);

/**
 * @brief Get the hardware version from an HW info handle.
 *
 * @param[in] hwInfo The handle to get the configuration from.
 *
 * @param[out] hwVersion Receives the currently configured hardware version.
 *                       Must not be NULL.
 *
 * @return QAIRT_SUCCESS on success.
 *         QAIRT_COMMON_ERROR_INVALID_HANDLE if hwInfo is not valid.
 *         QAIRT_COMMON_ERROR_INVALID_ARGUMENT if hwVersion is NULL.
 *
 * @note Use corresponding API through QairtLpaiBackend_HwInfoV1_t.
 */
Qairt_Status_t QairtLpaiBackend_HwInfo_getHwVersion(QairtLpaiBackend_HwInfoHandle_t hwInfo,
                                                         QairtLpaiBackend_HwVersion_t* hwVersion);

//=============================================================================
// Public Functions — Backend Custom Config
//=============================================================================

/**
 * @brief Create an LPAI backend custom configuration handle.
 *
 * @param[out] customConfig Receives the newly created handle on success.
 *                          Must not be NULL.
 *
 * @return QAIRT_SUCCESS on success.
 *         QAIRT_COMMON_ERROR_INVALID_ARGUMENT if customConfig is NULL.
 *         QAIRT_COMMON_ERROR_MEM_ALLOC if allocation fails.
 *
 * @note Use corresponding API through QairtLpaiBackend_CustomConfigV1_t.
 */
Qairt_Status_t QairtLpaiBackend_CustomConfig_create(
    QairtLpaiBackend_CustomConfigHandle_t* customConfig);

/**
 * @brief Free an LPAI backend custom configuration handle.
 *
 * @param[in] customConfig The handle to free.
 *
 * @return QAIRT_SUCCESS on success.
 *         QAIRT_COMMON_ERROR_INVALID_HANDLE if customConfig is NULL or not valid.
 *
 * @note Use corresponding API through QairtLpaiBackend_CustomConfigV1_t.
 */
Qairt_Status_t QairtLpaiBackend_CustomConfig_free(
    QairtLpaiBackend_CustomConfigHandle_t customConfig);

/**
 * @brief Associate an HW info handle with a backend custom config.
 *
 *        The HW info handle is copied by value into the custom config.
 *        The caller retains ownership of hwInfo and may free it after
 *        this call.
 *
 * @param[in] customConfig The handle to set the configuration on.
 *
 * @param[in] hwInfo A valid, populated HW info handle.
 *
 * @return QAIRT_SUCCESS on success.
 *         QAIRT_COMMON_ERROR_INVALID_HANDLE if either handle is not valid.
 *
 * @note Use corresponding API through QairtLpaiBackend_CustomConfigV1_t.
 */
Qairt_Status_t QairtLpaiBackend_CustomConfig_setHwInfo(
    QairtLpaiBackend_CustomConfigHandle_t customConfig, QairtLpaiBackend_HwInfoHandle_t hwInfo);

/**
 * @brief Get a non-owning reference to the HW info stored in this custom config.
 *
 *        The returned handle is a non-owning reference valid for the lifetime
 *        of customConfig.  Do not free it.
 *
 * @param[in] customConfig The handle to get the configuration from.
 *
 * @param[out] hwInfo Receives a non-owning handle to the stored HW info.
 *                    Must not be NULL.
 *
 * @return QAIRT_SUCCESS on success.
 *         QAIRT_COMMON_ERROR_INVALID_HANDLE if customConfig is not valid.
 *         QAIRT_COMMON_ERROR_INVALID_ARGUMENT if hwInfo is NULL or if
 *         setHwInfo() has not been called on this handle.
 *
 * @note Use corresponding API through QairtLpaiBackend_CustomConfigV1_t.
 */
Qairt_Status_t QairtLpaiBackend_CustomConfig_getHwInfo(
    QairtLpaiBackend_CustomConfigHandle_t customConfig, QairtLpaiBackend_HwInfoHandle_t* hwInfo);

//=============================================================================
// Function Pointer Typedefs — HW Info
//=============================================================================

typedef Qairt_Status_t (*QairtLpaiBackend_HwInfo_CreateFn_t)(QairtLpaiBackend_HwInfoHandle_t*);
typedef Qairt_Status_t (*QairtLpaiBackend_HwInfo_FreeFn_t)(QairtLpaiBackend_HwInfoHandle_t);

/* Target */
typedef Qairt_Status_t (*QairtLpaiBackend_HwInfo_SetTargetFn_t)(
    QairtLpaiBackend_HwInfoHandle_t, QairtLpaiBackend_Target_t);
typedef Qairt_Status_t (*QairtLpaiBackend_HwInfo_GetTargetFn_t)(
    QairtLpaiBackend_HwInfoHandle_t, QairtLpaiBackend_Target_t*);

/* HW Version */
typedef Qairt_Status_t (*QairtLpaiBackend_HwInfo_SetHwVersionFn_t)(
    QairtLpaiBackend_HwInfoHandle_t, QairtLpaiBackend_HwVersion_t);
typedef Qairt_Status_t (*QairtLpaiBackend_HwInfo_GetHwVersionFn_t)(
    QairtLpaiBackend_HwInfoHandle_t, QairtLpaiBackend_HwVersion_t*);

//=============================================================================
// Function Pointer Typedefs — Backend Custom Config
//=============================================================================

typedef Qairt_Status_t (*QairtLpaiBackend_CustomConfig_CreateFn_t)(
    QairtLpaiBackend_CustomConfigHandle_t*);
typedef Qairt_Status_t (*QairtLpaiBackend_CustomConfig_FreeFn_t)(
    QairtLpaiBackend_CustomConfigHandle_t);

/* HW Info */
typedef Qairt_Status_t (*QairtLpaiBackend_CustomConfig_SetHwInfoFn_t)(
    QairtLpaiBackend_CustomConfigHandle_t, QairtLpaiBackend_HwInfoHandle_t);
typedef Qairt_Status_t (*QairtLpaiBackend_CustomConfig_GetHwInfoFn_t)(
    QairtLpaiBackend_CustomConfigHandle_t, QairtLpaiBackend_HwInfoHandle_t*);

//=============================================================================
// Public Interfaces
//=============================================================================

/**
 * @struct QairtLpaiBackend_HwInfoV1_t
 * @brief  Interface struct for LPAI backend HW info (V1).
 *         Retrieved via interface ID QAIRT_LPAI_BACKEND_HW_INFO_ID.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtLpaiBackend_HwInfo_CreateFn_t create;
  QairtLpaiBackend_HwInfo_FreeFn_t free;
  QairtLpaiBackend_HwInfo_SetTargetFn_t setTarget;
  QairtLpaiBackend_HwInfo_GetTargetFn_t getTarget;
  QairtLpaiBackend_HwInfo_SetHwVersionFn_t setHwVersion;
  QairtLpaiBackend_HwInfo_GetHwVersionFn_t getHwVersion;
} QairtLpaiBackend_HwInfoV1_t;

/**
 * @struct QairtLpaiBackend_CustomConfigV1_t
 * @brief  Interface struct for LPAI backend custom configuration (V1).
 *         Retrieved via interface ID QAIRT_LPAI_BACKEND_CUSTOM_CONFIG_ID.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtLpaiBackend_CustomConfig_CreateFn_t create;
  QairtLpaiBackend_CustomConfig_FreeFn_t free;
  QairtLpaiBackend_CustomConfig_SetHwInfoFn_t setHwInfo;
  QairtLpaiBackend_CustomConfig_GetHwInfoFn_t getHwInfo;
} QairtLpaiBackend_CustomConfigV1_t;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // QAIRT_LPAI_BACKEND_H

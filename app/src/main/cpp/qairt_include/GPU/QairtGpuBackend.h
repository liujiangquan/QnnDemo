//=============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//=============================================================================

#ifndef QAIRT_GPU_BACKEND_H
#define QAIRT_GPU_BACKEND_H

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
#define QAIRT_MIN_ID_GPU_BACKEND (QAIRT_MIN_ID_GPU + 1000)

#define QAIRT_GPU_BACKEND_CUSTOM_CONFIG_ID (QAIRT_MIN_ID_GPU_BACKEND + 0)

//=============================================================================
// Data Types
//=============================================================================

typedef QairtBackend_CustomConfigHandle_t QairtGpuBackend_CustomConfigHandle_t;

//=============================================================================
// Public Functions
//=============================================================================

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create a GPU backend custom configuration handle.
 *
 * @param[out] customConfig A pointer to receive the created handle.
 *
 * @return QAIRT_SUCCESS on success, or an error code.
 *
 * @note Use corresponding API through QairtGpuBackend_CustomConfigV1_t.
 */
Qairt_Status_t QairtGpuBackend_CustomConfig_create(
    QairtGpuBackend_CustomConfigHandle_t* customConfig);

/**
 * @brief Free a GPU backend custom configuration handle.
 *
 * @param[in] customConfig The handle to free.
 *
 * @return QAIRT_SUCCESS on success, or an error code.
 *
 * @note Use corresponding API through QairtGpuBackend_CustomConfigV1_t.
 */
Qairt_Status_t QairtGpuBackend_CustomConfig_free(
    QairtGpuBackend_CustomConfigHandle_t customConfig);

// Enable Tuning Mode
/**
 * @brief Enable or disable tuning mode for the GPU backend.
 *
 * @note Use corresponding API through QairtGpuBackend_CustomConfigV1_t.
 */
Qairt_Status_t QairtGpuBackend_CustomConfig_setEnableTuningMode(
    QairtGpuBackend_CustomConfigHandle_t customConfig, uint8_t enableTuningMode);

/**
 * @brief Get the tuning mode enablement setting.
 *
 * @note Use corresponding API through QairtGpuBackend_CustomConfigV1_t.
 */
Qairt_Status_t QairtGpuBackend_CustomConfig_getEnableTuningMode(
    QairtGpuBackend_CustomConfigHandle_t customConfig, uint8_t* enableTuningMode);

// Performance Cache Dir
/**
 * @brief Set the performance cache directory. Must be non-null.
 *
 * @note Use corresponding API through QairtGpuBackend_CustomConfigV1_t.
 */
Qairt_Status_t QairtGpuBackend_CustomConfig_setPerformanceCacheDir(
    QairtGpuBackend_CustomConfigHandle_t customConfig, const char* performanceCacheDir);

/**
 * @brief Get the performance cache directory.
 *
 * @note Use corresponding API through QairtGpuBackend_CustomConfigV1_t.
 */
Qairt_Status_t QairtGpuBackend_CustomConfig_getPerformanceCacheDir(
    QairtGpuBackend_CustomConfigHandle_t customConfig, const char** performanceCacheDir);

// Invalidate Performance Cache
/**
 * @brief Enable or disable invalidation of the performance cache on initialization.
 *
 * @note Use corresponding API through QairtGpuBackend_CustomConfigV1_t.
 */
Qairt_Status_t QairtGpuBackend_CustomConfig_setInvalidatePerformanceCache(
    QairtGpuBackend_CustomConfigHandle_t customConfig, uint8_t invalidatePerformanceCache);

/**
 * @brief Get the performance cache invalidation setting.
 *
 * @note Use corresponding API through QairtGpuBackend_CustomConfigV1_t.
 */
Qairt_Status_t QairtGpuBackend_CustomConfig_getInvalidatePerformanceCache(
    QairtGpuBackend_CustomConfigHandle_t customConfig, uint8_t* invalidatePerformanceCache);

// Weight Sharing Enabled
/**
 * @brief Enable or disable weight sharing for the GPU backend.
 *
 * @note Use corresponding API through QairtGpuBackend_CustomConfigV1_t.
 */
Qairt_Status_t QairtGpuBackend_CustomConfig_setWeightSharingEnabled(
    QairtGpuBackend_CustomConfigHandle_t customConfig, uint8_t weightSharingEnabled);

/**
 * @brief Get the weight sharing enablement setting.
 *
 * @note Use corresponding API through QairtGpuBackend_CustomConfigV1_t.
 */
Qairt_Status_t QairtGpuBackend_CustomConfig_getWeightSharingEnabled(
    QairtGpuBackend_CustomConfigHandle_t customConfig, uint8_t* weightSharingEnabled);

// Disable Kernel Profiling
/**
 * @brief Disable or enable kernel profiling in tuning mode.
 *
 * @note Use corresponding API through QairtGpuBackend_CustomConfigV1_t.
 */
Qairt_Status_t QairtGpuBackend_CustomConfig_setDisableKernelProfiling(
    QairtGpuBackend_CustomConfigHandle_t customConfig, uint8_t disableKernelProfiling);

/**
 * @brief Get the kernel profiling disable setting.
 *
 * @note Use corresponding API through QairtGpuBackend_CustomConfigV1_t.
 */
Qairt_Status_t QairtGpuBackend_CustomConfig_getDisableKernelProfiling(
    QairtGpuBackend_CustomConfigHandle_t customConfig, uint8_t* disableKernelProfiling);

//=============================================================================
// Function Pointer Typedefs
//=============================================================================

typedef Qairt_Status_t (*QairtGpuBackend_CustomConfig_CreateFn_t)(
    QairtGpuBackend_CustomConfigHandle_t*);
typedef Qairt_Status_t (*QairtGpuBackend_CustomConfig_FreeFn_t)(
    QairtGpuBackend_CustomConfigHandle_t);

/* Enable Tuning Mode */
typedef Qairt_Status_t (*QairtGpuBackend_CustomConfig_SetEnableTuningModeFn_t)(
    QairtGpuBackend_CustomConfigHandle_t, uint8_t);
typedef Qairt_Status_t (*QairtGpuBackend_CustomConfig_GetEnableTuningModeFn_t)(
    QairtGpuBackend_CustomConfigHandle_t, uint8_t*);

/* Performance Cache Dir */
typedef Qairt_Status_t (*QairtGpuBackend_CustomConfig_SetPerformanceCacheDirFn_t)(
    QairtGpuBackend_CustomConfigHandle_t, const char*);
typedef Qairt_Status_t (*QairtGpuBackend_CustomConfig_GetPerformanceCacheDirFn_t)(
    QairtGpuBackend_CustomConfigHandle_t, const char**);

/* Invalidate Performance Cache */
typedef Qairt_Status_t (*QairtGpuBackend_CustomConfig_SetInvalidatePerformanceCacheFn_t)(
    QairtGpuBackend_CustomConfigHandle_t, uint8_t);
typedef Qairt_Status_t (*QairtGpuBackend_CustomConfig_GetInvalidatePerformanceCacheFn_t)(
    QairtGpuBackend_CustomConfigHandle_t, uint8_t*);

/* Weight Sharing Enabled */
typedef Qairt_Status_t (*QairtGpuBackend_CustomConfig_SetWeightSharingEnabledFn_t)(
    QairtGpuBackend_CustomConfigHandle_t, uint8_t);
typedef Qairt_Status_t (*QairtGpuBackend_CustomConfig_GetWeightSharingEnabledFn_t)(
    QairtGpuBackend_CustomConfigHandle_t, uint8_t*);

/* Disable Kernel Profiling */
typedef Qairt_Status_t (*QairtGpuBackend_CustomConfig_SetDisableKernelProfilingFn_t)(
    QairtGpuBackend_CustomConfigHandle_t, uint8_t);
typedef Qairt_Status_t (*QairtGpuBackend_CustomConfig_GetDisableKernelProfilingFn_t)(
    QairtGpuBackend_CustomConfigHandle_t, uint8_t*);

//=============================================================================
// Public Interfaces
//=============================================================================

/**
 * @struct QairtGpuBackend_CustomConfigV1_t
 * @brief Interface struct for GPU backend custom configuration (V1).
 *        Retrieved via interface ID QAIRT_GPU_BACKEND_CUSTOM_CONFIG_ID.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtGpuBackend_CustomConfig_CreateFn_t create;
  QairtGpuBackend_CustomConfig_FreeFn_t free;
  QairtGpuBackend_CustomConfig_SetEnableTuningModeFn_t setEnableTuningMode;
  QairtGpuBackend_CustomConfig_GetEnableTuningModeFn_t getEnableTuningMode;
  QairtGpuBackend_CustomConfig_SetPerformanceCacheDirFn_t setPerformanceCacheDir;
  QairtGpuBackend_CustomConfig_GetPerformanceCacheDirFn_t getPerformanceCacheDir;
  QairtGpuBackend_CustomConfig_SetInvalidatePerformanceCacheFn_t setInvalidatePerformanceCache;
  QairtGpuBackend_CustomConfig_GetInvalidatePerformanceCacheFn_t getInvalidatePerformanceCache;
  QairtGpuBackend_CustomConfig_SetWeightSharingEnabledFn_t setWeightSharingEnabled;
  QairtGpuBackend_CustomConfig_GetWeightSharingEnabledFn_t getWeightSharingEnabled;
  QairtGpuBackend_CustomConfig_SetDisableKernelProfilingFn_t setDisableKernelProfiling;
  QairtGpuBackend_CustomConfig_GetDisableKernelProfilingFn_t getDisableKernelProfiling;
} QairtGpuBackend_CustomConfigV1_t;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // QAIRT_GPU_BACKEND_H

//=============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//=============================================================================

#ifndef QAIRT_GPU_CONTEXT_H
#define QAIRT_GPU_CONTEXT_H

#ifdef __cplusplus
#include <cstdint>
#else
#include <stdint.h>
#endif

#include "QairtCommon/QairtCommon.h"
#include "QairtContext/QairtContext.h"
#include "QairtContext/QairtContextConfig.h"

//=============================================================================
// Macros
//=============================================================================
#define QAIRT_MIN_ID_GPU_CONTEXT (QAIRT_MIN_ID_GPU + 0)

#define QAIRT_GPU_CONTEXT_CUSTOM_CONFIG_ID (QAIRT_MIN_ID_GPU_CONTEXT + 0)

//=============================================================================
// Data Types
//=============================================================================

typedef QairtContext_CustomConfigHandle_t QairtGpuContext_CustomConfigHandle_t;

/**
 * @brief An enum which defines the different GPU performance hint options.
 */
typedef enum {
  /// Sets the GPU performance hint to high performance, this is the default
  QAIRT_GPU_CONTEXT_PERF_HINT_HIGH = 0,
  /// Sets the GPU performance hint to normal performance
  QAIRT_GPU_CONTEXT_PERF_HINT_NORMAL = 1,
  /// Sets the GPU performance hint to low performance
  QAIRT_GPU_CONTEXT_PERF_HINT_LOW = 2,
  /// Unused, present to ensure 32 bits.
  QAIRT_GPU_CONTEXT_PERF_HINT_UNDEFINED = 0x7FFFFFFF
} QairtGpuContext_PerfHint_t;

//=============================================================================
// Public Functions
//=============================================================================

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create a GPU context custom configuration handle.
 *
 * @param[out] customConfig A pointer to receive the created handle.
 *
 * @return QAIRT_SUCCESS on success, or an error code.
 *
 * @note Use corresponding API through QairtGpuContext_CustomConfigV1_t.
 */
Qairt_Status_t QairtGpuContext_CustomConfig_create(
    QairtGpuContext_CustomConfigHandle_t* customConfig);

/**
 * @brief Free a GPU context custom configuration handle.
 *
 * @param[in] customConfig The handle to free.
 *
 * @return QAIRT_SUCCESS on success, or an error code.
 *
 * @note Use corresponding API through QairtGpuContext_CustomConfigV1_t.
 */
Qairt_Status_t QairtGpuContext_CustomConfig_free(
    QairtGpuContext_CustomConfigHandle_t customConfig);

// Performance Hint
/**
 * @brief Set the performance hint for the GPU context.
 *
 * @note Use corresponding API through QairtGpuContext_CustomConfigV1_t.
 */
Qairt_Status_t QairtGpuContext_CustomConfig_setPerfHint(
    QairtGpuContext_CustomConfigHandle_t customConfig, QairtGpuContext_PerfHint_t perfHint);

/**
 * @brief Get the performance hint from the GPU context custom config.
 *
 * @note Use corresponding API through QairtGpuContext_CustomConfigV1_t.
 */
Qairt_Status_t QairtGpuContext_CustomConfig_getPerfHint(
    QairtGpuContext_CustomConfigHandle_t customConfig, QairtGpuContext_PerfHint_t* perfHint);

// Use GL Buffers
/**
 * @brief Enable or disable OpenGL buffer usage.
 *
 * @note Use corresponding API through QairtGpuContext_CustomConfigV1_t.
 */
Qairt_Status_t QairtGpuContext_CustomConfig_setUseGLBuffers(
    QairtGpuContext_CustomConfigHandle_t customConfig, uint8_t useGLBuffers);

/**
 * @brief Get the OpenGL buffer usage setting.
 *
 * @note Use corresponding API through QairtGpuContext_CustomConfigV1_t.
 */
Qairt_Status_t QairtGpuContext_CustomConfig_getUseGLBuffers(
    QairtGpuContext_CustomConfigHandle_t customConfig, uint8_t* useGLBuffers);

// Kernel Repo Dir
/**
 * @brief Set the kernel disk cache directory. Must be non-null.
 *
 * @note Use corresponding API through QairtGpuContext_CustomConfigV1_t.
 */
Qairt_Status_t QairtGpuContext_CustomConfig_setKernelRepoDir(
    QairtGpuContext_CustomConfigHandle_t customConfig, const char* kernelRepoDir);

/**
 * @brief Get the kernel disk cache directory.
 *
 * @note Use corresponding API through QairtGpuContext_CustomConfigV1_t.
 */
Qairt_Status_t QairtGpuContext_CustomConfig_getKernelRepoDir(
    QairtGpuContext_CustomConfigHandle_t customConfig, const char** kernelRepoDir);

// Invalidate Kernel Repo
/**
 * @brief Enable or disable invalidation of the kernel disk cache on initialization.
 *
 * @note Use corresponding API through QairtGpuContext_CustomConfigV1_t.
 */
Qairt_Status_t QairtGpuContext_CustomConfig_setInvalidateKernelRepo(
    QairtGpuContext_CustomConfigHandle_t customConfig, uint8_t invalidateKernelRepo);

/**
 * @brief Get the kernel disk cache invalidation setting.
 *
 * @note Use corresponding API through QairtGpuContext_CustomConfigV1_t.
 */
Qairt_Status_t QairtGpuContext_CustomConfig_getInvalidateKernelRepo(
    QairtGpuContext_CustomConfigHandle_t customConfig, uint8_t* invalidateKernelRepo);

//=============================================================================
// Function Pointer Typedefs
//=============================================================================

typedef Qairt_Status_t (*QairtGpuContext_CustomConfig_CreateFn_t)(
    QairtGpuContext_CustomConfigHandle_t*);
typedef Qairt_Status_t (*QairtGpuContext_CustomConfig_FreeFn_t)(
    QairtGpuContext_CustomConfigHandle_t);

/* Performance Hint */
typedef Qairt_Status_t (*QairtGpuContext_CustomConfig_SetPerfHintFn_t)(
    QairtGpuContext_CustomConfigHandle_t, QairtGpuContext_PerfHint_t);
typedef Qairt_Status_t (*QairtGpuContext_CustomConfig_GetPerfHintFn_t)(
    QairtGpuContext_CustomConfigHandle_t, QairtGpuContext_PerfHint_t*);

/* Use GL Buffers */
typedef Qairt_Status_t (*QairtGpuContext_CustomConfig_SetUseGLBuffersFn_t)(
    QairtGpuContext_CustomConfigHandle_t, uint8_t);
typedef Qairt_Status_t (*QairtGpuContext_CustomConfig_GetUseGLBuffersFn_t)(
    QairtGpuContext_CustomConfigHandle_t, uint8_t*);

/* Kernel Repo Dir */
typedef Qairt_Status_t (*QairtGpuContext_CustomConfig_SetKernelRepoDirFn_t)(
    QairtGpuContext_CustomConfigHandle_t, const char*);
typedef Qairt_Status_t (*QairtGpuContext_CustomConfig_GetKernelRepoDirFn_t)(
    QairtGpuContext_CustomConfigHandle_t, const char**);

/* Invalidate Kernel Repo */
typedef Qairt_Status_t (*QairtGpuContext_CustomConfig_SetInvalidateKernelRepoFn_t)(
    QairtGpuContext_CustomConfigHandle_t, uint8_t);
typedef Qairt_Status_t (*QairtGpuContext_CustomConfig_GetInvalidateKernelRepoFn_t)(
    QairtGpuContext_CustomConfigHandle_t, uint8_t*);

//=============================================================================
// Public Interfaces
//=============================================================================

/**
 * @struct QairtGpuContext_CustomConfigV1_t
 * @brief Interface struct for GPU context custom configuration (V1).
 *        Retrieved via interface ID QAIRT_GPU_CONTEXT_CUSTOM_CONFIG_ID.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtGpuContext_CustomConfig_CreateFn_t create;
  QairtGpuContext_CustomConfig_FreeFn_t free;
  QairtGpuContext_CustomConfig_SetPerfHintFn_t setPerfHint;
  QairtGpuContext_CustomConfig_GetPerfHintFn_t getPerfHint;
  QairtGpuContext_CustomConfig_SetUseGLBuffersFn_t setUseGLBuffers;
  QairtGpuContext_CustomConfig_GetUseGLBuffersFn_t getUseGLBuffers;
  QairtGpuContext_CustomConfig_SetKernelRepoDirFn_t setKernelRepoDir;
  QairtGpuContext_CustomConfig_GetKernelRepoDirFn_t getKernelRepoDir;
  QairtGpuContext_CustomConfig_SetInvalidateKernelRepoFn_t setInvalidateKernelRepo;
  QairtGpuContext_CustomConfig_GetInvalidateKernelRepoFn_t getInvalidateKernelRepo;
} QairtGpuContext_CustomConfigV1_t;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // QAIRT_GPU_CONTEXT_H

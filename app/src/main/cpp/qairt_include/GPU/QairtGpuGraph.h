//=============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//=============================================================================

#ifndef QAIRT_GPU_GRAPH_H
#define QAIRT_GPU_GRAPH_H

#ifdef __cplusplus
#include <cstdint>
#else
#include <stdint.h>
#endif

#include "QairtCommon/QairtCommon.h"
#include "QairtGraph/QairtGraphConfig.h"

//=============================================================================
// Macros
//=============================================================================
#define QAIRT_MIN_ID_GPU_GRAPH (QAIRT_MIN_ID_GPU + 2000)

#define QAIRT_GPU_GRAPH_FENCE_CONFIG_ID  (QAIRT_MIN_ID_GPU_GRAPH + 0)
#define QAIRT_GPU_GRAPH_CUSTOM_CONFIG_ID (QAIRT_MIN_ID_GPU_GRAPH + 1)

//=============================================================================
// Data Types
//=============================================================================

typedef QairtGraph_CustomConfigHandle_t QairtGpuGraph_CustomConfigHandle_t;

/// Handle for a GPU graph fence configuration object
typedef struct _QairtGpuGraph_FenceConfigHandle_t* QairtGpuGraph_FenceConfigHandle_t;

/// Fence handle type (file descriptor pointer)
typedef int32_t* QairtGpuGraph_FenceHandle_t;

/**
 * @brief An enum which defines the different tensor optimization options. A
 *        tensor may be optimized to the specified QairtGpu_Precision_t when it
 *        is a graph tensor that is not a graph input or a graph output and
 *        does not connect two operations from different op packages.
 */
typedef enum {
  /// Sets the precision mode to floating point 32-bit (FP32)
  QAIRT_GPU_PRECISION_FP32 = 0,
  /// Sets the precision mode to floating point 16-bit (FP16)
  QAIRT_GPU_PRECISION_FP16 = 1,
  /// Sets the precision mode to FP16 for storage and FP32 for calculations
  QAIRT_GPU_PRECISION_HYBRID = 2,
  /// Uses the tensor data type provided by the user (default)
  QAIRT_GPU_PRECISION_USER_PROVIDED = 3,
  /// Unused, present to ensure 32 bits.
  QAIRT_GPU_PRECISION_UNDEFINED = 0x7FFFFFFF
} QairtGpu_Precision_t;

//=============================================================================
// Public Functions — Fence Config
//=============================================================================

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create a GPU graph fence configuration handle.
 *
 * @param[out] fenceConfig A pointer to receive the created handle.
 *
 * @return QAIRT_SUCCESS on success, or an error code.
 *
 * @note Use corresponding API through QairtGpuGraph_FenceConfigV1_t.
 */
Qairt_Status_t QairtGpuGraph_FenceConfig_create(
    QairtGpuGraph_FenceConfigHandle_t* fenceConfig);

/**
 * @brief Free a GPU graph fence configuration handle.
 *
 * @param[in] fenceConfig The handle to free.
 *
 * @return QAIRT_SUCCESS on success, or an error code.
 *
 * @note Use corresponding API through QairtGpuGraph_FenceConfigV1_t.
 */
Qairt_Status_t QairtGpuGraph_FenceConfig_free(QairtGpuGraph_FenceConfigHandle_t fenceConfig);

/**
 * @brief Set the pointer to the client-owned number-of-input-fences value.
 *
 * The pointed-to value is read by the backend during execute.
 *
 * @note Use corresponding API through QairtGpuGraph_FenceConfigV1_t.
 */
Qairt_Status_t QairtGpuGraph_FenceConfig_setNumInputFences(
    QairtGpuGraph_FenceConfigHandle_t fenceConfig, uint32_t* numInputFences);

/**
 * @brief Get the pointer to the client-owned number-of-input-fences value.
 *
 * @note Use corresponding API through QairtGpuGraph_FenceConfigV1_t.
 */
Qairt_Status_t QairtGpuGraph_FenceConfig_getNumInputFences(
    QairtGpuGraph_FenceConfigHandle_t fenceConfig, uint32_t** numInputFences);

/**
 * @brief Set the pointer to the client-owned array of input FD fence handles.
 *
 * The array is read by the backend during execute.
 *
 * @note Use corresponding API through QairtGpuGraph_FenceConfigV1_t.
 */
Qairt_Status_t QairtGpuGraph_FenceConfig_setInputFenceHandles(
    QairtGpuGraph_FenceConfigHandle_t fenceConfig,
    QairtGpuGraph_FenceHandle_t* inputFenceHandles);

/**
 * @brief Get the pointer to the client-owned array of input FD fence handles.
 *
 * @note Use corresponding API through QairtGpuGraph_FenceConfigV1_t.
 */
Qairt_Status_t QairtGpuGraph_FenceConfig_getInputFenceHandles(
    QairtGpuGraph_FenceConfigHandle_t fenceConfig,
    QairtGpuGraph_FenceHandle_t** inputFenceHandles);

/**
 * @brief Set whether the backend should fill in the output fence during execute.
 *
 * @note Use corresponding API through QairtGpuGraph_FenceConfigV1_t.
 */
Qairt_Status_t QairtGpuGraph_FenceConfig_setEnableOutputFence(
    QairtGpuGraph_FenceConfigHandle_t fenceConfig, uint8_t enableOutputFence);

/**
 * @brief Get the enable-output-fence flag.
 *
 * @note Use corresponding API through QairtGpuGraph_FenceConfigV1_t.
 */
Qairt_Status_t QairtGpuGraph_FenceConfig_getEnableOutputFence(
    QairtGpuGraph_FenceConfigHandle_t fenceConfig, uint8_t* enableOutputFence);

/**
 * @brief Set the pointer to the client-owned output fence FD location.
 *
 * The backend writes the output fence FD into this location during execute.
 *
 * @note Use corresponding API through QairtGpuGraph_FenceConfigV1_t.
 */
Qairt_Status_t QairtGpuGraph_FenceConfig_setOutputFence(
    QairtGpuGraph_FenceConfigHandle_t fenceConfig, QairtGpuGraph_FenceHandle_t outputFence);

/**
 * @brief Get the pointer to the client-owned output fence FD location.
 *
 * @note Use corresponding API through QairtGpuGraph_FenceConfigV1_t.
 */
Qairt_Status_t QairtGpuGraph_FenceConfig_getOutputFence(
    QairtGpuGraph_FenceConfigHandle_t fenceConfig, QairtGpuGraph_FenceHandle_t* outputFence);

//=============================================================================
// Public Functions — Graph Custom Config
//=============================================================================

/**
 * @brief Create a GPU graph custom configuration handle.
 *
 * @param[out] customConfig A pointer to receive the created handle.
 *
 * @return QAIRT_SUCCESS on success, or an error code.
 *
 * @note Use corresponding API through QairtGpuGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtGpuGraph_CustomConfig_create(
    QairtGpuGraph_CustomConfigHandle_t* customConfig);

/**
 * @brief Free a GPU graph custom configuration handle.
 *
 * @param[in] customConfig The handle to free.
 *
 * @return QAIRT_SUCCESS on success, or an error code.
 *
 * @note Use corresponding API through QairtGpuGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtGpuGraph_CustomConfig_free(
    QairtGpuGraph_CustomConfigHandle_t customConfig);

// Precision
/**
 * @brief Set the precision mode for the GPU graph.
 *
 * @note Use corresponding API through QairtGpuGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtGpuGraph_CustomConfig_setPrecision(
    QairtGpuGraph_CustomConfigHandle_t customConfig, QairtGpu_Precision_t precision);

/**
 * @brief Get the precision mode from the GPU graph custom config.
 *
 * @note Use corresponding API through QairtGpuGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtGpuGraph_CustomConfig_getPrecision(
    QairtGpuGraph_CustomConfigHandle_t customConfig, QairtGpu_Precision_t* precision);

// Disable Memory Optimizations
/**
 * @brief Disable or enable backend memory optimizations.
 *
 * @note Use corresponding API through QairtGpuGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtGpuGraph_CustomConfig_setDisableMemoryOptimizations(
    QairtGpuGraph_CustomConfigHandle_t customConfig, uint8_t disableMemoryOptimizations);

/**
 * @brief Get the memory optimization disable setting.
 *
 * @note Use corresponding API through QairtGpuGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtGpuGraph_CustomConfig_getDisableMemoryOptimizations(
    QairtGpuGraph_CustomConfigHandle_t customConfig, uint8_t* disableMemoryOptimizations);

// Disable Node Optimizations
/**
 * @brief Disable or enable backend node optimizations.
 *
 * @note Use corresponding API through QairtGpuGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtGpuGraph_CustomConfig_setDisableNodeOptimizations(
    QairtGpuGraph_CustomConfigHandle_t customConfig, uint8_t disableNodeOptimizations);

/**
 * @brief Get the node optimization disable setting.
 *
 * @note Use corresponding API through QairtGpuGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtGpuGraph_CustomConfig_getDisableNodeOptimizations(
    QairtGpuGraph_CustomConfigHandle_t customConfig, uint8_t* disableNodeOptimizations);

// Disable Queue Recording
/**
 * @brief Disable or enable the backend's recordable command queue.
 *
 * @note Use corresponding API through QairtGpuGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtGpuGraph_CustomConfig_setDisableQueueRecording(
    QairtGpuGraph_CustomConfigHandle_t customConfig, uint8_t disableQueueRecording);

/**
 * @brief Get the queue recording disable setting.
 *
 * @note Use corresponding API through QairtGpuGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtGpuGraph_CustomConfig_getDisableQueueRecording(
    QairtGpuGraph_CustomConfigHandle_t customConfig, uint8_t* disableQueueRecording);

// Fence Config
/**
 * @brief Associate a fence configuration handle with this graph custom config.
 *
 * @note Use corresponding API through QairtGpuGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtGpuGraph_CustomConfig_setFenceConfig(
    QairtGpuGraph_CustomConfigHandle_t customConfig,
    QairtGpuGraph_FenceConfigHandle_t fenceConfig);

/**
 * @brief Get the fence configuration handle associated with this graph custom config.
 *
 * @note Use corresponding API through QairtGpuGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtGpuGraph_CustomConfig_getFenceConfig(
    QairtGpuGraph_CustomConfigHandle_t customConfig,
    QairtGpuGraph_FenceConfigHandle_t* fenceConfig);

//=============================================================================
// Function Pointer Typedefs — Fence Config
//=============================================================================

typedef Qairt_Status_t (*QairtGpuGraph_FenceConfig_CreateFn_t)(
    QairtGpuGraph_FenceConfigHandle_t*);
typedef Qairt_Status_t (*QairtGpuGraph_FenceConfig_FreeFn_t)(
    QairtGpuGraph_FenceConfigHandle_t);
typedef Qairt_Status_t (*QairtGpuGraph_FenceConfig_SetNumInputFencesFn_t)(
    QairtGpuGraph_FenceConfigHandle_t, uint32_t*);
typedef Qairt_Status_t (*QairtGpuGraph_FenceConfig_GetNumInputFencesFn_t)(
    QairtGpuGraph_FenceConfigHandle_t, uint32_t**);
typedef Qairt_Status_t (*QairtGpuGraph_FenceConfig_SetInputFenceHandlesFn_t)(
    QairtGpuGraph_FenceConfigHandle_t, QairtGpuGraph_FenceHandle_t*);
typedef Qairt_Status_t (*QairtGpuGraph_FenceConfig_GetInputFenceHandlesFn_t)(
    QairtGpuGraph_FenceConfigHandle_t, QairtGpuGraph_FenceHandle_t**);
typedef Qairt_Status_t (*QairtGpuGraph_FenceConfig_SetEnableOutputFenceFn_t)(
    QairtGpuGraph_FenceConfigHandle_t, uint8_t);
typedef Qairt_Status_t (*QairtGpuGraph_FenceConfig_GetEnableOutputFenceFn_t)(
    QairtGpuGraph_FenceConfigHandle_t, uint8_t*);
typedef Qairt_Status_t (*QairtGpuGraph_FenceConfig_SetOutputFenceFn_t)(
    QairtGpuGraph_FenceConfigHandle_t, QairtGpuGraph_FenceHandle_t);
typedef Qairt_Status_t (*QairtGpuGraph_FenceConfig_GetOutputFenceFn_t)(
    QairtGpuGraph_FenceConfigHandle_t, QairtGpuGraph_FenceHandle_t*);

//=============================================================================
// Function Pointer Typedefs — Graph Custom Config
//=============================================================================

typedef Qairt_Status_t (*QairtGpuGraph_CustomConfig_CreateFn_t)(
    QairtGpuGraph_CustomConfigHandle_t*);
typedef Qairt_Status_t (*QairtGpuGraph_CustomConfig_FreeFn_t)(
    QairtGpuGraph_CustomConfigHandle_t);

/* Precision */
typedef Qairt_Status_t (*QairtGpuGraph_CustomConfig_SetPrecisionFn_t)(
    QairtGpuGraph_CustomConfigHandle_t, QairtGpu_Precision_t);
typedef Qairt_Status_t (*QairtGpuGraph_CustomConfig_GetPrecisionFn_t)(
    QairtGpuGraph_CustomConfigHandle_t, QairtGpu_Precision_t*);

/* Disable Memory Optimizations */
typedef Qairt_Status_t (*QairtGpuGraph_CustomConfig_SetDisableMemoryOptimizationsFn_t)(
    QairtGpuGraph_CustomConfigHandle_t, uint8_t);
typedef Qairt_Status_t (*QairtGpuGraph_CustomConfig_GetDisableMemoryOptimizationsFn_t)(
    QairtGpuGraph_CustomConfigHandle_t, uint8_t*);

/* Disable Node Optimizations */
typedef Qairt_Status_t (*QairtGpuGraph_CustomConfig_SetDisableNodeOptimizationsFn_t)(
    QairtGpuGraph_CustomConfigHandle_t, uint8_t);
typedef Qairt_Status_t (*QairtGpuGraph_CustomConfig_GetDisableNodeOptimizationsFn_t)(
    QairtGpuGraph_CustomConfigHandle_t, uint8_t*);

/* Disable Queue Recording */
typedef Qairt_Status_t (*QairtGpuGraph_CustomConfig_SetDisableQueueRecordingFn_t)(
    QairtGpuGraph_CustomConfigHandle_t, uint8_t);
typedef Qairt_Status_t (*QairtGpuGraph_CustomConfig_GetDisableQueueRecordingFn_t)(
    QairtGpuGraph_CustomConfigHandle_t, uint8_t*);

/* Fence Config */
typedef Qairt_Status_t (*QairtGpuGraph_CustomConfig_SetFenceConfigFn_t)(
    QairtGpuGraph_CustomConfigHandle_t, QairtGpuGraph_FenceConfigHandle_t);
typedef Qairt_Status_t (*QairtGpuGraph_CustomConfig_GetFenceConfigFn_t)(
    QairtGpuGraph_CustomConfigHandle_t, QairtGpuGraph_FenceConfigHandle_t*);

//=============================================================================
// Public Interfaces
//=============================================================================

/**
 * @struct QairtGpuGraph_FenceConfigV1_t
 * @brief Interface struct for GPU graph fence configuration (V1).
 *        Retrieved via interface ID QAIRT_GPU_GRAPH_FENCE_CONFIG_ID.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtGpuGraph_FenceConfig_CreateFn_t create;
  QairtGpuGraph_FenceConfig_FreeFn_t free;
  QairtGpuGraph_FenceConfig_SetNumInputFencesFn_t setNumInputFences;
  QairtGpuGraph_FenceConfig_GetNumInputFencesFn_t getNumInputFences;
  QairtGpuGraph_FenceConfig_SetInputFenceHandlesFn_t setInputFenceHandles;
  QairtGpuGraph_FenceConfig_GetInputFenceHandlesFn_t getInputFenceHandles;
  QairtGpuGraph_FenceConfig_SetEnableOutputFenceFn_t setEnableOutputFence;
  QairtGpuGraph_FenceConfig_GetEnableOutputFenceFn_t getEnableOutputFence;
  QairtGpuGraph_FenceConfig_SetOutputFenceFn_t setOutputFence;
  QairtGpuGraph_FenceConfig_GetOutputFenceFn_t getOutputFence;
} QairtGpuGraph_FenceConfigV1_t;

/**
 * @struct QairtGpuGraph_CustomConfigV1_t
 * @brief Interface struct for GPU graph custom configuration (V1).
 *        Retrieved via interface ID QAIRT_GPU_GRAPH_CUSTOM_CONFIG_ID.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtGpuGraph_CustomConfig_CreateFn_t create;
  QairtGpuGraph_CustomConfig_FreeFn_t free;
  QairtGpuGraph_CustomConfig_SetPrecisionFn_t setPrecision;
  QairtGpuGraph_CustomConfig_GetPrecisionFn_t getPrecision;
  QairtGpuGraph_CustomConfig_SetDisableMemoryOptimizationsFn_t setDisableMemoryOptimizations;
  QairtGpuGraph_CustomConfig_GetDisableMemoryOptimizationsFn_t getDisableMemoryOptimizations;
  QairtGpuGraph_CustomConfig_SetDisableNodeOptimizationsFn_t setDisableNodeOptimizations;
  QairtGpuGraph_CustomConfig_GetDisableNodeOptimizationsFn_t getDisableNodeOptimizations;
  QairtGpuGraph_CustomConfig_SetDisableQueueRecordingFn_t setDisableQueueRecording;
  QairtGpuGraph_CustomConfig_GetDisableQueueRecordingFn_t getDisableQueueRecording;
  QairtGpuGraph_CustomConfig_SetFenceConfigFn_t setFenceConfig;
  QairtGpuGraph_CustomConfig_GetFenceConfigFn_t getFenceConfig;
} QairtGpuGraph_CustomConfigV1_t;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // QAIRT_GPU_GRAPH_H

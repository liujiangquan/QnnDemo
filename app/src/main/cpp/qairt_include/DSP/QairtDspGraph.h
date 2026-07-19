//=============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//=============================================================================

#ifndef QAIRT_DSP_GRAPH_H
#define QAIRT_DSP_GRAPH_H

#ifdef __cplusplus
#include <cstddef>
#include <cstdint>
#else
#include <stddef.h>
#include <stdint.h>
#endif

#include "QairtContext/QairtContextConfig.h"
#include "QairtGraph/QairtGraphConfig.h"

//=============================================================================
// Macros
//=============================================================================
#define QAIRT_MIN_ID_DSP_GRAPH (QAIRT_MIN_ID_DSP + 200)

#define QAIRT_DSP_GRAPH_GROUP_REGISTRATION_ID (QAIRT_MIN_ID_DSP_GRAPH + 0)
#define QAIRT_DSP_GRAPH_CUSTOM_CONFIG_ID      (QAIRT_MIN_ID_DSP_GRAPH + 1)

//=============================================================================
// Data Types
//=============================================================================

/**
 * @brief This enum provides different DSP graph optimization types
 *        that can be used when finalizing a graph for optimum performance.
 */
typedef enum {
  QAIRT_DSP_GRAPH_OPTIMIZATION_TYPE_SCHEDULE_THRESHOLD         = 1,
  QAIRT_DSP_GRAPH_OPTIMIZATION_TYPE_FINALIZE_RETRIES           = 2,
  QAIRT_DSP_GRAPH_OPTIMIZATION_TYPE_FINALIZE_OPTIMIZATION_FLAG = 3,
  QAIRT_DSP_GRAPH_OPTIMIZATION_TYPE_ENABLE_DLBC                = 4,
  QAIRT_DSP_GRAPH_OPTIMIZATION_TYPE_UNKNOWN                    = 0x7fffffff
} QairtDspGraph_OptimizationType_t;

/**
 * @brief This enum provides the encoding types for a DSP graph.
 */
typedef enum {
  QAIRT_DSP_GRAPH_ENCODING_DYNAMIC = 1,
  QAIRT_DSP_GRAPH_ENCODING_STATIC  = 2,
  QAIRT_DSP_GRAPH_ENCODING_UNKNOWN = 0x7fffffff
} QairtDspGraph_Encoding_t;

typedef QairtGraph_CustomConfigHandle_t QairtDspGraph_CustomConfigHandle_t;

//=============================================================================
// Public Functions
//=============================================================================

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create a custom configuration handle for DSP graph operations.
 *
 * Allocates and initializes a custom configuration object that can be used to
 * override default graph settings for DSP programs.
 *
 * @param[out] customConfig A pointer to receive the created custom configuration handle.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Custom configuration handle was successfully created.
 *         - QAIRT_GRAPH_ERROR_INVALID_ARGUMENT: At least one argument is invalid (e.g.,
 * customConfig is NULL).
 *         - QAIRT_GRAPH_ERROR_MEM_ALLOC: Memory allocation failed during creation.
 *         - QAIRT_GRAPH_ERROR_UNSUPPORTED_FEATURE: The requested operation is not supported.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence (successful recovery).
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence (unsuccessful recovery).
 *
 * @note Use corresponding API through QairtDspGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtDspGraph_CustomConfig_create(
    QairtDspGraph_CustomConfigHandle_t* customConfig);

/**
 * @brief Free a previously created custom configuration handle and associated resources.
 *
 * Releases all resources associated with the custom configuration handle. On success,
 * the handle pointed to by customConfig will be set to NULL.
 *
 * @param[in] customConfig The custom configuration handle to free.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Custom configuration handle was successfully freed.
 *         - QAIRT_GRAPH_ERROR_INVALID_ARGUMENT: At least one argument is invalid (e.g.,
 * customConfig is NULL).
 *         - QAIRT_GRAPH_ERROR_INVALID_HANDLE: The handle referenced by customConfig is not valid
 * or already freed.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence (successful recovery).
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence (unsuccessful recovery).
 *
 * @note Use corresponding API through QairtDspGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtDspGraph_CustomConfig_free(
    QairtDspGraph_CustomConfigHandle_t customConfig);

/**
 * @brief Set a graph optimization option.
 *
 * Configures a specific optimization strategy for graph finalization on the DSP.
 *
 * Allowable values per optimization type:
 *   - QAIRT_DSP_GRAPH_OPTIMIZATION_TYPE_SCHEDULE_THRESHOLD:         Reserved
 *   - QAIRT_DSP_GRAPH_OPTIMIZATION_TYPE_FINALIZE_RETRIES:           Reserved
 *   - QAIRT_DSP_GRAPH_OPTIMIZATION_TYPE_FINALIZE_OPTIMIZATION_FLAG: 1.0f = faster prepare,
 *                                                                    2.0f = more optimal graph
 *   - QAIRT_DSP_GRAPH_OPTIMIZATION_TYPE_ENABLE_DLBC:                Reserved
 *
 * @param[in] customConfig Handle to the custom configuration object.
 * @param[in] type         The optimization type to configure.
 * @param[in] floatValue   The value associated with the optimization type.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Optimization option was successfully set.
 *         - QAIRT_GRAPH_ERROR_INVALID_ARGUMENT: At least one argument is invalid.
 *         - QAIRT_GRAPH_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *         - QAIRT_GRAPH_ERROR_UNSUPPORTED_FEATURE: Operation is not supported on the target.
 *         - QAIRT_GRAPH_ERROR_INVALID_CONFIG: Provided value is out of the allowed range.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence (successful recovery).
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence (unsuccessful recovery).
 *
 * @note Use corresponding API through QairtDspGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtDspGraph_CustomConfig_setOptimization(
    QairtDspGraph_CustomConfigHandle_t customConfig,
    QairtDspGraph_OptimizationType_t type,
    float floatValue);

/**
 * @brief Get a graph optimization option.
 *
 * Retrieves the float value associated with a specific optimization type.
 *
 * @param[in]  customConfig Handle to the custom configuration object.
 * @param[out]  type        The optimization type of the config.
 * @param[out] floatValue   Pointer to receive the value for the given optimization type.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Optimization option was successfully retrieved.
 *         - QAIRT_GRAPH_ERROR_INVALID_ARGUMENT: At least one argument is invalid (e.g.,
 * floatValue is NULL).
 *         - QAIRT_GRAPH_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *         - QAIRT_GRAPH_ERROR_UNSUPPORTED_FEATURE: Operation is not supported on the target.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence (successful recovery).
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence (unsuccessful recovery).
 *
 * @note Use corresponding API through QairtDspGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtDspGraph_CustomConfig_getOptimization(
    QairtDspGraph_CustomConfigHandle_t customConfig,
    QairtDspGraph_OptimizationType_t* type,
    float* floatValue);

/**
 * @brief Set the encoding type for the DSP graph.
 *
 * Specifies whether the graph uses dynamic or static encoding.
 *
 * @param[in] customConfig Handle to the custom configuration object.
 * @param[in] encoding     The encoding type to set (QairtDspGraph_Encoding_t).
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Encoding was successfully set.
 *         - QAIRT_GRAPH_ERROR_INVALID_ARGUMENT: At least one argument is invalid.
 *         - QAIRT_GRAPH_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *         - QAIRT_GRAPH_ERROR_UNSUPPORTED_FEATURE: Operation is not supported on the target.
 *         - QAIRT_GRAPH_ERROR_INVALID_CONFIG: Provided encoding value is not recognized.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence (successful recovery).
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence (unsuccessful recovery).
 *
 * @note Use corresponding API through QairtDspGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtDspGraph_CustomConfig_setEncoding(
    QairtDspGraph_CustomConfigHandle_t customConfig, QairtDspGraph_Encoding_t encoding);

/**
 * @brief Get the encoding type configured for the DSP graph.
 *
 * Retrieves the currently configured encoding type.
 *
 * @param[in]  customConfig Handle to the custom configuration object.
 * @param[out] encoding     Pointer to receive the configured encoding type.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Encoding was successfully retrieved.
 *         - QAIRT_GRAPH_ERROR_INVALID_ARGUMENT: At least one argument is invalid (e.g.,
 * encoding is NULL).
 *         - QAIRT_GRAPH_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *         - QAIRT_GRAPH_ERROR_UNSUPPORTED_FEATURE: Operation is not supported on the target.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence (successful recovery).
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence (unsuccessful recovery).
 *
 * @note Use corresponding API through QairtDspGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtDspGraph_CustomConfig_getEncoding(
    QairtDspGraph_CustomConfigHandle_t customConfig, QairtDspGraph_Encoding_t* encoding);

/**
 * @brief Set the execution priority for the DSP graph.
 *
 * Specifies the scheduling priority for graph execution on the DSP.
 *
 * @param[in] customConfig Handle to the custom configuration object.
 * @param[in] priority     The priority to set (Qairt_Priority_t).
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Priority was successfully set.
 *         - QAIRT_GRAPH_ERROR_INVALID_ARGUMENT: At least one argument is invalid.
 *         - QAIRT_GRAPH_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *         - QAIRT_GRAPH_ERROR_UNSUPPORTED_FEATURE: Operation is not supported on the target.
 *         - QAIRT_GRAPH_ERROR_INVALID_CONFIG: Provided priority value is not recognized.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence (successful recovery).
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence (unsuccessful recovery).
 *
 * @note Use corresponding API through QairtDspGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtDspGraph_CustomConfig_setPriority(
    QairtDspGraph_CustomConfigHandle_t customConfig, Qairt_Priority_t priority);

/**
 * @brief Get the execution priority configured for the DSP graph.
 *
 * Retrieves the currently configured execution priority.
 *
 * @param[in]  customConfig Handle to the custom configuration object.
 * @param[out] priority     Pointer to receive the configured priority.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Priority was successfully retrieved.
 *         - QAIRT_GRAPH_ERROR_INVALID_ARGUMENT: At least one argument is invalid (e.g.,
 * priority is NULL).
 *         - QAIRT_GRAPH_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *         - QAIRT_GRAPH_ERROR_UNSUPPORTED_FEATURE: Operation is not supported on the target.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence (successful recovery).
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence (unsuccessful recovery).
 *
 * @note Use corresponding API through QairtDspGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtDspGraph_CustomConfig_getPriority(
    QairtDspGraph_CustomConfigHandle_t customConfig, Qairt_Priority_t* priority);

/**
 * @brief Set the compute precision for the DSP graph.
 *
 * Specifies the numerical precision used during graph execution on the DSP.
 * The value is backend-defined; refer to QnnDspGraph_ConfigOption_t documentation
 * for supported precision values.
 *
 * @param[in] customConfig Handle to the custom configuration object.
 * @param[in] precision    The precision value to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Precision was successfully set.
 *         - QAIRT_GRAPH_ERROR_INVALID_ARGUMENT: At least one argument is invalid.
 *         - QAIRT_GRAPH_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *         - QAIRT_GRAPH_ERROR_UNSUPPORTED_FEATURE: Operation is not supported on the target.
 *         - QAIRT_GRAPH_ERROR_INVALID_CONFIG: Provided precision value is not recognized.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence (successful recovery).
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence (unsuccessful recovery).
 *
 * @note Use corresponding API through QairtDspGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtDspGraph_CustomConfig_setPrecision(
    QairtDspGraph_CustomConfigHandle_t customConfig, Qairt_Precision_t precision);

/**
 * @brief Get the compute precision configured for the DSP graph.
 *
 * Retrieves the currently configured precision value.
 *
 * @param[in]  customConfig Handle to the custom configuration object.
 * @param[out] precision    Pointer to receive the configured precision value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Precision was successfully retrieved.
 *         - QAIRT_GRAPH_ERROR_INVALID_ARGUMENT: At least one argument is invalid (e.g.,
 * precision is NULL).
 *         - QAIRT_GRAPH_ERROR_INVALID_HANDLE: customConfig is not a valid handle.
 *         - QAIRT_GRAPH_ERROR_UNSUPPORTED_FEATURE: Operation is not supported on the target.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence (successful recovery).
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence (unsuccessful recovery).
 *
 * @note Use corresponding API through QairtDspGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtDspGraph_CustomConfig_getPrecision(
    QairtDspGraph_CustomConfigHandle_t customConfig, Qairt_Precision_t* precision);

typedef Qairt_Status_t (*QairtDspGraph_CustomConfig_CreateFn_t)(
    QairtDspGraph_CustomConfigHandle_t*);

typedef Qairt_Status_t (*QairtDspGraph_CustomConfig_FreeFn_t)(
    QairtDspGraph_CustomConfigHandle_t);

typedef Qairt_Status_t (*QairtDspGraph_CustomConfig_SetOptimizationFn_t)(
    QairtDspGraph_CustomConfigHandle_t, QairtDspGraph_OptimizationType_t, float);

typedef Qairt_Status_t (*QairtDspGraph_CustomConfig_GetOptimizationFn_t)(
    QairtDspGraph_CustomConfigHandle_t, QairtDspGraph_OptimizationType_t*, float*);

typedef Qairt_Status_t (*QairtDspGraph_CustomConfig_SetEncodingFn_t)(
    QairtDspGraph_CustomConfigHandle_t, QairtDspGraph_Encoding_t);

typedef Qairt_Status_t (*QairtDspGraph_CustomConfig_GetEncodingFn_t)(
    QairtDspGraph_CustomConfigHandle_t, QairtDspGraph_Encoding_t*);

typedef Qairt_Status_t (*QairtDspGraph_CustomConfig_SetPriorityFn_t)(
    QairtDspGraph_CustomConfigHandle_t, Qairt_Priority_t);

typedef Qairt_Status_t (*QairtDspGraph_CustomConfig_GetPriorityFn_t)(
    QairtDspGraph_CustomConfigHandle_t, Qairt_Priority_t*);

typedef Qairt_Status_t (*QairtDspGraph_CustomConfig_SetPrecisionFn_t)(
    QairtDspGraph_CustomConfigHandle_t, Qairt_Precision_t);

typedef Qairt_Status_t (*QairtDspGraph_CustomConfig_GetPrecisionFn_t)(
    QairtDspGraph_CustomConfigHandle_t, Qairt_Precision_t*);

//=============================================================================
// Public Interfaces
//=============================================================================

typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtDspGraph_CustomConfig_CreateFn_t create;
  QairtDspGraph_CustomConfig_FreeFn_t free;
  QairtDspGraph_CustomConfig_SetOptimizationFn_t setOptimization;
  QairtDspGraph_CustomConfig_GetOptimizationFn_t getOptimization;
  QairtDspGraph_CustomConfig_SetEncodingFn_t setEncoding;
  QairtDspGraph_CustomConfig_GetEncodingFn_t getEncoding;
  QairtDspGraph_CustomConfig_SetPriorityFn_t setPriority;
  QairtDspGraph_CustomConfig_GetPriorityFn_t getPriority;
  QairtDspGraph_CustomConfig_SetPrecisionFn_t setPrecision;
  QairtDspGraph_CustomConfig_GetPrecisionFn_t getPrecision;
} QairtDspGraph_CustomConfigV1_t;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif

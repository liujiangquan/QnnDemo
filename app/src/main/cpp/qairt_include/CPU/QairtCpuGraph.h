//==============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//==============================================================================

#ifndef QAIRT_CPU_GRAPH_H
#define QAIRT_CPU_GRAPH_H

#ifdef __cplusplus
#include <cstdint>
#else
#include <stdint.h>
#endif

#include "QairtCommon/QairtCommon.h"
#include "QairtGraph/QairtGraphConfig.h"
#include "QnnCpuGraph.h"

//=============================================================================
// Macros
//=============================================================================
#define QAIRT_MIN_ID_CPU_GRAPH (QAIRT_MIN_ID_CPU + 2000)

#define QAIRT_CPU_GRAPH_CUSTOM_CONFIG_ID (QAIRT_MIN_ID_CPU_GRAPH + 0)

//=============================================================================
// Data Types
//=============================================================================

typedef QairtGraph_CustomConfigHandle_t QairtCpuGraph_CustomConfigHandle_t;

//=============================================================================
// Public Functions
//=============================================================================

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create a CPU graph custom configuration handle.
 *
 * @param[out] customConfig A pointer to receive the created handle.
 *
 * @return QAIRT_SUCCESS on success, or an error code.
 *
 * @note Use corresponding API through QairtCpuGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtCpuGraph_CustomConfig_create(
    QairtCpuGraph_CustomConfigHandle_t* customConfig);

/**
 * @brief Free a CPU graph custom configuration handle.
 *
 * @param[in] customConfig The handle to free.
 *
 * @return QAIRT_SUCCESS on success, or an error code.
 *
 * @note Use corresponding API through QairtCpuGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtCpuGraph_CustomConfig_free(
    QairtCpuGraph_CustomConfigHandle_t customConfig);

// Use QMX
/**
 * @brief Enable or disable QMX (Qualcomm Math Extensions) for the CPU graph.
 *
 * @param[in] customConfig The custom configuration handle.
 * @param[in] useQmx Boolean flag to enable (1) or disable (0) QMX.
 *
 * @return QAIRT_SUCCESS on success, or an error code.
 *
 * @note Use corresponding API through QairtCpuGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtCpuGraph_CustomConfig_setUseQmx(
    QairtCpuGraph_CustomConfigHandle_t customConfig, uint8_t useQmx);

/**
 * @brief Get the QMX enable/disable setting from the CPU graph custom config.
 *
 * @param[in] customConfig The custom configuration handle.
 * @param[out] useQmx Pointer to receive the QMX setting.
 *
 * @return QAIRT_SUCCESS on success, or an error code.
 *
 * @note Use corresponding API through QairtCpuGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtCpuGraph_CustomConfig_getUseQmx(
    QairtCpuGraph_CustomConfigHandle_t customConfig, uint8_t* useQmx);

//=============================================================================
// Function Pointer Typedefs
//=============================================================================

typedef Qairt_Status_t (*QairtCpuGraph_CustomConfig_CreateFn_t)(
    QairtCpuGraph_CustomConfigHandle_t*);
typedef Qairt_Status_t (*QairtCpuGraph_CustomConfig_FreeFn_t)(
    QairtCpuGraph_CustomConfigHandle_t);

/* Use QMX */
typedef Qairt_Status_t (*QairtCpuGraph_CustomConfig_SetUseQmxFn_t)(
    QairtCpuGraph_CustomConfigHandle_t, uint8_t);
typedef Qairt_Status_t (*QairtCpuGraph_CustomConfig_GetUseQmxFn_t)(
    QairtCpuGraph_CustomConfigHandle_t, uint8_t*);

//=============================================================================
// Public Interfaces
//=============================================================================

/**
 * @struct QairtCpuGraph_CustomConfigV1_t
 * @brief Interface struct for CPU graph custom configuration (V1).
 *        Retrieved via interface ID QAIRT_CPU_GRAPH_CUSTOM_CONFIG_ID.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtCpuGraph_CustomConfig_CreateFn_t create;
  QairtCpuGraph_CustomConfig_FreeFn_t free;
  QairtCpuGraph_CustomConfig_SetUseQmxFn_t setUseQmx;
  QairtCpuGraph_CustomConfig_GetUseQmxFn_t getUseQmx;
} QairtCpuGraph_CustomConfigV1_t;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // QAIRT_CPU_GRAPH_H
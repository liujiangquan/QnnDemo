//==============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//==============================================================================

#ifndef QAIRT_CPU_CONTEXT_H
#define QAIRT_CPU_CONTEXT_H

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
#define QAIRT_MIN_ID_CPU_CONTEXT (QAIRT_MIN_ID_CPU + 0)

#define QAIRT_CPU_CONTEXT_CUSTOM_CONFIG_ID (QAIRT_MIN_ID_CPU_CONTEXT + 0)

//=============================================================================
// Data Types
//=============================================================================

typedef QairtContext_CustomConfigHandle_t QairtCpuContext_CustomConfigHandle_t;

//=============================================================================
// Public Functions
//=============================================================================

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create a CPU context custom configuration handle.
 *
 * @param[out] customConfig A pointer to receive the created handle.
 *
 * @return QAIRT_SUCCESS on success, or an error code.
 *
 * @note Use corresponding API through QairtCpuContext_CustomConfigV1_t.
 */
Qairt_Status_t QairtCpuContext_CustomConfig_create(
    QairtCpuContext_CustomConfigHandle_t* customConfig);

/**
 * @brief Free a CPU context custom configuration handle.
 *
 * @param[in] customConfig The handle to free.
 *
 * @return QAIRT_SUCCESS on success, or an error code.
 *
 * @note Use corresponding API through QairtCpuContext_CustomConfigV1_t.
 */
Qairt_Status_t QairtCpuContext_CustomConfig_free(
    QairtCpuContext_CustomConfigHandle_t customConfig);

// Use QMX
/**
 * @brief Enable or disable QMX (Qualcomm Math Extensions) for the CPU context.
 *
 * @param[in] customConfig The custom configuration handle.
 * @param[in] useQmx Boolean flag to enable (1) or disable (0) QMX.
 *
 * @return QAIRT_SUCCESS on success, or an error code.
 *
 * @note Use corresponding API through QairtCpuContext_CustomConfigV1_t.
 */
Qairt_Status_t QairtCpuContext_CustomConfig_setUseQmx(
    QairtCpuContext_CustomConfigHandle_t customConfig, uint8_t useQmx);

/**
 * @brief Get the QMX enable/disable setting from the CPU context custom config.
 *
 * @param[in] customConfig The custom configuration handle.
 * @param[out] useQmx Pointer to receive the QMX setting.
 *
 * @return QAIRT_SUCCESS on success, or an error code.
 *
 * @note Use corresponding API through QairtCpuContext_CustomConfigV1_t.
 */
Qairt_Status_t QairtCpuContext_CustomConfig_getUseQmx(
    QairtCpuContext_CustomConfigHandle_t customConfig, uint8_t* useQmx);

//=============================================================================
// Function Pointer Typedefs
//=============================================================================

typedef Qairt_Status_t (*QairtCpuContext_CustomConfig_CreateFn_t)(
    QairtCpuContext_CustomConfigHandle_t*);
typedef Qairt_Status_t (*QairtCpuContext_CustomConfig_FreeFn_t)(
    QairtCpuContext_CustomConfigHandle_t);

/* Use QMX */
typedef Qairt_Status_t (*QairtCpuContext_CustomConfig_SetUseQmxFn_t)(
    QairtCpuContext_CustomConfigHandle_t, uint8_t);
typedef Qairt_Status_t (*QairtCpuContext_CustomConfig_GetUseQmxFn_t)(
    QairtCpuContext_CustomConfigHandle_t, uint8_t*);

//=============================================================================
// Public Interfaces
//=============================================================================

/**
 * @struct QairtCpuContext_CustomConfigV1_t
 * @brief Interface struct for CPU context custom configuration (V1).
 *        Retrieved via interface ID QAIRT_CPU_CONTEXT_CUSTOM_CONFIG_ID.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtCpuContext_CustomConfig_CreateFn_t create;
  QairtCpuContext_CustomConfig_FreeFn_t free;
  QairtCpuContext_CustomConfig_SetUseQmxFn_t setUseQmx;
  QairtCpuContext_CustomConfig_GetUseQmxFn_t getUseQmx;
} QairtCpuContext_CustomConfigV1_t;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // QAIRT_CPU_CONTEXT_H
//=============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//=============================================================================

/** @file
 *  @brief QAIRT LPAI Context API specialization.
 */

#ifndef QAIRT_LPAI_CONTEXT_H
#define QAIRT_LPAI_CONTEXT_H

#ifdef __cplusplus
#include <cstdint>
#else
#include <stdint.h>
#endif

#include "LPAI/QairtLpaiCommon.h"
#include "QairtCommon/QairtCommon.h"
#include "QairtContext/QairtContextConfig.h"

//=============================================================================
// Macros
//=============================================================================

/// Base ID for all LPAI context interface IDs.
#define QAIRT_MIN_ID_LPAI_CONTEXT (QAIRT_MIN_ID_LPAI + 0)

/// Interface ID for QairtLpaiContext_CustomConfigV1_t.
#define QAIRT_LPAI_CONTEXT_CUSTOM_CONFIG_ID (QAIRT_MIN_ID_LPAI_CONTEXT + 0)

//=============================================================================
// Data Types
//=============================================================================

/**
 * @brief Handle for an LPAI context custom configuration object.
 */
typedef QairtContext_CustomConfigHandle_t QairtLpaiContext_CustomConfigHandle_t;

//=============================================================================
// Public Functions
//=============================================================================

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create an LPAI context custom configuration handle.
 *
 * @param[out] customConfig Receives the newly created handle on success.
 *                          Must not be NULL.
 *
 * @return QAIRT_SUCCESS on success.
 *         QAIRT_COMMON_ERROR_INVALID_ARGUMENT if customConfig is NULL.
 *         QAIRT_COMMON_ERROR_MEM_ALLOC if allocation fails.
 *
 * @note Use corresponding API through QairtLpaiContext_CustomConfigV1_t.
 */
Qairt_Status_t QairtLpaiContext_CustomConfig_create(
    QairtLpaiContext_CustomConfigHandle_t* customConfig);

/**
 * @brief Free an LPAI context custom configuration handle.
 *
 * @param[in] customConfig The handle to free.
 *
 * @return QAIRT_SUCCESS on success.
 *         QAIRT_COMMON_ERROR_INVALID_HANDLE if customConfig is NULL or
 *         not a valid handle.
 *
 * @note Use corresponding API through QairtLpaiContext_CustomConfigV1_t.
 */
Qairt_Status_t QairtLpaiContext_CustomConfig_free(
    QairtLpaiContext_CustomConfigHandle_t customConfig);

/**
 * @brief Set the model buffer memory type for the LPAI context.

 * @param[in] customConfig The handle to set the configuration on.
 *
 * @param[in] memType The desired memory type for model buffers.
 *
 * @return QAIRT_SUCCESS on success.
 *         QAIRT_COMMON_ERROR_INVALID_HANDLE if customConfig is not valid.
 *
 * @note Use corresponding API through QairtLpaiContext_CustomConfigV1_t.
 */
Qairt_Status_t QairtLpaiContext_CustomConfig_setModelBufferMemType(
    QairtLpaiContext_CustomConfigHandle_t customConfig, QairtLpai_MemType_t memType);

/**
 * @brief Get the model buffer memory type from the LPAI context custom config.
 *
 * @param[in] customConfig The handle to get the configuration from.
 *
 * @param[out] memType Receives the currently configured memory type.
 *                     Must not be NULL.
 *
 * @return QAIRT_SUCCESS on success.
 *         QAIRT_COMMON_ERROR_INVALID_HANDLE if customConfig is not valid.
 *         QAIRT_COMMON_ERROR_INVALID_ARGUMENT if memType is NULL or if
 *         setModelBufferMemType() has not been called on this handle.
 *
 * @note Use corresponding API through QairtLpaiContext_CustomConfigV1_t.
 */
Qairt_Status_t QairtLpaiContext_CustomConfig_getModelBufferMemType(
    QairtLpaiContext_CustomConfigHandle_t customConfig, QairtLpai_MemType_t* memType);

/**
 * @brief Enable island mode for the LPAI context or a specific graph.
 *
 * @param[in] customConfig The handle to set the configuration on.
 *
 * @param[in] graphName NULL for context-level island mode; a null-terminated
 *                      graph name string for graph-level island mode. Both
 *                      can not be set simltaneously.
 *
 * @return QAIRT_SUCCESS on success.
 *         QAIRT_COMMON_ERROR_INVALID_HANDLE if customConfig is not valid.
 *
 * @note Use corresponding API through QairtLpaiContext_CustomConfigV1_t.
 */
Qairt_Status_t QairtLpaiContext_CustomConfig_setEnableIsland(
    QairtLpaiContext_CustomConfigHandle_t customConfig, const char* graphName);

/**
 * @brief Get the island mode graph name from the LPAI context custom config.
 *
 * @param[in] customConfig The handle to get the configuration from.
 *
 * @param[out] graphName Receives a pointer to the stored graph name string,
 *                       or NULL if context-level island mode was set.
 *                       Must not be NULL. The returned pointer is valid for
 *                       the lifetime of the handle.
 *
 * @return QAIRT_SUCCESS on success.
 *         QAIRT_COMMON_ERROR_INVALID_HANDLE if customConfig is not valid.
 *         QAIRT_COMMON_ERROR_INVALID_ARGUMENT if graphName is NULL or if
 *         setEnableIsland() has not been called on this handle.
 *
 * @note Use corresponding API through QairtLpaiContext_CustomConfigV1_t.
 */
Qairt_Status_t QairtLpaiContext_CustomConfig_getEnableIsland(
    QairtLpaiContext_CustomConfigHandle_t customConfig, const char** graphName);

//=============================================================================
// Function Pointer Typedefs
//=============================================================================

typedef Qairt_Status_t (*QairtLpaiContext_CustomConfig_CreateFn_t)(
    QairtLpaiContext_CustomConfigHandle_t*);
typedef Qairt_Status_t (*QairtLpaiContext_CustomConfig_FreeFn_t)(
    QairtLpaiContext_CustomConfigHandle_t);

/* Model Buffer Mem Type */
typedef Qairt_Status_t (*QairtLpaiContext_CustomConfig_SetModelBufferMemTypeFn_t)(
    QairtLpaiContext_CustomConfigHandle_t, QairtLpai_MemType_t);
typedef Qairt_Status_t (*QairtLpaiContext_CustomConfig_GetModelBufferMemTypeFn_t)(
    QairtLpaiContext_CustomConfigHandle_t, QairtLpai_MemType_t*);

/* Enable Island */
typedef Qairt_Status_t (*QairtLpaiContext_CustomConfig_SetEnableIslandFn_t)(
    QairtLpaiContext_CustomConfigHandle_t, const char*);
typedef Qairt_Status_t (*QairtLpaiContext_CustomConfig_GetEnableIslandFn_t)(
    QairtLpaiContext_CustomConfigHandle_t, const char**);

//=============================================================================
// Public Interfaces
//=============================================================================

/**
 * @struct QairtLpaiContext_CustomConfigV1_t
 * @brief  Interface struct for LPAI context custom configuration (V1).
 *         Retrieved via interface ID QAIRT_LPAI_CONTEXT_CUSTOM_CONFIG_ID.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtLpaiContext_CustomConfig_CreateFn_t create;
  QairtLpaiContext_CustomConfig_FreeFn_t free;
  QairtLpaiContext_CustomConfig_SetModelBufferMemTypeFn_t setModelBufferMemType;
  QairtLpaiContext_CustomConfig_GetModelBufferMemTypeFn_t getModelBufferMemType;
  QairtLpaiContext_CustomConfig_SetEnableIslandFn_t setEnableIsland;
  QairtLpaiContext_CustomConfig_GetEnableIslandFn_t getEnableIsland;
} QairtLpaiContext_CustomConfigV1_t;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // QAIRT_LPAI_CONTEXT_H

//=============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//=============================================================================

/** @file
 *  @brief QAIRT LPAI Graph API specialization.
 */

#ifndef QAIRT_LPAI_GRAPH_H
#define QAIRT_LPAI_GRAPH_H

#ifdef __cplusplus
#include <cstdint>
#else
#include <stdint.h>
#endif

#include "LPAI/QairtLpaiCommon.h"
#include "QairtCommon/QairtCommon.h"
#include "QairtGraph/QairtGraphConfig.h"

//=============================================================================
// Macros
//=============================================================================

/// Base ID for all LPAI graph interface IDs.
#define QAIRT_MIN_ID_LPAI_GRAPH (QAIRT_MIN_ID_LPAI + 2000)

/// Interface ID for QairtLpaiGraph_MemConfigV1_t.
#define QAIRT_LPAI_GRAPH_MEM_CONFIG_ID (QAIRT_MIN_ID_LPAI_GRAPH + 0)

/// Interface ID for QairtLpaiGraph_PerfCfgV1_t.
#define QAIRT_LPAI_GRAPH_PERF_CFG_ID (QAIRT_MIN_ID_LPAI_GRAPH + 1)

/// Interface ID for QairtLpaiGraph_CoreAffinityV1_t.
#define QAIRT_LPAI_GRAPH_CORE_AFFINITY_ID (QAIRT_MIN_ID_LPAI_GRAPH + 2)

/// Interface ID for QairtLpaiGraph_PrepareCfgV1_t.
#define QAIRT_LPAI_GRAPH_PREPARE_CFG_ID (QAIRT_MIN_ID_LPAI_GRAPH + 3)

/// Interface ID for QairtLpaiGraph_CustomConfigV1_t.
#define QAIRT_LPAI_GRAPH_CUSTOM_CONFIG_ID (QAIRT_MIN_ID_LPAI_GRAPH + 4)

//=============================================================================
// Data Types
//=============================================================================

/**
 * @brief Handle for an LPAI graph memory configuration object.
 *        Used for both scratch memory and persistent memory options.
 */
typedef struct _QairtLpaiGraph_MemConfigHandle_t* QairtLpaiGraph_MemConfigHandle_t;

/**
 * @brief Handle for an LPAI graph performance configuration object.
 */
typedef struct _QairtLpaiGraph_PerfCfgHandle_t* QairtLpaiGraph_PerfCfgHandle_t;

/**
 * @brief Handle for an LPAI graph core affinity configuration object.
 */
typedef struct _QairtLpaiGraph_CoreAffinityHandle_t* QairtLpaiGraph_CoreAffinityHandle_t;

/**
 * @brief Handle for an LPAI graph prepare configuration object.
 */
typedef struct _QairtLpaiGraph_PrepareCfgHandle_t* QairtLpaiGraph_PrepareCfgHandle_t;

/**
 * @brief Handle for an LPAI graph custom configuration object.
 */
typedef QairtGraph_CustomConfigHandle_t QairtLpaiGraph_CustomConfigHandle_t;

/**
 * @brief Client performance type for LPAI graph execution.
 */
typedef enum {
  QAIRT_LPAI_GRAPH_CLIENT_PERF_TYPE_REAL_TIME     = 1,
  QAIRT_LPAI_GRAPH_CLIENT_PERF_TYPE_NON_REAL_TIME = 2,
  // Unused, present to ensure 32 bits.
  QAIRT_LPAI_GRAPH_CLIENT_PERF_TYPE_UNDEFINED = 0x7FFFFFFF
} QairtLpaiGraph_ClientPerfType_t;

/**
 * @brief Core affinity type for LPAI graph execution.
 */
typedef enum {
  QAIRT_LPAI_GRAPH_CORE_AFFINITY_SOFT = 1,
  QAIRT_LPAI_GRAPH_CORE_AFFINITY_HARD = 2,
  // Unused, present to ensure 32 bits.
  QAIRT_LPAI_GRAPH_CORE_AFFINITY_UNDEFINED = 0x7FFFFFFF
} QairtLpaiGraph_CoreAffinityType_t;

//=============================================================================
// Public Functions — Memory Config
//=============================================================================

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create an LPAI graph memory configuration handle.
 *
 * @param[out] memConfig Receives the newly created handle on success.
 *                       Must not be NULL.
 *
 * @return QAIRT_SUCCESS on success.
 *         QAIRT_COMMON_ERROR_INVALID_ARGUMENT if memConfig is NULL.
 *         QAIRT_COMMON_ERROR_MEM_ALLOC if allocation fails.
 *
 * @note Use corresponding API through QairtLpaiGraph_MemConfigV1_t.
 */
Qairt_Status_t QairtLpaiGraph_MemConfig_create(QairtLpaiGraph_MemConfigHandle_t* memConfig);

/**
 * @brief Free an LPAI graph memory configuration handle.
 *
 * @param[in] memConfig The handle to free.
 *
 * @return QAIRT_SUCCESS on success.
 *         QAIRT_COMMON_ERROR_INVALID_HANDLE if memConfig is NULL or not valid.
 *
 * @note Use corresponding API through QairtLpaiGraph_MemConfigV1_t.
 */
Qairt_Status_t QairtLpaiGraph_MemConfig_free(QairtLpaiGraph_MemConfigHandle_t memConfig);

/**
 * @brief Set the memory type on a memory config handle.
 *
 * @param[in] memConfig The handle to set the configuration on.
 *
 * @param[in] memType The desired memory type.
 *
 * @return QAIRT_SUCCESS on success.
 *         QAIRT_COMMON_ERROR_INVALID_HANDLE if memConfig is not valid.
 *
 * @note Use corresponding API through QairtLpaiGraph_MemConfigV1_t.
 */
Qairt_Status_t QairtLpaiGraph_MemConfig_setMemType(QairtLpaiGraph_MemConfigHandle_t memConfig,
                                                        QairtLpai_MemType_t memType);

/**
 * @brief Get the memory type from a memory config handle.
 *
 * @param[in] memConfig The handle to get the configuration from.
 *
 * @param[out] memType Receives the configured memory type. Must not be NULL.
 *
 * @return QAIRT_SUCCESS on success.
 *         QAIRT_COMMON_ERROR_INVALID_HANDLE if memConfig is not valid.
 *         QAIRT_COMMON_ERROR_INVALID_ARGUMENT if memType is NULL.
 *
 * @note Use corresponding API through QairtLpaiGraph_MemConfigV1_t.
 */
Qairt_Status_t QairtLpaiGraph_MemConfig_getMemType(QairtLpaiGraph_MemConfigHandle_t memConfig,
                                                        QairtLpai_MemType_t* memType);

/**
 * @brief Set the buffer size on a memory config handle.
 *
 * @param[in] memConfig The handle to set the configuration on.
 *
 * @param[in] size Buffer size in bytes.
 *
 * @return QAIRT_SUCCESS on success.
 *         QAIRT_COMMON_ERROR_INVALID_HANDLE if memConfig is not valid.
 *
 * @note Use corresponding API through QairtLpaiGraph_MemConfigV1_t.
 */
Qairt_Status_t QairtLpaiGraph_MemConfig_setSize(QairtLpaiGraph_MemConfigHandle_t memConfig,
                                                     uint32_t size);

/**
 * @brief Get the buffer size from a memory config handle.
 *
 * @param[in] memConfig The handle to get the configuration from.
 *
 * @param[out] size Receives the configured buffer size in bytes. Must not be NULL.
 *
 * @return QAIRT_SUCCESS on success.
 *         QAIRT_COMMON_ERROR_INVALID_HANDLE if memConfig is not valid.
 *         QAIRT_COMMON_ERROR_INVALID_ARGUMENT if size is NULL.
 *
 * @note Use corresponding API through QairtLpaiGraph_MemConfigV1_t.
 */
Qairt_Status_t QairtLpaiGraph_MemConfig_getSize(QairtLpaiGraph_MemConfigHandle_t memConfig,
                                                     uint32_t* size);

/**
 * @brief Set the buffer address on a memory config handle.
 *
 * @param[in] memConfig The handle to set the configuration on.
 *
 * @param[in] addr Pointer to the buffer.
 *
 * @return QAIRT_SUCCESS on success.
 *         QAIRT_COMMON_ERROR_INVALID_HANDLE if memConfig is not valid.
 *
 * @note Use corresponding API through QairtLpaiGraph_MemConfigV1_t.
 */
Qairt_Status_t QairtLpaiGraph_MemConfig_setAddr(QairtLpaiGraph_MemConfigHandle_t memConfig,
                                                     void* addr);

/**
 * @brief Get the buffer address from a memory config handle.
 *
 * @param[in] memConfig The handle to get the configuration from.
 *
 * @param[out] addr Receives the configured buffer address. Must not be NULL.
 *
 * @return QAIRT_SUCCESS on success.
 *         QAIRT_COMMON_ERROR_INVALID_HANDLE if memConfig is not valid.
 *         QAIRT_COMMON_ERROR_INVALID_ARGUMENT if addr is NULL.
 *
 * @note Use corresponding API through QairtLpaiGraph_MemConfigV1_t.
 */
Qairt_Status_t QairtLpaiGraph_MemConfig_getAddr(QairtLpaiGraph_MemConfigHandle_t memConfig,
                                                     void** addr);

//=============================================================================
// Public Functions — Perf Config
//=============================================================================

/**
 * @brief Create an LPAI graph performance configuration handle.
 *
 * @param[out] perfCfg Receives the newly created handle on success.
 *                     Must not be NULL.
 *
 * @return QAIRT_SUCCESS on success.
 *         QAIRT_COMMON_ERROR_INVALID_ARGUMENT if perfCfg is NULL.
 *         QAIRT_COMMON_ERROR_MEM_ALLOC if allocation fails.
 *
 * @note Use corresponding API through QairtLpaiGraph_PerfCfgV1_t.
 */
Qairt_Status_t QairtLpaiGraph_PerfCfg_create(QairtLpaiGraph_PerfCfgHandle_t* perfCfg);

/**
 * @brief Free an LPAI graph performance configuration handle.
 *
 * @param[in] perfCfg The handle to free.
 *
 * @return QAIRT_SUCCESS on success.
 *         QAIRT_COMMON_ERROR_INVALID_HANDLE if perfCfg is NULL or not valid.
 *
 * @note Use corresponding API through QairtLpaiGraph_PerfCfgV1_t.
 */
Qairt_Status_t QairtLpaiGraph_PerfCfg_free(QairtLpaiGraph_PerfCfgHandle_t perfCfg);

/**
 * @brief Set the target frames per second on a perf config handle.
 *
 * @param[in] perfCfg The handle to set the configuration on.
 *
 * @param[in] fps Target frames per second.
 *
 * @return QAIRT_SUCCESS on success.
 *         QAIRT_COMMON_ERROR_INVALID_HANDLE if perfCfg is not valid.
 *
 * @note Use corresponding API through QairtLpaiGraph_PerfCfgV1_t.
 */
Qairt_Status_t QairtLpaiGraph_PerfCfg_setFps(QairtLpaiGraph_PerfCfgHandle_t perfCfg,
                                                  uint32_t fps);

/**
 * @brief Get the target frames per second from a perf config handle.
 *
 * @param[in] perfCfg The handle to get the configuration from.
 *
 * @param[out] fps Receives the configured fps value. Must not be NULL.
 *
 * @return QAIRT_SUCCESS on success.
 *         QAIRT_COMMON_ERROR_INVALID_HANDLE if perfCfg is not valid.
 *         QAIRT_COMMON_ERROR_INVALID_ARGUMENT if fps is NULL.
 *
 * @note Use corresponding API through QairtLpaiGraph_PerfCfgV1_t.
 */
Qairt_Status_t QairtLpaiGraph_PerfCfg_getFps(QairtLpaiGraph_PerfCfgHandle_t perfCfg,
                                                  uint32_t* fps);

/**
 * @brief Set the faster-than-real-time ratio on a perf config handle.
 *
 * @param[in] perfCfg The handle to set the configuration on.
 *
 * @param[in] ftrtRatio Faster-than-real-time ratio.
 *
 * @return QAIRT_SUCCESS on success.
 *         QAIRT_COMMON_ERROR_INVALID_HANDLE if perfCfg is not valid.
 *
 * @note Use corresponding API through QairtLpaiGraph_PerfCfgV1_t.
 */
Qairt_Status_t QairtLpaiGraph_PerfCfg_setFtrtRatio(QairtLpaiGraph_PerfCfgHandle_t perfCfg,
                                                        uint32_t ftrtRatio);

/**
 * @brief Get the faster-than-real-time ratio from a perf config handle.
 *
 * @param[in] perfCfg The handle to get the configuration from.
 *
 * @param[out] ftrtRatio Receives the configured ftrt ratio. Must not be NULL.
 *
 * @return QAIRT_SUCCESS on success.
 *         QAIRT_COMMON_ERROR_INVALID_HANDLE if perfCfg is not valid.
 *         QAIRT_COMMON_ERROR_INVALID_ARGUMENT if ftrtRatio is NULL.
 *
 * @note Use corresponding API through QairtLpaiGraph_PerfCfgV1_t.
 */
Qairt_Status_t QairtLpaiGraph_PerfCfg_getFtrtRatio(QairtLpaiGraph_PerfCfgHandle_t perfCfg,
                                                        uint32_t* ftrtRatio);

/**
 * @brief Set the client performance type on a perf config handle.
 *
 * @param[in] perfCfg The handle to set the configuration on.
 *
 * @param[in] clientType Whether the client is real-time or non-real-time.
 *
 * @return QAIRT_SUCCESS on success.
 *         QAIRT_COMMON_ERROR_INVALID_HANDLE if perfCfg is not valid.
 *
 * @note Use corresponding API through QairtLpaiGraph_PerfCfgV1_t.
 */
Qairt_Status_t QairtLpaiGraph_PerfCfg_setClientType(
    QairtLpaiGraph_PerfCfgHandle_t perfCfg, QairtLpaiGraph_ClientPerfType_t clientType);

/**
 * @brief Get the client performance type from a perf config handle.
 *
 * @param[in] perfCfg The handle to get the configuration from.
 *
 * @param[out] clientType Receives the configured client type. Must not be NULL.
 *
 * @return QAIRT_SUCCESS on success.
 *         QAIRT_COMMON_ERROR_INVALID_HANDLE if perfCfg is not valid.
 *         QAIRT_COMMON_ERROR_INVALID_ARGUMENT if clientType is NULL.
 *
 * @note Use corresponding API through QairtLpaiGraph_PerfCfgV1_t.
 */
Qairt_Status_t QairtLpaiGraph_PerfCfg_getClientType(
    QairtLpaiGraph_PerfCfgHandle_t perfCfg, QairtLpaiGraph_ClientPerfType_t* clientType);

//=============================================================================
// Public Functions — Core Affinity
//=============================================================================

/**
 * @brief Create an LPAI graph core affinity configuration handle.
 *
 * @param[out] coreAffinity Receives the newly created handle on success.
 *                          Must not be NULL.
 *
 * @return QAIRT_SUCCESS on success.
 *         QAIRT_COMMON_ERROR_INVALID_ARGUMENT if coreAffinity is NULL.
 *         QAIRT_COMMON_ERROR_MEM_ALLOC if allocation fails.
 *
 * @note Use corresponding API through QairtLpaiGraph_CoreAffinityV1_t.
 */
Qairt_Status_t QairtLpaiGraph_CoreAffinity_create(
    QairtLpaiGraph_CoreAffinityHandle_t* coreAffinity);

/**
 * @brief Free an LPAI graph core affinity configuration handle.
 *
 * @param[in] coreAffinity The handle to free.
 *
 * @return QAIRT_SUCCESS on success.
 *         QAIRT_COMMON_ERROR_INVALID_HANDLE if coreAffinity is NULL or not valid.
 *
 * @note Use corresponding API through QairtLpaiGraph_CoreAffinityV1_t.
 */
Qairt_Status_t QairtLpaiGraph_CoreAffinity_free(
    QairtLpaiGraph_CoreAffinityHandle_t coreAffinity);

/**
 * @brief Set the affinity type on a core affinity handle.
 *
 * @param[in] coreAffinity The handle to set the configuration on.
 *
 * @param[in] affinity Soft or hard affinity.
 *
 * @return QAIRT_SUCCESS on success.
 *         QAIRT_COMMON_ERROR_INVALID_HANDLE if coreAffinity is not valid.
 *
 * @note Use corresponding API through QairtLpaiGraph_CoreAffinityV1_t.
 */
Qairt_Status_t QairtLpaiGraph_CoreAffinity_setAffinity(
    QairtLpaiGraph_CoreAffinityHandle_t coreAffinity, QairtLpaiGraph_CoreAffinityType_t affinity);

/**
 * @brief Get the affinity type from a core affinity handle.
 *
 * @param[in] coreAffinity The handle to get the configuration from.
 *
 * @param[out] affinity Receives the configured affinity type. Must not be NULL.
 *
 * @return QAIRT_SUCCESS on success.
 *         QAIRT_COMMON_ERROR_INVALID_HANDLE if coreAffinity is not valid.
 *         QAIRT_COMMON_ERROR_INVALID_ARGUMENT if affinity is NULL.
 *
 * @note Use corresponding API through QairtLpaiGraph_CoreAffinityV1_t.
 */
Qairt_Status_t QairtLpaiGraph_CoreAffinity_getAffinity(
    QairtLpaiGraph_CoreAffinityHandle_t coreAffinity, QairtLpaiGraph_CoreAffinityType_t* affinity);

/**
 * @brief Set the core selection bitmask on a core affinity handle.
 *
 *        Each bit represents one core (bit 0 = core 0, bit 1 = core 1, etc.).
 *        Set to 0 for no specific preference. Only a single bit may be set;
 *        multi-core selection is not yet supported. If multiple bits are set,
 *        the behavior is undefined and may result in an error.
 *
 * @param[in] coreAffinity The handle to set the configuration on.
 *
 * @param[in] coreSelection Core selection bitmask.
 *
 * @return QAIRT_SUCCESS on success.
 *         QAIRT_COMMON_ERROR_INVALID_HANDLE if coreAffinity is not valid.
 *
 * @note Use corresponding API through QairtLpaiGraph_CoreAffinityV1_t.
 */
Qairt_Status_t QairtLpaiGraph_CoreAffinity_setCoreSelection(
    QairtLpaiGraph_CoreAffinityHandle_t coreAffinity, uint32_t coreSelection);

/**
 * @brief Get the core selection bitmask from a core affinity handle.
 *
 * @param[in] coreAffinity The handle to get the configuration from.
 *
 * @param[out] coreSelection Receives the configured core bitmask. Must not be NULL.
 *
 * @return QAIRT_SUCCESS on success.
 *         QAIRT_COMMON_ERROR_INVALID_HANDLE if coreAffinity is not valid.
 *         QAIRT_COMMON_ERROR_INVALID_ARGUMENT if coreSelection is NULL.
 *
 * @note Use corresponding API through QairtLpaiGraph_CoreAffinityV1_t.
 */
Qairt_Status_t QairtLpaiGraph_CoreAffinity_getCoreSelection(
    QairtLpaiGraph_CoreAffinityHandle_t coreAffinity, uint32_t* coreSelection);

//=============================================================================
// Public Functions — Prepare Config
//=============================================================================

/**
 * @brief Create an LPAI graph prepare configuration handle.
 *
 * @param[out] prepareCfg Receives the newly created handle on success.
 *                        Must not be NULL.
 *
 * @return QAIRT_SUCCESS on success.
 *         QAIRT_COMMON_ERROR_INVALID_ARGUMENT if prepareCfg is NULL.
 *         QAIRT_COMMON_ERROR_MEM_ALLOC if allocation fails.
 *
 * @note Use corresponding API through QairtLpaiGraph_PrepareCfgV1_t.
 */
Qairt_Status_t QairtLpaiGraph_PrepareCfg_create(QairtLpaiGraph_PrepareCfgHandle_t* prepareCfg);

/**
 * @brief Free an LPAI graph prepare configuration handle.
 *
 * @param[in] prepareCfg The handle to free.
 *
 * @return QAIRT_SUCCESS on success.
 *         QAIRT_COMMON_ERROR_INVALID_HANDLE if prepareCfg is NULL or not valid.
 *
 * @note Use corresponding API through QairtLpaiGraph_PrepareCfgV1_t.
 */
Qairt_Status_t QairtLpaiGraph_PrepareCfg_free(QairtLpaiGraph_PrepareCfgHandle_t prepareCfg);

/**
 * @brief Set the per-layer profiling flag on a prepare config handle.
 *
 * @param[in] prepareCfg The handle to set the configuration on.
 *
 * @param[in] enablePerLayer Non-zero to enable per-layer profiling during
 *                           graph preparation.
 *
 * @return QAIRT_SUCCESS on success.
 *         QAIRT_COMMON_ERROR_INVALID_HANDLE if prepareCfg is not valid.
 *
 * @note Use corresponding API through QairtLpaiGraph_PrepareCfgV1_t.
 */
Qairt_Status_t QairtLpaiGraph_PrepareCfg_setEnablePerLayer(
    QairtLpaiGraph_PrepareCfgHandle_t prepareCfg, uint32_t enablePerLayer);

/**
 * @brief Get the per-layer profiling flag from a prepare config handle.
 *
 * @param[in] prepareCfg The handle to get the configuration from.
 *
 * @param[out] enablePerLayer Receives the per-layer profiling flag.
 *                            Must not be NULL.
 *
 * @return QAIRT_SUCCESS on success.
 *         QAIRT_COMMON_ERROR_INVALID_HANDLE if prepareCfg is not valid.
 *         QAIRT_COMMON_ERROR_INVALID_ARGUMENT if enablePerLayer is NULL.
 *
 * @note Use corresponding API through QairtLpaiGraph_PrepareCfgV1_t.
 */
Qairt_Status_t QairtLpaiGraph_PrepareCfg_getEnablePerLayer(
    QairtLpaiGraph_PrepareCfgHandle_t prepareCfg, uint32_t* enablePerLayer);

/**
 * @brief Set the core selection string on a prepare config handle.
 *
 * @param[in] prepareCfg The handle to set the configuration on.
 *
 * @param[in] enableCoreSelection Null-terminated string specifying core
 *                                selection for preparation, or NULL.
 *
 * @return QAIRT_SUCCESS on success.
 *         QAIRT_COMMON_ERROR_INVALID_HANDLE if prepareCfg is not valid.
 *
 * @note Use corresponding API through QairtLpaiGraph_PrepareCfgV1_t.
 */
Qairt_Status_t QairtLpaiGraph_PrepareCfg_setEnableCoreSelection(
    QairtLpaiGraph_PrepareCfgHandle_t prepareCfg, const char* enableCoreSelection);

/**
 * @brief Get the core selection string from a prepare config handle.
 *
 * @param[in] prepareCfg The handle to get the configuration from.
 *
 * @param[out] enableCoreSelection Receives a pointer to the stored string,
 *                                 or NULL if none was set. Must not be NULL.
 *                                 The returned pointer is valid for the
 *                                 lifetime of the handle.
 *
 * @return QAIRT_SUCCESS on success.
 *         QAIRT_COMMON_ERROR_INVALID_HANDLE if prepareCfg is not valid.
 *         QAIRT_COMMON_ERROR_INVALID_ARGUMENT if enableCoreSelection is NULL.
 *
 * @note Use corresponding API through QairtLpaiGraph_PrepareCfgV1_t.
 */
Qairt_Status_t QairtLpaiGraph_PrepareCfg_getEnableCoreSelection(
    QairtLpaiGraph_PrepareCfgHandle_t prepareCfg, const char** enableCoreSelection);

//=============================================================================
// Public Functions — Graph Custom Config
//=============================================================================

/**
 * @brief Create an LPAI graph custom configuration handle.
 *
 * @param[out] customConfig Receives the newly created handle on success.
 *                          Must not be NULL.
 *
 * @return QAIRT_SUCCESS on success.
 *         QAIRT_COMMON_ERROR_INVALID_ARGUMENT if customConfig is NULL.
 *         QAIRT_COMMON_ERROR_MEM_ALLOC if allocation fails.
 *
 * @note Use corresponding API through QairtLpaiGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtLpaiGraph_CustomConfig_create(
    QairtLpaiGraph_CustomConfigHandle_t* customConfig);

/**
 * @brief Free an LPAI graph custom configuration handle.
 *
 * @param[in] customConfig The handle to free.
 *
 * @return QAIRT_SUCCESS on success.
 *         QAIRT_COMMON_ERROR_INVALID_HANDLE if customConfig is NULL or not valid.
 *
 * @note Use corresponding API through QairtLpaiGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtLpaiGraph_CustomConfig_free(
    QairtLpaiGraph_CustomConfigHandle_t customConfig);

/**
 * @brief Associate a scratch memory config with this graph custom config.
 *
 *        The mem config is copied by value. The caller retains ownership
 *        of memConfig and may free it after this call.
 *
 * @param[in] customConfig The handle to set the configuration on.
 *
 * @param[in] memConfig A valid, populated memory config handle.
 *
 * @return QAIRT_SUCCESS on success.
 *         QAIRT_COMMON_ERROR_INVALID_HANDLE if either handle is not valid.
 *
 * @note Use corresponding API through QairtLpaiGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtLpaiGraph_CustomConfig_setScratchMem(
    QairtLpaiGraph_CustomConfigHandle_t customConfig, QairtLpaiGraph_MemConfigHandle_t memConfig);

/**
 * @brief Get a non-owning reference to the scratch memory config.
 *
 *        The returned handle is valid for the lifetime of customConfig.
 *        Do not free it.
 *
 * @param[in] customConfig The handle to get the configuration from.
 *
 * @param[out] memConfig Receives a non-owning handle. Must not be NULL.
 *
 * @return QAIRT_SUCCESS on success.
 *         QAIRT_COMMON_ERROR_INVALID_HANDLE if customConfig is not valid.
 *         QAIRT_COMMON_ERROR_INVALID_ARGUMENT if memConfig is NULL or if
 *         setScratchMem() has not been called on this handle.
 *
 * @note Use corresponding API through QairtLpaiGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtLpaiGraph_CustomConfig_getScratchMem(
    QairtLpaiGraph_CustomConfigHandle_t customConfig, QairtLpaiGraph_MemConfigHandle_t* memConfig);

/**
 * @brief Associate a persistent memory config with this graph custom config.
 *
 *        The mem config is copied by value. The caller retains ownership
 *        of memConfig and may free it after this call.
 *
 * @param[in] customConfig The handle to set the configuration on.
 *
 * @param[in] memConfig A valid, populated memory config handle.
 *
 * @return QAIRT_SUCCESS on success.
 *         QAIRT_COMMON_ERROR_INVALID_HANDLE if either handle is not valid.
 *
 * @note Use corresponding API through QairtLpaiGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtLpaiGraph_CustomConfig_setPersistentMem(
    QairtLpaiGraph_CustomConfigHandle_t customConfig, QairtLpaiGraph_MemConfigHandle_t memConfig);

/**
 * @brief Get a non-owning reference to the persistent memory config.
 *
 *        The returned handle is valid for the lifetime of customConfig.
 *        Do not free it.
 *
 * @param[in] customConfig The handle to get the configuration from.
 *
 * @param[out] memConfig Receives a non-owning handle. Must not be NULL.
 *
 * @return QAIRT_SUCCESS on success.
 *         QAIRT_COMMON_ERROR_INVALID_HANDLE if customConfig is not valid.
 *         QAIRT_COMMON_ERROR_INVALID_ARGUMENT if memConfig is NULL or if
 *         setPersistentMem() has not been called on this handle.
 *
 * @note Use corresponding API through QairtLpaiGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtLpaiGraph_CustomConfig_getPersistentMem(
    QairtLpaiGraph_CustomConfigHandle_t customConfig, QairtLpaiGraph_MemConfigHandle_t* memConfig);

/**
 * @brief Associate a performance config with this graph custom config.
 *
 *        The perf config is copied by value. The caller retains ownership
 *        of perfCfg and may free it after this call.
 *
 * @param[in] customConfig The handle to set the configuration on.
 *
 * @param[in] perfCfg A valid, populated perf config handle.
 *
 * @return QAIRT_SUCCESS on success.
 *         QAIRT_COMMON_ERROR_INVALID_HANDLE if either handle is not valid.
 *
 * @note Use corresponding API through QairtLpaiGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtLpaiGraph_CustomConfig_setPerfCfg(
    QairtLpaiGraph_CustomConfigHandle_t customConfig, QairtLpaiGraph_PerfCfgHandle_t perfCfg);

/**
 * @brief Get a non-owning reference to the performance config.
 *
 *        The returned handle is valid for the lifetime of customConfig.
 *        Do not free it.
 *
 * @param[in] customConfig The handle to get the configuration from.
 *
 * @param[out] perfCfg Receives a non-owning handle. Must not be NULL.
 *
 * @return QAIRT_SUCCESS on success.
 *         QAIRT_COMMON_ERROR_INVALID_HANDLE if customConfig is not valid.
 *         QAIRT_COMMON_ERROR_INVALID_ARGUMENT if perfCfg is NULL or if
 *         setPerfCfg() has not been called on this handle.
 *
 * @note Use corresponding API through QairtLpaiGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtLpaiGraph_CustomConfig_getPerfCfg(
    QairtLpaiGraph_CustomConfigHandle_t customConfig, QairtLpaiGraph_PerfCfgHandle_t* perfCfg);

/**
 * @brief Associate a core affinity config with this graph custom config.
 *
 *        The core affinity config is copied by value. The caller retains
 *        ownership of coreAffinity and may free it after this call.
 *
 * @param[in] customConfig The handle to set the configuration on.
 *
 * @param[in] coreAffinity A valid, populated core affinity handle.
 *
 * @return QAIRT_SUCCESS on success.
 *         QAIRT_COMMON_ERROR_INVALID_HANDLE if either handle is not valid.
 *
 * @note Use corresponding API through QairtLpaiGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtLpaiGraph_CustomConfig_setCoreAffinity(
    QairtLpaiGraph_CustomConfigHandle_t customConfig,
    QairtLpaiGraph_CoreAffinityHandle_t coreAffinity);

/**
 * @brief Get a non-owning reference to the core affinity config.
 *
 *        The returned handle is valid for the lifetime of customConfig.
 *        Do not free it.
 *
 * @param[in] customConfig The handle to get the configuration from.
 *
 * @param[out] coreAffinity Receives a non-owning handle. Must not be NULL.
 *
 * @return QAIRT_SUCCESS on success.
 *         QAIRT_COMMON_ERROR_INVALID_HANDLE if customConfig is not valid.
 *         QAIRT_COMMON_ERROR_INVALID_ARGUMENT if coreAffinity is NULL or if
 *         setCoreAffinity() has not been called on this handle.
 *
 * @note Use corresponding API through QairtLpaiGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtLpaiGraph_CustomConfig_getCoreAffinity(
    QairtLpaiGraph_CustomConfigHandle_t customConfig,
    QairtLpaiGraph_CoreAffinityHandle_t* coreAffinity);

/**
 * @brief Pause graph execution. No additional config data is required.
 *
 * @param[in] customConfig The handle to set the configuration on.
 *
 * @return QAIRT_SUCCESS on success.
 *         QAIRT_COMMON_ERROR_INVALID_HANDLE if customConfig is not valid.
 *
 * @note Use corresponding API through QairtLpaiGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtLpaiGraph_CustomConfig_setPauseExecution(
    QairtLpaiGraph_CustomConfigHandle_t customConfig);

/**
 * @brief Resume graph execution. No additional config data is required.
 *
 * @param[in] customConfig The handle to set the configuration on.
 *
 * @return QAIRT_SUCCESS on success.
 *         QAIRT_COMMON_ERROR_INVALID_HANDLE if customConfig is not valid.
 *
 * @note Use corresponding API through QairtLpaiGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtLpaiGraph_CustomConfig_setResumeExecution(
    QairtLpaiGraph_CustomConfigHandle_t customConfig);

// -------------------------------
// Graph Preparation Configuration
// -------------------------------

/**
 * @brief Associate a prepare config with this graph custom config.
 *
 *        The prepare config is copied by value. The caller retains ownership
 *        of prepareCfg and may free it after this call.
 *
 * @param[in] customConfig The handle to set the configuration on.
 *
 * @param[in] prepareCfg A valid, populated prepare config handle.
 *
 * @return QAIRT_SUCCESS on success.
 *         QAIRT_COMMON_ERROR_INVALID_HANDLE if either handle is not valid.
 *
 * @note Use corresponding API through QairtLpaiGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtLpaiGraph_CustomConfig_setPrepareCfg(
    QairtLpaiGraph_CustomConfigHandle_t customConfig, QairtLpaiGraph_PrepareCfgHandle_t prepareCfg);

/**
 * @brief Get a non-owning reference to the prepare config.
 *
 *        The returned handle is valid for the lifetime of customConfig.
 *        Do not free it.
 *
 * @param[in] customConfig The handle to get the configuration from.
 *
 * @param[out] prepareCfg Receives a non-owning handle. Must not be NULL.
 *
 * @return QAIRT_SUCCESS on success.
 *         QAIRT_COMMON_ERROR_INVALID_HANDLE if customConfig is not valid.
 *         QAIRT_COMMON_ERROR_INVALID_ARGUMENT if prepareCfg is NULL or if
 *         setPrepareCfg() has not been called on this handle.
 *
 * @note Use corresponding API through QairtLpaiGraph_CustomConfigV1_t.
 */
Qairt_Status_t QairtLpaiGraph_CustomConfig_getPrepareCfg(
    QairtLpaiGraph_CustomConfigHandle_t customConfig,
    QairtLpaiGraph_PrepareCfgHandle_t* prepareCfg);

//=============================================================================
// Function Pointer Typedefs — Memory Config
//=============================================================================

typedef Qairt_Status_t (*QairtLpaiGraph_MemConfig_CreateFn_t)(
    QairtLpaiGraph_MemConfigHandle_t*);
typedef Qairt_Status_t (*QairtLpaiGraph_MemConfig_FreeFn_t)(QairtLpaiGraph_MemConfigHandle_t);
typedef Qairt_Status_t (*QairtLpaiGraph_MemConfig_SetMemTypeFn_t)(
    QairtLpaiGraph_MemConfigHandle_t, QairtLpai_MemType_t);
typedef Qairt_Status_t (*QairtLpaiGraph_MemConfig_GetMemTypeFn_t)(
    QairtLpaiGraph_MemConfigHandle_t, QairtLpai_MemType_t*);
typedef Qairt_Status_t (*QairtLpaiGraph_MemConfig_SetSizeFn_t)(
    QairtLpaiGraph_MemConfigHandle_t, uint32_t);
typedef Qairt_Status_t (*QairtLpaiGraph_MemConfig_GetSizeFn_t)(
    QairtLpaiGraph_MemConfigHandle_t, uint32_t*);
typedef Qairt_Status_t (*QairtLpaiGraph_MemConfig_SetAddrFn_t)(
    QairtLpaiGraph_MemConfigHandle_t, void*);
typedef Qairt_Status_t (*QairtLpaiGraph_MemConfig_GetAddrFn_t)(
    QairtLpaiGraph_MemConfigHandle_t, void**);

//=============================================================================
// Function Pointer Typedefs — Perf Config
//=============================================================================

typedef Qairt_Status_t (*QairtLpaiGraph_PerfCfg_CreateFn_t)(QairtLpaiGraph_PerfCfgHandle_t*);
typedef Qairt_Status_t (*QairtLpaiGraph_PerfCfg_FreeFn_t)(QairtLpaiGraph_PerfCfgHandle_t);
typedef Qairt_Status_t (*QairtLpaiGraph_PerfCfg_SetFpsFn_t)(QairtLpaiGraph_PerfCfgHandle_t,
                                                                 uint32_t);
typedef Qairt_Status_t (*QairtLpaiGraph_PerfCfg_GetFpsFn_t)(QairtLpaiGraph_PerfCfgHandle_t,
                                                                 uint32_t*);
typedef Qairt_Status_t (*QairtLpaiGraph_PerfCfg_SetFtrtRatioFn_t)(
    QairtLpaiGraph_PerfCfgHandle_t, uint32_t);
typedef Qairt_Status_t (*QairtLpaiGraph_PerfCfg_GetFtrtRatioFn_t)(
    QairtLpaiGraph_PerfCfgHandle_t, uint32_t*);
typedef Qairt_Status_t (*QairtLpaiGraph_PerfCfg_SetClientTypeFn_t)(
    QairtLpaiGraph_PerfCfgHandle_t, QairtLpaiGraph_ClientPerfType_t);
typedef Qairt_Status_t (*QairtLpaiGraph_PerfCfg_GetClientTypeFn_t)(
    QairtLpaiGraph_PerfCfgHandle_t, QairtLpaiGraph_ClientPerfType_t*);

//=============================================================================
// Function Pointer Typedefs — Core Affinity
//=============================================================================

typedef Qairt_Status_t (*QairtLpaiGraph_CoreAffinity_CreateFn_t)(
    QairtLpaiGraph_CoreAffinityHandle_t*);
typedef Qairt_Status_t (*QairtLpaiGraph_CoreAffinity_FreeFn_t)(
    QairtLpaiGraph_CoreAffinityHandle_t);
typedef Qairt_Status_t (*QairtLpaiGraph_CoreAffinity_SetAffinityFn_t)(
    QairtLpaiGraph_CoreAffinityHandle_t, QairtLpaiGraph_CoreAffinityType_t);
typedef Qairt_Status_t (*QairtLpaiGraph_CoreAffinity_GetAffinityFn_t)(
    QairtLpaiGraph_CoreAffinityHandle_t, QairtLpaiGraph_CoreAffinityType_t*);
typedef Qairt_Status_t (*QairtLpaiGraph_CoreAffinity_SetCoreSelectionFn_t)(
    QairtLpaiGraph_CoreAffinityHandle_t, uint32_t);
typedef Qairt_Status_t (*QairtLpaiGraph_CoreAffinity_GetCoreSelectionFn_t)(
    QairtLpaiGraph_CoreAffinityHandle_t, uint32_t*);

//=============================================================================
// Function Pointer Typedefs — Prepare Config
//=============================================================================

typedef Qairt_Status_t (*QairtLpaiGraph_PrepareCfg_CreateFn_t)(
    QairtLpaiGraph_PrepareCfgHandle_t*);
typedef Qairt_Status_t (*QairtLpaiGraph_PrepareCfg_FreeFn_t)(
    QairtLpaiGraph_PrepareCfgHandle_t);
typedef Qairt_Status_t (*QairtLpaiGraph_PrepareCfg_SetEnablePerLayerFn_t)(
    QairtLpaiGraph_PrepareCfgHandle_t, uint32_t);
typedef Qairt_Status_t (*QairtLpaiGraph_PrepareCfg_GetEnablePerLayerFn_t)(
    QairtLpaiGraph_PrepareCfgHandle_t, uint32_t*);
typedef Qairt_Status_t (*QairtLpaiGraph_PrepareCfg_SetEnableCoreSelectionFn_t)(
    QairtLpaiGraph_PrepareCfgHandle_t, const char*);
typedef Qairt_Status_t (*QairtLpaiGraph_PrepareCfg_GetEnableCoreSelectionFn_t)(
    QairtLpaiGraph_PrepareCfgHandle_t, const char**);

//=============================================================================
// Function Pointer Typedefs — Graph Custom Config
//=============================================================================

typedef Qairt_Status_t (*QairtLpaiGraph_CustomConfig_CreateFn_t)(
    QairtLpaiGraph_CustomConfigHandle_t*);
typedef Qairt_Status_t (*QairtLpaiGraph_CustomConfig_FreeFn_t)(
    QairtLpaiGraph_CustomConfigHandle_t);
typedef Qairt_Status_t (*QairtLpaiGraph_CustomConfig_SetScratchMemFn_t)(
    QairtLpaiGraph_CustomConfigHandle_t, QairtLpaiGraph_MemConfigHandle_t);
typedef Qairt_Status_t (*QairtLpaiGraph_CustomConfig_GetScratchMemFn_t)(
    QairtLpaiGraph_CustomConfigHandle_t, QairtLpaiGraph_MemConfigHandle_t*);
typedef Qairt_Status_t (*QairtLpaiGraph_CustomConfig_SetPersistentMemFn_t)(
    QairtLpaiGraph_CustomConfigHandle_t, QairtLpaiGraph_MemConfigHandle_t);
typedef Qairt_Status_t (*QairtLpaiGraph_CustomConfig_GetPersistentMemFn_t)(
    QairtLpaiGraph_CustomConfigHandle_t, QairtLpaiGraph_MemConfigHandle_t*);
typedef Qairt_Status_t (*QairtLpaiGraph_CustomConfig_SetPerfCfgFn_t)(
    QairtLpaiGraph_CustomConfigHandle_t, QairtLpaiGraph_PerfCfgHandle_t);
typedef Qairt_Status_t (*QairtLpaiGraph_CustomConfig_GetPerfCfgFn_t)(
    QairtLpaiGraph_CustomConfigHandle_t, QairtLpaiGraph_PerfCfgHandle_t*);
typedef Qairt_Status_t (*QairtLpaiGraph_CustomConfig_SetCoreAffinityFn_t)(
    QairtLpaiGraph_CustomConfigHandle_t, QairtLpaiGraph_CoreAffinityHandle_t);
typedef Qairt_Status_t (*QairtLpaiGraph_CustomConfig_GetCoreAffinityFn_t)(
    QairtLpaiGraph_CustomConfigHandle_t, QairtLpaiGraph_CoreAffinityHandle_t*);
typedef Qairt_Status_t (*QairtLpaiGraph_CustomConfig_SetPauseExecutionFn_t)(
    QairtLpaiGraph_CustomConfigHandle_t);
typedef Qairt_Status_t (*QairtLpaiGraph_CustomConfig_SetResumeExecutionFn_t)(
    QairtLpaiGraph_CustomConfigHandle_t);
typedef Qairt_Status_t (*QairtLpaiGraph_CustomConfig_SetPrepareCfgFn_t)(
    QairtLpaiGraph_CustomConfigHandle_t, QairtLpaiGraph_PrepareCfgHandle_t);
typedef Qairt_Status_t (*QairtLpaiGraph_CustomConfig_GetPrepareCfgFn_t)(
    QairtLpaiGraph_CustomConfigHandle_t, QairtLpaiGraph_PrepareCfgHandle_t*);

//=============================================================================
// Public Interfaces
//=============================================================================

/**
 * @struct QairtLpaiGraph_MemConfigV1_t
 * @brief  Interface struct for LPAI graph memory configuration (V1).
 *         Retrieved via interface ID QAIRT_LPAI_GRAPH_MEM_CONFIG_ID.
 *         Used for both scratch memory and persistent memory options.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtLpaiGraph_MemConfig_CreateFn_t create;
  QairtLpaiGraph_MemConfig_FreeFn_t free;
  QairtLpaiGraph_MemConfig_SetMemTypeFn_t setMemType;
  QairtLpaiGraph_MemConfig_GetMemTypeFn_t getMemType;
  QairtLpaiGraph_MemConfig_SetSizeFn_t setSize;
  QairtLpaiGraph_MemConfig_GetSizeFn_t getSize;
  QairtLpaiGraph_MemConfig_SetAddrFn_t setAddr;
  QairtLpaiGraph_MemConfig_GetAddrFn_t getAddr;
} QairtLpaiGraph_MemConfigV1_t;

/**
 * @struct QairtLpaiGraph_PerfCfgV1_t
 * @brief  Interface struct for LPAI graph performance configuration (V1).
 *         Retrieved via interface ID QAIRT_LPAI_GRAPH_PERF_CFG_ID.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtLpaiGraph_PerfCfg_CreateFn_t create;
  QairtLpaiGraph_PerfCfg_FreeFn_t free;
  QairtLpaiGraph_PerfCfg_SetFpsFn_t setFps;
  QairtLpaiGraph_PerfCfg_GetFpsFn_t getFps;
  QairtLpaiGraph_PerfCfg_SetFtrtRatioFn_t setFtrtRatio;
  QairtLpaiGraph_PerfCfg_GetFtrtRatioFn_t getFtrtRatio;
  QairtLpaiGraph_PerfCfg_SetClientTypeFn_t setClientType;
  QairtLpaiGraph_PerfCfg_GetClientTypeFn_t getClientType;
} QairtLpaiGraph_PerfCfgV1_t;

/**
 * @struct QairtLpaiGraph_CoreAffinityV1_t
 * @brief  Interface struct for LPAI graph core affinity configuration (V1).
 *         Retrieved via interface ID QAIRT_LPAI_GRAPH_CORE_AFFINITY_ID.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtLpaiGraph_CoreAffinity_CreateFn_t create;
  QairtLpaiGraph_CoreAffinity_FreeFn_t free;
  QairtLpaiGraph_CoreAffinity_SetAffinityFn_t setAffinity;
  QairtLpaiGraph_CoreAffinity_GetAffinityFn_t getAffinity;
  QairtLpaiGraph_CoreAffinity_SetCoreSelectionFn_t setCoreSelection;
  QairtLpaiGraph_CoreAffinity_GetCoreSelectionFn_t getCoreSelection;
} QairtLpaiGraph_CoreAffinityV1_t;

/**
 * @struct QairtLpaiGraph_PrepareCfgV1_t
 * @brief  Interface struct for LPAI graph prepare configuration (V1).
 *         Retrieved via interface ID QAIRT_LPAI_GRAPH_PREPARE_CFG_ID.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtLpaiGraph_PrepareCfg_CreateFn_t create;
  QairtLpaiGraph_PrepareCfg_FreeFn_t free;
  QairtLpaiGraph_PrepareCfg_SetEnablePerLayerFn_t setEnablePerLayer;
  QairtLpaiGraph_PrepareCfg_GetEnablePerLayerFn_t getEnablePerLayer;
  QairtLpaiGraph_PrepareCfg_SetEnableCoreSelectionFn_t setEnableCoreSelection;
  QairtLpaiGraph_PrepareCfg_GetEnableCoreSelectionFn_t getEnableCoreSelection;
} QairtLpaiGraph_PrepareCfgV1_t;

/**
 * @struct QairtLpaiGraph_CustomConfigV1_t
 * @brief  Interface struct for LPAI graph custom configuration (V1).
 *         Retrieved via interface ID QAIRT_LPAI_GRAPH_CUSTOM_CONFIG_ID.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtLpaiGraph_CustomConfig_CreateFn_t create;
  QairtLpaiGraph_CustomConfig_FreeFn_t free;
  QairtLpaiGraph_CustomConfig_SetScratchMemFn_t setScratchMem;
  QairtLpaiGraph_CustomConfig_GetScratchMemFn_t getScratchMem;
  QairtLpaiGraph_CustomConfig_SetPersistentMemFn_t setPersistentMem;
  QairtLpaiGraph_CustomConfig_GetPersistentMemFn_t getPersistentMem;
  QairtLpaiGraph_CustomConfig_SetPerfCfgFn_t setPerfCfg;
  QairtLpaiGraph_CustomConfig_GetPerfCfgFn_t getPerfCfg;
  QairtLpaiGraph_CustomConfig_SetCoreAffinityFn_t setCoreAffinity;
  QairtLpaiGraph_CustomConfig_GetCoreAffinityFn_t getCoreAffinity;
  QairtLpaiGraph_CustomConfig_SetPauseExecutionFn_t setPauseExecution;
  QairtLpaiGraph_CustomConfig_SetResumeExecutionFn_t setResumeExecution;
  QairtLpaiGraph_CustomConfig_SetPrepareCfgFn_t setPrepareCfg;
  QairtLpaiGraph_CustomConfig_GetPrepareCfgFn_t getPrepareCfg;
} QairtLpaiGraph_CustomConfigV1_t;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // QAIRT_LPAI_GRAPH_H

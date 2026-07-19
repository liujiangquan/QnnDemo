//=============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//=============================================================================

/**
 * @file
 * @brief   Context Config component API
 *
 *          A header which contains the QAIRT context config component
 *          and the interfaces it implements.
 */

#ifndef QAISW_QAIRTCONTEXTCONFIG_H
#define QAISW_QAIRTCONTEXTCONFIG_H

#ifdef __cplusplus
#include <cstddef>
#include <cstdint>
extern "C" {
#else
#include <stddef.h>
#include <stdint.h>
#endif

#include "QairtContextCommon.h"

//=============================================================================
// Macros
//=============================================================================

//=============================================================================
// Data Types
//=============================================================================
/// Define Qairt Context Custom Config handle
typedef struct _QairtContext_CustomConfigHandle_t* QairtContext_CustomConfigHandle_t;

/// Define Qairt Context Async Execution depth handle
typedef struct _QairtContext_AsyncExecutionDepthHandle_t*
    QairtContext_AsyncExecutionDepthHandle_t;

/// Define OemKey handle
typedef struct _QairtContext_OemHandle_t* QairtContext_OemHandle_t;

/// Define Qairt Context Configuration handle
typedef struct _QairtContext_ConfigHandle_t* QairtContext_ConfigHandle_t;

/// Define Qairt Context Binary Compatibility Type
typedef enum {
  /// A binary cache is compatible if it could run on the device. This is the
  /// default.
  QAIRT_CONTEXT_BINARY_COMPATIBILITY_PERMISSIVE = 0,
  /// A binary cache is compatible if it could run on the device and fully
  /// utilize hardware capability, otherwise QairtContext_CreateFromBinary
  /// may return QAIRT_CONTEXT_ERROR_BINARY_SUBOPTIMAL.
  QAIRT_CONTEXT_BINARY_COMPATIBILITY_STRICT = 1,
  // Unused, present to ensure 32 bits
  QAIRT_CONTEXT_BINARY_COMPATIBILITY_TYPE_UNDEFINED = 0x7FFFFFF
} QairtContext_BinaryCompatibilityType_t;

//=============================================================================
// Public Functions
//=============================================================================

/**
 * @brief Create an async execution depth handle.
 *
 * @param[out] asyncDepthHandle A handle to the created async execution depth.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Async execution depth was successfully
 *           created.
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: _asyncDepthHandle_ is
 *           NULL.
 *         - QAIRT_CONTEXT_ERROR_MEM_ALLOC: Memory allocation error while
 *           creating async execution depth.
 *
 * @note Use corresponding API through QairtContext_AsyncExecutionDepthV1_t.
 */
Qairt_Status_t QairtContext_AsyncExecutionCreate(
    QairtContext_AsyncExecutionDepthHandle_t* asyncDepthHandle);

/**
 * @brief Free an async execution depth handle and associated resources.
 *
 * @param[in] asyncDepthHandle A handle to a created async execution depth.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Async execution depth was successfully freed.
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: _asyncDepthHandle_ is not
 *           a valid handle.
 *         - QAIRT_CONTEXT_ERROR_MEM_ALLOC: Memory deallocation error.
 *
 * @note Use corresponding API through QairtContext_AsyncExecutionDepthV1_t.
 */
Qairt_Status_t QairtContext_AsyncExecutionFree(
    QairtContext_AsyncExecutionDepthHandle_t asyncDepthHandle);

/**
 * @brief Set the depth value for async execution.
 *
 * @param[in] asyncDepthHandle A handle to a created async execution depth.
 *
 * @param[in] depth The depth value to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Depth was successfully set.
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: _asyncDepthHandle_ is not
 *           a valid handle.
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: _depth_ is invalid.
 *
 * @note Use corresponding API through QairtContext_AsyncExecutionDepthV1_t.
 */
Qairt_Status_t QairtContext_AsyncExecutionSetDepth(
    QairtContext_AsyncExecutionDepthHandle_t asyncDepthHandle, uint32_t depth);

/**
 * @brief Get the depth value for async execution.
 *
 * @param[in] asyncDepthHandle A handle to a created async execution depth.
 *
 * @param[out] depth Pointer to retrieve the depth value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Depth was successfully retrieved.
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: _asyncDepthHandle_ is not
 *           a valid handle.
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: _depth_ is NULL.
 *
 * @note Use corresponding API through QairtContext_AsyncExecutionDepthV1_t.
 */
Qairt_Status_t QairtContext_AsyncExecutionGetDepth(
    QairtContext_AsyncExecutionDepthHandle_t asyncDepthHandle, uint32_t* depth);

/**
 * @brief Create a context configuration handle.
 *
 * @param[out] configHandle A handle to the created context configuration.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Context configuration was successfully
 *           created.
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: _configHandle_ is NULL.
 *         - QAIRT_CONTEXT_ERROR_MEM_ALLOC: Memory allocation error while
 *           creating context configuration.
 *
 * @note Use corresponding API through QairtContext_ConfigV1_t.
 */
Qairt_Status_t QairtContext_Config_create(QairtContext_ConfigHandle_t* configHandle);

/**
 * @brief Free a context configuration handle and associated resources.
 *
 * @param[in] configHandle A handle to a created context configuration.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Context configuration was successfully freed.
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: _configHandle_ is not a
 *           valid handle.
 *         - QAIRT_CONTEXT_ERROR_MEM_ALLOC: Memory deallocation error.
 *
 * @note Use corresponding API through QairtContext_ConfigV1_t.
 */
Qairt_Status_t QairtContext_Config_free(QairtContext_ConfigHandle_t configHandle);

/**
 * @brief Set custom configurations for a context configuration.
 *
 * @param[in] configHandle A handle to a created context configuration.
 *
 * @param[in] customConfigHandles An array of custom configuration handles
 *                                to set on the context configuration.
 *
 * @param[in] numCustomConfigs The number of custom configuration handles
 *                             in the _customConfigHandles_ array.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Custom configurations were successfully set.
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: _configHandle_ is not a
 *           valid handle, or one or more handles in _customConfigHandles_
 *           are not valid handles.
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: _customConfigHandles_ is
 *           NULL when _numCustomConfigs_ is greater than zero.
 *
 * @note Use corresponding API through QairtContext_ConfigV1_t.
 */
Qairt_Status_t QairtContext_Config_setCustomConfigs(
    QairtContext_ConfigHandle_t configHandle,
    QairtContext_CustomConfigHandle_t* customConfigHandles,
    uint32_t numCustomConfigs);

/**
 * @brief Get the number of custom configurations set on a context configuration.
 *
 * @param[in] configHandle A handle to a created context configuration.
 *
 * @param[out] numCustomConfigs The number of custom configuration handles
 *                              currently set on the context configuration.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Count was successfully retrieved.
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: _configHandle_ is not a
 *           valid handle.
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: _numCustomConfigs_ is NULL.
 *
 * @note Use corresponding API through QairtContext_ConfigV1_t.
 */
Qairt_Status_t QairtContext_Config_getNumCustomConfigs(
    QairtContext_ConfigHandle_t configHandle, uint32_t* numCustomConfigs);

/**
 * @brief Get the custom configuration handle at a given index.
 *
 * @param[in] configHandle A handle to a created context configuration.
 *
 * @param[in] index Index of the custom configuration to retrieve. Must be
 *                  less than the value returned by
 *                  QairtContext_Config_getNumCustomConfigs().
 *
 * @param[out] customConfigHandle The retrieved custom configuration handle.
 *                                Should not be freed.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Handle was successfully retrieved.
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: _configHandle_ is not a
 *           valid handle.
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: _customConfigHandle_ is
 *           NULL or _index_ is out of range.
 *
 * @note Use corresponding API through QairtContext_ConfigV1_t.
 */
Qairt_Status_t QairtContext_Config_getCustomConfigAt(
    QairtContext_ConfigHandle_t configHandle,
    uint32_t index,
    QairtContext_CustomConfigHandle_t* customConfigHandle);

/**
 * @brief Set the priority for a context configuration.
 *
 * @param[in] configHandle A handle to a created context configuration.
 *
 * @param[in] priority The priority value to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Priority was successfully set.
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: _configHandle_ is not a
 *           valid handle.
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: _priority_ is invalid.
 *
 * @note Use corresponding API through QairtContext_ConfigV1_t.
 */
Qairt_Status_t QairtContext_Config_setPriority(QairtContext_ConfigHandle_t configHandle,
                                                    Qairt_Priority_t priority);

/**
 * @brief Get the priority from a context configuration.
 *
 * @param[in] configHandle A handle to a created context configuration.
 *
 * @param[out] priority Pointer to retrieve the priority value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Priority was successfully retrieved.
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: _configHandle_ is not a
 *           valid handle.
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: _priority_ is NULL.
 *
 * @note Use corresponding API through QairtContext_ConfigV1_t.
 */
Qairt_Status_t QairtContext_Config_getPriority(QairtContext_ConfigHandle_t configHandle,
                                                    Qairt_Priority_t* priority);

/**
 * @brief Set the OEM key for a context configuration.
 *
 * @param[in] configHandle A handle to a created context configuration.
 *
 * @param[in] oemKey The OEM key string to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: OEM key was successfully set.
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: _configHandle_ is not a
 *           valid handle.
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: _oemKey_ is NULL.
 *
 * @note Use corresponding API through QairtContext_ConfigV1_t.
 */
Qairt_Status_t QairtContext_Config_setOemKey(QairtContext_ConfigHandle_t configHandle,
                                                  const char* oemKey);

/**
 * @brief Get the OEM key from a context configuration.
 *
 * @param[in] configHandle A handle to a created context configuration.
 *
 * @param[out] oemKey Pointer to retrieve the OEM key string.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: OEM key was successfully retrieved.
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: _configHandle_ is not a
 *           valid handle.
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: _oemKey_ is NULL.
 *
 * @note Use corresponding API through QairtContext_ConfigV1_t.
 */
Qairt_Status_t QairtContext_Config_getOemKey(QairtContext_ConfigHandle_t configHandle,
                                                  const char** oemKey);

/**
 * @brief Set the async queue depth for a context configuration.
 *
 * @param[in] configHandle A handle to a created context configuration.
 *
 * @param[in] depthHandle A handle to an async execution depth.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Async queue depth was successfully set.
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: _configHandle_ or
 *           _depthHandle_ is not a valid handle.
 *
 * @note Use corresponding API through QairtContext_ConfigV1_t.
 */
Qairt_Status_t QairtContext_Config_setAsyncQueueDepth(
    QairtContext_ConfigHandle_t configHandle,
    QairtContext_AsyncExecutionDepthHandle_t depthHandle);

/**
 * @brief Get the async queue depth from a context configuration.
 *
 * @param[in] configHandle A handle to a created context configuration.
 *
 * @param[out] depthHandle Pointer to retrieve the async execution depth handle.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Async queue depth was successfully retrieved.
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: _configHandle_ is not a
 *           valid handle.
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: _depthHandle_ is NULL.
 *
 * @note Use corresponding API through QairtContext_ConfigV1_t.
 */
Qairt_Status_t QairtContext_Config_getAsyncQueueDepth(
    QairtContext_ConfigHandle_t configHandle,
    QairtContext_AsyncExecutionDepthHandle_t* depthHandle);

/**
 * @brief Set the memory limit hint for a context configuration.
 *
 * @param[in] configHandle A handle to a created context configuration.
 *
 * @param[in] memoryLimitHint The memory limit hint value in bytes.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Memory limit hint was successfully set.
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: _configHandle_ is not a
 *           valid handle.
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: _memoryLimitHint_ is
 *           invalid.
 *
 * @note Use corresponding API through QairtContext_ConfigV1_t.
 */
Qairt_Status_t QairtContext_Config_setMemoryLimitHint(
    QairtContext_ConfigHandle_t configHandle, uint64_t memoryLimitHint);

/**
 * @brief Get the memory limit hint from a context configuration.
 *
 * @param[in] configHandle A handle to a created context configuration.
 *
 * @param[out] memoryLimitHint Pointer to retrieve the memory limit hint value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Memory limit hint was successfully retrieved.
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: _configHandle_ is not a
 *           valid handle.
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: _memoryLimitHint_ is
 *           NULL.
 *
 * @note Use corresponding API through QairtContext_ConfigV1_t.
 */
Qairt_Status_t QairtContext_Config_getMemoryLimitHint(
    QairtContext_ConfigHandle_t configHandle, uint64_t* memoryLimitHint);

/**
 * @brief Set the persistent binary flag for a context configuration.
 *
 * @param[in] configHandle A handle to a created context configuration.
 *
 * @param[in] isPersistentBinary Flag indicating whether binary should be
 *                               persistent.
 * @return Error code:
 *         - QAIRT_SUCCESS: Persistent binary flag was successfully set.
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: _configHandle_ is not a
 *           valid handle.
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: _isPersistentBinary_ is
 *           invalid.
 *
 * @note Use corresponding API through QairtContext_ConfigV1_t.
 */
Qairt_Status_t QairtContext_Config_setIsPersistentBinary(
    QairtContext_ConfigHandle_t configHandle, uint8_t isPersistentBinary);

/**
 * @brief Get the persistent binary flag from a context configuration.
 *
 * @param[in] configHandle A handle to a created context configuration.
 *
 * @param[out] isPersistentBinary Pointer to retrieve the persistent binary
 *                                flag.
 * @return Error code:
 *         - QAIRT_SUCCESS: Persistent binary flag was successfully
 *           retrieved.
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: _configHandle_ is not a
 *           valid handle.
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: _isPersistentBinary_ is
 *           NULL.
 *
 * @note Use corresponding API through QairtContext_ConfigV1_t.
 */
Qairt_Status_t QairtContext_Config_getIsPersistentBinary(
    QairtContext_ConfigHandle_t configHandle, uint8_t* isPersistentBinary);

/**
 * @brief Set the binary compatibility type for a context configuration.
 *
 * @param[in] configHandle A handle to a created context configuration.
 *
 * @param[in] compatibilityType The binary compatibility type to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Binary compatibility type was successfully
 *           set.
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: _configHandle_ is not a
 *           valid handle.
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: _compatibilityType_ is
 *           invalid.
 *
 * @note Use corresponding API through QairtContext_ConfigV1_t.
 */
Qairt_Status_t QairtContext_Config_setBinaryCompatibilityType(
    QairtContext_ConfigHandle_t configHandle,
    QairtContext_BinaryCompatibilityType_t compatibilityType);

/**
 * @brief Get the binary compatibility type from a context configuration.
 *
 * @param[in] configHandle A handle to a created context configuration.
 *
 * @param[out] compatibilityType Pointer to retrieve the binary compatibility
 *                               type.
 * @return Error code:
 *         - QAIRT_SUCCESS: Binary compatibility type was successfully
 *           retrieved.
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: _configHandle_ is not a
 *           valid handle.
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: _compatibilityType_ is
 *           NULL.
 *
 * @note Use corresponding API through QairtContext_ConfigV1_t.
 */
Qairt_Status_t QairtContext_Config_getBinaryCompatibilityType(
    QairtContext_ConfigHandle_t configHandle,
    QairtContext_BinaryCompatibilityType_t* compatibilityType);

/**
 * @brief Adds an enabled graph to the Context Config.
 *
 * @param[in] configHandle A handle to a created context configuration.
 *
 * @param[in] graphName A null-terminated graph name.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: _configHandle_ is not a
 *           valid handle
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: _graphName_ is
 *           NULL or invalid
 *
 * @note Use corresponding API through QairtContext_ConfigHandle_t.
 */
Qairt_Status_t QairtContext_Config_setEnableGraph(QairtContext_ConfigHandle_t configHandle,
                                                       const char* graphName);

/**
 * @brief Gets the number of enabled graphs associated with a Context Config
 *        handle.
 *
 * @param[in] configHandle A handle to a created context configuration.
 *
 * @param[out] numEnabledGraphs Pointer to retrieve the number of enabled
 *                              graphs associated with the Context Config
 *                              handle.
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: _configHandle_ is not a
 *           valid handle
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: _numEnabledGraphs_ is
 *           NULL
 *
 * @note Use corresponding API through QairtContext_ConfigHandle_t.
 */
Qairt_Status_t QairtContext_Config_getNumEnabledGraphs(
    QairtContext_ConfigHandle_t configHandle, uint32_t* numEnabledGraphs);

/**
 * @brief Gets enabled graph name at a particular index associated with the
 *        Context Config Handle.
 *
 * @param[in] configHandle A handle to a created context configuration.
 *
 * @param[in] index Index of the enabled graph. Value must be less than
 *                  getNumEnabledGraphs.
 *
 * @param[out] graphName The null-terminated graph name.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: _configHandle_ is not a
 *           valid handle
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: _graphName_ is
 *           NULL or _index_ is invalid
 *
 * @note Use corresponding API through QairtContext_ConfigHandle_t.
 */
Qairt_Status_t QairtContext_Config_getEnabledGraphAt(
    QairtContext_ConfigHandle_t configHandle, uint32_t index, const char** graphName);

//=============================================================================
/// AsyncExecutionDepth
//=============================================================================
typedef Qairt_Status_t (*QairtContext_AsyncExecutionSetDepthFn_t)(
    QairtContext_AsyncExecutionDepthHandle_t, uint32_t);

typedef Qairt_Status_t (*QairtContext_AsyncExecutionGetDepthFn_t)(
    QairtContext_AsyncExecutionDepthHandle_t, uint32_t*);

typedef Qairt_Status_t (*QairtContext_AsyncExecutionCreateFn_t)(
    QairtContext_AsyncExecutionDepthHandle_t*);

typedef Qairt_Status_t (*QairtContext_AsyncExecutionFreeFn_t)(
    QairtContext_AsyncExecutionDepthHandle_t);

/// ContextConfig
typedef Qairt_Status_t (*QairtContext_Config_CreateFn_t)(QairtContext_ConfigHandle_t*);

typedef Qairt_Status_t (*QairtContext_Config_FreeFn_t)(QairtContext_ConfigHandle_t);

typedef Qairt_Status_t (*QairtContext_Config_SetCustomConfigsFn_t)(
    QairtContext_ConfigHandle_t, QairtContext_CustomConfigHandle_t*, uint32_t numCustomConfigs);

typedef Qairt_Status_t (*QairtContext_Config_GetNumCustomConfigsFn_t)(
    QairtContext_ConfigHandle_t, uint32_t* numConfigs);

typedef Qairt_Status_t (*QairtContext_Config_GetCustomConfigAtFn_t)(
    QairtContext_ConfigHandle_t, uint32_t index, QairtContext_CustomConfigHandle_t*);

typedef Qairt_Status_t (*QairtContext_Config_SetPriorityFn_t)(QairtContext_ConfigHandle_t,
                                                                   Qairt_Priority_t);

typedef Qairt_Status_t (*QairtContext_Config_GetPriorityFn_t)(QairtContext_ConfigHandle_t,
                                                                   Qairt_Priority_t*);

typedef Qairt_Status_t (*QairtContext_Config_SetOemKeyFn_t)(QairtContext_ConfigHandle_t,
                                                                 const char* oemKey);

typedef Qairt_Status_t (*QairtContext_Config_GetOemKeyFn_t)(QairtContext_ConfigHandle_t,
                                                                 const char** oemKey);

typedef Qairt_Status_t (*QairtContext_Config_SetAsyncQueueDepthFn_t)(
    QairtContext_ConfigHandle_t, QairtContext_AsyncExecutionDepthHandle_t);

typedef Qairt_Status_t (*QairtContext_Config_GetAsyncQueueDepthFn_t)(
    QairtContext_ConfigHandle_t, QairtContext_AsyncExecutionDepthHandle_t*);

typedef Qairt_Status_t (*QairtContext_Config_SetEnableGraphFn_t)(QairtContext_ConfigHandle_t,
                                                                      const char* graphName);

typedef Qairt_Status_t (*QairtContext_Config_GetNumEnabledGraphsFn_t)(
    QairtContext_ConfigHandle_t, uint32_t*);

typedef Qairt_Status_t (*QairtContext_Config_GetEnabledGraphAtFn_t)(
    QairtContext_ConfigHandle_t, uint32_t, const char**);

typedef Qairt_Status_t (*QairtContext_Config_SetMemoryLimitHintFn_t)(
    QairtContext_ConfigHandle_t, uint64_t);

typedef Qairt_Status_t (*QairtContext_Config_GetMemoryLimitHintFn_t)(
    QairtContext_ConfigHandle_t, uint64_t*);

typedef Qairt_Status_t (*QairtContext_Config_SetIsPersistentBinaryFn_t)(
    QairtContext_ConfigHandle_t, uint8_t);

typedef Qairt_Status_t (*QairtContext_Config_GetIsPersistentBinaryFn_t)(
    QairtContext_ConfigHandle_t, uint8_t*);

typedef Qairt_Status_t (*QairtContext_Config_SetBinaryCompatibilityFn_t)(
    QairtContext_ConfigHandle_t, QairtContext_BinaryCompatibilityType_t);

typedef Qairt_Status_t (*QairtContext_Config_GetBinaryCompatibilityFn_t)(
    QairtContext_ConfigHandle_t, QairtContext_BinaryCompatibilityType_t*);

//=============================================================================
// Public Interfaces
//=============================================================================

/**
 * @struct QairtContext_AsyncExecutionDepthV1_t
 * @brief This struct contains the functions that define the V1 interface for
 *        QAIRT Context Async Execution Depth.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtContext_AsyncExecutionCreateFn_t create;
  QairtContext_AsyncExecutionFreeFn_t free;
  QairtContext_AsyncExecutionSetDepthFn_t setAsyncExecutionDepth;
  QairtContext_AsyncExecutionGetDepthFn_t getAsyncExecutionDepth;
} QairtContext_AsyncExecutionDepthV1_t;

/**
 * @struct QairtContext_ConfigV1_t
 * @brief This struct contains the functions that define the V1 interface for
 *        QAIRT Context Config.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtContext_Config_CreateFn_t create;
  QairtContext_Config_FreeFn_t free;
  QairtContext_Config_SetCustomConfigsFn_t setCustomConfigs;
  QairtContext_Config_GetNumCustomConfigsFn_t getNumCustomConfigs;
  QairtContext_Config_GetCustomConfigAtFn_t getCustomConfigAt;
  QairtContext_Config_SetPriorityFn_t setPriority;
  QairtContext_Config_GetPriorityFn_t getPriority;
  QairtContext_Config_SetOemKeyFn_t setOemKey;
  QairtContext_Config_GetOemKeyFn_t getOemKey;
  QairtContext_Config_SetAsyncQueueDepthFn_t setAsyncQueueDepth;
  QairtContext_Config_GetAsyncQueueDepthFn_t getAsyncQueueDepth;
  QairtContext_Config_SetEnableGraphFn_t setEnableGraph;
  QairtContext_Config_GetNumEnabledGraphsFn_t getNumEnabledGraphs;
  QairtContext_Config_GetEnabledGraphAtFn_t getEnabledGraphAt;
  QairtContext_Config_SetMemoryLimitHintFn_t setMemoryLimitHint;
  QairtContext_Config_GetMemoryLimitHintFn_t getMemoryLimitHint;
  QairtContext_Config_SetIsPersistentBinaryFn_t setIsPersistentBinary;
  QairtContext_Config_GetIsPersistentBinaryFn_t getIsPersistentBinary;
  QairtContext_Config_SetBinaryCompatibilityFn_t setBinaryCompatibilityType;
  QairtContext_Config_GetBinaryCompatibilityFn_t getBinaryCompatibilityType;
} QairtContext_ConfigV1_t;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // QAISW_QAIRTCONTEXTCONFIG_H

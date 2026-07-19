//=============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//=============================================================================

#ifndef QAIRT_GRAPHCONFIG_H
#define QAIRT_GRAPHCONFIG_H

#include "QairtCommon/QairtCommon.h"
#include "QairtOpConfig/QairtOpConfig.h"
#include "QairtProfile/QairtProfile.h"
#include "QairtTensor/QairtTensor.h"

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
// Datatypes
//=============================================================================
/**
 * @brief This enum defines graph profiling states.
 */
typedef enum {
  /// Profiling is enabled for the graph
  QAIRT_GRAPH_PROFILING_STATE_ENABLED = 1,
  /// Profiling is disabled for the graph
  QAIRT_GRAPH_PROFILING_STATE_DISABLED = 2,
  // Unused, present to ensure 32 bits.
  QAIRT_GRAPH_PROFILING_STATE_UNDEFINED = 0x7FFFFFFF
} QairtGraph_ProfilingState_t;

/**
 * @brief   Graph Config handle used to interface with Graph Config APIs
 *
 *          The handle itself is created and freed by the client.
 */
typedef struct _QairtGraph_ConfigHandle_t* QairtGraph_ConfigHandle_t;

/**
 * @brief   Graph Custom Config handle used to interface with Graph Custom
 *          Config APIs, which are specific to each backend.
 *
 *          The handle itself is created and freed by the client.
 */
typedef struct _QairtGraph_CustomConfigHandle_t* QairtGraph_CustomConfigHandle_t;

//=============================================================================
// Public Functions
//=============================================================================
/**
 * @brief Creates a graph config handle.
 *
 * @param[out] cfgHandle A handle to the created graph config.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_GRAPH_ERROR_INVALID_HANDLE: _cfgHandle_ is not an
 *           assignable handle
 *
 * @note Use corresponding API through QairtGraph_ConfigV1_t.
 */
Qairt_Status_t QairtGraph_Config_create(QairtGraph_ConfigHandle_t* cfgHandle);

/**
 * @brief Frees a graph config handle.
 *
 * @param[in] configHandle A handle to the created graph config.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_GRAPH_ERROR_INVALID_HANDLE: _configHandle_ is not a
 *           valid handle
 *
 * @note Use corresponding API through QairtGraph_ConfigV1_t.
 */
Qairt_Status_t QairtGraph_Config_free(QairtGraph_ConfigHandle_t configHandle);

/**
 * @brief Gets the number of custom configs stored
 *
 * @param[in] cfgHandle A handle to the created graph config.
 *
 * @param[out] numConfigs number of custom configs.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_GRAPH_ERROR_INVALID_HANDLE: _cfgHandle_ is not a valid
 *           handle
 *         - QAIRT_GRAPH_ERROR_INVALID_ARGUMENT: at least one argument is
 *           invalid
 *
 * @note Use corresponding API through QairtGraph_ConfigV1_t.
 */
Qairt_Status_t QairtGraph_Config_getNumCustomConfigs(QairtGraph_ConfigHandle_t cfgHandle,
                                                          uint32_t* numConfigs);

/**
 * @brief Gets a custom config at a particular index. Index must be less than
 *        the result of QairtGraph_Config_getNumCustomConfigs.
 *
 * @param[in] cfgHandle A handle to the created config.
 *
 * @param[in] index Index of the custom config
 *
 * @param[out] customConfigHandle A referenced graph custom config handle.
 *                                Reference handles should not be freed.
 *                                Referenced handles are invalidated if modification operations are
 * performed from the same component API.
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_GRAPH_ERROR_INVALID_HANDLE: _cfgHandle_ is not a valid
 *           handle
 *         - QAIRT_GRAPH_ERROR_INVALID_ARGUMENT: at least one argument is
 *           invalid
 *
 * @note Use corresponding API through QairtGraph_ConfigV1_t.
 */
Qairt_Status_t QairtGraph_Config_getCustomConfigAt(
    QairtGraph_ConfigHandle_t cfgHandle,
    uint32_t index,
    QairtGraph_CustomConfigHandle_t* customConfigHandle);

/**
 * @brief Gets the priority handle associated with a graph config handle.
 *
 * @param[in] cfgHandle A handle to the created graph config.
 *
 * @param[out] priorityHandle A referenced priority handle. Reference handles
 *                            should not be freed. Referenced handles are
 *                            invalidated if modification operations are performed from the same
 * component API.
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_GRAPH_ERROR_INVALID_HANDLE: _cfgHandle_ or
 *           _priorityHandle_ is not a valid handle
 *         - QAIRT_GRAPH_ERROR_INVALID_ARGUMENT: at least one argument is
 *           invalid
 *
 * @note Use corresponding API through QairtGraph_ConfigV1_t.
 */
Qairt_Status_t QairtGraph_Config_getPriority(QairtGraph_ConfigHandle_t cfgHandle,
                                                  Qairt_Priority_t* priority);

/**
 * @brief Gets the priority handle associated with a graph config handle.
 *
 * @param[in] cfgHandle A handle to the created graph config.
 *
 * @param[out] profileHandle A referenced profile handle. Reference handles
 *                           should not be freed. Referenced handles are
 *                           invalidated if modification operations are performed from the same
 * component API.
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_GRAPH_ERROR_INVALID_HANDLE: _cfgHandle_ or
 *           _profileHandle_ is not a valid handle
 *         - QAIRT_GRAPH_ERROR_INVALID_ARGUMENT: at least one argument is
 *           invalid
 *
 * @note Use corresponding API through QairtGraph_ConfigV1_t.
 */
Qairt_Status_t QairtGraph_Config_getProfileHandle(QairtGraph_ConfigHandle_t cfgHandle,
                                                       QairtProfile_Handle_t* profileHandle);

/**
 * @brief Gets the profiling state associated with a graph config handle.
 *
 * @param[in] cfgHandle A handle to the created graph config.
 *
 * @param[out] profilingState The config's profiling state.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_GRAPH_ERROR_INVALID_HANDLE: _cfgHandle_ is not a valid
 *           handle
 *         - QAIRT_GRAPH_ERROR_INVALID_ARGUMENT: at least one argument is
 *           invalid
 *
 * @note Use corresponding API through QairtGraph_ConfigV1_t.
 */
Qairt_Status_t QairtGraph_Config_getProfilingState(
    QairtGraph_ConfigHandle_t cfgHandle, QairtGraph_ProfilingState_t* profilingState);

/**
 * @brief Gets the number of profiling executions associated with a graph config
 *        handle.
 *
 * @param[in] cfgHandle A handle to the created graph config.
 *
 * @param[out] numProfilingExecutions The number of profiling executions.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_GRAPH_ERROR_INVALID_HANDLE: _cfgHandle_ is not a valid
 *           handle
 *         - QAIRT_GRAPH_ERROR_INVALID_ARGUMENT: at least one argument is
 *           invalid
 *
 * @note Use corresponding API through QairtGraph_ConfigV1_t.
 */
Qairt_Status_t QairtGraph_Config_getNumProfilingExecutions(
    QairtGraph_ConfigHandle_t cfgHandle, uint32_t* numProfilingExecutions);

/**
 * @brief Sets an array of custom graph config handles onto a graph config handle.
 *
 * @param[in] cfgHandle A handle to the created graph config.
 *
 * @param[in] customCfgHandles An array of custom config handles.
 *
 * @param[in] numCustomConfigs The number of handles in the array.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_GRAPH_ERROR_INVALID_HANDLE: _cfgHandle_ or one or more
 *           handles in _customCfgHandles_ are not valid handles
 *         - QAIRT_GRAPH_ERROR_INVALID_ARGUMENT: _customCfgHandles_ is NULL
 *           when _numCustomConfigs_ is greater than zero
 *
 * @note Use corresponding API through QairtGraph_ConfigV1_t.
 */
Qairt_Status_t QairtGraph_Config_setCustomConfigs(
    QairtGraph_ConfigHandle_t cfgHandle,
    QairtGraph_CustomConfigHandle_t* customCfgHandles,
    uint32_t numCustomConfigs);

/**
 * @brief Sets a priority handle onto a graph config handle.
 *
 * @param[in] cfgHandle A handle to the created graph config.
 *
 * @param[in] priorityHandle A priority handle, created and configured via the
 *                           Priority component's APIs.
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_GRAPH_ERROR_INVALID_HANDLE: _cfgHandle_ or
 *           _priorityHandle_ is not a valid handle
 *         - QAIRT_GRAPH_ERROR_INVALID_ARGUMENT: at least one argument is
 *           invalid
 *
 * @note Use corresponding API through QairtGraph_ConfigV1_t.
 */
Qairt_Status_t QairtGraph_Config_setPriority(QairtGraph_ConfigHandle_t cfgHandle,
                                                  Qairt_Priority_t priority);

/**
 * @brief Sets a profile handle onto a graph config handle.
 *
 * @param[in] cfgHandle A handle to the created graph config.
 *
 * @param[in] profileHandle A profile handle, created and configured via the
 *                          Profiling component's APIs.
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_GRAPH_ERROR_INVALID_HANDLE: _cfgHandle_ or
 *           _profileHandle_ is not a valid handle
 *         - QAIRT_GRAPH_ERROR_INVALID_ARGUMENT: at least one argument is
 *           invalid
 *
 * @note Use corresponding API through QairtGraph_ConfigV1_t.
 */
Qairt_Status_t QairtGraph_Config_setProfileHandle(QairtGraph_ConfigHandle_t cfgHandle,
                                                       QairtProfile_Handle_t profileHandle);

/**
 * @brief Sets the profiling state on a graph config handle.
 *
 * @param[in] cfgHandle A handle to the created graph config.
 *
 * @param[in] profilingState The profiling state to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_GRAPH_ERROR_INVALID_HANDLE: _cfgHandle_ is not a valid
 *           handle
 *         - QAIRT_GRAPH_ERROR_INVALID_ARGUMENT: at least one argument is
 *           invalid
 *
 * @note Use corresponding API through QairtGraph_ConfigV1_t.
 */
Qairt_Status_t QairtGraph_Config_setProfilingState(
    QairtGraph_ConfigHandle_t cfgHandle, const QairtGraph_ProfilingState_t profilingState);

/**
 * @brief Sets the number of profiling executions on a graph config handle.
 *
 * @param[in] cfgHandle A handle to the created graph config.
 *
 * @param[in] numProfilingExecutions The number of profiling executions to
 *                                   perform.
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_GRAPH_ERROR_INVALID_HANDLE: _cfgHandle_ is not a valid
 *           handle
 *         - QAIRT_GRAPH_ERROR_INVALID_ARGUMENT: at least one argument is
 *           invalid
 *
 * @note Use corresponding API through QairtGraph_ConfigV1_t.
 */
Qairt_Status_t QairtGraph_Config_setNumProfilingExecutions(
    QairtGraph_ConfigHandle_t cfgHandle, const uint32_t numProfilingExecutions);

typedef Qairt_Status_t (*QairtGraph_Config_CreateFn_t)(QairtGraph_ConfigHandle_t*);
typedef Qairt_Status_t (*QairtGraph_Config_FreeFn_t)(QairtGraph_ConfigHandle_t);

typedef Qairt_Status_t (*QairtGraph_Config_GetNumCustomConfigsFn_t)(
    QairtGraph_ConfigHandle_t, uint32_t*);

typedef Qairt_Status_t (*QairtGraph_Config_GetCustomConfigAtFn_t)(
    QairtGraph_ConfigHandle_t, uint32_t, QairtGraph_CustomConfigHandle_t*);

typedef Qairt_Status_t (*QairtGraph_Config_GetPriorityFn_t)(QairtGraph_ConfigHandle_t,
                                                                 Qairt_Priority_t*);

typedef Qairt_Status_t (*QairtGraph_Config_GetProfileHandleFn_t)(QairtGraph_ConfigHandle_t,
                                                                      QairtProfile_Handle_t*);

typedef Qairt_Status_t (*QairtGraph_Config_GetProfilingStateFn_t)(
    QairtGraph_ConfigHandle_t, QairtGraph_ProfilingState_t*);

typedef Qairt_Status_t (*QairtGraph_Config_GetNumProfilingExecutionsFn_t)(
    QairtGraph_ConfigHandle_t, uint32_t*);

typedef Qairt_Status_t (*QairtGraph_Config_SetCustomConfigsFn_t)(
    QairtGraph_ConfigHandle_t, QairtGraph_CustomConfigHandle_t*, uint32_t);

typedef Qairt_Status_t (*QairtGraph_Config_SetPriorityFn_t)(QairtGraph_ConfigHandle_t,
                                                                 Qairt_Priority_t);

typedef Qairt_Status_t (*QairtGraph_Config_SetProfileHandleFn_t)(QairtGraph_ConfigHandle_t,
                                                                      QairtProfile_Handle_t);

typedef Qairt_Status_t (*QairtGraph_Config_SetProfilingStateFn_t)(
    QairtGraph_ConfigHandle_t, const QairtGraph_ProfilingState_t);

typedef Qairt_Status_t (*QairtGraph_Config_SetNumProfilingExecutionsFn_t)(
    QairtGraph_ConfigHandle_t, const uint32_t);
//=============================================================================
// Public Interfaces
//=============================================================================
/**
 * @struct QairtGraph_ConfigV1_t
 * @brief This struct contains the functions that define the V1 interface for
 *        QAIRT Graph Config. This interface can be retrieved through the
 *        id QAIRT_GRAPH_CONFIG_V1_ID.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtGraph_Config_CreateFn_t create;
  QairtGraph_Config_FreeFn_t free;
  QairtGraph_Config_GetNumCustomConfigsFn_t getNumCustomConfigs;
  QairtGraph_Config_GetCustomConfigAtFn_t getCustomConfigAt;
  QairtGraph_Config_GetPriorityFn_t getPriority;
  QairtGraph_Config_GetProfileHandleFn_t getProfileHandle;
  QairtGraph_Config_GetProfilingStateFn_t getProfilingState;
  QairtGraph_Config_GetNumProfilingExecutionsFn_t getNumProfilingExecutions;
  QairtGraph_Config_SetCustomConfigsFn_t setCustomConfigs;
  QairtGraph_Config_SetPriorityFn_t setPriority;
  QairtGraph_Config_SetProfileHandleFn_t setProfileHandle;
  QairtGraph_Config_SetProfilingStateFn_t setProfilingState;
  QairtGraph_Config_SetNumProfilingExecutionsFn_t setNumProfilingExecutions;
} QairtGraph_ConfigV1_t;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // QAIRT_GRAPHCONFIG_H

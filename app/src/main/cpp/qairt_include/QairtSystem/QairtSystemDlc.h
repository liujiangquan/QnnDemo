//==============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//==============================================================================

/**
 * @file
 * @brief   System DLC component API
 *
 *          A header which provides Deep Learning Container (DLC) services
 *          for loading and composing graphs from serialized model files.
 */

#ifndef QAISW_QAIRTSYSTEMDLC_H
#define QAISW_QAIRTSYSTEMDLC_H

#include "QairtBackend/QairtBackend.h"
#include "QairtCommon/QairtCommon.h"
#include "QairtContext/QairtContext.h"
#include "QairtGraph/QairtGraph.h"
#include "QairtSystem/QairtSystemContext.h"
#include "QairtSystem/QairtSystemLog.h"

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
// Macros
//=============================================================================

#define QAIRT_SYSTEM_DLC_V1_ID QAIRT_MIN_ID_SYSTEM_DLC

#define QAIRT_SYSTEM_DLC_GRAPH_CONFIG_INFO_V1_ID QAIRT_MIN_ID_SYSTEM_DLC + 1

//=============================================================================
// Data Types
//=============================================================================

/**
 * @brief QAIRT System DLC API result / error codes.
 */
typedef enum {
  QAIRT_SYSTEM_DLC_MIN_ERROR = QAIRT_MIN_ERROR_SYSTEM_DLC,
  ////////////////////////////////////////

  /// System DLC success
  QAIRT_SYSTEM_DLC_NO_ERROR = QAIRT_SUCCESS,
  /// There is optional API component that is not supported yet.
  QAIRT_SYSTEM_DLC_ERROR_UNSUPPORTED_FEATURE = QAIRT_COMMON_ERROR_NOT_SUPPORTED,
  /// General error relating to memory allocation in processing System DLC API
  QAIRT_SYSTEM_DLC_ERROR_MEM_ALLOC = QAIRT_COMMON_ERROR_MEM_ALLOC,
  /// General type of System DLC error, which has not been identified as any
  /// other error type.
  QAIRT_SYSTEM_DLC_ERROR_GENERAL = QAIRT_COMMON_ERROR_GENERAL,
  /// Invalid System DLC handle
  QAIRT_SYSTEM_DLC_ERROR_INVALID_HANDLE = 35000,
  /// One or more arguments to a System DLC API is/are NULL/invalid.
  QAIRT_SYSTEM_DLC_ERROR_INVALID_ARGUMENT = 35001,
  /// Generic failure in achieving the objective of a System DLC API
  QAIRT_SYSTEM_DLC_ERROR_OPERATION_FAILED = 35002,
  /// Malformed DLC binary
  QAIRT_SYSTEM_DLC_ERROR_MALFORMED_BINARY = 35003,

  ////////////////////////////////////////
  QAIRT_SYSTEM_DLC_MAX_ERROR = QAIRT_MAX_ERROR_SYSTEM_DLC,
  // Unused, present to ensure 32 bits.
  QAIRT_SYSTEM_DLC_ERROR_UNDEFINED = 0x7FFFFFFF
} QairtSystemDlc_Error_t;

/**
 * @brief A handle to a System DLC instance.
 */
typedef struct _QairtSystemDlc_Handle_t* QairtSystemDlc_Handle_t;

/**
 * @brief A handle to graph configuration information.
 */
typedef struct _QairtSystemDlc_GraphConfigInfoHandle_t* QairtSystemDlc_GraphConfigInfoHandle_t;

/**
 * @brief Typedef for a pointer to the QAIRT API interface.
 */
typedef const void* QairtApi_Interface_t;

//=============================================================================
// Public Functions
//=============================================================================

/**
 * @brief Create a graph configuration information instance.
 *
 * @param[out] graphConfigInfoHandle Handle to the created graph config info.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Successfully created graph config info instance.
 *         - QAIRT_SYSTEM_DLC_ERROR_INVALID_ARGUMENT: _graphConfigInfoHandle_
 *           is NULL.
 *         - QAIRT_SYSTEM_DLC_ERROR_OPERATION_FAILED: Failed to create graph
 *           config info instance.
 *
 * @note Use corresponding API through QairtSystemDlc_GraphConfigInfoV1_t.
 */
Qairt_Status_t QairtSystemDlc_GraphConfigInfo_create(
    QairtSystemDlc_GraphConfigInfoHandle_t* graphConfigInfoHandle);

/**
 * @brief Free a graph configuration information instance.
 *
 * @param[in] graphConfigInfoHandle Handle to the graph config info.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Successfully freed graph config info instance.
 *         - QAIRT_SYSTEM_DLC_ERROR_INVALID_HANDLE: Invalid graph config info
 *           handle.
 *
 * @note Use corresponding API through QairtSystemDlc_GraphConfigInfoV1_t.
 */
Qairt_Status_t QairtSystemDlc_GraphConfigInfo_free(
    QairtSystemDlc_GraphConfigInfoHandle_t graphConfigInfoHandle);

/**
 * @brief Set the graph name for the configuration.
 *
 * @param[in] graphConfigInfoHandle Handle to the graph config info.
 * @param[in] graphName Name of the graph.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Successfully set graph name.
 *         - QAIRT_SYSTEM_DLC_ERROR_INVALID_ARGUMENT: _graphName_ is NULL.
 *         - QAIRT_SYSTEM_DLC_ERROR_INVALID_HANDLE: Invalid graph config info
 *           handle.
 *         - QAIRT_SYSTEM_DLC_ERROR_OPERATION_FAILED: Failed to set graph name.
 *
 * @note Use corresponding API through QairtSystemDlc_GraphConfigInfoV1_t.
 */
Qairt_Status_t QairtSystemDlc_GraphConfigInfo_setGraphName(
    QairtSystemDlc_GraphConfigInfoHandle_t graphConfigInfoHandle, const char* graphName);

/**
 * @brief Get the graph name from the configuration.
 *
 * @param[in] graphConfigInfoHandle Handle to the graph config info.
 * @param[out] graphName Name of the graph.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Successfully retrieved graph name.
 *         - QAIRT_SYSTEM_DLC_ERROR_INVALID_ARGUMENT: _graphName_ is NULL.
 *         - QAIRT_SYSTEM_DLC_ERROR_INVALID_HANDLE: Invalid graph config info
 *           handle.
 *         - QAIRT_SYSTEM_DLC_ERROR_OPERATION_FAILED: Failed to retrieve graph
 *           name.
 *
 * @note Use corresponding API through QairtSystemDlc_GraphConfigInfoV1_t.
 */
Qairt_Status_t QairtSystemDlc_GraphConfigInfo_getGraphName(
    QairtSystemDlc_GraphConfigInfoHandle_t graphConfigInfoHandle, const char** graphName);

/**
 * @brief Set the graph configuration.
 *
 * @param[in] graphConfigInfoHandle Handle to the graph config info.
 * @param[in] graphConfig A graph configuration handle representing the
 *                        configuration for this graph.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Successfully set graph configuration.
 *         - QAIRT_SYSTEM_DLC_ERROR_INVALID_HANDLE: Invalid graph config info
 *           handle.
 *         - QAIRT_SYSTEM_DLC_ERROR_INVALID_ARGUMENT: _graphConfig_ is
 *           NULL/invalid.
 *
 * @note Use corresponding API through QairtSystemDlc_GraphConfigInfoV1_t.
 */
Qairt_Status_t QairtSystemDlc_GraphConfigInfo_setGraphConfig(
    QairtSystemDlc_GraphConfigInfoHandle_t graphConfigInfoHandle,
    QairtGraph_ConfigHandle_t graphConfig);

/**
 * @brief Create a System DLC instance from a file.
 *
 * @param[in] systemLogHandle Handle to system log for logging.
 *
 * @param[in] dlcPath Path to the DLC file.
 *
 * @param[out] dlcHandle A handle to the created System DLC instance.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Successfully created a System DLC instance.
 *         - QAIRT_SYSTEM_DLC_ERROR_INVALID_ARGUMENT: _systemLogHandle_,
 *           _dlcPath_, or _dlcHandle_ is NULL.
 *         - QAIRT_SYSTEM_DLC_ERROR_MEM_ALLOC: Error encountered in
 *           allocating memory for System DLC instance.
 *         - QAIRT_SYSTEM_DLC_ERROR_UNSUPPORTED_FEATURE: System DLC
 *           features not supported.
 *         - QAIRT_SYSTEM_DLC_ERROR_MALFORMED_BINARY: DLC file is
 *           malformed or corrupted.
 *         - QAIRT_SYSTEM_DLC_ERROR_OPERATION_FAILED: Failed to create
 *           System DLC instance.
 *
 * @note Use corresponding API through QairtSystemDlc_V1_t.
 */
Qairt_Status_t QairtSystemDlc_createFromFile(QairtSystemLog_Handle_t systemLogHandle,
                                                  const char* dlcPath,
                                                  QairtSystemDlc_Handle_t* dlcHandle);

/**
 * @brief Create a System DLC instance from a binary buffer.
 *
 * @param[in] systemLogHandle Handle to system log for logging.
 *
 * @param[in] buffer Pointer to buffer representing the DLC.
 *
 * @param[in] bufferSize Size of the binary buffer in bytes.
 *
 * @param[out] dlcHandle A handle to the created System DLC instance.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Successfully created a System DLC instance.
 *         - QAIRT_SYSTEM_DLC_ERROR_INVALID_ARGUMENT: _systemLogHandle_,
 *           _buffer_, or _dlcHandle_ is NULL, or _bufferSize_ is 0.
 *         - QAIRT_SYSTEM_DLC_ERROR_MEM_ALLOC: Error encountered in
 *           allocating memory for System DLC instance.
 *         - QAIRT_SYSTEM_DLC_ERROR_UNSUPPORTED_FEATURE: System DLC
 *           features not supported.
 *         - QAIRT_SYSTEM_DLC_ERROR_MALFORMED_BINARY: DLC buffer is
 *           malformed, corrupted, or an error occurred during creation.
 *
 * @note Use corresponding API through QairtSystemDlc_V1_t.
 */
Qairt_Status_t QairtSystemDlc_createFromBinary(QairtSystemLog_Handle_t systemLogHandle,
                                                    const uint8_t* buffer,
                                                    const uint64_t bufferSize,
                                                    QairtSystemDlc_Handle_t* dlcHandle);

/**
 * @brief Compose graphs from a DLC on a particular backend and context.
 *        Memory allocated in _graphHandles_ is owned by clients and may be
 *        released with calls to free().
 *
 * @param[in] dlcHandle The DLC handle to retrieve graphs from.
 *
 * @param[in] graphConfigHandles Array of graph configuration information handles
 *                               for specific graphs. NULL is allowed and indicates
 *                               default configuration for all graphs.
 * @param[in] numGraphConfigs Number of graph configurations in the array.
 *                            Must be 0 if _graphConfigHandles_ is NULL.
 * @param[in] backendHandle The backend handle on which to compose the graphs.
 *
 * @param[in] contextHandle The context handle on which to compose the graphs.
 *
 * @param[in] qairtInterface Pointer to the QAIRT API interface retrieved via
 *                           QairtInterface_getInterface for the backend.
 *
 * @param[out] graphInfoSetHandle A handle to a set of graph information
 *                                representing what was created with the
 *                                backend. Memory is owned by the client.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Successfully composed graphs.
 *         - QAIRT_SYSTEM_DLC_ERROR_INVALID_ARGUMENT: _dlcHandle_,
 *           _backendHandle_, _contextHandle_, _backendInterface_,
 *           or _graphInfoSetHandle_ is NULL.
 *         - QAIRT_SYSTEM_DLC_ERROR_INVALID_HANDLE: Invalid DLC, backend,
 *           or context handle.
 *         - QAIRT_SYSTEM_DLC_ERROR_MEM_ALLOC: Error encountered in
 *           allocating memory.
 *         - QAIRT_SYSTEM_DLC_ERROR_UNSUPPORTED_FEATURE: DLC features not
 *           supported.
 *         - QAIRT_SYSTEM_DLC_ERROR_OPERATION_FAILED: Failed to compose
 *           graphs.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence
 *           (successful recovery).
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence
 *           (unsuccessful recovery).
 *
 * @note Use corresponding API through QairtSystemDlc_V1_t.
 */
Qairt_Status_t QairtSystemDlc_composeGraphs(
    QairtSystemDlc_Handle_t dlcHandle,
    const QairtSystemDlc_GraphConfigInfoHandle_t* graphConfigHandles,
    const uint32_t numGraphConfigs,
    QairtBackend_Handle_t backendHandle,
    QairtContext_Handle_t contextHandle,
    QairtApi_Interface_t qairtInterface,
    QairtSystem_Context_GraphInfoSetHandle_t* graphInfoSetHandle);

/**
 * @brief Retrieve operation mapping information from a DLC.
 *
 * @param[in] dlcHandle Handle to the DLC.
 *
 * @param[out] opMappings A list of operation mappings. The memory allocated
 *                        here is owned by the System DLC library and is
 *                        released when the corresponding DLC handle is freed.
 * @param[out] numOpMappings The number of operation mappings.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Successfully retrieved operation mappings.
 *         - QAIRT_SYSTEM_DLC_ERROR_INVALID_HANDLE: Invalid DLC handle.
 *         - QAIRT_SYSTEM_DLC_ERROR_INVALID_ARGUMENT: _opMappings_ or
 *           _numOpMappings_ is NULL.
 *         - QAIRT_SYSTEM_DLC_ERROR_UNSUPPORTED_FEATURE: Operation
 *           mappings not supported.
 *         - QAIRT_SYSTEM_DLC_ERROR_OPERATION_FAILED: Failed to retrieve
 *           operation mappings.
 *
 * @note Use corresponding API through QairtSystemDlc_V1_t.
 */
Qairt_Status_t QairtSystemDlc_getOpMappings(QairtSystemDlc_Handle_t dlcHandle,
                                                 const void** opMappings,
                                                 uint32_t* numOpMappings);

/**
 * @brief Free the System DLC instance.
 *        This API clears any intermediate memory allocated and associated
 *        with a valid handle.
 *
 * @param[in] dlcHandle Handle to the System DLC instance.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Successfully freed System DLC instance.
 *         - QAIRT_SYSTEM_DLC_ERROR_INVALID_HANDLE: Invalid System DLC
 *           handle.
 *         - QAIRT_SYSTEM_DLC_ERROR_UNSUPPORTED_FEATURE: Free operation
 *           not supported.
 *         - QAIRT_SYSTEM_DLC_ERROR_OPERATION_FAILED: Failed to free
 *           System DLC instance.
 *
 * @note Use corresponding API through QairtSystemDlc_V1_t.
 */
Qairt_Status_t QairtSystemDlc_free(QairtSystemDlc_Handle_t dlcHandle);

//=============================================================================
// Function Pointer Types
//=============================================================================

typedef Qairt_Status_t (*QairtSystemDlc_CreateFromFileFn_t)(QairtSystemLog_Handle_t,
                                                                 const char*,
                                                                 QairtSystemDlc_Handle_t*);

typedef Qairt_Status_t (*QairtSystemDlc_CreateFromBinaryFn_t)(QairtSystemLog_Handle_t,
                                                                   const uint8_t*,
                                                                   const uint64_t,
                                                                   QairtSystemDlc_Handle_t*);

typedef Qairt_Status_t (*QairtSystemDlc_ComposeGraphsFn_t)(
    QairtSystemDlc_Handle_t,
    const QairtSystemDlc_GraphConfigInfoHandle_t*,
    const uint32_t,
    QairtBackend_Handle_t,
    QairtContext_Handle_t,
    QairtApi_Interface_t,
    QairtSystem_Context_GraphInfoSetHandle_t*);

typedef Qairt_Status_t (*QairtSystemDlc_GetOpMappingsFn_t)(QairtSystemDlc_Handle_t,
                                                                const void**,
                                                                uint32_t*);

typedef Qairt_Status_t (*QairtSystemDlc_FreeFn_t)(QairtSystemDlc_Handle_t);

typedef Qairt_Status_t (*QairtSystemDlc_GraphConfigInfo_CreateFn_t)(
    QairtSystemDlc_GraphConfigInfoHandle_t*);

typedef Qairt_Status_t (*QairtSystemDlc_GraphConfigInfo_FreeFn_t)(
    QairtSystemDlc_GraphConfigInfoHandle_t);

typedef Qairt_Status_t (*QairtSystemDlc_GraphConfigInfo_SetGraphNameFn_t)(
    QairtSystemDlc_GraphConfigInfoHandle_t, const char*);

typedef Qairt_Status_t (*QairtSystemDlc_GraphConfigInfo_GetGraphNameFn_t)(
    QairtSystemDlc_GraphConfigInfoHandle_t, const char**);

typedef Qairt_Status_t (*QairtSystemDlc_GraphConfigInfo_SetGraphConfigFn_t)(
    QairtSystemDlc_GraphConfigInfoHandle_t, QairtGraph_ConfigHandle_t);

//=============================================================================
// Public Interfaces
//=============================================================================

/**
 * @struct QairtSystemDlc_V1_t
 * @brief This struct contains the functions that define the V1 interface for
 *        QAIRT System DLC.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtSystemDlc_CreateFromFileFn_t createFromFile;
  QairtSystemDlc_CreateFromBinaryFn_t createFromBinary;
  QairtSystemDlc_ComposeGraphsFn_t composeGraphs;
  QairtSystemDlc_GetOpMappingsFn_t getOpMappings;
  QairtSystemDlc_FreeFn_t free;
} QairtSystem_DlcV1_t;

typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtSystemDlc_GraphConfigInfo_CreateFn_t create;
  QairtSystemDlc_GraphConfigInfo_FreeFn_t free;
  QairtSystemDlc_GraphConfigInfo_SetGraphNameFn_t setGraphName;
  QairtSystemDlc_GraphConfigInfo_GetGraphNameFn_t getGraphName;
  QairtSystemDlc_GraphConfigInfo_SetGraphConfigFn_t setGraphConfig;
} QairtSystem_Dlc_GraphConfigInfoV1_t;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // QAISW_QAIRTSYSTEMDLC_H

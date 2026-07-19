//=============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//=============================================================================

/**
 * @file
 * @brief   Context component API
 *
 *          A header which contains the top level QAIRT context component and the interfaces it
 * implements.
 */

#ifndef QAISW_QAIRTCONTEXT_H
#define QAISW_QAIRTCONTEXT_H

#include "QairtBackend/QairtBackend.h"
#include "QairtCommon/QairtCommon.h"
#include "QairtContextBinary.h"
#include "QairtContextConfig.h"
#include "QairtDevice/QairtDevice.h"
#include "QairtGraph/QairtGraph.h"
#include "QairtMem/QairtMemCommon.h"
#include "QairtMem/QairtMemDescriptor.h"
#include "QairtTensor/QairtTensor.h"

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
// Macros
//=============================================================================

#define QAIRT_CONTEXT_V1_ID QAIRT_MIN_ID_CONTEXT

#define QAIRT_CONTEXT_ASYNC_EXEC_DEPTH_V1_ID QAIRT_MIN_ID_CONTEXT + 1

#define QAIRT_CONTEXT_ENABLE_GRAPHS_V1_ID QAIRT_MIN_ID_CONTEXT + 2

#define QAIRT_CONTEXT_CONFIG_V1_ID QAIRT_MIN_ID_CONTEXT + 3

#define QAIRT_CONTEXT_BINARY_BUFFER_V1_ID QAIRT_MIN_ID_CONTEXT + 4

#define QAIRT_CONTEXT_BINARY_V1_ID QAIRT_MIN_ID_CONTEXT + 5

//=============================================================================
// Data Types
//=============================================================================
typedef struct _QairtContext_Handle_t* QairtContext_Handle_t;

//=============================================================================
// Public Functions
//=============================================================================

/**
 * @brief Create a context handle associated with a backend and device.
 *
 * @param[in] backendHandle A handle to a created backend.
 *
 * @param[in] deviceHandle A handle to a device. NULL is allowed and is
 *                         equivalent to calling QairtDevice_create() with a
 *                         NULL config.
 * @param[in] configHandle A handle to a context configuration. NULL is allowed
 *                         and indicates default configuration values are used.
 * @param[out] contextHandle A handle to the created context.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Context was successfully created.
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: At least one argument
 *           is invalid.
 *         - QAIRT_CONTEXT_ERROR_MEM_ALLOC: Failure in allocating memory
 *           when creating context.
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: _backendHandle_ or
 *           _deviceHandle_ is not a valid handle.
 *         - QAIRT_CONTEXT_ERROR_UNSUPPORTED_FEATURE: An optional feature
 *           is not supported.
 *         - QAIRT_CONTEXT_ERROR_INVALID_CONFIG: One or more config values
 *           is invalid.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence
 *           (successful recovery).
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence
 *           (unsuccessful recovery).
 *
 * @note Use corresponding API through QairtContext_V1_t.
 */
Qairt_Status_t QairtContext_create(QairtBackend_Handle_t backendHandle,
                                   QairtDevice_Handle_t deviceHandle,
                                   QairtContext_ConfigHandle_t configHandle,
                                   QairtContext_Handle_t* contextHandle);

/**
 * @brief Free a context handle and associated resources.
 *
 * @param[in] contextHandle A handle to a created context.
 *
 * @param[in] profileHandle Optional profile handle to collect metrics during
 *                          context teardown. Use NULL to disable profiling. A
 *                          handle being re-used would reset and is populated with values from the
 *                          current call.
 * @return Error code:
 *         - QAIRT_SUCCESS: Context was successfully freed.
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: _contextHandle_ is not a
 *           valid handle.
 *         - QAIRT_CONTEXT_ERROR_MEM_ALLOC: Memory deallocation error.
 *         - QAIRT_CONTEXT_ERROR_SET_PROFILE: Failed to set profiling
 *           info.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence
 *           (successful recovery).
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence
 *           (unsuccessful recovery).
 *
 * @note Use corresponding API through QairtContext_V1_t.
 */
Qairt_Status_t QairtContext_free(QairtContext_Handle_t contextHandle,
                                 QairtProfile_Handle_t profileHandle);

/**
 * @brief Set or modify configuration options on an already created context.
 *        Backends are not required to support this API.
 *
 * @param[in] contextHandle A handle to a created context.
 *
 * @param[in] configHandle A handle to a context configuration. If a backend
 *                         cannot support the provided config, this call will
 *                         fail.
 * @return Error code:
 *         - QAIRT_SUCCESS: Configuration was successfully set.
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: _contextHandle_ or
 *           _configHandle_ is not a valid handle.
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: At least one config
 *           option is invalid.
 *         - QAIRT_CONTEXT_ERROR_UNSUPPORTED_FEATURE: One or more
 *           configuration options are not supported.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence
 *           (successful recovery).
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence
 *           (unsuccessful recovery).
 *
 * @note Use corresponding API through QairtContext_V1_t.
 */
Qairt_Status_t QairtContext_setConfig(QairtContext_Handle_t contextHandle,
                                      QairtContext_ConfigHandle_t configHandle);

/**
 * @brief Get the size of the binary (serialized) representation of a context.
 *        This function must be called after all entities in the context
 *        have been finalized.
 *
 * @param[in] contextHandle A handle to a created context.
 *
 * @param[out] size The size in bytes of the context binary.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Size was successfully retrieved.
 *         - QAIRT_CONTEXT_ERROR_UNSUPPORTED_FEATURE: Binary serialization
 *           is not supported.
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: _contextHandle_ is not a
 *           valid handle.
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: _size_ is NULL.
 *         - QAIRT_CONTEXT_ERROR_NOT_FINALIZED: If there were any
 *           non-finalized entities in the context.
 *         - QAIRT_CONTEXT_ERROR_GET_BINARY_SIZE_FAILED: Operation failure
 *           due to other factors.
 *         - QAIRT_COMMON_ERROR_OPERATION_NOT_PERMITTED: Attempting to get
 *           binary size for a context re-created from a cached binary.
 *         - QAIRT_CONTEXT_ERROR_MEM_ALLOC: Not enough memory is available
 *           to retrieve the context content.
 *
 * @note Use corresponding API through QairtContext_V1_t.
 */
Qairt_Status_t QairtContext_getBinarySize(QairtContext_Handle_t contextHandle, uint64_t* size);

/**
 * @brief Get the binary (serialized) representation of a context. The binary
 *        can be used to re-create context by using
 *        QairtContext_createFromBinary(). This function must be called after all
 *        entities in the context have been finalized. Unconsumed tensors are not
 *        included in the binary. Client is responsible for allocation of sufficient
 *        and valid memory to hold the serialized context content produced by this
 *        method. It is recommended the user calls QairtContext_getBinarySize() to
 *        allocate a buffer of sufficient space to hold the binary.
 *
 * @param[in] contextHandle A handle to a created context.
 *
 * @param[in] binaryBufferHandle A handle to a binary buffer to store the
 *                               context binary.
 * @param[out] writtenBufferSize The actual size written to
 *                               _binaryBufferHandle_'s underlying buffer, in
 *                               bytes.
 * @return Error code:
 *         - QAIRT_SUCCESS: Binary was successfully retrieved.
 *         - QAIRT_CONTEXT_ERROR_UNSUPPORTED_FEATURE: Binary serialization
 *           is not supported.
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: _contextHandle_ or
 *           _binaryBufferHandle_ is not a valid handle.
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: One of the arguments to
 *           the API is invalid/NULL.
 *         - QAIRT_CONTEXT_ERROR_NOT_FINALIZED: If there were any
 *           non-finalized entities in the context.
 *         - QAIRT_CONTEXT_ERROR_GET_BINARY_FAILED: Operation failure due
 *           to other factors.
 *         - QAIRT_COMMON_ERROR_OPERATION_NOT_PERMITTED: Attempting to get
 *           binary for a context re-created from a cached binary.
 *         - QAIRT_CONTEXT_ERROR_MEM_ALLOC: Not enough memory is available
 *           to retrieve the context content.
 *
 * @note Use corresponding API through QairtContext_V1_t.
 */
Qairt_Status_t QairtContext_getBinary(QairtContext_Handle_t contextHandle,
                                      QairtContext_BinaryBufferHandle_t binaryBufferHandle,
                                      uint64_t* writtenBufferSize);

/**
 * @brief Validate a context binary for compatibility with the backend and
 *        device.
 *
 * @param[in] backendHandle A handle to a created backend.
 *
 * @param[in] deviceHandle A handle to a device. NULL is allowed and indicates
 *                         the default device.
 * @param[in] configHandle A handle to a context configuration. NULL is allowed.
 *
 * @param[in] contextBinaryHandle A handle to a context binary buffer to
 *                                validate.
 * @return Error code:
 *         - QAIRT_SUCCESS: Binary is valid and compatible.
 *         - QAIRT_CONTEXT_ERROR_UNSUPPORTED_FEATURE: Binary validation is
 *           not supported.
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: _contextBinaryHandle_
 *           is NULL.
 *         - QAIRT_CONTEXT_ERROR_MEM_ALLOC: Memory allocation error while
 *           validating binary cache.
 *         - QAIRT_CONTEXT_ERROR_CREATE_FROM_BINARY: Failed to validate
 *           binary cache.
 *         - QAIRT_CONTEXT_ERROR_BINARY_VERSION: Incompatible version of
 *           the binary.
 *         - QAIRT_CONTEXT_ERROR_BINARY_CONFIGURATION: Binary is not
 *           configured for this device.
 *         - QAIRT_CONTEXT_ERROR_BINARY_SUBOPTIMAL: Suboptimal binary is
 *           used when QAIRT_CONTEXT_BINARY_COMPATIBILITY_STRICT is specified in the
 *           config option.
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: _backendHandle_ or
 *           _deviceHandle_ is not a valid handle.
 *         - QAIRT_CONTEXT_ERROR_INVALID_CONFIG: One or more config values
 *           is invalid.
 *
 * @note Use corresponding API through QairtContext_V1_t.
 */
Qairt_Status_t QairtContext_validateBinary(QairtBackend_Handle_t backendHandle,
                                           QairtDevice_Handle_t deviceHandle,
                                           QairtContext_ConfigHandle_t configHandle,
                                           QairtContext_BinaryBufferHandle_t contextBinaryHandle);

/**
 * @brief Create a context from a binary representation.
 *
 * @param[in] backendHandle A handle to a created backend.
 *
 * @param[in] deviceHandle A handle to a device. NULL is allowed and indicates
 *                         the default device.
 * @param[in] configHandle A handle to a context configuration. NULL is allowed.
 *
 * @param[in] signalHandle Optional signal handle for controlling the operation.
 *                         Use NULL to proceed without signal control. The
 *                         signal object, if not NULL, is considered to be in-use for the duration
 * of the call.
 * @param[in] profileHandle Optional profile handle to collect metrics. Use NULL
 *                          to disable profiling. A handle being re-used would
 *                          reset and is populated with values from the current call.
 * @param[in] binaryHandle A handle to a context binary buffer.
 *
 * @param[out] contextHandle A handle to the created context.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Context was successfully created from binary.
 *         - QAIRT_CONTEXT_ERROR_UNSUPPORTED_FEATURE: Binary
 *           deserialization is not supported.
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: _binaryHandle_ or
 *           _contextHandle_ is NULL.
 *         - QAIRT_CONTEXT_ERROR_MEM_ALLOC: Memory allocation error while
 *           creating context.
 *         - QAIRT_CONTEXT_ERROR_CREATE_FROM_BINARY: Failed to deserialize
 *           binary and create context from it.
 *         - QAIRT_CONTEXT_ERROR_BINARY_VERSION: Incompatible version of
 *           the binary.
 *         - QAIRT_CONTEXT_ERROR_BINARY_CONFIGURATION: Binary is not
 *           configured for this device.
 *         - QAIRT_CONTEXT_ERROR_BINARY_SUBOPTIMAL: Suboptimal binary is
 *           used when QAIRT_CONTEXT_BINARY_COMPATIBILITY_STRICT is specified in the
 *           config option.
 *         - QAIRT_CONTEXT_ERROR_SET_PROFILE: Failed to set profiling
 *           info.
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: _backendHandle_,
 *           _profileHandle_, or _deviceHandle_ is not a valid handle.
 *         - QAIRT_CONTEXT_ERROR_INVALID_CONFIG: One or more config values
 *           is invalid.
 *         - QAIRT_CONTEXT_ERROR_ABORTED: The call is aborted before
 *           completion due to user cancellation.
 *         - QAIRT_CONTEXT_ERROR_TIMED_OUT: The call is aborted before
 *           completion due to a timeout.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence
 *           (successful recovery).
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence
 *           (unsuccessful recovery).
 *
 * @note Use corresponding API through QairtContext_V1_t.
 */
Qairt_Status_t QairtContext_createFromBinary(QairtBackend_Handle_t backendHandle,
                                             QairtDevice_Handle_t deviceHandle,
                                             QairtContext_ConfigHandle_t configHandle,
                                             QairtSignal_Handle_t signalHandle,
                                             QairtProfile_Handle_t profileHandle,
                                             QairtContext_BinaryBufferHandle_t binaryHandle,
                                             QairtContext_Handle_t* contextHandle);

/**
 * @brief A function to finish context creation when originally created with
 *        deferred graph initialization enabled (see
 *        QAIRT_CONTEXT_CONFIG_OPTION_DEFER_GRAPH_INIT)
 *
 * @param[in] contextHandle A handle to a created context.
 *
 * @param[in] profileHandle Optional profile handle to collect metrics during
 *                          finalization. Use NULL to disable profiling. A
 *                          handle being re-used would reset and is populated with values from the
 *                          current call.
 * @return Error code:
 *         - QAIRT_SUCCESS: Context was successfully finalized.
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: _contextHandle_ is not a
 *           valid handle.
 *         - QAIRT_CONTEXT_ERROR_MEM_ALLOC: Memory allocation error while
 *           creating context.
 *         - QAIRT_CONTEXT_ERROR_CREATE_FROM_BINARY: Failed to deserialize
 *           binary and create context from it.
 *         - QAIRT_CONTEXT_ERROR_BINARY_VERSION: Incompatible version of
 *           the binary.
 *         - QAIRT_CONTEXT_ERROR_BINARY_CONFIGURATION: Binary is not
 *           configured for this device.
 *         - QAIRT_CONTEXT_ERROR_BINARY_SUBOPTIMAL: Suboptimal binary is
 *           used when QAIRT_CONTEXT_BINARY_COMPATIBILITY_STRICT is specified in the
 *           config option.
 *         - QAIRT_CONTEXT_ERROR_SET_PROFILE: Failed to set profiling
 *           info.
 *         - QAIRT_CONTEXT_ERROR_INVALID_CONFIG: One or more config values
 *           is invalid.
 *         - QAIRT_CONTEXT_ERROR_UNSUPPORTED_FEATURE: Context finalization
 *           is not supported.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence
 *           (successful recovery).
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence
 *           (unsuccessful recovery).
 *
 * @note Use corresponding API through QairtContext_V1_t.
 */
Qairt_Status_t QairtContext_finalize(QairtContext_Handle_t contextHandle,
                                     QairtProfile_Handle_t profileHandle);

/**
 * @brief Create a graph within a given context associated with a created graph
 *        handle.
 *
 * @param[in] contextHandle A handle to the context in which the graph would be
 *                          created. This must be a valid context handle
 *                          previously created via QairtContext APIs.
 * @param[in] graphName A string identifier for the graph. This name must be
 *                      unique within the context and is used for retrieval and
 *                      caching purposes.
 * @param[in] configHandle A handle to a configuration object for the graph.
 *                         This handle may encapsulate multiple configuration
 *                         options. If NULL, default configuration values are used.
 * @param[out] graphHandle A handle to the created graph.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Graph was successfully created.
 *         - QAIRT_GRAPH_ERROR_INVALID_ARGUMENT: One or more arguments are
 *           invalid (e.g., NULL graph handle).
 *         - QAIRT_GRAPH_ERROR_INVALID_NAME: Graph name is NULL or not
 *           unique within the context.
 *         - QAIRT_GRAPH_ERROR_INVALID_HANDLE: Context handle is invalid.
 *         - QAIRT_GRAPH_ERROR_MEM_ALLOC: Graph creation failed due to
 *           memory/resource allocation issues.
 *         - QAIRT_GRAPH_ERROR_UNSUPPORTED_FEATURE: One or more
 *           configuration options are not supported.
 *         - QAIRT_GRAPH_ERROR_CREATE_FAILED: Graph creation failed due to
 *           an internal error.
 *         - QAIRT_GRAPH_ERROR_PROFILE_IN_USE: A profile handle used in
 *           config is already bound to another graph.
 *         - QAIRT_COMMON_ERROR_OPERATION_NOT_PERMITTED: Operation not
 *           permitted in the current context state.
 *
 * @note Use corresponding API through QairtContext_V1_t.
 */
Qairt_Status_t QairtContext_createGraph(QairtContext_Handle_t contextHandle,
                                        const char* graphName,
                                        const QairtGraph_ConfigHandle_t configHandle,
                                        QairtGraph_Handle_t* graphHandle);

/**
 * @brief Retrieve a graph from a context by name.
 *
 * @param[in] contextHandle A handle to the context from which to retrieve the
 *                          graph.
 * @param[in] graphName The name of the graph to retrieve.
 *
 * @param[out] graphHandle A handle to the retrieved graph.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Graph was successfully retrieved.
 *         - QAIRT_GRAPH_ERROR_INVALID_ARGUMENT: _graphName_ or
 *           _graphHandle_ is NULL.
 *         - QAIRT_GRAPH_ERROR_INVALID_HANDLE: _contextHandle_ is not a
 *           valid handle.
 *         - QAIRT_GRAPH_ERROR_GRAPH_DOES_NOT_EXIST: No graph with the
 *           specified name exists in the context.
 *         - QAIRT_GRAPH_ERROR_INVALID_NAME: _graphName_ is invalid.
 *         - QAIRT_GRAPH_ERROR_MEM_ALLOC: Memory allocation error while
 *           retrieving graph.
 *         - QAIRT_GRAPH_ERROR_UNSUPPORTED_FEATURE: Graph retrieval is not
 *           supported.
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: _contextHandle_ is not a
 *           valid context handle.
 *         - QAIRT_CONTEXT_ERROR_CTX_DOES_NOT_EXIST: Context does not
 *           exist.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurrence
 *           (successful recovery).
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurrence
 *           (unsuccessful recovery).
 *
 * @note Use corresponding API through QairtContext_V1_t.
 */
Qairt_Status_t QairtContext_retrieveGraph(QairtContext_Handle_t contextHandle,
                                          const char* graphName,
                                          QairtGraph_Handle_t* graphHandle);

/**
 * @brief Register existing memory to memory handles.
 *        Used to instruct QAIRT to use this memory directly.
 *
 * @param[in] contextHandle A context handle.
 *
 * @param[in] memDescriptorHandles Array of memory descriptor handles to be
 *                                 registered.
 * @param[in] numDescriptors Number of memory descriptor handles in the array.
 *
 * @param[out] memHandles Array of allocated memory handles, length is
 *                        _numDescriptors_. Same shape as _memDescriptorHandles_
 *                        (i.e. memHandles[n] corresponds to
 *                        memDescriptorHandles[n]).
 * @note memHandles parameter: Array memory is owned by the client. Array size
 *       must be at least _numDescriptors_*sizeof(QairtMem_Handle_t). The
 *       array will be initialized to NULL by the backend. Upon failure, no
 *       memory will be registered and the _memHandles_ array will remain NULL.
 * @return Error code:
 *         - QAIRT_SUCCESS: memory was successfully registered
 *         - QAIRT_CONTEXT_ERROR_UNSUPPORTED_FEATURE: backend does not
 *           support this API
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: _contextHandle_ is not a
 *           valid handle
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: NULL array ptr, invalid
 *           memory descriptor, or _numDescriptors_ is 0
 *         - QAIRT_CONTEXT_ERROR_MEM_ALLOC: Memory allocation error during
 *           registration
 *         - QAIRT_CONTEXT_ERROR_MEM_ALREADY_REGISTERED: memory has already
 *           been registered
 *         - QAIRT_CONTEXT_ERROR_MEM_UNSUPPORTED_TYPE: backend does not
 *           support a memType specified within _memDescriptorHandles_
 *         - QAIRT_CONTEXT_ERROR_MEM_MAPPING: failed to map between memory
 *           file descriptor and memory address
 *         - QAIRT_CONTEXT_ERROR_MEM_INVALID_SHAPE: backend does not support
 *           a memShape specified within _memDescriptorHandles_
 *
 * @note Use corresponding API through QairtContext_V1_t.
 */
QAIRT_API
Qairt_Status_t QairtContext_registerMem(QairtContext_Handle_t contextHandle,
                                        QairtMem_DescriptorHandle_t* memDescriptorHandles,
                                        uint32_t numDescriptors,
                                        QairtMem_Handle_t* memHandles);

/**
 * @brief Create a new tensor on QairtContext_Handle_t.
 *
 *        This call may or may not allocate memory, depending on the tensor type
 *        and the accelerator implementation. Optionally it may be initialized with
 *        data provided in the tensor if present.
 *
 * @warning Context tensors cannot be of type QAIRT_TENSOR_TYPE_NATIVE.
 *          Native tensors connect nodes within a single graph.
 *
 * @warning Context tensors cannot be of datatype QAIRT_DATATYPE_STRING.
 *
 * @param[in] contextHandle The context in which the tensor would be created.
 *
 * @param[in,out] tensorHandle A pre-configured tensor handle. For tensors
 *                             containing static data (such as weights or
 *                             biases), the tensor type is expected to be
 *                             QAIRT_TENSOR_TYPE_STATIC. Valid data must be
 *                             presented in the tensor at creation. This data
 *                             will be copied, and may be safely de-allocated
 *                             after this call returns. Other tensor types
 *                             (e.g: APP_READ, APP_WRITE, APP_READWRITE, NULL)
 *                             must have the data pointer set to NULL at the
 *                             time of creation. Creating a tensor with a name
 *                             that duplicates a previously created tensor name
 *                             in the context and all child graphs results in
 *                             undefined behaviour. The dimensions are treated
 *                             as the maximum dimensions during tensor creation.
 *                             The tensor ID will be assigned by the backend
 *                             directly to this handle as part of this call.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Successfully created a context tensor
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: Provided context or tensor
 *           handle is invalid
 *         - QAIRT_TENSOR_ERROR_INVALID_TENSOR_PARAM: One or more tensor
 *           parameters is invalid
 *         - QAIRT_TENSOR_ERROR_UNSUPPORTED_TENSOR_PARAM: One or more
 *           tensor parameters are unsupported
 *         - QAIRT_CONTEXT_ERROR_MEM_ALLOC: Failure in creating tensor due
 *           to issues with memory allocation
 *         - QAIRT_CONTEXT_ERROR_UNSUPPORTED_FEATURE: some API feature is
 *           not supported yet
 *
 * @note Use corresponding API through QairtContext_V1_t.
 */
Qairt_Status_t QairtContext_createContextTensor(QairtContext_Handle_t contextHandle,
                                                QairtTensor_Handle_t tensorHandle);

/**
 * @brief Get the size of a binary section for a context. The section type
 *        specifies which portion of the binary to query. This function must
 *        be called after all entities in the context have been finalized.
 *
 * @param[in] contextHandle A handle to a created context.
 *
 * @param[in] graphHandle A graph handle. This argument is optional. When
 *                        supplied, the returned size only applies to the
 *                        context binary section pertaining to this graph.
 *                        When NULL, the returned size applies to all graphs
 *                        in the context.
 * @param[in] section The section of the binary to query.
 *
 * @param[out] binaryBufferSize The amount of memory in bytes a client will
 *                              need to allocate to hold the context binary
 *                              section.
 * @return Error code:
 *         - QAIRT_SUCCESS: Size was successfully retrieved.
 *         - QAIRT_CONTEXT_ERROR_UNSUPPORTED_FEATURE: Binary section
 *           retrieval is not supported.
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: _contextHandle_ is not a
 *           valid handle.
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: _binaryBufferSize_ is
 *           NULL.
 *         - QAIRT_CONTEXT_ERROR_NOT_FINALIZED: If there were any
 *           non-finalized entities in the context.
 *         - QAIRT_CONTEXT_ERROR_GET_BINARY_SIZE_FAILED: Operation failure
 *           due to other factors.
 *         - QAIRT_COMMON_ERROR_OPERATION_NOT_PERMITTED: Attempting to get
 *           binary size for a context re-created from a cached binary.
 *         - QAIRT_CONTEXT_ERROR_MEM_ALLOC: Not enough memory is available
 *           to retrieve the context content.
 *
 * @note Use corresponding API through QairtContext_V1_t.
 */
Qairt_Status_t QairtContext_getBinarySectionSize(QairtContext_Handle_t contextHandle,
                                                 QairtGraph_Handle_t graphHandle,
                                                 QairtContext_SectionType_t section,
                                                 uint64_t* binaryBufferSize);

/**
 * @brief Retrieve a section of the context binary. The section type specifies
 *        which portion of the binary to retrieve. The size of the section
 *        should first be queried via QairtContext_getBinarySectionSize() to
 *        allocate a buffer of sufficient size.
 *
 * @param[in] contextHandle A handle to a created context.
 *
 * @param[in] graphHandle A graph handle. This argument is optional. When
 *                        supplied, the returned binary only contains the
 *                        context binary section pertaining to this graph.
 *                        When NULL, the returned binary contains associated
 *                        updates to all graphs in the context.
 * @param[in] section The section of the binary to retrieve.
 *
 * @param[in] binaryBufferHandle A handle to a binary buffer to store the
 *                               section binary. The buffer must be
 *                               pre-allocated with sufficient size.
 * @param[in] profileHandle Optional profile handle to collect metrics. Use
 *                          NULL to disable profiling. A handle being re-used
 *                          would reset and is populated with values from the
 *                          current call.
 * @param[in] signalHandle Optional signal handle for controlling the
 *                         operation. Use NULL to proceed without signal
 *                         control. The signal object, if not NULL, is
 *                         considered to be in-use for the duration of the
 *                         call.
 * @param[out] writtenBufferSize The actual number of bytes written into
 *                               _binaryBufferHandle_'s underlying buffer.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Binary section was successfully retrieved.
 *         - QAIRT_CONTEXT_ERROR_UNSUPPORTED_FEATURE: Binary section
 *           retrieval is not supported.
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: _contextHandle_ or
 *           _graphHandle_ is not a valid handle.
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: One of the arguments to
 *           the API is invalid/NULL.
 *         - QAIRT_CONTEXT_ERROR_NOT_FINALIZED: If there were any
 *           non-finalized entities in the context.
 *         - QAIRT_CONTEXT_ERROR_GET_BINARY_FAILED: Operation failure due
 *           to other factors.
 *         - QAIRT_COMMON_ERROR_OPERATION_NOT_PERMITTED: Attempting to get
 *           binary section for a context re-created from a cached binary.
 *         - QAIRT_CONTEXT_ERROR_MEM_ALLOC: Not enough memory is available
 *           to retrieve the context content.
 *
 * @note Use corresponding API through QairtContext_V1_t.
 */
Qairt_Status_t QairtContext_getBinarySection(QairtContext_Handle_t contextHandle,
                                             QairtGraph_Handle_t graphHandle,
                                             QairtContext_SectionType_t section,
                                             QairtContext_BinaryBufferHandle_t binaryBufferHandle,
                                             QairtProfile_Handle_t profileHandle,
                                             QairtSignal_Handle_t signalHandle,
                                             uint64_t* writtenBufferSize);

/**
 * @brief Apply a binary section to a context. The section was previously
 *        obtained via QairtContext_getBinarySection(). If successful, this
 *        section overwrites previously applied sections. If the call fails,
 *        the context retains its prior state and remains valid for subsequent
 *        inferences.
 *
 * @param[in] contextHandle A handle to a created context.
 * @param[in] graphHandle A graph handle. This argument is optional. When
 *                        supplied, the binary section pertains to this graph.
 *                        When NULL, the binary section applies to all graphs
 *                        in the context.
 * @param[in] section The section type  of the binary to apply.
 * @param[in] binaryBufferHandle A handle to a binary buffer containing the
 *                               section to apply.
 * @param[in] profileHandle Optional profile handle to collect metrics. Use
 *                          NULL to disable profiling. A handle being re-used
 *                          would reset and is populated with values from the
 *                          current call.
 * @param[in] signalHandle Optional signal handle for controlling the
 *                         operation. Use NULL to proceed without signal
 *                         control. The signal object, if not NULL, is
 *                         considered to be in-use for the duration of the
 *                         call.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Binary section was successfully applied.
 *         - QAIRT_CONTEXT_ERROR_UNSUPPORTED_FEATURE: A feature is not
 *           supported.
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: A provided handle is not a
 *           valid.
 *         - QAIRT_CONTEXT_ERROR_MEM_ALLOC: Memory allocation error while
 *           applying context update.
 *         - QAIRT_CONTEXT_ERROR_SET_PROFILE: Failed to set profiling info.
 *
 * @note Use corresponding API through QairtContext_V1_t.
 * @note When using this API with QAIRT_CONTEXT_CONFIG_PERSISTENT_BINARY
 *       enabled, binaryBufferHandle should remain valid from the first call
 *       to QairtContext_applyBinarySection until QairtContext_free.
 */
Qairt_Status_t QairtContext_applyBinarySection(QairtContext_Handle_t contextHandle,
                                               QairtGraph_Handle_t graphHandle,
                                               QairtContext_SectionType_t section,
                                               QairtContext_BinaryBufferHandle_t binaryBufferHandle,
                                               QairtProfile_Handle_t profileHandle,
                                               QairtSignal_Handle_t signalHandle);

typedef Qairt_Status_t (*QairtContext_CreateFn_t)(QairtBackend_Handle_t,
                                                  QairtDevice_Handle_t,
                                                  QairtContext_ConfigHandle_t,
                                                  QairtContext_Handle_t*);

typedef Qairt_Status_t (*QairtContext_FreeFn_t)(QairtContext_Handle_t, QairtProfile_Handle_t);

typedef Qairt_Status_t (*QairtContext_SetConfigFn_t)(QairtContext_Handle_t,
                                                     QairtContext_ConfigHandle_t);

typedef Qairt_Status_t (*QairtContext_GetBinarySizeFn_t)(QairtContext_Handle_t, uint64_t*);

typedef Qairt_Status_t (*QairtContext_GetBinaryFn_t)(QairtContext_Handle_t,
                                                     QairtContext_BinaryBufferHandle_t,
                                                     uint64_t*);

typedef Qairt_Status_t (*QairtContext_ValidateBinaryFn_t)(QairtBackend_Handle_t,
                                                          QairtDevice_Handle_t,
                                                          QairtContext_ConfigHandle_t,
                                                          QairtContext_BinaryBufferHandle_t);

typedef Qairt_Status_t (*QairtContext_CreateFromBinaryFn_t)(QairtBackend_Handle_t,
                                                            QairtDevice_Handle_t,
                                                            QairtContext_ConfigHandle_t,
                                                            QairtSignal_Handle_t,
                                                            QairtProfile_Handle_t,
                                                            QairtContext_BinaryBufferHandle_t,
                                                            QairtContext_Handle_t*);

typedef Qairt_Status_t (*QairtContext_FinalizeFn_t)(QairtContext_Handle_t, QairtProfile_Handle_t);

typedef Qairt_Status_t (*QairtContext_CreateGraphFn_t)(QairtContext_Handle_t,
                                                       const char*,
                                                       const QairtGraph_ConfigHandle_t,
                                                       QairtGraph_Handle_t*);

typedef Qairt_Status_t (*QairtContext_RetrieveGraphFn_t)(QairtContext_Handle_t,
                                                         const char*,
                                                         QairtGraph_Handle_t*);

typedef Qairt_Status_t (*QairtContext_RegisterMemFn_t)(QairtContext_Handle_t,
                                                       QairtMem_DescriptorHandle_t*,
                                                       uint32_t,
                                                       QairtMem_Handle_t*);

typedef Qairt_Status_t (*QairtContext_CreateContextTensorFn_t)(QairtContext_Handle_t,
                                                               QairtTensor_Handle_t);

/**
 * @brief Update tensors previously created on a context. Valid fields to update
 *        are: data and quantization parameters for UPDATEABLE_STATIC tensors,
 *        quantization parameters for UPDATEABLE_NATIVE, UPDATEABLE_APP_READ,
 *        UPDATEABLE_APP_WRITE, and UPDATEABLE_APP_READWRITE tensors.
 *
 * @param[in] contextHandle The context on which the tensors were created.
 *
 * @param[in] tensorHandles Array of tensor handles to update. Each handle must
 *                          carry the ID assigned during tensor creation.
 * @param[in] numTensors Number of tensor handles in the _tensorHandles_ array.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Successfully updated context tensors
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: Provided context or tensor
 *           handle is invalid
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: _tensorHandles_ is NULL
 *         - QAIRT_TENSOR_ERROR_INCOMPATIBLE_TENSOR_UPDATE: Tensor type does
 *           not support updates
 *         - QAIRT_CONTEXT_ERROR_UNSUPPORTED_FEATURE: some API feature is
 *           not supported yet
 *
 * @note Multiple calls can be made, but updates will not take effect until
 *       QairtGraph_finalize() is called for one or more of the graphs to which
 *       the context tensors are associated.
 *
 * @note Use corresponding API through QairtContext_V1_t.
 */
Qairt_Status_t QairtContext_updateContextTensors(QairtContext_Handle_t contextHandle,
                                                 const QairtTensor_Handle_t* tensorHandles,
                                                 uint64_t numTensors);

typedef Qairt_Status_t (*QairtContext_UpdateContextTensorsFn_t)(QairtContext_Handle_t,
                                                                const QairtTensor_Handle_t*,
                                                                uint64_t);

typedef Qairt_Status_t (*QairtContext_GetBinarySectionSizeFn_t)(QairtContext_Handle_t,
                                                                QairtGraph_Handle_t,
                                                                QairtContext_SectionType_t,
                                                                uint64_t*);

typedef Qairt_Status_t (*QairtContext_GetBinarySectionFn_t)(QairtContext_Handle_t,
                                                            QairtGraph_Handle_t,
                                                            QairtContext_SectionType_t,
                                                            QairtContext_BinaryBufferHandle_t,
                                                            QairtProfile_Handle_t,
                                                            QairtSignal_Handle_t,
                                                            uint64_t*);

typedef Qairt_Status_t (*QairtContext_ApplyBinarySectionFn_t)(QairtContext_Handle_t,
                                                              QairtGraph_Handle_t,
                                                              QairtContext_SectionType_t,
                                                              QairtContext_BinaryBufferHandle_t,
                                                              QairtProfile_Handle_t,
                                                              QairtSignal_Handle_t);

//=============================================================================
// Public Interfaces
//=============================================================================

/**
 * @struct QairtContext_V1_t
 * @brief This struct contains the functions that define the V1 interface for
 *        QAIRT Context.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtContext_CreateFn_t create;
  QairtContext_FreeFn_t free;
  QairtContext_SetConfigFn_t setConfig;
  QairtContext_GetBinarySizeFn_t getBinarySize;
  QairtContext_GetBinaryFn_t getBinary;
  QairtContext_ValidateBinaryFn_t validateBinary;
  QairtContext_CreateFromBinaryFn_t createFromBinary;
  QairtContext_FinalizeFn_t finalize;
  QairtContext_CreateGraphFn_t createGraph;
  QairtContext_RetrieveGraphFn_t retrieveGraph;
  QairtContext_RegisterMemFn_t registerMem;
  QairtContext_CreateContextTensorFn_t createContextTensor;
  QairtContext_UpdateContextTensorsFn_t updateContextTensors;
  QairtContext_GetBinarySectionSizeFn_t getBinarySectionSize;
  QairtContext_GetBinarySectionFn_t getBinarySection;
  QairtContext_ApplyBinarySectionFn_t applyBinarySection;
} QairtContext_V1_t;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // QAISW_QAIRTCONTEXT_H

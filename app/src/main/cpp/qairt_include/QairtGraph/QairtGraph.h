//==============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//==============================================================================

#ifndef QAISW_QAIRTGRAPH_H
#define QAISW_QAIRTGRAPH_H

#include "QairtGraphConfig.h"
#include "QairtProfile/QairtProfile.h"
#include "QairtSignal/QairtSignal.h"
#include "QairtTensor/QairtTensor.h"

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
// Macros
//=============================================================================

#define QAIRT_GRAPH_V1_ID        QAIRT_MIN_ID_GRAPH
#define QAIRT_GRAPH_CONFIG_V1_ID QAIRT_MIN_ID_GRAPH + 2

//=============================================================================
// Datatypes
//=============================================================================
/**
 * @brief QAIRT Graph API result / error codes.
 */
typedef enum {
  QAIRT_GRAPH_MIN_ERROR = QAIRT_MIN_ERROR_GRAPH,
  ////////////////////////////////////////

  /// QAIRT Graph success
  QAIRT_GRAPH_NO_ERROR = QAIRT_SUCCESS,
  /// There is an optional API component that is not supported yet. See
  /// QairtCapability.
  QAIRT_GRAPH_ERROR_UNSUPPORTED_FEATURE = QAIRT_COMMON_ERROR_NOT_SUPPORTED,
  /// General error relating to memory allocation in processing graph API
  QAIRT_GRAPH_ERROR_MEM_ALLOC = QAIRT_COMMON_ERROR_MEM_ALLOC,
  /// General type of graph error, which has not been identified as any
  /// other error type. Any Graph API can return this error code.
  QAIRT_GRAPH_ERROR_GENERAL = QAIRT_COMMON_ERROR_GENERAL,
  /// An argument to QNN API is deemed invalid by a backend
  QAIRT_GRAPH_ERROR_INVALID_ARGUMENT = 6000,
  /// Invalid graph handle
  QAIRT_GRAPH_ERROR_INVALID_HANDLE = 6001,
  /// No graph with specified info is registered in the backend
  QAIRT_GRAPH_ERROR_GRAPH_DOES_NOT_EXIST = 6002,
  /// Invalid or duplicate graph name
  QAIRT_GRAPH_ERROR_INVALID_NAME = 6003,
  /// Invalid or NULL QNN tensor
  QAIRT_GRAPH_ERROR_INVALID_TENSOR = 6004,
  /// Some elements in the op config data are invalid
  QAIRT_GRAPH_ERROR_INVALID_OP_CONFIG = 6005,
  /// Failure to set profile
  QAIRT_GRAPH_ERROR_SET_PROFILE = 6006,
  /// Node added before its dependent node(s)
  QAIRT_GRAPH_ERROR_UNCONNECTED_NODE = 6007,
  /// Failure in creating graph with specified configuration
  QAIRT_GRAPH_ERROR_CREATE_FAILED = 6020,
  /// Graph couldn't be optimized with specified list of ops or config
  QAIRT_GRAPH_ERROR_OPTIMIZATION_FAILED = 6021,
  /// Graph finalize failed
  QAIRT_GRAPH_ERROR_FINALIZE_FAILED = 6022,
  /// Attempt to execute graph before finalizing it
  QAIRT_GRAPH_ERROR_GRAPH_NOT_FINALIZED = 6023,
  /// Attempt to modify graph after finalizing it
  QAIRT_GRAPH_ERROR_GRAPH_FINALIZED = 6024,
  /// FIFO queue cannot register any more async execution requests
  QAIRT_GRAPH_ERROR_EXECUTION_ASYNC_FIFO_FULL = 6025,
  /// A control signal object was provided to a call, but that signal object
  /// is already in-use by another call.
  QAIRT_GRAPH_ERROR_SIGNAL_IN_USE = 6030,
  /// Call aborted early due to a QnnSignal_trigger call issued
  /// to the observed signal object.
  QAIRT_GRAPH_ERROR_ABORTED = 6031,
  /// Attempt to bind to a graph a profile handle that is already in-use
  /// by another graph.
  QAIRT_GRAPH_ERROR_PROFILE_IN_USE = 6032,
  /// Call aborted early due to a QnnSignal timeout
  QAIRT_GRAPH_ERROR_TIMED_OUT = 6033,
  /// Operation not permitted on a subgraph
  QAIRT_GRAPH_ERROR_SUBGRAPH = 6034,
  /// Graph is not enabled
  QAIRT_GRAPH_ERROR_DISABLED = 6035,
  /// Dynamic tensor shape error
  QAIRT_GRAPH_ERROR_DYNAMIC_TENSOR_SHAPE = 6036,
  /// Tensor sparsity error
  QAIRT_GRAPH_ERROR_TENSOR_SPARSITY = 6037,
  /// Early termination error
  QAIRT_GRAPH_ERROR_EARLY_TERMINATION = 6038,
  /// Invalid context error
  QAIRT_GRAPH_ERROR_INVALID_CONTEXT = 6039,
  /// Failed to terminate
  QAIRT_GRAPH_ERROR_TERMINATE_FAILED = 6040,

  ////////////////////////////////////////
  QAIRT_GRAPH_MAX_ERROR = QAIRT_MAX_ERROR_GRAPH,
  // Unused, present to ensure 32 bits.
  QAIRT_GRAPH_ERROR_UNDEFINED = 0x7FFFFFFF
} QairtGraph_Error_t;

/**
 * @brief   Graph handle used to interface with Graph APIs
 *
 *          The handle itself is created and freed by the client.
 */
typedef struct _QairtGraph_Handle_t* QairtGraph_Handle_t;

typedef void (*Qairt_NotifyFn_t)(void* notifyParam, Qairt_Status_t status);

//=============================================================================
// Public Functions
//=============================================================================

/**
 * @brief Frees a graph handle
 *
 * @param[in] graphHandle A handle to a created graph.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered.
 *         - QAIRT_GRAPH_ERROR_MEM_ALLOC: error related to memory
 *           deallocation
 *         - QAIRT_GRAPH_ERROR_TERMINATE_FAILED: indicates failure to free
 *           resources or failure to invalidate handles and pointers
 *           allocated by the library
 *         - QAIRT_GRAPH_ERROR_INVALID_HANDLE: _graph_ is not a valid
 *           handle
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurence
 *           (successful recovery)
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurence
 *           (unsuccessful recovery)
 *
 * @note Use corresponding API through QairtGraph_V1_t.
 */
Qairt_Status_t QairtGraph_free(QairtGraph_Handle_t graphHandle);

/**
 * @brief A function to create an empty graph which will be a subgraph of
 *        another graph. The function returns a handle to the created
 *        subgraph. A subgraph can not be explicitly finalized or executed. Only a
 *        graph with no parent graphs can be finalized and executed.
 *
 * @param[in] graphHandle Handle to the graph in which the subgraph is created.
 *
 * @param[in] graphName A string which identifies the graph. Graph name allows
 *                      retrieval of the graph after creating the context from
 *                      cached binary. _graphName_ must be unique within the _context_.
 * @param[out] subgraphHandle The created subgraph handle.
 *
 * @note A subgraph can have another subgraph as a parent.
 *
 * @note Nodes and tensors can be added to a subgraph before and/or after the
 *       subgraph handle has been included as part of an op config added
 *       as a node.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: the graph was successfully created
 *         - QAIRT_GRAPH_ERROR_INVALID_ARGUMENT: _subgraphHandle_ is NULL
 *         - QAIRT_GRAPH_ERROR_INVALID_NAME: _graphName_ is NULL or not
 *           unique within the _context_
 *         - QAIRT_GRAPH_ERROR_INVALID_HANDLE: _graphHandle_ is not a
 *           valid handle
 *         - QAIRT_GRAPH_ERROR_MEM_ALLOC: create failed due to
 *           memory/resource allocation
 *         - QAIRT_GRAPH_ERROR_UNSUPPORTED_FEATURE: This API is not yet
 *           supported
 *         - QAIRT_GRAPH_ERROR_CREATE_FAILED: create failed due to some
 *           other reason
 *         - QAIRT_COMMON_ERROR_OPERATION_NOT_PERMITTED: create failed
 *           when context is re-created from binary using QairtContext_createFromBinary().
 *
 * @note Use corresponding API through QairtGraph_V1_t.
 */
Qairt_Status_t QairtGraph_createSubgraph(QairtGraph_Handle_t graphHandle,
                                         const char* graphName,
                                         QairtGraph_Handle_t* subgraphHandle);

/**
 * @brief A function to set/modify configuration options on an already created
 *        graph. Backends are not required to support this API.
 *
 * @param[in] graphHandle A graph handle.
 *
 * @param[in] configHandle A created graph config handle. NULL is allowed
 *                         and indicates no config options are provided. All
 *                         config options have default value, in case not provided. If a backend
 * cannot support all provided configs it will fail.
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_GRAPH_ERROR_INVALID_HANDLE: _graphHandle_
 *           or _configHandle_ is not a valid handle
 *         - QAIRT_GRAPH_ERROR_INVALID_ARGUMENT: at least one config
 *           option is invalid
 *         - QAIRT_GRAPH_ERROR_GRAPH_FINALIZED: at least one valid config
 *           option is not valid on a finalized graph
 *         - QAIRT_GRAPH_ERROR_SUBGRAPH: operation not permitted on a
 *           subgraph
 *         - QAIRT_GRAPH_ERROR_UNSUPPORTED_FEATURE: at least one valid
 *           config option is not supported
 *         - QAIRT_GRAPH_ERROR_PROFILE_IN_USE: when a profile handle is
 *           passed as graph config, that profile handle can only be bound to one graph at
 *           a time
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurence
 *           (successful recovery)
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurence
 *           (unsuccessful recovery)
 *
 * @note Use corresponding API through QairtGraph_V1_t.
 */
Qairt_Status_t QairtGraph_setConfig(QairtGraph_Handle_t graphHandle,
                                    const QairtGraph_ConfigHandle_t configHandle);

/**
 * @brief A function to add a node to the graph.
 *
 * @param[in] graphHandle A handle to the graph to which the node will be added.
 *
 * @note The following conditions must be honored by tensors specified as part
 *       of opConfig:
 *       1. No tensor in the opConfig's output tensors can be of type
 *       QAIRT_TENSOR_TYPE_APP_WRITE or QAIRT_TENSOR_TYPE_STATIC.
 *       2. All parameters in the opConfig that happen to be tensors must be
 *       of the type QAIRT_TENSOR_TYPE_STATIC.
 *       3. Tensors express connectivity between nodes, but it is
 *       permissible for tensors to remain unconsumed (i.e., not act as inputs to any
 *       other node in the graph).
 *
 * @note QairtGraph does not validate opConfig used in creating node beyond
 *       checks for basic sanity. A thorough validation of opConfig
 *       for this node defined in a certain op package has to be done via the
 *       QairtBackend_validateOpConfig API.
 *
 * @note Nodes must be added in dependency order. i.e. all
 *       QAIRT_TENSOR_TYPE_NATIVE inputs to the node must be outputs of a
 *       previously added node.
 *
 * @param[in] opConfigHandle A handle to the operation configuration which
 *                           should be added as a node in the graph. The tensors
 *                           associated with this handle must be created with QairtTensor APIs which
 *                           register them with a backend. Unrecognized tensors in the opConfig
 * result in failure. Since the tensor ID is provided by the backend and is unique, it is sufficient
 * to only specify a valid tensor ID in the Tensor handles associated with the opConfig. All other
 * fields including any static data are ignored by the backend when parsing these tensors.
 * @return Error code:
 *         - QAIRT_SUCCESS: Node was successfully added to the graph.
 *         - QAIRT_GRAPH_ERROR_INVALID_OP_CONFIG: The operation
 *           configuration is invalid or inconsistent with registered op packages or
 *           tensor metadata.
 *         - QAIRT_GRAPH_ERROR_INVALID_TENSOR: One or more tensors
 *           referenced in the opConfig are invalid.
 *         - QAIRT_GRAPH_ERROR_INVALID_HANDLE: The graph handle is
 *           invalid.
 *         - QAIRT_GRAPH_ERROR_GRAPH_FINALIZED: Cannot add nodes to a
 *           finalized graph.
 *         - QAIRT_GRAPH_ERROR_UNCONNECTED_NODE: Node added before its
 *           dependencies.
 *         - QAIRT_GRAPH_ERROR_UNSUPPORTED_FEATURE: One or more features
 *           in the opConfig are not supported.
 *
 * @note Use corresponding API through QairtGraph_V1_t.
 */
Qairt_Status_t QairtGraph_addNode(QairtGraph_Handle_t graphHandle,
                                  QairtOpConfig_Handle_t opConfigHandle);

/**
 * @brief Finalize the graph for execution. If called on a composed graph, the
 *        runtime will process the graph, validate that all operations
 *        are created successfully and that connectivity is correct. If called on a
 *        graph that was retrieved from a context binary (subject to backend support,
 *        see QAIRT_CAPABILITY_GRAPH_SUPPORT_FINALIZE_DESERIALIZED_GRAPH), the runtime
 *        will perform additional setup required before execution.
 *
 * @param[in] graphHandle Handle to the graph to be finalized.
 *
 * @param[in] profileHandle Optional handle to a profiling object. If provided,
 *                          performance metrics will be collected during
 *                          finalization. Use NULL to disable profiling. If continuous profiling has
 * been configured via QAIRT_GRAPH_CONFIG_OPTION_PROFILE_HANDLE, this handle must be NULL.
 * @param[in] signalHandle Optional signal object used to control the execution
 *                         of the finalize process. If NULL, the finalize
 *                         operation proceeds uninterrupted. If provided, the signal object is
 *                         considered in-use for the duration of the call.
 * @note Graphs with zero nodes cannot be finalized and will result in an error.
 *
 * @note Some backends may require that this function is called before execution
 *       of a graph restored from serialized contexts. Refer to
 *       backend-specific documentation for details.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Graph finalized successfully.
 *         - QAIRT_GRAPH_ERROR_INVALID_HANDLE: Graph handle is invalid.
 *         - QAIRT_GRAPH_ERROR_INVALID_ARGUMENT: Invalid parameters
 *           passed, or continuous graph profiling is enabled and the per-API handle is
 *           not NULL.
 *         - QAIRT_GRAPH_ERROR_CREATE_FAILED: Op/kernel creation failed.
 *         - QAIRT_GRAPH_ERROR_OPTIMIZATION_FAILED: Graph optimization
 *           failed.
 *         - QAIRT_GRAPH_ERROR_UNSUPPORTED_FEATURE: One or more features
 *           (e.g., signal or profile) are not supported by the backend.
 *         - QAIRT_GRAPH_ERROR_SET_PROFILE: Failed to set profile handle.
 *         - QAIRT_GRAPH_ERROR_SIGNAL_IN_USE: Signal handle is already in
 *           use by another call.
 *         - QAIRT_GRAPH_ERROR_ABORTED: Finalization aborted due to user
 *           cancellation.
 *         - QAIRT_GRAPH_ERROR_TIMED_OUT: Finalization aborted due to
 *           timeout.
 *         - QAIRT_GRAPH_ERROR_FINALIZE_FAILED: Finalization failed due to
 *           an unspecified error.
 *         - QAIRT_GRAPH_ERROR_SUBGRAPH: Finalization not permitted on
 *           subgraphs.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurence
 *           (successful recovery)
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurence
 *           (unsuccessful recovery)
 *
 * @note Use corresponding API through QairtGraph_V1_t.
 */
Qairt_Status_t QairtGraph_finalize(QairtGraph_Handle_t graphHandle,
                                   QairtProfile_Handle_t profileHandle,
                                   QairtSignal_Handle_t signalHandle);

/**
 * @brief A function to create a new tensor on QairtGraph_Handle_t.
 *
 *        This call may or may not allocate memory, depending on the tensor type
 *        and the accelerator implementation. Optionally it may be initialized with
 *        data provided in the tensor if present.
 *
 * @warning Graph tensors cannot be both inputs and outputs. R/W tensors connect
 *                multiple graphs.
 * @warning Graph tensors cannot be of datatype QAIRT_DATATYPE_STRING.
 *
 * @param[in] graphHandle The graph or sub-graph in which the tensor would be
 *                        created.
 * @param[in,out] tensorHandle A pre-configured tensor handle to be used for
 *                             tensor creation. The tensor ID will be assigned
 *                             by the backend directly to this handle as part
 *                             of this call.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Successfully created a graph tensor
 *         - QAIRT_GRAPH_ERROR_INVALID_HANDLE: Provided graph or tensor
 *           handle is invalid
 *         - QAIRT_TENSOR_ERROR_INVALID_TENSOR_PARAM: One or more tensor
 *           parameters is invalid
 *         - QAIRT_TENSOR_ERROR_UNSUPPORTED_TENSOR_PARAM: One or more
 *           tensor parameters are unsupported
 *         - QAIRT_GRAPH_ERROR_MEM_ALLOC: Failure in creating tensor due
 *           to issues with memory allocation
 *         - QAIRT_GRAPH_ERROR_UNSUPPORTED_FEATURE: some API feature is
 *           not supported yet
 *
 * @note Use corresponding API through QairtGraph_V1_t.
 */

Qairt_Status_t QairtGraph_createGraphTensor(QairtGraph_Handle_t graphHandle,
                                            QairtTensor_Handle_t tensorHandle);

/**
 * @brief Update tensors previously created on a graph. Valid fields to update
 *        are: data and quantization parameters for UPDATEABLE_STATIC tensors,
 *        quantization parameters for UPDATEABLE_NATIVE, UPDATEABLE_APP_READ,
 *        UPDATEABLE_APP_WRITE, and UPDATEABLE_APP_READWRITE tensors.
 *
 * @param[in] graphHandle The graph on which the tensors were created.
 *
 * @param[in] tensorHandles Array of tensor handles to update. Each handle must
 *                          carry the ID assigned during tensor creation.
 * @param[in] numTensors Number of tensor handles in the _tensorHandles_ array.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Successfully updated graph tensors
 *         - QAIRT_GRAPH_ERROR_INVALID_HANDLE: Provided graph or tensor
 *           handle is invalid
 *         - QAIRT_GRAPH_ERROR_INVALID_ARGUMENT: _tensorHandles_ is NULL
 *         - QAIRT_TENSOR_ERROR_INCOMPATIBLE_TENSOR_UPDATE: Tensor type does
 *           not support updates
 *         - QAIRT_GRAPH_ERROR_GRAPH_NOT_FINALIZED: graph must be finalized
 *           before updating tensors
 *         - QAIRT_GRAPH_ERROR_UNSUPPORTED_FEATURE: some API feature is
 *           not supported yet
 *
 * @note Multiple calls can be made, but updates will not take effect until
 *       QairtGraph_finalize() is called.
 *
 * @note Use corresponding API through QairtGraph_V1_t.
 */
Qairt_Status_t QairtGraph_updateGraphTensors(QairtGraph_Handle_t graphHandle,
                                             const QairtTensor_Handle_t* tensorHandles,
                                             uint64_t numTensors);

/**
 * @brief Synchronously execute a finalized graph with the provided input and
 *        output tensors.
 *
 * @param[in] graphHandle Handle to a finalized graph. Must be created and
 *                        finalized via QairtGraph_create() and
 *                        QairtGraph_finalize().
 * @param[in] inputHandlesHandle Array of input tensor handles to populate graph
 *                               inputs. Each tensor must carry the same ID
 *                               assigned during creation and registration with the backend.
 * @param[in] numInputs Number of input tensor handles in the
 *                      _inputHandlesHandle_ array.
 * @param[out] outputHandlesHandle Array of output tensor handles to be
 *                                 populated with results from graph execution.
 *                                 Each tensor must carry the same ID assigned during creation.
 * @param[in] numOutputs Number of output tensors in the _outputHandlesHandle_
 *                       array.
 * @param[in] profileHandle Optional handle to a profiling object. If provided,
 *                          performance metrics will be collected during
 *                          execution. A handle being reused would reset and is populated with
 * values from the current call. Use NULL to disable profiling. This handle must be NULL if
 * continuous profiling was configured via the QAIRT_GRAPH_CONFIG_OPTION_PROFILE_HANDLE option.
 * @param[in] signalHandle Optional signal object used to control execution. If
 *                         NULL, execution proceeds uninterrupted. If provided,
 *                         the signal handle is considered in-use for the duration of the call.
 * @note Input and output tensors must match the IDs and metadata used during
 *       registration. However, the following exceptions apply:
 *       - Client-provided buffers (e.g., _clientBuf_) may change between
 *       executions.
 *       - Batch dimension may be an integer multiple of the original
 *       dimension, provided all tensors share the same multiple.
 *       - Output dimensions may vary post-execution for dynamic tensors.
 *       - Dynamic input dimensions must be provided if marked as dynamic
 *       during tensor creation.
 *       - Output tensors with dynamic dimensions will be populated by the
 *       backend.
 *       - Data type changes may be permitted for certain ops that support
 *       conversion.
 *       - Execution may proceed without inputs if the graph contains no
 *       application-writable tensors.
 *       - Only application-readable/writeable tensor types are supported.
 *       Optional tensors may be excluded from _inputs_ or _outputs_
 *       depending on backend support.
 *       - Mixing tensor versions (e.g., V1 and V2) may impact
 *       performance.
 *
 * @note Execution is synchronous and blocks until completion. If other
 *       executions are enqueued, this call will wait in queue.
 *       Execution priority is equal to asynchronous calls.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Graph executed successfully.
 *         - QAIRT_GRAPH_ERROR_INVALID_HANDLE: Graph handle is invalid.
 *         - QAIRT_GRAPH_ERROR_GRAPH_NOT_FINALIZED: Graph has not been
 *           finalized.
 *         - QAIRT_GRAPH_ERROR_SUBGRAPH: Execution not permitted on
 *           subgraphs.
 *         - QAIRT_GRAPH_ERROR_INVALID_ARGUMENT: Invalid inputs, outputs,
 *           or profile handle.
 *         - QAIRT_GRAPH_ERROR_INVALID_TENSOR: One or more tensors are
 *           invalid or unrecognized.
 *         - QAIRT_GRAPH_ERROR_UNSUPPORTED_FEATURE: Feature not supported
 *           (e.g., signal, profile, batch multiplier).
 *         - QAIRT_GRAPH_ERROR_SET_PROFILE: Failed to set profile handle.
 *         - QAIRT_GRAPH_ERROR_SIGNAL_IN_USE: Signal handle is already in
 *           use.
 *         - QAIRT_GRAPH_ERROR_ABORTED: Execution aborted due to user
 *           cancellation.
 *         - QAIRT_GRAPH_ERROR_TIMED_OUT: Execution aborted due to
 *           timeout.
 *         - QAIRT_GRAPH_ERROR_DISABLED: Graph was disabled during context
 *           deserialization.
 *         - QAIRT_GRAPH_ERROR_DYNAMIC_TENSOR_SHAPE: Error due to dynamic
 *           tensor shape exceeding limits.
 *         - QAIRT_GRAPH_ERROR_TENSOR_SPARSITY: Error due to tensor
 *           sparsity constraints.
 *         - QAIRT_GRAPH_ERROR_EARLY_TERMINATION: Execution terminated
 *           early due to op behavior.
 *         - QAIRT_GRAPH_ERROR_INVALID_CONTEXT: Context was freed prior to
 *           execution.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: Recoverable
 *           system-level communication error.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: Unrecoverable
 *           system-level communication error.
 *
 * @note Use corresponding API through QairtGraph_V1_t.
 */
Qairt_Status_t QairtGraph_execute(QairtGraph_Handle_t graphHandle,
                                  const QairtTensor_Handle_t* inputHandlesHandle,
                                  const uint32_t numInputs,
                                  QairtTensor_Handle_t* outputHandlesHandle,
                                  const uint32_t numOutputs,
                                  QairtProfile_Handle_t profileHandle,
                                  QairtSignal_Handle_t signalHandle);

/**
 * @brief Asynchronously execute a finalized graph. Graphs will be enqueued for
 *        execution in FIFO order. There is no guarantee that
 *        graphs will finish execution in the same order they were enqueued. If the
 *        execution queue is full, this function will block until space is available.
 *
 * @param[in] graphHandle Handle of finalized graph to execute.
 *
 * @param[in] inputHandlesHandle Array of input tensor handles with which to
 *                               populate graph inputs.
 * @param[in] numInputs Number of input tensor handles in the
 *                      _inputHandlesHandle_ array.
 * @param[out] outputHandlesHandle Array of tensors handles which the graph will
 *                                 populate with output values.
 * @param[in] numOutputs Number of output tensor handles in the
 *                       _outputHandlesHandle_ array.
 * @param[in] profileHandle The profile handle on which metrics is populated and
 *                          can be queried. Use NULL handle to disable profile
 *                          collection. A handle being reused would reset and is populated with
 * values from the enqueued execute call. Profile handle management/reuse across asynchronous calls
 * is client's responsibility. Behavior is undefined if same profile handle is used by two enqueued
 * execute instances at the same time. This handle must be NULL when a continuous profile handle has
 * been configured via the QAIRT_GRAPH_CONFIG_OPTION_PROFILE_HANDLE option
 * @param[in] signalHandle Signal object which may be used to control the
 *                         execution of this call. NULL indicates execution
 *                         should proceed as normal. All pending executions in the queue are
 * affected by Signal control. Instance executing when Signal control is issued may not be affected.
 * The signal object, if not NULL, is considered to be in-use for the duration of the call. For
 * timeout signals, the timeout duration applies from the QairtGraph_executeAsync call until the
 *                         callback is called. The same QairtGraph_Handle_t can be used for
 * multiple calls to QairtGraph_executeAsync, however, different Qairt_SignalHandle_t must be
 * supplied.
 * @param[in] notifyFn Pointer to notification function, called when execution
 *                     is finished. NULL indicates no notification is requested.
 *                     _notifyFn_ will be called in context of backend owned thread, with priority
 *                     equal or lower than client's calling thread. Please note that a failed call
 *                     to QairtGraph_executeAsync does not call the notification function.
 * @param[in] notifyParam Client-supplied data object which will be passed back
 *                        via _notifyFn_ and can be used to identify
 *                        asynchronous execution instance. Can be NULL.
 * @note Tensors in _inputs_ and _outputs_ must carry the same ID that was
 *       assigned when they were created. Values for all other
 *       attributes in QairtTensor are assumed from the point at which they were
 *       registered with a backend at the time of tensor creation, with the following
 *       exceptions:
 *       - Tensor data provided by client such as in _clientBuf_ can be
 *       changed between invocations to execute().
 *       - Batch multiple: An _inputHandlesHandle_ or
 *       _outputHandlesHandle_ tensor _dimensions_ value, if non-null, should match
 *       the values provided at tensor creation, with the following exception. The
 *       batch dimension, as determined by the op definition, can be an integer
 *       multiple of the respective dimension provided at tensor creation. All
 *       _inputHandlesHandle_ and _outputHandlesHandle_ tensors must have the same
 *       batch multiple.
 *       - Dynamic output dimensions: An _outputsHandles_ tensor
 *       QairtTensor _dimensions_ value, if non-null, can vary after graph execution.
 *       As determined by the op definition, non-batch dimensions may be less than the
 *       respective dimension at tensor creation.
 *       - Dynamic dimensions: If an _inputHandlesHandle_ tensor was
 *       created with a non-null _isDynamicDimensions_ value, the corresponding
 *       dynamic dimensions must be provided by the caller. If an
 *       _outputHandlesHandle_ tensor was created with a non-null
 *       _isDynamicDimensions_ value, the _dimensions_ must be non-null and the output
 *       dimensions will be written by the backend. In a scenario where maximum
 *       dimensions will be exceeded, the backend will generate an error code
 *       indicating loss of data and will fill the tensor with as much data as
 *       possible.
 *       - Other Tensor values like _dataType_ can also be permitted to
 *       change between invocations to QairtGraph_execute()/QairtGraph_executeAsync()
 *       for certain ops that perform data type conversions.
 *       - Some backends may be able to execute a graph with no
 *       _inputHandlesHandle_ provided the graph has no
 *       application-writable tensors.
 *       - QairtGraph_executeAsync() can only accept tensors of type
 *       QAIRT_TENSOR_TYPE_APP_READ, QAIRT_TENSOR_TYPE_APP_WRITE,
 *       QAIRT_TENSOR_TYPE_APP_READ_WRITE,
 *       QAIRT_TENSOR_TYPE_OPTIONAL_APP_READ, QAIRT_TENSOR_TYPE_OPTIONAL_APP_WRITE,
 *       and QAIRT_TENSOR_TYPE_OPTIONAL_APP_READWRITE. Tensors provided with a
 *       different type will result in QairtGraph_execute() failure.
 *       - Clients may exclude tensors of type
 *       QAIRT_TENSOR_TYPE_OPTIONAL_APP_READ,
 *       QAIRT_TENSOR_TYPE_OPTIONAL_APP_WRITE, and QAIRT_TENSOR_TYPE_OPTIONAL_APP_READ
 *       from the _inputHandlesHandle_ and _outputHandlesHandle_ arguments. If a
 *       QAIRT_TENSOR_TYPE_OPTIONAL_APP_WRITE tensor is excluded from the
 *       _inputHandlesHandle_ argument, the value of that tensor will be dictated by
 *       the backend defined behavior for that model.
 *       QAIRT_TENSOR_TYPE_OPTIONAL_APP_READ tensors may be excluded
 *       from the _outputHandlesHandle_ argument. In this case a backend will not
 *       populate the tensor on the QairtGraph_execute() call, and the data of these
 *       tensors is null. This is an optional feature. Backends broadcast support for
 *       this feature with QAIRT_CAPABILITY_TENSOR_SUPPORT_OPTIONAL_APP_WRITE,
 *       QAIRT_CAPABILITY_TENSOR_SUPPORT_OPTIONAL_APP_READ, and
 *       QAIRT_CAPABILITY_TENSOR_SUPPORT_OPTIONAL_APP_READWRITE.
 *
 * @note If there are simultaneous calls to QairtGraph_execute() and
 *       QairtGraph_executeAsync(), the priority for enqueuing or executing
 *       is equal. Both functions will add to the same queue, the only difference in
 *       behavior is whether the function returns when the execution is enqueued, or
 *       when the execution finishes.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: the graph was successfully executed
 *         - QAIRT_GRAPH_ERROR_INVALID_HANDLE: _graph_ is not a valid
 *           handle
 *         - QAIRT_GRAPH_ERROR_GRAPH_NOT_FINALIZED: graph was not
 *           finalized
 *         - QAIRT_GRAPH_ERROR_SUBGRAPH: cannot execute a subgraph
 *         - QAIRT_GRAPH_ERROR_INVALID_ARGUMENT:
 *         - _inputHandlesHandle_ or _outputHandlesHandle_ is NULL or
 *           ill-formed OR
 *         - _inputHandlesHandle_ is NOT NULL and _numInputs_ is 0 OR
 *         - _outputHandlesHandle_ is NOT NULL and _numOutputs_ is 0 OR
 *         - _profileHandle_ handle is invalid OR
 *         - continuous graph profiling is enabled and the per-API handle
 *           is not NULL.
 *         - QAIRT_GRAPH_ERROR_INVALID_TENSOR: one or more tensors in
 *           _inputHandlesHandle_ or _outputHandlesHandle_ is invalid or not
 *           recognized by graph
 *         - QAIRT_GRAPH_ERROR_UNSUPPORTED_FEATURE: asynchronous graph
 *           execution is not supported on this backend or some API feature is not
 *           supported yet, e.g. signal, profile, or batch multiplier
 *         - QAIRT_GRAPH_ERROR_SIGNAL_IN_USE: the supplied control signal
 *           is already in-use by another call.
 *         - QAIRT_GRAPH_ERROR_ABORTED: the call is aborted before
 *           completion due to user cancellation
 *         - QAIRT_GRAPH_ERROR_TIMED_OUT: the call is aborted before
 *           completion due to a timeout
 *         - QAIRT_GRAPH_ERROR_DISABLED: the graph was not enabled when
 *           the context was deserialized
 *         - QAIRT_GRAPH_ERROR_DYNAMIC_TENSOR_SHAPE: An error occurred
 *           that is related to dynamic tensor shape. For example, a tensor maximum
 *           dimension was exceeded.
 *         - QAIRT_GRAPH_ERROR_TENSOR_SPARSITY: An error occurred that is
 *           related to tensor sparsity. For example, the maximum number of
 *           specified elements was exceeded.
 *         - QAIRT_GRAPH_ERROR_EARLY_TERMINATION: Graph execution
 *           terminated early due to defined op behavior.
 *         - QAIRT_GRAPH_ERROR_INVALID_CONTEXT: Graph execution failed due
 *           to context already being freed.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurence
 *           (successful recovery)
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurence
 *           (unsuccessful recovery)
 *
 * @note Use corresponding API through QairtGraph_V1_t.
 */
Qairt_Status_t QairtGraph_executeAsync(QairtGraph_Handle_t graphHandle,
                                       const QairtTensor_Handle_t* inputHandlesHandle,
                                       const uint32_t numInputs,
                                       QairtTensor_Handle_t* outputHandlesHandle,
                                       const uint32_t numOutputs,
                                       QairtProfile_Handle_t profileHandle,
                                       QairtSignal_Handle_t signalHandle,
                                       Qairt_NotifyFn_t notifyFn,
                                       void* notifyParam);

typedef Qairt_Status_t (*QairtGraph_FreeFn_t)(QairtGraph_Handle_t);

typedef Qairt_Status_t (*QairtGraph_CreateSubgraphFn_t)(QairtGraph_Handle_t,
                                                        const char*,
                                                        QairtGraph_Handle_t*);

typedef Qairt_Status_t (*QairtGraph_SetConfigFn_t)(QairtGraph_Handle_t,
                                                   const QairtGraph_ConfigHandle_t);

typedef Qairt_Status_t (*QairtGraph_AddNodeFn_t)(QairtGraph_Handle_t, QairtOpConfig_Handle_t);

typedef Qairt_Status_t (*QairtGraph_FinalizeFn_t)(QairtGraph_Handle_t,
                                                  QairtProfile_Handle_t,
                                                  QairtSignal_Handle_t);

typedef Qairt_Status_t (*QairtGraph_CreateGraphTensorFn_t)(QairtGraph_Handle_t,
                                                           QairtTensor_Handle_t);

typedef Qairt_Status_t (*QairtGraph_UpdateGraphTensorsFn_t)(QairtGraph_Handle_t,
                                                            const QairtTensor_Handle_t*,
                                                            uint64_t);

typedef Qairt_Status_t (*QairtGraph_ExecuteFn_t)(QairtGraph_Handle_t,
                                                 const QairtTensor_Handle_t*,
                                                 uint32_t,
                                                 QairtTensor_Handle_t*,
                                                 uint32_t,
                                                 QairtProfile_Handle_t,
                                                 QairtSignal_Handle_t);

typedef Qairt_Status_t (*QairtGraph_ExecuteAsyncFn_t)(QairtGraph_Handle_t,
                                                      const QairtTensor_Handle_t*,
                                                      uint32_t,
                                                      QairtTensor_Handle_t*,
                                                      uint32_t,
                                                      QairtProfile_Handle_t,
                                                      QairtSignal_Handle_t,
                                                      Qairt_NotifyFn_t,
                                                      void*);

//=============================================================================
// Public Interfaces
//=============================================================================
/**
 * @struct QairtGraph_V1_t
 * @brief This struct contains the functions that define the V1 interface for
 *        QAIRT Graph. This interface can be retrieved through the id
 *        QAIRT_GRAPH_V1_ID.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtGraph_FreeFn_t free;
  QairtGraph_CreateSubgraphFn_t createSubgraph;
  QairtGraph_SetConfigFn_t setConfig;
  QairtGraph_AddNodeFn_t addNode;
  QairtGraph_FinalizeFn_t finalize;
  QairtGraph_CreateGraphTensorFn_t createGraphTensor;
  QairtGraph_UpdateGraphTensorsFn_t updateGraphTensors;
  QairtGraph_ExecuteFn_t execute;
  QairtGraph_ExecuteAsyncFn_t executeAsync;
} QairtGraph_V1_t;

#ifdef __cplusplus
}  // extern "C"
#endif
#endif  // QAISW_QAIRTGRAPH_H

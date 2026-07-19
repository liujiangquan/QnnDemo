//==============================================================================
//
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// All rights reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//==============================================================================

/**
 * @file
 * @brief C++ wrapper for the QAIRT graph API.
 */

#pragma once

#include <memory>
#include <optional>

#include "QairtApi.hpp"
#include "QairtGraph/QairtGraph.h"
#include "QairtGraph/QairtGraphConfig.h"
#include "QairtOpConfig.hpp"
#include "QairtProfile.hpp"
#include "QairtSignal.hpp"
#include "QairtTensor.hpp"

namespace qairt {

/**
 * @brief Execution priority levels for graph scheduling.
 *
 * @details
 * | Enumerator    | Description                                              |
 * |---------------|----------------------------------------------------------|
 * | `Low`         | Lowest scheduling priority.                              |
 * | `NormalLow`   | Below-normal scheduling priority.                       |
 * | `Normal`      | Normal scheduling priority.                              |
 * | `Default`     | Default scheduling priority, equivalent to Normal.      |
 * | `NormalHigh`  | Above-normal scheduling priority.                       |
 * | `High`        | High scheduling priority.                                |
 * | `HighPlus`    | Higher than High scheduling priority.                   |
 * | `Critical`    | Critical scheduling priority.                           |
 * | `CriticalPlus`| Highest scheduling priority.                            |
 * | `Undefined`   | Priority is not set or unrecognized.                    |
 */
QAIRT_CPPAPI_DEFINE_ENUM(Priority,
                         Qairt_Priority_t,
                         Low        = QAIRT_PRIORITY_LOW,
                         NormalLow  = QAIRT_PRIORITY_NORMAL_LOW,
                         Normal     = QAIRT_PRIORITY_NORMAL,
                         Default    = QAIRT_PRIORITY_DEFAULT,
                         NormalHigh = QAIRT_PRIORITY_NORMAL_HIGH,
                         High       = QAIRT_PRIORITY_HIGH,
                         HighPlus   = QAIRT_PRIORITY_HIGH_PLUS,
                         Critical   = QAIRT_PRIORITY_CRITICAL,
                         CriticalPlus = QAIRT_PRIORITY_CRITICAL_PLUS,
                         Undefined    = QAIRT_PRIORITY_UNDEFINED)

/**
 * @brief Error codes returned by QAIRT graph operations.
 *
 * @details
 * | Enumerator              | Description                                                          |
 * |-------------------------|----------------------------------------------------------------------|
 * | `MinError`              | Sentinel for the minimum error value.                               |
 * | `NoError`               | Operation succeeded.                                                 |
 * | `UnsupportedFeature`    | An optional API feature is not yet supported.                        |
 * | `MemAlloc`              | Memory allocation failure in graph processing.                       |
 * | `General`               | Unclassified graph error; any graph API may return this.             |
 * | `InvalidArguemnt`       | An argument to the graph API is invalid.                             |
 * | `InvalidHandle`         | The provided graph handle is not valid.                              |
 * | `GraphDoesNotExist`     | No graph with the specified name is registered in the backend.       |
 * | `InvalidName`           | Graph name is NULL, empty, or duplicates an existing name.           |
 * | `InvalidTensor`         | A tensor handle is NULL or invalid.                                  |
 * | `InvalidOpConfig`       | One or more elements of the op configuration are invalid.            |
 * | `SetProfile`            | Failed to bind the profile handle to the graph.                      |
 * | `UnconnectedNode`       | A node was added before one or more of its input-producing nodes.    |
 * | `CreateFailed`          | Graph creation failed.                                               |
 * | `OtimizationFailed`     | Graph optimization failed with the specified ops or configuration.   |
 * | `FinalizeFailed`        | Graph finalization failed.                                           |
 * | `GraphNotFinalized`     | Attempted to execute a graph that has not been finalized.            |
 * | `GraphFinalized`        | Attempted to modify a graph after finalization.                      |
 * | `ExecutionAsyncFifoFull`| Async execution queue is full; no new requests can be registered.    |
 * | `SignalInUse`           | The supplied signal object is already in use by another call.        |
 * | `Aborted`               | Call aborted early due to a signal trigger.                          |
 * | `ProfileInUse`          | The profile handle is already bound to another graph.                |
 * | `TimedOut`              | Call aborted early due to a signal timeout.                          |
 * | `Subgraph`              | Operation is not permitted on a subgraph.                            |
 * | `Disabled`              | The graph was disabled during context deserialization.               |
 * | `DynamicTensorShape`    | Dynamic tensor shape exceeded configured limits.                     |
 * | `TensorSparsity`        | Tensor sparsity constraint violation.                                |
 * | `EarlyTermination`      | Graph execution terminated early due to op-defined behavior.         |
 * | `InvalidContext`        | The context associated with this graph has already been freed.       |
 * | `MaxError`              | Sentinel for the maximum error value.                                |
 * | `Undefined`             | Unused; present to ensure a 32-bit enum size.                        |
 */
QAIRT_CPPAPI_DEFINE_ENUM(
    GraphError,
    QairtGraph_Error_t,
    MinError = QAIRT_GRAPH_MIN_ERROR,
    NoError = QAIRT_GRAPH_NO_ERROR,
    UnsupportedFeature = QAIRT_GRAPH_ERROR_UNSUPPORTED_FEATURE,
    MemAlloc = QAIRT_GRAPH_ERROR_MEM_ALLOC,
    General = QAIRT_GRAPH_ERROR_GENERAL,
    InvalidArguemnt = QAIRT_GRAPH_ERROR_INVALID_ARGUMENT,
    InvalidHandle = QAIRT_GRAPH_ERROR_INVALID_HANDLE,
    GraphDoesNotExist = QAIRT_GRAPH_ERROR_GRAPH_DOES_NOT_EXIST,
    InvalidName = QAIRT_GRAPH_ERROR_INVALID_NAME,
    InvalidTensor = QAIRT_GRAPH_ERROR_INVALID_TENSOR,
    InvalidOpConfig = QAIRT_GRAPH_ERROR_INVALID_OP_CONFIG,
    SetProfile = QAIRT_GRAPH_ERROR_SET_PROFILE,
    UnconnectedNode = QAIRT_GRAPH_ERROR_UNCONNECTED_NODE,
    CreateFailed = QAIRT_GRAPH_ERROR_CREATE_FAILED,
    OtimizationFailed = QAIRT_GRAPH_ERROR_OPTIMIZATION_FAILED,
    FinalizeFailed = QAIRT_GRAPH_ERROR_FINALIZE_FAILED,
    GraphNotFinalized = QAIRT_GRAPH_ERROR_GRAPH_NOT_FINALIZED,
    GraphFinalized = QAIRT_GRAPH_ERROR_GRAPH_FINALIZED,
    ExecutionAsyncFifoFull = QAIRT_GRAPH_ERROR_EXECUTION_ASYNC_FIFO_FULL,
    SignalInUse = QAIRT_GRAPH_ERROR_SIGNAL_IN_USE,
    Aborted = QAIRT_GRAPH_ERROR_ABORTED,
    ProfileInUse = QAIRT_GRAPH_ERROR_PROFILE_IN_USE,
    TimedOut = QAIRT_GRAPH_ERROR_TIMED_OUT,
    Subgraph = QAIRT_GRAPH_ERROR_SUBGRAPH,
    Disabled = QAIRT_GRAPH_ERROR_DISABLED,
    DynamicTensorShape = QAIRT_GRAPH_ERROR_DYNAMIC_TENSOR_SHAPE,
    TensorSparsity = QAIRT_GRAPH_ERROR_TENSOR_SPARSITY,
    EarlyTermination = QAIRT_GRAPH_ERROR_EARLY_TERMINATION,
    InvalidContext = QAIRT_GRAPH_ERROR_INVALID_CONTEXT,
    MaxError = QAIRT_GRAPH_MAX_ERROR,
    Undefined = QAIRT_GRAPH_ERROR_UNDEFINED, )

/**
 * @brief Profiling enabled/disabled state for a graph.
 *
 * @details
 * | Enumerator  | Description                                    |
 * |-------------|------------------------------------------------|
 * | `Enabled`   | Profiling is active for this graph. |
 * | `Disabled`  | Profiling is not active for this graph.        |
 * | `Undefined` | Unused; present to ensure a 32-bit enum size.  |
 */
QAIRT_CPPAPI_DEFINE_ENUM(GraphProfilingState,
                         QairtGraph_ProfilingState_t,
                         Enabled   = QAIRT_GRAPH_PROFILING_STATE_ENABLED,
                         Disabled  = QAIRT_GRAPH_PROFILING_STATE_DISABLED,
                         Undefined = QAIRT_GRAPH_PROFILING_STATE_UNDEFINED, )

/**
 * @brief Abstract base class for a single backend-specific graph custom configuration entry.
 */
class GraphCustomConfig : public CustomConfigType {
 protected:
  GraphCustomConfig()                                        = default;
  GraphCustomConfig(const GraphCustomConfig&)                = default;
  GraphCustomConfig(GraphCustomConfig&&) noexcept            = default;
  GraphCustomConfig& operator=(const GraphCustomConfig&)     = default;
  GraphCustomConfig& operator=(GraphCustomConfig&&) noexcept = default;

 public:
  virtual ~GraphCustomConfig()                                          = default;
  virtual QairtGraph_CustomConfigHandle_t getCustomConfigHandle() const = 0;
};

/**
 * @brief Abstract base class for a collection of backend-specific graph custom configuration entries.
 */
class GraphCustomConfiguration {
 protected:
  GraphCustomConfiguration()                                               = default;
  GraphCustomConfiguration(const GraphCustomConfiguration&)                = default;
  GraphCustomConfiguration(GraphCustomConfiguration&&) noexcept            = default;
  GraphCustomConfiguration& operator=(const GraphCustomConfiguration&)     = default;
  GraphCustomConfiguration& operator=(GraphCustomConfiguration&&) noexcept = default;

 public:
  virtual ~GraphCustomConfiguration()                                           = default;
  virtual std::vector<QairtGraph_CustomConfigHandle_t> getCustomConfigs() const = 0;
};

static_assert(sizeof(QairtGraph_ConfigV1_t::size) == sizeof(uint64_t));
static_assert(detail::has_size_member_v<QairtGraph_ConfigV1_t>);
/**
 * @brief Configuration object for graph creation and execution behavior.
 *
 *        Construct directly — `GraphConfiguration()` — and call setter methods to
 *        configure priority, profiling, and custom options before passing to
 *        Context::createGraph().
 */
class GraphConfiguration : public ApiType<GraphConfiguration, QairtGraph_ConfigV1_t> {
  friend class Api;

  /// @brief Profile handle bound to this graph configuration for continuous profiling.
  detail::crossable<detail::non_owning<Profile>,
                    &interface_type::getProfileHandle,
                    &interface_type::setProfileHandle>
      m_profile;

 public:
  GraphConfiguration() noexcept                                = default;
  GraphConfiguration(GraphConfiguration&&) noexcept            = default;
  GraphConfiguration& operator=(GraphConfiguration&&) noexcept = default;

 private:
  friend class Api;
  GraphConfiguration(const std::shared_ptr<ApiTable>& apiTable, QairtGraph_ConfigHandle_t handle)
      : ApiType(apiTable) {
    setHandle(handle);
    setNonOwning(true);
  }

 public:
  /**
   * @brief Set a single backend-specific custom configuration entry on this graph configuration.
   *
   * @param[in] config  Single custom configuration entry.
   *
   * @return Reference to this configuration object, enabling method chaining.
   *
   * @throws qairt::Exception on invalid handle or invalid argument.
   *
   * @see QairtGraph_Config_setCustomConfigs
   */
  GraphConfiguration& setCustomConfig(const GraphCustomConfig& config) {
    auto cch = config.getCustomConfigHandle();
    callAndConfirm<&interface_type::setCustomConfigs>(&cch, 1u);
    return *this;
  }

  /**
   * @brief Set multiple backend-specific custom configuration entries on this graph configuration.
   *
   * @param[in] config  Collection of custom configuration entries.
   *
   * @throws qairt::Exception on invalid handle or invalid argument.
   *
   * @see QairtGraph_Config_setCustomConfigs
   */
  void setCustomConfigs(const GraphCustomConfiguration& config) {
    auto cfgHandles = config.getCustomConfigs();
    callAndConfirm<&interface_type::setCustomConfigs>(cfgHandles.data(),
                                                      static_cast<uint32_t>(cfgHandles.size()));
  }

  /**
   * @brief Get the scheduling priority for this graph configuration.
   *
   * @return The current priority level.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtGraph_Config_getPriority
   */
  Priority getPriority() const {
    return static_cast<Priority>(callGetter<&interface_type::getPriority>());
  }

  /**
   * @brief Set the scheduling priority for this graph configuration.
   *
   * @param[in] priority  Desired scheduling priority level.
   *
   * @throws qairt::Exception on invalid handle or invalid argument.
   *
   * @see QairtGraph_Config_setPriority
   */
  void setPriority(Priority priority) {
    callSetter<&interface_type::setPriority>(static_cast<Qairt_Priority_t>(priority));
  }

  /**
   * @brief Get the profile handle bound to this graph configuration.
   *
   * @return Reference to the bound Profile object.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtGraph_Config_getProfileHandle
   */
  Profile& getProfile() { return m_profile.bindNonOwning(*this); }

  /**
   * @brief Get the profile handle bound to this graph configuration.
   *
   * @return Const reference to the bound Profile object.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtGraph_Config_getProfileHandle
   */
  const Profile& getProfile() const { return m_profile.bindNonOwning(*this); }

  /**
   * @brief Set the profile handle on this graph configuration.
   *
   * @param[in] profile  Profile object to bind.
   *
   * @throws qairt::Exception on invalid handle or if the profile is already in use.
   *
   * @see QairtGraph_Config_setProfileHandle
   */
  void setProfile(const Profile& profile) {
    QAIRT_NO_RETURN m_profile.setNonOwning(*this, profile);
  }

  /**
   * @brief Get the profiling state for this graph configuration.
   *
   * @return The current profiling state.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtGraph_Config_getProfilingState
   */
  GraphProfilingState getGraphProfilingState() const {
    return static_cast<GraphProfilingState>(callGetter<&interface_type::getProfilingState>());
  }

  /**
   * @brief Set the profiling state for this graph configuration.
   *
   * @param[in] graphProfilingState  Desired profiling state.
   *
   * @throws qairt::Exception on invalid handle or invalid argument.
   *
   * @see QairtGraph_Config_setProfilingState
   */
  void setGraphProfilingState(GraphProfilingState graphProfilingState) {
    callSetter<&interface_type::setProfilingState>(
        static_cast<QairtGraph_ProfilingState_t>(graphProfilingState));
  }

  /**
   * @brief Get the number of profiling executions configured for this graph.
   *
   * @return Number of executions to profile.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtGraph_Config_getNumProfilingExecutions
   */
  uint32_t getNumProfilingExecutions() const {
    return callGetter<&interface_type::getNumProfilingExecutions>();
  }

  /**
   * @brief Set the number of executions to profile for this graph configuration.
   *
   * @param[in] numProfilingExecutions  Number of executions to profile.
   *
   * @throws qairt::Exception on invalid handle or invalid argument.
   *
   * @see QairtGraph_Config_setNumProfilingExecutions
   */
  void setNumProfilingExecutions(uint32_t numProfilingExecutions) {
    callSetter<&interface_type::setNumProfilingExecutions>(numProfilingExecutions);
  }

 private:
  void prepareToCross() const { prepareMultipleToCross(m_profile); }
  void updateAfterCross() const { updateMultipleAfterCross(m_profile); }

  explicit GraphConfiguration(const std::shared_ptr<ApiTable>& apiTable) : ApiType(apiTable) {
    construct();
  }
};

enum class TensorSetMemType {};

class TensorSet {
 public:
  TensorSet()                                = default;
  TensorSet(TensorSet&&) noexcept            = default;
  TensorSet(const TensorSet&)                = delete;
  TensorSet& operator=(TensorSet&&) noexcept = default;
  TensorSet& operator=(const TensorSet&)     = delete;

  std::vector<std::shared_ptr<Tensor>>& getInputs() { throw QAIRT_CPPAPI_STUB_EXCEPTION; }
  const std::vector<std::shared_ptr<Tensor>>& getInputs() const {
    throw QAIRT_CPPAPI_STUB_EXCEPTION;
  }
  void setInputs(std::vector<std::shared_ptr<Tensor>> inputs) { (void)inputs; throw QAIRT_CPPAPI_STUB_EXCEPTION; }

  TensorSetMemType getMemType() { throw QAIRT_CPPAPI_STUB_EXCEPTION; }
  TensorSetMemType getMemType() const { throw QAIRT_CPPAPI_STUB_EXCEPTION; }
  void setMemType(TensorSetMemType memType) { (void)memType; throw QAIRT_CPPAPI_STUB_EXCEPTION; }

  std::vector<std::shared_ptr<Tensor>>& getOutputs() { throw QAIRT_CPPAPI_STUB_EXCEPTION; }
  const std::vector<std::shared_ptr<Tensor>>& getOutputs() const {
    throw QAIRT_CPPAPI_STUB_EXCEPTION;
  }
  void setOutputs(std::vector<std::shared_ptr<Tensor>> outputs) {
    (void)outputs;
    throw QAIRT_CPPAPI_STUB_EXCEPTION;
  }
};

struct NotifyStatus {
  QairtGraph_Error_t error;
};

class Context;

/**
 * @brief Wrapper for a QAIRT graph handle.
 *
 *        Obtained via Context::createGraph() or Context::retrieveGraph().
 */
class Graph : public ApiType<Graph, QairtGraph_V1_t> {
  friend class Api;

  template <typename, typename, typename>
  friend class ::qairt::ApiType;

  friend Context;

  // Handle wrapper to avoid misinterpreting handle type for subgraph constructor
  struct ParentGraphHandle {
    QairtGraph_Handle_t m_parentHandle;
  };

  struct GraphRetrieveContext {
    QairtContext_Handle_t m_contextHandle;
  };

  void customFree(handle_type handle) {
    if (!m_isRetreived) {
      QAIRT_NO_RETURN callNoHandle<&interface_type::free>(handle);
    }
  }

  /// @brief True if this graph was retrieved from an existing context rather than created.
  bool m_isRetreived = false;

 public:
  ~Graph() = default;  // You cannot delete a graph in isolation

  Graph(const Graph&)                = delete;
  Graph(Graph&&) noexcept            = default;
  Graph& operator=(const Graph&)     = delete;
  Graph& operator=(Graph&&) noexcept = default;

  /**
   * @brief Create a tensor registered with this graph.
   *
   * @param[in,out] tensor  Pre-configured tensor to register. The backend assigns
   *                        a tensor ID directly to this handle as part of this call.
   *
   * @throws qairt::Exception on:
   *         - invalid graph or tensor handle
   *         - invalid or unsupported tensor parameters
   *         - memory allocation failure
   *
   * @see QairtGraph_createGraphTensor
   */
  void createGraphTensor(Tensor& tensor) {
    detail::QairtTypeAdmin::prepareToCross(tensor);
    callAndConfirm<&interface_type::createGraphTensor>(getHandle(tensor));
    detail::QairtTypeAdmin::updateAfterCross(tensor);
  }

  /**
   * @brief Update previously created graph tensors with new data or quantization parameters.
   *
   * Valid fields to update depend on tensor type:
   * - UPDATEABLE_STATIC tensors: data and quantization parameters.
   * - UPDATEABLE_NATIVE, UPDATEABLE_APP_READ, UPDATEABLE_APP_WRITE,
   *   UPDATEABLE_APP_READWRITE tensors: quantization parameters only.
   *
   * @param[in] tensors  Array of pointers to tensors to update. Each tensor must
   *                     carry the ID assigned during creation. Must not be empty.
   *
   * @throws qairt::Exception on:
   *         - invalid graph or tensor handle
   *         - incompatible tensor update
   *         - graph not finalized
   *
   * @see QairtGraph_updateGraphTensors
   */
  void updateGraphTensors(const std::vector<Tensor*>& tensors) {
    std::vector<QairtTensor_Handle_t> tensorHandles(tensors.size());
    for (size_t i = 0; i < tensors.size(); ++i) {
      detail::QairtTypeAdmin::prepareToCross(*tensors[i]);
      tensorHandles[i] = getHandle(*tensors[i]);
    }
    callAndConfirm<&interface_type::updateGraphTensors>(
        tensorHandles.data(), static_cast<uint64_t>(tensorHandles.size()));
  }

  /**
   * @brief Add an operation node to this graph.
   *
   * Nodes must be added in dependency order: all native input tensors to the
   * node must be outputs of a previously added node.
   *
   * @param[in] opConfig  Operation configuration describing the node to add.
   *                      All tensors referenced must have been created via
   *                      createGraphTensor().
   *
   * @throws qairt::Exception on:
   *         - invalid graph handle
   *         - invalid op configuration or tensor reference
   *         - graph already finalized
   *         - node added out of dependency order
   *
   * @see QairtGraph_addNode
   */
  void addNode(const OpConfig& opConfig) {
    detail::QairtTypeAdmin::prepareToCross(opConfig);
    callAndConfirm<&interface_type::addNode>(getHandle(opConfig));
    detail::QairtTypeAdmin::updateAfterCross(opConfig);
  }

  /**
   * @brief Create a named subgraph as a child of this graph.
   *
   * A subgraph cannot be finalized or executed directly. Only a top-level
   * graph with no parent can be finalized and executed. Nodes and tensors
   * may be added to a subgraph before or after it is referenced in an op
   * configuration.
   *
   * @param[in] graphName  Unique name for the subgraph within the parent context.
   *                       Must not be NULL or duplicate an existing graph name.
   *
   * @return A new Graph object representing the created subgraph.
   *
   * @throws qairt::Exception on:
   *         - invalid or duplicate graph name
   *         - invalid parent graph handle
   *         - memory allocation failure
   *
   * @see QairtGraph_createSubgraph
   */
  Graph createSubgraph(const std::string& graphName) {
    return create<Graph>(getHandle(), graphName.c_str());
  }

  /**
   * @brief Apply a configuration to this graph.
   *
   * Modifies configuration options on an already-created graph. Must be called
   * before finalize(). If the backend cannot support all provided configuration
   * options, this call will fail.
   *
   * @param[in] config  Configuration object specifying priority, profiling, and
   *                    custom options to apply.
   *
   * @throws qairt::Exception on:
   *         - invalid graph or configuration handle
   *         - unsupported configuration option
   *         - graph already finalized
   *         - profile handle already in use by another graph
   *
   * @see QairtGraph_setConfig
   */
  void setConfig(const GraphConfiguration& config) {
    detail::QairtTypeAdmin::prepareToCross(config);
    callAndConfirm<&interface_type::setConfig>(getHandle(config));
    detail::QairtTypeAdmin::updateAfterCross(config);
  }

  /**
   * @brief Finalize this graph for execution without a profiling handle.
   *
   * Validates all operations, checks connectivity, and prepares the graph for
   * execution. Some backends also require finalization of graphs retrieved from
   * a context binary before execution.
   *
   * @throws qairt::Exception on:
   *         - invalid graph handle
   *         - op or kernel creation failure
   *         - graph optimization failure
   *         - subgraph finalization attempt
   *         - graph has zero nodes
   *
   * @see QairtGraph_finalize
   */
  void finalize() { callAndConfirm<&interface_type::finalize>(nullptr, nullptr); }

  /// @see Graph::finalize()
  void finalize(Profile& profile) {
    callAndConfirm<&interface_type::finalize>(getHandle(profile), nullptr);
  }

  class IOTensorSet {
    friend Graph;

   public:
    IOTensorSet(std::vector<Tensor> inputs, std::vector<Tensor> outputs) {
      (void)inputs;
      (void)outputs;
      throw QAIRT_CPPAPI_STUB_EXCEPTION;
    }

    std::vector<Tensor>& getInputs() { throw QAIRT_CPPAPI_STUB_EXCEPTION; }
    const std::vector<Tensor>& getInputs() const { throw QAIRT_CPPAPI_STUB_EXCEPTION; }
    std::vector<Tensor>& getOutputs() { throw QAIRT_CPPAPI_STUB_EXCEPTION; }
    const std::vector<Tensor>& getOutputs() const { throw QAIRT_CPPAPI_STUB_EXCEPTION; }
  };

  void execute(IOTensorSet& ioTensors) { (void)ioTensors; throw QAIRT_CPPAPI_STUB_EXCEPTION; }

  /**
   * @brief Execute this finalized graph synchronously with the given input and output tensors.
   *
   * Blocks until execution completes. If other executions are already enqueued,
   * this call waits in the same queue with equal priority to asynchronous calls.
   *
   * @param[in]  inputs   Input tensors. Each must carry the ID assigned during
   *                      createGraphTensor(). May be empty only if the graph has
   *                      no application-writable tensors.
   * @param[out] outputs  Output tensors to be populated by the backend. Each must
   *                      carry the ID assigned during createGraphTensor().
   * @param[in]  profile  Optional profile object for collecting execution metrics.
   *                      Must be null if continuous profiling is configured via
   *                      GraphConfiguration::setProfile().
   * @param[in]  signal   Optional signal for aborting or timing out execution.
   *
   * @throws qairt::Exception on:
   *         - invalid graph handle
   *         - graph not finalized
   *         - subgraph execution attempted
   *         - invalid or null tensors
   *         - invalid or in-use signal
   *         - set profile failed
   *         - graph disabled during context deserialization
   *         - dynamic tensor shape limit exceeded
   *         - tensor sparsity constraint violated
   *         - execution terminated early
   *         - execution aborted or timed out
   *         - context freed prior to execution
   *
   * @see QairtGraph_execute
   */
  void execute(const std::vector<Tensor>& inputs,
               std::vector<Tensor>& outputs,
               std::shared_ptr<Profile> profile = nullptr,
               std::shared_ptr<Signal> signal   = nullptr) {
    std::vector<QairtTensor_Handle_t> inputHandles(inputs.size());
    std::vector<QairtTensor_Handle_t> outputHandles(outputs.size());
    QAIRT_NO_RETURN std::transform(
        inputs.begin(), inputs.end(), inputHandles.begin(), [](const Tensor& tensor) {
          detail::QairtTypeAdmin::prepareToCross(tensor);
          return getHandle(tensor);
        });
    QAIRT_NO_RETURN std::transform(
        outputs.begin(), outputs.end(), outputHandles.begin(), [](const Tensor& tensor) {
          detail::QairtTypeAdmin::prepareToCross(tensor);
          return getHandle(tensor);
        });

    QairtProfile_Handle_t profileHandle = profile ? getHandle(*profile) : nullptr;
    (void)signal;
    QairtSignal_Handle_t signalHandle = nullptr;

    callAndConfirm<&interface_type::execute>(inputHandles.data(),
                                             static_cast<uint64_t>(inputHandles.size()),
                                             outputHandles.data(),
                                             static_cast<uint64_t>(outputHandles.size()),
                                             profileHandle,
                                             signalHandle);
  }

  /// @see Graph::execute(const std::vector<Tensor>&, std::vector<Tensor>&, std::shared_ptr<Profile>, std::shared_ptr<Signal>)
  void execute(const std::vector<std::shared_ptr<Tensor>>& inputs,
               std::vector<std::shared_ptr<Tensor>>& outputs) {
    std::vector<QairtTensor_Handle_t> inputHandles(inputs.size());
    std::vector<QairtTensor_Handle_t> outputHandles(outputs.size());
    QAIRT_NO_RETURN std::transform(inputs.begin(), inputs.end(), inputHandles.begin(), [](auto& e) {
      if (e) {
        detail::QairtTypeAdmin::prepareToCross(*e);
        return detail::QairtTypeAdmin::getHandle(*e);
      } else {
        return decltype(detail::QairtTypeAdmin::getHandle(*e)){};
      }
    });
    QAIRT_NO_RETURN std::transform(
        outputs.begin(), outputs.end(), outputHandles.begin(), [](auto& e) {
          if (e) {
            detail::QairtTypeAdmin::prepareToCross(*e);
            return detail::QairtTypeAdmin::getHandle(*e);
          } else {
            return decltype(detail::QairtTypeAdmin::getHandle(*e)){};
          }
        });

    callAndConfirm<&interface_type::execute>(inputHandles.data(),
                                             static_cast<uint64_t>(inputHandles.size()),
                                             outputHandles.data(),
                                             static_cast<uint64_t>(outputHandles.size()),
                                             nullptr,
                                             nullptr);
  }

 private:
  inline static void asyncCallbackTrampoline(void* trampolineObject, Qairt_Status_t status) {
    (void)trampolineObject;
    (void)status;
    throw QAIRT_CPPAPI_STUB_EXCEPTION;
  }

 public:
#if defined(__clang__) //Checks if the compiler is Clang
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#endif
  void executeAsync(const std::vector<std::shared_ptr<Tensor>>& inputs,
                    std::vector<std::shared_ptr<Tensor>>& outputs,
                    ApiTypeRef<const Profile&> profile,
                    ApiTypeRef<const Signal&> signal,
                    std::function<void(void*, NotifyStatus)> fn,
                    void* notifyParam) {
    throw QAIRT_CPPAPI_STUB_EXCEPTION;
  }

  void executeAsync(const std::vector<std::shared_ptr<Tensor>>& inputs,
                    std::vector<std::shared_ptr<Tensor>>& outputs,
                    std::function<void(void*, NotifyStatus)> fn,
                    void* notifyParam) {
    throw QAIRT_CPPAPI_STUB_EXCEPTION;
  }

  void executeAsync(const std::vector<std::shared_ptr<Tensor>>& inputs,
                    std::vector<std::shared_ptr<Tensor>>& outputs,
                    std::function<void(NotifyStatus)> fn) {
    throw QAIRT_CPPAPI_STUB_EXCEPTION;
  }

  void executeAsync(const std::vector<std::shared_ptr<Tensor>>& inputs,
                    std::vector<std::shared_ptr<Tensor>>& outputs) {
    throw QAIRT_CPPAPI_STUB_EXCEPTION;
  }
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
 private:
  Graph(const std::shared_ptr<ApiTable>& apiTable,
        QairtContext_Handle_t contextHandle,
        const char* name,
        ApiTypeRef<const GraphConfiguration&> graphConfig)
      : ApiType(apiTable) {
    construct<&QairtContext_V1_t::createGraph>(contextHandle, name, detail::getHandle(graphConfig));
  }

  Graph(const std::shared_ptr<ApiTable>& apiTable,
        QairtContext_Handle_t contextHandle,
        const char* name)
      : ApiType(apiTable), m_isRetreived{true} {
    construct<&QairtContext_V1_t::retrieveGraph>(contextHandle, name);
  }

  Graph(const std::shared_ptr<ApiTable>& apiTable,
        QairtGraph_Handle_t parentHandle,
        const char* subgraphName)
      : ApiType(apiTable), m_isRetreived{false} {
    construct<&interface_type::createSubgraph>(parentHandle, subgraphName);
  }
};

}  // namespace qairt

//=============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//=============================================================================

/**
 * @file
 * @brief C++ wrapper for the QAIRT context API.
 *
 *        A Backend must be created before constructing a Context object.
 */

#pragma once

#include <unordered_map>

#include "QairtApi.hpp"
#include "QairtContext/QairtContext.h"
#include "QairtContext/QairtContextConfig.h"
#include "QairtDevice.hpp"
#include "QairtGraph.hpp"
#include "QairtProfile.hpp"

namespace qairt {

/**
 * @brief Binary compatibility policy used when loading a cached context binary.
 *
 * @details
 * | Enumerator   | Description                                                              |
 * |--------------|--------------------------------------------------------------------------|
 * | `Permissive` | Binary is accepted if it can run on the device. Default policy.          |
 * | `Strict`     | Binary is accepted only if it fully utilizes hardware capability.        |
 * | `Undefined`  | Sentinel value; not a valid policy selection.                            |
 */
QAIRT_CPPAPI_DEFINE_ENUM(ContextBinaryCompatibilityType,
                         QairtContext_BinaryCompatibilityType_t,
                         Permissive = QAIRT_CONTEXT_BINARY_COMPATIBILITY_PERMISSIVE,
                         Strict     = QAIRT_CONTEXT_BINARY_COMPATIBILITY_STRICT,
                         Undefined  = QAIRT_CONTEXT_BINARY_COMPATIBILITY_TYPE_UNDEFINED)

/**
 * @brief Error codes returned by QAIRT context operations.
 *
 * @details
 * | Enumerator              | Description                                                        |
 * |-------------------------|--------------------------------------------------------------------|
 * | `NoError`               | Operation succeeded.                                               |
 * | `UnsupportedFeature`    | An optional API feature is not supported by the backend.           |
 * | `MemAlloc`              | Memory allocation or deallocation failure.                         |
 * | `InvalidArgument`       | An argument to the operation was invalid.                          |
 * | `CtxDoesNotExist`       | The context has not yet been created in the backend.               |
 * | `InvalidHandle`         | The provided handle is not valid.                                  |
 * | `NotFinalized`          | Operation attempted before all graphs in the context were finalized.|
 * | `BinaryVersion`         | The context binary has an incompatible version.                    |
 * | `CreateFromBinary`      | Failed to create a context from a binary.                          |
 * | `GetBinarySizeFailed`   | Failed to retrieve the size of the serialized context.             |
 * | `GetBinaryFailed`       | Failed to generate the serialized context.                         |
 * | `BinaryConfiguration`   | The context binary configuration is invalid.                       |
 * | `SetProfile`            | Failed to set profiling information.                               |
 * | `InvalidConfig`         | One or more configuration values are invalid.                      |
 * | `BinarySuboptimal`      | A suboptimal binary was used with strict compatibility mode.       |
 * | `Aborted`               | Call was aborted early due to a signal trigger.                    |
 * | `TimedOut`              | Call was aborted early due to a signal timeout.                    |
 * | `IncrementInvalidBuffer`| The incremental binary buffer was not allocated by the backend.    |
 * | `Undefined`             | An undefined or unknown error occurred.                            |
 */
QAIRT_CPPAPI_DEFINE_ENUM(ContextError,
                         QairtContext_Error_t,
                         NoError                = QAIRT_CONTEXT_NO_ERROR,
                         UnsupportedFeature     = QAIRT_CONTEXT_ERROR_UNSUPPORTED_FEATURE,
                         MemAlloc               = QAIRT_CONTEXT_ERROR_MEM_ALLOC,
                         InvalidArgument        = QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT,
                         CtxDoesNotExist        = QAIRT_CONTEXT_ERROR_CTX_DOES_NOT_EXIST,
                         InvalidHandle          = QAIRT_CONTEXT_ERROR_INVALID_HANDLE,
                         NotFinalized           = QAIRT_CONTEXT_ERROR_NOT_FINALIZED,
                         BinaryVersion          = QAIRT_CONTEXT_ERROR_BINARY_VERSION,
                         CreateFromBinary       = QAIRT_CONTEXT_ERROR_CREATE_FROM_BINARY,
                         GetBinarySizeFailed    = QAIRT_CONTEXT_ERROR_GET_BINARY_SIZE_FAILED,
                         GetBinaryFailed        = QAIRT_CONTEXT_ERROR_GET_BINARY_FAILED,
                         BinaryConfiguration    = QAIRT_CONTEXT_ERROR_BINARY_CONFIGURATION,
                         SetProfile             = QAIRT_CONTEXT_ERROR_SET_PROFILE,
                         InvalidConfig          = QAIRT_CONTEXT_ERROR_INVALID_CONFIG,
                         BinarySuboptimal       = QAIRT_CONTEXT_ERROR_BINARY_SUBOPTIMAL,
                         Aborted                = QAIRT_CONTEXT_ERROR_ABORTED,
                         TimedOut               = QAIRT_CONTEXT_ERROR_TIMED_OUT,
                         IncrementInvalidBuffer = QAIRT_CONTEXT_ERROR_INCREMENT_INVALID_BUFFER,
                         Undefined              = QAIRT_CONTEXT_ERROR_UNDEFINED)

/**
 * @brief Storage format of a context binary.
 *
 * @details
 * | Enumerator   | Description                                              |
 * |--------------|----------------------------------------------------------|
 * | `Raw`        | Binary stored as a raw memory buffer.                    |
 * | `MemHandle`  | Binary referenced via a memory handle.                   |
 * | `Undefined`  | Sentinel value; not a valid binary type.                 |
 */
QAIRT_CPPAPI_DEFINE_ENUM(ContextBinaryType,
                         QairtContext_BinaryType_t,
                         Raw       = QAIRT_CONTEXT_BINARY_TYPE_RAW,
                         MemHandle = QAIRT_CONTEXT_BINARY_TYPE_MEM_HANDLE,
                         Undefined = QAIRT_CONTEXT_BINARY_TYPE_UNDEFINED)

/**
 * @brief Portion of the context binary targeted by section operations.
 *
 * @details
 * | Enumerator            | Description                                                          |
 * |-----------------------|----------------------------------------------------------------------|
 * | `Updatable`           | Section containing all recent updates applied via tensor update APIs.|
 * | `UpdatableWeights`    | Section containing recent static weight updates only.                |
 * | `UpdatableQuantParams`| Section containing recent quantization parameter updates only.       |
 * | `Undefined`           | Sentinel value; not a valid section type.                            |
 */
QAIRT_CPPAPI_DEFINE_ENUM(ContextSectionType,
                         QairtContext_SectionType_t,
                         Updatable            = QAIRT_CONTEXT_SECTION_UPDATABLE,
                         UpdatableWeights     = QAIRT_CONTEXT_SECTION_UPDATABLE_WEIGHTS,
                         UpdatableQuantParams = QAIRT_CONTEXT_SECTION_UPDATABLE_QUANT_PARAMS,
                         Undefined            = QAIRT_CONTEXT_SECTION_UNDEFINED)

/**
 * @brief Buffer descriptor for a serialized context binary.
 *
 *        Obtained via `Api::make<ContextBinaryBuffer>()`.
 */
class ContextBinaryBuffer : public ApiType<ContextBinaryBuffer, QairtContext_BinaryBufferV1_t> {
  friend Api;
  template <typename, typename, typename>
  friend class ::qairt::ApiType;

 public:
  ContextBinaryBuffer() noexcept                                 = default;
  ContextBinaryBuffer(ContextBinaryBuffer&&) noexcept            = default;
  ContextBinaryBuffer& operator=(ContextBinaryBuffer&&) noexcept = default;

  /**
   * @brief Get the raw data pointer stored in this buffer.
   *
   * @return Pointer to the underlying buffer memory, or nullptr if none has been set.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtContext_BinaryBuffer_getData
   */
  void* getData() { return callGetter<&interface_type::getData>(); }

  /**
   * @brief Get the raw data pointer stored in this buffer.
   *
   * @return Pointer to the underlying buffer memory, or nullptr if none has been set.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtContext_BinaryBuffer_getData
   */
  const void* getData() const { return callGetter<&interface_type::getData>(); }

  /**
   * @brief Set the raw data pointer for this buffer.
   *
   * @param[in] data  Pointer to the memory region to associate with this buffer.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtContext_BinaryBuffer_setData
   */
  void setData(void* data) { callSetter<&interface_type::setData>(data); }

  /**
   * @brief Get the size of this buffer in bytes.
   *
   * @return Buffer size in bytes, or 0 if no size has been set.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtContext_BinaryBuffer_getSize
   */
  uint64_t getSize() const { return callGetter<&interface_type::getSize>(); }

  /**
   * @brief Set the size of this buffer in bytes.
   *
   * @param[in] size  Size in bytes of the memory region referenced by this buffer.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtContext_BinaryBuffer_setSize
   */
  void setSize(uint64_t size) const { callSetter<&interface_type::setSize>(size); }

 private:
  explicit ContextBinaryBuffer(const std::shared_ptr<ApiTable>& apiTable) : ApiType(apiTable) {
    construct();
  }
};

/**
 * @brief Queue depth configuration for asynchronous context execution.
 */
class ContextAsyncExecutionQueueDepth
    : public ApiType<ContextAsyncExecutionQueueDepth, QairtContext_AsyncExecutionDepthV1_t> {
  friend class Api;

 public:
  ContextAsyncExecutionQueueDepth() noexcept                                             = default;
  ContextAsyncExecutionQueueDepth(ContextAsyncExecutionQueueDepth&&) noexcept            = default;
  ContextAsyncExecutionQueueDepth& operator=(ContextAsyncExecutionQueueDepth&&) noexcept = default;

  /**
   * @brief Get the current queue depth for asynchronous execution.
   *
   * @return Maximum number of outstanding asynchronous execution requests.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtContext_AsyncExecutionGetDepth
   */
  uint32_t getDepth() const { return callGetter<&interface_type::getAsyncExecutionDepth>(); }

  /**
   * @brief Set the queue depth for asynchronous execution.
   *
   * @param[in] depth  Maximum number of outstanding asynchronous execution requests.
   *
   * @throws qairt::Exception on invalid handle or invalid argument.
   *
   * @see QairtContext_AsyncExecutionSetDepth
   */
  void setDepth(uint32_t depth) {
    return callSetter<&interface_type::setAsyncExecutionDepth>(depth);
  }

 private:
  explicit ContextAsyncExecutionQueueDepth(const std::shared_ptr<ApiTable>& apiTable)
      : ApiType(apiTable) {
    construct();
  }
};

class ContextConfiguration;

/**
 * @brief Abstract base class for a single backend-specific context custom configuration entry.
 */
class ContextCustomConfig : public CustomConfigType {
 protected:
  ContextCustomConfig()                                          = default;
  ContextCustomConfig(const ContextCustomConfig&)                = default;
  ContextCustomConfig(ContextCustomConfig&&) noexcept            = default;
  ContextCustomConfig& operator=(const ContextCustomConfig&)     = default;
  ContextCustomConfig& operator=(ContextCustomConfig&&) noexcept = default;

 public:
  virtual ~ContextCustomConfig()                                          = default;
  virtual QairtContext_CustomConfigHandle_t getCustomConfigHandle() const = 0;
};

/**
 * @brief Abstract base class for a collection of backend-specific context custom configuration entries.
 */
class ContextCustomConfiguration {
 protected:
  ContextCustomConfiguration()                                                 = default;
  ContextCustomConfiguration(const ContextCustomConfiguration&)                = default;
  ContextCustomConfiguration(ContextCustomConfiguration&&) noexcept            = default;
  ContextCustomConfiguration& operator=(const ContextCustomConfiguration&)     = default;
  ContextCustomConfiguration& operator=(ContextCustomConfiguration&&) noexcept = default;

 public:
  virtual ~ContextCustomConfiguration()                                           = default;
  virtual std::vector<QairtContext_CustomConfigHandle_t> getCustomConfigs() const = 0;
};

/**
 * @brief Configuration object for context creation and reconfiguration.
 */
class ContextConfiguration : public ApiType<ContextConfiguration, QairtContext_ConfigV1_t> {
  friend Api;

  /// @brief Original Equipment Manufacturer (OEM) key string for backend authentication.
  detail::crossable<std::string, &interface_type::getOemKey, &interface_type::setOemKey> m_oemKey;
  /// @brief Maximum number of outstanding asynchronous execution requests.
  detail::crossable<detail::non_owning<ContextAsyncExecutionQueueDepth>,
                    &interface_type::getAsyncQueueDepth,
                    &interface_type::setAsyncQueueDepth>
      m_depth;
  /// @brief Names of graphs selectively enabled for this context configuration.
  mutable std::vector<std::string> m_enabledGraphs;

 public:
  ContextConfiguration() noexcept                                  = default;
  ContextConfiguration(ContextConfiguration&&) noexcept            = default;
  ContextConfiguration& operator=(ContextConfiguration&&) noexcept = default;

  /**
   * @brief Set the scheduling priority for this context configuration.
   *
   * @param[in] p  Desired execution priority.
   *
   * @throws qairt::Exception on invalid handle or invalid argument.
   *
   * @see QairtContext_Config_setPriority
   */
  void setPriority(Priority p) {
    callSetter<&interface_type::setPriority>(static_cast<Qairt_Priority_t>(p));
  }

  /**
   * @brief Get the scheduling priority for this context configuration.
   *
   * @return Current execution priority.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtContext_Config_getPriority
   */
  Priority getPriority() const {
    return static_cast<Priority>(callGetter<&interface_type::getPriority>());
  }

  /**
   * @brief Get the Original Equipment Manufacturer (OEM) key string for this context configuration.
   *
   * @return Reference to the OEM authentication key string. Empty if not set.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtContext_Config_getOemKey
   */
  std::string& getOemKey() { return m_oemKey.get(); }

  /**
   * @brief Get the Original Equipment Manufacturer (OEM) key string for this context configuration.
   *
   * @return Const reference to the OEM authentication key string. Empty if not set.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtContext_Config_getOemKey
   */
  const std::string& getOemKey() const { return m_oemKey.get(); }

  /**
   * @brief Set the Original Equipment Manufacturer (OEM) key string for this context configuration.
   *
   * @param[in] oemKey  OEM authentication key string to set.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtContext_Config_setOemKey
   */
  void setOemKey(std::string&& oemKey) { m_oemKey.assignString(std::move(oemKey)); }

  void getOemKey(const std::string& oemKey) { m_oemKey.assignString(oemKey); }

  /**
   * @brief Set the asynchronous execution queue depth for this context configuration.
   *
   * @param[in] aed  Async execution queue depth configuration object.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtContext_Config_setAsyncQueueDepth
   */
  void setAsyncExecutionQueueDepth(const ContextAsyncExecutionQueueDepth& aed) {
    QAIRT_NO_RETURN m_depth.setNonOwning(*this, aed);
  }

  /**
   * @brief Get the asynchronous execution queue depth configuration for this context.
   *
   * @return Reference to the async execution queue depth object.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtContext_Config_getAsyncQueueDepth
   */
  ContextAsyncExecutionQueueDepth& getAsyncExecutionQueueDepth() {
    return m_depth.bindNonOwning(*this);
  }

  /**
   * @brief Get the asynchronous execution queue depth configuration for this context.
   *
   * @return Const reference to the async execution queue depth object.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtContext_Config_getAsyncQueueDepth
   */
  const ContextAsyncExecutionQueueDepth& getAsyncExecutionQueueDepth() const {
    return m_depth.bindNonOwning(*this);
  }

  /**
   * @brief Set a single backend-specific custom configuration entry on this context configuration.
   *
   * @param[in] config  Backend-specific custom configuration entry to apply.
   *
   * @return Reference to this configuration object, allowing method chaining.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtContext_Config_setCustomConfigs
   */
  ContextConfiguration& setCustomConfig(const ContextCustomConfig& config) {
    auto cch = config.getCustomConfigHandle();
    callAndConfirm<&interface_type::setCustomConfigs>(&cch, 1u);
    return *this;
  }

  /**
   * @brief Set a collection of backend-specific custom configuration entries on this context configuration.
   *
   * @param[in] config  Collection of backend-specific custom configuration entries to apply.
   *
   * @return Reference to this configuration object, allowing method chaining.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtContext_Config_setCustomConfigs
   */
  ContextConfiguration& setCustomConfigs(const ContextCustomConfiguration& config) {
    std::vector<QairtContext_CustomConfigHandle_t> cfgHandles = config.getCustomConfigs();

    callAndConfirm<&interface_type::setCustomConfigs>(cfgHandles.data(), static_cast<uint32_t>(cfgHandles.size()));
    return *this;
  }

  /**
   * @brief Get the list of graph names selectively enabled for this context configuration.
   *
   * @return Reference to the vector of enabled graph name strings.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtContext_Config_getNumEnabledGraphs
   */
  std::vector<std::string>& getEnableGraphs() { return m_enabledGraphs; }

  /**
   * @brief Get the list of graph names selectively enabled for this context configuration.
   *
   * @return Const reference to the vector of enabled graph name strings.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtContext_Config_getNumEnabledGraphs
   */
  const std::vector<std::string>& getEnableGraphs() const { return m_enabledGraphs; }

  /**
   * @brief Set the list of graph names selectively enabled for this context configuration.
   *
   * @param[in] enabledGraphs  Names of graphs to enable.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtContext_Config_setEnableGraph
   */
  void setEnableGraphs(std::vector<std::string> enabledGraphs) {
    m_enabledGraphs = std::move(enabledGraphs);
  }

  /**
   * @brief Set a hint on the maximum memory the backend should use for this context.
   *
   * This is advisory only; the backend may exceed the limit if required.
   *
   * @param[in] limit  Memory limit hint in bytes.
   *
   * @throws qairt::Exception on invalid handle or invalid argument.
   *
   * @see QairtContext_Config_setMemoryLimitHint
   */
  void setMemoryLimitHint(uint64_t limit) {
    callSetter<&interface_type::setMemoryLimitHint>(limit);
  }

  /**
   * @brief Get the memory limit hint for this context configuration.
   *
   * @return Memory limit hint in bytes, or 0 if no limit has been set.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtContext_Config_getMemoryLimitHint
   */
  uint64_t getMemoryLimitHint() const { return callGetter<&interface_type::getMemoryLimitHint>(); }

  /**
   * @brief Set whether the context binary should be treated as persistent.
   *
   * @param[in] isPersistentBinary  True to enable persistent binary mode; false to disable.
   *
   * @throws qairt::Exception on invalid handle or invalid argument.
   *
   * @see QairtContext_Config_setIsPersistentBinary
   */
  void setIsPersistentBinary(bool isPersistentBinary) {
    callSetter<&interface_type::setIsPersistentBinary>(detail::convertBool(isPersistentBinary));
  }

  /**
   * @brief Get whether the context binary is configured as persistent.
   *
   * @return True if persistent binary mode is enabled; false otherwise.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtContext_Config_getIsPersistentBinary
   */
  bool getIsPersistentBinary() const {
    return detail::resolveBool(callGetter<&interface_type::getIsPersistentBinary>());
  }

  /**
   * @brief Set the binary compatibility policy for loading cached context binaries.
   *
   * @param[in] bct  Binary compatibility policy to enforce.
   *
   * @throws qairt::Exception on invalid handle or invalid argument.
   *
   * @see QairtContext_Config_setBinaryCompatibilityType
   */
  void setBinaryCompatibilityType(ContextBinaryCompatibilityType bct) {
    callSetter<&interface_type::setBinaryCompatibilityType>(
        static_cast<QairtContext_BinaryCompatibilityType_t>(bct));
  }

  /**
   * @brief Get the binary compatibility policy for loading cached context binaries.
   *
   * @return Current binary compatibility policy.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtContext_Config_getBinaryCompatibilityType
   */
  ContextBinaryCompatibilityType getBinaryCompatibilityType() const {
    return static_cast<ContextBinaryCompatibilityType>(
        callGetter<&interface_type::getBinaryCompatibilityType>());
  }

 private:
  void prepareToCross() const {
    prepareMultipleToCross(m_oemKey, m_depth);
    for (const auto& graph : m_enabledGraphs) {
      callSetter<&interface_type::setEnableGraph>(graph.c_str());
    }
  }
  void updateAfterCross() const {
    updateMultipleAfterCross(m_oemKey, m_depth);
    m_enabledGraphs.clear();
    uint32_t numEnabledGraphs;
    callAndConfirm<&interface_type::getNumEnabledGraphs>(&numEnabledGraphs);
    for (uint32_t i = 0u; i < numEnabledGraphs; ++i) {
      const char* graphName;
      callAndConfirm<&interface_type::getEnabledGraphAt>(i, &graphName);
      QAIRT_NO_RETURN m_enabledGraphs.emplace_back(graphName);
    }
  }
  explicit ContextConfiguration(const std::shared_ptr<ApiTable>& apiTable) : ApiType(apiTable) {
    construct();
  }
};

/**
 * @brief Descriptor pairing a binary type with its associated buffer for context serialization.
 */
class ContextBinary : public ApiType<ContextBinary, QairtContext_BinaryV1_t> {
  friend Api;

  /// @brief Binary buffer associated with this context binary object.
  detail::crossable<detail::non_owning<ContextBinaryBuffer>,
                    &interface_type::getBuffer,
                    &interface_type::setBuffer>
      m_buffer;

 public:
  ContextBinary() noexcept                           = default;
  ContextBinary(ContextBinary&&) noexcept            = default;
  ContextBinary& operator=(ContextBinary&&) noexcept = default;

  /**
   * @brief Get the storage format type of this context binary.
   *
   * @return Storage format of this binary (e.g., Raw or MemHandle).
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtContext_binaryGetType
   */
  ContextBinaryType getType() const {
    return static_cast<ContextBinaryType>(callGetter<&interface_type::getType>());
  }

  /**
   * @brief Get the binary buffer associated with this context binary.
   *
   * @return Reference to the associated ContextBinaryBuffer.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtContext_binaryGetBuffer
   */
  ContextBinaryBuffer& getBuffer() { return m_buffer.bindNonOwning(*this); }

  /**
   * @brief Get the binary buffer associated with this context binary.
   *
   * @return Const reference to the associated ContextBinaryBuffer.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtContext_binaryGetBuffer
   */
  const ContextBinaryBuffer& getBuffer() const { return m_buffer.bindNonOwning(*this); }

  /**
   * @brief Set the binary buffer for this context binary.
   *
   * @param[in] buffer  Binary buffer to associate with this context binary object.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtContext_binarySetBuffer
   */
  void setBuffer(ContextBinaryBuffer&& buffer) {
    QAIRT_NO_RETURN m_buffer.setNonOwning(*this, std::move(buffer));
  }
};

/**
 * @brief Wrapper for a QAIRT context handle.
 *
 *        Obtained via Backend::createContext() or Backend::createContextFromBinary().
 */
class Backend;
class Context : public ApiType<Context, QairtContext_V1_t> {
  friend Api;

  template <typename, typename, typename>
  friend class ::qairt::ApiType;

  /// @brief Profile handle used to collect metrics during context teardown.
  QairtProfile_Handle_t m_freeProfileHandle = nullptr;

 public:
  using ApiType::ApiType;
  using ApiType::operator=;

  Context()                              = default;
  Context(const Context&)                = delete;
  Context(Context&&) noexcept            = default;
  Context& operator=(const Context&)     = delete;
  Context& operator=(Context&&) noexcept = default;

  // Non-owning constructor: wraps an existing context handle without taking ownership.
  // The destructor will NOT call free() on the handle.
  Context(const std::shared_ptr<ApiTable>& apiTable, QairtContext_Handle_t handle)
      : ApiType(apiTable) {
    setHandle(handle);
    setNonOwning(true);
  }

  /**
   * @brief Set or modify configuration options on this context.
   *
   * Backends are not required to support reconfiguration after context creation.
   * If the backend does not support the provided configuration, this call will fail.
   *
   * @param[in] config  Context configuration object containing the options to apply.
   *
   * @throws qairt::Exception on invalid handle, unsupported feature, or invalid configuration.
   *
   * @see QairtContext_setConfig
   */
  void setConfig(const ContextConfiguration& config) {
    callSetter<&interface_type::setConfig>(getHandle(config));
  }

  /**
   * @brief Apply a collection of backend-specific custom configuration entries to this context.
   *
   * @param[in] customConfigs  Backend-specific custom configuration collection whose handles
   *                           are applied individually to this context.
   *
   * @throws qairt::Exception on invalid handle.
   */
  template <typename T>
  std::enable_if_t<std::is_base_of_v<ContextCustomConfiguration, T>> setConfiguration(
      const T& customConfigs) {
    for (const auto& e : customConfigs.getCustomConfigs()) {
      setConfig(e);
    }
  }

  /**
   * @brief Get the size in bytes of the serialized binary representation of this context.
   *
   * All graphs in the context must be finalized before calling this method.
   * Call getBinary() or getBinary(void*, uint64_t) after allocating a buffer of
   * at least this size.
   *
   * @return Size in bytes required to hold the serialized context binary.
   *
   * @throws qairt::Exception on:
   *          - invalid handle
   *          - unsupported feature
   *          - unfinalized graphs in the context
   *          - other retrieval failure
   *
   * @see QairtContext_getBinarySize
   */
  uint64_t getBinarySize() const { return callGetter<&interface_type::getBinarySize>(); }

  /**
   * @brief Serialize this context into the provided binary buffer.
   *
   * All graphs in the context must be finalized before calling this method.
   * Call getBinarySize() first to determine the required buffer size.
   * The buffer's data pointer and size must be set before calling this method.
   *
   * @param[in,out] buffer  Pre-allocated binary buffer to receive the serialized context.
   *                        The buffer's size field must be at least getBinarySize() bytes.
   *
   * @return Number of bytes written into the buffer.
   *
   * @throws qairt::Exception on:
   *          - invalid handle
   *          - unsupported feature
   *          - unfinalized graphs in the context
   *          - other serialization failure
   *
   * @see QairtContext_getBinary
   */
  uint64_t getBinary(ContextBinaryBuffer& buffer) {
    uint64_t toRet;
    QAIRT_NO_RETURN call<&interface_type::getBinary>(
        detail::getHandle(ApiTypeRef<const ContextBinaryBuffer&>{buffer}), &toRet);
    return toRet;
  }

  /**
   * @brief Wrapper which allows for serializing directly into a caller-managed raw memory buffer.
   *
   * @see Context::getBinary(ContextBinaryBuffer&)
   */
  // Write into the user allocated buffer. Return the number of bytes written
  uint64_t getBinary(void* buffer, uint64_t bufferSize) {
    ContextBinaryBuffer buff = create<ContextBinaryBuffer>();
    buff.setData(buffer);
    buff.setSize(bufferSize);
    return getBinary(buff);
  }

  /**
   * @brief Update the data and quantization parameters of previously created context tensors.
   *
   * Valid fields to update depend on tensor type:
   * - UPDATEABLE_STATIC: data and quantization parameters.
   * - UPDATEABLE_NATIVE, UPDATEABLE_APP_READ, UPDATEABLE_APP_WRITE,
   *   UPDATEABLE_APP_READWRITE: quantization parameters only.
   *
   * Updates take effect only after QairtGraph_finalize() is called for one or more
   * of the graphs to which the context tensors are associated.
   *
   * @param[in] tensors  Pointers to tensors to update. Each tensor must carry the ID
   *                     assigned during tensor creation.
   *
   * @throws qairt::Exception on:
   *          - invalid context or tensor handle
   *          - NULL tensor handle array
   *          - incompatible tensor type
   *          - unsupported feature
   *
   * @see QairtContext_updateContextTensors
   */
  void updateContextTensors(const std::vector<Tensor*>& tensors) {
    std::vector<QairtTensor_Handle_t> tensorHandles(tensors.size());
    for (size_t i = 0; i < tensors.size(); ++i) {
      detail::QairtTypeAdmin::prepareToCross(*tensors[i]);
      tensorHandles[i] = getHandle(*tensors[i]);
    }
    callAndConfirm<&interface_type::updateContextTensors>(
        tensorHandles.data(), static_cast<uint64_t>(tensorHandles.size()));
  }

  /**
   * @brief Get the size in bytes of a binary section for a specific graph.
   *
   * All graphs in the context must be finalized before calling this method.
   * Use this to determine the buffer size needed before calling getBinarySection().
   *
   * @param[in] graph    Graph whose binary section size is queried.
   * @param[in] section  Portion of the context binary to query.
   *
   * @return Size in bytes needed to hold the requested binary section.
   *
   * @throws qairt::Exception on:
   *          - invalid handle
   *          - unsupported feature
   *          - unfinalized graphs in the context
   *          - other retrieval failure
   *
   * @see QairtContext_getBinarySectionSize
   */
  uint64_t getBinarySectionSize(const Graph& graph, ContextSectionType section) const {
    uint64_t toRet;
    callAndConfirm<&interface_type::getBinarySectionSize>(
        getHandle(graph), static_cast<QairtContext_SectionType_t>(section), &toRet);
    return toRet;
  }

  /**
   * @brief Retrieve a section of the context binary for a specific graph.
   *
   * All graphs in the context must be finalized before calling this method.
   * Call getBinarySectionSize() first to determine the required buffer size.
   * The signal, if non-null, is considered in-use for the duration of this call.
   *
   * @param[in] graph    Graph whose binary section is retrieved.
   * @param[in] section  Portion of the context binary to retrieve.
   * @param[in,out] buffer  Pre-allocated binary buffer to receive the section.
   *                        Must be sized to at least getBinarySectionSize() bytes.
   * @param[in] profile  Optional profile handle to collect metrics.
   * @param[in] signal   Optional signal handle for controlling the operation.
   *
   * @return Number of bytes written into the buffer.
   *
   * @throws qairt::Exception on:
   *          - invalid handle
   *          - unsupported feature
   *          - unfinalized graphs in the context
   *          - other serialization failure
   *
   * @see QairtContext_getBinarySection
   */
  uint64_t getBinarySection(const Graph& graph,
                            ContextSectionType section,
                            ContextBinaryBuffer& buffer,
                            ApiTypeRef<const Profile&> profile,
                            ApiTypeRef<const Signal&> signal) {
    uint64_t toRet;
    QAIRT_NO_RETURN call<&interface_type::getBinarySection>(
        getHandle(graph),
        static_cast<QairtContext_SectionType_t>(section),
        detail::getHandle(ApiTypeRef<const ContextBinaryBuffer&>{buffer}),
        detail::getHandle(profile),
        detail::getHandle(signal),
        &toRet);
    return toRet;
  }

  /**
   * @brief Apply a previously retrieved binary section to this context.
   *
   * @param[in] graph    Graph to which the binary section applies.
   * @param[in] section  Portion of the context binary being applied.
   * @param[in] buffer   Binary buffer containing the section to apply. When persistent
   *                     binary mode is enabled, this buffer must remain valid through
   *                     context teardown.
   * @param[in] profile  Optional profile handle to collect metrics.
   * @param[in] signal   Optional signal handle for controlling the operation.
   *
   * @throws qairt::Exception on:
   *          - invalid handle
   *          - unsupported feature
   *          - memory allocation failure
   *          - profiling error
   *
   * @see QairtContext_applyBinarySection
   */
  void applyBinarySection(const Graph& graph,
                          ContextSectionType section,
                          ContextBinaryBuffer& buffer,
                          ApiTypeRef<const Profile&> profile,
                          ApiTypeRef<const Signal&> signal) {
    callAndConfirm<&interface_type::applyBinarySection>(
        getHandle(graph),
        static_cast<QairtContext_SectionType_t>(section),
        detail::getHandle(ApiTypeRef<const ContextBinaryBuffer&>{buffer}),
        detail::getHandle(profile),
        detail::getHandle(signal));
  }

  /**
   * @brief Create a new graph within this context.
   *
   * @param[in] graphName           Unique null-terminated identifier for the graph within
   *                                this context.
   * @param[in] graphConfiguration  Configuration options for the graph. Optional.
   *
   * @return The newly created Graph object.
   *
   * @throws qairt::Exception on:
   *          - invalid context handle
   *          - NULL or duplicate graph name
   *          - memory or resource allocation failure
   *          - unsupported configuration options
   *
   * @see QairtContext_createGraph
   */
  Graph createGraph(const char* graphName,
                    ApiTypeRef<const qairt::GraphConfiguration&> graphConfiguration) {
    return create<Graph>(getHandle(*this), graphName, graphConfiguration);
  }

  /// @see Context::retrieveGraph(const std::string&)
  std::shared_ptr<Graph> retrieveGraph(const char* graphName) {
    return retrieveGraph(std::string(graphName));
  }

  /**
   * @brief Retrieve an existing graph from this context by name.
   *
   * @param[in] graphName  Name of the graph to retrieve.
   *
   * @return Shared pointer to the retrieved Graph object.
   *
   * @throws qairt::Exception on:
   *          - invalid context handle
   *          - NULL or invalid graph name
   *          - no graph with the specified name exists in this context
   *          - memory allocation failure
   *
   * @see QairtContext_retrieveGraph
   */
  std::shared_ptr<Graph> retrieveGraph(const std::string& graphName) {
    return std::make_shared<Graph>(create<Graph>(getHandle(*this), graphName.c_str()));
  }

  /**
   * @brief Set the profile handle used to collect metrics during context teardown.
   *
   * @param[in] profile  Profile object to populate during context teardown.
   *
   * @see QairtContext_free
   */
  void setFreeProfile(Profile& profile) { m_freeProfileHandle = getHandle(profile); }

 private:
  Context(const std::shared_ptr<ApiTable>& apiTable,
          QairtBackend_Handle_t backendHandle,
          QairtDevice_Handle_t deviceHandle,
          QairtContext_ConfigHandle_t contextConfigHandle)
      : ApiType(apiTable) {
    construct(backendHandle, deviceHandle, contextConfigHandle);
  }

  Context(const std::shared_ptr<ApiTable>& apiTable,
          QairtBackend_Handle_t backendHandle,
          QairtDevice_Handle_t deviceHandle,
          QairtContext_ConfigHandle_t contextConfigHandle,
          QairtContext_BinaryBufferHandle_t binaryBufferHandle,
          QairtSignal_Handle_t signalHandle   = nullptr,
          QairtProfile_Handle_t profileHandle = nullptr)
      : ApiType(apiTable) {
    construct<&interface_type::createFromBinary>(backendHandle,
                                                 deviceHandle,
                                                 contextConfigHandle,
                                                 signalHandle,
                                                 profileHandle,
                                                 binaryBufferHandle);
  }

  void customFree(QairtContext_Handle_t handle) const {
    QAIRT_NO_RETURN callNoHandle<&interface_type::free>(handle, m_freeProfileHandle);
  }
};

}  // namespace qairt

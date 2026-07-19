//=============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//=============================================================================

/**
 * @file
 * @brief C++ wrapper for the QAIRT Backend API.
 */

#pragma once

#include <functional>
#include <optional>
#include <string_view>

#include "QairtApi.hpp"
#include "QairtBackend/QairtBackend.h"
#include "QairtCommon/QairtErrorReportingConfig.h"
#include "QairtContext.hpp"
#include "QairtLog.hpp"
#include "QairtOpConfig.hpp"
#include "QairtSignal.hpp"

namespace qairt {

/**
 * @brief Error codes returned by QAIRT backend operations.
 *
 * @details
 * | Enumerator                   | Description                                                     |
 * |------------------------------|-----------------------------------------------------------------|
 * | `NoError`                    | Operation succeeded.                                            |
 * | `MemAlloc`                   | Memory allocation failure.                                      |
 * | `UnsupportedPlatform`        | Backend creation attempted on an unsupported platform.          |
 * | `CannotInitialize`           | Backend failed to initialize.                                   |
 * | `TerminateFailed`            | Failed to free allocated resources during termination.          |
 * | `NotSupported`               | Requested functionality is not supported by this backend.       |
 * | `InvalidArgument`            | An argument to the operation was invalid.                       |
 * | `OpPackageNotFound`          | The specified op package library could not be found.            |
 * | `OpPackageIfProviderNotFound`| The interface provider symbol was not found in the op package.  |
 * | `OpPackageRegistrationFailed`| Op package registration failed.                                 |
 * | `OpPackageUnsupportedVersion`| The op package interface version is not supported.              |
 * | `OpPackageDuplicate`         | An op with the same package and op name is already registered.  |
 * | `InconsistentConfig`         | Backend configuration is inconsistent across create calls.      |
 * | `InvalidHandle`              | The provided backend handle is not valid.                       |
 * | `InvalidConfig`              | One or more configuration values are invalid.                   |
 * | `Undefined`                  | An undefined or unknown error occurred.                         |
 */
QAIRT_CPPAPI_DEFINE_ENUM(
    BackendError,
    QairtBackend_Error_t,
    NoError                     = QAIRT_BACKEND_NO_ERROR,
    MemAlloc                    = QAIRT_BACKEND_ERROR_MEM_ALLOC,
    UnsupportedPlatform         = QAIRT_BACKEND_ERROR_UNSUPPORTED_PLATFORM,
    CannotInitialize            = QAIRT_BACKEND_ERROR_CANNOT_INITIALIZE,
    TerminateFailed             = QAIRT_BACKEND_ERROR_TERMINATE_FAILED,
    NotSupported                = QAIRT_BACKEND_ERROR_NOT_SUPPORTED,
    InvalidArgument             = QAIRT_BACKEND_ERROR_INVALID_ARGUMENT,
    OpPackageNotFound           = QAIRT_BACKEND_ERROR_OP_PACKAGE_NOT_FOUND,
    OpPackageIfProviderNotFound = QAIRT_BACKEND_ERROR_OP_PACKAGE_IF_PROVIDER_NOT_FOUND,
    OpPackageRegistrationFailed = QAIRT_BACKEND_ERROR_OP_PACKAGE_REGISTRATION_FAILED,
    OpPackageUnsupportedVersion = QAIRT_BACKEND_ERROR_OP_PACKAGE_UNSUPPORTED_VERSION,
    OpPackageDuplicate          = QAIRT_BACKEND_ERROR_OP_PACKAGE_DUPLICATE,
    InconsistentConfig          = QAIRT_BACKEND_ERROR_INCONSISTENT_CONFIG,
    InvalidHandle               = QAIRT_BACKEND_ERROR_INVALID_HANDLE,
    InvalidConfig               = QAIRT_BACKEND_ERROR_INVALID_CONFIG,
    Undefined                   = QAIRT_BACKEND_ERROR_UNDEFINED)

/**
 * @brief Verbosity levels for the error reporting configuration.
 *
 * @details
 * | Enumerator  | Description                                              |
 * |-------------|----------------------------------------------------------|
 * | `Brief`     | Collect basic summary information about each error.      |
 * | `Detailed`  | Collect detailed, memory-resident error information.     |
 * | `Undefined` | Level is unset or unknown.                               |
 */
QAIRT_CPPAPI_DEFINE_ENUM(ErrorReportingConfigLevel,
                         QairtErrorReporting_Config_Level_t,
                         Brief     = QAIRT_ERROR_REPORTING_LEVEL_BRIEF,
                         Detailed  = QAIRT_ERROR_REPORTING_LEVEL_DETAILED,
                         Undefined = QAIRT_ERROR_REPORTING_LEVEL_UNDEFINED)

/**
 * @brief Name descriptor for a single operation supported by a backend.
 *
 * Obtained from @ref Backend::getSupportedOperations(). All string views
 * are non-owning references to backend-managed memory.
 */
// Use string_view to avoid copying out, as it's all owned by the backend
class BackendOperationName {
  /// @brief The op package that provides this operation.
  std::string_view m_packageName;
  /// @brief The name of the operation within its package.
  std::string_view m_name;
  /// @brief The target platform for which this operation entry is registered.
  std::string_view m_target;

 public:
  constexpr BackendOperationName() noexcept                                       = default;
  constexpr BackendOperationName(const BackendOperationName&) noexcept            = default;
  constexpr BackendOperationName(BackendOperationName&&) noexcept                 = default;
  constexpr BackendOperationName& operator=(const BackendOperationName&) noexcept = default;
  constexpr BackendOperationName& operator=(BackendOperationName&&) noexcept      = default;

  /**
   * @brief Construct a BackendOperationName from its three name components.
   *
   * @param[in] packageName  Name of the op package that provides this operation.
   * @param[in] name         Name of the operation within the op package.
   * @param[in] target       Target platform for this operation entry.
   *                         May be empty if the backend does not distinguish targets.
   */
  constexpr BackendOperationName(std::string_view packageName,
                                 std::string_view name,
                                 std::string_view target) noexcept
      : m_packageName{std::move(packageName)},
        m_name{std::move(name)},
        m_target{std::move(target)} {}

  /**
   * @brief Get the op package name for this operation.
   *
   * @return Name of the op package that provides this operation.
   *
   * @see QairtBackend_OperationName_getPackageName
   */
  const std::string_view& getPackageName() const noexcept { return m_packageName; }

  /**
   * @brief Get the operation name within its op package.
   *
   * @return Name of the operation within its package.
   *
   * @see QairtBackend_OperationName_getName
   */
  const std::string_view& getName() const noexcept { return m_name; }

  /**
   * @brief Get the target platform for this operation entry.
   *
   * @return Target platform string, or an empty view if unused by this backend.
   *
   * @see QairtBackend_OperationName_getTarget
   */
  const std::string_view getTarget() const noexcept { return m_target; }
};

/**
 * @brief Configuration object for backend error reporting behavior.
 *
 * Controls how much detail is captured when errors occur and how much
 * memory is reserved for error data. Obtained via @ref Api::createBackend()
 * or set on a @ref BackendConfiguration before backend creation.
 */
class ErrorReportingConfig : public ApiType<ErrorReportingConfig, QairtErrorReporting_Config_V1_t> {
  friend class Api;
  template <typename, typename, typename>
  friend class ::qairt::ApiType;

 public:
  ErrorReportingConfig() = default;

  /**
   * @brief Get the reporting verbosity level for this error reporting configuration.
   *
   * @return The current reporting level.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtErrorReporting_Config_getReportingLevel
   */
  ErrorReportingConfigLevel getReportingLevel() const {
    return static_cast<ErrorReportingConfigLevel>(callGetter<&interface_type::getReportingLevel>());
  }

  /**
   * @brief Set the reporting verbosity level for this error reporting configuration.
   *
   * @param[in] level  Desired reporting verbosity. Must be a valid enumerator.
   *
   * @throws qairt::Exception on invalid handle or invalid level.
   *
   * @see QairtErrorReporting_Config_setReportingLevel
   */
  void setReportingLevel(ErrorReportingConfigLevel level) {
    callSetter<&interface_type::setReportingLevel>(
        static_cast<QairtErrorReporting_Config_Level_t>(level));
  }

  /**
   * @brief Get the memory storage limit for this error reporting configuration.
   *
   * @return Storage limit in kilobytes.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtErrorReporting_Config_getStorageLimit
   */
  uint32_t getStorageLimit() const { return callGetter<&interface_type::getStorageLimit>(); }

  /**
   * @brief Set the memory storage limit for this error reporting configuration.
   *
   * @param[in] limit  Maximum memory reserved for error information, in kilobytes.
   *
   * @throws qairt::Exception on invalid handle or invalid limit.
   *
   * @see QairtErrorReporting_Config_setStorageLimit
   */
  void setStorageLimit(uint32_t limit) { callSetter<&interface_type::setStorageLimit>(limit); }

 private:
  ErrorReportingConfig(const std::shared_ptr<ApiTable>& apiTable,
                       ErrorReportingConfigLevel level,
                       uint32_t storageLimit)
      : ApiType(apiTable) {
    construct(static_cast<QairtErrorReporting_Config_Level_t>(level), storageLimit);
  }
  explicit ErrorReportingConfig(const std::shared_ptr<ApiTable>& apiTable)
      : ErrorReportingConfig(apiTable, ErrorReportingConfigLevel::Undefined, uint32_t{}) {}
};

/**
 * @brief Abstract base class for a single backend-specific custom configuration item.
 *
 * Subclass this to provide a backend-specific configuration handle to
 * @ref BackendConfiguration::setCustomConfig(). Refer to the backend
 * documentation for the concrete subclass and valid handle values.
 */
class BackendCustomConfig : public CustomConfigType {
 protected:
  BackendCustomConfig()                                          = default;
  BackendCustomConfig(const BackendCustomConfig&)                = default;
  BackendCustomConfig(BackendCustomConfig&&) noexcept            = default;
  BackendCustomConfig& operator=(const BackendCustomConfig&)     = default;
  BackendCustomConfig& operator=(BackendCustomConfig&&) noexcept = default;

 public:
  virtual ~BackendCustomConfig()                                          = default;

  /**
   * @brief Get the underlying C handle for this custom configuration item.
   *
   * @return The backend-specific custom configuration handle.
   */
  virtual QairtBackend_CustomConfigHandle_t getCustomConfigHandle() const = 0;
};

/**
 * @brief Abstract base class for a collection of backend-specific custom configuration items.
 *
 * Subclass this to provide multiple backend-specific configuration handles to
 * @ref BackendConfiguration::setCustomConfigs(). Refer to the backend
 * documentation for the concrete subclass and valid handle values.
 */
class BackendCustomConfiguration {
 protected:
  BackendCustomConfiguration()                                                 = default;
  BackendCustomConfiguration(const BackendCustomConfiguration&)                = default;
  BackendCustomConfiguration(BackendCustomConfiguration&&) noexcept            = default;
  BackendCustomConfiguration& operator=(const BackendCustomConfiguration&)     = default;
  BackendCustomConfiguration& operator=(BackendCustomConfiguration&&) noexcept = default;

 public:
  virtual ~BackendCustomConfiguration()                                           = default;

  /**
   * @brief Get the list of underlying C handles for this custom configuration collection.
   *
   * @return Vector of backend-specific custom configuration handles.
   */
  virtual std::vector<QairtBackend_CustomConfigHandle_t> getCustomConfigs() const = 0;
};

/**
 * @brief Configuration object for backend creation and reconfiguration.
 *
 * Construct directly — `BackendConfiguration()` — and call setter methods to
 * populate options before passing to @ref Api::createBackend() or
 * @ref Backend::setConfig(). All setter methods return `*this` to support
 * method chaining.
 */
class BackendConfiguration : public ApiType<BackendConfiguration, QairtBackend_ConfigV1_t> {
  friend class Api;
  /// @brief Optional error reporting configuration cross-linked to the C handle.
  std::optional<detail::crossable<detail::non_owning<ErrorReportingConfig>,
                                  &interface_type::getErrorReportingConfig,
                                  &interface_type::setErrorReportingConfig>>
      m_errorReportingConfig;

 public:
  BackendConfiguration() noexcept                                  = default;
  BackendConfiguration(const BackendConfiguration&)                = delete;
  BackendConfiguration(BackendConfiguration&&) noexcept            = default;
  BackendConfiguration& operator=(const BackendConfiguration&)     = delete;
  BackendConfiguration& operator=(BackendConfiguration&&) noexcept = default;

 public:
  /**
   * @brief Set a single backend-specific custom configuration item on this configuration.
   *
   * @param[in] backendCustomConfig  The custom configuration item whose handle will be applied.
   *
   * @return Reference to this object for method chaining.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtBackend_Config_setCustomConfigs
   */
  BackendConfiguration& setCustomConfig(const BackendCustomConfig& backendCustomConfig) {
    auto cch = backendCustomConfig.getCustomConfigHandle();
    callAndConfirm<&interface_type::setCustomConfigs>(&cch, 1u);
    return *this;
  }

  /**
   * @brief Set a collection of backend-specific custom configuration items on this configuration.
   *
   * @param[in] config  The custom configuration collection whose handles will be applied.
   *
   * @return Reference to this object for method chaining.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtBackend_Config_setCustomConfigs
   */
  BackendConfiguration& setCustomConfigs(const BackendCustomConfiguration& config) {
    auto cfgHandles = config.getCustomConfigs();
    callAndConfirm<&interface_type::setCustomConfigs>(cfgHandles.data(),
                                                      static_cast<uint32_t>(cfgHandles.size()));
    return *this;
  }

  /**
   * @brief Get the number of platform options set on this configuration.
   *
   * @return Number of platform option strings currently set.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtBackend_Config_getNumPlatformOptions
   */
  uint32_t getNumPlatformOptions() const {
    return callGetter<&interface_type::getNumPlatformOptions>();
  }

  /**
   * @brief Get the platform option string at the specified index.
   *
   * @param[in] idx  Zero-based index into the platform options list.
   *                 Must be less than getNumPlatformOptions().
   *
   * @return The null-terminated platform option key-value pair string at _idx_,
   *         or an empty view if the stored pointer is null.
   *
   * @throws qairt::Exception on invalid handle or out-of-range index.
   *
   * @see QairtBackend_Config_getPlatformOptionAt
   */
  std::string_view getPlatformOptionAt(uint32_t idx) const {
    const char* str{};
    callAndConfirm<&interface_type::getPlatformOptionAt>(idx, &str);
    return std::string_view(str == nullptr ? "" : str);
  }

  /**
   * @brief Get all platform option strings set on this configuration.
   *
   * @return Vector of platform option key-value pair strings.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtBackend_Config_getNumPlatformOptions
   */
  std::vector<std::string_view> getPlatformOptions() const {
    const uint32_t size = getNumPlatformOptions();
    std::vector<std::string_view> toret(size);
    for (uint32_t i = 0; i < size; ++i) {
      toret[i] = getPlatformOptionAt(i);
    }
    return toret;
  }

  /**
   * @brief Set the platform options on this configuration from an array of C strings.
   *
   * @param[in] platformOptions  Array of null-terminated platform option strings.
   *
   * @return Reference to this object for method chaining.
   *
   * @throws qairt::Exception on invalid handle or invalid options.
   *
   * @see QairtBackend_Config_setPlatformOptions
   */
  BackendConfiguration& setPlatformOptions(const std::vector<const char*>& platformOptions) {
    callSetter<&interface_type::setPlatformOptions>(platformOptions.data(),
                                                    static_cast<uint32_t>(platformOptions.size()));
    return *this;
  }

  /**
   * @brief Wrapper which allows for `std::string` platform option values instead of `const char*`.
   *
   * @see BackendConfiguration::setPlatformOptions(const std::vector<const char*>&)
   */
  BackendConfiguration& setPlatformOptions(const std::vector<std::string>& platformOptions) {
    std::vector<const char*> optionPtrs(platformOptions.size());
    QAIRT_NO_RETURN std::transform(platformOptions.begin(),
                                   platformOptions.end(),
                                   optionPtrs.begin(),
                                   [](const std::string& s) { return s.c_str(); });
    return setPlatformOptions(optionPtrs);
  }

  /**
   * @brief Wrapper which allows for `std::string_view` platform option values instead of `const char*`.
   *
   * @see BackendConfiguration::setPlatformOptions(const std::vector<const char*>&)
   */
  BackendConfiguration& setPlatformOptions(const std::vector<std::string_view>& platformOptions) {
    std::vector<std::string> optionStrs(platformOptions.size());
    QAIRT_NO_RETURN std::transform(platformOptions.begin(),
                                   platformOptions.end(),
                                   optionStrs.begin(),
                                   [](const std::string_view& sv) { return std::string(sv); });
    return setPlatformOptions(optionStrs);
  }

  /**
   * @brief Clear all platform options from this configuration.
   *
   * @return Reference to this object for method chaining.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtBackend_Config_setPlatformOptions
   */
  BackendConfiguration& resetPlatformOptions() {
    return setPlatformOptions(std::vector<const char*>{});
  }

  /**
   * @brief Get the error reporting configuration attached to this backend configuration.
   *
   * @return A reference wrapper to the attached @ref ErrorReportingConfig, or an empty
   *         optional if none has been set.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtBackend_Config_getErrorReportingConfig
   */
  std::optional<std::reference_wrapper<ErrorReportingConfig>> getErrorReportingConfig() {
    if (m_errorReportingConfig.has_value()) {
      return m_errorReportingConfig->bindNonOwning(*this);
    }
    return {};
  }

  /**
   * @brief Get the error reporting configuration attached to this backend configuration.
   *
   * @return A const reference wrapper to the attached @ref ErrorReportingConfig, or an
   *         empty optional if none has been set.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtBackend_Config_getErrorReportingConfig
   */
  std::optional<std::reference_wrapper<ErrorReportingConfig>> getErrorReportingConfig() const {
    if (m_errorReportingConfig.has_value()) {
      return m_errorReportingConfig->bindNonOwning(*this);
    }
    return {};
  }

  /**
   * @brief Attach an error reporting configuration to this backend configuration.
   *
   * @param[in] errorReportingConfig  The error reporting configuration to attach.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtBackend_Config_setErrorReportingConfig
   */
  void setErrorReportingConfig(const ErrorReportingConfig& errorReportingConfig) {
    if (!m_errorReportingConfig.has_value()) {
      QAIRT_NO_RETURN m_errorReportingConfig.emplace(
          typename decltype(m_errorReportingConfig)::value_type{});
    }
    QAIRT_NO_RETURN m_errorReportingConfig->setNonOwning(*this, errorReportingConfig);
  }

 private:
  void prepareToCross() const {
    if (m_errorReportingConfig.has_value()) {
      prepareMultipleToCross(*m_errorReportingConfig);
    }
  }
  void updateAfterCross() const {
    if (m_errorReportingConfig.has_value()) {
      updateMultipleAfterCross(*m_errorReportingConfig);
    }
  }

 private:
  explicit BackendConfiguration(const std::shared_ptr<ApiTable>& apiTable) : ApiType(apiTable) {
    construct();
  }
};

/**
 * @brief Wrapper for a QAIRT Backend handle.
 *
 * Obtained via @ref Api::createBackend().
 */
class Backend : public ApiType<Backend, QairtBackend_V1_t> {
  friend class Api;

  /// @brief Staging storage for a BackendConfiguration being crossed to the C layer.
  detail::crossable<detail::set_only<BackendConfiguration>, nullptr, &interface_type::setConfig>
      m_memory;

 public:
  Backend() noexcept                     = default;
  Backend(const Backend&)                = delete;
  Backend(Backend&&) noexcept            = default;
  Backend& operator=(const Backend&)     = delete;
  Backend& operator=(Backend&&) noexcept = default;

  // Non-owning constructor: wraps an existing backend handle without taking ownership.
  // The destructor will NOT call free() on the handle.
  Backend(const std::shared_ptr<ApiTable>& apiTable, QairtBackend_Handle_t handle)
      : ApiType(apiTable) {
    setHandle(handle);
    setNonOwning(true);
  }

  /**
   * @brief Set configuration options on this backend after creation.
   *
   * @param[in] config  The backend configuration to apply.
   *
   * @throws qairt::Exception on invalid handle, invalid config, or unsupported feature.
   *
   * @see QairtBackend_setConfig
   */
  void setConfig(const BackendConfiguration& config) {
    detail::QairtTypeAdmin::prepareToCross(config);
    callSetter<&interface_type::setConfig>(getHandle(config));
  }

  /**
   * @brief Register an op package library with this backend.
   *
   * Loads the shared library at _packagePath_ and registers its operations
   * using the interface provider function _interfaceProvider_. An optional
   * _target_ platform string restricts registration to a specific processing unit.
   *
   * @param[in] packagePath       Path on disk to the op package shared library.
   *                              Must not be NULL.
   * @param[in] interfaceProvider Name of the interface provider function exported
   *                              by the op package library. Must not be NULL.
   * @param[in] target            Optional target platform string. NULL applies
   *                              no target restriction.
   *
   * @throws qairt::Exception on:
   *  - invalid handle
   *  - NULL _packagePath_ or _interfaceProvider_
   *  - library not found
   *  - interface provider symbol not found
   *  - registration failure
   *  - unsupported op package interface version
   *  - duplicate op registration
   *
   * @see QairtBackend_registerOpPackage
   */
  void registerOpPackage(const char* packagePath,
                         const char* interfaceProvider,
                         const char* target) {
    callSetter<&interface_type::registerOpPackage>(packagePath, interfaceProvider, target);
  }

  /**
   * @brief Wrapper which allows for `std::string` path arguments instead of `const char*`.
   *
   * @see Backend::registerOpPackage(const char*, const char*, const char*)
   */
  void registerOpPackage(const std::string& packagePath,
                         const std::string& interfaceProvider,
                         const std::string& target) {
    registerOpPackage(packagePath.c_str(), interfaceProvider.c_str(), target.c_str());
  }

  /**
   * @brief Get all operations supported by this backend, including built-in ops.
   *
   * @return Vector of @ref BackendOperationName descriptors, one per supported operation.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtBackend_getSupportedOperations
   */
  std::vector<BackendOperationName> getSupportedOperations() const {
    const QairtBackend_OperationNameHandle_t* opnames;
    uint32_t count;
    callAndConfirm<&interface_type::getSupportedOperations>(&opnames, &count);
    std::vector<BackendOperationName> toret;
    auto confirm = [](auto status) {
      if (status != QAIRT_SUCCESS) {
        throw std::runtime_error("failed to get backend operation name");
      }
    };
    auto tempDodgeNull = [](const char* str) -> std::string_view {
      return str ? str : "{nullptr}";
    };
    for (uint32_t i = 0; i < count; ++i) {
      const char *a, *b, *c;
      confirm(callNoHandle<&QairtBackend_OperationNameV1_t::getPackageName>(opnames[i], &a));
      confirm(callNoHandle<&QairtBackend_OperationNameV1_t::getName>(opnames[i], &b));
      confirm(callNoHandle<&QairtBackend_OperationNameV1_t::getTarget>(opnames[i], &c));
      ;
      QAIRT_NO_RETURN toret.emplace_back(tempDodgeNull(a), tempDodgeNull(b), tempDodgeNull(c));
    }
    return toret;
  }

  /**
   * @brief Validate an op configuration against the appropriate registered op package.
   *
   * The backend selects the op package for validation based on attributes of _opConfig_.
   *
   * @param[in] opConfig  The op configuration to validate.
   *
   * @throws qairt::Exception on:
   *  - invalid handle
   *  - validation failure
   *  - validation not supported by this backend
   *  - no matching op package found
   *
   * @see QairtBackend_validateOpConfig
   */
  void validateOpConfig(const OpConfig& opConfig) {
    detail::QairtTypeAdmin::prepareToCross(opConfig);
    callSetter<&interface_type::validateOpConfig>(getHandle(opConfig));
  }

  /**
   * @brief Validate a context binary against a device and context configuration.
   *
   * Checks that the binary is compatible with _device_ and the options in _contextConfig_
   * before it is loaded via createContextFromBinary().
   *
   * @param[in] device         The device on which the binary would be loaded.
   * @param[in] contextBinary  The context binary buffer to validate.
   * @param[in] contextConfig  The context configuration to validate against.
   *
   * @throws qairt::Exception on invalid handle or validation failure.
   */
  void validateContextBinary(ApiTypeRef<const Device&> device,
                             ApiTypeRef<const ContextBinaryBuffer&> contextBinary,
                             ApiTypeRef<const ContextConfiguration&> contextConfig) {
    callAndConfirm<&QairtContext_V1_t::validateBinary>(detail::getHandle(device),
                                                       detail::getHandle(contextConfig),
                                                       detail::getHandle(contextBinary));
  }

  /**
   * @brief Create a context using this backend.
   *
   * Creates a context with no device (uses backend default) and an optional
   * context configuration.
   *
   * @param[in] contextConfig  Context configuration. Optional.
   *
   * @return A new @ref Context.
   *
   * @throws qairt::Exception on invalid handle or configuration error.
   */
  Context createContext(ApiTypeRef<const ContextConfiguration&> contextConfig = {}) {
    return create<Context>(getHandle(*this), nullptr, detail::getHandle(contextConfig));
  }

  /**
   * @brief Create a context for a specific device using this backend.
   *
   * @param[in] device        The device on which to create the context.
   * @param[in] contextConfig Context configuration. Optional.
   *
   * @return A new @ref Context.
   *
   * @throws qairt::Exception on invalid handle or configuration error.
   */
  Context createContext(ApiTypeRef<const Device&> device,
                        ApiTypeRef<const ContextConfiguration&> contextConfig) {
    return create<Context>(
        getHandle(*this), detail::getHandle(device), detail::getHandle(contextConfig));
  }

  /**
   * @brief Create a context from a serialized context binary.
   *
   * Pass a @ref Signal to enable aborting or timing out the load operation.
   *
   * @param[in] device               The device on which to load the context binary.
   * @param[in] contextConfig        Context configuration. Optional.
   * @param[in] contextBinaryBuffer  The serialized context binary to load.
   * @param[in] signal               Optional signal to control the load operation.
   * @param[in] profile              Optional profile handle to collect load-time events.
   *
   * @return A new @ref Context.
   *
   * @throws qairt::Exception on invalid handle, binary incompatibility, or configuration error.
   */
  Context createContextFromBinary(ApiTypeRef<const Device&> device,
                                  ApiTypeRef<const ContextConfiguration&> contextConfig,
                                  ApiTypeRef<const ContextBinaryBuffer&> contextBinaryBuffer,
                                  ApiTypeRef<const Signal&> signal   = {},
                                  ApiTypeRef<const Profile&> profile = {}) {
    return create<Context>(getHandle(*this),
                           detail::getHandle(device),
                           detail::getHandle(contextConfig),
                           detail::getHandle(contextBinaryBuffer),
                           detail::getHandle(signal),
                           detail::getHandle(profile));
  }

  /**
   * @brief Create a profiling handle at the specified granularity.
   *
   * @param[in] level  Granularity level at which events should be collected.
   *
   * @return A new @ref Profile.
   *
   * @throws qairt::Exception on invalid handle, unsupported profiling level, or memory error.
   *
   * @see QairtBackend_createProfile
   */
  Profile createProfile(uint32_t level) {
    QairtProfile_Handle_t profileHandle;
    callAndConfirm<&interface_type::createProfile>(level, &profileHandle);
    return create<Profile>(profileHandle);
  }

  /**
   * @brief Create a shared profiling handle at the specified granularity.
   *
   * @param[in] level  Granularity level at which events should be collected.
   *
   * @return A `std::shared_ptr` to a new @ref Profile.
   *
   * @throws qairt::Exception on invalid handle, unsupported profiling level, or memory error.
   *
   * @see QairtBackend_createProfile
   */
  std::shared_ptr<Profile> createSharedProfile(uint32_t level) {
    QairtProfile_Handle_t profileHandle;
    callAndConfirm<&interface_type::createProfile>(level, &profileHandle);
    return std::make_shared<Profile>(create<Profile>(profileHandle));
  }

  /**
   * @brief Create a new signal object associated with this backend.
   *
   * Signals are used to control the execution of API calls that accept
   * them (e.g., @ref Graph::execute, @ref Context::createFromBinary).
   * The created signal is idle and immediately available for use.
   *
   * The signal is backend-scoped: the backend allocates any required
   * synchronization primitives and validates signal support. A signal
   * may only be used with the backend that created it.
   *
   * @param[in] signalConfig  Configuration for the signal. Optional.
   *
   * @return A new @ref Signal object.
   *
   * @throws qairt::Exception on:
   *  - invalid handle
   *  - signals not supported by this backend
   *  - invalid signal configuration
   *  - memory allocation failure
   */
  Signal createSignal(ApiTypeRef<const SignalConfiguration&> signalConfig = {}) {
    return create<Signal>(getHandle(*this), detail::getHandle(signalConfig));
  }

 private:
  Backend(const std::shared_ptr<ApiTable>& apiTable,
          ApiTypeRef<const Log&> log,
          ApiTypeRef<const BackendConfiguration&> config = {})
      : ApiType(apiTable) {
    construct(detail::getHandle(log), detail::getHandle(config));
  }
};

}  // namespace qairt

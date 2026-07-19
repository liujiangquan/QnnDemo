//=============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//=============================================================================

#pragma once

/**
 * @file
 * @brief C++ wrapper for the QAIRT Signal and Signal Configuration APIs.
 */

#include "QairtApi.hpp"
#include "QairtBackend/QairtBackend.h"
#include "QairtSignal/QairtSignal.h"
#include "QairtSignal/QairtSignalConfig.h"

namespace qairt {

/**
 * @brief Opaque handle type for backend-specific signal custom configuration.
 *
 *        Refer to the backend documentation for valid values and usage.
 *        This is an alias for the underlying C handle type.
 */
using SignalCustomConfig = QairtSignal_CustomConfigHandle_t;

/**
 * @brief Configuration object for a signal.
 *
 *        Created via @ref Api::make<SignalConfiguration>().
 */
class SignalConfiguration : public ApiType<SignalConfiguration, QairtSignal_ConfigV1_t> {
  friend Api;
  template <typename, typename, typename>
  friend class ::qairt::ApiType;

 public:
  SignalConfiguration() noexcept                                 = default;
  SignalConfiguration(const SignalConfiguration&)                = delete;
  SignalConfiguration(SignalConfiguration&&) noexcept            = default;
  SignalConfiguration& operator=(const SignalConfiguration&)     = delete;
  SignalConfiguration& operator=(SignalConfiguration&&) noexcept = default;

  /**
   * @brief Set the timeout duration for this signal configuration.
   *
   *        Configures the signal as a timeout signal. Once set, the signal
   *        will interrupt the associated API call after the specified duration.
   *        Tolerance for the actual timeout is platform-dependent and cannot
   *        be guaranteed.
   *
   *        Setting a timeout is mutually exclusive with abort behavior.
   *        Call @ref resetTimeoutDurationUs() to restore abort behavior.
   *
   * @param timeoutDurationUs  Timeout in microseconds. Must be non-zero.
   *
   * @throws Exception on invalid handle or invalid argument.
   */
  void setTimeoutDurationUs(uint64_t timeoutDurationUs) {
    callSetter<&interface_type::setTimeoutDuration>(timeoutDurationUs);
  }

  /**
   * @brief Get the configured timeout duration.
   *
   * @return Timeout in microseconds, or 0 if no timeout has been set
   *         (signal is configured for abort behavior).
   *
   * @throws Exception on invalid handle.
   */
  uint64_t getTimeoutDurationUs() const {
    return callGetter<&interface_type::getTimeoutDuration>();
  }

  /**
   * @brief Reset the timeout duration to zero, restoring implicit abort behavior.
   *
   * @throws Exception on invalid handle.
   */
  void resetTimeoutDurationUs() { callSetter<&interface_type::setTimeoutDuration>(uint64_t{0}); }

  /**
   * @brief Set backend-specific custom configuration for this signal.
   *
   *        Refer to the backend documentation for valid handle values and
   *        semantics.
   *
   * @param customConfig  Handle to the backend-specific custom config.
   *
   * @throws Exception on invalid handle.
   */
  void setCustomConfig(SignalCustomConfig customConfig) {
    callSetter<&interface_type::setCustomConfig>(customConfig);
  }

  /**
   * @brief Get the backend-specific custom configuration.
   *
   * @return Handle to the custom config, or nullptr if none was set.
   *
   * @throws Exception on invalid handle.
   */
  SignalCustomConfig getCustomConfig() const {
    return callGetter<&interface_type::getCustomConfig>();
  }

  /**
   * @brief Clear any previously set custom configuration.
   *
   * @throws Exception on invalid handle.
   */
  void resetCustomConfig() {
    callSetter<&interface_type::setCustomConfig>(static_cast<SignalCustomConfig>(nullptr));
  }

 private:
  explicit SignalConfiguration(const std::shared_ptr<ApiTable>& apiTable) : ApiType(apiTable) {
    construct();
  }
};

// Forward declaration — Backend is the factory for Signal instances.
class Backend;

/**
 * @brief Wrapper for a QAIRT Signal object.
 *
 *        Created via @ref Backend::createSignal().
 */
class Signal : public ApiType<Signal, QairtSignal_V1_t> {
  friend Api;
  friend Backend;
  template <typename, typename, typename>
  friend class ::qairt::ApiType;

 public:
  Signal() noexcept                    = default;
  Signal(const Signal&)                = delete;
  Signal(Signal&&) noexcept            = default;
  Signal& operator=(const Signal&)     = delete;
  Signal& operator=(Signal&&) noexcept = default;

  /**
   * @brief Reconfigure this signal with a new configuration.
   *
   * @param config  The new signal configuration.
   *
   * @throws Exception if the signal is in-use, the handle is invalid,
   *         or the configuration is invalid.
   */
  void setConfig(const SignalConfiguration& config) {
    callSetter<&interface_type::setConfig>(getHandle(config));
  }

  /**
   * @brief Trigger the signal action on the associated in-progress API call.
   *
   * @throws Exception if the signal is idle, the handle is invalid, or
   *         the signal type is incompatible with the associated API call.
   */
  void trigger() { callSetter<&interface_type::trigger>(); }

 private:
  Signal(const std::shared_ptr<ApiTable>& apiTable,
         QairtBackend_Handle_t backendHandle,
         QairtSignal_ConfigHandle_t configHandle)
      : ApiType(apiTable) {
    construct(backendHandle, configHandle);
  }
};

}  // namespace qairt

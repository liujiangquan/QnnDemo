//=============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//=============================================================================
#pragma once

#include <optional>
#include <stdexcept>
#include <vector>

#include "QairtCppApi/QairtBackend.hpp"
#include "QairtDsp.hpp"
#include "QairtDspBackend.h"

namespace qairt {

QAIRT_CPPAPI_DEFINE_ENUM(DspArch,
                         QairtDspBackend_DspArch_t,
                         None    = QAIRT_DSP_BACKEND_DSP_ARCH_NONE,
                         V65     = QAIRT_DSP_BACKEND_DSP_ARCH_V65,
                         V66     = QAIRT_DSP_BACKEND_DSP_ARCH_V66,
                         V68     = QAIRT_DSP_BACKEND_DSP_ARCH_V68,
                         V69     = QAIRT_DSP_BACKEND_DSP_ARCH_V69,
                         V73     = QAIRT_DSP_BACKEND_DSP_ARCH_V73,
                         Unknown = QAIRT_DSP_BACKEND_DSP_ARCH_UNKNOWN)

class DspBackendCustomConfig
    : public DspApiType<DspBackendCustomConfig, QairtDspBackend_CustomConfigV1_t> {
  friend detail::TempWorkaround;
  template <typename>
  friend class BackendExtension;

 public:
  /// Returns the handle as a generic backend custom config handle, compatible
  /// with BackendConfiguration::setCustomConfig().
  QairtBackend_CustomConfigHandle_t getBackendCustomConfigHandle() const { return getHandle(); }

  DspBackendCustomConfig()                                             = default;
  DspBackendCustomConfig(const DspBackendCustomConfig&)                = delete;
  DspBackendCustomConfig(DspBackendCustomConfig&&) noexcept            = default;
  DspBackendCustomConfig& operator=(const DspBackendCustomConfig&)     = delete;
  DspBackendCustomConfig& operator=(DspBackendCustomConfig&&) noexcept = default;

  void setFoldReluActivationIntoConvOff(bool off) {
    callSetter<&interface_type::setFoldReluActivationIntoConvOff>(static_cast<uint8_t>(off));
  }
  bool getFoldReluActivationIntoConvOff() const {
    return static_cast<bool>(callGetter<&interface_type::getFoldReluActivationIntoConvOff>());
  }

  void setShortDepthConvOnHmxOff(bool off) {
    callSetter<&interface_type::setShortDepthConvOnHmxOff>(static_cast<uint8_t>(off));
  }
  bool getShortDepthConvOnHmxOff() const {
    return static_cast<bool>(callGetter<&interface_type::getShortDepthConvOnHmxOff>());
  }

  void setUseSignedProcessDomain(bool use) {
    callSetter<&interface_type::setUseSignedProcessDomain>(static_cast<uint8_t>(use));
  }
  bool getUseSignedProcessDomain() const {
    return static_cast<bool>(callGetter<&interface_type::getUseSignedProcessDomain>());
  }

  void setArch(QairtDspBackend_DspArch_t arch) { callSetter<&interface_type::setArch>(arch); }
  QairtDspBackend_DspArch_t getArch() const { return callGetter<&interface_type::getArch>(); }

 private:
  DspBackendCustomConfig(const std::weak_ptr<api_table_type>& apiTable) : ApiType(apiTable.lock()) {
    if (!apiTable.lock()) {
      throw std::runtime_error("Failed to lock apiTable");
    }
    construct();
    if (!getHandle()) {
      throw std::runtime_error("Failed to construct DspBackendCustomConfig");
    }
  }
};

class DspBackendCustomConfiguration : public detail::TabularBEType<DspBackendExtensionTable>,
                                      public BackendCustomConfiguration {
  std::optional<bool> m_foldReluActivationIntoConvOff{};
  std::optional<bool> m_shortDepthConvOnHmxOff{};
  std::optional<bool> m_useSignedProcessDomain{};
  std::optional<QairtDspBackend_DspArch_t> m_arch{};

  mutable std::vector<DspBackendCustomConfig> m_customCfgs;

 public:
  /// Builds and returns the list of raw backend custom config handles, suitable
  /// for passing to BackendConfiguration::setCustomConfigs().
  std::vector<QairtBackend_CustomConfigHandle_t> getCustomConfigs() const override {
    return getCustomConfigHandles();
  }

  std::vector<QairtBackend_CustomConfigHandle_t> getCustomConfigHandles() const {
    std::vector<QairtBackend_CustomConfigHandle_t> toret;
    m_customCfgs = buildCustomConfigs();
    toret.reserve(m_customCfgs.size());
    for (auto& cfg : m_customCfgs) {
      toret.push_back(cfg.getBackendCustomConfigHandle());
    }
    return toret;
  }

  std::vector<DspBackendCustomConfig> buildCustomConfigs() const {
    if (!m_backendExtensionTable.lock()) {
      throw std::runtime_error("Malformed DspBackendCustomConfiguration: missing API table.");
    }
    std::vector<DspBackendCustomConfig> toret;

    if (m_foldReluActivationIntoConvOff.has_value()) {
      DspBackendCustomConfig& cfg = toret.emplace_back(make<DspBackendCustomConfig>());
      cfg.setFoldReluActivationIntoConvOff(m_foldReluActivationIntoConvOff.value());
    }
    if (m_shortDepthConvOnHmxOff.has_value()) {
      DspBackendCustomConfig& cfg = toret.emplace_back(make<DspBackendCustomConfig>());
      cfg.setShortDepthConvOnHmxOff(m_shortDepthConvOnHmxOff.value());
    }
    if (m_useSignedProcessDomain.has_value()) {
      DspBackendCustomConfig& cfg = toret.emplace_back(make<DspBackendCustomConfig>());
      cfg.setUseSignedProcessDomain(m_useSignedProcessDomain.value());
    }
    if (m_arch.has_value()) {
      DspBackendCustomConfig& cfg = toret.emplace_back(make<DspBackendCustomConfig>());
      cfg.setArch(m_arch.value());
    }

    return toret;
  }

 public:
  using base_type::base_type;

  // Fold Relu Activation Into Conv Off
  void clearFoldReluActivationIntoConvOff() { m_foldReluActivationIntoConvOff.reset(); }
  void setFoldReluActivationIntoConvOff(bool off) { m_foldReluActivationIntoConvOff = off; }
  std::optional<bool>& getFoldReluActivationIntoConvOff() {
    return m_foldReluActivationIntoConvOff;
  }
  const std::optional<bool>& getFoldReluActivationIntoConvOff() const {
    return m_foldReluActivationIntoConvOff;
  }

  // Short Depth Conv On HMX Off
  void clearShortDepthConvOnHmxOff() { m_shortDepthConvOnHmxOff.reset(); }
  void setShortDepthConvOnHmxOff(bool off) { m_shortDepthConvOnHmxOff = off; }
  std::optional<bool>& getShortDepthConvOnHmxOff() { return m_shortDepthConvOnHmxOff; }
  const std::optional<bool>& getShortDepthConvOnHmxOff() const { return m_shortDepthConvOnHmxOff; }

  // Use Signed Process Domain
  void clearUseSignedProcessDomain() { m_useSignedProcessDomain.reset(); }
  void setUseSignedProcessDomain(bool use) { m_useSignedProcessDomain = use; }
  std::optional<bool>& getUseSignedProcessDomain() { return m_useSignedProcessDomain; }
  const std::optional<bool>& getUseSignedProcessDomain() const { return m_useSignedProcessDomain; }

  // DSP Architecture
  void clearArch() { m_arch.reset(); }
  void setArch(QairtDspBackend_DspArch_t arch) { m_arch = arch; }
  std::optional<QairtDspBackend_DspArch_t>& getArch() { return m_arch; }
  const std::optional<QairtDspBackend_DspArch_t>& getArch() const { return m_arch; }
};

}  // namespace qairt

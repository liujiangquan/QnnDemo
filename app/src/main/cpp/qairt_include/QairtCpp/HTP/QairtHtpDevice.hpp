//=============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//=============================================================================
#pragma once

#include <optional>
#include <vector>

#include "HTP/QairtHtpDevice.h"
#include "QairtCppApi/QairtBackend.hpp"
#include "QairtHtp.hpp"

namespace qairt {

QAIRT_CPPAPI_DEFINE_ENUM(HtpDeviceArch,
                         QairtHtpDevice_Arch_t,
                         None    = QAIRT_HTP_DEVICE_ARCH_NONE,
                         V68     = QAIRT_HTP_DEVICE_ARCH_V68,
                         V69     = QAIRT_HTP_DEVICE_ARCH_V69,
                         V73     = QAIRT_HTP_DEVICE_ARCH_V73,
                         V75     = QAIRT_HTP_DEVICE_ARCH_V75,
                         V79     = QAIRT_HTP_DEVICE_ARCH_V79,
                         V81     = QAIRT_HTP_DEVICE_ARCH_V81,
                         V85     = QAIRT_HTP_DEVICE_ARCH_V85,
                         V89     = QAIRT_HTP_DEVICE_ARCH_V89,
                         Unknown = QAIRT_HTP_DEVICE_ARCH_UNKNOWN)

class HtpDeviceCustomConfig
    : public HtpApiType<HtpDeviceCustomConfig, QairtHtpDevice_CustomConfigV1_t>,
      public BackendCustomConfig {
  friend detail::TempWorkaround;
  template <typename>
  friend class BackendExtension;

 public:
  QairtBackend_CustomConfigHandle_t getCustomConfigHandle() const override { return getHandle(); }

  HtpDeviceCustomConfig()                                            = default;
  HtpDeviceCustomConfig(const HtpDeviceCustomConfig&)                = delete;
  HtpDeviceCustomConfig(HtpDeviceCustomConfig&&) noexcept            = default;
  HtpDeviceCustomConfig& operator=(const HtpDeviceCustomConfig&)     = delete;
  HtpDeviceCustomConfig& operator=(HtpDeviceCustomConfig&&) noexcept = default;

  // SoC Model
  void setSocModel(uint32_t socModel) { callSetter<&interface_type::setSocModel>(socModel); }
  uint32_t getSocModel() const { return callGetter<&interface_type::getSocModel>(); }

  // Minimum Architecture
  void setArch(uint32_t deviceId, QairtHtpDevice_Arch_t arch) {
    callAndConfirm<&interface_type::setArch>(deviceId, arch);
  }
  std::pair<uint32_t, QairtHtpDevice_Arch_t> getArch() const {
    uint32_t deviceId{};
    QairtHtpDevice_Arch_t arch{};
    callAndConfirm<&interface_type::getArch>(&deviceId, &arch);
    return {deviceId, arch};
  }

  // Use Signed Process Domain
  void setUseSignedProcessDomain(uint32_t deviceId, bool useSignedProcessDomain) {
    callAndConfirm<&interface_type::setUseSignedProcessDomain>(
        deviceId, detail::convertBool(useSignedProcessDomain));
  }
  std::pair<uint32_t, bool> getUseSignedProcessDomain() const {
    uint32_t deviceId{};
    uint8_t val{};
    callAndConfirm<&interface_type::getUseSignedProcessDomain>(&deviceId, &val);
    return {deviceId, detail::resolveBool(val)};
  }

  // Use Secure Process Domain
  void setUseSecureProcessDomain(uint32_t deviceId, bool useSecureProcessDomain) {
    callAndConfirm<&interface_type::setUseSecureProcessDomain>(
        deviceId, detail::convertBool(useSecureProcessDomain));
  }
  std::pair<uint32_t, bool> getUseSecureProcessDomain() const {
    uint32_t deviceId{};
    uint8_t val{};
    callAndConfirm<&interface_type::getUseSecureProcessDomain>(&deviceId, &val);
    return {deviceId, detail::resolveBool(val)};
  }

 private:
  explicit HtpDeviceCustomConfig(const std::weak_ptr<api_table_type>& apiTable)
      : ApiType(apiTable.lock()) {
    if (!apiTable.lock()) {
      throw std::runtime_error("Failed to lock apiTable");
    }
    construct();
    if (!getHandle()) {
      throw std::runtime_error("Failed to construct");
    }
  }
};

struct HtpDeviceArchConfig {
  uint32_t deviceId{};
  QairtHtpDevice_Arch_t arch{QAIRT_HTP_DEVICE_ARCH_UNKNOWN};
};

struct HtpDeviceSignedPdConfig {
  uint32_t deviceId{};
  bool useSignedProcessDomain{};
};

struct HtpDeviceSecurePdConfig {
  uint32_t deviceId{};
  bool useSecureProcessDomain{};
};

class HtpDeviceCustomConfiguration : public detail::TabularBEType<HtpBackendExtensionTable>,
                                     public BackendCustomConfiguration {
  std::optional<uint32_t> m_socModel{};
  std::optional<HtpDeviceArchConfig> m_arch{};
  std::optional<HtpDeviceSignedPdConfig> m_useSignedProcessDomain{};
  std::optional<HtpDeviceSecurePdConfig> m_useSecureProcessDomain{};

  mutable std::vector<HtpDeviceCustomConfig> m_customCfgs;

 public:
  std::vector<QairtBackend_CustomConfigHandle_t> getCustomConfigs() const override {
    std::vector<QairtBackend_CustomConfigHandle_t> toret;
    m_customCfgs = buildCustomConfigsVector();
    toret.reserve(m_customCfgs.size());
    for (auto& e : m_customCfgs) {
      toret.push_back(e.getCustomConfigHandle());
    }
    return toret;
  }

  using base_type::base_type;

  // SoC Model
  void clearSocModel() { m_socModel.reset(); }
  void setSocModel(uint32_t socModel) { m_socModel = socModel; }
  std::optional<uint32_t>& getSocModel() { return m_socModel; }
  const std::optional<uint32_t>& getSocModel() const { return m_socModel; }

  // Minimum Architecture
  void clearArch() { m_arch.reset(); }
  void setArch(uint32_t deviceId, QairtHtpDevice_Arch_t arch) {
    m_arch = HtpDeviceArchConfig{deviceId, arch};
  }
  std::optional<HtpDeviceArchConfig>& getArch() { return m_arch; }
  const std::optional<HtpDeviceArchConfig>& getArch() const { return m_arch; }

  // Use Signed Process Domain
  void clearUseSignedProcessDomain() { m_useSignedProcessDomain.reset(); }
  void setUseSignedProcessDomain(uint32_t deviceId, bool useSignedProcessDomain) {
    m_useSignedProcessDomain = HtpDeviceSignedPdConfig{deviceId, useSignedProcessDomain};
  }
  std::optional<HtpDeviceSignedPdConfig>& getUseSignedProcessDomain() {
    return m_useSignedProcessDomain;
  }
  const std::optional<HtpDeviceSignedPdConfig>& getUseSignedProcessDomain() const {
    return m_useSignedProcessDomain;
  }

  // Use Secure Process Domain
  void clearUseSecureProcessDomain() { m_useSecureProcessDomain.reset(); }
  void setUseSecureProcessDomain(uint32_t deviceId, bool useSecureProcessDomain) {
    m_useSecureProcessDomain = HtpDeviceSecurePdConfig{deviceId, useSecureProcessDomain};
  }
  std::optional<HtpDeviceSecurePdConfig>& getUseSecureProcessDomain() {
    return m_useSecureProcessDomain;
  }
  const std::optional<HtpDeviceSecurePdConfig>& getUseSecureProcessDomain() const {
    return m_useSecureProcessDomain;
  }

 private:
  std::vector<HtpDeviceCustomConfig> buildCustomConfigsVector() const {
    if (!m_backendExtensionTable.lock()) {
      throw std::runtime_error("HtpDeviceCustomConfiguration has no API table");
    }
    std::vector<HtpDeviceCustomConfig> toret;
    if (m_socModel.has_value()) {
      HtpDeviceCustomConfig& back = toret.emplace_back(make<HtpDeviceCustomConfig>());
      back.setSocModel(m_socModel.value());
    }
    if (m_arch.has_value()) {
      HtpDeviceCustomConfig& back = toret.emplace_back(make<HtpDeviceCustomConfig>());
      back.setArch(m_arch.value().deviceId, m_arch.value().arch);
    }
    if (m_useSignedProcessDomain.has_value()) {
      HtpDeviceCustomConfig& back = toret.emplace_back(make<HtpDeviceCustomConfig>());
      back.setUseSignedProcessDomain(m_useSignedProcessDomain.value().deviceId,
                                     m_useSignedProcessDomain.value().useSignedProcessDomain);
    }
    if (m_useSecureProcessDomain.has_value()) {
      HtpDeviceCustomConfig& back = toret.emplace_back(make<HtpDeviceCustomConfig>());
      back.setUseSecureProcessDomain(m_useSecureProcessDomain.value().deviceId,
                                     m_useSecureProcessDomain.value().useSecureProcessDomain);
    }
    return toret;
  }
};

}  // namespace qairt

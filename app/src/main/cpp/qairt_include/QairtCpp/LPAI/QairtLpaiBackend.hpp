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

#include "LPAI/QairtLpai.hpp"
#include "LPAI/QairtLpaiBackend.h"
#include "QairtCppApi/QairtBackend.hpp"

namespace qairt {

class LpaiBackendHwInfoConfig
    : public LpaiApiType<LpaiBackendHwInfoConfig, QairtLpaiBackend_HwInfoV1_t> {
  friend detail::TempWorkaround;
  template <typename>
  friend class BackendExtension;

 public:
  LpaiBackendHwInfoConfig()                                              = default;
  LpaiBackendHwInfoConfig(const LpaiBackendHwInfoConfig&)                = delete;
  LpaiBackendHwInfoConfig(LpaiBackendHwInfoConfig&&) noexcept            = default;
  LpaiBackendHwInfoConfig& operator=(const LpaiBackendHwInfoConfig&)     = delete;
  LpaiBackendHwInfoConfig& operator=(LpaiBackendHwInfoConfig&&) noexcept = default;

  // Target
  void setTarget(QairtLpaiBackend_Target_t target) {
    callSetter<&interface_type::setTarget>(target);
  }
  QairtLpaiBackend_Target_t getTarget() const { return callGetter<&interface_type::getTarget>(); }

  // HW Version
  void setHwVersion(QairtLpaiBackend_HwVersion_t hwVersion) {
    callSetter<&interface_type::setHwVersion>(hwVersion);
  }
  QairtLpaiBackend_HwVersion_t getHwVersion() const {
    return callGetter<&interface_type::getHwVersion>();
  }

 private:
  static std::shared_ptr<api_table_type> lockOrThrow(
      const std::weak_ptr<api_table_type>& apiTable) {
    auto locked = apiTable.lock();
    if (!locked) throw std::runtime_error("Failed to lock apiTable");
    return locked;
  }

  LpaiBackendHwInfoConfig(const std::weak_ptr<api_table_type>& apiTable)
      : ApiType(lockOrThrow(apiTable)) {
    construct();
    if (!getHandle()) throw std::runtime_error("Failed to construct LpaiBackendHwInfoConfig");
  }
};

class LpaiBackendCustomConfig
    : public LpaiApiType<LpaiBackendCustomConfig, QairtLpaiBackend_CustomConfigV1_t>,
      public BackendCustomConfig {
  friend detail::TempWorkaround;
  template <typename>
  friend class BackendExtension;

  detail::crossable<detail::non_owning<LpaiBackendHwInfoConfig>,
                    &interface_type::getHwInfo,
                    &interface_type::setHwInfo>
      m_hwInfo;

 public:
  QairtBackend_CustomConfigHandle_t getCustomConfigHandle() const override { return getHandle(); }

  LpaiBackendCustomConfig()                                              = default;
  LpaiBackendCustomConfig(const LpaiBackendCustomConfig&)                = delete;
  LpaiBackendCustomConfig(LpaiBackendCustomConfig&&) noexcept            = default;
  LpaiBackendCustomConfig& operator=(const LpaiBackendCustomConfig&)     = delete;
  LpaiBackendCustomConfig& operator=(LpaiBackendCustomConfig&&) noexcept = default;

  void setHwInfo(const LpaiBackendHwInfoConfig& hwInfo) { m_hwInfo.setNonOwning(*this, hwInfo); }
  LpaiBackendHwInfoConfig& getHwInfo() { return m_hwInfo.bindNonOwning(*this); }

 private:
  static std::shared_ptr<api_table_type> lockOrThrow(
      const std::weak_ptr<api_table_type>& apiTable) {
    auto locked = apiTable.lock();
    if (!locked) throw std::runtime_error("Failed to lock apiTable");
    return locked;
  }

  LpaiBackendCustomConfig(const std::weak_ptr<api_table_type>& apiTable)
      : ApiType(lockOrThrow(apiTable)) {
    construct();
    if (!getHandle()) throw std::runtime_error("Failed to construct LpaiBackendCustomConfig");
  }
};

class LpaiBackendCustomConfiguration : public detail::TabularBEType<LpaiBackendExtensionTable>,
                                       public BackendCustomConfiguration {
  std::optional<QairtLpaiBackend_Target_t> m_target{};
  std::optional<QairtLpaiBackend_HwVersion_t> m_hwVersion{};

  mutable std::vector<LpaiBackendCustomConfig> m_customCfgs;

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

  // Target
  void clearTarget() { m_target.reset(); }
  void setTarget(QairtLpaiBackend_Target_t target) { m_target = target; }
  std::optional<QairtLpaiBackend_Target_t>& getTarget() { return m_target; }
  const std::optional<QairtLpaiBackend_Target_t>& getTarget() const { return m_target; }

  // HW Version
  void clearHwVersion() { m_hwVersion.reset(); }
  void setHwVersion(QairtLpaiBackend_HwVersion_t hwVersion) { m_hwVersion = hwVersion; }
  std::optional<QairtLpaiBackend_HwVersion_t>& getHwVersion() { return m_hwVersion; }
  const std::optional<QairtLpaiBackend_HwVersion_t>& getHwVersion() const { return m_hwVersion; }

 private:
  std::vector<LpaiBackendCustomConfig> buildCustomConfigsVector() const {
    if (!m_backendExtensionTable.lock()) {
      throw std::runtime_error("LpaiBackendCustomConfiguration has no API table");
    }
    std::vector<LpaiBackendCustomConfig> toret;
    // Both target and hwVersion are fields of the single HW_INFO option.
    // If either is set, create one custom config with both values applied.
    if (m_target.has_value() || m_hwVersion.has_value()) {
      LpaiBackendCustomConfig& cfg   = toret.emplace_back(make<LpaiBackendCustomConfig>());
      LpaiBackendHwInfoConfig hwInfo = make<LpaiBackendHwInfoConfig>();
      if (m_target.has_value()) {
        hwInfo.setTarget(m_target.value());
      }
      if (m_hwVersion.has_value()) {
        hwInfo.setHwVersion(m_hwVersion.value());
      }
      cfg.setHwInfo(hwInfo);
    }
    return toret;
  }
};

}  // namespace qairt

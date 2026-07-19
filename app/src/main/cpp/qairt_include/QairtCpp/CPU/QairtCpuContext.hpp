//==============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//==============================================================================
#pragma once

#include <optional>
#include <vector>

#include "CPU/QairtCpu.hpp"
#include "CPU/QairtCpuContext.h"
#include "QairtCppApi/QairtContext.hpp"

namespace qairt {

class CpuContextCustomConfig
    : public CpuApiType<CpuContextCustomConfig, QairtCpuContext_CustomConfigV1_t>,
      public ContextCustomConfig {
  friend class ApiType<CpuContextCustomConfig,
                       QairtCpuContext_CustomConfigV1_t,
                       CpuBackendExtensionTable>;
  friend detail::TempWorkaround;
  template <typename>
  friend class BackendExtension;

 public:
  QairtContext_CustomConfigHandle_t getCustomConfigHandle() const override { return getHandle(); }

  CpuContextCustomConfig()                                             = default;
  CpuContextCustomConfig(const CpuContextCustomConfig&)                = delete;
  CpuContextCustomConfig(CpuContextCustomConfig&&) noexcept            = default;
  CpuContextCustomConfig& operator=(const CpuContextCustomConfig&)     = delete;
  CpuContextCustomConfig& operator=(CpuContextCustomConfig&&) noexcept = default;

  // Use QMX
  void setUseQmx(bool enable) {
    callSetter<&interface_type::setUseQmx>(static_cast<uint8_t>(enable));
  }
  bool getUseQmx() const { return static_cast<bool>(callGetter<&interface_type::getUseQmx>()); }

 private:
  CpuContextCustomConfig(const std::weak_ptr<api_table_type>& apiTable) : ApiType(apiTable.lock()) {
    if (!apiTable.lock()) {
      throw std::runtime_error("Failed to lock apiTable");
    }
    construct();
    if (!getHandle()) {
      throw std::runtime_error("Somehow failed to construct?");
    }
  }
};

class CpuContextCustomConfiguration : public detail::TabularBEType<CpuBackendExtensionTable>,
                                      public ContextCustomConfiguration {
  std::optional<bool> m_useQmx{};

  mutable std::vector<CpuContextCustomConfig> m_customCfgs;
  friend Context;

 public:
  std::vector<QairtContext_CustomConfigHandle_t> getCustomConfigs() const override {
    std::vector<QairtContext_CustomConfigHandle_t> toret;
    m_customCfgs = buildCustomConfigsVector();
    toret.reserve(m_customCfgs.size());
    for (auto& e : m_customCfgs) {
      toret.push_back(e.getCustomConfigHandle());
    }
    return toret;
  }

  using base_type::base_type;

  // Use QMX
  void clearUseQmx() { m_useQmx.reset(); }
  void setUseQmx(bool enable) { m_useQmx = enable; }
  std::optional<bool>& getUseQmx() { return m_useQmx; }
  const std::optional<bool>& getUseQmx() const { return m_useQmx; }

 private:
  std::vector<CpuContextCustomConfig> buildCustomConfigsVector() const {
    if (!m_backendExtensionTable.lock()) {
      throw std::runtime_error("CpuContextCustomConfiguration has no API table");
    }
    std::vector<CpuContextCustomConfig> toret;
    if (m_useQmx.has_value()) {
      CpuContextCustomConfig& back = toret.emplace_back(make<CpuContextCustomConfig>());
      back.setUseQmx(m_useQmx.value());
    }
    return toret;
  }
};

}  // namespace qairt
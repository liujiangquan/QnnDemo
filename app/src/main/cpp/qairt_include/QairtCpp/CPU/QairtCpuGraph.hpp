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
#include "CPU/QairtCpuGraph.h"
#include "QairtCppApi/QairtGraph.hpp"

namespace qairt {

class CpuGraphCustomConfig
    : public CpuApiType<CpuGraphCustomConfig, QairtCpuGraph_CustomConfigV1_t>,
      public GraphCustomConfig {
  friend class ApiType<CpuGraphCustomConfig,
                       QairtCpuGraph_CustomConfigV1_t,
                       CpuBackendExtensionTable>;
  friend detail::TempWorkaround;
  template <typename>
  friend class BackendExtension;

 public:
  QairtGraph_CustomConfigHandle_t getCustomConfigHandle() const override { return getHandle(); }

  CpuGraphCustomConfig()                                           = default;
  CpuGraphCustomConfig(const CpuGraphCustomConfig&)                = delete;
  CpuGraphCustomConfig(CpuGraphCustomConfig&&) noexcept            = default;
  CpuGraphCustomConfig& operator=(const CpuGraphCustomConfig&)     = delete;
  CpuGraphCustomConfig& operator=(CpuGraphCustomConfig&&) noexcept = default;

  // Use QMX
  void setUseQmx(bool enable) {
    callSetter<&interface_type::setUseQmx>(static_cast<uint8_t>(enable));
  }
  bool getUseQmx() const { return static_cast<bool>(callGetter<&interface_type::getUseQmx>()); }

 private:
  CpuGraphCustomConfig(const std::weak_ptr<api_table_type>& apiTable) : ApiType(apiTable.lock()) {
    if (!apiTable.lock()) {
      throw std::runtime_error("Failed to lock apiTable");
    }
    construct();
    if (!getHandle()) {
      throw std::runtime_error("Failed to construct");
    }
  }
};

class CpuGraphCustomConfiguration : public detail::TabularBEType<CpuBackendExtensionTable>,
                                    public GraphCustomConfiguration {
  std::optional<bool> m_useQmx{};

  mutable std::vector<CpuGraphCustomConfig> m_customCfgs;

 public:
  std::vector<QairtGraph_CustomConfigHandle_t> getCustomConfigs() const override {
    std::vector<QairtGraph_CustomConfigHandle_t> toret;
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
  std::vector<CpuGraphCustomConfig> buildCustomConfigsVector() const {
    if (!m_backendExtensionTable.lock()) {
      throw std::runtime_error("CpuGraphCustomConfiguration has no API table");
    }
    std::vector<CpuGraphCustomConfig> toret;
    if (m_useQmx.has_value()) {
      CpuGraphCustomConfig& back = toret.emplace_back(make<CpuGraphCustomConfig>());
      back.setUseQmx(m_useQmx.value());
    }
    return toret;
  }
};

}  // namespace qairt
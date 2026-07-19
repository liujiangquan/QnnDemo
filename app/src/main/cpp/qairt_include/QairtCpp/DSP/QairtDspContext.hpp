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

#include "QairtCppApi/QairtContext.hpp"
#include "QairtDsp.hpp"
#include "QairtDspContext.h"

namespace qairt {

class DspContextCustomConfig
    : public DspApiType<DspContextCustomConfig, QairtDspContext_CustomConfigV1_t>,
      public ContextCustomConfig {
  friend detail::TempWorkaround;
  template <typename>
  friend class BackendExtension;

 public:
  QairtContext_CustomConfigHandle_t getCustomConfigHandle() const override { return getHandle(); }

  DspContextCustomConfig()                                             = default;
  DspContextCustomConfig(const DspContextCustomConfig&)                = delete;
  DspContextCustomConfig(DspContextCustomConfig&&) noexcept            = default;
  DspContextCustomConfig& operator=(const DspContextCustomConfig&)     = delete;
  DspContextCustomConfig& operator=(DspContextCustomConfig&&) noexcept = default;

  void setFileReadMemoryBudget(uint32_t budgetInMb) {
    callSetter<&interface_type::setFileReadMemoryBudget>(budgetInMb);
  }
  uint32_t getFileReadMemoryBudget() const {
    return callGetter<&interface_type::getFileReadMemoryBudget>();
  }

 private:
  DspContextCustomConfig(const std::weak_ptr<api_table_type>& apiTable) : ApiType(apiTable.lock()) {
    if (!apiTable.lock()) {
      throw std::runtime_error("Failed to lock apiTable");
    }
    construct();
    if (!getHandle()) {
      throw std::runtime_error("Failed to construct DspContextCustomConfig");
    }
  }
};

class DspContextCustomConfiguration : public detail::TabularBEType<DspBackendExtensionTable>,
                                      public ContextCustomConfiguration {
  std::optional<uint32_t> m_fileReadMemoryBudgetInMb{};

  mutable std::vector<DspContextCustomConfig> m_customCfgs;
  friend Context;

 public:
  std::vector<QairtContext_CustomConfigHandle_t> getCustomConfigs() const override {
    std::vector<QairtContext_CustomConfigHandle_t> toret;
    m_customCfgs = buildCustomConfigs();
    toret.reserve(m_customCfgs.size());
    for (auto& cfg : m_customCfgs) {
      toret.push_back(cfg.getCustomConfigHandle());
    }
    return toret;
  }

  std::vector<DspContextCustomConfig> buildCustomConfigs() const {
    if (!m_backendExtensionTable.lock()) {
      throw std::runtime_error("Malformed DspContextCustomConfiguration: missing API table.");
    }
    std::vector<DspContextCustomConfig> toret;

    if (m_fileReadMemoryBudgetInMb.has_value()) {
      DspContextCustomConfig& cfg = toret.emplace_back(make<DspContextCustomConfig>());
      cfg.setFileReadMemoryBudget(m_fileReadMemoryBudgetInMb.value());
    }

    return toret;
  }

 public:
  using base_type::base_type;

  // File Read Memory Budget
  void clearFileReadMemoryBudget() { m_fileReadMemoryBudgetInMb.reset(); }
  void setFileReadMemoryBudget(uint32_t budgetInMb) { m_fileReadMemoryBudgetInMb = budgetInMb; }
  std::optional<uint32_t>& getFileReadMemoryBudget() { return m_fileReadMemoryBudgetInMb; }
  const std::optional<uint32_t>& getFileReadMemoryBudget() const {
    return m_fileReadMemoryBudgetInMb;
  }
};

}  // namespace qairt

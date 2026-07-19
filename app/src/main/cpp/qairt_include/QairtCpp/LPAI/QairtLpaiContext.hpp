//=============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//=============================================================================
#pragma once

#include <optional>
#include <string>
#include <vector>

#include "LPAI/QairtLpai.hpp"
#include "LPAI/QairtLpaiContext.h"
#include "QairtCppApi/QairtContext.hpp"

namespace qairt {

class LpaiContextCustomConfig
    : public LpaiApiType<LpaiContextCustomConfig, QairtLpaiContext_CustomConfigV1_t>,
      public ContextCustomConfig {
  friend detail::TempWorkaround;
  template <typename>
  friend class BackendExtension;

 public:
  QairtContext_CustomConfigHandle_t getCustomConfigHandle() const override { return getHandle(); }

  LpaiContextCustomConfig()                                              = default;
  LpaiContextCustomConfig(const LpaiContextCustomConfig&)                = delete;
  LpaiContextCustomConfig(LpaiContextCustomConfig&&) noexcept            = default;
  LpaiContextCustomConfig& operator=(const LpaiContextCustomConfig&)     = delete;
  LpaiContextCustomConfig& operator=(LpaiContextCustomConfig&&) noexcept = default;

  // Model Buffer Mem Type
  void setModelBufferMemType(QairtLpai_MemType_t memType) {
    callSetter<&interface_type::setModelBufferMemType>(memType);
  }
  QairtLpai_MemType_t getModelBufferMemType() const {
    return callGetter<&interface_type::getModelBufferMemType>();
  }

  // Enable Island (graphName=nullptr for context-level; non-null for graph-level)
  void setEnableIsland(const char* graphName) {
    callSetter<&interface_type::setEnableIsland>(graphName);
  }
  const char* getEnableIsland() const { return callGetter<&interface_type::getEnableIsland>(); }

 private:
  static std::shared_ptr<api_table_type> lockOrThrow(
      const std::weak_ptr<api_table_type>& apiTable) {
    auto locked = apiTable.lock();
    if (!locked) throw std::runtime_error("Failed to lock apiTable");
    return locked;
  }

  LpaiContextCustomConfig(const std::weak_ptr<api_table_type>& apiTable)
      : ApiType(lockOrThrow(apiTable)) {
    construct();
    if (!getHandle()) throw std::runtime_error("Failed to construct LpaiContextCustomConfig");
  }
};

class LpaiContextCustomConfiguration : public detail::TabularBEType<LpaiBackendExtensionTable>,
                                       public ContextCustomConfiguration {
  std::optional<QairtLpai_MemType_t> m_modelBufferMemType{};
  // Context-level island: m_islandContextLevel = true, m_islandGraphNames empty
  // Graph-level island: m_islandContextLevel = false, m_islandGraphNames has entries
  std::optional<bool> m_islandContextLevel{};
  std::vector<std::string> m_islandGraphNames;

  mutable std::vector<LpaiContextCustomConfig> m_customCfgs;
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

  // Model Buffer Mem Type
  void clearModelBufferMemType() { m_modelBufferMemType.reset(); }
  void setModelBufferMemType(QairtLpai_MemType_t memType) { m_modelBufferMemType = memType; }
  std::optional<QairtLpai_MemType_t>& getModelBufferMemType() { return m_modelBufferMemType; }
  const std::optional<QairtLpai_MemType_t>& getModelBufferMemType() const {
    return m_modelBufferMemType;
  }

  // Context-level island (all graphs run in island mode)
  void clearIsland() {
    m_islandContextLevel.reset();
    m_islandGraphNames.clear();
  }
  void setEnableIslandContextLevel() {
    m_islandContextLevel = true;
    m_islandGraphNames.clear();
  }

  // Graph-level island (specific graph runs in island mode)
  void addEnableIslandForGraph(std::string graphName) {
    m_islandContextLevel = false;
    m_islandGraphNames.push_back(std::move(graphName));
  }

  const std::optional<bool>& getIslandContextLevel() const { return m_islandContextLevel; }
  const std::vector<std::string>& getIslandGraphNames() const { return m_islandGraphNames; }

 private:
  std::vector<LpaiContextCustomConfig> buildCustomConfigsVector() const {
    if (!m_backendExtensionTable.lock()) {
      throw std::runtime_error("LpaiContextCustomConfiguration has no API table");
    }
    std::vector<LpaiContextCustomConfig> toret;
    if (m_modelBufferMemType.has_value()) {
      LpaiContextCustomConfig& cfg = toret.emplace_back(make<LpaiContextCustomConfig>());
      cfg.setModelBufferMemType(m_modelBufferMemType.value());
    }
    if (m_islandContextLevel.has_value()) {
      if (m_islandContextLevel.value()) {
        // Context-level island: one config with graphName=nullptr
        LpaiContextCustomConfig& cfg = toret.emplace_back(make<LpaiContextCustomConfig>());
        cfg.setEnableIsland(nullptr);
      } else {
        // Graph-level island: one config per graph name
        for (const auto& graphName : m_islandGraphNames) {
          LpaiContextCustomConfig& cfg = toret.emplace_back(make<LpaiContextCustomConfig>());
          cfg.setEnableIsland(graphName.c_str());
        }
      }
    }
    return toret;
  }
};

}  // namespace qairt

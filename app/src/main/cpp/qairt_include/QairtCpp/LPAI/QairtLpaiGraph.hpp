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
#include "LPAI/QairtLpaiGraph.h"
#include "QairtCppApi/QairtGraph.hpp"

namespace qairt {

//=============================================================================
// LpaiGraphMemConfig — C++ wrapper for the memory config handle
//=============================================================================

class LpaiGraphMemConfig : public LpaiApiType<LpaiGraphMemConfig, QairtLpaiGraph_MemConfigV1_t> {
  friend detail::TempWorkaround;
  template <typename>
  friend class BackendExtension;

 public:
  LpaiGraphMemConfig()                                         = default;
  LpaiGraphMemConfig(const LpaiGraphMemConfig&)                = delete;
  LpaiGraphMemConfig(LpaiGraphMemConfig&&) noexcept            = default;
  LpaiGraphMemConfig& operator=(const LpaiGraphMemConfig&)     = delete;
  LpaiGraphMemConfig& operator=(LpaiGraphMemConfig&&) noexcept = default;

  void setMemType(QairtLpai_MemType_t memType) { callSetter<&interface_type::setMemType>(memType); }
  QairtLpai_MemType_t getMemType() const { return callGetter<&interface_type::getMemType>(); }

  void setSize(uint32_t size) { callSetter<&interface_type::setSize>(size); }
  uint32_t getSize() const { return callGetter<&interface_type::getSize>(); }

  void setAddr(void* addr) { callSetter<&interface_type::setAddr>(addr); }
  void* getAddr() const { return callGetter<&interface_type::getAddr>(); }

 private:
  static std::shared_ptr<api_table_type> lockOrThrow(
      const std::weak_ptr<api_table_type>& apiTable) {
    auto locked = apiTable.lock();
    if (!locked) throw std::runtime_error("Failed to lock apiTable");
    return locked;
  }

  LpaiGraphMemConfig(const std::weak_ptr<api_table_type>& apiTable)
      : ApiType(lockOrThrow(apiTable)) {
    construct();
    if (!getHandle()) throw std::runtime_error("Failed to construct LpaiGraphMemConfig");
  }
};

//=============================================================================
// LpaiGraphPerfCfg — C++ wrapper for the perf config handle
//=============================================================================

class LpaiGraphPerfCfg : public LpaiApiType<LpaiGraphPerfCfg, QairtLpaiGraph_PerfCfgV1_t> {
  friend detail::TempWorkaround;
  template <typename>
  friend class BackendExtension;

 public:
  LpaiGraphPerfCfg()                                       = default;
  LpaiGraphPerfCfg(const LpaiGraphPerfCfg&)                = delete;
  LpaiGraphPerfCfg(LpaiGraphPerfCfg&&) noexcept            = default;
  LpaiGraphPerfCfg& operator=(const LpaiGraphPerfCfg&)     = delete;
  LpaiGraphPerfCfg& operator=(LpaiGraphPerfCfg&&) noexcept = default;

  void setFps(uint32_t fps) { callSetter<&interface_type::setFps>(fps); }
  uint32_t getFps() const { return callGetter<&interface_type::getFps>(); }

  void setFtrtRatio(uint32_t ftrtRatio) { callSetter<&interface_type::setFtrtRatio>(ftrtRatio); }
  uint32_t getFtrtRatio() const { return callGetter<&interface_type::getFtrtRatio>(); }

  void setClientType(QairtLpaiGraph_ClientPerfType_t clientType) {
    callSetter<&interface_type::setClientType>(clientType);
  }
  QairtLpaiGraph_ClientPerfType_t getClientType() const {
    return callGetter<&interface_type::getClientType>();
  }

 private:
  static std::shared_ptr<api_table_type> lockOrThrow(
      const std::weak_ptr<api_table_type>& apiTable) {
    auto locked = apiTable.lock();
    if (!locked) throw std::runtime_error("Failed to lock apiTable");
    return locked;
  }

  LpaiGraphPerfCfg(const std::weak_ptr<api_table_type>& apiTable) : ApiType(lockOrThrow(apiTable)) {
    construct();
    if (!getHandle()) throw std::runtime_error("Failed to construct LpaiGraphPerfCfg");
  }
};

//=============================================================================
// LpaiGraphCoreAffinity — C++ wrapper for the core affinity handle
//=============================================================================

class LpaiGraphCoreAffinity
    : public LpaiApiType<LpaiGraphCoreAffinity, QairtLpaiGraph_CoreAffinityV1_t> {
  friend detail::TempWorkaround;
  template <typename>
  friend class BackendExtension;

 public:
  LpaiGraphCoreAffinity()                                            = default;
  LpaiGraphCoreAffinity(const LpaiGraphCoreAffinity&)                = delete;
  LpaiGraphCoreAffinity(LpaiGraphCoreAffinity&&) noexcept            = default;
  LpaiGraphCoreAffinity& operator=(const LpaiGraphCoreAffinity&)     = delete;
  LpaiGraphCoreAffinity& operator=(LpaiGraphCoreAffinity&&) noexcept = default;

  void setAffinity(QairtLpaiGraph_CoreAffinityType_t affinity) {
    callSetter<&interface_type::setAffinity>(affinity);
  }
  QairtLpaiGraph_CoreAffinityType_t getAffinity() const {
    return callGetter<&interface_type::getAffinity>();
  }

  void setCoreSelection(uint32_t coreSelection) {
    callSetter<&interface_type::setCoreSelection>(coreSelection);
  }
  uint32_t getCoreSelection() const { return callGetter<&interface_type::getCoreSelection>(); }

 private:
  static std::shared_ptr<api_table_type> lockOrThrow(
      const std::weak_ptr<api_table_type>& apiTable) {
    auto locked = apiTable.lock();
    if (!locked) throw std::runtime_error("Failed to lock apiTable");
    return locked;
  }

  LpaiGraphCoreAffinity(const std::weak_ptr<api_table_type>& apiTable)
      : ApiType(lockOrThrow(apiTable)) {
    construct();
    if (!getHandle()) throw std::runtime_error("Failed to construct LpaiGraphCoreAffinity");
  }
};

//=============================================================================
// LpaiGraphPrepareCfg — C++ wrapper for the prepare config handle
//=============================================================================

class LpaiGraphPrepareCfg : public LpaiApiType<LpaiGraphPrepareCfg, QairtLpaiGraph_PrepareCfgV1_t> {
  friend detail::TempWorkaround;
  template <typename>
  friend class BackendExtension;

 public:
  LpaiGraphPrepareCfg()                                          = default;
  LpaiGraphPrepareCfg(const LpaiGraphPrepareCfg&)                = delete;
  LpaiGraphPrepareCfg(LpaiGraphPrepareCfg&&) noexcept            = default;
  LpaiGraphPrepareCfg& operator=(const LpaiGraphPrepareCfg&)     = delete;
  LpaiGraphPrepareCfg& operator=(LpaiGraphPrepareCfg&&) noexcept = default;

  void setEnablePerLayer(uint32_t enablePerLayer) {
    callSetter<&interface_type::setEnablePerLayer>(enablePerLayer);
  }
  uint32_t getEnablePerLayer() const { return callGetter<&interface_type::getEnablePerLayer>(); }

  void setEnableCoreSelection(const char* enableCoreSelection) {
    callSetter<&interface_type::setEnableCoreSelection>(enableCoreSelection);
  }
  const char* getEnableCoreSelection() const {
    return callGetter<&interface_type::getEnableCoreSelection>();
  }

 private:
  static std::shared_ptr<api_table_type> lockOrThrow(
      const std::weak_ptr<api_table_type>& apiTable) {
    auto locked = apiTable.lock();
    if (!locked) throw std::runtime_error("Failed to lock apiTable");
    return locked;
  }

  LpaiGraphPrepareCfg(const std::weak_ptr<api_table_type>& apiTable)
      : ApiType(lockOrThrow(apiTable)) {
    construct();
    if (!getHandle()) throw std::runtime_error("Failed to construct LpaiGraphPrepareCfg");
  }
};

//=============================================================================
// LpaiGraphCustomConfig — C++ wrapper for the graph custom config handle
//=============================================================================

class LpaiGraphCustomConfig
    : public LpaiApiType<LpaiGraphCustomConfig, QairtLpaiGraph_CustomConfigV1_t>,
      public GraphCustomConfig {
  friend detail::TempWorkaround;
  template <typename>
  friend class BackendExtension;

  detail::crossable<detail::non_owning<LpaiGraphMemConfig>,
                    &interface_type::getScratchMem,
                    &interface_type::setScratchMem>
      m_scratchMem;
  detail::crossable<detail::non_owning<LpaiGraphMemConfig>,
                    &interface_type::getPersistentMem,
                    &interface_type::setPersistentMem>
      m_persistentMem;
  detail::crossable<detail::non_owning<LpaiGraphPerfCfg>,
                    &interface_type::getPerfCfg,
                    &interface_type::setPerfCfg>
      m_perfCfg;
  detail::crossable<detail::non_owning<LpaiGraphCoreAffinity>,
                    &interface_type::getCoreAffinity,
                    &interface_type::setCoreAffinity>
      m_coreAffinity;
  detail::crossable<detail::non_owning<LpaiGraphPrepareCfg>,
                    &interface_type::getPrepareCfg,
                    &interface_type::setPrepareCfg>
      m_prepareCfg;

 public:
  QairtGraph_CustomConfigHandle_t getCustomConfigHandle() const override { return getHandle(); }

  LpaiGraphCustomConfig()                                            = default;
  LpaiGraphCustomConfig(const LpaiGraphCustomConfig&)                = delete;
  LpaiGraphCustomConfig(LpaiGraphCustomConfig&&) noexcept            = default;
  LpaiGraphCustomConfig& operator=(const LpaiGraphCustomConfig&)     = delete;
  LpaiGraphCustomConfig& operator=(LpaiGraphCustomConfig&&) noexcept = default;

  // Scratch Mem
  void setScratchMem(const LpaiGraphMemConfig& memConfig) {
    m_scratchMem.setNonOwning(*this, memConfig);
  }
  LpaiGraphMemConfig& getScratchMem() { return m_scratchMem.bindNonOwning(*this); }

  // Persistent Mem
  void setPersistentMem(const LpaiGraphMemConfig& memConfig) {
    m_persistentMem.setNonOwning(*this, memConfig);
  }
  LpaiGraphMemConfig& getPersistentMem() { return m_persistentMem.bindNonOwning(*this); }

  // Perf Cfg
  void setPerfCfg(const LpaiGraphPerfCfg& perfCfg) { m_perfCfg.setNonOwning(*this, perfCfg); }
  LpaiGraphPerfCfg& getPerfCfg() { return m_perfCfg.bindNonOwning(*this); }

  // Core Affinity
  void setCoreAffinity(const LpaiGraphCoreAffinity& coreAffinity) {
    m_coreAffinity.setNonOwning(*this, coreAffinity);
  }
  LpaiGraphCoreAffinity& getCoreAffinity() { return m_coreAffinity.bindNonOwning(*this); }

  // Pause / Resume Execution
  void setPauseExecution() { callSetter<&interface_type::setPauseExecution>(); }
  void setResumeExecution() { callSetter<&interface_type::setResumeExecution>(); }

  // Prepare Cfg
  void setPrepareCfg(const LpaiGraphPrepareCfg& prepareCfg) {
    m_prepareCfg.setNonOwning(*this, prepareCfg);
  }
  LpaiGraphPrepareCfg& getPrepareCfg() { return m_prepareCfg.bindNonOwning(*this); }

 private:
  static std::shared_ptr<api_table_type> lockOrThrow(
      const std::weak_ptr<api_table_type>& apiTable) {
    auto locked = apiTable.lock();
    if (!locked) throw std::runtime_error("Failed to lock apiTable");
    return locked;
  }

  LpaiGraphCustomConfig(const std::weak_ptr<api_table_type>& apiTable)
      : ApiType(lockOrThrow(apiTable)) {
    construct();
    if (!getHandle()) throw std::runtime_error("Failed to construct LpaiGraphCustomConfig");
  }
};

//=============================================================================
// LpaiGraphCustomConfiguration tabular builder
//=============================================================================

class LpaiGraphCustomConfiguration : public detail::TabularBEType<LpaiBackendExtensionTable>,
                                     public GraphCustomConfiguration {
  std::optional<LpaiGraphMemConfig> m_scratchMem{};
  std::optional<LpaiGraphMemConfig> m_persistentMem{};
  std::optional<LpaiGraphPerfCfg> m_perfCfg{};
  std::optional<LpaiGraphCoreAffinity> m_coreAffinity{};
  std::optional<bool> m_pauseExecution{};
  std::optional<LpaiGraphPrepareCfg> m_prepareCfg{};

  mutable std::vector<LpaiGraphCustomConfig> m_customCfgs;

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

  // Scratch Mem
  void clearScratchMem() { m_scratchMem.reset(); }
  void setScratchMem(LpaiGraphMemConfig mem) { m_scratchMem = std::move(mem); }
  std::optional<LpaiGraphMemConfig>& getScratchMem() { return m_scratchMem; }
  const std::optional<LpaiGraphMemConfig>& getScratchMem() const { return m_scratchMem; }

  // Persistent Mem
  void clearPersistentMem() { m_persistentMem.reset(); }
  void setPersistentMem(LpaiGraphMemConfig mem) { m_persistentMem = std::move(mem); }
  std::optional<LpaiGraphMemConfig>& getPersistentMem() { return m_persistentMem; }
  const std::optional<LpaiGraphMemConfig>& getPersistentMem() const { return m_persistentMem; }

  // Perf Cfg
  void clearPerfCfg() { m_perfCfg.reset(); }
  void setPerfCfg(LpaiGraphPerfCfg perf) { m_perfCfg = std::move(perf); }
  std::optional<LpaiGraphPerfCfg>& getPerfCfg() { return m_perfCfg; }
  const std::optional<LpaiGraphPerfCfg>& getPerfCfg() const { return m_perfCfg; }

  // Core Affinity
  void clearCoreAffinity() { m_coreAffinity.reset(); }
  void setCoreAffinity(LpaiGraphCoreAffinity aff) { m_coreAffinity = std::move(aff); }
  std::optional<LpaiGraphCoreAffinity>& getCoreAffinity() { return m_coreAffinity; }
  const std::optional<LpaiGraphCoreAffinity>& getCoreAffinity() const { return m_coreAffinity; }

  // Pause / Resume Execution
  void clearPauseResumeExecution() { m_pauseExecution.reset(); }
  void setPauseExecution() { m_pauseExecution = true; }
  void setResumeExecution() { m_pauseExecution = false; }
  const std::optional<bool>& getPauseExecution() const { return m_pauseExecution; }

  // Prepare Cfg
  void clearPrepareCfg() { m_prepareCfg.reset(); }
  void setPrepareCfg(LpaiGraphPrepareCfg prep) { m_prepareCfg = std::move(prep); }
  std::optional<LpaiGraphPrepareCfg>& getPrepareCfg() { return m_prepareCfg; }
  const std::optional<LpaiGraphPrepareCfg>& getPrepareCfg() const { return m_prepareCfg; }

 private:
  std::vector<LpaiGraphCustomConfig> buildCustomConfigsVector() const {
    if (!m_backendExtensionTable.lock()) {
      throw std::runtime_error("LpaiGraphCustomConfiguration has no API table");
    }
    std::vector<LpaiGraphCustomConfig> toret;

    if (m_scratchMem.has_value()) {
      LpaiGraphCustomConfig& cfg = toret.emplace_back(make<LpaiGraphCustomConfig>());
      cfg.setScratchMem(m_scratchMem.value());
    }
    if (m_persistentMem.has_value()) {
      LpaiGraphCustomConfig& cfg = toret.emplace_back(make<LpaiGraphCustomConfig>());
      cfg.setPersistentMem(m_persistentMem.value());
    }
    if (m_perfCfg.has_value()) {
      LpaiGraphCustomConfig& cfg = toret.emplace_back(make<LpaiGraphCustomConfig>());
      cfg.setPerfCfg(m_perfCfg.value());
    }
    if (m_coreAffinity.has_value()) {
      LpaiGraphCustomConfig& cfg = toret.emplace_back(make<LpaiGraphCustomConfig>());
      cfg.setCoreAffinity(m_coreAffinity.value());
    }
    if (m_pauseExecution.has_value()) {
      LpaiGraphCustomConfig& cfg = toret.emplace_back(make<LpaiGraphCustomConfig>());
      if (m_pauseExecution.value()) {
        cfg.setPauseExecution();
      } else {
        cfg.setResumeExecution();
      }
    }
    if (m_prepareCfg.has_value()) {
      LpaiGraphCustomConfig& cfg = toret.emplace_back(make<LpaiGraphCustomConfig>());
      cfg.setPrepareCfg(m_prepareCfg.value());
    }
    return toret;
  }
};

}  // namespace qairt

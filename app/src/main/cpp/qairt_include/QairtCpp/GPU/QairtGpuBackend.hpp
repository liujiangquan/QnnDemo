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

#include "GPU/QairtGpu.hpp"
#include "GPU/QairtGpuBackend.h"
#include "QairtCppApi/QairtBackend.hpp"

namespace qairt {

class GpuBackendCustomConfig
    : public GpuApiType<GpuBackendCustomConfig, QairtGpuBackend_CustomConfigV1_t>,
      public BackendCustomConfig {
  friend detail::TempWorkaround;
  template <typename>
  friend class BackendExtension;

 public:
  QairtBackend_CustomConfigHandle_t getCustomConfigHandle() const override { return getHandle(); }

  GpuBackendCustomConfig()                                             = default;
  GpuBackendCustomConfig(const GpuBackendCustomConfig&)                = delete;
  GpuBackendCustomConfig(GpuBackendCustomConfig&&) noexcept            = default;
  GpuBackendCustomConfig& operator=(const GpuBackendCustomConfig&)     = delete;
  GpuBackendCustomConfig& operator=(GpuBackendCustomConfig&&) noexcept = default;

  // Enable Tuning Mode
  void setEnableTuningMode(bool enable) {
    callSetter<&interface_type::setEnableTuningMode>(static_cast<uint8_t>(enable));
  }
  bool getEnableTuningMode() const {
    return static_cast<bool>(callGetter<&interface_type::getEnableTuningMode>());
  }

  // Performance Cache Dir
  void setPerformanceCacheDir(const char* dir) {
    callSetter<&interface_type::setPerformanceCacheDir>(dir);
  }
  const char* getPerformanceCacheDir() const {
    return callGetter<&interface_type::getPerformanceCacheDir>();
  }

  // Invalidate Performance Cache
  void setInvalidatePerformanceCache(bool invalidate) {
    callSetter<&interface_type::setInvalidatePerformanceCache>(static_cast<uint8_t>(invalidate));
  }
  bool getInvalidatePerformanceCache() const {
    return static_cast<bool>(callGetter<&interface_type::getInvalidatePerformanceCache>());
  }

  // Weight Sharing Enabled
  void setWeightSharingEnabled(bool enable) {
    callSetter<&interface_type::setWeightSharingEnabled>(static_cast<uint8_t>(enable));
  }
  bool getWeightSharingEnabled() const {
    return static_cast<bool>(callGetter<&interface_type::getWeightSharingEnabled>());
  }

  // Disable Kernel Profiling
  void setDisableKernelProfiling(bool disable) {
    callSetter<&interface_type::setDisableKernelProfiling>(static_cast<uint8_t>(disable));
  }
  bool getDisableKernelProfiling() const {
    return static_cast<bool>(callGetter<&interface_type::getDisableKernelProfiling>());
  }

 private:
  static std::shared_ptr<api_table_type> lockOrThrow(
      const std::weak_ptr<api_table_type>& apiTable) {
    auto locked = apiTable.lock();
    if (!locked) {
      throw std::runtime_error("Failed to lock apiTable");
    }
    return locked;
  }

  GpuBackendCustomConfig(const std::weak_ptr<api_table_type>& apiTable)
      : ApiType(lockOrThrow(apiTable)) {
    construct();
    if (!getHandle()) {
      throw std::runtime_error("Failed to construct");
    }
  }
};

class GpuBackendCustomConfiguration : public detail::TabularBEType<GpuBackendExtensionTable>,
                                      public BackendCustomConfiguration {
  std::optional<bool> m_enableTuningMode{};
  std::optional<std::string> m_performanceCacheDir{};
  std::optional<bool> m_invalidatePerformanceCache{};
  std::optional<bool> m_weightSharingEnabled{};
  std::optional<bool> m_disableKernelProfiling{};

  mutable std::vector<GpuBackendCustomConfig> m_customCfgs;

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

  // Enable Tuning Mode
  void clearEnableTuningMode() { m_enableTuningMode.reset(); }
  void setEnableTuningMode(bool enable) { m_enableTuningMode = enable; }
  std::optional<bool>& getEnableTuningMode() { return m_enableTuningMode; }
  const std::optional<bool>& getEnableTuningMode() const { return m_enableTuningMode; }

  // Performance Cache Dir
  void clearPerformanceCacheDir() { m_performanceCacheDir.reset(); }
  void setPerformanceCacheDir(std::string dir) { m_performanceCacheDir = std::move(dir); }
  std::optional<std::string>& getPerformanceCacheDir() { return m_performanceCacheDir; }
  const std::optional<std::string>& getPerformanceCacheDir() const { return m_performanceCacheDir; }

  // Invalidate Performance Cache
  void clearInvalidatePerformanceCache() { m_invalidatePerformanceCache.reset(); }
  void setInvalidatePerformanceCache(bool invalidate) { m_invalidatePerformanceCache = invalidate; }
  std::optional<bool>& getInvalidatePerformanceCache() { return m_invalidatePerformanceCache; }
  const std::optional<bool>& getInvalidatePerformanceCache() const {
    return m_invalidatePerformanceCache;
  }

  // Weight Sharing Enabled
  void clearWeightSharingEnabled() { m_weightSharingEnabled.reset(); }
  void setWeightSharingEnabled(bool enable) { m_weightSharingEnabled = enable; }
  std::optional<bool>& getWeightSharingEnabled() { return m_weightSharingEnabled; }
  const std::optional<bool>& getWeightSharingEnabled() const { return m_weightSharingEnabled; }

  // Disable Kernel Profiling
  void clearDisableKernelProfiling() { m_disableKernelProfiling.reset(); }
  void setDisableKernelProfiling(bool disable) { m_disableKernelProfiling = disable; }
  std::optional<bool>& getDisableKernelProfiling() { return m_disableKernelProfiling; }
  const std::optional<bool>& getDisableKernelProfiling() const { return m_disableKernelProfiling; }

 private:
  std::vector<GpuBackendCustomConfig> buildCustomConfigsVector() const {
    if (!m_backendExtensionTable.lock()) {
      throw std::runtime_error("GpuBackendCustomConfiguration has no API table");
    }
    std::vector<GpuBackendCustomConfig> toret;
    if (m_enableTuningMode.has_value()) {
      GpuBackendCustomConfig& back = toret.emplace_back(make<GpuBackendCustomConfig>());
      back.setEnableTuningMode(m_enableTuningMode.value());
    }
    if (m_performanceCacheDir.has_value()) {
      GpuBackendCustomConfig& back = toret.emplace_back(make<GpuBackendCustomConfig>());
      back.setPerformanceCacheDir(m_performanceCacheDir.value().c_str());
    }
    if (m_invalidatePerformanceCache.has_value()) {
      GpuBackendCustomConfig& back = toret.emplace_back(make<GpuBackendCustomConfig>());
      back.setInvalidatePerformanceCache(m_invalidatePerformanceCache.value());
    }
    if (m_weightSharingEnabled.has_value()) {
      GpuBackendCustomConfig& back = toret.emplace_back(make<GpuBackendCustomConfig>());
      back.setWeightSharingEnabled(m_weightSharingEnabled.value());
    }
    if (m_disableKernelProfiling.has_value()) {
      GpuBackendCustomConfig& back = toret.emplace_back(make<GpuBackendCustomConfig>());
      back.setDisableKernelProfiling(m_disableKernelProfiling.value());
    }
    return toret;
  }
};

}  // namespace qairt

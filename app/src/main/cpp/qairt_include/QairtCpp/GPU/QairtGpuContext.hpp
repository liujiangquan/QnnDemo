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

#include "GPU/QairtGpu.hpp"
#include "GPU/QairtGpuContext.h"
#include "QairtCppApi/QairtContext.hpp"

namespace qairt {

QAIRT_CPPAPI_DEFINE_ENUM(QairtGpuContextPerfHint,
                         QairtGpuContext_PerfHint_t,
                         High      = QAIRT_GPU_CONTEXT_PERF_HINT_HIGH,
                         Normal    = QAIRT_GPU_CONTEXT_PERF_HINT_NORMAL,
                         Low       = QAIRT_GPU_CONTEXT_PERF_HINT_LOW,
                         Undefined = QAIRT_GPU_CONTEXT_PERF_HINT_UNDEFINED)

class GpuContextCustomConfig
    : public GpuApiType<GpuContextCustomConfig, QairtGpuContext_CustomConfigV1_t>,
      public ContextCustomConfig {
  friend detail::TempWorkaround;
  template <typename>
  friend class BackendExtension;

 public:
  QairtContext_CustomConfigHandle_t getCustomConfigHandle() const override { return getHandle(); }

  GpuContextCustomConfig()                                             = default;
  GpuContextCustomConfig(const GpuContextCustomConfig&)                = delete;
  GpuContextCustomConfig(GpuContextCustomConfig&&) noexcept            = default;
  GpuContextCustomConfig& operator=(const GpuContextCustomConfig&)     = delete;
  GpuContextCustomConfig& operator=(GpuContextCustomConfig&&) noexcept = default;

  // Performance Hint
  void setPerfHint(QairtGpuContext_PerfHint_t hint) {
    callSetter<&interface_type::setPerfHint>(hint);
  }
  QairtGpuContext_PerfHint_t getPerfHint() const {
    return callGetter<&interface_type::getPerfHint>();
  }

  // Use GL Buffers
  void setUseGLBuffers(bool enable) {
    callSetter<&interface_type::setUseGLBuffers>(static_cast<uint8_t>(enable));
  }
  bool getUseGLBuffers() const {
    return static_cast<bool>(callGetter<&interface_type::getUseGLBuffers>());
  }

  // Kernel Repo Dir
  void setKernelRepoDir(const char* dir) { callSetter<&interface_type::setKernelRepoDir>(dir); }
  const char* getKernelRepoDir() const { return callGetter<&interface_type::getKernelRepoDir>(); }

  // Invalidate Kernel Repo
  void setInvalidateKernelRepo(bool invalidate) {
    callSetter<&interface_type::setInvalidateKernelRepo>(static_cast<uint8_t>(invalidate));
  }
  bool getInvalidateKernelRepo() const {
    return static_cast<bool>(callGetter<&interface_type::getInvalidateKernelRepo>());
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

  GpuContextCustomConfig(const std::weak_ptr<api_table_type>& apiTable)
      : ApiType(lockOrThrow(apiTable)) {
    construct();
    if (!getHandle()) {
      throw std::runtime_error("Somehow failed to construct?");
    }
  }
};

class GpuContextCustomConfiguration : public detail::TabularBEType<GpuBackendExtensionTable>,
                                      public ContextCustomConfiguration {
  std::optional<QairtGpuContext_PerfHint_t> m_perfHint{};
  std::optional<bool> m_useGLBuffers{};
  std::optional<std::string> m_kernelRepoDir{};
  std::optional<bool> m_invalidateKernelRepo{};

  mutable std::vector<GpuContextCustomConfig> m_customCfgs;
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

  // Performance Hint
  void clearPerfHint() { m_perfHint.reset(); }
  void setPerfHint(QairtGpuContext_PerfHint_t hint) { m_perfHint = hint; }
  std::optional<QairtGpuContext_PerfHint_t>& getPerfHint() { return m_perfHint; }
  const std::optional<QairtGpuContext_PerfHint_t>& getPerfHint() const { return m_perfHint; }

  // Use GL Buffers
  void clearUseGLBuffers() { m_useGLBuffers.reset(); }
  void setUseGLBuffers(bool enable) { m_useGLBuffers = enable; }
  std::optional<bool>& getUseGLBuffers() { return m_useGLBuffers; }
  const std::optional<bool>& getUseGLBuffers() const { return m_useGLBuffers; }

  // Kernel Repo Dir
  void clearKernelRepoDir() { m_kernelRepoDir.reset(); }
  void setKernelRepoDir(std::string dir) { m_kernelRepoDir = std::move(dir); }
  std::optional<std::string>& getKernelRepoDir() { return m_kernelRepoDir; }
  const std::optional<std::string>& getKernelRepoDir() const { return m_kernelRepoDir; }

  // Invalidate Kernel Repo
  void clearInvalidateKernelRepo() { m_invalidateKernelRepo.reset(); }
  void setInvalidateKernelRepo(bool invalidate) { m_invalidateKernelRepo = invalidate; }
  std::optional<bool>& getInvalidateKernelRepo() { return m_invalidateKernelRepo; }
  const std::optional<bool>& getInvalidateKernelRepo() const { return m_invalidateKernelRepo; }

 private:
  std::vector<GpuContextCustomConfig> buildCustomConfigsVector() const {
    if (!m_backendExtensionTable.lock()) {
      throw std::runtime_error("GpuContextCustomConfiguration has no API table");
    }
    std::vector<GpuContextCustomConfig> toret;
    if (m_perfHint.has_value()) {
      GpuContextCustomConfig& back = toret.emplace_back(make<GpuContextCustomConfig>());
      back.setPerfHint(m_perfHint.value());
    }
    if (m_useGLBuffers.has_value()) {
      GpuContextCustomConfig& back = toret.emplace_back(make<GpuContextCustomConfig>());
      back.setUseGLBuffers(m_useGLBuffers.value());
    }
    if (m_kernelRepoDir.has_value()) {
      GpuContextCustomConfig& back = toret.emplace_back(make<GpuContextCustomConfig>());
      back.setKernelRepoDir(m_kernelRepoDir.value().c_str());
    }
    if (m_invalidateKernelRepo.has_value()) {
      GpuContextCustomConfig& back = toret.emplace_back(make<GpuContextCustomConfig>());
      back.setInvalidateKernelRepo(m_invalidateKernelRepo.value());
    }
    return toret;
  }
};

}  // namespace qairt

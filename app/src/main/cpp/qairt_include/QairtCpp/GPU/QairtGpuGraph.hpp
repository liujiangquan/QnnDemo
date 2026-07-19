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
#include "GPU/QairtGpuGraph.h"
#include "QairtCppApi/QairtGraph.hpp"

namespace qairt {

QAIRT_CPPAPI_DEFINE_ENUM(QairtGpuPrecision,
                         QairtGpu_Precision_t,
                         FP32         = QAIRT_GPU_PRECISION_FP32,
                         FP16         = QAIRT_GPU_PRECISION_FP16,
                         Hybrid       = QAIRT_GPU_PRECISION_HYBRID,
                         UserProvided = QAIRT_GPU_PRECISION_USER_PROVIDED,
                         Undefined    = QAIRT_GPU_PRECISION_UNDEFINED)

//=============================================================================
// GpuGraphFenceConfig — C++ wrapper for the fence configuration handle
//=============================================================================

class GpuGraphFenceConfig : public GpuApiType<GpuGraphFenceConfig, QairtGpuGraph_FenceConfigV1_t> {
  friend detail::TempWorkaround;
  template <typename>
  friend class BackendExtension;

 public:
  GpuGraphFenceConfig()                                          = default;
  GpuGraphFenceConfig(const GpuGraphFenceConfig&)                = delete;
  GpuGraphFenceConfig(GpuGraphFenceConfig&&) noexcept            = default;
  GpuGraphFenceConfig& operator=(const GpuGraphFenceConfig&)     = delete;
  GpuGraphFenceConfig& operator=(GpuGraphFenceConfig&&) noexcept = default;

  void setNumInputFences(uint32_t* numInputFences) {
    callSetter<&interface_type::setNumInputFences>(numInputFences);
  }
  uint32_t* getNumInputFences() const { return callGetter<&interface_type::getNumInputFences>(); }

  void setInputFenceHandles(QairtGpuGraph_FenceHandle_t* handles) {
    callSetter<&interface_type::setInputFenceHandles>(handles);
  }
  QairtGpuGraph_FenceHandle_t* getInputFenceHandles() const {
    return callGetter<&interface_type::getInputFenceHandles>();
  }

  void setEnableOutputFence(bool enable) {
    callSetter<&interface_type::setEnableOutputFence>(static_cast<uint8_t>(enable));
  }
  bool getEnableOutputFence() const {
    return static_cast<bool>(callGetter<&interface_type::getEnableOutputFence>());
  }

  void setOutputFence(QairtGpuGraph_FenceHandle_t outputFence) {
    callSetter<&interface_type::setOutputFence>(outputFence);
  }
  QairtGpuGraph_FenceHandle_t getOutputFence() const {
    return callGetter<&interface_type::getOutputFence>();
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

  GpuGraphFenceConfig(const std::weak_ptr<api_table_type>& apiTable)
      : ApiType(lockOrThrow(apiTable)) {
    construct();
    if (!getHandle()) {
      throw std::runtime_error("Failed to construct");
    }
  }
};

//=============================================================================
// GpuGraphCustomConfig — C++ wrapper for the graph custom config handle
//=============================================================================

class GpuGraphCustomConfig
    : public GpuApiType<GpuGraphCustomConfig, QairtGpuGraph_CustomConfigV1_t>,
      public GraphCustomConfig {
  friend detail::TempWorkaround;
  template <typename>
  friend class BackendExtension;

  detail::crossable<detail::non_owning<GpuGraphFenceConfig>,
                    &interface_type::getFenceConfig,
                    &interface_type::setFenceConfig>
      m_fenceConfig;

 public:
  QairtGraph_CustomConfigHandle_t getCustomConfigHandle() const override { return getHandle(); }

  GpuGraphCustomConfig()                                           = default;
  GpuGraphCustomConfig(const GpuGraphCustomConfig&)                = delete;
  GpuGraphCustomConfig(GpuGraphCustomConfig&&) noexcept            = default;
  GpuGraphCustomConfig& operator=(const GpuGraphCustomConfig&)     = delete;
  GpuGraphCustomConfig& operator=(GpuGraphCustomConfig&&) noexcept = default;

  // Precision
  void setPrecision(QairtGpu_Precision_t precision) {
    callSetter<&interface_type::setPrecision>(precision);
  }
  QairtGpu_Precision_t getPrecision() const { return callGetter<&interface_type::getPrecision>(); }

  // Disable Memory Optimizations
  void setDisableMemoryOptimizations(bool disable) {
    callSetter<&interface_type::setDisableMemoryOptimizations>(static_cast<uint8_t>(disable));
  }
  bool getDisableMemoryOptimizations() const {
    return static_cast<bool>(callGetter<&interface_type::getDisableMemoryOptimizations>());
  }

  // Disable Node Optimizations
  void setDisableNodeOptimizations(bool disable) {
    callSetter<&interface_type::setDisableNodeOptimizations>(static_cast<uint8_t>(disable));
  }
  bool getDisableNodeOptimizations() const {
    return static_cast<bool>(callGetter<&interface_type::getDisableNodeOptimizations>());
  }

  // Disable Queue Recording
  void setDisableQueueRecording(bool disable) {
    callSetter<&interface_type::setDisableQueueRecording>(static_cast<uint8_t>(disable));
  }
  bool getDisableQueueRecording() const {
    return static_cast<bool>(callGetter<&interface_type::getDisableQueueRecording>());
  }

  // Fence Config
  void setFenceConfig(const GpuGraphFenceConfig& fenceConfig) {
    m_fenceConfig.setNonOwning(*this, fenceConfig);
  }
  GpuGraphFenceConfig& getFenceConfig() { return m_fenceConfig.bindNonOwning(*this); }

 private:
  static std::shared_ptr<api_table_type> lockOrThrow(
      const std::weak_ptr<api_table_type>& apiTable) {
    auto locked = apiTable.lock();
    if (!locked) {
      throw std::runtime_error("Failed to lock apiTable");
    }
    return locked;
  }

  GpuGraphCustomConfig(const std::weak_ptr<api_table_type>& apiTable)
      : ApiType(lockOrThrow(apiTable)) {
    construct();
    if (!getHandle()) {
      throw std::runtime_error("Failed to construct");
    }
  }
};

//=============================================================================
// GpuGraphCustomConfiguration — higher-level configuration builder
//=============================================================================

class GpuGraphCustomConfiguration : public detail::TabularBEType<GpuBackendExtensionTable>,
                                    public GraphCustomConfiguration {
  std::optional<QairtGpu_Precision_t> m_precision{};
  std::optional<bool> m_disableMemoryOptimizations{};
  std::optional<bool> m_disableNodeOptimizations{};
  std::optional<bool> m_disableQueueRecording{};
  std::optional<GpuGraphFenceConfig> m_fenceConfig{};

  mutable std::vector<GpuGraphCustomConfig> m_customCfgs;

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

  // Precision
  void clearPrecision() { m_precision.reset(); }
  void setPrecision(QairtGpu_Precision_t precision) { m_precision = precision; }
  std::optional<QairtGpu_Precision_t>& getPrecision() { return m_precision; }
  const std::optional<QairtGpu_Precision_t>& getPrecision() const { return m_precision; }

  // Disable Memory Optimizations
  void clearDisableMemoryOptimizations() { m_disableMemoryOptimizations.reset(); }
  void setDisableMemoryOptimizations(bool disable) { m_disableMemoryOptimizations = disable; }
  std::optional<bool>& getDisableMemoryOptimizations() { return m_disableMemoryOptimizations; }
  const std::optional<bool>& getDisableMemoryOptimizations() const {
    return m_disableMemoryOptimizations;
  }

  // Disable Node Optimizations
  void clearDisableNodeOptimizations() { m_disableNodeOptimizations.reset(); }
  void setDisableNodeOptimizations(bool disable) { m_disableNodeOptimizations = disable; }
  std::optional<bool>& getDisableNodeOptimizations() { return m_disableNodeOptimizations; }
  const std::optional<bool>& getDisableNodeOptimizations() const {
    return m_disableNodeOptimizations;
  }

  // Disable Queue Recording
  void clearDisableQueueRecording() { m_disableQueueRecording.reset(); }
  void setDisableQueueRecording(bool disable) { m_disableQueueRecording = disable; }
  std::optional<bool>& getDisableQueueRecording() { return m_disableQueueRecording; }
  const std::optional<bool>& getDisableQueueRecording() const { return m_disableQueueRecording; }

  // Fence Config
  void clearFenceConfig() { m_fenceConfig.reset(); }
  void setFenceConfig(GpuGraphFenceConfig fenceConfig) { m_fenceConfig = std::move(fenceConfig); }
  std::optional<GpuGraphFenceConfig>& getFenceConfig() { return m_fenceConfig; }
  const std::optional<GpuGraphFenceConfig>& getFenceConfig() const { return m_fenceConfig; }

 private:
  std::vector<GpuGraphCustomConfig> buildCustomConfigsVector() const {
    if (!m_backendExtensionTable.lock()) {
      throw std::runtime_error("GpuGraphCustomConfiguration has no API table");
    }
    std::vector<GpuGraphCustomConfig> toret;
    if (m_precision.has_value()) {
      GpuGraphCustomConfig& back = toret.emplace_back(make<GpuGraphCustomConfig>());
      back.setPrecision(m_precision.value());
    }
    if (m_disableMemoryOptimizations.has_value()) {
      GpuGraphCustomConfig& back = toret.emplace_back(make<GpuGraphCustomConfig>());
      back.setDisableMemoryOptimizations(m_disableMemoryOptimizations.value());
    }
    if (m_disableNodeOptimizations.has_value()) {
      GpuGraphCustomConfig& back = toret.emplace_back(make<GpuGraphCustomConfig>());
      back.setDisableNodeOptimizations(m_disableNodeOptimizations.value());
    }
    if (m_disableQueueRecording.has_value()) {
      GpuGraphCustomConfig& back = toret.emplace_back(make<GpuGraphCustomConfig>());
      back.setDisableQueueRecording(m_disableQueueRecording.value());
    }
    if (m_fenceConfig.has_value()) {
      GpuGraphCustomConfig& back = toret.emplace_back(make<GpuGraphCustomConfig>());
      back.setFenceConfig(m_fenceConfig.value());
    }
    return toret;
  }
};

}  // namespace qairt

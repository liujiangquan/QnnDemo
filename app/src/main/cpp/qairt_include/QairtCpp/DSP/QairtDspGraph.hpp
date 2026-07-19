//=============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//=============================================================================
#pragma once

#include <optional>
#include <utility>
#include <vector>

#include "QairtCppApi/QairtGraph.hpp"
#include "QairtDsp.hpp"
#include "QairtDspGraph.h"

namespace qairt {

QAIRT_CPPAPI_DEFINE_ENUM(
    DspGraphOptimizationType,
    QairtDspGraph_OptimizationType_t,
    ScheduleThreshold        = QAIRT_DSP_GRAPH_OPTIMIZATION_TYPE_SCHEDULE_THRESHOLD,
    FinalizeRetries          = QAIRT_DSP_GRAPH_OPTIMIZATION_TYPE_FINALIZE_RETRIES,
    FinalizeOptimizationFlag = QAIRT_DSP_GRAPH_OPTIMIZATION_TYPE_FINALIZE_OPTIMIZATION_FLAG,
    EnableDlbc               = QAIRT_DSP_GRAPH_OPTIMIZATION_TYPE_ENABLE_DLBC,
    Unknown                  = QAIRT_DSP_GRAPH_OPTIMIZATION_TYPE_UNKNOWN)

QAIRT_CPPAPI_DEFINE_ENUM(DspGraphEncoding,
                         QairtDspGraph_Encoding_t,
                         Dynamic = QAIRT_DSP_GRAPH_ENCODING_DYNAMIC,
                         Static  = QAIRT_DSP_GRAPH_ENCODING_STATIC,
                         Unknown = QAIRT_DSP_GRAPH_ENCODING_UNKNOWN)

QAIRT_CPPAPI_DEFINE_ENUM(Precision,
                         Qairt_Precision_t,
                         Float32   = QAIRT_PRECISION_FLOAT32,
                         Float16   = QAIRT_PRECISION_FLOAT16,
                         Undefined = QAIRT_PRECISION_UNDEFINED)

class DspGraphCustomConfig
    : public DspApiType<DspGraphCustomConfig, QairtDspGraph_CustomConfigV1_t>,
      public GraphCustomConfig {
  friend detail::TempWorkaround;
  template <typename>
  friend class BackendExtension;

 public:
  QairtGraph_CustomConfigHandle_t getCustomConfigHandle() const override { return getHandle(); }

  DspGraphCustomConfig()                                           = default;
  DspGraphCustomConfig(const DspGraphCustomConfig&)                = delete;
  DspGraphCustomConfig(DspGraphCustomConfig&&) noexcept            = default;
  DspGraphCustomConfig& operator=(const DspGraphCustomConfig&)     = delete;
  DspGraphCustomConfig& operator=(DspGraphCustomConfig&&) noexcept = default;

  void setOptimization(QairtDspGraph_OptimizationType_t type, float value) {
    callAndConfirm<&interface_type::setOptimization>(type, value);
  }
  std::pair<QairtDspGraph_OptimizationType_t, float> getOptimization() const {
    QairtDspGraph_OptimizationType_t type{};
    float value{};
    callAndConfirm<&interface_type::getOptimization>(&type, &value);
    return {type, value};
  }

  void setEncoding(QairtDspGraph_Encoding_t encoding) {
    callSetter<&interface_type::setEncoding>(encoding);
  }
  QairtDspGraph_Encoding_t getEncoding() const {
    return callGetter<&interface_type::getEncoding>();
  }

  void setPriority(Qairt_Priority_t priority) {
    callSetter<&interface_type::setPriority>(priority);
  }
  Qairt_Priority_t getPriority() const { return callGetter<&interface_type::getPriority>(); }

  void setPrecision(Qairt_Precision_t precision) {
    callSetter<&interface_type::setPrecision>(precision);
  }
  Qairt_Precision_t getPrecision() const { return callGetter<&interface_type::getPrecision>(); }

 private:
  DspGraphCustomConfig(const std::weak_ptr<api_table_type>& apiTable) : ApiType(apiTable.lock()) {
    if (!apiTable.lock()) {
      throw std::runtime_error("Failed to lock apiTable");
    }
    construct();
    if (!getHandle()) {
      throw std::runtime_error("Failed to construct DspGraphCustomConfig");
    }
  }
};

class DspGraphCustomConfiguration : public detail::TabularBEType<DspBackendExtensionTable>,
                                    public GraphCustomConfiguration {
  // Each optimization type is stored independently as it maps to a separate
  // set call on the underlying C API.
  std::optional<float> m_scheduleThreshold{};
  std::optional<float> m_finalizeRetries{};
  std::optional<float> m_finalizeOptimizationFlag{};
  std::optional<float> m_enableDlbc{};

  std::optional<QairtDspGraph_Encoding_t> m_encoding{};
  std::optional<Qairt_Priority_t> m_priority{};
  std::optional<Qairt_Precision_t> m_precision{};

  mutable std::vector<DspGraphCustomConfig> m_customCfgs;
  friend Graph;

 public:
  std::vector<QairtGraph_CustomConfigHandle_t> getCustomConfigs() const override {
    std::vector<QairtGraph_CustomConfigHandle_t> toret;
    m_customCfgs = buildCustomConfigs();
    toret.reserve(m_customCfgs.size());
    for (auto& cfg : m_customCfgs) {
      toret.push_back(cfg.getCustomConfigHandle());
    }
    return toret;
  }

  std::vector<DspGraphCustomConfig> buildCustomConfigs() const {
    if (!m_backendExtensionTable.lock()) {
      throw std::runtime_error("Malformed DspGraphCustomConfiguration: missing API table.");
    }
    std::vector<DspGraphCustomConfig> toret;

    if (m_scheduleThreshold.has_value()) {
      DspGraphCustomConfig& cfg = toret.emplace_back(make<DspGraphCustomConfig>());
      cfg.setOptimization(QAIRT_DSP_GRAPH_OPTIMIZATION_TYPE_SCHEDULE_THRESHOLD,
                          m_scheduleThreshold.value());
    }
    if (m_finalizeRetries.has_value()) {
      DspGraphCustomConfig& cfg = toret.emplace_back(make<DspGraphCustomConfig>());
      cfg.setOptimization(QAIRT_DSP_GRAPH_OPTIMIZATION_TYPE_FINALIZE_RETRIES,
                          m_finalizeRetries.value());
    }
    if (m_finalizeOptimizationFlag.has_value()) {
      DspGraphCustomConfig& cfg = toret.emplace_back(make<DspGraphCustomConfig>());
      cfg.setOptimization(QAIRT_DSP_GRAPH_OPTIMIZATION_TYPE_FINALIZE_OPTIMIZATION_FLAG,
                          m_finalizeOptimizationFlag.value());
    }
    if (m_enableDlbc.has_value()) {
      DspGraphCustomConfig& cfg = toret.emplace_back(make<DspGraphCustomConfig>());
      cfg.setOptimization(QAIRT_DSP_GRAPH_OPTIMIZATION_TYPE_ENABLE_DLBC, m_enableDlbc.value());
    }
    if (m_encoding.has_value()) {
      DspGraphCustomConfig& cfg = toret.emplace_back(make<DspGraphCustomConfig>());
      cfg.setEncoding(m_encoding.value());
    }
    if (m_priority.has_value()) {
      DspGraphCustomConfig& cfg = toret.emplace_back(make<DspGraphCustomConfig>());
      cfg.setPriority(m_priority.value());
    }
    if (m_precision.has_value()) {
      DspGraphCustomConfig& cfg = toret.emplace_back(make<DspGraphCustomConfig>());
      cfg.setPrecision(m_precision.value());
    }

    return toret;
  }

 public:
  using base_type::base_type;

  // Schedule Threshold
  void clearScheduleThreshold() { m_scheduleThreshold.reset(); }
  void setScheduleThreshold(float value) { m_scheduleThreshold = value; }
  std::optional<float>& getScheduleThreshold() { return m_scheduleThreshold; }
  const std::optional<float>& getScheduleThreshold() const { return m_scheduleThreshold; }

  // Finalize Retries
  void clearFinalizeRetries() { m_finalizeRetries.reset(); }
  void setFinalizeRetries(float value) { m_finalizeRetries = value; }
  std::optional<float>& getFinalizeRetries() { return m_finalizeRetries; }
  const std::optional<float>& getFinalizeRetries() const { return m_finalizeRetries; }

  // Finalize Optimization Flag
  void clearFinalizeOptimizationFlag() { m_finalizeOptimizationFlag.reset(); }
  void setFinalizeOptimizationFlag(float value) { m_finalizeOptimizationFlag = value; }
  std::optional<float>& getFinalizeOptimizationFlag() { return m_finalizeOptimizationFlag; }
  const std::optional<float>& getFinalizeOptimizationFlag() const {
    return m_finalizeOptimizationFlag;
  }

  // Enable DLBC
  void clearEnableDlbc() { m_enableDlbc.reset(); }
  void setEnableDlbc(float value) { m_enableDlbc = value; }
  std::optional<float>& getEnableDlbc() { return m_enableDlbc; }
  const std::optional<float>& getEnableDlbc() const { return m_enableDlbc; }

  // Encoding
  void clearEncoding() { m_encoding.reset(); }
  void setEncoding(QairtDspGraph_Encoding_t encoding) { m_encoding = encoding; }
  std::optional<QairtDspGraph_Encoding_t>& getEncoding() { return m_encoding; }
  const std::optional<QairtDspGraph_Encoding_t>& getEncoding() const { return m_encoding; }

  // Priority
  void clearPriority() { m_priority.reset(); }
  void setPriority(Qairt_Priority_t priority) { m_priority = priority; }
  std::optional<Qairt_Priority_t>& getPriority() { return m_priority; }
  const std::optional<Qairt_Priority_t>& getPriority() const { return m_priority; }

  // Precision
  void clearPrecision() { m_precision.reset(); }
  void setPrecision(Qairt_Precision_t precision) { m_precision = precision; }
  std::optional<Qairt_Precision_t>& getPrecision() { return m_precision; }
  const std::optional<Qairt_Precision_t>& getPrecision() const { return m_precision; }
};

}  // namespace qairt

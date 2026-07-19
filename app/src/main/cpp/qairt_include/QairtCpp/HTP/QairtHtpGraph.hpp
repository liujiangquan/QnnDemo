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

#include "QairtCppApi/QairtGraph.hpp"
#include "QairtHtp.hpp"
#include "QairtHtpGraph.h"

namespace qairt {

QAIRT_CPPAPI_DEFINE_ENUM(
    HtpGraphOptimizationType,
    QairtHtpGraph_OptimizationType_t,
    ScheduleThreshold        = QAIRT_HTP_GRAPH_OPTIMIZATION_TYPE_SCHEDULE_THRESHOLD,
    FinalizeRetries          = QAIRT_HTP_GRAPH_OPTIMIZATION_TYPE_FINALIZE_RETRIES,
    FinalizeOptimizationFlag = QAIRT_HTP_GRAPH_OPTIMIZATION_TYPE_FINALIZE_OPTIMIZATION_FLAG,
    EnableDlbc               = QAIRT_HTP_GRAPH_OPTIMIZATION_TYPE_ENABLE_DLBC,
    EnableDlbcWeights        = QAIRT_HTP_GRAPH_OPTIMIZATION_TYPE_ENABLE_DLBC_WEIGHTS,
    EnableSparseWeightsCompression =
        QAIRT_HTP_GRAPH_OPTIMIZATION_TYPE_ENABLE_SPARSE_WEIGHTS_COMPRESSION,
    EnableSlcAllocator = QAIRT_HTP_GRAPH_OPTIMIZATION_TYPE_ENABLE_SLC_ALLOCATOR,
    Unknown            = QAIRT_HTP_GRAPH_OPTIMIZATION_TYPE_UNKNOWN)

QAIRT_CPPAPI_DEFINE_ENUM(HtpGraphPrecision,
                         QairtHtpGraph_Precision_t,
                         Float16   = QAIRT_HTP_GRAPH_PRECISION_FLOAT16,
                         Float32   = QAIRT_HTP_GRAPH_PRECISION_FLOAT32,
                         Default   = QAIRT_HTP_GRAPH_PRECISION_DEFAULT,
                         Undefined = QAIRT_HTP_GRAPH_PRECISION_UNDEFINED)

struct HtpGraphOptimizationOption;

class HtpGraphOptimizationOption
    : public HtpApiType<HtpGraphOptimizationOption, QairtHtpGraph_OptimizationOptionV1_t> {
  template <typename>
  friend class BackendExtension;
  friend detail::TempWorkaround;

 public:
  using base_type::base_type;
  HtpGraphOptimizationOption() = default;

  HtpGraphOptimizationOption(const HtpGraphOptimizationOption& other)
      : base_type(copy_table_tag, other) {
    construct();
    setType(other.getType());
    setFloatValue(other.getFloatValue());
  }
  HtpGraphOptimizationOption(HtpGraphOptimizationOption&& other) noexcept
      : base_type(static_cast<base_type&&>(other)) {}
  HtpGraphOptimizationOption& operator=(HtpGraphOptimizationOption&& other) noexcept {
    if (this != &other) {
      std::swap(static_cast<base_type&>(*this), static_cast<base_type&>(other));
    }
    return *this;
  }
  HtpGraphOptimizationOption& operator=(const HtpGraphOptimizationOption& other) {
    if (this != &other) {
      *this = HtpGraphOptimizationOption(other);
    }
    return *this;
  }
  ~HtpGraphOptimizationOption() { QAIRT_TRIVIAL }

  void setType(HtpGraphOptimizationType type) {
    callSetter<&interface_type::setType>(static_cast<QairtHtpGraph_OptimizationType_t>(type));
  }
  HtpGraphOptimizationType getType() const {
    return static_cast<HtpGraphOptimizationType>(callGetter<&interface_type::getType>());
  }

  void setFloatValue(float floatValue) { callSetter<&interface_type::setFloatValue>(floatValue); }
  float getFloatValue() const { return callGetter<&interface_type::getFloatValue>(); }

 private:
  explicit HtpGraphOptimizationOption(const std::weak_ptr<api_table_type>& apiTable)
      : ApiType(apiTable.lock()) {
    construct();
  }
};

class HtpHmxBoundingInfo : public HtpApiType<HtpHmxBoundingInfo, QairtHtp_HmxBoundingInfoV1_t> {
  template <typename>
  friend class BackendExtension;
  friend detail::TempWorkaround;

 public:
  using base_type::base_type;
  HtpHmxBoundingInfo() = default;

  HtpHmxBoundingInfo(const HtpHmxBoundingInfo& other) : base_type(copy_table_tag, other) {
    construct();
    setTargetHmxFreqHz(other.getTargetHmxFreqHz());
    setTargetDspCoreFreq(other.getTargetDspCoreFreq());
  }
  HtpHmxBoundingInfo(HtpHmxBoundingInfo&& other) noexcept
      : base_type(static_cast<base_type&&>(other)) {}
  HtpHmxBoundingInfo& operator=(HtpHmxBoundingInfo&& other) noexcept {
    if (this != &other) {
      std::swap(static_cast<base_type&>(*this), static_cast<base_type&>(other));
    }
    return *this;
  }
  HtpHmxBoundingInfo& operator=(const HtpHmxBoundingInfo& other) {
    if (this != &other) {
      *this = HtpHmxBoundingInfo(other);
    }
    return *this;
  }
  ~HtpHmxBoundingInfo() { QAIRT_TRIVIAL }

  void setTargetHmxFreqHz(float targetHmxFreqHz) {
    callSetter<&interface_type::setTargetHmxFreqHz>(targetHmxFreqHz);
  }
  float getTargetHmxFreqHz() const { return callGetter<&interface_type::getTargetHmxFreqHz>(); }

  void setTargetDspCoreFreq(float targetDspCoreFreq) {
    callSetter<&interface_type::setTargetDspCoreFreq>(targetDspCoreFreq);
  }
  float getTargetDspCoreFreq() const { return callGetter<&interface_type::getTargetDspCoreFreq>(); }

 private:
  explicit HtpHmxBoundingInfo(const std::weak_ptr<api_table_type>& apiTable)
      : ApiType(apiTable.lock()) {
    construct();
  }
};

class HtpGraphCustomConfig
    : public HtpApiType<HtpGraphCustomConfig, QairtHtpGraph_CustomConfigV1_t>,
      public GraphCustomConfig {
  detail::crossable<detail::non_owning<HtpGraphOptimizationOption>,
                    &interface_type::getOptimizationOption,
                    &interface_type::setOptimizationOption>
      m_optimizationOption;
  detail::crossable<detail::non_owning<HtpHmxBoundingInfo>,
                    &interface_type::getHmxBoundingInfo,
                    &interface_type::setHmxBoundingInfo>
      m_hmxBoundingInfo;

  template <typename>
  friend class BackendExtension;

  friend detail::TempWorkaround;

 public:
  HtpGraphCustomConfig()                                           = default;
  HtpGraphCustomConfig(const HtpGraphCustomConfig&)                = delete;
  HtpGraphCustomConfig(HtpGraphCustomConfig&&) noexcept            = default;
  HtpGraphCustomConfig& operator=(const HtpGraphCustomConfig&)     = delete;
  HtpGraphCustomConfig& operator=(HtpGraphCustomConfig&&) noexcept = default;

  QairtGraph_CustomConfigHandle_t getCustomConfigHandle() const override { return getHandle(); }

  // Optimization Option
  void setOptimizationOption(const HtpGraphOptimizationOption& optimizationOption) {
    callSetter<&interface_type::setOptimizationOption>(
        detail::QairtTypeAdmin::getHandle(optimizationOption));
  }
  HtpGraphOptimizationOption getOptimizationOption() const {
    return m_optimizationOption.bindNonOwning(*this);
  }

  // Precision
  void setPrecision(HtpGraphPrecision precision) {
    callSetter<&interface_type::setPrecision>(static_cast<QairtHtpGraph_Precision_t>(precision));
  }
  HtpGraphPrecision getPrecision() const {
    return static_cast<HtpGraphPrecision>(callGetter<&interface_type::getPrecision>());
  }

  // VTCM Size (MB)
  void setVtcmSizeInMB(uint32_t vtcmSizeInMB) {
    callSetter<&interface_type::setVtcmSizeInMB>(vtcmSizeInMB);
  }
  uint32_t getVtcmSizeInMB() const { return callGetter<&interface_type::getVtcmSizeInMB>(); }

  // VTCM Size (Bytes)
  void setVtcmSizeInBytes(uint32_t vtcmSizeInBytes) {
    callSetter<&interface_type::setVtcmSizeInBytes>(vtcmSizeInBytes);
  }
  uint32_t getVtcmSizeInBytes() const { return callGetter<&interface_type::getVtcmSizeInBytes>(); }

  // Fold ReLU Activation Into Conv Off
  void setFoldReluActivationIntoConvOff(bool foldReluOff) {
    callSetter<&interface_type::setFoldReluActivationIntoConvOff>(detail::convertBool(foldReluOff));
  }
  bool getFoldReluActivationIntoConvOff() const {
    return detail::resolveBool(callGetter<&interface_type::getFoldReluActivationIntoConvOff>());
  }

  // Short Depth Conv On HMX Off
  void setShortDepthConvOnHmxOff(bool shortDepthConvOff) {
    callSetter<&interface_type::setShortDepthConvOnHmxOff>(detail::convertBool(shortDepthConvOff));
  }
  bool getShortDepthConvOnHmxOff() const {
    return detail::resolveBool(callGetter<&interface_type::getShortDepthConvOnHmxOff>());
  }

  // Num HVX Threads
  void setNumHvxThreads(uint64_t numHvxThreads) {
    callSetter<&interface_type::setNumHvxThreads>(numHvxThreads);
  }
  uint64_t getNumHvxThreads() const { return callGetter<&interface_type::getNumHvxThreads>(); }

  // Num Cores
  void setNumCores(uint32_t numCores) { callSetter<&interface_type::setNumCores>(numCores); }
  uint32_t getNumCores() const { return callGetter<&interface_type::getNumCores>(); }

  // HMX Bounding Info
  void setHmxBoundingInfo(const HtpHmxBoundingInfo& hmxBoundingInfo) {
    callSetter<&interface_type::setHmxBoundingInfo>(
        detail::QairtTypeAdmin::getHandle(hmxBoundingInfo));
  }
  HtpHmxBoundingInfo getHmxBoundingInfo() const { return m_hmxBoundingInfo.bindNonOwning(*this); }

  // Weights Packing
  void setWeightsPacking(bool weightsPacking) {
    callSetter<&interface_type::setWeightsPacking>(detail::convertBool(weightsPacking));
  }
  bool getWeightsPacking() const {
    return detail::resolveBool(callGetter<&interface_type::getWeightsPacking>());
  }

  // Assume Same Quant
  void setAssumeSameQuant(bool assumeSameQuant) {
    callSetter<&interface_type::setAssumeSameQuant>(detail::convertBool(assumeSameQuant));
  }
  bool getAssumeSameQuant() const {
    return detail::resolveBool(callGetter<&interface_type::getAssumeSameQuant>());
  }

  // Share IO Buffer
  void setShareIOBuffer(bool shareIOBuffer) {
    callSetter<&interface_type::setShareIOBuffer>(detail::convertBool(shareIOBuffer));
  }
  bool getShareIOBuffer() const {
    return detail::resolveBool(callGetter<&interface_type::getShareIOBuffer>());
  }

  // Advanced Activation Fusion
  void setAdvancedActivationFusion(bool advancedActivationFusion) {
    callSetter<&interface_type::setAdvancedActivationFusion>(
        detail::convertBool(advancedActivationFusion));
  }
  bool getAdvancedActivationFusion() const {
    return detail::resolveBool(callGetter<&interface_type::getAdvancedActivationFusion>());
  }

  // High Precision Sigmoid
  void setHighPrecisionSigmoid(bool highPrecisionSigmoid) {
    callSetter<&interface_type::setHighPrecisionSigmoid>(detail::convertBool(highPrecisionSigmoid));
  }
  bool getHighPrecisionSigmoid() const {
    return detail::resolveBool(callGetter<&interface_type::getHighPrecisionSigmoid>());
  }

  // Monolithic LSTM
  void setMonolithicLstm(bool monolithicLstm) {
    callSetter<&interface_type::setMonolithicLstm>(detail::convertBool(monolithicLstm));
  }
  bool getMonolithicLstm() const {
    return detail::resolveBool(callGetter<&interface_type::getMonolithicLstm>());
  }

 private:
  explicit HtpGraphCustomConfig(const std::weak_ptr<api_table_type>& apiTable)
      : ApiType(apiTable.lock()) {
    if (!apiTable.lock()) {
      throw std::runtime_error("Failed to lock apiTable");
    }
    construct();

    if (!getHandle()) {
      throw std::runtime_error("Failed to construct HtpGraphCustomConfig");
    }
  }
};

class HtpGraphCustomConfiguration : public detail::TabularBEType<HtpBackendExtensionTable>,
                                    public GraphCustomConfiguration {
  std::optional<HtpGraphOptimizationOption> m_optimizationOption{};
  std::optional<HtpGraphPrecision> m_precision{};
  std::optional<uint32_t> m_vtcmSizeInMB{};
  std::optional<uint32_t> m_vtcmSizeInBytes{};
  std::optional<bool> m_foldReluActivationIntoConvOff{};
  std::optional<bool> m_shortDepthConvOnHmxOff{};
  std::optional<uint64_t> m_numHvxThreads{};
  std::optional<uint32_t> m_numCores{};
  std::optional<HtpHmxBoundingInfo> m_hmxBoundingInfo{};
  std::optional<bool> m_weightsPacking{};
  std::optional<bool> m_assumeSameQuant{};
  std::optional<bool> m_shareIOBuffer{};
  std::optional<bool> m_advancedActivationFusion{};
  std::optional<bool> m_highPrecisionSigmoid{};
  std::optional<bool> m_monolithicLstm{};

  mutable std::vector<HtpGraphCustomConfig> m_customCfgs;
  friend Graph;

 public:
  std::vector<QairtGraph_CustomConfigHandle_t> getCustomConfigs() const override {
    std::vector<QairtGraph_CustomConfigHandle_t> toret;
    m_customCfgs = getCustomConfigs2();
    toret.reserve(m_customCfgs.size());
    for (auto& e : m_customCfgs) {
      toret.push_back(e.getCustomConfigHandle());
    }
    return toret;
  }
  std::vector<HtpGraphCustomConfig> getCustomConfigs2() const {
    if (!m_backendExtensionTable.lock()) {
      throw std::runtime_error("Malformed HtpGraphCustomConfiguration: missing API table.");
    }
    std::vector<HtpGraphCustomConfig> toret;
    if (m_optimizationOption.has_value()) {
      HtpGraphCustomConfig& back = toret.emplace_back(make<HtpGraphCustomConfig>());
      back.setOptimizationOption(m_optimizationOption.value());
    }
    if (m_precision.has_value()) {
      HtpGraphCustomConfig& back = toret.emplace_back(make<HtpGraphCustomConfig>());
      back.setPrecision(m_precision.value());
    }
    if (m_vtcmSizeInMB.has_value()) {
      HtpGraphCustomConfig& back = toret.emplace_back(make<HtpGraphCustomConfig>());
      back.setVtcmSizeInMB(m_vtcmSizeInMB.value());
    }
    if (m_vtcmSizeInBytes.has_value()) {
      HtpGraphCustomConfig& back = toret.emplace_back(make<HtpGraphCustomConfig>());
      back.setVtcmSizeInBytes(m_vtcmSizeInBytes.value());
    }
    if (m_foldReluActivationIntoConvOff.has_value()) {
      HtpGraphCustomConfig& back = toret.emplace_back(make<HtpGraphCustomConfig>());
      back.setFoldReluActivationIntoConvOff(m_foldReluActivationIntoConvOff.value());
    }
    if (m_shortDepthConvOnHmxOff.has_value()) {
      HtpGraphCustomConfig& back = toret.emplace_back(make<HtpGraphCustomConfig>());
      back.setShortDepthConvOnHmxOff(m_shortDepthConvOnHmxOff.value());
    }
    if (m_numHvxThreads.has_value()) {
      HtpGraphCustomConfig& back = toret.emplace_back(make<HtpGraphCustomConfig>());
      back.setNumHvxThreads(m_numHvxThreads.value());
    }
    if (m_numCores.has_value()) {
      HtpGraphCustomConfig& back = toret.emplace_back(make<HtpGraphCustomConfig>());
      back.setNumCores(m_numCores.value());
    }
    if (m_hmxBoundingInfo.has_value()) {
      HtpGraphCustomConfig& back = toret.emplace_back(make<HtpGraphCustomConfig>());
      back.setHmxBoundingInfo(m_hmxBoundingInfo.value());
    }
    if (m_weightsPacking.has_value()) {
      HtpGraphCustomConfig& back = toret.emplace_back(make<HtpGraphCustomConfig>());
      back.setWeightsPacking(m_weightsPacking.value());
    }
    if (m_assumeSameQuant.has_value()) {
      HtpGraphCustomConfig& back = toret.emplace_back(make<HtpGraphCustomConfig>());
      back.setAssumeSameQuant(m_assumeSameQuant.value());
    }
    if (m_shareIOBuffer.has_value()) {
      HtpGraphCustomConfig& back = toret.emplace_back(make<HtpGraphCustomConfig>());
      back.setShareIOBuffer(m_shareIOBuffer.value());
    }
    if (m_advancedActivationFusion.has_value()) {
      HtpGraphCustomConfig& back = toret.emplace_back(make<HtpGraphCustomConfig>());
      back.setAdvancedActivationFusion(m_advancedActivationFusion.value());
    }
    if (m_highPrecisionSigmoid.has_value()) {
      HtpGraphCustomConfig& back = toret.emplace_back(make<HtpGraphCustomConfig>());
      back.setHighPrecisionSigmoid(m_highPrecisionSigmoid.value());
    }
    if (m_monolithicLstm.has_value()) {
      HtpGraphCustomConfig& back = toret.emplace_back(make<HtpGraphCustomConfig>());
      back.setMonolithicLstm(m_monolithicLstm.value());
    }
    return toret;
  }

 public:
  using base_type::base_type;

  // Optimization Option
  void clearOptimizationOption() { m_optimizationOption.reset(); }
  void setOptimizationOption(HtpGraphOptimizationOption optimizationOption) {
    m_optimizationOption = optimizationOption;
  }
  std::optional<HtpGraphOptimizationOption>& getOptimizationOption() {
    return m_optimizationOption;
  }
  const std::optional<HtpGraphOptimizationOption>& getOptimizationOption() const {
    return m_optimizationOption;
  }

  // Precision
  void clearPrecision() { m_precision.reset(); }
  void setPrecision(HtpGraphPrecision precision) { m_precision = precision; }
  std::optional<HtpGraphPrecision>& getPrecision() { return m_precision; }
  const std::optional<HtpGraphPrecision>& getPrecision() const { return m_precision; }

  // VTCM Size (MB)
  void clearVtcmSizeInMB() { m_vtcmSizeInMB.reset(); }
  void setVtcmSizeInMB(uint32_t vtcmSizeInMB) { m_vtcmSizeInMB = vtcmSizeInMB; }
  std::optional<uint32_t>& getVtcmSizeInMB() { return m_vtcmSizeInMB; }
  const std::optional<uint32_t>& getVtcmSizeInMB() const { return m_vtcmSizeInMB; }

  // VTCM Size (Bytes)
  void clearVtcmSizeInBytes() { m_vtcmSizeInBytes.reset(); }
  void setVtcmSizeInBytes(uint32_t vtcmSizeInBytes) { m_vtcmSizeInBytes = vtcmSizeInBytes; }
  std::optional<uint32_t>& getVtcmSizeInBytes() { return m_vtcmSizeInBytes; }
  const std::optional<uint32_t>& getVtcmSizeInBytes() const { return m_vtcmSizeInBytes; }

  // Fold ReLU Activation Into Conv Off
  void clearFoldReluActivationIntoConvOff() { m_foldReluActivationIntoConvOff.reset(); }
  void setFoldReluActivationIntoConvOff(bool foldReluOff) {
    m_foldReluActivationIntoConvOff = foldReluOff;
  }
  std::optional<bool>& getFoldReluActivationIntoConvOff() {
    return m_foldReluActivationIntoConvOff;
  }
  const std::optional<bool>& getFoldReluActivationIntoConvOff() const {
    return m_foldReluActivationIntoConvOff;
  }

  // Short Depth Conv On HMX Off
  void clearShortDepthConvOnHmxOff() { m_shortDepthConvOnHmxOff.reset(); }
  void setShortDepthConvOnHmxOff(bool shortDepthConvOff) {
    m_shortDepthConvOnHmxOff = shortDepthConvOff;
  }
  std::optional<bool>& getShortDepthConvOnHmxOff() { return m_shortDepthConvOnHmxOff; }
  const std::optional<bool>& getShortDepthConvOnHmxOff() const { return m_shortDepthConvOnHmxOff; }

  // Num HVX Threads
  void clearNumHvxThreads() { m_numHvxThreads.reset(); }
  void setNumHvxThreads(uint64_t numHvxThreads) { m_numHvxThreads = numHvxThreads; }
  std::optional<uint64_t>& getNumHvxThreads() { return m_numHvxThreads; }
  const std::optional<uint64_t>& getNumHvxThreads() const { return m_numHvxThreads; }

  // Num Cores
  void clearNumCores() { m_numCores.reset(); }
  void setNumCores(uint32_t numCores) { m_numCores = numCores; }
  std::optional<uint32_t>& getNumCores() { return m_numCores; }
  const std::optional<uint32_t>& getNumCores() const { return m_numCores; }

  // HMX Bounding Info
  void clearHmxBoundingInfo() { m_hmxBoundingInfo.reset(); }
  void setHmxBoundingInfo(HtpHmxBoundingInfo hmxBoundingInfo) {
    m_hmxBoundingInfo = std::move(hmxBoundingInfo);
  }
  std::optional<HtpHmxBoundingInfo>& getHmxBoundingInfo() { return m_hmxBoundingInfo; }
  const std::optional<HtpHmxBoundingInfo>& getHmxBoundingInfo() const { return m_hmxBoundingInfo; }

  // Weights Packing
  void clearWeightsPacking() { m_weightsPacking.reset(); }
  void setWeightsPacking(bool weightsPacking) { m_weightsPacking = weightsPacking; }
  std::optional<bool>& getWeightsPacking() { return m_weightsPacking; }
  const std::optional<bool>& getWeightsPacking() const { return m_weightsPacking; }

  // Assume Same Quant
  void clearAssumeSameQuant() { m_assumeSameQuant.reset(); }
  void setAssumeSameQuant(bool assumeSameQuant) { m_assumeSameQuant = assumeSameQuant; }
  std::optional<bool>& getAssumeSameQuant() { return m_assumeSameQuant; }
  const std::optional<bool>& getAssumeSameQuant() const { return m_assumeSameQuant; }

  // Share IO Buffer
  void clearShareIOBuffer() { m_shareIOBuffer.reset(); }
  void setShareIOBuffer(bool shareIOBuffer) { m_shareIOBuffer = shareIOBuffer; }
  std::optional<bool>& getShareIOBuffer() { return m_shareIOBuffer; }
  const std::optional<bool>& getShareIOBuffer() const { return m_shareIOBuffer; }

  // Advanced Activation Fusion
  void clearAdvancedActivationFusion() { m_advancedActivationFusion.reset(); }
  void setAdvancedActivationFusion(bool advancedActivationFusion) {
    m_advancedActivationFusion = advancedActivationFusion;
  }
  std::optional<bool>& getAdvancedActivationFusion() { return m_advancedActivationFusion; }
  const std::optional<bool>& getAdvancedActivationFusion() const {
    return m_advancedActivationFusion;
  }

  // High Precision Sigmoid
  void clearHighPrecisionSigmoid() { m_highPrecisionSigmoid.reset(); }
  void setHighPrecisionSigmoid(bool highPrecisionSigmoid) {
    m_highPrecisionSigmoid = highPrecisionSigmoid;
  }
  std::optional<bool>& getHighPrecisionSigmoid() { return m_highPrecisionSigmoid; }
  const std::optional<bool>& getHighPrecisionSigmoid() const { return m_highPrecisionSigmoid; }

  // Monolithic LSTM
  void clearMonolithicLstm() { m_monolithicLstm.reset(); }
  void setMonolithicLstm(bool monolithicLstm) { m_monolithicLstm = monolithicLstm; }
  std::optional<bool>& getMonolithicLstm() { return m_monolithicLstm; }
  const std::optional<bool>& getMonolithicLstm() const { return m_monolithicLstm; }
};

}  // namespace qairt

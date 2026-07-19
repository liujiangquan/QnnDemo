//==============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All Rights Reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//==============================================================================

/**
 * @file
 * @brief  C++ wrappers for the QAIRT System Context API.
 *
 *         A SystemContext must be created via SystemApi before calling getMetadata().
 *
 */

#pragma once

#include <cstdint>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <variant>

#include "../QairtBuffer.hpp"
#include "../QairtDevice.hpp"
#include "../QairtInfo.hpp"
#include "../QairtTensor.hpp"
#include "QairtSystem/QairtSystemContext.h"
#include "QairtSystemApi.hpp"

namespace qairt {

/**
 * @brief Quantization encoding type for a context tensor.
 *
 * @details
 * | Enumerator              | Description                                                    |
 * |-------------------------|----------------------------------------------------------------|
 * | `ScaleOffset`           | Per-tensor scale and offset encoding.                          |
 * | `AxisScaleOffset`       | Per-axis (e.g., per-channel) scale and offset encoding.        |
 * | `BwScaleOffset`         | Per-tensor bit-width, scale, and offset encoding.              |
 * | `BwAxisScaleOffset`     | Per-axis bit-width, scale, and offset encoding.                |
 * | `BwAxisScaleOffsetMapped` | Per-axis bit-width, scale, and offset with a value-mapping scheme. |
 * | `Undefined`             | Encoding type is unknown or not set.                           |
 */
QAIRT_CPPAPI_DEFINE_ENUM(SystemContextQuantInfoType,
                         Qairt_QuantizationInfoType_t,
                         QAIRT_QUANTIZATION_INFO_SCALE_OFFSET                = 0x00,
                         QAIRT_QUANTIZATION_INFO_AXIS_SCALE_OFFSET           = 0x01,
                         QAIRT_QUANTIZATION_INFO_BW_SCALE_OFFSET             = 0x02,
                         QAIRT_QUANTIZATION_INFO_BW_AXIS_SCALE_OFFSET        = 0x03,
                         QAIRT_QUANTIZATION_INFO_BW_AXIS_SCALE_OFFSET_MAPPED = 0x08,
                         QAIRT_QUANTIZATION_INFO_UNDEFINED                   = 0x7FFFFFFF)

/**
 * @brief Quantized value mapping scheme for bit-width axis scale-offset encodings.
 *
 * @details
 * | Enumerator                      | Description                                                                    |
 * |---------------------------------|--------------------------------------------------------------------------------|
 * | `StandardSymmetric`             | Standard symmetric two's complement mapping.                                   |
 * | `AsymmetricPlusOne`             | Two's complement mapping with a positive shift of one.                         |
 * | `LinearSymmetricExcludeZero`    | Linear mapping symmetric about zero, excluding zero from the representable range. |
 * | `Undefined`                     | Mapping scheme is unknown or not set.                                          |
 */
QAIRT_CPPAPI_DEFINE_ENUM(Qairt_QuantizationInfoEncodingMapping,
                         Qairt_QuantizationInfoEncodingMapping_t,
                         QAIRT_QUANTIZATION_INFO_MAPPING_STANDARD_SYMMETRIC            = 0x00,
                         QAIRT_QUANTIZATION_INFO_MAPPING_ASYMMETRIC_PLUS_ONE           = 0x01,
                         QAIRT_QUANTIZATION_INFO_MAPPING_LINEAR_SYMMETRIC_EXCLUDE_ZERO = 0x02,
                         QAIRT_QUANTIZATION_INFO_MAPPING_UNDEFINED                     = 0x7FFFFFFF)

/**
 * @brief Per-tensor scale and offset quantization parameters for a context tensor.
 *
 *        Obtained via SystemContextQuantizationInfo::getScaleOffset().
 */
class SystemContextScaleOffset : public ApiType<SystemContextScaleOffset,
                                                QairtSystem_Context_QuantizeParams_ScaleOffsetV1_t,
                                                SystemApiTable> {
  friend Api;
  friend SystemApi;

 public:
  using ApiType::ApiType;
  using ApiType::operator=;

  SystemContextScaleOffset() noexcept                                      = default;
  SystemContextScaleOffset(const SystemContextScaleOffset&)                = delete;
  SystemContextScaleOffset(SystemContextScaleOffset&&) noexcept            = default;
  SystemContextScaleOffset& operator=(const SystemContextScaleOffset&)     = delete;
  SystemContextScaleOffset& operator=(SystemContextScaleOffset&&) noexcept = default;

  /// @see QairtSystem_Context_QuantizeParams_ScaleOffset_getScale
  float getScale() const { return callGetter<&interface_type::getScale>(); }
  /// @see QairtSystem_Context_QuantizeParams_ScaleOffset_getOffset
  int32_t getOffset() const { return callGetter<&interface_type::getOffset>(); }

 private:
  void prepareToCross() const {}
  void updateAfterCross() const {}
};

/**
 * @brief Per-axis (e.g., per-channel) scale and offset quantization parameters for a context tensor.
 *
 *        Obtained via SystemContextQuantizationInfo::getAxisScaleOffset().
 */
class SystemContextAxisScaleOffset
    : public ApiType<SystemContextAxisScaleOffset,
                     QairtSystem_Context_QuantizeParams_AxisScaleOffsetV1_t,
                     SystemApiTable> {
  friend Api;
  friend SystemApi;
  /// @brief Per-axis collection of scale-offset pairs, lazy-loaded after a C API cross.
  detail::crossable<std::vector<detail::non_owning<SystemContextScaleOffset>>,
                    &interface_type::getScaleOffsetAt,
                    &interface_type::getNumScaleOffsets>
      m_scaleOffsets;

 public:
  using ApiType::ApiType;
  using ApiType::operator=;

  SystemContextAxisScaleOffset() noexcept                                          = default;
  SystemContextAxisScaleOffset(const SystemContextAxisScaleOffset&)                = delete;
  SystemContextAxisScaleOffset(SystemContextAxisScaleOffset&&) noexcept            = default;
  SystemContextAxisScaleOffset& operator=(const SystemContextAxisScaleOffset&)     = delete;
  SystemContextAxisScaleOffset& operator=(SystemContextAxisScaleOffset&&) noexcept = default;

  /// @see QairtSystem_Context_QuantizeParams_AxisScaleOffset_getAxis
  int32_t getAxis() const { return callGetter<&interface_type::getAxis>(); }

  /**
   * @brief Get the collection of per-axis scale-offset pairs.
   *
   * Returns a reference to the cached vector of SystemContextScaleOffset objects,
   * one per axis element. The vector is populated on first access via a C API cross.
   *
   * @return Reference to the per-axis scale-offset vector.
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtSystem_Context_QuantizeParams_AxisScaleOffset_getScaleOffsetAt
   */
  std::vector<SystemContextScaleOffset>& getScaleOffsets() { return m_scaleOffsets.get(*this); }

  /// @see SystemContextAxisScaleOffset::getScaleOffsets()
  const std::vector<SystemContextScaleOffset>& getScaleOffsets() const {
    return m_scaleOffsets.get(*this);
  }

 private:
  void prepareToCross() const {}
  void updateAfterCross() const { m_scaleOffsets.updateAfterCross(*this); }
};

/**
 * @brief Per-tensor bit-width, scale, and offset quantization parameters for a context tensor.
 *
 *        Obtained via SystemContextQuantizationInfo::getBwScaleOffset().
 */
class SystemContextBwScaleOffset
    : public ApiType<SystemContextBwScaleOffset,
                     QairtSystem_Context_QuantizeParams_BwScaleOffsetV1_t,
                     SystemApiTable> {
  friend Api;
  friend SystemApi;

 public:
  using ApiType::ApiType;
  using ApiType::operator=;

  SystemContextBwScaleOffset() noexcept                                        = default;
  SystemContextBwScaleOffset(const SystemContextBwScaleOffset&)                = delete;
  SystemContextBwScaleOffset& operator=(SystemContextBwScaleOffset&&) noexcept = default;
  SystemContextBwScaleOffset& operator=(const SystemContextBwScaleOffset&)     = delete;
  SystemContextBwScaleOffset(SystemContextBwScaleOffset&&) noexcept            = default;

  /// @see QairtSystem_Context_QuantizeParams_BwScaleOffset_getScale
  float getScale() const { return callGetter<&interface_type::getScale>(); }
  /// @see QairtSystem_Context_QuantizeParams_BwScaleOffset_getOffset
  int32_t getOffset() const { return callGetter<&interface_type::getOffset>(); }
  /// @see QairtSystem_Context_QuantizeParams_BwScaleOffset_getBw
  uint32_t getBw() const { return callGetter<&interface_type::getBw>(); }

 private:
  void prepareToCross() const {}
  void updateAfterCross() const {}
};

/**
 * @brief Per-axis bit-width, scale, and offset quantization parameters for a context tensor.
 *
 *        Obtained via SystemContextQuantizationInfo::getBwAxisScaleOffset().
 */
class SystemContextBwAxisScaleOffset
    : public ApiType<SystemContextBwAxisScaleOffset,
                     QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetV1_t,
                     SystemApiTable> {
  friend Api;
  friend SystemApi;

  /// @brief Cached per-axis scale values, populated after a C API cross.
  mutable std::vector<float> m_scales;
  /// @brief Cached per-axis offset values, populated after a C API cross.
  mutable std::vector<int32_t> m_offsets;

 public:
  using ApiType::ApiType;
  using ApiType::operator=;

  SystemContextBwAxisScaleOffset() noexcept                                            = default;
  SystemContextBwAxisScaleOffset(const SystemContextBwAxisScaleOffset&)                = delete;
  SystemContextBwAxisScaleOffset(SystemContextBwAxisScaleOffset&&) noexcept            = default;
  SystemContextBwAxisScaleOffset& operator=(const SystemContextBwAxisScaleOffset&)     = delete;
  SystemContextBwAxisScaleOffset& operator=(SystemContextBwAxisScaleOffset&&) noexcept = default;

  /// @see QairtSystem_Context_QuantizeParams_BwAxisScaleOffset_getBw
  uint32_t getBitwidth() const { return callGetter<&interface_type::getBw>(); }
  /// @see QairtSystem_Context_QuantizeParams_BwAxisScaleOffset_getAxis
  int32_t getAxis() const { return callGetter<&interface_type::getAxis>(); }

  /**
   * @brief Get the cached per-axis scale values.
   *
   * @return Const reference to the per-axis scale values.
   *
   * @see QairtSystem_Context_QuantizeParams_BwAxisScaleOffset_getScaleAt
   */
  const std::vector<float>& getScales() const { return m_scales; }

  /**
   * @brief Get the cached per-axis offset values.
   *
   * @return Const reference to the per-axis offset values.
   *
   * @see QairtSystem_Context_QuantizeParams_BwAxisScaleOffset_getOffsetAt
   */
  const std::vector<int32_t>& getOffsets() const { return m_offsets; }

 private:
  void prepareToCross() const {}
  void updateAfterCross() const {
    uint32_t numScales{0u};
    callAndConfirm<&interface_type::getNumScales>(&numScales);
    if (numScales > 0) {
      m_scales.resize(numScales);
      for (uint32_t i = 0; i < numScales; i++) {
        callAndConfirm<&interface_type::getScaleAt>(i, &m_scales[i]);
      }
    }

    uint32_t numOffsets{0u};
    callAndConfirm<&interface_type::getNumOffsets>(&numOffsets);
    if (numOffsets > 0) {
      m_offsets.resize(numOffsets);
      for (uint32_t i = 0u; i < numOffsets; ++i) {
        callAndConfirm<&interface_type::getOffsetAt>(i, &m_offsets[i]);
      }
    }
  }
};

/**
 * @brief Per-axis bit-width, scale, and offset quantization parameters with a value-mapping scheme.
 *
 *        Obtained via SystemContextQuantizationInfo::getBwAxisScaleOffsetMapped().
 */
class SystemContextBwAxisScaleOffsetMapped
    : public ApiType<SystemContextBwAxisScaleOffsetMapped,
                     QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMappedV1_t,
                     SystemApiTable> {
  friend Api;
  friend SystemApi;

  /// @brief Cached per-axis scale values, populated after a C API cross.
  mutable std::vector<float> m_scales;
  /// @brief Cached per-axis offset values, populated after a C API cross.
  mutable std::vector<int32_t> m_offsets;

 public:
  using ApiType::ApiType;
  using ApiType::operator=;

  SystemContextBwAxisScaleOffsetMapped() noexcept                                       = default;
  SystemContextBwAxisScaleOffsetMapped(const SystemContextBwAxisScaleOffsetMapped&)     = delete;
  SystemContextBwAxisScaleOffsetMapped(SystemContextBwAxisScaleOffsetMapped&&) noexcept = default;
  SystemContextBwAxisScaleOffsetMapped& operator=(const SystemContextBwAxisScaleOffsetMapped&) =
      delete;
  SystemContextBwAxisScaleOffsetMapped& operator=(SystemContextBwAxisScaleOffsetMapped&&) noexcept =
      default;

  /// @see QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMapped_getBw
  uint32_t getBitwidth() const { return callGetter<&interface_type::getBw>(); }
  /// @see QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMapped_getAxis
  int32_t getAxis() const { return callGetter<&interface_type::getAxis>(); }

  /**
   * @brief Get the cached per-axis scale values.
   *
   * @return Const reference to the per-axis scale values.
   *
   * @see QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMapped_getScaleAt
   */
  const std::vector<float>& getScales() const { return m_scales; }

  /**
   * @brief Get the cached per-axis offset values.
   *
   * @return Const reference to the per-axis offset values.
   *
   * @see QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMapped_getOffsetAt
   */
  const std::vector<int32_t>& getOffsets() const { return m_offsets; }

  /**
   * @brief Get the quantized value mapping scheme for this encoding.
   *
   * @return The active Qairt_QuantizationInfoEncodingMapping value cast to
   *         QuantizationEncodingMapping.
   *
   * @see QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMapped_getMapping
   */
  QuantizationEncodingMapping getMapping() const {
    return static_cast<QuantizationEncodingMapping>(callGetter<&interface_type::getMapping>());
  }

 private:
  void prepareToCross() const {}

  void updateAfterCross() const {
    uint32_t numScales{0u};
    callAndConfirm<&interface_type::getNumScales>(&numScales);
    if (numScales > 0) {
      m_scales.resize(numScales);
      for (uint32_t i = 0; i < numScales; i++) {
        callAndConfirm<&interface_type::getScaleAt>(i, &m_scales[i]);
      }
    }

    uint32_t numOffsets{0u};
    callAndConfirm<&interface_type::getNumOffsets>(&numOffsets);
    if (numOffsets > 0) {
      m_offsets.resize(numOffsets);
      for (uint32_t i = 0u; i < numOffsets; ++i) {
        callAndConfirm<&interface_type::getOffsetAt>(i, &m_offsets[i]);
      }
    }
  }
};

/**
 * @brief Quantization encoding parameters for a single context tensor.
 *
 *        Obtained via SystemContextTensorInfo::getQuantInfo(). The active
 *        encoding variant (scale-offset, axis-scale-offset, bit-width variants,
 *        or mapped) is identified by getQuantizationType() and then retrieved
 *        via the corresponding typed accessor.
 */
class SystemContextQuantizationInfo : public ApiType<SystemContextQuantizationInfo,
                                                     QairtSystem_Context_QuantParamInfoV1_t,
                                                     SystemApiTable> {
  friend SystemApi;
  friend Api;

  using CrossableScaleOffset = detail::crossable<detail::non_owning<SystemContextScaleOffset>,
                                                 &interface_type::getScaleOffset>;
  using CrossableAxisScaleOffset =
      detail::crossable<detail::non_owning<SystemContextAxisScaleOffset>,
                        &interface_type::getAxisScaleOffset>;
  using CrossableBwScaleOffset = detail::crossable<detail::non_owning<SystemContextBwScaleOffset>,
                                                   &interface_type::getBwScaleOffset>;
  using CrossableBwAxisScaleOffset =
      detail::crossable<detail::non_owning<SystemContextBwAxisScaleOffset>,
                        &interface_type::getBwAxisScaleOffset>;

  using CrossableBwAxisScaleOffsetMapped =
      detail::crossable<detail::non_owning<SystemContextBwAxisScaleOffsetMapped>,
                        &interface_type::getBwAxisScaleOffsetMapped>;

  /// @brief Active quantization encoding, held as a type-safe variant populated on first access.
  mutable std::variant<std::monostate,
                       CrossableScaleOffset,
                       CrossableAxisScaleOffset,
                       CrossableBwScaleOffset,
                       CrossableBwAxisScaleOffset,
                       CrossableBwAxisScaleOffsetMapped>
      m_encoding;

 public:
  using base_type::base_type;
  using base_type::operator=;

  /**
   * @brief Get the active quantization encoding type for this tensor.
   *
   * Use the returned type to determine which typed accessor to call next:
   * - SystemContextQuantInfoType::ScaleOffset           → getScaleOffset()
   * - SystemContextQuantInfoType::AxisScaleOffset       → getAxisScaleOffset()
   * - SystemContextQuantInfoType::BwScaleOffset         → getBwScaleOffset()
   * - SystemContextQuantInfoType::BwAxisScaleOffset     → getBwAxisScaleOffset()
   * - SystemContextQuantInfoType::BwAxisScaleOffsetMapped → getBwAxisScaleOffsetMapped()
   * - SystemContextQuantInfoType::Undefined             → tensor is unquantized
   *
   * @return The active SystemContextQuantInfoType enumerator.
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtSystem_Context_QuantParamInfo_getQuantizationType
   */
  SystemContextQuantInfoType getQuantizationType() const {
    return static_cast<SystemContextQuantInfoType>(
        callGetter<&interface_type::getQuantizationType>());
  }

  /**
   * @brief Get the per-tensor scale and offset quantization parameters.
   *
   * Valid only when getQuantizationType() returns
   * SystemContextQuantInfoType::ScaleOffset.
   *
   * @return Const reference to the per-tensor SystemContextScaleOffset encoding.
   * @throws qairt::Exception on invalid handle or wrong encoding type.
   *
   * @see QairtSystem_Context_QuantParamInfo_getScaleOffset
   */
  const SystemContextScaleOffset& getScaleOffset() const {
    if (std::holds_alternative<std::monostate>(m_encoding)) {
      m_encoding = CrossableScaleOffset{};
    }
    std::get<CrossableScaleOffset>(m_encoding).updateAfterCross(*this);
    return std::get<CrossableScaleOffset>(m_encoding).bindNonOwning(*this);
  }

  /**
   * @brief Get the per-axis scale and offset quantization parameters.
   *
   * Valid only when getQuantizationType() returns
   * SystemContextQuantInfoType::AxisScaleOffset.
   *
   * @return Const reference to the per-axis SystemContextAxisScaleOffset encoding.
   * @throws qairt::Exception on invalid handle or wrong encoding type.
   *
   * @see QairtSystem_Context_QuantParamInfo_getAxisScaleOffset
   */
  const SystemContextAxisScaleOffset& getAxisScaleOffset() const {
    std::get<CrossableAxisScaleOffset>(m_encoding).updateAfterCross(*this);
    return std::get<CrossableAxisScaleOffset>(m_encoding).bindNonOwning(*this);
  }

  /**
   * @brief Get the per-tensor bit-width, scale, and offset quantization parameters.
   *
   * Valid only when getQuantizationType() returns
   * SystemContextQuantInfoType::BwScaleOffset.
   *
   * @return Const reference to the per-tensor SystemContextBwScaleOffset encoding.
   * @throws qairt::Exception on invalid handle or wrong encoding type.
   *
   * @see QairtSystem_Context_QuantParamInfo_getBwScaleOffset
   */
  const SystemContextBwScaleOffset& getBwScaleOffset() const {
    std::get<CrossableBwScaleOffset>(m_encoding).updateAfterCross(*this);
    return std::get<CrossableBwScaleOffset>(m_encoding).bindNonOwning(*this);
  }

  /**
   * @brief Get the per-axis bit-width, scale, and offset quantization parameters.
   *
   * Valid only when getQuantizationType() returns
   * SystemContextQuantInfoType::BwAxisScaleOffset.
   *
   * @return Const reference to the per-axis SystemContextBwAxisScaleOffset encoding.
   * @throws qairt::Exception on invalid handle or wrong encoding type.
   *
   * @see QairtSystem_Context_QuantParamInfo_getBwAxisScaleOffset
   */
  const SystemContextBwAxisScaleOffset& getBwAxisScaleOffset() const {
    std::get<CrossableBwAxisScaleOffset>(m_encoding).updateAfterCross(*this);
    return std::get<CrossableBwAxisScaleOffset>(m_encoding).bindNonOwning(*this);
  }

  /**
   * @brief Get the per-axis bit-width, scale, offset, and mapping quantization parameters.
   *
   * Valid only when getQuantizationType() returns
   * SystemContextQuantInfoType::BwAxisScaleOffsetMapped.
   *
   * @return Const reference to the per-axis SystemContextBwAxisScaleOffsetMapped encoding.
   * @throws qairt::Exception on invalid handle or wrong encoding type.
   *
   * @see QairtSystem_Context_QuantParamInfo_getBwAxisScaleOffsetMapped
   */
  const SystemContextBwAxisScaleOffsetMapped& getBwAxisScaleOffsetMapped() const {
    std::get<CrossableBwAxisScaleOffsetMapped>(m_encoding).updateAfterCross(*this);
    return std::get<CrossableBwAxisScaleOffsetMapped>(m_encoding).bindNonOwning(*this);
  }

 private:
  void prepareToCross() const {}
  void updateAfterCross() const {}
};

/**
 * @brief Metadata for a single tensor within a compiled context graph.
 *
 *        Obtained via SystemContextGraphInfo::getGraphInputs() or
 *        getGraphOutputs(). Provides the tensor's name, rank, dimensions,
 *        data type, and quantization parameters.
 */
class SystemContextTensorInfo
    : public ApiType<SystemContextTensorInfo, QairtSystem_Context_TensorInfoV1_t, SystemApiTable> {
  friend SystemApi;
  friend Api;
  /// @brief Tensor name string, lazy-loaded from the C API after a cross.
  detail::crossable<std::string, &interface_type::getName> m_name;
  /// @brief Quantization parameter info, lazy-loaded from the C API after a cross.
  detail::crossable<detail::non_owning<SystemContextQuantizationInfo>,
                    &interface_type::getQuantInfo>
      m_quantInfo;

 public:
  using base_type::base_type;
  using base_type::operator=;
  SystemContextTensorInfo()                                              = default;
  SystemContextTensorInfo(const SystemContextTensorInfo&)                = delete;
  SystemContextTensorInfo(SystemContextTensorInfo&&) noexcept            = default;
  SystemContextTensorInfo& operator=(const SystemContextTensorInfo&)     = delete;
  SystemContextTensorInfo& operator=(SystemContextTensorInfo&&) noexcept = default;

  /// @see QairtSystem_Context_TensorInfo_getId
  uint64_t getId() const { return callGetter<&QairtSystem_Context_TensorInfoV1_t::getId>(); }

  /**
   * @brief Get the name of this tensor.
   *
   * @return The tensor name string (e.g. "input_0" or "output_logits").
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtSystem_Context_TensorInfo_getName
   */
  std::string getName() const { return m_name.get(); }

  /// @see QairtSystem_Context_TensorInfo_getRank
  uint32_t getRank() const { return callGetter<&QairtSystem_Context_TensorInfoV1_t::getRank>(); }

  /**
   * @brief Get a pointer to the dimension sizes array for this tensor.
   *
   * @return Pointer to the dimension sizes array. The array has getRank() elements.
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtSystem_Context_TensorInfo_getDimensions
   */
  const uint32_t* getDimensions() {
    return callGetter<&QairtSystem_Context_TensorInfoV1_t::getDimensions>();
  }

  /**
   * @brief Get the data type of this tensor.
   *
   * @return The DataType enumerator for this tensor (e.g. DataType::Float32,
   *         DataType::Ufixed8, etc.).
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtSystem_Context_TensorInfo_getDatatype
   */
  DataType getDataType() {
    return static_cast<DataType>(callGetter<&QairtSystem_Context_TensorInfoV1_t::getDatatype>());
  }

  /**
   * @brief Get the quantization parameter information for this tensor.
   *
   * Returns a reference to the cached SystemContextQuantizationInfo object.
   * Call getQuantizationType() on the returned object to determine the active
   * encoding, then call the corresponding typed accessor.
   *
   * @return Reference to the quantization info for this tensor.
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtSystem_Context_TensorInfo_getQuantInfo
   */
  const SystemContextQuantizationInfo& getQuantInfo() { return m_quantInfo.bindNonOwning(*this); }

 private:
  void prepareToCross() const { prepareMultipleToCross(m_name, m_quantInfo); }
  void updateAfterCross() const { updateMultipleAfterCross(m_name, m_quantInfo); }
};

/**
 * @brief Metadata for a single compiled graph within a context binary.
 *
 *        Obtained via SystemContextGraphInfoSet::getGraphInfos() or
 *        Qairt::getGraphInfoAt(). Provides the graph name and the ordered
 *        input, output, and updateable tensor metadata lists.
 */
class SystemContextGraphInfo
    : public ApiType<SystemContextGraphInfo, QairtSystem_Context_GraphInfoV1_t, SystemApiTable> {
  friend SystemApi;
  friend Api;

  /// @brief Graph name string, lazy-loaded from the C API after a cross.
  detail::crossable<std::string, &interface_type::getName> m_name;
  /// @brief Input tensor info collection, lazy-loaded from the C API after a cross.
  detail::crossable<std::vector<detail::non_owning<qairt::SystemContextTensorInfo>>,
                    &interface_type::getInputAt,
                    &interface_type::getNumInputs>
      m_inputTensorInfos;

  /// @brief Output tensor info collection, lazy-loaded from the C API after a cross.
  detail::crossable<std::vector<detail::non_owning<qairt::SystemContextTensorInfo>>,
                    &interface_type::getOutputAt,
                    &interface_type::getNumOutputs>
      m_outputTensorInfos;

  /// @brief Updateable tensor info collection, lazy-loaded from the C API after a cross.
  detail::crossable<std::vector<detail::non_owning<qairt::SystemContextTensorInfo>>,
                    &interface_type::getUpdateableAt,
                    &interface_type::getNumUpdateables>
      m_updateableTensorInfos;

 public:
  using base_type::base_type;
  using base_type::operator=;
  SystemContextGraphInfo()                                             = default;
  SystemContextGraphInfo(const SystemContextGraphInfo&)                = delete;
  SystemContextGraphInfo(SystemContextGraphInfo&&) noexcept            = default;
  SystemContextGraphInfo& operator=(const SystemContextGraphInfo&)     = delete;
  SystemContextGraphInfo& operator=(SystemContextGraphInfo&&) noexcept = default;

  /**
   * @brief Get the name of this graph.
   *
   * @return Const reference to the graph name string.
   *
   * @see QairtSystem_Context_GraphInfo_getName
   */
  const std::string& getGraphName() const { return m_name.get(); }

  /**
   * @brief Get the ordered list of input tensor metadata for this graph.
   *
   * @return Reference to the input tensor info vector.
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtSystem_Context_GraphInfo_getInputAt
   */
  std::vector<qairt::SystemContextTensorInfo>& getGraphInputs() {
    return m_inputTensorInfos.get(*this);
  }

  /// @see SystemContextGraphInfo::getGraphInputs()
  const std::vector<qairt::SystemContextTensorInfo>& getGraphInputs() const {
    return m_inputTensorInfos.get(*this);
  }

  /**
   * @brief Get the ordered list of output tensor metadata for this graph.
   *
   * @return Reference to the output tensor info vector.
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtSystem_Context_GraphInfo_getOutputAt
   */
  std::vector<qairt::SystemContextTensorInfo>& getGraphOutputs() {
    return m_outputTensorInfos.get(*this);
  }

  /// @see SystemContextGraphInfo::getGraphOutputs()
  const std::vector<qairt::SystemContextTensorInfo>& getGraphOutputs() const {
    return m_outputTensorInfos.get(*this);
  }

  /**
   * @brief Get the ordered list of updateable tensor metadata for this graph.
   *
   * Updateable tensors are model weights that can be replaced at runtime
   * (e.g. for on-device fine-tuning or weight swapping). The returned vector
   * follows the same format as getGraphInputs().
   *
   * @return Reference to the updateable tensor info vector.
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtSystem_Context_GraphInfo_getUpdateableAt
   */
  std::vector<qairt::SystemContextTensorInfo>& getUpdateableTensors() {
    return m_updateableTensorInfos.get(*this);
  }

  /// @see SystemContextGraphInfo::getUpdateableTensors()
  const std::vector<qairt::SystemContextTensorInfo>& getUpdateableTensors() const {
    return m_updateableTensorInfos.get(*this);
  }

  /**
   * @brief Get the size in bytes of the raw graph-info serialization blob.
   *
   * @return Size in bytes. 0 if the backend did not populate a blob.
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtSystem_Context_GraphInfo_getGraphInfoBlobSize
   */
  uint32_t getGraphBlobInfoSize() const {
    return callGetter<&interface_type::getGraphInfoBlobSize>();
  }

  /**
   * @brief Get a pointer to the raw graph-info serialization blob.
   *
   * @return Pointer to the blob data, or nullptr if no blob was populated.
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtSystem_Context_GraphInfo_getGraphInfoBlob
   */
  void* getGraphBlobInfo() const { return callGetter<&interface_type::getGraphInfoBlob>(); }

  /**
   * @brief Get the start operation index for this graph in the context's op table.
   *
   * @return Zero-based index of the first operation belonging to this graph.
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtSystem_Context_GraphInfo_getStartOpIndex
   */
  uint32_t getStartOpIndex() const { return callGetter<&interface_type::getStartOpIndex>(); }

  /**
   * @brief Get the end operation index for this graph in the context's op table.
   *
   * @return One-past-the-last index of the operations belonging to this graph.
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtSystem_Context_GraphInfo_getEndOpIndex
   */
  uint32_t getEndOpIndex() const { return callGetter<&interface_type::getEndOpIndex>(); }

 private:
  void prepareToCross() const {
    prepareMultipleToCross(
        m_inputTensorInfos, m_outputTensorInfos, m_updateableTensorInfos, m_name);
  }
  void updateAfterCross() const {
    updateMultipleAfterCross(
        m_inputTensorInfos, m_outputTensorInfos, m_updateableTensorInfos, m_name);
  }
};

class SystemDlc;

/**
 * @brief Ordered collection of compiled graph metadata returned by SystemDlc::composeGraphs().
 *
 *        Obtained via SystemDlc::composeGraphs(). Each entry exposes the graph
 *        name and its input, output, and updateable tensor metadata.
 */
class SystemContextGraphInfoSet : public ApiType<SystemContextGraphInfoSet,
                                                 QairtSystem_Context_GraphInfoSetV1_t,
                                                 SystemApiTable> {
  friend SystemApi;
  friend SystemDlc;
  friend Api;
  /// @brief Graph info collection for the set, populated on construction.
  detail::crossable<std::vector<detail::non_owning<qairt::SystemContextGraphInfo>>,
                    &interface_type::getGraphInfoAt,
                    &interface_type::getNumGraphInfos>
      m_graphInfos;

 public:
  using base_type::base_type;
  using base_type::operator=;
  SystemContextGraphInfoSet()                                                = default;
  SystemContextGraphInfoSet(const SystemContextGraphInfoSet&)                = delete;
  SystemContextGraphInfoSet(SystemContextGraphInfoSet&&) noexcept            = default;
  SystemContextGraphInfoSet& operator=(const SystemContextGraphInfoSet&)     = delete;
  SystemContextGraphInfoSet& operator=(SystemContextGraphInfoSet&&) noexcept = default;

  /**
   * @brief Get the ordered list of compiled graph metadata in this set.
   *
   * @return Reference to the graph info vector.
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtSystem_Context_GraphInfoSet_getGraphInfoAt
   */
  std::vector<qairt::SystemContextGraphInfo>& getGraphInfos() { return m_graphInfos.get(*this); }

  /// @see SystemContextGraphInfoSet::getGraphInfos()
  const std::vector<qairt::SystemContextGraphInfo>& getGraphInfos() const {
    return m_graphInfos.get(*this);
  }

 private:
  void prepareToCross() const {}
  void updateAfterCross() const { m_graphInfos.updateAfterCross(*this); }
  SystemContextGraphInfoSet(const std::shared_ptr<SystemApiTable>& apiTable,
                            QairtSystem_Context_GraphInfoSetHandle_t handle)
      : ApiType(apiTable) {
    this->setHandle(handle);
    updateAfterCross();
  }
};

/**
 * @brief Metadata extracted from a serialized context binary (e.g., a compiled DLC).
 *
 *        Obtained via SystemContext::getMetadata(). Provides version information,
 *        build identifiers, SoC versions, and the graph metadata contained in the
 *        binary.
 */
class SystemContextBinaryInfo
    : public ApiType<SystemContextBinaryInfo, QairtSystem_Context_BinaryInfoV1_t, SystemApiTable> {
  friend SystemApi;
  friend Api;

  /// @brief Graph info collection for the binary, lazy-loaded from the C API after a cross.
  detail::crossable<std::vector<detail::non_owning<SystemContextGraphInfo>>,
                    &interface_type::getGraphInfoAt,
                    &interface_type::getNumGraphInfos>
      m_graphInfos;
  detail::crossable<detail::non_owning<DevicePlatformInfo>,
                    &interface_type::getDevicePlatformInfo,
                    nullptr>
      m_platformInfo;
  /// @brief Build identifier string, lazy-loaded from the C API after a cross.
  detail::crossable<std::string, &interface_type::getBuildId> m_buildId;

 public:
  using base_type::base_type;
  using base_type::operator=;

  /**
   * @brief Get the numeric backend identifier recorded in the binary.
   *
   * @return Backend ID as an unsigned integer. Corresponds to a backend-specific
   *         constant (e.g. the HTP backend ID).
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtSystem_Context_BinaryInfo_getBackendId
   */
  uint32_t getBackendId() { return callGetter<&interface_type::getBackendId>(); }

  /**
   * @brief Get the build identifier string recorded in the binary.
   *
   * @return Const reference to the build ID string.
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtSystem_Context_BinaryInfo_getBuildId
   */
  const std::string& getBuildId() const { return m_buildId.get(); }

  /**
   * @brief Get the QAIRT core API version recorded in the binary.
   *
   * @return A Version struct with the major, minor, and patch version numbers of
   *         the QAIRT core API used when the binary was compiled.
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtSystem_Context_BinaryInfo_getApiVersion
   */
  const Version getCoreApiVersion() const {
    Version toret{};
    QAIRT_NO_RETURN call<&interface_type::getApiVersion>(&toret.major, &toret.minor, &toret.patch);
    return toret;
  }

  /**
   * @brief Get the backend API version recorded in the binary.
   *
   * @return A Version struct with the major, minor, and patch version numbers of
   *         the backend API used when the binary was compiled.
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtSystem_Context_BinaryInfo_getBackendApiVersion
   */
  const Version getBackendApiVersion() const {
    Version toret{};
    QAIRT_NO_RETURN call<&interface_type::getBackendApiVersion>(
        &toret.major, &toret.minor, &toret.patch);
    return toret;
  }

  /**
   * @brief Get the System-on-Chip (SoC) version string recorded in the binary.
   *
   * Currently reads only the first SoC version entry. Returns an empty string
   * if the binary contains no SoC version information.
   *
   * @return SoC version string, or an empty string if not present.
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtSystem_Context_BinaryInfo_getSocVersionAt
   */
  std::string getSocVersion() const {
    // C API has getNumSocVersions/getSocVersionAt, but for now we only support one
    uint32_t numVersions = 0;
    QAIRT_NO_RETURN call<&interface_type::getNumSocVersions>(&numVersions);
    if (numVersions > 0) {
      const char* socVersion = nullptr;
      QAIRT_NO_RETURN call<&interface_type::getSocVersionAt>(0, &socVersion);
      return socVersion ? std::string(socVersion) : std::string();
    }
    return std::string();
  }
  std::vector<SystemContextGraphInfo>& getGraphs() { return m_graphInfos.get(*this); }
  const std::vector<SystemContextGraphInfo>& getGraphs() const { return m_graphInfos.get(*this); }
  const DevicePlatformInfo& getPlatformInfo() const { return m_platformInfo.bindNonOwning(*this); }
  uint32_t getSocModel() const { return callGetter<&interface_type::getSocModel>(); }
  
 private:
  void prepareToCross() const {
    prepareMultipleToCross(m_graphInfos, m_platformInfo, m_buildId);
  }
  void updateAfterCross() const {
    updateMultipleAfterCross(m_graphInfos, m_platformInfo, m_buildId);
  }
};

/**
 * @brief Handle for inspecting compiled context binaries.
 *
 *        Obtained via `SystemApi::make<SystemContext>()`.
 */
class SystemContext : public ApiType<SystemContext, QairtSystem_ContextV1_t, SystemApiTable> {
  friend SystemApi;
  friend Api;

 public:
  ~SystemContext()                                   = default;
  SystemContext() noexcept                           = default;
  SystemContext(const SystemContext&)                = delete;
  SystemContext(SystemContext&&) noexcept            = default;
  SystemContext& operator=(const SystemContext&)     = delete;
  SystemContext& operator=(SystemContext&&) noexcept = default;

  SystemContextBinaryInfo getBinaryInfo(void* binaryBuffer, uint64_t binaryBufferSize) {
    QairtSystem_Context_BinaryInfoHandle_t binaryInfo;
    callAndConfirm<&QairtSystem_ContextV1_t::getBinaryInfo>(
        binaryBuffer, binaryBufferSize, &binaryInfo);
    return constructNonOwning<SystemContextBinaryInfo>(binaryInfo);
  }
  SystemContextBinaryInfo getBinaryInfo(Buffer& buffer) {
    return getBinaryInfo(buffer.getData(), buffer.getDataSize());
  }

 private:
  explicit SystemContext(const std::shared_ptr<SystemApiTable>& apiTable) : ApiType(apiTable) {
    construct();
  }
};

}  // namespace qairt

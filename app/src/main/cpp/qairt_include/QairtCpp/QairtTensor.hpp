//==============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All Rights Reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//==============================================================================

/**
 * @file
 * @brief  C++ wrapper for the QAIRT tensor API.
 *
 *         Provides data-type, quantization-encoding, and tensor classes used
 *         to describe and transfer tensor data across QAIRT backends.
 *
 */

#pragma once

#include <algorithm>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include "QairtApi.hpp"
#include "QairtBuffer.hpp"
#include "QairtCommon/QairtScalar.h"
#include "QairtQuantizeParams/QairtQuantizeParams.h"
#include "QairtTensor/QairtTensor.h"

namespace qairt {

/**
 * @brief Element data types supported for tensor data.
 *
 * @details
 * | Enumerator       | Description                                      |
 * |------------------|--------------------------------------------------|
 * | `Int2`           | Signed 2-bit integer.                            |
 * | `Int4`           | Signed 4-bit integer.                            |
 * | `Int8`           | Signed 8-bit integer.                            |
 * | `Int16`          | Signed 16-bit integer.                           |
 * | `Int32`          | Signed 32-bit integer.                           |
 * | `Int64`          | Signed 64-bit integer.                           |
 * | `UInt2`          | Unsigned 2-bit integer.                          |
 * | `UInt4`          | Unsigned 4-bit integer.                          |
 * | `UInt8`          | Unsigned 8-bit integer.                          |
 * | `UInt16`         | Unsigned 16-bit integer.                         |
 * | `UInt32`         | Unsigned 32-bit integer.                         |
 * | `UInt64`         | Unsigned 64-bit integer.                         |
 * | `Float4`         | 4-bit floating-point.                            |
 * | `Float8`         | 8-bit floating-point.                            |
 * | `Float16`        | 16-bit floating-point (IEEE 754 half precision). |
 * | `BFloat16`       | 16-bit brain floating-point (bfloat16).          |
 * | `Float32`        | 32-bit floating-point (IEEE 754 single).         |
 * | `Float64`        | 64-bit floating-point (IEEE 754 double).         |
 * | `SFixedPoint2`   | Signed 2-bit fixed-point.                        |
 * | `SFixedPoint4`   | Signed 4-bit fixed-point.                        |
 * | `SFixedPoint8`   | Signed 8-bit fixed-point.                        |
 * | `SFixedPoint16`  | Signed 16-bit fixed-point.                       |
 * | `SFixedPoint32`  | Signed 32-bit fixed-point.                       |
 * | `UFixedPoint2`   | Unsigned 2-bit fixed-point.                      |
 * | `UFixedPoint4`   | Unsigned 4-bit fixed-point.                      |
 * | `UFixedPoint8`   | Unsigned 8-bit fixed-point.                      |
 * | `UFixedPoint16`  | Unsigned 16-bit fixed-point.                     |
 * | `UFixedPoint32`  | Unsigned 32-bit fixed-point.                     |
 * | `Bool8`          | 8-bit boolean.                                   |
 * | `String`         | Variable-length string.                          |
 * | `Undefined`      | Unspecified or unknown data type.                |
 */
QAIRT_CPPAPI_DEFINE_ENUM(DataType,
                         Qairt_DataType_t,
                         Int2          = QAIRT_DATATYPE_INT_2,
                         Int4          = QAIRT_DATATYPE_INT_4,
                         Int8          = QAIRT_DATATYPE_INT_8,
                         Int16         = QAIRT_DATATYPE_INT_16,
                         Int32         = QAIRT_DATATYPE_INT_32,
                         Int64         = QAIRT_DATATYPE_INT_64,
                         UInt2         = QAIRT_DATATYPE_UINT_2,
                         UInt4         = QAIRT_DATATYPE_UINT_4,
                         UInt8         = QAIRT_DATATYPE_UINT_8,
                         UInt16        = QAIRT_DATATYPE_UINT_16,
                         UInt32        = QAIRT_DATATYPE_UINT_32,
                         UInt64        = QAIRT_DATATYPE_UINT_64,
                         Float4        = QAIRT_DATATYPE_FLOAT_4,
                         Float8        = QAIRT_DATATYPE_FLOAT_8,
                         Float16       = QAIRT_DATATYPE_FLOAT_16,
                         BFloat16      = QAIRT_DATATYPE_BFLOAT_16,
                         Float32       = QAIRT_DATATYPE_FLOAT_32,
                         Float64       = QAIRT_DATATYPE_FLOAT_64,
                         SFixedPoint2  = QAIRT_DATATYPE_SFIXED_POINT_2,
                         SFixedPoint4  = QAIRT_DATATYPE_SFIXED_POINT_4,
                         SFixedPoint8  = QAIRT_DATATYPE_SFIXED_POINT_8,
                         SFixedPoint16 = QAIRT_DATATYPE_SFIXED_POINT_16,
                         SFixedPoint32 = QAIRT_DATATYPE_SFIXED_POINT_32,
                         UFixedPoint2  = QAIRT_DATATYPE_UFIXED_POINT_2,
                         UFixedPoint4  = QAIRT_DATATYPE_UFIXED_POINT_4,
                         UFixedPoint8  = QAIRT_DATATYPE_UFIXED_POINT_8,
                         UFixedPoint16 = QAIRT_DATATYPE_UFIXED_POINT_16,
                         UFixedPoint32 = QAIRT_DATATYPE_UFIXED_POINT_32,
                         Bool8         = QAIRT_DATATYPE_BOOL_8,
                         String        = QAIRT_DATATYPE_STRING,
                         Undefined     = QAIRT_DATATYPE_UNDEFINED)

/**
 * @brief Quantization encoding types for tensor data.
 *
 * @details
 * | Enumerator                   | Description                                                     |
 * |------------------------------|-----------------------------------------------------------------|
 * | `ScaleOffset`                | Per-tensor scale-offset encoding.                               |
 * | `AxisScaleOffset`            | Per-axis (e.g., per-channel) scale-offset encoding.             |
 * | `BwScaleOffset`              | Bit-width scale-offset encoding.                                |
 * | `BwAxisScaleOffset`          | Bit-width per-axis scale-offset encoding.                       |
 * | `Block`                      | Per-block scale-offset encoding.                                |
 * | `BlockwiseExpansion`         | Blockwise expansion encoding.                                   |
 * | `Vector`                     | Vector quantization (VQ) compression encoding.                  |
 * | `BwAxisScaleOffsetMapped`    | Bit-width per-axis scale-offset encoding with mapping.          |
 * | `BwBlockMapped`              | Bit-width per-block scale-offset encoding with mapping.         |
 * | `BwBlockwiseExpansionMapped` | Bit-width blockwise expansion encoding with mapping.            |
 * | `FloatBlock`                 | Per-block float scale-offset encoding.                          |
 * | `BwFloatBlock`               | Bit-width per-block float scale-offset encoding.                |
 * | `Microscaling`               | Microscaling (MX) encoding.                                     |
 * | `Undefined`                  | Unused sentinel; present to ensure a 32-bit enum storage.       |
 */
QAIRT_CPPAPI_DEFINE_ENUM(
    QuantizationEncoding,
    Qairt_QuantizationEncoding_t,
    ScaleOffset                 = QAIRT_QUANTIZATION_ENCODING_SCALE_OFFSET,
    AxisScaleOffset             = QAIRT_QUANTIZATION_ENCODING_AXIS_SCALE_OFFSET,
    BwScaleOffset               = QAIRT_QUANTIZATION_ENCODING_BW_SCALE_OFFSET,
    BwAxisScaleOffset           = QAIRT_QUANTIZATION_ENCODING_BW_AXIS_SCALE_OFFSET,
    Block                       = QAIRT_QUANTIZATION_ENCODING_BLOCK,
    BlockwiseExpansion          = QAIRT_QUANTIZATION_ENCODING_BLOCKWISE_EXPANSION,
    Vector                      = QAIRT_QUANTIZATION_ENCODING_VECTOR,
    BwAxisScaleOffsetMapped     = QAIRT_QUANTIZATION_ENCODING_BW_AXIS_SCALE_OFFSET_MAPPED,
    BwBlockMapped               = QAIRT_QUANTIZATION_ENCODING_BW_BLOCK_MAPPED,
    BwBlockwiseExpansionMapped  = QAIRT_QUANTIZATION_ENCODING_BW_BLOCKWISE_EXPANSION_MAPPED,
    FloatBlock                  = QAIRT_QUANTIZATION_ENCODING_FLOAT_BLOCK,
    BwFloatBlock                = QAIRT_QUANTIZATION_ENCODING_BW_FLOAT_BLOCK,
    Microscaling                = QAIRT_QUANTIZATION_ENCODING_MICROSCALING,
    Undefined                   = QAIRT_QUANTIZATION_ENCODING_UNDEFINED)

/**
 * @brief Quantized value mapping schemes for scale-offset encodings.
 *
 * @details
 * | Enumerator                    | Description                                                             |
 * |-------------------------------|-------------------------------------------------------------------------|
 * | `StandardSymmetric`           | Standard symmetric two's complement mapping.                            |
 * | `AsymmetricPlusOne`           | Two's complement mapping with a positive shift of one.                  |
 * | `LinearSymmetricExcludeZero`  | Linear mapping symmetric about zero, excluding zero from the range.     |
 * | `Undefined`                   | Unused sentinel; present to ensure a 32-bit enum storage.               |
 */
QAIRT_CPPAPI_DEFINE_ENUM(
    QuantizationEncodingMapping,
    Qairt_QuantizationEncodingMapping_t,
    StandardSymmetric            = QAIRT_QUANTIZATION_ENCODING_MAPPING_STANDARD_SYMMETRIC,
    AsymmetricPlusOne            = QAIRT_QUANTIZATION_ENCODING_MAPPING_ASYMMETRIC_PLUS_ONE,
    LinearSymmetricExcludeZero   = QAIRT_QUANTIZATION_ENCODING_MAPPING_LINEAR_SYMMETRIC_EXCLUDE_ZERO,
    Undefined                    = QAIRT_QUANTIZATION_ENCODING_MAPPING_UNDEFINED)

/**
 * @brief Floating-point sub-format encodings used in Microscaling (MX) quantization.
 *
 * @details
 * | Enumerator    | Description                                          |
 * |---------------|------------------------------------------------------|
 * | `MXFP8_E5M2`  | MXFP8 format with 5 exponent bits and 2 mantissa bits; compatible with Float8. |
 * | `MXFP8_E4M3`  | MXFP8 format with 4 exponent bits and 3 mantissa bits; compatible with Float8. |
 * | `MXFP6_E3M2`  | MXFP6 format with 3 exponent bits and 2 mantissa bits.                         |
 * | `MXFP6_E2M3`  | MXFP6 format with 2 exponent bits and 3 mantissa bits.                         |
 * | `MXFP4_E2M1`  | MXFP4 format with 2 exponent bits and 1 mantissa bit.                          |
 * | `Undefined`   | Unused sentinel; present to ensure a 32-bit enum storage.                      |
 */
QAIRT_CPPAPI_DEFINE_ENUM(FloatEncoding,
                         Qairt_FloatEncoding_t,
                         MXFP8_E5M2 = QAIRT_FLOAT_ENCODING_MXFP8_E5M2,
                         MXFP8_E4M3 = QAIRT_FLOAT_ENCODING_MXFP8_E4M3,
                         MXFP6_E3M2 = QAIRT_FLOAT_ENCODING_MXFP6_E3M2,
                         MXFP6_E2M3 = QAIRT_FLOAT_ENCODING_MXFP6_E2M3,
                         MXFP4_E2M1 = QAIRT_FLOAT_ENCODING_MXFP4_E2M1,
                         Undefined  = QAIRT_FLOAT_ENCODING_UNDEFINED)

/**
 * @brief Memory access strategies for tensor data.
 *
 * @details
 * | Enumerator    | Description                                                              |
 * |---------------|--------------------------------------------------------------------------|
 * | `Raw`         | Raw memory pointer provided directly by the client.                      |
 * | `MemHandle`   | Shared memory object handle enabling memory sharing across backends.     |
 * | `RetrieveRaw` | Callback-based retrieval; the backend calls client-supplied callbacks to fetch raw data. |
 * | `Undefined`   | Unused sentinel; present to ensure a 32-bit enum storage.                |
 */
QAIRT_CPPAPI_DEFINE_ENUM(TensorMemType,
                         QairtTensor_MemoryType_t,
                         Raw         = QAIRT_TENSORMEMTYPE_RAW,
                         MemHandle   = QAIRT_TENSORMEMTYPE_MEMHANDLE,
                         RetrieveRaw = QAIRT_TENSORMEMTYPE_RETRIEVE_RAW,
                         Undefined   = QAIRT_TENSORMEMTYPE_UNDEFINED)

/**
 * @brief Storage bit-widths for block scales in blockwise expansion quantization.
 *
 * @details
 * | Enumerator   | Description                                             |
 * |--------------|---------------------------------------------------------|
 * | `Storage8`   | Block scales stored as 8-bit values.                    |
 * | `Storage16`  | Block scales stored as 16-bit values.                   |
 * | `Undefined`  | Unused sentinel; present to ensure a 32-bit enum storage. |
 */
QAIRT_CPPAPI_DEFINE_ENUM(BlockwiseExpansionBlockScaleStorageType,
                         Qairt_BlockwiseExpansionBlockScaleStorageType_t,
                         Storage8  = QAIRT_BLOCKWISE_EXPANSION_BITWIDTH_SCALE_STORAGE_8,
                         Storage16 = QAIRT_BLOCKWISE_EXPANSION_BITWIDTH_SCALE_STORAGE_16,
                         Undefined = QAIRT_BLOCKWISE_EXPANSION_BITWIDTH_SCALE_STORAGE_UNDEFINED)

class TensorMemory;
class Tensor;

/**
 * @brief Raw memory descriptor for tensor data provided by the caller.
 *
 * Construct directly: ClientBuffer(). Associate with a @ref Tensor via
 * @ref TensorMemory::setClientBuffer().
 */
class ClientBuffer : public ApiType<ClientBuffer, QairtTensor_ClientBufferV1_t> {
  friend Api;
  friend class TensorMemory;
  friend class Tensor;

 public:
  ClientBuffer() noexcept                          = default;
  ClientBuffer(const ClientBuffer&)                = delete;
  ClientBuffer& operator=(const ClientBuffer&)     = delete;
  ClientBuffer(ClientBuffer&&) noexcept            = default;
  ClientBuffer& operator=(ClientBuffer&&) noexcept = default;

  /// @see QairtTensor_ClientBuffer_getData
  void* getData() { return callGetter<&interface_type::getData>(); }
  /// @see QairtTensor_ClientBuffer_getData
  const void* getData() const { return callGetter<&interface_type::getData>(); }
  /// @see QairtTensor_ClientBuffer_setData
  void setData(void* data) { callSetter<&interface_type::setData>(data); }

  /// @see QairtTensor_ClientBuffer_getDataSize
  uint32_t getDataSize() const { return callGetter<&interface_type::getDataSize>(); }
  /// @see QairtTensor_ClientBuffer_setDataSize
  void setDataSize(uint32_t dataSize) { callSetter<&interface_type::setDataSize>(dataSize); }

 private:
  explicit ClientBuffer(const std::shared_ptr<ApiTable>& apiTable) : ApiType(apiTable) {
    construct();
  }

  void prepareToCross() const {}
  void updateAfterCross() const {}
};

/**
 * @brief Attribute flags describing the role and usage of a tensor within a graph.
 *
 * Construct directly: TensorProperties(). Attach to a @ref Tensor via
 * @ref Tensor::setTensorProperties().
 */
class TensorProperties : public ApiType<TensorProperties, QairtTensor_PropertiesV1_t> {
  friend class Api;

 public:
  TensorProperties() noexcept                              = default;
  TensorProperties(const TensorProperties&)                = delete;
  TensorProperties(TensorProperties&&) noexcept            = default;
  TensorProperties& operator=(const TensorProperties&)     = delete;
  TensorProperties& operator=(TensorProperties&&) noexcept = default;

  /// @see QairtTensor_Properties_setIsInput
  void setIsInput(bool value) {
    callSetter<&QairtTensor_PropertiesV1_t::setIsInput>(detail::convertBool(value));
  }
  /// @see QairtTensor_Properties_setIsOutput
  void setIsOutput(bool value) {
    callSetter<&QairtTensor_PropertiesV1_t::setIsOutput>(detail::convertBool(value));
  }
  /// @see QairtTensor_Properties_setIsNative
  void setIsNative(bool value) {
    callSetter<&QairtTensor_PropertiesV1_t::setIsNative>(detail::convertBool(value));
  }
  /// @see QairtTensor_Properties_setIsNull
  void setIsNull(bool value) {
    callSetter<&QairtTensor_PropertiesV1_t::setIsNull>(detail::convertBool(value));
  }
  /// @see QairtTensor_Properties_setIsStatic
  void setIsStatic(bool value) {
    callSetter<&QairtTensor_PropertiesV1_t::setIsStatic>(detail::convertBool(value));
  }
  /// @see QairtTensor_Properties_setIsOptional
  void setIsOptional(bool value) {
    callSetter<&QairtTensor_PropertiesV1_t::setIsOptional>(detail::convertBool(value));
  }
  /// @see QairtTensor_Properties_setIsUpdatable
  void setIsUpdatable(bool value) {
    callSetter<&QairtTensor_PropertiesV1_t::setIsUpdatable>(detail::convertBool(value));
  }

  /// @see QairtTensor_Properties_getIsInput
  bool isInput() const {
    return detail::resolveBool(callGetter<&QairtTensor_PropertiesV1_t::getIsInput>());
  }
  /// @see QairtTensor_Properties_getIsOutput
  bool isOutput() const {
    return detail::resolveBool(callGetter<&QairtTensor_PropertiesV1_t::getIsOutput>());
  }
  /// @see QairtTensor_Properties_getIsNative
  bool isNative() const {
    return detail::resolveBool(callGetter<&QairtTensor_PropertiesV1_t::getIsNative>());
  }
  /// @see QairtTensor_Properties_getIsNull
  bool isNull() const {
    return detail::resolveBool(callGetter<&QairtTensor_PropertiesV1_t::getIsNull>());
  }
  /// @see QairtTensor_Properties_getIsStatic
  bool isStatic() const {
    return detail::resolveBool(callGetter<&QairtTensor_PropertiesV1_t::getIsStatic>());
  }
  /// @see QairtTensor_Properties_getIsOptional
  bool isOptional() const {
    return detail::resolveBool(callGetter<&QairtTensor_PropertiesV1_t::getIsOptional>());
  }
  /// @see QairtTensor_Properties_getIsUpdatable
  bool isUpdatable() const {
    return detail::resolveBool(callGetter<&QairtTensor_PropertiesV1_t::getIsUpdatable>());
  }

 private:
  explicit TensorProperties(const std::shared_ptr<ApiTable>& apiTable) : ApiType(apiTable) {
    construct();
  }
};

/**
 * @brief Memory descriptor specifying how tensor data is stored or retrieved at runtime.
 *
 * Construct directly: TensorMemory(). Attach to a @ref Tensor via
 * @ref Tensor::setTensorMemory(). The memory type (Raw, MemHandle, or RetrieveRaw)
 * determines which accessor fields are active.
 */
class TensorMemory : public ApiType<TensorMemory, QairtTensor_MemoryV1_t> {
  friend class Api;

  /// @brief Client buffer associated with this tensor memory when the memory type is Raw.
  detail::crossable<detail::non_owning<ClientBuffer>,
                    &interface_type::getClientBuffer,
                    &interface_type::setClientBuffer>
      m_clientBuffer;

 public:
  TensorMemory() noexcept                          = default;
  TensorMemory(const TensorMemory&)                = delete;
  TensorMemory(TensorMemory&&) noexcept            = default;
  TensorMemory& operator=(const TensorMemory&)     = delete;
  TensorMemory& operator=(TensorMemory&&) noexcept = default;

  /// @see QairtTensor_Memory_getMemoryType
  TensorMemType getMemoryType() const {
    return static_cast<TensorMemType>(callGetter<&QairtTensor_MemoryV1_t::getMemoryType>());
  }

  /**
   * @brief Get the client buffer associated with this tensor memory.
   *
   * Valid only when the memory type is Raw. The returned reference is bound
   * to this TensorMemory and is invalidated if modification operations are
   * performed on the same component API.
   *
   * @return Reference to the associated @ref ClientBuffer.
   *
   * @throws qairt::Exception on invalid handle or memory type mismatch.
   *
   * @see QairtTensor_Memory_getClientBuffer
   */
  ClientBuffer& getClientBuffer() { return m_clientBuffer.bindNonOwning(*this); }
  /**
   * @brief Wrapper which allows for const access to the associated client buffer.
   *
   * @see TensorMemory::getClientBuffer()
   */
  const ClientBuffer& getClientBuffer() const { return m_clientBuffer.bindNonOwning(*this); }
  /**
   * @brief Set a client buffer on this tensor memory.
   *
   * Sets the memory type to Raw and associates the given client buffer.
   *
   * @param[in] clientBuffer  The client buffer to associate with this tensor memory.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtTensor_Memory_setClientBuffer
   */
  void setClientBuffer(const ClientBuffer& clientBuffer) {
    QAIRT_NO_RETURN m_clientBuffer.setNonOwning(*this, clientBuffer);
  }
  /**
   * @brief Set a raw memory buffer on this tensor memory.
   *
   * Constructs a @ref ClientBuffer from the given @ref RawBuffer and associates it
   * with this tensor memory. Falls back silently if the backend does not support
   * ClientBuffer creation.
   *
   * @param[in] buffer  Raw memory buffer whose data pointer and size are transferred
   *                    into the new client buffer.
   */
  void setClientBuffer(RawBuffer&& buffer) {
    // QairtTensor_ClientBuffer_create may be unsupported in some backends.
    // Try to create a ClientBuffer; if unsupported, fall back silently.
    try {
      if (auto table = detail::QairtTypeAdmin::lockApiTable(*this)) {
        ClientBuffer cb(table);
        cb.setData(buffer.getData());
        cb.setDataSize(static_cast<uint32_t>(buffer.getDataSize()));
        setClientBuffer(cb);
      }
    } catch (const Exception&) {
      // ClientBuffer::create not supported by this backend — skip.
    }
  }

  /// @see QairtTensor_Memory_setMemHandle
  void setMemHandle(QairtMem_Handle_t memHandle) {
    callSetter<&QairtTensor_MemoryV1_t::setMemHandle>(memHandle);
  }

  /// @see QairtTensor_Memory_getMemHandle
  QairtMem_Handle_t getMemHandle() const {
    return callGetter<&QairtTensor_MemoryV1_t::getMemHandle>();
  }

  /// @see QairtTensor_Memory_setRawRetrieveCallbacks
  void setRawRetrieveCallbacks(Qairt_GetTensorRawDataFn_t getCallback,
                               Qairt_FreeTensorRawDataFn_t freeCallback,
                               void* cookie) {
    callSetter<&QairtTensor_MemoryV1_t::setRawRetrieveCallbacks>(getCallback, freeCallback, cookie);
  }

  /// @see QairtTensor_Memory_getRawRetrieveCallbacks
  void getRawRetrieveCallbacks(Qairt_GetTensorRawDataFn_t* getCallback,
                               Qairt_FreeTensorRawDataFn_t* freeCallback,
                               void** cookie) const {
    callAndConfirm<&QairtTensor_MemoryV1_t::getRawRetrieveCallbacks>(
        getCallback, freeCallback, cookie);
  }

 private:
  explicit TensorMemory(const std::shared_ptr<ApiTable>& apiTable) : ApiType(apiTable) {
    construct();
  }

  void prepareToCross() const {
    if (getMemoryType() == TensorMemType::Raw) {
      m_clientBuffer.prepareToCross(*this);
    }
  }
  void updateAfterCross() const {
    if (getMemoryType() == TensorMemType::Raw) {
      m_clientBuffer.updateAfterCross(*this);
    }
  }
};

/**
 * @brief Per-tensor scale and integer offset for scale-offset quantization.
 *
 * Construct directly: ScaleOffset(float scale, int32_t offset). Attach to a
 * @ref QuantizeParams via @ref QuantizeParams::setScaleOffsetEncoding().
 */
class ScaleOffset : public ApiType<ScaleOffset, QairtQuantizeParams_ScaleOffsetV1_t> {
  friend class Api;

 public:
  using ApiType::ApiType;
  using ApiType::operator=;

  ~ScaleOffset(){QAIRT_TRIVIAL} ScaleOffset() noexcept = default;
  ScaleOffset(ScaleOffset&& other) noexcept : ApiType(static_cast<ApiType&&>(other)) {}
  ScaleOffset& operator=(ScaleOffset&& other) noexcept {
    if (this != &other) {
      std::swap(static_cast<ApiType&>(*this), static_cast<ApiType&>(other));
    }
    return *this;
  }

  /**
   * @brief Construct a scale-offset pair with the given scale and offset.
   *
   * @param[in] scale   Scale factor.
   * @param[in] offset  Integer zero-point offset.
   */
  ScaleOffset(float scale, int32_t offset) {
    construct();
    setScale(scale);
    setOffset(offset);
  }

  ScaleOffset(const ScaleOffset& other) : ScaleOffset(other.shallowCopy()) {}

  ScaleOffset& operator=(const ScaleOffset& other) {
    if (this != &other) {
      *this = other.shallowCopy();
    }
    return *this;
  }

  ScaleOffset shallowCopy() const {
    ScaleOffset toret(copy_table_tag, *this);
    toret.construct();
    toret.setScale(getScale());
    toret.setOffset(getOffset());
    return toret;
  }

  /// @see QairtQuantizeParams_ScaleOffset_getScale
  float getScale() const { return callGetter<&interface_type::getScale>(); }
  /// @see QairtQuantizeParams_ScaleOffset_setScale
  void setScale(float scale) const { callSetter<&interface_type::setScale>(scale); }

  /// @see QairtQuantizeParams_ScaleOffset_getOffset
  int32_t getOffset() const { return callGetter<&interface_type::getOffset>(); }
  /// @see QairtQuantizeParams_ScaleOffset_setOffset
  void setOffset(int32_t offset) const { callSetter<&interface_type::setOffset>(offset); }

 private:
  void prepareToCross() const {}
  void updateAfterCross() const {}
  explicit ScaleOffset(const std::shared_ptr<ApiTable>& apiTable) : ApiType(apiTable) {
    construct();
  }
};

/**
 * @brief Bit-width, scale, and integer offset for bit-width scale-offset quantization.
 *
 * Construct directly: BwScaleOffset(uint32_t bitwidth, float scale, int32_t offset). Attach to
 * a @ref QuantizeParams via @ref QuantizeParams::setBwScaleOffsetEncoding().
 */
class BwScaleOffset : public ApiType<BwScaleOffset, QairtQuantizeParams_BwScaleOffsetV1_t> {
  friend class Api;

 public:
  using ApiType::ApiType;
  using ApiType::operator=;

  ~BwScaleOffset(){QAIRT_TRIVIAL} BwScaleOffset() noexcept = default;
  BwScaleOffset(BwScaleOffset&& other) noexcept : ApiType(static_cast<ApiType&&>(other)) {}
  BwScaleOffset& operator=(BwScaleOffset&& other) noexcept {
    if (this != &other) {
      std::swap(static_cast<ApiType&>(*this), static_cast<ApiType&>(other));
    }
    return *this;
  }

  BwScaleOffset(const BwScaleOffset& other) : BwScaleOffset(copy_table_tag, other) {
    construct();
    setBitwidth(other.getBitwidth());
    setScale(other.getScale());
    setOffset(other.getOffset());
  }

  BwScaleOffset& operator=(const BwScaleOffset& other) {
    if (this != &other) {
      *this = other.shallowCopy();
    }
    return *this;
  }

  BwScaleOffset shallowCopy() const { return BwScaleOffset(*this); }

  /**
   * @brief Construct a bit-width scale-offset encoding with the given parameters.
   *
   * @param[in] bitwidth  Storage bit-width of the quantized values.
   * @param[in] scale     Scale factor.
   * @param[in] offset    Integer zero-point offset.
   */
  BwScaleOffset(uint32_t bitwidth, float scale, int32_t offset) {
    construct();
    setBitwidth(bitwidth);
    setScale(scale);
    setOffset(offset);
  }

  /// @see QairtQuantizeParams_BwScaleOffset_getBw
  uint32_t getBitwidth() const { return callGetter<&interface_type::getBw>(); }
  /// @see QairtQuantizeParams_BwScaleOffset_setBw
  void setBitwidth(uint32_t bitwidth) const { callSetter<&interface_type::setBw>(bitwidth); }

  /// @see QairtQuantizeParams_BwScaleOffset_getScale
  float getScale() const { return callGetter<&interface_type::getScale>(); }
  /// @see QairtQuantizeParams_BwScaleOffset_setScale
  void setScale(float scale) const { callSetter<&interface_type::setScale>(scale); }

  /// @see QairtQuantizeParams_BwScaleOffset_getOffset
  int32_t getOffset() const { return callGetter<&interface_type::getOffset>(); }
  /// @see QairtQuantizeParams_BwScaleOffset_setOffset
  void setOffset(int32_t offset) const { callSetter<&interface_type::setOffset>(offset); }

 private:
  void prepareToCross() const {}
  void updateAfterCross() const {}
  explicit BwScaleOffset(const std::shared_ptr<ApiTable>& apiTable) : ApiType(apiTable) {
    construct();
  }
};

/**
 * @brief Per-axis (e.g., per-channel) collection of scale-offset pairs for axis scale-offset
 *        quantization.
 *
 * Construct directly: AxisScaleOffset(). Set the quantization axis via setAxis() and supply
 * one @ref ScaleOffset per axis element via setScaleOffsets(). Attach to a @ref QuantizeParams
 * via @ref QuantizeParams::setAxisScaleOffsetEncoding().
 */
class AxisScaleOffset : public ApiType<AxisScaleOffset, QairtQuantizeParams_AxisScaleOffsetV1_t> {
  friend class Api;
  /// @brief Per-axis scale-offset pairs indexed along the quantization axis.
  detail::crossable<std::vector<detail::non_owning<ScaleOffset>>,
                    &interface_type::getScaleOffsetAt,
                    &interface_type::getNumScaleOffsets,
                    &interface_type::setScaleOffsets>
      m_scaleOffsets;

 public:
  using ApiType::ApiType;
  using ApiType::operator=;

  ~AxisScaleOffset(){QAIRT_TRIVIAL} AxisScaleOffset() noexcept = default;
  AxisScaleOffset(AxisScaleOffset&& other) noexcept
      : ApiType(static_cast<ApiType&&>(other)), m_scaleOffsets(std::move(other.m_scaleOffsets)) {}
  AxisScaleOffset& operator=(AxisScaleOffset&& other) noexcept {
    if (this != &other) {
      std::swap(static_cast<ApiType&>(*this), static_cast<ApiType&>(other));
      std::swap(m_scaleOffsets, other.m_scaleOffsets);
    }
    return *this;
  }

  AxisScaleOffset(const AxisScaleOffset& other) : AxisScaleOffset(copy_table_tag, other) {
    construct();
    setAxis(other.getAxis());
    setScaleOffsets(other.getScaleOffsets());
  }

  AxisScaleOffset& operator=(const AxisScaleOffset& other) {
    if (this != &other) {
      *this = other.shallowCopy();
    }
    return *this;
  }

  AxisScaleOffset shallowCopy() const { return AxisScaleOffset(*this); }

  /// @see QairtQuantizeParams_AxisScaleOffset_getAxis
  int32_t getAxis() const { return callGetter<&interface_type::getAxis>(); }
  /// @see QairtQuantizeParams_AxisScaleOffset_setAxis
  void setAxis(int32_t axis) const { return callSetter<&interface_type::setAxis>(axis); }

  /**
   * @brief Get the per-axis scale-offset pairs for this encoding.
   *
   * @return Reference to the vector of @ref ScaleOffset objects, one per axis element.
   *
   * @throws qairt::Exception on invalid handle.
   */
  std::vector<ScaleOffset>& getScaleOffsets() { return m_scaleOffsets.get(*this); }
  /**
   * @brief Wrapper which allows for const access to the per-axis scale-offset pairs.
   *
   * @see AxisScaleOffset::getScaleOffsets()
   */
  const std::vector<ScaleOffset>& getScaleOffsets() const { return m_scaleOffsets.get(*this); }
  /**
   * @brief Set the per-axis scale-offset pairs for this encoding.
   *
   * @param[in] scaleOffsets  Scale-offset pairs, one per element along the quantization axis.
   *
   * @throws qairt::Exception on invalid handle.
   */
  void setScaleOffsets(const std::vector<ScaleOffset>& scaleOffsets) {
    m_scaleOffsets.set(*this, scaleOffsets);
  }

 private:
  void prepareToCross() const {}
  void updateAfterCross() const {}
  explicit AxisScaleOffset(const std::shared_ptr<ApiTable>& apiTable) : ApiType(apiTable) {
    construct();
  }
};

/**
 * @brief Bit-width, per-axis scales and offsets, and a quantization mapping for
 *        bit-width per-axis scale-offset mapped quantization.
 *
 * Construct directly: BwAxisScaleOffsetMapped(). Supply scales via setScales(), offsets
 * via setOffsets(), and a mapping scheme via setMapping(). Attach to a @ref QuantizeParams
 * via @ref QuantizeParams::setBwAxisScaleOffsetMappedEncoding().
 */
class BwAxisScaleOffsetMapped
    : public ApiType<BwAxisScaleOffsetMapped, QairtQuantizeParams_BwAxisScaleOffsetMappedV1_t> {
  friend Api;
  // detail::crossable<std::vector<float>,
  //                   &interface_type::getScaleAt,
  //                   &interface_type::getNumScales,
  //                   &interface_type::setScales>
  //     m_scales;
  // detail::crossable<std::vector<int32_t>,
  //                   &interface_type::getOffsetAt,
  //                   &interface_type::getNumOffsets,
  //                   &interface_type::setOffsets>
  //     m_offsets;
  /// @brief Per-axis scale values for the mapped encoding, one per quantization axis element.
  mutable std::vector<float> m_scales;
  /// @brief Per-axis offset values for the mapped encoding, one per quantization axis element.
  mutable std::vector<int32_t> m_offsets;

 public:
  using ApiType::ApiType;
  using ApiType::operator=;

  ~BwAxisScaleOffsetMapped(){QAIRT_TRIVIAL} BwAxisScaleOffsetMapped() noexcept = default;
  BwAxisScaleOffsetMapped(BwAxisScaleOffsetMapped&& other) noexcept
      : ApiType(static_cast<ApiType&&>(other)),
        m_scales(std::move(other.m_scales)),
        m_offsets(std::move(other.m_offsets)) {}
  BwAxisScaleOffsetMapped& operator=(BwAxisScaleOffsetMapped&& other) noexcept {
    if (this != &other) {
      std::swap(static_cast<ApiType&>(*this), static_cast<ApiType&>(other));
      std::swap(m_scales, other.m_scales);
      std::swap(m_offsets, other.m_offsets);
    }
    return *this;
  }

  BwAxisScaleOffsetMapped(const BwAxisScaleOffsetMapped& other)
      : BwAxisScaleOffsetMapped(copy_table_tag, other) {
    construct();
    setBitwidth(other.getBitwidth());
    setAxis(other.getAxis());
    setMapping(other.getMapping());
    setScales(other.getScales());
    setOffsets(other.getOffsets());
  }

  BwAxisScaleOffsetMapped& operator=(const BwAxisScaleOffsetMapped& other) {
    if (this != &other) {
      *this = other.shallowCopy();
    }
    return *this;
  }

  BwAxisScaleOffsetMapped shallowCopy() const { return BwAxisScaleOffsetMapped(*this); }

  /// @see QairtQuantizeParams_BwAxisScaleOffsetMapped_getBw
  uint32_t getBitwidth() const { return callGetter<&interface_type::getBw>(); }
  /// @see QairtQuantizeParams_BwAxisScaleOffsetMapped_setBw
  void setBitwidth(uint32_t bitwidth) const { callSetter<&interface_type::setBw>(bitwidth); }

  /// @see QairtQuantizeParams_BwAxisScaleOffsetMapped_getAxis
  int32_t getAxis() const { return callGetter<&interface_type::getAxis>(); }
  /// @see QairtQuantizeParams_BwAxisScaleOffsetMapped_setAxis
  void setAxis(int32_t axis) const { callSetter<&interface_type::setAxis>(axis); }

  /// @see QairtQuantizeParams_BwAxisScaleOffsetMapped_getMapping
  QuantizationEncodingMapping getMapping() const {
    return static_cast<QuantizationEncodingMapping>(callGetter<&interface_type::getMapping>());
  }
  /// @see QairtQuantizeParams_BwAxisScaleOffsetMapped_setMapping
  void setMapping(QuantizationEncodingMapping mapping) const {
    callSetter<&interface_type::setMapping>(
        static_cast<Qairt_QuantizationEncodingMapping_t>(mapping));
  }

  /**
   * @brief Get the per-axis scale values for this encoding.
   *
   * @return Reference to the vector of scale values, one per element along the quantization axis.
   */
  const std::vector<float>& getScales() const { return m_scales; }
  /**
   * @brief Set the per-axis scale values for this encoding.
   *
   * @param[in] scales  Scale values, one per element along the quantization axis.
   */
  void setScales(const std::vector<float>& scales) { m_scales = scales; }
  /// @see BwAxisScaleOffsetMapped::setScales(const std::vector<float>&)
  void setScales(std::vector<float>&& scales) { m_scales = std::move(scales); }

  /**
   * @brief Get the per-axis offset values for this encoding.
   *
   * @return Reference to the vector of offset values, one per element along the quantization axis.
   */
  const std::vector<int32_t>& getOffsets() const { return m_offsets; }
  /**
   * @brief Set the per-axis offset values for this encoding.
   *
   * @param[in] offsets  Integer zero-point offsets, one per element along the quantization axis.
   */
  void setOffsets(const std::vector<int32_t>& offsets) { m_offsets = offsets; }
  /// @see BwAxisScaleOffsetMapped::setOffsets(const std::vector<int32_t>&)
  void setOffsets(std::vector<int32_t>&& offsets) { m_offsets = std::move(offsets); }

 private:
  void prepareToCross() const {
    if (!m_scales.empty()) {
      callSetter<&interface_type::setScales>(m_scales.data(),
                                             static_cast<uint32_t>(m_scales.size()));
    }
    if (!m_offsets.empty()) {
      callSetter<&interface_type::setOffsets>(m_offsets.data(),
                                              static_cast<uint32_t>(m_offsets.size()));
    }
  }

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

  explicit BwAxisScaleOffsetMapped(const std::shared_ptr<ApiTable>& apiTable) : ApiType(apiTable) {
    construct();
  }
};

/**
 * @brief Floating-point scale and offset pair for float block quantization encodings.
 *
 * Construct directly: FloatScaleOffset(float scale, float offset). Used as elements
 * within @ref BwFloatBlockEncoding and @ref FloatBlockEncoding.
 */
class FloatScaleOffset
    : public ApiType<FloatScaleOffset, QairtQuantizeParams_FloatScaleOffsetV1_t> {
  friend Api;

 public:
  using ApiType::ApiType;
  using ApiType::operator=;

  ~FloatScaleOffset(){QAIRT_TRIVIAL} FloatScaleOffset() noexcept = default;
  FloatScaleOffset(FloatScaleOffset&& other) noexcept : ApiType(static_cast<ApiType&&>(other)) {}
  FloatScaleOffset& operator=(FloatScaleOffset&& other) noexcept {
    if (this != &other) {
      std::swap(static_cast<ApiType&>(*this), static_cast<ApiType&>(other));
    }
    return *this;
  }

  FloatScaleOffset(const FloatScaleOffset& other) : FloatScaleOffset(copy_table_tag, other) {
    construct();
    setScale(other.getScale());
    setOffset(other.getOffset());
  }

  FloatScaleOffset& operator=(const FloatScaleOffset& other) {
    if (this != &other) {
      *this = other.shallowCopy();
    }
    return *this;
  }

  FloatScaleOffset shallowCopy() const { return FloatScaleOffset(*this); }

  /**
   * @brief Construct a floating-point scale-offset pair with the given scale and offset.
   *
   * @param[in] scale   Scale factor.
   * @param[in] offset  Floating-point zero-point offset.
   */
  FloatScaleOffset(float scale, float offset) {
    construct();
    setScale(scale);
    setOffset(offset);
  }

  /// @see QairtQuantizeParams_FloatScaleOffset_getScale
  float getScale() { return callGetter<&interface_type::getScale>(); }
  /// @see QairtQuantizeParams_FloatScaleOffset_getScale
  float getScale() const { return callGetter<&interface_type::getScale>(); }
  /// @see QairtQuantizeParams_FloatScaleOffset_setScale
  void setScale(float scale) const { callSetter<&interface_type::setScale>(scale); }

  /// @see QairtQuantizeParams_FloatScaleOffset_getOffset
  float getOffset() { return callGetter<&interface_type::getOffset>(); }
  /// @see QairtQuantizeParams_FloatScaleOffset_getOffset
  float getOffset() const { return callGetter<&interface_type::getOffset>(); }
  /// @see QairtQuantizeParams_FloatScaleOffset_setOffset
  void setOffset(float offset) const { callSetter<&interface_type::setOffset>(offset); }

 private:
  void prepareToCross() const {}
  void updateAfterCross() const {}
  explicit FloatScaleOffset(const std::shared_ptr<ApiTable>& apiTable) : ApiType(apiTable) {
    construct();
  }
};

/**
 * @brief Bit-width and per-block floating-point scale-offset pairs for bit-width float
 *        block quantization.
 *
 * Construct directly: BwFloatBlockEncoding(). Supply block sizes via setBlockSizes() and
 * one @ref FloatScaleOffset per block via setFloatScaleOffsets(). Attach to a
 * @ref QuantizeParams via @ref QuantizeParams::setBwFloatBlockEncoding().
 */
class BwFloatBlockEncoding
    : public ApiType<BwFloatBlockEncoding, QairtQuantizeParams_BwFloatBlockEncodingV1_t> {
  friend Api;
  /// @brief Block size along each quantization dimension for the float block encoding.
  mutable std::vector<uint32_t> m_blockSizes;
  /// @brief Float scale-offset pairs, one per quantization block.
  detail::crossable<std::vector<detail::non_owning<FloatScaleOffset>>,
                    &interface_type::getFloatScaleOffsetAt,
                    &interface_type::getNumFloatScaleOffsets,
                    &interface_type::setFloatScaleOffsets>
      m_floatScaleOffsets;

 public:
  using ApiType::ApiType;
  using ApiType::operator=;

  ~BwFloatBlockEncoding(){QAIRT_TRIVIAL} BwFloatBlockEncoding() noexcept = default;
  BwFloatBlockEncoding(BwFloatBlockEncoding&& other) noexcept
      : ApiType(static_cast<ApiType&&>(other)),
        m_blockSizes(std::move(other.m_blockSizes)),
        m_floatScaleOffsets(std::move(other.m_floatScaleOffsets)) {}
  BwFloatBlockEncoding& operator=(BwFloatBlockEncoding&& other) noexcept {
    if (this != &other) {
      std::swap(static_cast<ApiType&>(*this), static_cast<ApiType&>(other));
      std::swap(m_blockSizes, other.m_blockSizes);
      std::swap(m_floatScaleOffsets, other.m_floatScaleOffsets);
    }
    return *this;
  }

  BwFloatBlockEncoding(const BwFloatBlockEncoding& other)
      : BwFloatBlockEncoding(copy_table_tag, other) {
    construct();
    setBitwidth(other.getBitwidth());
    setBlockSizes(other.getBlockSizes());
    setFloatScaleOffsets(other.getFloatScaleOffsets());
  }

  BwFloatBlockEncoding& operator=(const BwFloatBlockEncoding& other) {
    if (this != &other) {
      *this = other.shallowCopy();
    }
    return *this;
  }

  BwFloatBlockEncoding shallowCopy() const { return BwFloatBlockEncoding(*this); }

  /// @see QairtQuantizeParams_BwFloatBlockEncoding_getBw
  uint32_t getBitwidth() const { return callGetter<&interface_type::getBw>(); }
  /// @see QairtQuantizeParams_BwFloatBlockEncoding_setBw
  void setBitwidth(uint32_t bitwidth) const { callSetter<&interface_type::setBw>(bitwidth); }

  /**
   * @brief Get the block sizes along each quantization dimension.
   *
   * @return Reference to the vector of block sizes.
   */
  const std::vector<uint32_t>& getBlockSizes() const { return m_blockSizes; }
  /**
   * @brief Set the block sizes along each quantization dimension.
   *
   * @param[in] blockSizes  Block sizes, one per quantization dimension.
   */
  void setBlockSizes(const std::vector<uint32_t>& blockSizes) { m_blockSizes = blockSizes; }
  /// @see BwFloatBlockEncoding::setBlockSizes(const std::vector<uint32_t>&)
  void setBlockSizes(std::vector<uint32_t>&& blockSizes) { m_blockSizes = std::move(blockSizes); }

  /**
   * @brief Get the per-block floating-point scale-offset pairs.
   *
   * @return Reference to the vector of @ref FloatScaleOffset objects, one per quantization block.
   *
   * @throws qairt::Exception on invalid handle.
   */
  std::vector<FloatScaleOffset>& getFloatScaleOffsets() { return m_floatScaleOffsets.get(*this); }
  /**
   * @brief Wrapper which allows for const access to the per-block float scale-offset pairs.
   *
   * @see BwFloatBlockEncoding::getFloatScaleOffsets()
   */
  const std::vector<FloatScaleOffset>& getFloatScaleOffsets() const {
    return m_floatScaleOffsets.get(*this);
  }
  /**
   * @brief Set the per-block floating-point scale-offset pairs.
   *
   * @param[in] floatScaleOffsets  Float scale-offset pairs, one per quantization block.
   *
   * @throws qairt::Exception on invalid handle.
   */
  void setFloatScaleOffsets(std::vector<FloatScaleOffset> floatScaleOffsets) {
    m_floatScaleOffsets.set(*this, floatScaleOffsets);
  }

 private:
  void prepareToCross() const {
    if (!m_blockSizes.empty()) {
      callSetter<&interface_type::setBlockSize>(const_cast<uint32_t*>(m_blockSizes.data()),
                                                static_cast<uint32_t>(m_blockSizes.size()));
    }
  }

  void updateAfterCross() const {
    uint32_t numBlockSizes{0u};
    callAndConfirm<&interface_type::getNumBlockSizes>(&numBlockSizes);
    m_blockSizes.resize(numBlockSizes);
    for (uint32_t i = 0; i < numBlockSizes; ++i) {
      callAndConfirm<&interface_type::getBlockSizeAt>(i, &m_blockSizes[i]);
    }
  }

  explicit BwFloatBlockEncoding(const std::shared_ptr<ApiTable>& apiTable) : ApiType(apiTable) {
    construct();
  }
};

/**
 * @brief Per-block floating-point scale-offset pairs for float block quantization.
 *
 * Construct directly: FloatBlockEncoding(). Supply block sizes via setBlockSizes() and
 * one @ref FloatScaleOffset per block via setFloatScaleOffsets(). Attach to a
 * @ref QuantizeParams via @ref QuantizeParams::setFloatBlockEncoding().
 */
class FloatBlockEncoding
    : public ApiType<FloatBlockEncoding, QairtQuantizeParams_FloatBlockEncodingV1_t> {
  friend Api;
  /// @brief Block size along each quantization dimension for the float block encoding.
  mutable std::vector<uint32_t> m_blockSizes;
  /// @brief Float scale-offset pairs, one per quantization block.
  detail::crossable<std::vector<detail::non_owning<FloatScaleOffset>>,
                    &interface_type::getFloatScaleOffsetAt,
                    &interface_type::getNumFloatScaleOffsets,
                    &interface_type::setFloatScaleOffsets>
      m_floatScaleOffsets;

 public:
  using ApiType::ApiType;
  using ApiType::operator=;

  ~FloatBlockEncoding(){QAIRT_TRIVIAL} FloatBlockEncoding() noexcept = default;
  FloatBlockEncoding(FloatBlockEncoding&& other) noexcept
      : ApiType(static_cast<ApiType&&>(other)),
        m_blockSizes(std::move(other.m_blockSizes)),
        m_floatScaleOffsets(std::move(other.m_floatScaleOffsets)) {}
  FloatBlockEncoding& operator=(FloatBlockEncoding&& other) noexcept {
    if (this != &other) {
      std::swap(static_cast<ApiType&>(*this), static_cast<ApiType&>(other));
      std::swap(m_blockSizes, other.m_blockSizes);
      std::swap(m_floatScaleOffsets, other.m_floatScaleOffsets);
    }
    return *this;
  }

  FloatBlockEncoding(const FloatBlockEncoding& other) : FloatBlockEncoding(copy_table_tag, other) {
    construct();
    setBlockSizes(other.getBlockSizes());
    setFloatScaleOffsets(other.getFloatScaleOffsets());
  }

  FloatBlockEncoding& operator=(const FloatBlockEncoding& other) {
    if (this != &other) {
      *this = other.shallowCopy();
    }
    return *this;
  }

  FloatBlockEncoding shallowCopy() const { return FloatBlockEncoding(*this); }

  /**
   * @brief Get the block sizes along each quantization dimension.
   *
   * @return Reference to the vector of block sizes.
   */
  const std::vector<uint32_t>& getBlockSizes() const { return m_blockSizes; }
  /**
   * @brief Set the block sizes along each quantization dimension.
   *
   * @param[in] blockSizes  Block sizes, one per quantization dimension.
   */
  void setBlockSizes(const std::vector<uint32_t>& blockSizes) { m_blockSizes = blockSizes; }
  /// @see FloatBlockEncoding::setBlockSizes(const std::vector<uint32_t>&)
  void setBlockSizes(std::vector<uint32_t>&& blockSizes) { m_blockSizes = std::move(blockSizes); }

  /**
   * @brief Get the per-block floating-point scale-offset pairs.
   *
   * @return Reference to the vector of @ref FloatScaleOffset objects, one per quantization block.
   *
   * @throws qairt::Exception on invalid handle.
   */
  std::vector<FloatScaleOffset>& getFloatScaleOffsets() { return m_floatScaleOffsets.get(*this); }
  /**
   * @brief Wrapper which allows for const access to the per-block float scale-offset pairs.
   *
   * @see FloatBlockEncoding::getFloatScaleOffsets()
   */
  const std::vector<FloatScaleOffset>& getFloatScaleOffsets() const {
    return m_floatScaleOffsets.get(*this);
  }
  /**
   * @brief Set the per-block floating-point scale-offset pairs.
   *
   * @param[in] floatScaleOffsets  Float scale-offset pairs, one per quantization block.
   *
   * @throws qairt::Exception on invalid handle.
   */
  void setFloatScaleOffsets(std::vector<FloatScaleOffset> floatScaleOffsets) {
    m_floatScaleOffsets.set(*this, floatScaleOffsets);
  }

 private:
  void prepareToCross() const {
    if (!m_blockSizes.empty()) {
      callSetter<&interface_type::setBlockSize>(const_cast<uint32_t*>(m_blockSizes.data()),
                                                static_cast<uint32_t>(m_blockSizes.size()));
    }
  }

  void updateAfterCross() const {
    uint32_t numBlockSizes{0u};
    callAndConfirm<&interface_type::getNumBlockSizes>(&numBlockSizes);
    m_blockSizes.resize(numBlockSizes);
    for (uint32_t i = 0; i < numBlockSizes; ++i) {
      callAndConfirm<&interface_type::getBlockSizeAt>(i, &m_blockSizes[i]);
    }
  }

  explicit FloatBlockEncoding(const std::shared_ptr<ApiTable>& apiTable) : ApiType(apiTable) {
    construct();
  }
};

/**
 * @brief Bit-width, block sizes, a quantization mapping, and per-block scale-offset pairs
 *        for bit-width per-block mapped quantization.
 *
 * Construct directly: BwBlockMapped(). Supply block sizes via setBlockSizes(), a mapping
 * scheme via setMapping(), and per-block @ref ScaleOffset values via setScaleOffsets().
 * Attach to a @ref QuantizeParams via @ref QuantizeParams::setBwBlockMapped().
 */
class BwBlockMapped : public ApiType<BwBlockMapped, QairtQuantizeParams_BwBlockMappedV1_t> {
  friend Api;
  /// @brief Block sizes along each quantization dimension.
  mutable std::vector<uint32_t> m_blockSizes;
  /// @brief Scale-offset pairs for each quantization block.
  detail::crossable<std::vector<detail::non_owning<ScaleOffset>>,
                    &interface_type::getScaleOffsetAt,
                    &interface_type::getNumScaleOffsets,
                    &interface_type::setScaleOffsets>
      m_scaleOffsets;

 public:
  using ApiType::ApiType;
  using ApiType::operator=;

  ~BwBlockMapped(){QAIRT_TRIVIAL} BwBlockMapped() noexcept = default;
  BwBlockMapped(BwBlockMapped&& other) noexcept
      : ApiType(static_cast<ApiType&&>(other)),
        m_blockSizes(std::move(other.m_blockSizes)),
        m_scaleOffsets(std::move(other.m_scaleOffsets)) {}
  BwBlockMapped& operator=(BwBlockMapped&& other) noexcept {
    if (this != &other) {
      std::swap(static_cast<ApiType&>(*this), static_cast<ApiType&>(other));
      std::swap(m_blockSizes, other.m_blockSizes);
      std::swap(m_scaleOffsets, other.m_scaleOffsets);
    }
    return *this;
  }

  BwBlockMapped(const BwBlockMapped& other) : BwBlockMapped(copy_table_tag, other) {
    construct();
    setBitwidth(other.getBitwidth());
    setMapping(other.getMapping());
    setBlockSizes(other.getBlockSizes());
    setScaleOffsets(other.getScaleOffsets());
  }

  BwBlockMapped& operator=(const BwBlockMapped& other) {
    if (this != &other) {
      *this = other.shallowCopy();
    }
    return *this;
  }

  BwBlockMapped shallowCopy() const { return BwBlockMapped(*this); }

  /// @see QairtQuantizeParams_BwBlockMapped_getBw
  uint32_t getBitwidth() const { return callGetter<&interface_type::getBw>(); }
  /// @see QairtQuantizeParams_BwBlockMapped_setBw
  void setBitwidth(uint32_t bitwidth) const { callSetter<&interface_type::setBw>(bitwidth); }

  /// @see QairtQuantizeParams_BwBlockMapped_getMapping
  QuantizationEncodingMapping getMapping() const {
    return static_cast<QuantizationEncodingMapping>(callGetter<&interface_type::getMapping>());
  }
  /// @see QairtQuantizeParams_BwBlockMapped_setMapping
  void setMapping(QuantizationEncodingMapping mapping) const {
    callSetter<&interface_type::setMapping>(
        static_cast<Qairt_QuantizationEncodingMapping_t>(mapping));
  }

  /**
   * @brief Get the block sizes along each quantization dimension.
   *
   * @return Reference to the vector of block sizes.
   */
  const std::vector<uint32_t>& getBlockSizes() const { return m_blockSizes; }
  /**
   * @brief Set the block sizes along each quantization dimension.
   *
   * @param[in] blockSizes  Block sizes, one per quantization dimension.
   */
  void setBlockSizes(const std::vector<uint32_t>& blockSizes) { m_blockSizes = blockSizes; }
  /// @see BwBlockMapped::setBlockSizes(const std::vector<uint32_t>&)
  void setBlockSizes(std::vector<uint32_t>&& blockSizes) { m_blockSizes = std::move(blockSizes); }
  /// @see BwBlockMapped::setBlockSizes(const std::vector<uint32_t>&)
  void setBlockSize(std::vector<uint32_t> blockSizes) { m_blockSizes = std::move(blockSizes); }

  /**
   * @brief Get the per-block scale-offset pairs for this encoding.
   *
   * @return Reference to the vector of @ref ScaleOffset objects, one per quantization block.
   *
   * @throws qairt::Exception on invalid handle.
   */
  std::vector<ScaleOffset>& getScaleOffsets() { return m_scaleOffsets.get(*this); }
  /**
   * @brief Wrapper which allows for const access to the per-block scale-offset pairs.
   *
   * @see BwBlockMapped::getScaleOffsets()
   */
  const std::vector<ScaleOffset>& getScaleOffsets() const { return m_scaleOffsets.get(*this); }
  /**
   * @brief Set the per-block scale-offset pairs for this encoding.
   *
   * @param[in] scaleOffsets  Scale-offset pairs, one per quantization block.
   *
   * @throws qairt::Exception on invalid handle.
   */
  void setScaleOffsets(std::vector<ScaleOffset> scaleOffsets) {
    m_scaleOffsets.set(*this, scaleOffsets);
  }

 private:
  void prepareToCross() const {
    if (!m_blockSizes.empty()) {
      callSetter<&interface_type::setBlockSize>(const_cast<uint32_t*>(m_blockSizes.data()),
                                                static_cast<uint32_t>(m_blockSizes.size()));
    }
  }

  void updateAfterCross() const {
    uint32_t numBlockSizes{0u};
    callAndConfirm<&interface_type::getNumBlockSizes>(&numBlockSizes);
    m_blockSizes.resize(numBlockSizes);
    for (uint32_t i = 0; i < numBlockSizes; ++i) {
      callAndConfirm<&interface_type::getBlockSizeAt>(i, &m_blockSizes[i]);
    }
  }

  explicit BwBlockMapped(const std::shared_ptr<ApiTable>& apiTable) : ApiType(apiTable) {
    construct();
  }
};

/**
 * @brief Bit-width, per-axis block expansion parameters, a quantization mapping, and
 *        per-block scale data for bit-width blockwise expansion mapped quantization.
 *
 * Construct directly: BwBlockwiseExpansionMapped(). Supply block scale data as either
 * 8-bit values via setBlocksScale8() or 16-bit values via setBlocksScale16() — these are
 * mutually exclusive. Attach to a @ref QuantizeParams via
 * @ref QuantizeParams::setBwBlockwiseExpansionMapped().
 *
 * @note Setting 8-bit block scales clears any 16-bit block scales and vice versa.
 */
class BwBlockwiseExpansionMapped
    : public ApiType<BwBlockwiseExpansionMapped,
                     QairtQuantizeParams_BwBlockwiseExpansionMappedV1_t> {
  friend Api;

  /// @brief Scale-offset pairs for each quantization block in the mapped encoding.
  detail::crossable<std::vector<detail::non_owning<ScaleOffset>>,
                    &interface_type::getScaleOffsetAt,
                    &interface_type::getNumScaleOffsets,
                    &interface_type::setScaleOffsets>
      m_scaleOffsets;

  /// @brief Block scale values stored as 8-bit data; mutually exclusive with m_blocksScale16.
  mutable std::vector<uint8_t> m_blocksScale8;
  /// @brief Block scale values stored as 16-bit data; mutually exclusive with m_blocksScale8.
  mutable std::vector<uint16_t> m_blocksScale16;

 public:
  using ApiType::ApiType;
  using ApiType::operator=;

  ~BwBlockwiseExpansionMapped(){QAIRT_TRIVIAL} BwBlockwiseExpansionMapped() noexcept = default;
  BwBlockwiseExpansionMapped(BwBlockwiseExpansionMapped&& other) noexcept
      : ApiType(static_cast<ApiType&&>(other)),
        m_scaleOffsets(std::move(other.m_scaleOffsets)),
        m_blocksScale8(std::move(other.m_blocksScale8)),
        m_blocksScale16(std::move(other.m_blocksScale16)) {}
  BwBlockwiseExpansionMapped& operator=(BwBlockwiseExpansionMapped&& other) noexcept {
    if (this != &other) {
      std::swap(static_cast<ApiType&>(*this), static_cast<ApiType&>(other));
      std::swap(m_scaleOffsets, other.m_scaleOffsets);
      std::swap(m_blocksScale8, other.m_blocksScale8);
      std::swap(m_blocksScale16, other.m_blocksScale16);
    }
    return *this;
  }

  BwBlockwiseExpansionMapped(const BwBlockwiseExpansionMapped& other)
      : BwBlockwiseExpansionMapped(copy_table_tag, other) {
    construct();
    setBitwidth(other.getBitwidth());
    setMapping(other.getMapping());
    setAxis(other.getAxis());
    setScaleOffsets(other.getScaleOffsets());
    setNumBlocksPerAxis(other.getNumBlocksPerAxis());
    setBlockScaleBitwidth(other.getBlockScaleBitwidth());
    BlockwiseExpansionBlockScaleStorageType storageType{};
    {
      // Determine storage type from which vector is non-empty
      if (!other.m_blocksScale8.empty()) {
        storageType = BlockwiseExpansionBlockScaleStorageType::Storage8;
      } else if (!other.m_blocksScale16.empty()) {
        storageType = BlockwiseExpansionBlockScaleStorageType::Storage16;
      }
    }
    switch (storageType) {
      case BlockwiseExpansionBlockScaleStorageType::Storage8:
        setBlocksScale8(other.getBlocksScale8());
        break;
      case BlockwiseExpansionBlockScaleStorageType::Storage16:
        setBlocksScale16(other.getBlocksScale16());
        break;
      default:
        break;
    }
  }

  BwBlockwiseExpansionMapped& operator=(const BwBlockwiseExpansionMapped& other) {
    if (this != &other) {
      *this = other.shallowCopy();
    }
    return *this;
  }

  BwBlockwiseExpansionMapped shallowCopy() const { return BwBlockwiseExpansionMapped(*this); }

  /// @see QairtQuantizeParams_BwBlockwiseExpansionMapped_getBw
  uint32_t getBitwidth() const { return callGetter<&interface_type::getBw>(); }
  /// @see QairtQuantizeParams_BwBlockwiseExpansionMapped_setBw
  void setBitwidth(uint32_t bitwidth) const { callSetter<&interface_type::setBw>(bitwidth); }

  /// @see QairtQuantizeParams_BwBlockwiseExpansionMapped_getMapping
  QuantizationEncodingMapping getMapping() const {
    return static_cast<QuantizationEncodingMapping>(callGetter<&interface_type::getMapping>());
  }
  /// @see QairtQuantizeParams_BwBlockwiseExpansionMapped_setMapping
  void setMapping(QuantizationEncodingMapping mapping) const {
    callSetter<&interface_type::setMapping>(
        static_cast<Qairt_QuantizationEncodingMapping_t>(mapping));
  }

  /// @see QairtQuantizeParams_BwBlockwiseExpansionMapped_getAxis
  int32_t getAxis() const { return callGetter<&interface_type::getAxis>(); }
  /// @see QairtQuantizeParams_BwBlockwiseExpansionMapped_setAxis
  void setAxis(int32_t axis) const { callSetter<&interface_type::setAxis>(axis); }

  /// @see QairtQuantizeParams_BwBlockwiseExpansionMapped_getNumBlocksPerAxis
  uint32_t getNumBlocksPerAxis() const {
    return callGetter<&interface_type::getNumBlocksPerAxis>();
  }
  /// @see QairtQuantizeParams_BwBlockwiseExpansionMapped_setNumBlocksPerAxis
  void setNumBlocksPerAxis(uint32_t numBlocksPerAxis) const {
    callSetter<&interface_type::setNumBlocksPerAxis>(numBlocksPerAxis);
  }

  /// @see QairtQuantizeParams_BwBlockwiseExpansionMapped_getBlockScaleBitwidth
  uint32_t getBlockScaleBitwidth() const {
    return callGetter<&interface_type::getBlockScaleBitwidth>();
  }
  /// @see QairtQuantizeParams_BwBlockwiseExpansionMapped_setBlockScaleBitwidth
  void setBlockScaleBitwidth(uint32_t blockScaleBitwidth) const {
    callSetter<&interface_type::setBlockScaleBitwidth>(blockScaleBitwidth);
  }

  /**
   * @brief Get the per-block scale-offset pairs for this encoding.
   *
   * @return Reference to the vector of @ref ScaleOffset objects, one per quantization block.
   *
   * @throws qairt::Exception on invalid handle.
   */
  std::vector<ScaleOffset>& getScaleOffsets() { return m_scaleOffsets.get(*this); }
  /**
   * @brief Wrapper which allows for const access to the per-block scale-offset pairs.
   *
   * @see BwBlockwiseExpansionMapped::getScaleOffsets()
   */
  const std::vector<ScaleOffset>& getScaleOffsets() const { return m_scaleOffsets.get(*this); }
  /**
   * @brief Set the per-block scale-offset pairs for this encoding.
   *
   * @param[in] scaleOffsets  Scale-offset pairs, one per quantization block.
   *
   * @throws qairt::Exception on invalid handle.
   */
  void setScaleOffsets(const std::vector<ScaleOffset>& scaleOffsets) {
    m_scaleOffsets.set(*this, scaleOffsets);
  }

  /**
   * @brief Get the 8-bit block scale values.
   *
   * @return Reference to the vector of 8-bit scale values.
   *
   * @note Valid only when block scales were set via setBlocksScale8().
   */
  const std::vector<uint8_t>& getBlocksScale8() const { return m_blocksScale8; }
  /**
   * @brief Get the 16-bit block scale values.
   *
   * @return Reference to the vector of 16-bit scale values.
   *
   * @note Valid only when block scales were set via setBlocksScale16().
   */
  const std::vector<uint16_t>& getBlocksScale16() const { return m_blocksScale16; }

  /**
   * @brief Set the 8-bit block scale values.
   *
   * Clears any previously set 16-bit block scales.
   *
   * @param[in] blocksScale8  8-bit block scale values, one per quantization block.
   */
  void setBlocksScale8(const std::vector<uint8_t>& blocksScale8) {
    m_blocksScale8 = blocksScale8;
    m_blocksScale16.clear();
  }

  /**
   * @brief Set the 16-bit block scale values.
   *
   * Clears any previously set 8-bit block scales.
   *
   * @param[in] blocksScale16  16-bit block scale values, one per quantization block.
   */
  void setBlocksScale16(const std::vector<uint16_t>& blocksScale16) {
    m_blocksScale16 = blocksScale16;
    m_blocksScale8.clear();
  }

 private:
  void prepareToCross() const {
    if (!m_blocksScale8.empty()) {
      callSetter<&interface_type::setBlocksScale8>(const_cast<uint8_t*>(m_blocksScale8.data()),
                                                   static_cast<uint32_t>(m_blocksScale8.size()));
    }
    if (!m_blocksScale16.empty()) {
      callSetter<&interface_type::setBlocksScale16>(const_cast<uint16_t*>(m_blocksScale16.data()),
                                                    static_cast<uint32_t>(m_blocksScale16.size()));
    }
  }

  void updateAfterCross() const {
    uint32_t num8{0u};
    callAndConfirm<&interface_type::getNumBlocksScale8>(&num8);
    if (num8 > 0) {
      m_blocksScale8.resize(num8);
      for (uint32_t i = 0; i < num8; ++i) {
        callAndConfirm<&interface_type::getBlocksScale8At>(i, &m_blocksScale8[i]);
      }
    }
    uint32_t num16{0u};
    callAndConfirm<&interface_type::getNumBlocksScale16>(&num16);
    if (num16 > 0) {
      m_blocksScale16.resize(num16);
      for (uint32_t i = 0; i < num16; ++i) {
        callAndConfirm<&interface_type::getBlocksScale16At>(i, &m_blocksScale16[i]);
      }
    }
  }

  explicit BwBlockwiseExpansionMapped(const std::shared_ptr<ApiTable>& apiTable)
      : ApiType(apiTable) {
    construct();
  }
};

/**
 * @brief Microscaling (MX) quantization parameters, including float encoding, block
 *        dimensions, and per-block scale values.
 *
 * Construct directly: Microscaling(). Set the value encoding format via
 * setValueEncoding(), block dimensions via setBlockDimensions(), and block scale data
 * via setBlockScales8() (for Float8 scale type) or setBlockScalesFloat() (for Float16 or
 * Float32 scale type). Attach to a @ref QuantizeParams via
 * @ref QuantizeParams::setMicroscalingEncoding().
 *
 * @note Setting 8-bit block scales implicitly sets the scale data type to Float8; setting
 *       float block scales implicitly sets it to Float32.
 */
class Microscaling : public ApiType<Microscaling, QairtQuantizeParams_MicroscalingEncodingV1_t> {
  friend Api;
  /// @brief Size of each quantization block along each dimension.
  mutable std::vector<uint32_t> m_blockDimensions;
  /// @brief Block scale values; holds uint8_t for Float8 scale data type or float otherwise.
  mutable std::variant<std::monostate, std::vector<uint8_t>, std::vector<float>> m_blockScales;

 public:
  using ApiType::ApiType;
  using ApiType::operator=;

  ~Microscaling(){QAIRT_TRIVIAL} Microscaling() noexcept = default;
  Microscaling(Microscaling&& other) noexcept
      : ApiType(static_cast<ApiType&&>(other)),
        m_blockDimensions(std::move(other.m_blockDimensions)),
        m_blockScales(std::move(other.m_blockScales)) {}
  Microscaling& operator=(Microscaling&& other) noexcept {
    if (this != &other) {
      std::swap(static_cast<ApiType&>(*this), static_cast<ApiType&>(other));
      std::swap(m_blockDimensions, other.m_blockDimensions);
      std::swap(m_blockScales, other.m_blockScales);
    }
    return *this;
  }

  Microscaling(const Microscaling& other) : Microscaling(copy_table_tag, other) {
    construct();
    setValueEncoding(other.getValueEncoding());
    setBlockDimensions(other.getBlockDimensions());
    auto scaleDataType = other.getScaleDataType();
    setScaleDataType(scaleDataType);
    if (scaleDataType == DataType::Float8) {
      setBlockScales8(other.getBlockScales8());
    } else if (scaleDataType == DataType::Float16 || scaleDataType == DataType::Float32) {
      setBlockScalesFloat(other.getBlockScalesFloat());
    }
  }

  Microscaling& operator=(const Microscaling& other) {
    if (this != &other) {
      *this = other.shallowCopy();
    }
    return *this;
  }

  Microscaling shallowCopy() const { return Microscaling(*this); }

  /// @see QairtQuantizeParams_MicroscalingEncoding_getValueEncoding
  FloatEncoding getValueEncoding() const {
    return static_cast<FloatEncoding>(callGetter<&interface_type::getValueEncoding>());
  }
  /// @see QairtQuantizeParams_MicroscalingEncoding_setValueEncoding
  void setValueEncoding(FloatEncoding valueEncoding) const {
    callSetter<&interface_type::setValueEncoding>(
        static_cast<Qairt_FloatEncoding_t>(valueEncoding));
  }

  /**
   * @brief Get the block dimensions for this microscaling encoding.
   *
   * @return Reference to the vector of block dimension sizes, one per quantization dimension.
   */
  const std::vector<uint32_t>& getBlockDimensions() const { return m_blockDimensions; }
  /**
   * @brief Set the block dimensions for this microscaling encoding.
   *
   * @param[in] blockDimensions  Block sizes, one per quantization dimension.
   */
  void setBlockDimensions(const std::vector<uint32_t>& blockDimensions) {
    m_blockDimensions = blockDimensions;
  }
  /// @see Microscaling::setBlockDimensions(const std::vector<uint32_t>&)
  void setBlockDimensions(std::vector<uint32_t>&& blockDimensions) {
    m_blockDimensions = std::move(blockDimensions);
  }

  /**
   * @brief Get the number of blocks in this microscaling encoding.
   *
   * @return Number of block scale values currently stored, or 0 if none have been set.
   */
  size_t getBlockCount() const {
    if (std::holds_alternative<std::vector<uint8_t>>(m_blockScales)) {
      return getBlockScales8().size();
    } else if (std::holds_alternative<std::vector<float>>(m_blockScales)) {
      return getBlockScalesFloat().size();
    } else {
      return 0u;
    }
  }

  /**
   * @brief Set the data type used to store block scale values.
   *
   * Initializes the internal block-scale storage to match the given data type.
   * Float8 uses a `std::vector<uint8_t>` buffer; Float16 and Float32 use a
   * `std::vector<float>` buffer. Calling this does not clear existing scale data
   * unless the storage type changes.
   *
   * @param[in] dtype  Data type for block scales. Must be Float8, Float16, or Float32.
   *                  
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtQuantizeParams_MicroscalingEncoding_setScaleDataType
   */
  void setScaleDataType(DataType dtype) const {
    callSetter<&interface_type::setScaleDataType>(static_cast<Qairt_DataType_t>(dtype));
    if (dtype == DataType::Float8 && !std::holds_alternative<std::vector<uint8_t>>(m_blockScales)) {
      m_blockScales = std::vector<uint8_t>();
    } else if ((dtype == DataType::Float16 || dtype == DataType::Float32) &&
               !std::holds_alternative<std::vector<float>>(m_blockScales)) {
      m_blockScales = std::vector<float>();
    }
  }

  /**
   * @brief Get the data type used to store block scale values.
   *
   * @return The currently set scale data type.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtQuantizeParams_MicroscalingEncoding_getScaleDataType
   */
  DataType getScaleDataType() const {
    return static_cast<DataType>(callGetter<&interface_type::getScaleDataType>());
  }

  /**
   * @brief Get the 8-bit block scale values.
   *
   * Valid when the scale data type is Float8.
   *
   * @return Reference to the vector of 8-bit scale values.
   */
  const std::vector<uint8_t>& getBlockScales8() const {
    if (std::holds_alternative<std::monostate>(m_blockScales)) {
      QAIRT_NO_RETURN m_blockScales.template emplace<std::vector<uint8_t>>();
    }
    return std::get<std::vector<uint8_t>>(m_blockScales);
  }

  /**
   * @brief Set the 8-bit block scale values.
   *
   * Implicitly sets the scale data type to Float8. Replaces any previously
   * set float block scales.
   *
   * @param[in] blockScales8  8-bit block scale values, one per quantization block.
   */
  void setBlockScales8(const std::vector<uint8_t>& blockScales8) {
    if (!std::holds_alternative<std::vector<uint8_t>>(m_blockScales)) {
      m_blockScales = blockScales8;
      setScaleDataType(DataType::Float8);
    } else {
      m_blockScales = blockScales8;
    }
  }
  /// @see Microscaling::setBlockScales8(const std::vector<uint8_t>&)
  void setBlockScales8(std::vector<uint8_t>&& blockScales8) {
    if (!std::holds_alternative<std::vector<uint8_t>>(m_blockScales)) {
      m_blockScales = std::move(blockScales8);
      setScaleDataType(DataType::Float8);
    } else {
      m_blockScales = std::move(blockScales8);
    }
  }

  /**
   * @brief Get the floating-point block scale values.
   *
   * Valid when the scale data type is Float16 or Float32.
   *
   * @return Reference to the vector of float scale values.
   */
  const std::vector<float>& getBlockScalesFloat() const {
    if (std::holds_alternative<std::monostate>(m_blockScales)) {
      QAIRT_NO_RETURN m_blockScales.template emplace<std::vector<float>>();
    }
    return std::get<std::vector<float>>(m_blockScales);
  }

  /**
   * @brief Set the floating-point block scale values.
   *
   * Implicitly sets the scale data type to Float32. Replaces any previously
   * set 8-bit block scales.
   *
   * @param[in] blockScalesFloat  Float block scale values, one per quantization block.
   */
  void setBlockScalesFloat(const std::vector<float>& blockScalesFloat) {
    if (!std::holds_alternative<std::vector<float>>(m_blockScales)) {
      m_blockScales = blockScalesFloat;
      setScaleDataType(DataType::Float32);
    } else {
      m_blockScales = blockScalesFloat;
    }
  }

  /// @see Microscaling::setBlockScalesFloat(const std::vector<float>&)
  void setBlockScalesFloat(std::vector<float>&& blockScalesFloat) {
    if (!std::holds_alternative<std::vector<float>>(m_blockScales)) {
      m_blockScales = std::move(blockScalesFloat);
      setScaleDataType(DataType::Float32);
    } else {
      m_blockScales = std::move(blockScalesFloat);
    }
  }

 private:
  void prepareToCross() const {
    if (!m_blockDimensions.empty()) {
      callSetter<&interface_type::setBlockDimensions>(
          const_cast<uint32_t*>(m_blockDimensions.data()),
          static_cast<uint32_t>(m_blockDimensions.size()));
    }
    if (std::holds_alternative<std::vector<uint8_t>>(m_blockScales)) {
      auto& blockScales = std::get<std::vector<uint8_t>>(m_blockScales);
      callSetter<&interface_type::setBlockScales8>(blockScales.data(),
                                                   static_cast<uint32_t>(blockScales.size()));
    } else if (std::holds_alternative<std::vector<float>>(m_blockScales)) {
      auto& blockScales = std::get<std::vector<float>>(m_blockScales);
      callSetter<&interface_type::setBlockScalesFloat>(blockScales.data(),
                                                       static_cast<uint32_t>(blockScales.size()));
    } else {
      callSetter<&interface_type::setScaleDataType>(QAIRT_DATATYPE_UNDEFINED);
    }
  }

  void updateAfterCross() const {
    m_blockDimensions.clear();
    uint32_t blockRank{0u};
    callAndConfirm<&interface_type::getNumBlockDimensions>(&blockRank);
    m_blockDimensions.resize(blockRank);
    for (uint32_t i = 0; i < blockRank; ++i) {
      callAndConfirm<&interface_type::getBlockDimensionAt>(i, &m_blockDimensions[i]);
    }
    Qairt_DataType_t dataType;
    callAndConfirm<&interface_type::getScaleDataType>(&dataType);
    uint32_t numScales{0u};
    callAndConfirm<&interface_type::getNumBlockScales>(&numScales);
    switch (dataType) {
      case QAIRT_DATATYPE_FLOAT_8: {
        std::vector<uint8_t> scales(numScales);
        for (uint32_t i = 0; i < numScales; ++i) {
          callAndConfirm<&interface_type::getBlockScale8At>(i, &scales[i]);
        }
        m_blockScales = std::move(scales);
      } break;
      case QAIRT_DATATYPE_FLOAT_16:
      case QAIRT_DATATYPE_FLOAT_32: {
        std::vector<float> scales(numScales);
        for (uint32_t i = 0; i < numScales; ++i) {
          callAndConfirm<&interface_type::getBlockScaleFloatAt>(i, &scales[i]);
        }
        m_blockScales = std::move(scales);
      } break;
      default:
        m_blockScales = std::monostate();
        break;
    }
  }

  explicit Microscaling(const std::shared_ptr<ApiTable>& apiTable) : ApiType(apiTable) {
    construct();
  }
};

/**
 * @brief Bit-width and per-axis float scales and integer offsets for bit-width per-axis
 *        scale-offset quantization.
 *
 * Construct directly: BwAxisScaleOffset(uint32_t bitwidth, int32_t axis,
 * const std::vector<float>& scales, const std::vector<int32_t>& offsets). Attach to a
 * @ref QuantizeParams via @ref QuantizeParams::setBwAxisScaleOffsetEncoding(), or supply
 * as the codebook descriptor for @ref VectorEncoding.
 */
class BwAxisScaleOffset
    : public ApiType<BwAxisScaleOffset, QairtQuantizeParams_BwAxisScaleOffsetV1_t> {
  friend Api;

  /// @brief Per-axis scale values, one per element along the quantization axis.
  mutable std::vector<float> m_scales;
  /// @brief Per-axis offset values, one per element along the quantization axis.
  mutable std::vector<int32_t> m_offsets;

 public:
  using ApiType::ApiType;
  using ApiType::operator=;

  ~BwAxisScaleOffset(){QAIRT_TRIVIAL} BwAxisScaleOffset() noexcept = default;
  BwAxisScaleOffset(BwAxisScaleOffset&& other) noexcept
      : ApiType(static_cast<ApiType&&>(other)),
        m_scales(std::move(other.m_scales)),
        m_offsets(std::move(other.m_offsets)) {}
  BwAxisScaleOffset& operator=(BwAxisScaleOffset&& other) noexcept {
    if (this != &other) {
      std::swap(static_cast<ApiType&>(*this), static_cast<ApiType&>(other));
      std::swap(m_scales, other.m_scales);
      std::swap(m_offsets, other.m_offsets);
    }
    return *this;
  }

  BwAxisScaleOffset(const BwAxisScaleOffset& other) : BwAxisScaleOffset(copy_table_tag, other) {
    construct();
    setBitwidth(other.getBitwidth());
    setAxis(other.getAxis());
    setScales(other.getScales());
    setOffsets(other.getOffsets());
  }

  BwAxisScaleOffset& operator=(const BwAxisScaleOffset& other) {
    if (this != &other) {
      *this = other.shallowCopy();
    }
    return *this;
  }

  BwAxisScaleOffset shallowCopy() const { return BwAxisScaleOffset(*this); }

  /**
   * @brief Construct a bit-width per-axis scale-offset encoding with the given parameters.
   *
   * @param[in] bitwidth  Storage bit-width of the quantized values.
   * @param[in] axis      Quantization axis index.
   * @param[in] scales    Per-axis scale values, one per element along the quantization axis.
   * @param[in] offsets   Per-axis integer zero-point offsets, one per axis element.
   */
  BwAxisScaleOffset(uint32_t bitwidth,
                    int32_t axis,
                    const std::vector<float>& scales,
                    const std::vector<int32_t>& offsets) {
    construct();
    setBitwidth(bitwidth);
    setAxis(axis);
    setScales(scales);
    setOffsets(offsets);
  }

  /// @see QairtQuantizeParams_BwAxisScaleOffset_getBw
  uint32_t getBitwidth() const { return callGetter<&interface_type::getBw>(); }
  /// @see QairtQuantizeParams_BwAxisScaleOffset_setBw
  void setBitwidth(uint32_t bitwidth) const { callSetter<&interface_type::setBw>(bitwidth); }
  /// @see QairtQuantizeParams_BwAxisScaleOffset_getAxis
  int32_t getAxis() const { return callGetter<&interface_type::getAxis>(); }
  /// @see QairtQuantizeParams_BwAxisScaleOffset_setAxis
  void setAxis(int32_t axis) const { callSetter<&interface_type::setAxis>(axis); }

  /**
   * @brief Get the per-axis scale values.
   *
   * @return Reference to the vector of scale values, one per element along the quantization axis.
   */
  const std::vector<float>& getScales() const { return m_scales; }
  /**
   * @brief Set the per-axis scale values.
   *
   * @param[in] scales  Scale values, one per element along the quantization axis.
   */
  void setScales(const std::vector<float>& scales) { m_scales = scales; }
  /// @see BwAxisScaleOffset::setScales(const std::vector<float>&)
  void setScales(std::vector<float>&& scales) { m_scales = std::move(scales); }

  /**
   * @brief Get the per-axis integer offset values.
   *
   * @return Reference to the vector of offset values, one per element along the quantization axis.
   */
  const std::vector<int32_t>& getOffsets() const { return m_offsets; }
  /**
   * @brief Set the per-axis integer offset values.
   *
   * @param[in] offsets  Integer zero-point offsets, one per element along the quantization axis.
   */
  void setOffsets(const std::vector<int32_t>& offsets) { m_offsets = offsets; }
  /// @see BwAxisScaleOffset::setOffsets(const std::vector<int32_t>&)
  void setOffsets(std::vector<int32_t>&& offsets) { m_offsets = std::move(offsets); }

 private:
  void prepareToCross() const {
    if (!m_scales.empty()) {
      callSetter<&interface_type::setScales>(const_cast<float*>(m_scales.data()),
                                             static_cast<uint32_t>(m_scales.size()));
    }
    if (!m_offsets.empty()) {
      callSetter<&interface_type::setOffsets>(const_cast<int32_t*>(m_offsets.data()),
                                              static_cast<uint32_t>(m_offsets.size()));
    }
  }

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

  explicit BwAxisScaleOffset(const std::shared_ptr<ApiTable>& apiTable) : ApiType(apiTable) {
    construct();
  }
};

/**
 * @brief Block sizes and per-block scale-offset pairs for per-block scale-offset quantization.
 *
 * Construct directly: BlockEncoding(const std::vector<uint32_t>& blockSize,
 * const std::vector<ScaleOffset>& scaleOffsets). Attach to a @ref QuantizeParams via
 * @ref QuantizeParams::setBlockEncoding().
 */
class BlockEncoding : public ApiType<BlockEncoding, QairtQuantizeParams_BlockEncodingV1_t> {
  friend Api;

  /// @brief Block sizes along each quantization dimension.
  mutable std::vector<uint32_t> m_blockSize;
  /// @brief Scale-offset pairs for each quantization block.
  detail::crossable<std::vector<detail::non_owning<ScaleOffset>>,
                    &interface_type::getScaleOffsetAt,
                    &interface_type::getNumScaleOffsets,
                    &interface_type::setScaleOffsets>
      m_scaleOffsets;

 public:
  using ApiType::ApiType;
  using ApiType::operator=;

  ~BlockEncoding(){QAIRT_TRIVIAL} BlockEncoding() noexcept = default;
  BlockEncoding(BlockEncoding&& other) noexcept
      : ApiType(static_cast<ApiType&&>(other)),
        m_blockSize(std::move(other.m_blockSize)),
        m_scaleOffsets(std::move(other.m_scaleOffsets)) {}
  BlockEncoding& operator=(BlockEncoding&& other) noexcept {
    if (this != &other) {
      std::swap(static_cast<ApiType&>(*this), static_cast<ApiType&>(other));
      std::swap(m_blockSize, other.m_blockSize);
      std::swap(m_scaleOffsets, other.m_scaleOffsets);
    }
    return *this;
  }

  BlockEncoding(const BlockEncoding& other) : BlockEncoding(copy_table_tag, other) {
    construct();
    setBlockSizes(other.getBlockSizes());
    setScaleOffsets(other.getScaleOffsets());
  }

  BlockEncoding& operator=(const BlockEncoding& other) {
    if (this != &other) {
      *this = other.shallowCopy();
    }
    return *this;
  }

  BlockEncoding shallowCopy() const { return BlockEncoding(*this); }

  /**
   * @brief Construct a block encoding with the given block sizes and per-block scale-offset pairs.
   *
   * @param[in] blockSize    Block sizes, one per quantization dimension.
   * @param[in] scaleOffsets Scale-offset pairs, one per quantization block.
   */
  BlockEncoding(const std::vector<uint32_t>& blockSize,
                const std::vector<ScaleOffset>& scaleOffsets) {
    construct();
    setBlockSizes(blockSize);
    setScaleOffsets(scaleOffsets);
  }

  /**
   * @brief Get the block sizes along each quantization dimension.
   *
   * @return Vector of block sizes.
   */
  std::vector<uint32_t> getBlockSizes() const { return m_blockSize; }

  /**
   * @brief Set the block sizes along each quantization dimension.
   *
   * @param[in] blockSizes  Block sizes, one per quantization dimension.
   */
  void setBlockSizes(const std::vector<uint32_t>& blockSizes) const { m_blockSize = blockSizes; }
  /// @see BlockEncoding::setBlockSizes(const std::vector<uint32_t>&)
  void setBlockSizes(std::vector<uint32_t>&& blockSizes) const {
    m_blockSize = std::move(blockSizes);
  }

  /**
   * @brief Get the per-block scale-offset pairs for this encoding.
   *
   * @return Reference to the vector of @ref ScaleOffset objects, one per quantization block.
   *
   * @throws qairt::Exception on invalid handle.
   */
  std::vector<ScaleOffset>& getScaleOffsets() { return m_scaleOffsets.get(*this); }
  /**
   * @brief Wrapper which allows for const access to the per-block scale-offset pairs.
   *
   * @see BlockEncoding::getScaleOffsets()
   */
  const std::vector<ScaleOffset>& getScaleOffsets() const { return m_scaleOffsets.get(*this); }
  /**
   * @brief Set the per-block scale-offset pairs for this encoding.
   *
   * @param[in] scaleOffsets  Scale-offset pairs, one per quantization block.
   *
   * @throws qairt::Exception on invalid handle.
   */
  void setScaleOffsets(std::vector<ScaleOffset> scaleOffsets) {
    m_scaleOffsets.set(*this, scaleOffsets);
  }

 private:
  void prepareToCross() const {
    const auto num = static_cast<uint32_t>(m_blockSize.size());
    callSetter<&interface_type::setBlockSize>(const_cast<uint32_t*>(m_blockSize.data()), num);
  }
  void updateAfterCross() const {
    uint32_t numBlockSize{0u};
    callAndConfirm<&interface_type::getNumBlockSize>(&numBlockSize);
    if (numBlockSize > 0) {
      m_blockSize.resize(numBlockSize);
    }

    for (uint32_t i = 0; i < numBlockSize; ++i) {
      // call again with buffer
      callAndConfirm<&interface_type::getBlockSizeAt>(i, &m_blockSize[i]);
    }
  }
  explicit BlockEncoding(const std::shared_ptr<ApiTable>& apiTable) : ApiType(apiTable) {
    construct();
  }
};

/**
 * @brief Vector quantization (VQ) compression parameters, including a codebook
 *        descriptor and block-layout configuration.
 *
 * Construct directly: VectorEncoding(const BwAxisScaleOffset& bwAxisScaleOffset,
 * uint32_t rowsPerBlock, uint32_t colsPerBlock, uint32_t vectorStride,
 * uint32_t vectorDimension, uint32_t indexBitwidth). Attach to a @ref QuantizeParams via
 * @ref QuantizeParams::setVectorEncoding().
 */
class VectorEncoding : public ApiType<VectorEncoding, QairtQuantizeParams_VectorEncodingV1_t> {
  friend Api;

  using CrossableBwAxisScaleOffset = detail::crossable<detail::non_owning<BwAxisScaleOffset>,
                                                       &interface_type::getBwAxisScaleOffset,
                                                       &interface_type::setBwAxisScaleOffset>;
  /// @brief Bit-width per-axis scale-offset encoding used for the vector codebook.
  mutable CrossableBwAxisScaleOffset m_bwAxisScaleOffset;

 public:
  using ApiType::ApiType;
  using ApiType::operator=;

  ~VectorEncoding(){QAIRT_TRIVIAL} VectorEncoding() noexcept = default;
  VectorEncoding(VectorEncoding&& other) noexcept
      : ApiType(static_cast<ApiType&&>(other)),
        m_bwAxisScaleOffset(std::move(other.m_bwAxisScaleOffset)) {}
  VectorEncoding& operator=(VectorEncoding&& other) noexcept {
    if (this != &other) {
      std::swap(static_cast<ApiType&>(*this), static_cast<ApiType&>(other));
      std::swap(m_bwAxisScaleOffset, other.m_bwAxisScaleOffset);
    }
    return *this;
  }

  VectorEncoding(const VectorEncoding& other) : VectorEncoding(copy_table_tag, other) {
    construct();
    setBwAxisScaleOffset(other.getBwAxisScaleOffset());
    setRowsPerBlock(other.getRowsPerBlock());
    setColsPerBlock(other.getColsPerBlock());
    setVectorDimension(other.getVectorDimension());
    setVectorStride(other.getVectorStride());
    setIndexBw(other.getIndexBw());
  }

  VectorEncoding& operator=(const VectorEncoding& other) {
    if (this != &other) {
      *this = other.shallowCopy();
    }
    return *this;
  }

  VectorEncoding shallowCopy() const { return VectorEncoding(*this); }

  /**
   * @brief Construct a vector encoding with the given codebook and block layout.
   *
   * @param[in] bwAxisScaleOffset  Bit-width per-axis scale-offset encoding for the codebook.
   * @param[in] rowsPerBlock       Number of rows per block.
   * @param[in] colsPerBlock       Number of columns per block.
   * @param[in] vectorStride       Stride between vectors in the compressed representation.
   *                              
   * @param[in] vectorDimension    Dimension along which vectors are formed.
   * @param[in] indexBitwidth      Bit-width of codebook indices.
   */
  VectorEncoding(const BwAxisScaleOffset& bwAxisScaleOffset,
                 uint32_t rowsPerBlock,
                 uint32_t colsPerBlock,
                 uint32_t vectorStride,
                 uint32_t vectorDimension,
                 uint32_t indexBitwidth) {
    construct();
    setBwAxisScaleOffset(bwAxisScaleOffset);
    setRowsPerBlock(rowsPerBlock);
    setColsPerBlock(colsPerBlock);
    setVectorStride(vectorStride);
    setVectorDimension(vectorDimension);
    setIndexBw(indexBitwidth);
  }

  /// @see QairtQuantizeParams_VectorEncoding_getRowsPerBlock
  uint32_t getRowsPerBlock() const { return callGetter<&interface_type::getRowsPerBlock>(); }
  /// @see QairtQuantizeParams_VectorEncoding_setRowsPerBlock
  void setRowsPerBlock(uint32_t rowsPerBlock) const {
    callSetter<&interface_type::setRowsPerBlock>(rowsPerBlock);
  }
  /// @see QairtQuantizeParams_VectorEncoding_getColsPerBlock
  uint32_t getColsPerBlock() const { return callGetter<&interface_type::getColsPerBlock>(); }
  /// @see QairtQuantizeParams_VectorEncoding_setColsPerBlock
  void setColsPerBlock(uint32_t colsPerBlock) const {
    callSetter<&interface_type::setColsPerBlock>(colsPerBlock);
  }
  /// @see QairtQuantizeParams_VectorEncoding_getVectorStride
  uint32_t getVectorStride() const { return callGetter<&interface_type::getVectorStride>(); }
  /// @see QairtQuantizeParams_VectorEncoding_setVectorStride
  void setVectorStride(uint32_t vectorStride) const {
    callSetter<&interface_type::setVectorStride>(vectorStride);
  }
  /// @see QairtQuantizeParams_VectorEncoding_getVectorDimension
  uint32_t getVectorDimension() const { return callGetter<&interface_type::getVectorDimension>(); }
  /// @see QairtQuantizeParams_VectorEncoding_setVectorDimension
  void setVectorDimension(uint32_t vectorDimension) const {
    callSetter<&interface_type::setVectorDimension>(vectorDimension);
  }
  /// @see QairtQuantizeParams_VectorEncoding_getIndexBw
  uint32_t getIndexBw() const { return callGetter<&interface_type::getIndexBw>(); }
  /// @see QairtQuantizeParams_VectorEncoding_setIndexBw
  void setIndexBw(uint32_t indexBw) const { callSetter<&interface_type::setIndexBw>(indexBw); }

  /**
   * @brief Get the bit-width per-axis scale-offset encoding used for the vector codebook.
   *
   * @return Const reference to the @ref BwAxisScaleOffset codebook descriptor.
   *
   * @throws qairt::Exception on invalid handle.
   */
  const BwAxisScaleOffset& getBwAxisScaleOffset() const {
    return m_bwAxisScaleOffset.bindNonOwning(*this);
  }

  /**
   * @brief Set the bit-width per-axis scale-offset encoding used for the vector codebook.
   *
   * @param[in] bwAxisScaleOffset  Codebook descriptor for the vector quantization.
   *
   * @throws qairt::Exception on invalid handle.
   */
  void setBwAxisScaleOffset(const BwAxisScaleOffset& bwAxisScaleOffset) {
    QAIRT_NO_RETURN m_bwAxisScaleOffset.setNonOwning(*this, bwAxisScaleOffset);
  }

 private:
  void prepareToCross() const { m_bwAxisScaleOffset.prepareToCross(*this); }
  void updateAfterCross() const { m_bwAxisScaleOffset.updateAfterCross(*this); }
  explicit VectorEncoding(const std::shared_ptr<ApiTable>& apiTable) : ApiType(apiTable) {
    construct();
  }
};

/**
 * @brief Per-axis blockwise expansion quantization parameters, including axis, per-block
 *        scale-offset pairs, and block scale data.
 *
 * Construct directly: BlockwiseExpansion(int32_t axis,
 * const std::vector<ScaleOffset>& scaleOffsets, uint32_t numBlocksPerAxis,
 * uint32_t blockScaleBitwidth). Supply block scale data as either 8-bit values via
 * setBlocksScale8() or 16-bit values via setBlocksScale16() — these are mutually
 * exclusive. Attach to a @ref QuantizeParams via
 * @ref QuantizeParams::setBlockwiseExpansion().
 *
 * @note Setting 8-bit block scales clears any 16-bit block scales and vice versa.
 */
class BlockwiseExpansion
    : public ApiType<BlockwiseExpansion, QairtQuantizeParams_BlockwiseExpansionV1_t> {
  friend Api;

  /// @brief Scale-offset pairs for each quantization block in the expansion encoding.
  detail::crossable<std::vector<detail::non_owning<ScaleOffset>>,
                    &interface_type::getScaleOffsetAt,
                    &interface_type::getNumScaleOffsets,
                    &interface_type::setScaleOffsets>
      m_scaleOffsets;

  /// @brief Block scale values stored as 8-bit data; mutually exclusive with m_blocksScale16.
  mutable std::vector<uint8_t> m_blocksScale8;
  /// @brief Block scale values stored as 16-bit data; mutually exclusive with m_blocksScale8.
  mutable std::vector<uint16_t> m_blocksScale16;

 public:
  using ApiType::ApiType;
  using ApiType::operator=;

  ~BlockwiseExpansion(){QAIRT_TRIVIAL} BlockwiseExpansion() noexcept = default;
  BlockwiseExpansion(BlockwiseExpansion&& other) noexcept
      : ApiType(static_cast<ApiType&&>(other)),
        m_scaleOffsets(std::move(other.m_scaleOffsets)),
        m_blocksScale8(std::move(other.m_blocksScale8)),
        m_blocksScale16(std::move(other.m_blocksScale16)) {}
  BlockwiseExpansion& operator=(BlockwiseExpansion&& other) noexcept {
    if (this != &other) {
      std::swap(static_cast<ApiType&>(*this), static_cast<ApiType&>(other));
      std::swap(m_scaleOffsets, other.m_scaleOffsets);
      std::swap(m_blocksScale8, other.m_blocksScale8);
      std::swap(m_blocksScale16, other.m_blocksScale16);
    }
    return *this;
  }

  BlockwiseExpansion(const BlockwiseExpansion& other) : BlockwiseExpansion(copy_table_tag, other) {
    construct();
    setAxis(other.getAxis());
    setScaleOffsets(other.getScaleOffsets());
    setNumBlocksPerAxis(other.getNumBlocksPerAxis());
    setBlockScaleBitwidth(other.getBlockScaleBitwidth());
    switch (other.getStorageType()) {
      case BlockwiseExpansionBlockScaleStorageType::Storage8:
        setBlocksScale8(other.getBlocksScale8());
        break;
      case BlockwiseExpansionBlockScaleStorageType::Storage16:
        setBlocksScale16(other.getBlocksScale16());
        break;
      default:
        break;
    }
  }

  BlockwiseExpansion& operator=(const BlockwiseExpansion& other) {
    if (this != &other) {
      *this = other.shallowCopy();
    }
    return *this;
  }

  BlockwiseExpansion shallowCopy() const { return BlockwiseExpansion(*this); }

  BlockwiseExpansion(int32_t axis,
                     const std::vector<ScaleOffset>& ScaleOffsets,
                     uint32_t numBlocksPerAxis,
                     uint32_t blockScaleBitwidth) {
    construct();
    setAxis(axis);
    setScaleOffsets(ScaleOffsets);
    setNumBlocksPerAxis(numBlocksPerAxis);
    setBlockScaleBitwidth(blockScaleBitwidth);
  }

  int32_t getAxis() const { return callGetter<&interface_type::getAxis>(); }

  void setAxis(int32_t axis) const { callSetter<&interface_type::setAxis>(axis); }

  uint32_t getNumBlocksPerAxis() const {
    return callGetter<&interface_type::getNumBlocksPerAxis>();
  }

  void setNumBlocksPerAxis(uint32_t setNumBlocks) const {
    callSetter<&interface_type::setNumBlocksPerAxis>(setNumBlocks);
  }

  uint32_t getBlockScaleBitwidth() const { return callGetter<&interface_type::getBlockScaleBw>(); }

  void setBlockScaleBitwidth(uint32_t setBlockScaleBw) const {
    callSetter<&interface_type::setBlockScaleBw>(setBlockScaleBw);
  }

  std::vector<ScaleOffset>& getScaleOffsets() { return m_scaleOffsets.get(*this); }
  const std::vector<ScaleOffset>& getScaleOffsets() const { return m_scaleOffsets.get(*this); }
  void setScaleOffsets(const std::vector<ScaleOffset>& scaleOffsets) {
    m_scaleOffsets.set(*this, scaleOffsets);
  }

  BlockwiseExpansionBlockScaleStorageType getStorageType() const {
    Qairt_BlockwiseExpansionBlockScaleStorageType_t out{};
    callAndConfirm<&interface_type::getStorageType>(&out);
    return static_cast<BlockwiseExpansionBlockScaleStorageType>(out);
  }

  const std::vector<uint8_t>& getBlocksScale8() const { return m_blocksScale8; }
  const std::vector<uint16_t>& getBlocksScale16() const { return m_blocksScale16; }

  void setBlocksScale8(const std::vector<uint8_t>& blocksScale8) {
    m_blocksScale8 = blocksScale8;
    m_blocksScale16.clear();  // Clear conflicting storage
  }

  void setBlocksScale16(const std::vector<uint16_t>& blocksScale16) {
    m_blocksScale16 = blocksScale16;
    m_blocksScale8.clear();
  }

 private:
  void prepareToCross() const {
    if (!m_blocksScale8.empty()) {
      callSetter<&interface_type::setBlockScale8>(const_cast<uint8_t*>(m_blocksScale8.data()),
                                                  static_cast<uint32_t>(m_blocksScale8.size()));
    }
    if (!m_blocksScale16.empty()) {
      callSetter<&interface_type::setBlockScale16>(const_cast<uint16_t*>(m_blocksScale16.data()),
                                                   static_cast<uint32_t>(m_blocksScale16.size()));
    }
  }

  void updateAfterCross() const {
    auto storageType = getStorageType();
    if (storageType == BlockwiseExpansionBlockScaleStorageType::Storage8) {
      uint32_t num8{0u};
      callAndConfirm<&interface_type::getNumBlockScale8>(&num8);
      if (num8 > 0) {
        m_blocksScale8.resize(num8);
        for (uint32_t i = 0; i < num8; ++i) {
          callAndConfirm<&interface_type::getBlockScale8At>(i, &m_blocksScale8[i]);
        }
      }
    } else if (storageType == BlockwiseExpansionBlockScaleStorageType::Storage16) {
      uint32_t num16{0u};
      callAndConfirm<&interface_type::getNumBlockScale16>(&num16);
      if (num16 > 0) {
        m_blocksScale16.resize(num16);
        for (uint32_t i = 0; i < num16; ++i) {
          callAndConfirm<&interface_type::getBlockScale16At>(i, &m_blocksScale16[i]);
        }
      }
    }
  }
  explicit BlockwiseExpansion(const std::shared_ptr<ApiTable>& apiTable) : ApiType(apiTable) {
    construct();
  }
};

/**
 * @brief Container for the active quantization encoding applied to a tensor.
 *
 * Construct directly: QuantizeParams(). Set exactly one encoding variant by calling the
 * corresponding setter (e.g., setScaleOffsetEncoding(), setBwAxisScaleOffsetEncoding()).
 * Attach to a @ref Tensor via @ref Tensor::setQuantizeParams(). The active encoding type
 * is identified at runtime via getQuantizationEncoding().
 */
class QuantizeParams : public ApiType<QuantizeParams, QairtQuantizeParams_V1_t> {
  friend class Api;
  using CrossableScaleOffset     = detail::crossable<detail::non_owning<ScaleOffset>,
                                                 &interface_type::getScaleOffset,
                                                 &interface_type::setScaleOffset>;
  using CrossableAxisScaleOffset = detail::crossable<detail::non_owning<AxisScaleOffset>,
                                                     &interface_type::getAxisScaleOffset,
                                                     &interface_type::setAxisScaleOffset>;
  using CrossableBwScaleOffset   = detail::crossable<detail::non_owning<BwScaleOffset>,
                                                   &interface_type::getBwScaleOffset,
                                                   &interface_type::setBwScaleOffset>;
  using CrossableBwAxisScaleOffsetMapped =
      detail::crossable<detail::non_owning<BwAxisScaleOffsetMapped>,
                        &interface_type::getBwAxisScaleOffsetMapped,
                        &interface_type::setBwAxisScaleOffsetMapped>;
  using CrossableMicroscaling         = detail::crossable<detail::non_owning<Microscaling>,
                                                  &interface_type::getMicroscalingEncoding,
                                                  &interface_type::setMicroscalingEncoding>;
  using CrossableBwAxisScaleOffset    = detail::crossable<detail::non_owning<BwAxisScaleOffset>,
                                                       &interface_type::getBwAxisScaleOffset,
                                                       &interface_type::setBwAxisScaleOffset>;
  using CrossableBlockEncoding        = detail::crossable<detail::non_owning<BlockEncoding>,
                                                   &interface_type::getBlockEncoding,
                                                   &interface_type::setBlockEncoding>;
  using CrossableVectorEncoding       = detail::crossable<detail::non_owning<VectorEncoding>,
                                                    &interface_type::getVectorEncoding,
                                                    &interface_type::setVectorEncoding>;
  using CrossableBlockwiseExpansion   = detail::crossable<detail::non_owning<BlockwiseExpansion>,
                                                        &interface_type::getBlockwiseExpansion,
                                                        &interface_type::setBlockwiseExpansion>;
  using CrossableBwFloatBlockEncoding = detail::crossable<detail::non_owning<BwFloatBlockEncoding>,
                                                          &interface_type::getBwFloatBlockEncoding,
                                                          &interface_type::setBwFloatBlockEncoding>;
  using CrossableFloatBlockEncoding   = detail::crossable<detail::non_owning<FloatBlockEncoding>,
                                                        &interface_type::getFloatBlockEncoding,
                                                        &interface_type::setFloatBlockEncoding>;
  using CrossableBwBlockMapped        = detail::crossable<detail::non_owning<BwBlockMapped>,
                                                   &interface_type::getBwBlockMapped,
                                                   &interface_type::setBwBlockMapped>;
  using CrossableBwBlockwiseExpansionMapped =
      detail::crossable<detail::non_owning<BwBlockwiseExpansionMapped>,
                        &interface_type::getBwBlockwiseExpansionMapped,
                        &interface_type::setBwBlockwiseExpansionMapped>;
  /// @brief Active quantization encoding; the variant alternative indicates the encoding type.
  mutable std::variant<std::monostate,
                       CrossableScaleOffset,
                       CrossableAxisScaleOffset,
                       CrossableBwScaleOffset,
                       CrossableBwAxisScaleOffsetMapped,
                       CrossableMicroscaling,
                       CrossableBwAxisScaleOffset,
                       CrossableBlockEncoding,
                       CrossableVectorEncoding,
                       CrossableBlockwiseExpansion,
                       CrossableBwFloatBlockEncoding,
                       CrossableFloatBlockEncoding,
                       CrossableBwBlockMapped,
                       CrossableBwBlockwiseExpansionMapped>
      m_encoding;

 public:
  using ApiType::ApiType;
  using ApiType::operator=;
  ~QuantizeParams(){QAIRT_TRIVIAL} QuantizeParams() noexcept = default;
  QuantizeParams(QuantizeParams&& other) noexcept
      : ApiType(static_cast<ApiType&&>(other)), m_encoding(std::move(other.m_encoding)) {}
  QuantizeParams& operator=(QuantizeParams&& other) noexcept {
    if (this != &other) {
      std::swap(static_cast<ApiType&>(*this), static_cast<ApiType&>(other));
      std::swap(m_encoding, other.m_encoding);
    }
    return *this;
  }
  QuantizeParams(const QuantizeParams& other) : QuantizeParams(copy_table_tag, other) {
    construct();
    switch (other.getQuantizationEncoding()) {
      case QuantizationEncoding::ScaleOffset:
        m_encoding = CrossableScaleOffset();
        QAIRT_NO_RETURN std::get<CrossableScaleOffset>(m_encoding)
            .setNonOwning(*this, other.getScaleOffsetEncoding());
        break;
      case QuantizationEncoding::AxisScaleOffset:
        m_encoding = CrossableAxisScaleOffset();
        QAIRT_NO_RETURN std::get<CrossableAxisScaleOffset>(m_encoding)
            .setNonOwning(*this, other.getAxisScaleOffsetEncoding());
        break;
      case QuantizationEncoding::BwScaleOffset:
        m_encoding = CrossableBwScaleOffset();
        QAIRT_NO_RETURN std::get<CrossableBwScaleOffset>(m_encoding)
            .setNonOwning(*this, other.getBwScaleOffsetEncoding());
        break;
      case QuantizationEncoding::BwAxisScaleOffsetMapped:
        m_encoding = CrossableBwAxisScaleOffsetMapped();
        QAIRT_NO_RETURN std::get<CrossableBwAxisScaleOffsetMapped>(m_encoding)
            .setNonOwning(*this, other.getBwAxisScaleOffsetMappedEncoding());
        break;
      case QuantizationEncoding::Microscaling:
        m_encoding = CrossableMicroscaling();
        QAIRT_NO_RETURN std::get<CrossableMicroscaling>(m_encoding)
            .setNonOwning(*this, other.getMicroscalingEncoding());
        break;
      case QuantizationEncoding::BwFloatBlock:
        m_encoding = CrossableBwFloatBlockEncoding();
        QAIRT_NO_RETURN std::get<CrossableBwFloatBlockEncoding>(m_encoding)
            .setNonOwning(*this, other.getBwFloatBlockEncoding());
        break;
      case QuantizationEncoding::FloatBlock:
        m_encoding = CrossableFloatBlockEncoding();
        QAIRT_NO_RETURN std::get<CrossableFloatBlockEncoding>(m_encoding)
            .setNonOwning(*this, other.getFloatBlockEncoding());
        break;
      case QuantizationEncoding::BwAxisScaleOffset:
        m_encoding = CrossableBwAxisScaleOffset();
        QAIRT_NO_RETURN std::get<CrossableBwAxisScaleOffset>(m_encoding)
            .setNonOwning(*this, other.getBwAxisScaleOffsetEncoding());
        break;
      case QuantizationEncoding::Block:
        m_encoding = CrossableBlockEncoding();
        QAIRT_NO_RETURN std::get<CrossableBlockEncoding>(m_encoding)
            .setNonOwning(*this, other.getBlockEncoding());
        break;
      case QuantizationEncoding::BlockwiseExpansion:
        m_encoding = CrossableBlockwiseExpansion();
        QAIRT_NO_RETURN std::get<CrossableBlockwiseExpansion>(m_encoding)
            .setNonOwning(*this, other.getBlockwiseExpansion());
        break;
      case QuantizationEncoding::Vector:
        m_encoding = CrossableVectorEncoding();
        QAIRT_NO_RETURN std::get<CrossableVectorEncoding>(m_encoding)
            .setNonOwning(*this, other.getVectorEncoding());
        break;
      case QuantizationEncoding::BwBlockMapped:
        m_encoding = CrossableBwBlockMapped();
        QAIRT_NO_RETURN std::get<CrossableBwBlockMapped>(m_encoding)
            .setNonOwning(*this, other.getBwBlockMapped());
        break;
      case QuantizationEncoding::BwBlockwiseExpansionMapped:
        m_encoding = CrossableBwBlockwiseExpansionMapped();
        QAIRT_NO_RETURN std::get<CrossableBwBlockwiseExpansionMapped>(m_encoding)
            .setNonOwning(*this, other.getBwBlockwiseExpansionMapped());
        break;
      default:
        m_encoding = std::monostate();
        break;
    }
  }

  QuantizeParams shallowCopy() const {
    QuantizeParams toRet(copy_table_tag, *this);
    toRet.construct();
    switch (getQuantizationEncoding()) {
      case QuantizationEncoding::ScaleOffset:
        toRet.m_encoding = CrossableScaleOffset();
        QAIRT_NO_RETURN std::get<CrossableScaleOffset>(toRet.m_encoding)
            .setNonOwning(toRet, getScaleOffsetEncoding().shallowCopy());
        break;
      case QuantizationEncoding::AxisScaleOffset:
        toRet.m_encoding = CrossableAxisScaleOffset();
        QAIRT_NO_RETURN std::get<CrossableAxisScaleOffset>(toRet.m_encoding)
            .setNonOwning(toRet, getAxisScaleOffsetEncoding().shallowCopy());
        break;
      case QuantizationEncoding::BwScaleOffset:
        toRet.m_encoding = CrossableBwScaleOffset();
        QAIRT_NO_RETURN std::get<CrossableBwScaleOffset>(toRet.m_encoding)
            .setNonOwning(toRet, getBwScaleOffsetEncoding().shallowCopy());
        break;
      case QuantizationEncoding::BwAxisScaleOffsetMapped:
        toRet.m_encoding = CrossableBwAxisScaleOffsetMapped();
        QAIRT_NO_RETURN std::get<CrossableBwAxisScaleOffsetMapped>(toRet.m_encoding)
            .setNonOwning(toRet, getBwAxisScaleOffsetMappedEncoding().shallowCopy());
        break;
      case QuantizationEncoding::Microscaling:
        toRet.m_encoding = CrossableMicroscaling();
        QAIRT_NO_RETURN std::get<CrossableMicroscaling>(toRet.m_encoding)
            .setNonOwning(toRet, getMicroscalingEncoding().shallowCopy());
        break;
      case QuantizationEncoding::BwFloatBlock:
        toRet.m_encoding = CrossableBwFloatBlockEncoding();
        QAIRT_NO_RETURN std::get<CrossableBwFloatBlockEncoding>(toRet.m_encoding)
            .setNonOwning(toRet, getBwFloatBlockEncoding().shallowCopy());
        break;
      case QuantizationEncoding::FloatBlock:
        toRet.m_encoding = CrossableFloatBlockEncoding();
        QAIRT_NO_RETURN std::get<CrossableFloatBlockEncoding>(toRet.m_encoding)
            .setNonOwning(toRet, getFloatBlockEncoding().shallowCopy());
        break;
      case QuantizationEncoding::BwAxisScaleOffset:
        toRet.m_encoding = CrossableBwAxisScaleOffset();
        QAIRT_NO_RETURN std::get<CrossableBwAxisScaleOffset>(toRet.m_encoding)
            .setNonOwning(toRet, getBwAxisScaleOffsetEncoding().shallowCopy());
        break;
      case QuantizationEncoding::Block:
        toRet.m_encoding = CrossableBlockEncoding();
        QAIRT_NO_RETURN std::get<CrossableBlockEncoding>(toRet.m_encoding)
            .setNonOwning(toRet, getBlockEncoding().shallowCopy());
        break;
      case QuantizationEncoding::BlockwiseExpansion:
        toRet.m_encoding = CrossableBlockwiseExpansion();
        QAIRT_NO_RETURN std::get<CrossableBlockwiseExpansion>(toRet.m_encoding)
            .setNonOwning(toRet, getBlockwiseExpansion().shallowCopy());
        break;
      case QuantizationEncoding::Vector:
        toRet.m_encoding = CrossableVectorEncoding();
        QAIRT_NO_RETURN std::get<CrossableVectorEncoding>(toRet.m_encoding)
            .setNonOwning(toRet, getVectorEncoding().shallowCopy());
        break;
      case QuantizationEncoding::BwBlockMapped:
        toRet.m_encoding = CrossableBwBlockMapped();
        QAIRT_NO_RETURN std::get<CrossableBwBlockMapped>(toRet.m_encoding)
            .setNonOwning(toRet, getBwBlockMapped().shallowCopy());
        break;
      case QuantizationEncoding::BwBlockwiseExpansionMapped:
        toRet.m_encoding = CrossableBwBlockwiseExpansionMapped();
        QAIRT_NO_RETURN std::get<CrossableBwBlockwiseExpansionMapped>(toRet.m_encoding)
            .setNonOwning(toRet, getBwBlockwiseExpansionMapped().shallowCopy());
        break;

      default:
        toRet.m_encoding = std::monostate();
        break;
    }
    return toRet;
  }

  QuantizeParams& operator=(const QuantizeParams& other) {
    if (this != &other) {
      *this = other.shallowCopy();
    }
    return *this;
  }

  QuantizationEncoding getQuantizationEncoding() const {
    if (std::holds_alternative<CrossableScaleOffset>(m_encoding)) {
      return QuantizationEncoding::ScaleOffset;
    } else if (std::holds_alternative<CrossableAxisScaleOffset>(m_encoding)) {
      return QuantizationEncoding::AxisScaleOffset;
    } else if (std::holds_alternative<CrossableBwScaleOffset>(m_encoding)) {
      return QuantizationEncoding::BwScaleOffset;
    } else if (std::holds_alternative<CrossableBwAxisScaleOffsetMapped>(m_encoding)) {
      return QuantizationEncoding::BwAxisScaleOffsetMapped;
    } else if (std::holds_alternative<CrossableBwAxisScaleOffset>(m_encoding)) {
      return QuantizationEncoding::BwAxisScaleOffset;
    } else if (std::holds_alternative<CrossableMicroscaling>(m_encoding)) {
      return QuantizationEncoding::Microscaling;
    } else if (std::holds_alternative<CrossableBwFloatBlockEncoding>(m_encoding)) {
      return QuantizationEncoding::BwFloatBlock;
    } else if (std::holds_alternative<CrossableFloatBlockEncoding>(m_encoding)) {
      return QuantizationEncoding::FloatBlock;
    } else if (std::holds_alternative<CrossableBwBlockMapped>(m_encoding)) {
      return QuantizationEncoding::BwBlockMapped;
    } else if (std::holds_alternative<CrossableBwBlockwiseExpansionMapped>(m_encoding)) {
      return QuantizationEncoding::BwBlockwiseExpansionMapped;
    } else if (std::holds_alternative<CrossableBlockEncoding>(m_encoding)) {
      return QuantizationEncoding::Block;
    } else if (std::holds_alternative<CrossableBlockwiseExpansion>(m_encoding)) {
      return QuantizationEncoding::BlockwiseExpansion;
    } else if (std::holds_alternative<CrossableVectorEncoding>(m_encoding)) {
      return QuantizationEncoding::Vector;
    } else {
      return QuantizationEncoding::Undefined;
    }
  }

  void setScaleOffsetEncoding(ScaleOffset scaleOffset) {
    QAIRT_NO_RETURN m_encoding.template emplace<CrossableScaleOffset>();
    QAIRT_NO_RETURN std::get<CrossableScaleOffset>(m_encoding).setNonOwning(*this, scaleOffset);
  }

  const ScaleOffset& getScaleOffsetEncoding() const {
    if (std::holds_alternative<std::monostate>(m_encoding)) {
      QAIRT_NO_RETURN m_encoding.template emplace<CrossableScaleOffset>();
    }
    return std::get<CrossableScaleOffset>(m_encoding).bindNonOwning(*this);
  }
  ScaleOffset& getScaleOffsetEncoding() {
    if (std::holds_alternative<std::monostate>(m_encoding)) {
      QAIRT_NO_RETURN m_encoding.template emplace<CrossableScaleOffset>();
    }
    return std::get<CrossableScaleOffset>(m_encoding).bindNonOwning(*this);
  }

  void setAxisScaleOffsetEncoding(AxisScaleOffset axisScaleOffset) {
    QAIRT_NO_RETURN m_encoding.template emplace<CrossableAxisScaleOffset>();
    QAIRT_NO_RETURN std::get<CrossableAxisScaleOffset>(m_encoding)
        .setNonOwning(*this, axisScaleOffset);
  }

  AxisScaleOffset& getAxisScaleOffsetEncoding() {
    return std::get<CrossableAxisScaleOffset>(m_encoding).bindNonOwning(*this);
  }

  const AxisScaleOffset& getAxisScaleOffsetEncoding() const {
    return std::get<CrossableAxisScaleOffset>(m_encoding).bindNonOwning(*this);
  }

  void setBwScaleOffsetEncoding(BwScaleOffset bwScaleOffset) {
    QAIRT_NO_RETURN m_encoding.template emplace<CrossableBwScaleOffset>();
    QAIRT_NO_RETURN std::get<CrossableBwScaleOffset>(m_encoding).setNonOwning(*this, bwScaleOffset);
  }

  BwScaleOffset& getBwScaleOffsetEncoding() {
    return std::get<CrossableBwScaleOffset>(m_encoding).bindNonOwning(*this);
  }

  const BwScaleOffset& getBwScaleOffsetEncoding() const {
    return std::get<CrossableBwScaleOffset>(m_encoding).bindNonOwning(*this);
  }

  void setBwAxisScaleOffsetMappedEncoding(BwAxisScaleOffsetMapped bwAxisScaleOffsetMapped) {
    QAIRT_NO_RETURN m_encoding.template emplace<CrossableBwAxisScaleOffsetMapped>();
    QAIRT_NO_RETURN std::get<CrossableBwAxisScaleOffsetMapped>(m_encoding)
        .setNonOwning(*this, bwAxisScaleOffsetMapped);
  }

  BwAxisScaleOffsetMapped& getBwAxisScaleOffsetMappedEncoding() {
    return std::get<CrossableBwAxisScaleOffsetMapped>(m_encoding).bindNonOwning(*this);
  }

  const BwAxisScaleOffsetMapped& getBwAxisScaleOffsetMappedEncoding() const {
    return std::get<CrossableBwAxisScaleOffsetMapped>(m_encoding).bindNonOwning(*this);
  }

  void setMicroscalingEncoding(Microscaling microscaling) {
    QAIRT_NO_RETURN m_encoding.template emplace<CrossableMicroscaling>();
    QAIRT_NO_RETURN std::get<CrossableMicroscaling>(m_encoding).setNonOwning(*this, microscaling);
  }

  Microscaling& getMicroscalingEncoding() {
    return std::get<CrossableMicroscaling>(m_encoding).bindNonOwning(*this);
  }
  const Microscaling& getMicroscalingEncoding() const {
    return std::get<CrossableMicroscaling>(m_encoding).bindNonOwning(*this);
  }

  void setBwAxisScaleOffsetEncoding(BwAxisScaleOffset bwAxisScaleOffset) {
    QAIRT_NO_RETURN m_encoding.template emplace<CrossableBwAxisScaleOffset>();
    QAIRT_NO_RETURN std::get<CrossableBwAxisScaleOffset>(m_encoding)
        .setNonOwning(*this, bwAxisScaleOffset);
  }

  BwAxisScaleOffset& getBwAxisScaleOffsetEncoding() {
    return std::get<CrossableBwAxisScaleOffset>(m_encoding).bindNonOwning(*this);
  }
  const BwAxisScaleOffset& getBwAxisScaleOffsetEncoding() const {
    return std::get<CrossableBwAxisScaleOffset>(m_encoding).bindNonOwning(*this);
  }

  void setBlockEncoding(BlockEncoding blockEncoding) {
    QAIRT_NO_RETURN m_encoding.template emplace<CrossableBlockEncoding>();
    QAIRT_NO_RETURN std::get<CrossableBlockEncoding>(m_encoding).setNonOwning(*this, blockEncoding);
  }

  BlockEncoding& getBlockEncoding() {
    return std::get<CrossableBlockEncoding>(m_encoding).bindNonOwning(*this);
  }

  const BlockEncoding& getBlockEncoding() const {
    return std::get<CrossableBlockEncoding>(m_encoding).bindNonOwning(*this);
  }

  void setVectorEncoding(VectorEncoding vectorEncoding) {
    QAIRT_NO_RETURN m_encoding.template emplace<CrossableVectorEncoding>();
    QAIRT_NO_RETURN std::get<CrossableVectorEncoding>(m_encoding)
        .setNonOwning(*this, vectorEncoding);
  }

  VectorEncoding& getVectorEncoding() {
    return std::get<CrossableVectorEncoding>(m_encoding).bindNonOwning(*this);
  }

  const VectorEncoding& getVectorEncoding() const {
    return std::get<CrossableVectorEncoding>(m_encoding).bindNonOwning(*this);
  }

  void setBlockwiseExpansion(BlockwiseExpansion blockwiseExpansion) {
    QAIRT_NO_RETURN m_encoding.template emplace<CrossableBlockwiseExpansion>();
    QAIRT_NO_RETURN std::get<CrossableBlockwiseExpansion>(m_encoding)
        .setNonOwning(*this, blockwiseExpansion);
  }

  BlockwiseExpansion& getBlockwiseExpansion() {
    return std::get<CrossableBlockwiseExpansion>(m_encoding).bindNonOwning(*this);
  }
  const BlockwiseExpansion& getBlockwiseExpansion() const {
    return std::get<CrossableBlockwiseExpansion>(m_encoding).bindNonOwning(*this);
  }

  void setBwFloatBlockEncoding(BwFloatBlockEncoding bwFloatBlockEncoding) {
    QAIRT_NO_RETURN m_encoding.template emplace<CrossableBwFloatBlockEncoding>();
    QAIRT_NO_RETURN std::get<CrossableBwFloatBlockEncoding>(m_encoding)
        .setNonOwning(*this, bwFloatBlockEncoding);
  }

  BwFloatBlockEncoding& getBwFloatBlockEncoding() {
    return std::get<CrossableBwFloatBlockEncoding>(m_encoding).bindNonOwning(*this);
  }

  const BwFloatBlockEncoding& getBwFloatBlockEncoding() const {
    return std::get<CrossableBwFloatBlockEncoding>(m_encoding).bindNonOwning(*this);
  }

  void setFloatBlockEncoding(FloatBlockEncoding floatBlockEncoding) {
    QAIRT_NO_RETURN m_encoding.template emplace<CrossableFloatBlockEncoding>();
    QAIRT_NO_RETURN std::get<CrossableFloatBlockEncoding>(m_encoding)
        .setNonOwning(*this, floatBlockEncoding);
  }

  FloatBlockEncoding& getFloatBlockEncoding() {
    return std::get<CrossableFloatBlockEncoding>(m_encoding).bindNonOwning(*this);
  }

  const FloatBlockEncoding& getFloatBlockEncoding() const {
    return std::get<CrossableFloatBlockEncoding>(m_encoding).bindNonOwning(*this);
  }

  void setBwBlockMapped(BwBlockMapped bwBlockMapped) {
    QAIRT_NO_RETURN m_encoding.template emplace<CrossableBwBlockMapped>();
    QAIRT_NO_RETURN std::get<CrossableBwBlockMapped>(m_encoding).setNonOwning(*this, bwBlockMapped);
  }

  const BwBlockMapped& getBwBlockMapped() const {
    return std::get<CrossableBwBlockMapped>(m_encoding).bindNonOwning(*this);
  }

  void setBwBlockwiseExpansionMapped(BwBlockwiseExpansionMapped bwBlockwiseExpansionMapped) {
    QAIRT_NO_RETURN m_encoding.template emplace<CrossableBwBlockwiseExpansionMapped>();
    QAIRT_NO_RETURN std::get<CrossableBwBlockwiseExpansionMapped>(m_encoding)
        .setNonOwning(*this, bwBlockwiseExpansionMapped);
  }

  BwBlockwiseExpansionMapped& getBwBlockwiseExpansionMapped() {
    return std::get<CrossableBwBlockwiseExpansionMapped>(m_encoding).bindNonOwning(*this);
  }

  const BwBlockwiseExpansionMapped& getBwBlockwiseExpansionMapped() const {
    return std::get<CrossableBwBlockwiseExpansionMapped>(m_encoding).bindNonOwning(*this);
  }

 private:
  explicit QuantizeParams(const std::shared_ptr<ApiTable>& apiTable) : ApiType(apiTable) {
    construct();
  }

  void prepareToCross() const {
    if (std::holds_alternative<CrossableScaleOffset>(m_encoding)) {
      std::get<CrossableScaleOffset>(m_encoding).prepareToCross(*this);
    } else if (std::holds_alternative<CrossableAxisScaleOffset>(m_encoding)) {
      std::get<CrossableAxisScaleOffset>(m_encoding).prepareToCross(*this);
    } else if (std::holds_alternative<CrossableBwScaleOffset>(m_encoding)) {
      std::get<CrossableBwScaleOffset>(m_encoding).prepareToCross(*this);
    } else if (std::holds_alternative<CrossableBwAxisScaleOffsetMapped>(m_encoding)) {
      std::get<CrossableBwAxisScaleOffsetMapped>(m_encoding).prepareToCross(*this);
    } else if (std::holds_alternative<CrossableBwAxisScaleOffset>(m_encoding)) {
      std::get<CrossableBwAxisScaleOffset>(m_encoding).prepareToCross(*this);
    } else if (std::holds_alternative<CrossableMicroscaling>(m_encoding)) {
      std::get<CrossableMicroscaling>(m_encoding).prepareToCross(*this);
    } else if (std::holds_alternative<CrossableBlockEncoding>(m_encoding)) {
      std::get<CrossableBlockEncoding>(m_encoding).prepareToCross(*this);
    } else if (std::holds_alternative<CrossableBlockwiseExpansion>(m_encoding)) {
      std::get<CrossableBlockwiseExpansion>(m_encoding).prepareToCross(*this);
    } else if (std::holds_alternative<CrossableVectorEncoding>(m_encoding)) {
      std::get<CrossableVectorEncoding>(m_encoding).prepareToCross(*this);
    } else if (std::holds_alternative<CrossableBwFloatBlockEncoding>(m_encoding)) {
      std::get<CrossableBwFloatBlockEncoding>(m_encoding).prepareToCross(*this);
    } else if (std::holds_alternative<CrossableFloatBlockEncoding>(m_encoding)) {
      std::get<CrossableFloatBlockEncoding>(m_encoding).prepareToCross(*this);
    } else if (std::holds_alternative<CrossableBwBlockMapped>(m_encoding)) {
      std::get<CrossableBwBlockMapped>(m_encoding).prepareToCross(*this);
    } else if (std::holds_alternative<CrossableBwBlockwiseExpansionMapped>(m_encoding)) {
      std::get<CrossableBwBlockwiseExpansionMapped>(m_encoding).prepareToCross(*this);
    } else {
      //
    }
  }

  void updateAfterCross() const {
    QuantizationEncoding qe =
        static_cast<QuantizationEncoding>(callGetter<&interface_type::getQuantEncoding>());
    switch (qe) {
      case QuantizationEncoding::ScaleOffset:
        QAIRT_NO_RETURN m_encoding.template emplace<CrossableScaleOffset>();
        std::get<CrossableScaleOffset>(m_encoding).updateAfterCross(*this);
        break;
      case QuantizationEncoding::AxisScaleOffset:
        QAIRT_NO_RETURN m_encoding.template emplace<CrossableAxisScaleOffset>();
        std::get<CrossableAxisScaleOffset>(m_encoding).updateAfterCross(*this);
        break;
      case QuantizationEncoding::BwScaleOffset:
        QAIRT_NO_RETURN m_encoding.template emplace<CrossableBwScaleOffset>();
        std::get<CrossableBwScaleOffset>(m_encoding).updateAfterCross(*this);
        break;
      case QuantizationEncoding::BwAxisScaleOffsetMapped:
        QAIRT_NO_RETURN m_encoding.template emplace<CrossableBwAxisScaleOffsetMapped>();
        std::get<CrossableBwAxisScaleOffsetMapped>(m_encoding).updateAfterCross(*this);
        break;
      case QuantizationEncoding::Microscaling:
        QAIRT_NO_RETURN m_encoding.template emplace<CrossableMicroscaling>();
        std::get<CrossableMicroscaling>(m_encoding).updateAfterCross(*this);
        break;
      case QuantizationEncoding::BwFloatBlock:
        QAIRT_NO_RETURN m_encoding.template emplace<CrossableBwFloatBlockEncoding>();
        std::get<CrossableBwFloatBlockEncoding>(m_encoding).updateAfterCross(*this);
        break;
      case QuantizationEncoding::FloatBlock:
        QAIRT_NO_RETURN m_encoding.template emplace<CrossableFloatBlockEncoding>();
        std::get<CrossableFloatBlockEncoding>(m_encoding).updateAfterCross(*this);
        break;
      case QuantizationEncoding::BwAxisScaleOffset:
        QAIRT_NO_RETURN m_encoding.template emplace<CrossableBwAxisScaleOffset>();
        std::get<CrossableBwAxisScaleOffset>(m_encoding).updateAfterCross(*this);
        break;
      case QuantizationEncoding::Block:
        QAIRT_NO_RETURN m_encoding.template emplace<CrossableBlockEncoding>();
        std::get<CrossableBlockEncoding>(m_encoding).updateAfterCross(*this);
        break;
      case QuantizationEncoding::BlockwiseExpansion:
        QAIRT_NO_RETURN m_encoding.template emplace<CrossableBlockwiseExpansion>();
        std::get<CrossableBlockwiseExpansion>(m_encoding).updateAfterCross(*this);
        break;
      case QuantizationEncoding::Vector:
        QAIRT_NO_RETURN m_encoding.template emplace<CrossableVectorEncoding>();
        std::get<CrossableVectorEncoding>(m_encoding).updateAfterCross(*this);
        break;
      case QuantizationEncoding::BwBlockMapped:
        QAIRT_NO_RETURN m_encoding.template emplace<CrossableBwBlockMapped>();
        std::get<CrossableBwBlockMapped>(m_encoding).updateAfterCross(*this);
        break;
      case QuantizationEncoding::BwBlockwiseExpansionMapped:
        QAIRT_NO_RETURN m_encoding.template emplace<CrossableBwBlockwiseExpansionMapped>();
        std::get<CrossableBwBlockwiseExpansionMapped>(m_encoding).updateAfterCross(*this);
        break;
      default:
        m_encoding = std::monostate();
        break;
    }
  }
};

/**
 * @brief Descriptor for a single named tensor, including its shape, data type,
 *        memory binding, and quantization parameters.
 *
 * Obtained via Graph operations that return input or output tensors, or constructed
 * directly: Tensor(). Set the shape via setDimensions(), the data type via setDataType(),
 * a memory binding via setTensorMemory() or setClientBuffer(), and quantization parameters
 * via setQuantizeParams().
 */
class Tensor : public ApiType<Tensor, QairtTensor_V1_t> {
  friend Api;
  template <typename, typename, typename>
  friend class ::qairt::ApiType;

  /// @brief Tensor name used to identify this tensor within a graph.
  detail::crossable<std::string, &interface_type::getName, &interface_type::setName> m_name;
  /// @brief Tensor attribute flags (input, output, static, optional, etc.).
  detail::crossable<detail::non_owning<TensorProperties>,
                    &interface_type::getTensorProperties,
                    &interface_type::setTensorProperties>
      m_properties;
  /// @brief Memory descriptor specifying how tensor data is stored or retrieved.
  detail::crossable<detail::non_owning<TensorMemory>,
                    &interface_type::getTensorMemory,
                    &interface_type::setTensorMemory>
      m_memory;
  /// @brief Quantization parameters describing the encoding for this tensor's data.
  detail::crossable<detail::non_owning<QuantizeParams>,
                    &interface_type::getQuantizeParams,
                    &interface_type::setQuantizeParams>
      m_quantParams;

  /// @brief Shape of this tensor as a list of dimension sizes, ordered from outermost to innermost.
  mutable std::vector<uint32_t> m_dims;
  /// @brief Per-dimension dynamic flags; true indicates that dimension is dynamic at runtime.
  mutable std::vector<detail::bool_wrapper> m_isDynamicDims;

 public:
  using ApiType::ApiType;
  using ApiType::operator=;

  ~Tensor(){QAIRT_TRIVIAL} Tensor() = default;
  Tensor(const Tensor& other) : Tensor(other.shallowCopy()) {}
  Tensor(Tensor&& other) noexcept
      : ApiType(static_cast<ApiType&&>(other)),
        m_name(std::move(other.m_name)),
        m_properties(std::move(other.m_properties)),
        m_memory(std::move(other.m_memory)),
        m_quantParams(std::move(other.m_quantParams)),
        m_dims(std::move(other.m_dims)),
        m_isDynamicDims(std::move(other.m_isDynamicDims)) {}
  Tensor& operator=(Tensor&& other) noexcept {
    if (this != &other) {
      std::swap(static_cast<ApiType&>(*this), static_cast<ApiType&>(other));
      std::swap(m_name, other.m_name);
      std::swap(m_properties, other.m_properties);
      std::swap(m_memory, other.m_memory);
      std::swap(m_quantParams, other.m_quantParams);
      std::swap(m_dims, other.m_dims);
      std::swap(m_isDynamicDims, other.m_isDynamicDims);
    }
    return *this;
  }
  Tensor& operator=(const Tensor& other) {
    if (this != &other) {
      *this = other.shallowCopy();
    }
    return *this;
  }

  void setName(std::string str) { m_name = std::move(str); }
  const std::string& getName() const { return m_name.get(); }

  void setTensorProperties(const TensorProperties& tensorProperties) {
    QAIRT_NO_RETURN m_properties.setNonOwning(*this, tensorProperties);
  }
  TensorProperties& getTensorProperties() { return m_properties.bindNonOwning(*this); }
  const TensorProperties& getTensorProperties() const { return m_properties.bindNonOwning(*this); }

  void setDataFormat(uint32_t dataFormat) {
    callSetter<&interface_type::setDataFormat>(dataFormat);
  }
  uint32_t getDataFormat() const { return callGetter<&interface_type::getDataFormat>(); }

  void setDataType(DataType dataType) {
    callSetter<&interface_type::setDataType>(static_cast<Qairt_DataType_t>(dataType));
  }
  DataType getDataType() const {
    return static_cast<DataType>(callGetter<&interface_type::getDataType>());
  }

  QuantizeParams& getQuantizeParams() { return m_quantParams.bindNonOwning(*this); }
  const QuantizeParams& getQuantizeParams() const { return m_quantParams.bindNonOwning(*this); }
  void setQuantizeParams(const QuantizeParams& quantizeParams) {
    QAIRT_NO_RETURN m_quantParams.setNonOwning(*this, quantizeParams);
  }

  uint32_t getRank() const { return static_cast<uint32_t>(m_dims.size()); }

  void setDimensions(const std::vector<uint32_t>& dims) { m_dims = dims; }
  void setDimensions(std::vector<uint32_t>&& dims) { m_dims = std::move(dims); }
  const std::vector<uint32_t>& getDimensions() const { return m_dims; }
  std::vector<uint32_t>& getDimensions() { return m_dims; }

  TensorMemory& getTensorMemory() { return m_memory.bindNonOwning(*this); }
  const TensorMemory& getTensorMemory() const { return m_memory.bindNonOwning(*this); }
  void setTensorMemory(const TensorMemory& tensorMemory) {
    QAIRT_NO_RETURN m_memory.setNonOwning(*this, tensorMemory);
  }

  void setIsDynamicDimensions(const std::vector<bool>& isDynamicDims) {
    m_isDynamicDims.assign(isDynamicDims.begin(), isDynamicDims.end());
  }
  void setIsDynamicDimensions(const std::vector<detail::bool_wrapper>& isDynamicDims) {
    m_isDynamicDims.assign(isDynamicDims.begin(), isDynamicDims.end());
  }
  const std::vector<detail::bool_wrapper>& getIsDynamicDimensions() const {
    return m_isDynamicDims;
  }
  std::vector<detail::bool_wrapper>& getIsDynamicDimensions() { return m_isDynamicDims; }

  ClientBuffer& getClientBuffer() { return getTensorMemory().getClientBuffer(); }

  const ClientBuffer& getClientBuffer() const { return getTensorMemory().getClientBuffer(); }

  void setClientBuffer(const ClientBuffer& clientBuffer) {
    getTensorMemory().setClientBuffer(clientBuffer);
  }

  void setClientBuffer(RawBuffer&& buffer) { getTensorMemory().setClientBuffer(std::move(buffer)); }

  void setId(uint64_t id) { callSetter<&interface_type::setId>(id); }
  uint64_t getId() const { return callGetter<&interface_type::getId>(); }

  bool getIsProduced() const {
    return detail::resolveBool(callGetter<&interface_type::getIsProduced>());
  }

  Tensor shallowCopy() const {
    Tensor toret(copy_table_tag, *this);
    toret.setId(getId());
    toret.setName(getName());
    toret.setTensorProperties(getTensorProperties());
    toret.setDataType(getDataType());
    toret.setDataFormat(getDataFormat());
    toret.setTensorMemory(getTensorMemory());
    toret.setQuantizeParams(getQuantizeParams());
    toret.setDimensions(getDimensions());
    toret.setIsDynamicDimensions(getIsDynamicDimensions());
    return toret;
  }

 private:
  Tensor(copy_table_tag_t, const Tensor& other) : ApiType(copy_table_tag, other) { construct(); }

  explicit Tensor(const std::shared_ptr<ApiTable>& apiTable) : ApiType(apiTable) { construct(); }

  void prepareToCross() const {
    prepareMultipleToCross(m_name, m_properties, m_quantParams, m_memory);

    if (m_dims.empty()) {
      callSetter<&interface_type::setDims>(nullptr, static_cast<uint32_t>(0));
    } else {
      callSetter<&interface_type::setDims>(const_cast<uint32_t*>(m_dims.data()),
                                           static_cast<uint32_t>(m_dims.size()));
    }

    if (m_isDynamicDims.empty()) {
      callSetter<&interface_type::setIsDynamicDimensions>(nullptr, static_cast<uint32_t>(0));
    } else {
      callSetter<&interface_type::setIsDynamicDimensions>(
          reinterpret_cast<uint8_t*>(m_isDynamicDims.data()),
          static_cast<uint32_t>(m_isDynamicDims.size()));
    }
  }

  void updateAfterCross() const {
    updateMultipleAfterCross(m_name, m_properties, m_quantParams, m_memory);

    uint32_t rank = callGetter<&interface_type::getRank>();
    m_dims.resize(rank);
    if (rank > 0) {
      callAndConfirm<&interface_type::getDims>(m_dims.data());
      std::vector<uint8_t> temp(rank);
      callAndConfirm<&interface_type::getIsDynamicDimensions>(temp.data());
      if (temp.end() != std::find_if(temp.begin(), temp.end(), [](auto& e) { return e != 0u; })) {
        m_isDynamicDims.resize(rank);
        QAIRT_NO_RETURN std::transform(
            temp.begin(), temp.end(), m_isDynamicDims.begin(), [](auto& e) { return e != 0u; });
      }
    }
  }
};

}  // namespace qairt

// Explicit instantiation to work around a GCC 8.x bug causing linker errors when creating a
// std::vector<std::shared_ptr<qairt::Tensor>> with initializer-list syntax.
#if defined(__GNUC__) && (__GNUC__ < 9)
template class std::allocator<std::shared_ptr<qairt::Tensor>>;
#endif

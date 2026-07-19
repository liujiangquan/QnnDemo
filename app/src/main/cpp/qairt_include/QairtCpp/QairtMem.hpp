//==============================================================================
//
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// All rights reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//==============================================================================

#pragma once

/**
 * @file
 * @brief C++ wrappers for the QAIRT memory descriptor and shape APIs.
 *
 *        Provides type-safe access to memory descriptors and shapes used to
 *        describe tensor memory for QAIRT operations.
 *
 */

#include <vector>

#include "QairtApi.hpp"
#include "QairtMem/QairtMemCommon.h"
#include "QairtMem/QairtMemDescriptor.h"
#include "QairtMem/QairtMemShape.h"


namespace qairt {

/**
 * @brief Error codes returned by QAIRT memory operations.
 *
 * @details
 * | Enumerator          | Description                                          |
 * |---------------------|------------------------------------------------------|
 * | `NoError`           | Operation succeeded.                                 |
 * | `NotSupported`      | The requested functionality is not supported by the backend. |
 * | `InvalidArgument`   | An argument to the operation was invalid.            |
 * | `InvalidHandle`     | The provided handle is not valid.                    |
 * | `AlreadyRegistered` | The provided memory has already been registered.     |
 * | `Mapping`           | An error occurred during memory mapping.             |
 * | `InvalidShape`      | The memory shape is invalid for this backend.        |
 * | `UnsupportedMemtype`| The requested memory type is not supported by the backend. |
 * | `Undefined`         | An undefined or unknown error occurred.              |
 */
QAIRT_CPPAPI_DEFINE_ENUM(MemError,
                         QairtMem_Error_t,
                         NoError            = QAIRT_MEM_NO_ERROR,
                         NotSupported       = QAIRT_MEM_ERROR_NOT_SUPPORTED,
                         InvalidArgument    = QAIRT_MEM_ERROR_INVALID_ARGUMENT,
                         InvalidHandle      = QAIRT_MEM_ERROR_INVALID_HANDLE,
                         AlreadyRegistered  = QAIRT_MEM_ERROR_ALREADY_REGISTERED,
                         Mapping            = QAIRT_MEM_ERROR_MAPPING,
                         InvalidShape       = QAIRT_MEM_ERROR_INVALID_SHAPE,
                         UnsupportedMemtype = QAIRT_MEM_ERROR_UNSUPPORTED_MEMTYPE,
                         Undefined          = QAIRT_MEM_ERROR_UNDEFINED)

/**
 * @brief Memory types used to provide data for a QAIRT tensor.
 *
 * @details
 * | Enumerator    | Description                                         |
 * |---------------|-----------------------------------------------------|
 * | `Unsupported` | Memory type is not supported.                       |
 * | `Custom`      | Memory allocated by a custom backend mechanism.     |
 * | `DmaBuf`      | Memory allocated via the DMA-BUF subsystem.         |
 * | `Undefined`   | Memory type is undefined.                           |
 */
QAIRT_CPPAPI_DEFINE_ENUM(MemType,
                         Qairt_MemType_t,
                         Unsupported = QAIRT_UNSUPPORTED,
                         Custom      = QAIRT_MEM_TYPE_CUSTOM,
                         DmaBuf      = QAIRT_MEM_TYPE_DMA_BUF,
                         Undefined   = QAIRT_MEM_TYPE_UNDEFINED)

/**
 * @brief Wrapper for a QAIRT memory shape handle.
 */
class MemShape : public ApiType<MemShape, QairtMem_ShapeV1_t> {
  friend class Api;

  /// @brief Backend-specific shape configuration string.
  detail::crossable<std::string, &interface_type::getShapeConfig, &interface_type::setShapeConfig>
      m_shapeConfig;

  /// @brief Cached dimension values for this shape.
  mutable std::vector<uint32_t> m_dimensions;

 public:
  using ApiType::ApiType;
  using ApiType::operator=;

  MemShape() noexcept                      = default;
  MemShape(const MemShape&)                = delete;
  MemShape(MemShape&&) noexcept            = default;
  MemShape& operator=(const MemShape&)     = delete;
  MemShape& operator=(MemShape&&) noexcept = default;

  /**
   * @brief Create a shallow copy of this shape with the same rank, dimensions, and config.
   *
   * @return A new @ref MemShape with the same rank, dimensions, and shape config.
   *
   * @throws qairt::Exception on invalid handle.
   */
  MemShape shallowCopy() const {
    MemShape toret;

    toret.setDimensions(getDimensions());
    toret.setShapeConfig(getShapeConfig());

    return toret;
  }

  /**
   * @brief Get the backend-specific shape configuration string.
   *
   * @return Reference to the shape config string. Empty if none was set.
   *
   * @see QairtMem_Shape_getShapeConfig
   */
  std::string& getShapeConfig() { return m_shapeConfig.get(); }

  /**
   * @brief Get the backend-specific shape configuration string.
   *
   * @return Const reference to the shape config string. Empty if none was set.
   *
   * @see QairtMem_Shape_getShapeConfig
   */
  const std::string& getShapeConfig() const { return m_shapeConfig.get(); }

  /**
   * @brief Set the backend-specific shape configuration string.
   *
   * Refer to the backend documentation for valid values. NULL is allowed
   * in the underlying C API; pass an empty string to clear.
   *
   * @param[in] shapeConfig  Backend-specific configuration string for this shape.
   *
   * @see QairtMem_Shape_setShapeConfig
   */
  void setShapeConfig(std::string shapeConfig) { m_shapeConfig = std::move(shapeConfig); }

  /**
   * @brief Get the number of dimensions (rank) of this memory shape.
   *
   * @return Number of dimensions in this shape.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtMem_Shape_getRank
   */
  uint32_t getRank() const { return callGetter<&interface_type::getRank>(); }

  /**
   * @brief Get the size of a single dimension by index.
   *
   * @param[in] index  Zero-based dimension index. Must be less than getRank().
   *
   * @return Size of the dimension at _index_.
   *
   * @throws qairt::Exception on invalid handle or out-of-range index.
   *
   * @see QairtMem_Shape_getDimensionAt
   */
  uint32_t getDimensionAt(uint32_t index) const {
    uint32_t toret{};
    callAndConfirm<&interface_type::getDimensionAt>(index, &toret);
    return toret;
  }

  /**
   * @brief Get the full ordered list of dimension sizes for this shape.
   *
   * @return Reference to a vector of dimension sizes in order.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtMem_Shape_getDimensionAt
   */
  std::vector<uint32_t>& getDimensions() {
    const uint32_t rank = getRank();
    m_dimensions.resize(rank);
    for (uint32_t i = 0; i < rank; ++i) {
      callAndConfirm<&interface_type::getDimensionAt>(i, &(m_dimensions[i]));
    }
    return m_dimensions;
  }

  /**
   * @brief Get the full ordered list of dimension sizes for this shape.
   *
   * @return Const reference to a vector of dimension sizes in order.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtMem_Shape_getDimensionAt
   */
  const std::vector<uint32_t>& getDimensions() const {
    const uint32_t rank = getRank();
    m_dimensions.resize(rank);
    for (uint32_t i = 0; i < rank; ++i) {
      callAndConfirm<&interface_type::getDimensionAt>(i, &(m_dimensions[i]));
    }
    return m_dimensions;
  }

  /**
   * @brief Set the ordered list of dimension sizes for this shape.
   *
   * @param[in] dimensions  Ordered dimension sizes. The number of elements determines the rank.
   *
   * @see QairtMem_Shape_setDimensions
   */
  void setDimensions(std::vector<uint32_t> dimensions) { m_dimensions = std::move(dimensions); }

 private:
  void prepareToCross() const {
    prepareMultipleToCross(m_shapeConfig);

    callSetter<&interface_type::setDimensions>(m_dimensions.data(), m_dimensions.size());
  }
  void updateAfterCross() const {
    updateMultipleAfterCross(m_shapeConfig);
  }
};

/**
 * @brief Wrapper for a QAIRT memory descriptor handle.
 */
class MemDescriptor : public ApiType<MemDescriptor, QairtMem_DescriptorV1_t> {
  friend class Api;

  /// @brief Shape associated with this memory descriptor.
  detail::crossable<detail::non_owning<MemShape>,
                    &interface_type::getMemShape,
                    &interface_type::setMemShape>
      m_memShape;

 public:
  using ApiType::ApiType;
  using ApiType::operator=;

  MemDescriptor() noexcept                           = default;
  MemDescriptor(const MemDescriptor&)                = delete;
  MemDescriptor(MemDescriptor&&) noexcept            = default;
  MemDescriptor& operator=(const MemDescriptor&)     = delete;
  MemDescriptor& operator=(MemDescriptor&&) noexcept = default;

  /**
   * @brief Set the Direct Memory Access buffer (DMA-BUF) file descriptor and data pointer.
   *
   * @param[in] dmaBufFd    DMA-BUF file descriptor. Must be a valid open file
   *                        descriptor, or @c QAIRT_MEM_INVALID_FD.
   * @param[in] dmaBufData  Pointer to the memory-mapped region produced by @c mmap
   *                        on _dmaBufFd_. NULL is allowed when a DMA-BUF is not used.
   *
   * @throws qairt::Exception on invalid handle or invalid argument.
   *
   * @see QairtMem_Descriptor_setDmaBuf
   */
  void setDmaBuf(int32_t dmaBufFd, void* dmaBufData) {
    callSetter<&interface_type::setDmaBuf>(dmaBufFd, dmaBufData);
  }

  /**
   * @brief Get the Direct Memory Access buffer (DMA-BUF) file descriptor and data pointer.
   *
   * @return Pair of (file descriptor, data pointer) previously set via setDmaBuf().
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtMem_Descriptor_getDmaBuf
   */
  std::pair<int32_t, void*> getDmaBuf() const {
    std::pair<int32_t, void*> toret;
    callAndConfirm<&interface_type::getDmaBuf>(&std::get<0>(toret), &std::get<1>(toret));
    return toret;
  }

  /**
   * @brief Set the memory shape for this descriptor.
   *
   * @param[in] memShape  Memory shape describing the rank and per-dimension
   *                      sizes of the buffer.
   *
   * @throws qairt::Exception on invalid handle or invalid argument.
   *
   * @see QairtMem_Descriptor_setMemShape
   */
  void setMemShape(const MemShape& memShape) {
    QAIRT_NO_RETURN m_memShape.setNonOwning(*this, memShape);
  }

  /**
   * @brief Get the memory shape associated with this descriptor.
   *
   * @return Reference to the memory shape for this descriptor.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtMem_Descriptor_getMemShape
   */
  MemShape& getMemShape() { return m_memShape.bindNonOwning(*this); }

  /// @see MemDescriptor::getMemShape()
  const MemShape& getMemShape() const { return m_memShape.bindNonOwning(*this); }

 private:
  void prepareToCross() const { prepareMultipleToCross(m_memShape); }
  void updateAfterCross() const { updateMultipleAfterCross(m_memShape); }
};

}  // namespace qairt

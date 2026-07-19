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
 * @brief  C++ buffer types for passing data to and from the QAIRT API.
 */

#include <cstdint>
#include <functional>
#include <limits>
#include <memory>
#include <type_traits>
#include <vector>

namespace qairt {

class Buffer;

/**
 * @brief Non-owning typed view over a contiguous range of elements.
 *
 * @tparam T  Element type. Must not be a reference, pointer, const, or volatile type.
 */
template <typename T>
class BufferView {
  static_assert(!std::is_reference_v<T>);
  static_assert(!std::is_pointer_v<T>);
  static_assert(!std::is_const_v<T>);
  static_assert(!std::is_volatile_v<T>);

 public:
  using value_type             = T;
  using size_type              = std::size_t;
  using difference_type        = std::ptrdiff_t;
  using reference              = value_type&;
  using const_reference        = const value_type&;
  using pointer                = value_type*;
  using const_pointer          = const value_type*;
  using iterator               = pointer;
  using const_iterator         = const_pointer;
  using reverse_iterator       = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

 private:
  friend class Buffer;

  /// @brief Non-owning pointer to the first element of the viewed memory range.
  pointer m_data{};
  /// @brief Number of elements of type T in the viewed range.
  size_t m_size{};

  constexpr BufferView(pointer data, size_t size) noexcept : m_data{data}, m_size{size} {}

 public:
  constexpr BufferView() noexcept = default;

  reference at(size_type pos) {
    if (pos >= m_size || !m_data) {
      throw std::out_of_range("BufferView access out of range");
    }
    return m_data[pos];
  }

  const_reference at(size_type pos) const {
    if (pos >= m_size || !m_data) {
      throw std::out_of_range("BufferView access out of range");
    }
    return m_data[pos];
  }

  reference operator[](size_type pos) { return m_data[pos]; }
  const_reference operator[](size_type pos) const { return m_data[pos]; }

  reference front() { return m_data[0]; }
  const_reference front() const { return m_data[0]; }

  reference back() { return m_data[m_size - 1]; }
  const_reference back() const { return m_data[m_size - 1]; }

  T* data() { return m_data; }
  const T* data() const { return m_data; }

  iterator begin() { return m_data; }
  const_iterator begin() const { return m_data; }
  const_iterator cbegin() const noexcept { return m_data; }

  iterator end() { return m_data + m_size; }
  const_iterator end() const { return m_data + m_size; }
  const_iterator cend() const noexcept { return m_data + m_size; }

  reverse_iterator rbegin() { return reverse_iterator(end()); }
  const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
  const_reverse_iterator rcbegin() const noexcept { return const_reverse_iterator(cend()); }

  reverse_iterator rend() { return reverse_iterator(begin()); }
  const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }
  const_reverse_iterator rcend() const noexcept { return const_reverse_iterator(begin()); }

  size_type size() const { return m_size; }
  bool empty() const { return m_size == 0 || !m_data; }
};

/**
 * @brief Abstract base class representing a contiguous block of byte data.
 *
 * Use one of the concrete subclasses — @ref VectorBuffer, @ref RawBuffer,
 * or @ref OwnedBuffer — to hold tensor data when calling QAIRT APIs.
 * A typed view of the underlying bytes can be obtained via view<T>().
 */
class Buffer {
 protected:
  Buffer()                             = default;
  Buffer(const Buffer&)                = default;
  Buffer(Buffer&&) noexcept            = default;
  Buffer& operator=(const Buffer&)     = default;
  Buffer& operator=(Buffer&&) noexcept = default;

 public:
  virtual ~Buffer() = default;

  /**
   * @brief Get a pointer to the start of the buffer's byte data.
   *
   * @return Pointer to the first byte, or nullptr if the buffer is empty.
   */
  virtual void* getData()             = 0;

  /**
   * @brief Get a pointer to the start of the buffer's byte data.
   *
   * @return Const pointer to the first byte, or nullptr if the buffer is empty.
   */
  virtual const void* getData() const = 0;

  /**
   * @brief Get the total number of bytes in this buffer.
   *
   * @return Byte count.
   */
  // Number of bytes
  virtual uint64_t getDataSize() const = 0;

  /**
   * @brief Get a typed view over the buffer's contents.
   *
   * @tparam T  Element type for the view. Must not be a reference, pointer, const, or
   *            volatile type.
   *
   * @return A @ref BufferView<T> spanning the entire buffer.
   *
   * @throws std::runtime_error if the byte size is not a multiple of sizeof(T).
   */
  template <typename T>
  BufferView<T> view() {
    const auto size = getDataSize();
    if (size % sizeof(T) != 0)
      throw std::runtime_error("Attempting to view a buffer with a size that mismatches the type");
    return BufferView<T>{static_cast<T*>(getData()), static_cast<size_t>(size) / sizeof(T)};
  }
};

/**
 * @brief Buffer adapter that owns data as a std::vector.
 *
 * @tparam T_VectorParams  Template parameters forwarded to std::vector.
 */
template <typename... T_VectorParams>  // allow custom allocators
struct VectorBuffer : public Buffer {
 public:
  using vector_type            = std::vector<T_VectorParams...>;
  using value_type             = typename vector_type::value_type;
  using allocator_type         = typename vector_type::allocator_type;
  using size_type              = typename vector_type::size_type;
  using difference_type        = typename vector_type::difference_type;
  using reference              = typename vector_type::reference;
  using const_reference        = typename vector_type::const_reference;
  using pointer                = typename vector_type::pointer;
  using const_pointer          = typename vector_type::const_pointer;
  using iterator               = typename vector_type::iterator;
  using const_iterator         = typename vector_type::const_iterator;
  using reverse_iterator       = typename vector_type::reverse_iterator;
  using const_reverse_iterator = typename vector_type::const_reverse_iterator;

 private:
  /// @brief Underlying vector that owns the buffer's data.
  vector_type m_vector;

 public:
  VectorBuffer()                                   = default;
  VectorBuffer(const VectorBuffer&)                = delete;
  VectorBuffer& operator=(const VectorBuffer&)     = delete;
  VectorBuffer(VectorBuffer&&) noexcept            = default;
  VectorBuffer& operator=(VectorBuffer&&) noexcept = default;

  explicit VectorBuffer(vector_type&& vector) : m_vector{std::move(vector)} {}
  explicit VectorBuffer(const vector_type& vector) : m_vector{vector} {}

  /**
   * @brief Get a pointer to the start of the buffer's byte data.
   *
   * @return Pointer to the first byte, or nullptr if the vector is empty.
   */
  void* getData() override { return m_vector.empty() ? nullptr : m_vector.data(); }

  /**
   * @brief Get a pointer to the start of the buffer's byte data.
   *
   * @return Const pointer to the first byte, or nullptr if the vector is empty.
   */
  const void* getData() const override { return m_vector.empty() ? nullptr : m_vector.data(); }

  /**
   * @brief Get the total number of bytes in this buffer.
   *
   * @return Byte count: number of elements multiplied by sizeof(value_type).
   *
   * @throws std::runtime_error if the resulting byte count would overflow uint64_t.
   */
  uint64_t getDataSize() const override {
    if (m_vector.size() > std::numeric_limits<uint64_t>::max() / sizeof(value_type)) {
      throw std::runtime_error("Vector too large");
    }
    return m_vector.size() * sizeof(value_type);
  }

  VectorBuffer& operator=(const vector_type& other) {
    m_vector = other;
    return *this;
  }

  VectorBuffer& operator=(vector_type&& other) noexcept {
    m_vector = std::move(other);
    return *this;
  }

  template <typename... T_Args>
  void assign(T_Args&&... args) {
    m_vector.assign(std::forward<T_Args>(args)...);
  }

  allocator_type get_allocator() const { return m_vector.get_allocator(); }

  reference at(size_type pos) { return m_vector.at(pos); }
  const_reference at(size_type pos) const { return m_vector.at(pos); }

  reference operator[](size_type pos) { return m_vector.operator[](pos); }
  const_reference operator[](size_type pos) const { return m_vector.operator[](pos); }

  reference front() { return m_vector.front(); }
  const_reference front() const { return m_vector.front(); }

  reference back() { return m_vector.back(); }
  const_reference back() const { return m_vector.back(); }

  pointer data() noexcept { return m_vector.data(); }
  const_pointer data() const noexcept { return m_vector.data(); }

  iterator begin() noexcept { return m_vector.begin(); }
  const_iterator begin() const noexcept { return m_vector.begin(); }
  const_iterator cbegin() const noexcept { return m_vector.cbegin(); }

  iterator end() noexcept { return m_vector.end(); }
  const_iterator end() const noexcept { return m_vector.end(); }
  const_iterator cend() const noexcept { return m_vector.cend(); }

  reverse_iterator rbegin() noexcept { return m_vector.rbegin(); }
  const_reverse_iterator rbegin() const noexcept { return m_vector.rbegin(); }
  const_reverse_iterator crbegin() const noexcept { return m_vector.crbegin(); }

  reverse_iterator rend() noexcept { return m_vector.rend(); }
  const_reverse_iterator rend() const noexcept { return m_vector.rend(); }
  const_reverse_iterator crend() const noexcept { return m_vector.crend(); }

  bool empty() const noexcept { return m_vector.empty(); }
  size_type size() const noexcept { return m_vector.size(); }
  size_type max_size() const noexcept { return m_vector.max_size(); }

  void reserve(size_type newCap) { m_vector.reserve(newCap); }

  size_type capacity() const noexcept { return m_vector.capacity(); }
  void shrink_to_fit() { m_vector.shrink_to_fit(); }

  void clear() { m_vector.clear(); }

  template <typename... T_Args>
  iterator insert(T_Args&&... args) {
    return m_vector.insert(std::forward<T_Args>(args)...);
  }

  template <typename... T_Args>
  iterator emplace(T_Args&&... args) {
    return m_vector.emplace(std::forward<T_Args>(args)...);
  }

  template <typename... T_Args>
  iterator erase(T_Args&&... args) {
    return m_vector.erase(std::forward<T_Args>(args)...);
  }

  template <typename T_Arg>
  void push_back(T_Arg&& arg) {
    m_vector.push_back(std::forward<T_Arg>(arg));
  }

  template <typename... T_Args>
  reference emplace_back(T_Args&&... args) {
    return m_vector.emplace_back(std::forward<T_Args>(args)...);
  }

  void pop_back() { m_vector.pop_back(); }

  template <typename... T_Args>
  void resize(T_Args&&... args) {
    m_vector.resize(std::forward<T_Args>(args)...);
  }

  void swap(VectorBuffer& other) noexcept { m_vector.swap(other.m_vector); }
  void swap(vector_type& otherVector) noexcept { m_vector.swap(otherVector); }

  friend bool operator==(const VectorBuffer& lhs, const VectorBuffer& rhs) {
    return lhs.m_vector == rhs.m_vector;
  }
  friend bool operator!=(const VectorBuffer& lhs, const VectorBuffer& rhs) {
    return lhs.m_vector != rhs.m_vector;
  }
  friend bool operator<(const VectorBuffer& lhs, const VectorBuffer& rhs) {
    return lhs.m_vector < rhs.m_vector;
  }
  friend bool operator<=(const VectorBuffer& lhs, const VectorBuffer& rhs) {
    return lhs.m_vector <= rhs.m_vector;
  }
  friend bool operator>(const VectorBuffer& lhs, const VectorBuffer& rhs) {
    return lhs.m_vector > rhs.m_vector;
  }
  friend bool operator>=(const VectorBuffer& lhs, const VectorBuffer& rhs) {
    return lhs.m_vector >= rhs.m_vector;
  }
};

template <typename... Ts>
std::shared_ptr<VectorBuffer<Ts...>> makeBuffer(std::vector<Ts...> vec) {
  return std::make_shared<VectorBuffer<Ts...>>(std::move(vec));
}
template <typename T>
std::shared_ptr<VectorBuffer<T>> makeBuffer(std::vector<T> vec) {
  return std::make_shared<VectorBuffer<T>>(std::move(vec));
}

/**
 * @brief Non-owning Buffer wrapper around a raw pointer and byte size.
 */
class RawBuffer : public Buffer {
  /// @brief Non-owning pointer to the buffer data.
  void* m_data{};
  /// @brief Number of valid bytes beginning at m_data.
  uint64_t m_dataSize{};

 public:
  RawBuffer() = default;

  RawBuffer(const RawBuffer&)                = default;
  RawBuffer& operator=(const RawBuffer&)     = default;
  RawBuffer(RawBuffer&&) noexcept            = default;
  RawBuffer& operator=(RawBuffer&&) noexcept = default;

  RawBuffer(void* data, uint64_t dataSize) : m_data{data}, m_dataSize{dataSize} {}

  void* getData() override { return m_data; }
  const void* getData() const override { return m_data; }
  uint64_t getDataSize() const override { return m_dataSize; }

  void setData(void* data) noexcept { m_data = data; }
  void setDataSize(uint64_t dataSize) noexcept { m_dataSize = dataSize; }
};

/**
 * @brief Buffer that owns heap memory and releases it via a caller-supplied deleter.
 */
class OwnedBuffer : public Buffer {
  /// @brief Owning pointer to the buffer data; the stored deleter frees it on destruction.
  std::unique_ptr<void, std::function<void(void*)>> m_data;
  /// @brief Number of valid bytes beginning at the address held by m_data.
  uint64_t m_dataSize;

 public:
  OwnedBuffer()                                  = default;
  OwnedBuffer(const OwnedBuffer&)                = delete;
  OwnedBuffer& operator=(const OwnedBuffer&)     = delete;
  OwnedBuffer(OwnedBuffer&&) noexcept            = default;
  OwnedBuffer& operator=(OwnedBuffer&&) noexcept = default;

  OwnedBuffer(void* data, std::function<void(void*)> freer, uint64_t dataSize)
      : m_data(data, std::move(freer)), m_dataSize{dataSize} {}
  
  template <typename T_Type, typename T_Deleter>
  OwnedBuffer(std::unique_ptr<T_Type, T_Deleter> ptr, uint64_t dataSize)
      : m_data(ptr.release(), std::function<void(void*)>{ptr.get_deleter()}),
        m_dataSize{dataSize} {}

  void* getData() override { return m_data.get(); }
  const void* getData() const override { return m_data.get(); }
  uint64_t getDataSize() const override { return m_dataSize; }

  void setData(void* data, std::function<void(void*)> freer) noexcept {
    decltype(m_data)(data, freer).swap(m_data);
  }
  void setDataSize(uint64_t dataSize) noexcept { m_dataSize = dataSize; }

  RawBuffer asNonOwningBuffer() { return {m_data.get(), m_dataSize}; }
};

}  // namespace qairt

//==============================================================================
//
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// All rights reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//==============================================================================

/**
 * @file
 * @brief C++ wrapper for the QAIRT OpConfig, Param, and Scalar APIs.
 */

#pragma once

#include "QairtApi.hpp"
#include "QairtOpConfig/QairtOpConfig.h"
#include "QairtTensor.hpp"

namespace qairt {

/**
 * @brief Error codes returned by QAIRT OpConfig operations.
 *
 * @details
 * | Enumerator       | Description                                       |
 * |------------------|---------------------------------------------------|
 * | `NoError`        | Operation succeeded.                              |
 * | `MemAlloc`       | Memory allocation failure.                        |
 * | `InvalidArgument` | An argument to the operation was invalid.        |
 * | `InvalidHandle`  | The provided handle is not valid.                 |
 * | `Undefined`      | An undefined or unknown error occurred.           |
 */
QAIRT_CPPAPI_DEFINE_ENUM(OpConfigError,
                         QairtOpConfig_Error_t,
                         NoError         = QAIRT_OPCONFIG_NO_ERROR,
                         MemAlloc        = QAIRT_OPCONFIG_ERROR_MEM_ALLOC,
                         InvalidArgument = QAIRT_OPCONFIG_ERROR_INVALID_ARGUMENT,
                         InvalidHandle   = QAIRT_OPCONFIG_ERROR_INVALID_HANDLE,
                         Undefined       = QAIRT_OPCONFIG_ERROR_UNDEFINED)

/**
 * @brief Parameter type discriminator for QAIRT op parameters.
 *
 * @details
 * | Enumerator  | Description                              |
 * |-------------|------------------------------------------|
 * | `Scalar`    | Parameter holds a scalar value.          |
 * | `Tensor`    | Parameter holds a tensor handle.         |
 * | `Undefined` | Parameter type is unset or unrecognized. |
 */
QAIRT_CPPAPI_DEFINE_ENUM(ParamType,
                         Qairt_ParamType_t,
                         Scalar    = QAIRT_PARAMTYPE_SCALAR,
                         Tensor    = QAIRT_PARAMTYPE_TENSOR,
                         Undefined = QAIRT_PARAMTYPE_UNDEFINED)

/**
 * @brief Error codes returned by QAIRT Tensor operations.
 *
 * @details
 * | Enumerator                | Description                                                          |
 * |---------------------------|----------------------------------------------------------------------|
 * | `NoError`                 | Operation succeeded.                                                 |
 * | `InvalidHandle`           | The provided handle is not valid.                                    |
 * | `DoesNotExist`            | Tensor with the specified credentials is not registered.             |
 * | `InvalidTensorParam`      | A tensor parameter value is invalid.                                 |
 * | `UnsupportedTensorParam`  | A tensor parameter is not supported by this implementation.          |
 * | `IncompatibleTensorUpdate` | The tensor provided for update is incompatible with the original.   |
 * | `InvalidArgument`         | An argument to the operation was invalid.                            |
 * | `UnsupportedFeature`      | The requested feature is not yet supported.                          |
 * | `Undefined`               | An undefined or unknown error occurred.                              |
 */
QAIRT_CPPAPI_DEFINE_ENUM(TensorError,
                         QairtTensor_Error_t,
                         NoError                  = QAIRT_TENSOR_NO_ERROR,
                         InvalidHandle            = QAIRT_TENSOR_ERROR_INVALID_HANDLE,
                         DoesNotExist             = QAIRT_TENSOR_ERROR_DOES_NOT_EXIST,
                         InvalidTensorParam       = QAIRT_TENSOR_ERROR_INVALID_TENSOR_PARAM,
                         UnsupportedTensorParam   = QAIRT_TENSOR_ERROR_UNSUPPORTED_TENSOR_PARAM,
                         IncompatibleTensorUpdate = QAIRT_TENSOR_ERROR_INCOMPATIBLE_TENSOR_UPDATE,
                         InvalidArgument          = QAIRT_TENSOR_ERROR_INVALID_ARGUMENT,
                         UnsupportedFeature       = QAIRT_TENSOR_ERROR_UNSUPPORTED_FEATURE,
                         Undefined                = QAIRT_TENSOR_ERROR_UNDEFINED)

/**
 * @brief Wrapper for a QAIRT scalar value of a single primitive or string type.
 *
 *        Obtained via Api::make<Scalar>().
 */
class Scalar : public ApiType<Scalar, QairtScalar_V1_t> {
  friend class Api;

 public:
  using ApiType::ApiType;

  ~Scalar(){QAIRT_TRIVIAL} Scalar() noexcept = default;
  Scalar(Scalar&& other) noexcept : ApiType(static_cast<ApiType&&>(other)) {}
  Scalar& operator=(Scalar&& other) noexcept {
    if (this != &other) {
      std::swap(static_cast<ApiType&>(*this), static_cast<ApiType&>(other));
    }
    return *this;
  }

  /**
   * @brief Construct a deep copy of another Scalar.
   *
   * @param[in] other  Source scalar to copy. Must hold a valid data type.
   *
   * @throws std::runtime_error if _other_ holds an unrecognized data type.
   */
  Scalar(const Scalar& other) : ApiType(copy_table_tag, other) { copy(other, *this); }

  /**
   * @brief Assign a deep copy of another Scalar to this instance.
   *
   * @param[in] other  Source scalar to copy. Must hold a valid data type.
   *
   * @return Reference to this Scalar after assignment.
   *
   * @throws std::runtime_error if _other_ holds an unrecognized data type.
   */
  Scalar& operator=(const Scalar& other) {
    if (this != &other) {
      copy(other, *this);
    }
    return *this;
  }

 private:
  static void copy(const Scalar& src, Scalar& dst) {
    switch (src.getDataType()) {
      case DataType::Int8:
        dst.setInt8Value(src.getInt8Value());
        break;
      case DataType::Int16:
        dst.setInt16Value(src.getInt16Value());
        break;
      case DataType::Int32:
        dst.setInt32Value(src.getInt32Value());
        break;
      case DataType::Int64:
        dst.setInt64Value(src.getInt64Value());
        break;
      case DataType::UInt8:
        dst.setUint8Value(src.getUint8Value());
        break;
      case DataType::UInt16:
        dst.setUint16Value(src.getUint16Value());
        break;
      case DataType::UInt32:
        dst.setUint32Value(src.getUint32Value());
        break;
      case DataType::UInt64:
        dst.setUint64Value(src.getUint64Value());
        break;
      case DataType::Float32:
        dst.setFloatValue(src.getFloatValue());
        break;
      case DataType::Float64:
        dst.setDoubleValue(src.getDoubleValue());
        break;
      case DataType::Bool8:
        dst.setBoolValue(src.getBoolValue());
        break;
      case DataType::String:
        dst.setStringValue(src.getStringValue());
        break;
      default:
        throw std::runtime_error("Attempting to copy invalid scalar");
    }
  }

 public:
  /**
   * @brief Get the data type stored in this scalar.
   *
   * @return The active data type.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtScalar_getDataType
   */
  DataType getDataType() const {
    return static_cast<DataType>(callGetter<&interface_type::getDataType>());
  }

  float getFloatValue() const { return callGetter<&interface_type::getFloatValue>(); }

  double getDoubleValue() const { return callGetter<&interface_type::getDoubleValue>(); }

  uint64_t getUint64Value() const { return callGetter<&interface_type::getUint64Value>(); }

  uint32_t getUint32Value() const { return callGetter<&interface_type::getUint32Value>(); }

  uint16_t getUint16Value() const { return callGetter<&interface_type::getUint16Value>(); }

  uint8_t getUint8Value() const { return callGetter<&interface_type::getUint8Value>(); }

  int64_t getInt64Value() const { return callGetter<&interface_type::getInt64Value>(); }

  int32_t getInt32Value() const { return callGetter<&interface_type::getInt32Value>(); }

  int16_t getInt16Value() const { return callGetter<&interface_type::getInt16Value>(); }

  int8_t getInt8Value() const { return callGetter<&interface_type::getInt8Value>(); }

  const char* getStringValue() const { return callGetter<&interface_type::getStringValue>(); }

  bool getBoolValue() const {
    return detail::resolveBool(callGetter<&interface_type::getBoolValue>());
  }

  /// @see QairtScalar_setFloatValue
  void setFloatValue(float value) { callSetter<&interface_type::setFloatValue>(value); }

  /// @see QairtScalar_setDoubleValue
  void setDoubleValue(double value) { callSetter<&interface_type::setDoubleValue>(value); }

  /// @see QairtScalar_setUint64Value
  void setUint64Value(uint64_t value) { callSetter<&interface_type::setUint64Value>(value); }

  /// @see QairtScalar_setUint32Value
  void setUint32Value(uint32_t value) { callSetter<&interface_type::setUint32Value>(value); }

  /// @see QairtScalar_setUint16Value
  void setUint16Value(uint16_t value) { callSetter<&interface_type::setUint16Value>(value); }

  /// @see QairtScalar_setUint8Value
  void setUint8Value(uint8_t value) { callSetter<&interface_type::setUint8Value>(value); }

  /// @see QairtScalar_setInt64Value
  void setInt64Value(int64_t value) { callSetter<&interface_type::setInt64Value>(value); }

  /// @see QairtScalar_setInt32Value
  void setInt32Value(int32_t value) { callSetter<&interface_type::setInt32Value>(value); }

  /// @see QairtScalar_setInt16Value
  void setInt16Value(int16_t value) { callSetter<&interface_type::setInt16Value>(value); }

  /// @see QairtScalar_setInt8Value
  void setInt8Value(int8_t value) { callSetter<&interface_type::setInt8Value>(value); }

  /**
   * @brief Set the string value of this scalar.
   *
   * @param[in] value  Null-terminated string. Must not be NULL.
   *
   * @throws qairt::Exception on invalid handle or null argument.
   *
   * @see QairtScalar_setStringValue
   */
  void setStringValue(const char* value) { callSetter<&interface_type::setStringValue>(value); }

  /// @see QairtScalar_setBoolValue
  void setBoolValue(bool value) {
    callSetter<&interface_type::setBoolValue>(detail::convertBool(value));
  }

 private:
  explicit Scalar(const std::shared_ptr<ApiTable>& apiTable) : ApiType(apiTable) { construct(); }
};

/**
 * @brief Wrapper for a named QAIRT op parameter, holding either a Scalar or a Tensor.
 *
 *        Obtained via Api::make<Param>().
 */
class Param : public ApiType<Param, QairtOpConfig_ParamV1_t> {
  friend class Api;

  using scalar_type = detail::crossable<detail::non_owning<Scalar>,
                                        &interface_type::getScalarParam,
                                        &interface_type::setScalarParam>;
  using tensor_type = detail::crossable<detail::non_owning<Tensor>,
                                        &interface_type::getTensorParam,
                                        &interface_type::setTensorParam>;
  /// @brief Active parameter value: empty, scalar reference, or tensor reference.
  std::variant<std::monostate, scalar_type, tensor_type> m_var;
  /// @brief Name of this parameter, synchronized with the underlying C handle.
  detail::crossable<std::string, &interface_type::getName, &interface_type::setName> m_name;

 public:
  using ApiType::ApiType;
  using ApiType::operator=;

  Param() noexcept                   = default;
  Param(const Param&)                = delete;
  Param(Param&&) noexcept            = default;
  Param& operator=(const Param&)     = delete;
  Param& operator=(Param&&) noexcept = default;

  /**
   * @brief Construct a shallow copy of this parameter.
   *
   * The copy shares the same scalar or tensor reference as the original
   * without taking ownership. Use this to pass a parameter to multiple
   * op configs without duplicating its underlying data.
   *
   * @return A new Param referencing the same scalar or tensor as this instance.
   *
   * @throws qairt::Exception on invalid handle.
   */
  Param shallowCopy() const {
    Param toret(copy_table_tag, *this);
    std::visit(
        [&toret, this](auto&& arg) {
          using T = std::decay_t<decltype(arg)>;
          if constexpr (std::is_same_v<T, scalar_type>) {
            toret.setScalar(getScalar());
          } else if constexpr (std::is_same_v<T, tensor_type>) {
            toret.setTensor(getTensor());
          }
        },
        m_var);
    return toret;
  }

  /**
   * @brief Get the parameter type (scalar or tensor) of this parameter.
   *
   * @return The active parameter type.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtOpConfig_Param_getParamType
   */
  ParamType getParamType() const {
    return static_cast<ParamType>(callGetter<&interface_type::getParamType>());
  }

  /**
   * @brief Get the name of this parameter.
   *
   * @return Reference to the parameter name string.
   *
   * @see QairtOpConfig_Param_getName
   */
  std::string& getName() { return m_name.get(); }

  /**
   * @brief Get the name of this parameter.
   *
   * @return Const reference to the parameter name string.
   *
   * @see QairtOpConfig_Param_getName
   */
  const std::string& getName() const { return m_name.get(); }

  /**
   * @brief Set the name of this parameter.
   *
   * @param[in] name  Name to assign.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtOpConfig_Param_setName
   */
  void setName(const std::string& name) { m_name = name; }

  /// @see Param::setName(const std::string&)
  void setName(std::string&& name) { m_name = std::move(name); }

  /**
   * @brief Get the scalar associated with this parameter.
   *
   * The parameter type must be ParamType::Scalar; call getParamType() to verify.
   *
   * @return Reference to the scalar held by this parameter.
   *
   * @throws std::bad_variant_access if the active type is not scalar.
   *
   * @see QairtOpConfig_Param_getScalarParam
   */
  Scalar& getScalar() { return std::get<scalar_type>(m_var).bindNonOwning(*this); }

  /**
   * @brief Get the scalar associated with this parameter.
   *
   * The parameter type must be ParamType::Scalar; call getParamType() to verify.
   *
   * @return Const reference to the scalar held by this parameter.
   *
   * @throws std::bad_variant_access if the active type is not scalar.
   *
   * @see QairtOpConfig_Param_getScalarParam
   */
  const Scalar& getScalar() const { return std::get<scalar_type>(m_var).bindNonOwning(*this); }

  /**
   * @brief Set the scalar value of this parameter.
   *
   * @param[in] scalar  Scalar to associate with this parameter.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtOpConfig_Param_setScalarParam
   */
  void setScalar(const Scalar& scalar) {
    if (!std::holds_alternative<scalar_type>(m_var)) {
      QAIRT_NO_RETURN m_var.template emplace<scalar_type>();
    }
    QAIRT_NO_RETURN std::get<scalar_type>(m_var).setNonOwning(*this, scalar);
  }

  /**
   * @brief Get the tensor associated with this parameter.
   *
   * The parameter type must be ParamType::Tensor; call getParamType() to verify.
   *
   * @return Reference to the tensor held by this parameter.
   *
   * @throws std::bad_variant_access if the active type is not tensor.
   *
   * @see QairtOpConfig_Param_getTensorParam
   */
  Tensor& getTensor() { return std::get<tensor_type>(m_var).bindNonOwning(*this); }

  /**
   * @brief Get the tensor associated with this parameter.
   *
   * The parameter type must be ParamType::Tensor; call getParamType() to verify.
   *
   * @return Const reference to the tensor held by this parameter.
   *
   * @throws std::bad_variant_access if the active type is not tensor.
   *
   * @see QairtOpConfig_Param_getTensorParam
   */
  const Tensor& getTensor() const { return std::get<tensor_type>(m_var).bindNonOwning(*this); }

  /**
   * @brief Set the tensor value of this parameter.
   *
   * @param[in] tensor  Tensor to associate with this parameter.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtOpConfig_Param_setTensorParam
   */
  void setTensor(const Tensor& tensor) {
    if (!std::holds_alternative<tensor_type>(m_var)) {
      QAIRT_NO_RETURN m_var.template emplace<tensor_type>();
    }
    QAIRT_NO_RETURN std::get<tensor_type>(m_var).setNonOwning(*this, tensor);
  }

 private:
  explicit Param(const std::shared_ptr<ApiTable>& apiTable) : ApiType(apiTable) { construct(); }

  void prepareToCross() const {
    prepareMultipleToCross(m_name);
    std::visit(
        [this](auto& arg) {
          using T = std::decay_t<decltype(arg)>;
          if constexpr (!std::is_same_v<std::monostate, T>) {
            prepareMultipleToCross(arg);
          }
        },
        m_var);
  }
  void updateAfterCross() const {
    updateMultipleAfterCross(m_name);
    std::visit(
        [this](auto& arg) {
          using T = std::decay_t<decltype(arg)>;
          if constexpr (!std::is_same_v<std::monostate, T>) {
            updateMultipleAfterCross(arg);
          }
        },
        m_var);
  }
};

/**
 * @brief Wrapper for a QAIRT op configuration describing a single graph node.
 *
 *        Obtained via Api::make<OpConfig>().
 */
class OpConfig : public ApiType<OpConfig, QairtOpConfig_V1_t> {
  friend class Api;
  /// @brief Instance name of this op, synchronized with the underlying C handle.
  detail::crossable<std::string, &interface_type::getName, &interface_type::setName> m_name;
  /// @brief Package name that provides this op, synchronized with the underlying C handle.
  detail::crossable<std::string, &interface_type::getPackageName, &interface_type::setPackageName>
      m_packageName;
  /// @brief Op type name within the package, synchronized with the underlying C handle.
  detail::crossable<std::string, &interface_type::getTypeName, &interface_type::setTypeName>
      m_typeName;

  /// @brief Ordered list of op parameters, synchronized with the underlying C handle.
  detail::crossable<std::vector<detail::non_owning<Param>>,
                    &interface_type::getParamAt,
                    &interface_type::getNumParams,
                    &interface_type::setParams>
      m_params;

  /// @brief Ordered list of input tensors, synchronized with the underlying C handle.
  detail::crossable<std::vector<detail::non_owning<Tensor>>,
                    &interface_type::getInputTensorAt,
                    &interface_type::getNumInputTensors,
                    &interface_type::setInputTensors>
      m_inputs;
  /// @brief Ordered list of output tensors, synchronized with the underlying C handle.
  detail::crossable<std::vector<detail::non_owning<Tensor>>,
                    &interface_type::getOutputTensorAt,
                    &interface_type::getNumOutputTensors,
                    &interface_type::setOutputTensors>
      m_outputs;

 public:
  OpConfig() noexcept                      = default;
  OpConfig(const OpConfig&)                = delete;
  OpConfig(OpConfig&&) noexcept            = default;
  OpConfig& operator=(const OpConfig&)     = delete;
  OpConfig& operator=(OpConfig&&) noexcept = default;

  /**
   * @brief Get the instance name of this op configuration.
   *
   * @return Reference to the instance name string.
   *
   * @see QairtOpConfig_getName
   */
  std::string& getName() { return m_name.get(); }

  /**
   * @brief Get the instance name of this op configuration.
   *
   * @return Const reference to the instance name string.
   *
   * @see QairtOpConfig_getName
   */
  const std::string& getName() const { return m_name.get(); }

  /**
   * @brief Set the instance name of this op configuration.
   *
   * @param[in] name  Instance name to assign.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtOpConfig_setName
   */
  void setName(const std::string& name) { m_name = name; }

  /// @see OpConfig::setName(const std::string&)
  void setName(std::string&& name) { m_name = std::move(name); }

  /**
   * @brief Get the package name for this op configuration.
   *
   * @return Reference to the package name string.
   *
   * @see QairtOpConfig_getPackageName
   */
  std::string& getPackageName() { return m_packageName.get(); }

  /**
   * @brief Get the package name for this op configuration.
   *
   * @return Const reference to the package name string.
   *
   * @see QairtOpConfig_getPackageName
   */
  const std::string& getPackageName() const { return m_packageName.get(); }

  /**
   * @brief Set the package name for this op configuration.
   *
   * @param[in] packageName  Package that provides this op type.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtOpConfig_setPackageName
   */
  void setPackageName(const std::string& packageName) { m_packageName = packageName; }

  /// @see OpConfig::setPackageName(const std::string&)
  void setPackageName(std::string&& packageName) { m_packageName = std::move(packageName); }

  /**
   * @brief Get the op type name within its package.
   *
   * @return Reference to the type name string.
   *
   * @see QairtOpConfig_getTypeName
   */
  std::string& getTypeName() { return m_typeName.get(); }

  /**
   * @brief Get the op type name within its package.
   *
   * @return Const reference to the type name string.
   *
   * @see QairtOpConfig_getTypeName
   */
  const std::string& getTypeName() const { return m_typeName.get(); }

  /**
   * @brief Set the op type name within its package.
   *
   * @param[in] typeName  Op type identifier within the package.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtOpConfig_setTypeName
   */
  void setTypeName(const std::string& typeName) { m_typeName = typeName; }

  /// @see OpConfig::setTypeName(const std::string&)
  void setTypeName(std::string&& typeName) { m_typeName = std::move(typeName); }

  /**
   * @brief Set the parameters of this op configuration.
   *
   * @param[in] params  Ordered list of parameters for this op.
   *
   * @throws qairt::Exception on invalid handle or invalid argument.
   *
   * @see QairtOpConfig_setParams
   */
  void setParams(const std::vector<Param>& params) { m_params.set(*this, params); }

  /**
   * @brief Get the parameters of this op configuration.
   *
   * @return Const reference to the ordered list of parameters.
   *
   * @see QairtOpConfig_getParamAt
   */
  const std::vector<Param>& getParams() const { return m_params.get(*this); }

  /**
   * @brief Get the parameters of this op configuration.
   *
   * @return Reference to the ordered list of parameters.
   *
   * @see QairtOpConfig_getParamAt
   */
  std::vector<Param>& getParams() { return m_params.get(*this); }

  /**
   * @brief Set the input tensors of this op configuration from a vector.
   *
   * @param[in] inputs  Ordered list of input tensors.
   *
   * @throws qairt::Exception on invalid handle or invalid argument.
   *
   * @see QairtOpConfig_setInputTensors
   */
  void setInputs(const std::vector<Tensor>& inputs) { m_inputs.set(*this, inputs); }

  /**
   * @brief Get the input tensors of this op configuration.
   *
   * @return Reference to the ordered list of input tensors.
   *
   * @see QairtOpConfig_getInputTensorAt
   */
  std::vector<Tensor>& getInputs() { return m_inputs.get(*this); }

  /**
   * @brief Get the input tensors of this op configuration.
   *
   * @return Const reference to the ordered list of input tensors.
   *
   * @see QairtOpConfig_getInputTensorAt
   */
  const std::vector<Tensor>& getInputs() const { return m_inputs.get(*this); }

  /**
   * @brief Set the input tensors of this op configuration from individual tensor arguments.
   *
   * Convenience overload accepting one or more Tensor references directly.
   * Each tensor is shallow-copied into the input list.
   *
   * @param[in] tensor0   First input tensor.
   * @param[in] tensors   Additional input tensors (zero or more).
   *
   * @throws qairt::Exception on invalid handle or invalid argument.
   *
   * @see OpConfig::setInputs(const std::vector<Tensor>&)
   */
  template <typename... T_Tensors>
  auto setInputs(Tensor& tensor0, T_Tensors&... tensors) -> std::enable_if_t<
      detail::all_are_same_v<Tensor, std::remove_reference_t<std::remove_const_t<T_Tensors>>...> ||
          sizeof...(T_Tensors) == 0,
      void> {
    std::vector<Tensor> tmp;
    tmp.reserve(1 + sizeof...(T_Tensors));
    tmp.push_back(tensor0.shallowCopy());
    (tmp.push_back(tensors.shallowCopy()), ...);
    setInputs(std::move(tmp));
  }

  /**
   * @brief Set the output tensors of this op configuration from a vector.
   *
   * @param[in] outputs  Ordered list of output tensors.
   *
   * @throws qairt::Exception on invalid handle or invalid argument.
   *
   * @see QairtOpConfig_setOutputTensors
   */
  void setOutputs(const std::vector<Tensor>& outputs) { m_outputs.set(*this, outputs); }

  /**
   * @brief Get the output tensors of this op configuration.
   *
   * @return Reference to the ordered list of output tensors.
   *
   * @see QairtOpConfig_getOutputTensorAt
   */
  std::vector<Tensor>& getOutputs() { return m_outputs.get(*this); }

  /**
   * @brief Get the output tensors of this op configuration.
   *
   * @return Const reference to the ordered list of output tensors.
   *
   * @see QairtOpConfig_getOutputTensorAt
   */
  const std::vector<Tensor>& getOutputs() const { return m_outputs.get(*this); }

  /**
   * @brief Set the output tensors of this op configuration from individual tensor arguments.
   *
   * Convenience overload accepting one or more Tensor references directly.
   * Each tensor is shallow-copied into the output list.
   *
   * @param[in] tensor0   First output tensor.
   * @param[in] tensors   Additional output tensors (zero or more).
   *
   * @throws qairt::Exception on invalid handle or invalid argument.
   *
   * @see OpConfig::setOutputs(const std::vector<Tensor>&)
   */
  template <typename... T_Tensors>
  auto setOutputs(Tensor& tensor0, T_Tensors&... tensors) -> std::enable_if_t<
      detail::all_are_same_v<Tensor, std::remove_reference_t<std::remove_const_t<T_Tensors>>...> ||
          sizeof...(T_Tensors) == 0,
      void> {
    std::vector<Tensor> tmp;
    tmp.reserve(1 + sizeof...(T_Tensors));
    tmp.push_back(tensor0.shallowCopy());
    (tmp.push_back(tensors.shallowCopy()), ...);
    setOutputs(std::move(tmp));
  }

 private:
  explicit OpConfig(const std::shared_ptr<ApiTable>& apiTable) : ApiType(apiTable) { construct(); }

  void prepareToCross() const {
    prepareMultipleToCross(m_name, m_packageName, m_typeName, m_params, m_inputs, m_outputs);
  }
  void updateAfterCross() const {
    updateMultipleAfterCross(m_name, m_packageName, m_typeName, m_params);
  }
};

}  // namespace qairt

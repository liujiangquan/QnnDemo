//==============================================================================
//
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// All rights reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//==============================================================================

/**
 * @file
 * @brief C++ wrapper for the QAIRT Device API.
 */

#pragma once

#include <optional>

#include "QairtApi.hpp"
#include "QairtDevice/QairtDevice.h"

namespace qairt {

/**
 * @brief Error codes returned by QAIRT device operations.
 *
 * @details
 * | Enumerator           | Description                                         |
 * |----------------------|-----------------------------------------------------|
 * | `NoError`            | Operation succeeded.                                |
 * | `UnsupportedFeature` | Requested feature is not yet supported.             |
 * | `MemAlloc`           | Memory allocation or deallocation failure.          |
 * | `InvalidArgument`    | An argument to the operation was invalid.           |
 * | `OutOfRange`         | Requested element is out of range.                  |
 * | `InvalidHandle`      | The provided handle is not valid.                   |
 * | `InvalidConfig`      | One or more configuration values are invalid.       |
 * | `HardwareUnavailable`| Requested hardware is unavailable.                  |
 * | `AssociatedToContext`| Device is associated to an existing context.        |
 * | `Undefined`          | An undefined or unknown error occurred.             |
 */
QAIRT_CPPAPI_DEFINE_ENUM(DeviceError,
                         QairtDevice_Error_t,
                         NoError             = QAIRT_DEVICE_NO_ERROR,
                         UnsupportedFeature  = QAIRT_DEVICE_ERROR_UNSUPPORTED_FEATURE,
                         MemAlloc            = QAIRT_DEVICE_ERROR_MEM_ALLOC,
                         InvalidArgument     = QAIRT_DEVICE_ERROR_INVALID_ARGUMENT,
                         OutOfRange          = QAIRT_DEVICE_ERROR_OUT_OF_RANGE,
                         InvalidHandle       = QAIRT_DEVICE_ERROR_INVALID_HANDLE,
                         InvalidConfig       = QAIRT_DEVICE_ERROR_INVALID_CONFIG,
                         HardwareUnavailable = QAIRT_DEVICE_ERROR_HARDWARE_UNAVAILABLE,
                         AssociatedToContext = QAIRT_DEVICE_ERROR_ASSOCIATED_TO_CONTEXT,
                         Undefined           = QAIRT_DEVICE_ERROR_UNDEFINED)

/**
 * @brief Placeholder for future backend-specific extended core information.
 *
 *        Reserved for future use; no members are defined at this time.
 */
class DeviceHardwareCoreExtendedInfo {};

/**
 * @brief Hardware core information for a single compute core on a device.
 *
 *        Obtained as a sub-object of @ref DeviceHardwareDeviceInfo. Core objects
 *        are not constructed directly; they are returned by the device info API.
 */
class DeviceHardwareCoreInfo
    : public ApiType<DeviceHardwareCoreInfo, QairtDevice_HardwareCoreInfoV1_t> {
  friend class Api;

 public:
  DeviceHardwareCoreInfo() noexcept                                    = default;
  DeviceHardwareCoreInfo(const DeviceHardwareCoreInfo&)                = delete;
  DeviceHardwareCoreInfo(DeviceHardwareCoreInfo&&) noexcept            = default;
  DeviceHardwareCoreInfo& operator=(const DeviceHardwareCoreInfo&)     = delete;
  DeviceHardwareCoreInfo& operator=(DeviceHardwareCoreInfo&&) noexcept = default;

  /**
   * @brief Get the hardware core identifier.
   *
   * @return Numeric core ID for this core. Use @c QAIRT_DEVICE_DEFAULT_CORE_ID
   * to select the default core.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtDevice_HardwareCoreInfo_getCoreId
   */
  uint32_t getCoreId() const { return callGetter<&interface_type::getCoreId>(); }

  /**
   * @brief Get the hardware core type.
   *
   * @return Backend-defined numeric type code identifying the kind of compute core.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtDevice_HardwareCoreInfo_getCoreType
   */
  uint32_t getCoreType() const { return callGetter<&interface_type::getCoreType>(); }

  DeviceHardwareCoreExtendedInfo getExtendedInfo() const { throw QAIRT_CPPAPI_STUB_EXCEPTION; }
};

/**
 * @brief Hardware device information, including identity and core topology.
 *
 *        Obtained as a sub-object of @ref DevicePlatformInfo. Device info objects
 *        are not constructed directly; they are returned by the platform info API.
 */
class DeviceHardwareDeviceInfo
    : public ApiType<DeviceHardwareDeviceInfo, QairtDevice_HardwareDeviceInfoV1_t> {
  friend class Api;

 public:
  DeviceHardwareDeviceInfo() noexcept                                      = default;
  DeviceHardwareDeviceInfo(const DeviceHardwareDeviceInfo&)                = delete;
  DeviceHardwareDeviceInfo(DeviceHardwareDeviceInfo&&) noexcept            = default;
  DeviceHardwareDeviceInfo& operator=(const DeviceHardwareDeviceInfo&)     = delete;
  DeviceHardwareDeviceInfo& operator=(DeviceHardwareDeviceInfo&&) noexcept = default;

  /**
   * @brief Get the hardware device identifier.
   *
   * @return Numeric device ID for this hardware device. Use
   * @c QAIRT_DEVICE_DEFAULT_DEVICE_ID to select the default device.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtDevice_HardwareDeviceInfo_getDeviceId
   */
  uint32_t getDeviceId() const { return callGetter<&interface_type::getDeviceId>(); }

  /**
   * @brief Get the hardware device type.
   *
   * @return Backend-defined numeric type code identifying the class of hardware device.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtDevice_HardwareDeviceInfo_getDeviceType
   */
  uint32_t getDeviceType() const { return callGetter<&interface_type::getDeviceType>(); }

  /**
   * @brief Get the number of compute cores in this hardware device.
   *
   * @return Number of core info entries reported for this device.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtDevice_HardwareDeviceInfo_getNumCoreInfos
   */
  uint32_t getNumCores() const { return callGetter<&interface_type::getNumCoreInfos>(); }

  std::vector<DeviceHardwareCoreInfo>& getCores() { throw QAIRT_CPPAPI_STUB_EXCEPTION; }
  const std::vector<DeviceHardwareCoreInfo>& getCores() const { throw QAIRT_CPPAPI_STUB_EXCEPTION; }
};

/**
 * @brief Platform-level hardware information, enumerating the devices present on the platform.
 *
 *        Obtained via @ref DeviceConfiguration::getPlatformInfo().
 */
class DevicePlatformInfo : public ApiType<DevicePlatformInfo, QairtDevice_PlatformInfoV1_t> {
  friend class Api;

 public:
  using ApiType::ApiType;
  using ApiType::operator=;

  size_t getNumHwDevices() const { throw QAIRT_CPPAPI_STUB_EXCEPTION; }

  std::vector<DeviceHardwareDeviceInfo>& getHwDevices() { throw QAIRT_CPPAPI_STUB_EXCEPTION; }
  const std::vector<DeviceHardwareDeviceInfo>& getHwDevices() const {
    throw QAIRT_CPPAPI_STUB_EXCEPTION;
  }
  void setHwDevices(std::vector<DeviceHardwareDeviceInfo> hwDevices) {
    (void)hwDevices;
    throw QAIRT_CPPAPI_STUB_EXCEPTION;
  }

 private:
  void prepareToCross() const {}
  void updateAfterCross() const {}
};

/**
 * @brief Abstract base class for backend-specific device custom configuration.
 *
 *        Subclass to provide a backend-specific custom configuration handle; pass
 *        an instance to @ref DeviceConfiguration::setCustomConfig(). Refer to the
 *        backend documentation for required subclass implementations.
 */
class DeviceCustomConfig : public CustomConfigType {
 public:
  /**
   * @brief Get the underlying C API custom configuration handle.
   *
   * Implement this method in a backend-specific subclass to return the
   * handle that will be passed to @ref QairtDevice_Config_setCustomConfig.
   * Refer to the backend documentation for required subclass implementations
   * and valid handle values.
   *
   * @return Backend-specific custom configuration handle.
   *
   * @see QairtDevice_Config_setCustomConfig
   */
  virtual QairtDevice_CustomConfigHandle_t getCustomConfigHandle() const = 0;
};

/**
 * @brief Configuration object for device creation.
 *
 *        Construct via @ref Api::make<DeviceConfiguration>() and pass to the device
 *        creation API. Holds platform info and optional backend-specific custom config.
 */
class DeviceConfiguration : public ApiType<DeviceConfiguration, QairtDevice_ConfigV1_t> {
  friend class Api;

  detail::crossable<detail::non_owning<DevicePlatformInfo>,
                    &interface_type::getPlatformInfo,
                    &interface_type::setPlatformInfo>
      m_platformInfo;

 public:
  using ApiType::ApiType;
  using ApiType::operator=;

  /**
   * @brief Set backend-specific custom configuration for this device configuration.
   *
   * @param[in] config  Backend-specific custom configuration object.
   *
   * @throws qairt::Exception on invalid handle or invalid argument.
   *
   * @see QairtDevice_Config_setCustomConfig
   */
  void setCustomConfig(const DeviceCustomConfig& config) {
    auto cch = config.getCustomConfigHandle();
    callAndConfirm<&interface_type::setCustomConfig>(cch);
  }

  /**
   * @brief Clear all backend-specific custom configurations from this device configuration.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtDevice_Config_clearCustomConfigs
   */
  void clearCustomConfigs() { callAndConfirm<&interface_type::clearCustomConfigs>(); }

  /**
   * @brief Get the platform info associated with this device configuration.
   *
   * @return Reference to the platform info for this configuration.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtDevice_Config_getPlatformInfo
   */
  DevicePlatformInfo& getPlatformInfo() { return m_platformInfo.bindNonOwning(*this); }

  /**
   * @brief Get the platform info associated with this device configuration.
   *
   * @return Const reference to the platform info for this configuration.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtDevice_Config_getPlatformInfo
   */
  const DevicePlatformInfo& getPlatformInfo() const { return m_platformInfo.bindNonOwning(*this); }

  /**
   * @brief Set the platform info for this device configuration.
   *
   * @param[in] platformInfo  Platform info describing the hardware devices to
   *                          request. Must be a valid, fully configured object.
   *
   * @throws qairt::Exception on invalid handle or invalid argument.
   *
   * @see QairtDevice_Config_setPlatformInfo
   */
  void setPlatformInfo(const DevicePlatformInfo& platformInfo) {
    QAIRT_NO_RETURN m_platformInfo.setNonOwning(*this, platformInfo);
  }

 private:
  void prepareToCross() const { prepareMultipleToCross(m_platformInfo); }
  void updateAfterCross() const { updateMultipleAfterCross(m_platformInfo); }
};

/**
 * @brief Wrapper for a QAIRT Device handle.
 *
 * @note Not yet implemented; all methods throw @ref qairt::Exception at runtime.
 */
class Device : public ApiType<Device, QairtDevice_V1_t> {
  friend class Api;

  // STUB
 public:
  using ApiType::ApiType;
  using ApiType::operator=;

  void setConfig(const DeviceConfiguration& deviceConfig) { (void)deviceConfig; throw QAIRT_CPPAPI_STUB_EXCEPTION; }
  DeviceConfiguration& getConfig() { throw QAIRT_CPPAPI_STUB_EXCEPTION; }

  DevicePlatformInfo getInfo() { throw QAIRT_CPPAPI_STUB_EXCEPTION; }
};
//
}  // namespace qairt

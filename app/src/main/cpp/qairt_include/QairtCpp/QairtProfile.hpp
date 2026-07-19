//==============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//==============================================================================

#pragma once

/**
 * @file
 * @brief C++ wrapper for the QAIRT profiling API.
 *
 *        A Backend must be created before constructing a Profile object.
 */

#include <stdexcept>
#include <type_traits>
#include <variant>
#include <vector>

#include "QairtApi.hpp"
#include "QairtLog.hpp"
#include "QairtProfile/QairtProfile.h"
#include "QairtProfile/QairtProfileBackendOpaqueObject.h"
#include "QairtProfile/QairtProfileConfig.h"
#include "QairtProfile/QairtProfileEvent.h"
#include "QairtTensor.hpp"

namespace qairt {

/**
 * @brief Event type classifications for profiling events.
 *
 * @details
 * | Enumerator          | Description                                                    |
 * |---------------------|----------------------------------------------------------------|
 * | `Undefined`         | Unclassified or unknown event type.                            |
 * | `Init`              | Context creation and graph finalization stage.                 |
 * | `Finalize`          | Graph finalize operation stage.                                |
 * | `Execute`           | Graph execution stage.                                         |
 * | `Node`              | Individual operation execution; sub-event of Execute.          |
 * | `QueueWait`         | Time spent waiting in the execution queue; sub-event of Execute. |
 * | `ExecutePreprocess` | Pre-processing before on-device execution; sub-event of Execute. |
 * | `ExecuteDevice`     | On-device execution stage; sub-event of Execute.               |
 * | `ExecutePostprocess`| Post-processing after on-device execution; sub-event of Execute. |
 * | `DeInit`            | Context teardown and resource-free stage.                      |
 * | `Trace`             | Graph preparation and execution trace; detailed level only.    |
 * | `Backend`           | Backend-defined event type; consult backend documentation.     |
 */
enum class ProfileEventType : uint32_t {
  Undefined          = 0,
  Init               = QAIRT_PROFILE_EVENTTYPE_INIT,
  Finalize           = QAIRT_PROFILE_EVENTTYPE_FINALIZE,
  Execute            = QAIRT_PROFILE_EVENTTYPE_EXECUTE,
  Node               = QAIRT_PROFILE_EVENTTYPE_NODE,
  QueueWait          = QAIRT_PROFILE_EVENTTYPE_EXECUTE_QUEUE_WAIT,
  ExecutePreprocess  = QAIRT_PROFILE_EVENTTYPE_EXECUTE_PREPROCESS,
  ExecuteDevice      = QAIRT_PROFILE_EVENTTYPE_EXECUTE_DEVICE,
  ExecutePostprocess = QAIRT_PROFILE_EVENTTYPE_EXECUTE_POSTPROCESS,
  DeInit             = QAIRT_PROFILE_EVENTTYPE_DEINIT,
  Trace              = QAIRT_PROFILE_EVENTTYPE_TRACE,
  Backend            = QAIRT_PROFILE_EVENTTYPE_BACKEND
};

/**
 * @brief Measurement unit classifications for profiling event values.
 *
 * @details
 * | Enumerator  | Description                                              |
 * |-------------|----------------------------------------------------------|
 * | `Undefined` | Unit is unspecified or unknown.                          |
 * | `MicroSec`  | Time measured in microseconds.                           |
 * | `Bytes`     | Memory measured in bytes.                                |
 * | `Cycles`    | Time measured in processor cycles.                      |
 * | `Count`     | Dimensionless count.                                     |
 * | `Object`    | Opaque object; interpretation is backend-dependent.      |
 * | `None`      | No applicable unit.                                      |
 * | `Backend`   | Backend-defined unit; consult backend documentation.     |
 */
enum class ProfileEventUnit : uint32_t {
  Undefined = 0,
  MicroSec  = QAIRT_PROFILE_EVENTUNIT_MICROSEC,
  Bytes     = QAIRT_PROFILE_EVENTUNIT_BYTES,
  Cycles    = QAIRT_PROFILE_EVENTUNIT_CYCLES,
  Count     = QAIRT_PROFILE_EVENTUNIT_COUNT,
  Object    = QAIRT_PROFILE_EVENTUNIT_OBJECT,
  None      = QAIRT_PROFILE_EVENTUNIT_NONE,
  Backend   = QAIRT_PROFILE_EVENTUNIT_BACKEND
};

/**
 * @brief Backend-specific opaque data attached to a profiling event.
 *
 *        Obtained via ProfileEventData::getBackendOpaqueObject().
 */
class ProfileBackendOpaqueObject
    : public ApiType<ProfileBackendOpaqueObject, QairtProfile_BackendOpaqueObjectV1_t> {
  friend class Api;

  /// @brief Raw byte buffer holding the backend-specific opaque object payload.
  mutable RawBuffer m_opaqueObject{};
  /// @brief File name associated with the backend opaque object, if any.
  detail::crossable<std::string, &interface_type::getFileName, &interface_type::setFileName>
      m_fileName{};

 public:
  using ApiType::ApiType;
  using ApiType::operator=;

  /**
   * @brief Set the opaque data buffer for this backend opaque object.
   *
   * @param[in] data  Raw buffer containing the backend-specific payload.
   *
   * @see QairtProfile_BackendOpaqueObject_setData
   */
  void setData(RawBuffer data) { m_opaqueObject = data; }

  /**
   * @brief Wrapper which allows for a raw pointer and size instead of a RawBuffer.
   *
   * @see ProfileBackendOpaqueObject::setData(RawBuffer)
   */
  void setData(void* data, uint64_t size) { m_opaqueObject = RawBuffer(data, size); }

  /**
   * @brief Get the opaque data buffer from this backend opaque object.
   *
   * @return RawBuffer holding the backend-specific payload.
   *
   * @see QairtProfile_BackendOpaqueObject_getData
   */
  RawBuffer getData() { return m_opaqueObject; }

  /**
   * @brief Set the file name associated with this backend opaque object.
   *
   * @param[in] fileName  File name string.
   *
   * @see QairtProfile_BackendOpaqueObject_setFileName
   */
  void setFileName(std::string fileName) { m_fileName = std::move(fileName); }

  /**
   * @brief Get the file name associated with this backend opaque object.
   *
   * @return Reference to the file name string, or an empty string if not set.
   *
   * @see QairtProfile_BackendOpaqueObject_getFileName
   */
  std::string& getFileName() { return m_fileName.get(); }

 private:
  void prepareToCross() const {
    callSetter<&interface_type::setData>(m_opaqueObject.getData(), m_opaqueObject.getDataSize());
    m_fileName.prepareToCross(*this);
  }

  void updateAfterCross() const {
    void* data;
    uint64_t size;
    callAndConfirm<&interface_type::getData>(&data, &size);
    m_opaqueObject.setData(data);
    m_opaqueObject.setDataSize(size);
    m_fileName.updateAfterCross(*this);
  }

  explicit ProfileBackendOpaqueObject(const std::shared_ptr<ApiTable>& apiTable)
      : ApiType(apiTable) {
    construct();
  }
};

class ProfileConfiguration;

/**
 * @brief Opaque handle for backend-specific profile configuration.
 *
 *        Wraps a QairtProfile_CustomConfigHandle_t. Refer to the backend
 *        documentation for valid values and usage.
 */
class ProfileCustomConfig {
  friend class ProfileConfiguration;
  /// @brief Underlying C handle for the backend-specific custom config.
  QairtProfile_CustomConfigHandle_t m_handle;

 public:
  /**
   * @brief Construct a ProfileCustomConfig wrapping an existing C handle.
   *
   * @param[in] handle  Backend-specific custom config handle.
   */
  explicit ProfileCustomConfig(QairtProfile_CustomConfigHandle_t handle) : m_handle(handle) {}
  ProfileCustomConfig(const ProfileCustomConfig&) noexcept            = default;
  ProfileCustomConfig(ProfileCustomConfig&&) noexcept                 = default;
  ProfileCustomConfig& operator=(const ProfileCustomConfig&) noexcept = default;
  ProfileCustomConfig& operator=(ProfileCustomConfig&&) noexcept      = default;

  bool operator==(const ProfileCustomConfig& other) const { return m_handle == other.m_handle; }
};

/**
 * @brief Configuration object for a QAIRT profile handle.
 */
class ProfileConfiguration : public ApiType<ProfileConfiguration, QairtProfile_ConfigV1_t> {
  friend class Api;

 public:
  using ApiType::ApiType;
  using ApiType::operator=;

  /**
   * @brief Set the backend-specific custom configuration for this profile.
   *
   * @param[in] profileCustomConfig  Backend-specific custom config handle.
   *
   * @return Reference to this ProfileConfiguration, for method chaining.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtProfile_Config_setCustomConfig
   */
  ProfileConfiguration& setCustomConfig(ProfileCustomConfig profileCustomConfig) {
    callSetter<&interface_type::setCustomConfig>(profileCustomConfig.m_handle);
    return *this;
  }

  /**
   * @brief Get the backend-specific custom configuration from this profile.
   *
   * @return ProfileCustomConfig wrapping the current custom config handle,
   *         or a ProfileCustomConfig(nullptr) if none was set.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtProfile_Config_getCustomConfig
   */
  ProfileCustomConfig getCustomConfig() const {
    QairtProfile_CustomConfigHandle_t handle = callGetter<&interface_type::getCustomConfig>();
    return ProfileCustomConfig(handle);
  }

  /**
   * @brief Set the maximum number of profiling events to collect.
   *
   * Once the limit is reached, no additional events are stored. The effective
   * maximum is also subject to a ceiling determined by the backend and
   * available system resources. The default is backend-specific.
   *
   * @param[in] numMaxEvents  Maximum number of events to store.
   *
   * @return Reference to this ProfileConfiguration, for method chaining.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtProfile_Config_setMaxEvents
   */
  ProfileConfiguration& setNumMaxEvents(uint64_t numMaxEvents) {
    callSetter<&interface_type::setMaxEvents>(numMaxEvents);
    return *this;
  }

  /**
   * @brief Get the configured maximum number of profiling events.
   *
   * @return Maximum number of events that will be stored, as previously set
   *         by setNumMaxEvents().
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtProfile_Config_getMaxEvents
   */
  uint64_t getNumMaxEvents() const { return callGetter<&interface_type::getMaxEvents>(); }

  /**
   * @brief Set whether op-trace profiling is enabled.
   *
   * @param[in] enableOpTrace  `true` to enable op-trace; `false` to disable.
   *
   * @return Reference to this ProfileConfiguration, for method chaining.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtProfile_Config_setEnableOpTrace
   */
  ProfileConfiguration& setEnableOpTrace(bool enableOpTrace) {
    callSetter<&interface_type::setEnableOpTrace>(enableOpTrace ? uint8_t{1} : uint8_t{0});
    return *this;
  }

  /**
   * @brief Get whether op-trace profiling is enabled.
   *
   * @return `true` if op-trace is enabled; `false` otherwise.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtProfile_Config_getEnableOpTrace
   */
  bool getEnableOpTrace() const {
    return detail::resolveBool(callGetter<&interface_type::getEnableOpTrace>());
  }

 private:
  explicit ProfileConfiguration(const std::shared_ptr<ApiTable>& apiTable) : ApiType(apiTable) {
    construct();
  }
};

class Profile;

/**
 * @brief A single profiling event returned by a QAIRT Profile handle.
 */
class ProfileEventData : public ApiType<ProfileEventData, QairtProfile_EventV1_t> {
  friend class Api;
  friend class Profile;

  /// @brief Internal event identifier used to index into the profile event cache.
  uint64_t m_eventId{};
  /// @brief String identifier for this event, lazily synchronized with the C layer.
  mutable detail::
      crossable<std::string, &interface_type::getIdentifier, &interface_type::setIdentifier>
          m_identifier;
  /// @brief Scalar value for this profiling event.
  mutable detail::crossable<detail::non_owning<Scalar>, &interface_type::getScalar, nullptr>
      m_scalar;

 public:
  using ApiType::ApiType;
  using ApiType::operator=;

  // Constructor that wraps an existing handle from getEventAt/getSubEventAt.
  // Non-owning: the underlying data is owned by the Profile's event cache.
  ProfileEventData(const std::shared_ptr<ApiTable>& apiTable, QairtProfile_EventHandle_t handle)
      : ApiType(apiTable) {
    setHandle(handle);
    setNonOwning(true);
  }

  /**
   * @brief Get the event type classification for this profiling event.
   *
   * @return ProfileEventType enumerator for this event.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtProfile_Event_getEventType
   */
  ProfileEventType getEventType() const {
    QairtProfile_EventType_t type;
    callAndConfirm<&interface_type::getEventType>(&type);
    return static_cast<ProfileEventType>(type);
  }

  /**
   * @brief Set the event type classification for this profiling event.
   *
   * @param[in] type  Event type to assign to this event.
   *
   * @throws qairt::Exception on invalid handle or invalid argument.
   *
   * @see QairtProfile_Event_setEventType
   */
  void setEventType(ProfileEventType type) {
    callSetter<&interface_type::setEventType>(static_cast<QairtProfile_EventType_t>(type));
  }

  /**
   * @brief Get the measurement unit for this profiling event's scalar value.
   *
   * @return ProfileEventUnit enumerator for this event.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtProfile_Event_getUnit
   */
  ProfileEventUnit getEventUnit() const {
    QairtProfile_EventUnit_t unit;
    callAndConfirm<&interface_type::getUnit>(&unit);
    return static_cast<ProfileEventUnit>(unit);
  }

  /**
   * @brief Set the measurement unit for this profiling event's scalar value.
   *
   * @param[in] unit  Measurement unit to assign to this event.
   *
   * @throws qairt::Exception on invalid handle or invalid argument.
   *
   * @see QairtProfile_Event_setUnit
   */
  void setEventUnit(ProfileEventUnit unit) {
    callSetter<&interface_type::setUnit>(static_cast<QairtProfile_EventUnit_t>(unit));
  }

  /**
   * @brief Get the timestamp recorded for this profiling event.
   *
   * @return Timestamp in microseconds.
   *
   * @throws qairt::Exception on invalid handle or if the event does not support timestamps.
   *
   * @see QairtProfile_Event_getTimestamp
   */
  uint64_t getTimestamp() const {
    uint64_t timestamp;
    auto status = call<&interface_type::getTimestamp>(&timestamp);
    if (status == QAIRT_PROFILE_ERROR_DATA_NOT_AVAILABLE) {
      throw Exception("Event does not support timestamp", CommonError::NotSupported);
    }
    if (status != QAIRT_SUCCESS) {
      throw Exception("Failed to get timestamp", static_cast<CommonError>(status));
    }
    return timestamp;
  }

  /**
   * @brief Set the timestamp for this profiling event.
   *
   * @param[in] timestamp  Timestamp in microseconds.
   *
   * @throws qairt::Exception on invalid handle or if the event does not support timestamps.
   *
   * @see QairtProfile_Event_setTimestamp
   */
  void setTimestamp(uint64_t timestamp) {
    auto status = call<&interface_type::setTimestamp>(timestamp);
    if (status == QAIRT_PROFILE_ERROR_INCOMPATIBLE_EVENT) {
      throw Exception("Event does not support timestamp", CommonError::NotSupported);
    }
    if (status != QAIRT_SUCCESS) {
      throw Exception("Failed to set timestamp", static_cast<CommonError>(status));
    }
  }

  /**
   * @brief Get the string identifier for this profiling event.
   *
   * @return Const reference to the identifier string; empty if not set.
   *
   * @see QairtProfile_Event_getIdentifier
   */
  const std::string& getIdentifier() const { return m_identifier.get(); }

  /**
   * @brief Set the string identifier for this profiling event.
   *
   * @param[in] identifier  Descriptive name for this event.
   *
   * @see QairtProfile_Event_setIdentifier
   */
  void setIdentifier(std::string identifier) { m_identifier = std::move(identifier); }

  /**
   * @brief Get the scalar measurement value for this profiling event.
   *
   * @return Const reference to the Scalar value associated with this event.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtProfile_Event_getScalar
   */
  const Scalar& getScalar() const { return m_scalar.bindNonOwning(*this); }

  /**
   * @brief Set the scalar measurement value for this profiling event.
   *
   * @param[in] scalar  Scalar value to associate with this event.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtProfile_Event_setScalar
   */
  void setScalar(const Scalar& scalar) {
    detail::QairtTypeAdmin::prepareToCross(scalar);
    callSetter<&interface_type::setScalar>(getHandle(scalar));
  }

  /**
   * @brief Get the backend opaque object attached to this profiling event.
   *
   * Only available for events that support the extended event format. Call
   * supportsExtendedFormat() to check before calling this method.
   *
   * @return ProfileBackendOpaqueObject holding the backend-specific payload.
   *
   * @throws qairt::Exception on invalid handle or if the event does not support
   *         backend opaque objects.
   *
   * @see QairtProfile_Event_getBackendOpaqueObject
   */
  ProfileBackendOpaqueObject getBackendOpaqueObject() const {
    QairtProfile_BackendOpaqueObjectHandle_t objHandle;
    auto status = call<&interface_type::getBackendOpaqueObject>(&objHandle);
    if (status == QAIRT_PROFILE_ERROR_DATA_NOT_AVAILABLE) {
      throw Exception("Backend opaque object not available for this event",
                      CommonError::NotSupported);
    }
    if (status != QAIRT_SUCCESS) {
      throw Exception("Failed to get backend opaque object", static_cast<CommonError>(status));
    }
    auto obj = constructNonOwning<ProfileBackendOpaqueObject>(objHandle);
    detail::QairtTypeAdmin::updateAfterCross(obj);
    return obj;
  }

  /**
   * @brief Set the backend opaque object for this profiling event.
   *
   * Only valid for events that support the extended event format. Call
   * supportsExtendedFormat() to check before calling this method.
   *
   * @param[in] obj  ProfileBackendOpaqueObject to attach to this event.
   *
   * @throws qairt::Exception on invalid handle or if the event does not support
   *         backend opaque objects.
   *
   * @see QairtProfile_Event_setBackendOpaqueObject
   */
  void setBackendOpaqueObject(const ProfileBackendOpaqueObject& obj) {
    detail::QairtTypeAdmin::prepareToCross(obj);
    auto status = call<&interface_type::setBackendOpaqueObject>(getHandle(obj));
    if (status == QAIRT_PROFILE_ERROR_INCOMPATIBLE_EVENT) {
      throw Exception("Event does not support backend opaque objects", CommonError::NotSupported);
    }
    if (status != QAIRT_SUCCESS) {
      throw Exception("Failed to set backend opaque object", static_cast<CommonError>(status));
    }
  }

  /**
   * @brief Query whether this event supports the extended event format.
   *
   * Extended format events support timestamp and backend opaque object fields.
   * Call this before getTimestamp() or getBackendOpaqueObject() to avoid
   * exceptions on events that do not carry those fields.
   *
   * @return `true` if extended event fields are available; `false` otherwise.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtProfile_Event_supportsExtendedEvent
   */
  bool supportsExtendedFormat() const {
    uint8_t supports;
    callAndConfirm<&interface_type::supportsExtendedEvent>(&supports);
    return detail::resolveBool(supports);
  }

 private:
  explicit ProfileEventData(const std::shared_ptr<ApiTable>& apiTable) : ApiType(apiTable) {
    construct();
  }
  void setEventId(uint64_t eventId) { m_eventId = eventId; }

  void prepareToCross() const {
    // Event data is referenced by handle; C API setters (setIdentifier etc.) are not
    // implemented in the QAIRT layer. No sync needed when crossing as a non-owning ref.
  }

  void updateAfterCross() const {
    // Event data is referenced by handle; getter functions (getIdentifier etc.) are
    // not implemented in the QAIRT layer. No sync needed when crossing as a non-owning ref.
  }
};

class Backend;

/**
 * @brief Wrapper for a QAIRT Profile handle.
 *
 *        Obtained via Backend::createProfile().
 */
class Profile : public ApiType<Profile, QairtProfile_V1_t> {
  friend class Api;
  friend class Backend;
  friend class ProfileEventData;

 public:
  using ApiType::ApiType;
  using ApiType::operator=;

  Profile(const Profile&)                = delete;
  Profile(Profile&&) noexcept            = default;
  Profile& operator=(const Profile&)     = delete;
  Profile& operator=(Profile&&) noexcept = default;

  /**
   * @brief Set or update the configuration options for this profile handle.
   *
   * @param[in] configs  ProfileConfiguration to apply to this profile.
   *
   * @throws qairt::Exception on invalid handle, invalid argument,
   *         in-use handle, or unsupported configuration option.
   *
   * @see QairtProfile_setConfig
   */
  void setConfig(const ProfileConfiguration& configs) {
    detail::QairtTypeAdmin::prepareToCross(configs);
    callSetter<&interface_type::setConfig>(getHandle(configs));
  }

  /**
   * @brief Get all top-level profiling events collected on this profile handle.
   *
   * @return Vector of ProfileEventData objects for each collected event.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtProfile_getNumEvents
   * @see QairtProfile_getEventAt
   */
  std::vector<ProfileEventData> getEvents() {
    uint32_t numEvents;
    callAndConfirm<&interface_type::getNumEvents>(&numEvents);

    std::vector<ProfileEventData> events;
    events.reserve(numEvents);

    for (uint32_t i = 0; i < numEvents; ++i) {
      QairtProfile_EventHandle_t eventHandle;
      callAndConfirm<&interface_type::getEventAt>(i, &eventHandle);

      // getEventAt returns an owning handle, create a new object around this non owning handle.
      ProfileEventData event = create<ProfileEventData>(eventHandle);
      event.updateAfterCross_();
      events.push_back(std::move(event));
    }

    return events;
  }

  /**
   * @brief Get all sub-events nested under a given top-level profiling event.
   *
   * Sub-events are available for events of type ProfileEventType::Execute at
   * the detailed profiling level and represent per-operation breakdowns.
   *
   * @param[in] profileEvent  Top-level event whose sub-events to retrieve.
   *
   * @return Vector of ProfileEventData objects for each sub-event.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtProfile_getNumSubEvents
   * @see QairtProfile_getSubEventAt
   */
  std::vector<ProfileEventData> getSubEvents(const ProfileEventData& profileEvent) {
    uint64_t numSubEvents;
    callAndConfirm<&interface_type::getNumSubEvents>(getHandle(profileEvent), &numSubEvents);

    std::vector<ProfileEventData> subEvents;
    subEvents.reserve(numSubEvents);

    for (uint32_t i = 0; i < static_cast<uint32_t>(numSubEvents); ++i) {
      QairtProfile_EventHandle_t subEventHandle;
      callAndConfirm<&interface_type::getSubEventAt>(getHandle(profileEvent), i, &subEventHandle);

      // getSubEventAt returns an owning handle, create a new object around this non owning handle.
      ProfileEventData subEvent = create<ProfileEventData>(subEventHandle);
      subEvent.updateAfterCross_();
      subEvents.push_back(std::move(subEvent));
    }

    return subEvents;
  }

  // Constructor that takes ownership of an existing handle (for Backend::createProfile)
  Profile(const std::shared_ptr<ApiTable>& apiTable, QairtProfile_Handle_t handle)
      : ApiType(apiTable) {
    setHandle(handle);
  }
};

}  // namespace qairt

//==============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//==============================================================================

/**
 * @file
 * @brief  C++ wrapper for the QAIRT System Profile API.
 *
 */

#pragma once

#include "../QairtApi.hpp"
#include "../QairtProfile.hpp"
#include "QairtSystem/QairtSystemProfile.h"
#include "QairtSystemApi.hpp"

namespace qairt {

/**
 * @brief Visibility classification for system profile events.
 *
 * @details
 * | Enumerator | Description                                         |
 * |------------|-----------------------------------------------------|
 * | `Public`   | Event is visible to all consumers.                  |
 * | `Private`  | Event is internal and not exposed to consumers.     |
 */
QAIRT_CPPAPI_DEFINE_ENUM(SystemProfileVisibility,
                         QairtSystem_Profile_Visibility_t,
                         Public  = QAIRT_SYSTEM_PROFILE_VISIBILITY_PUBLIC,
                         Private = QAIRT_SYSTEM_PROFILE_VISIBILITY_PRIVATE)

/**
 * @brief Operation type associated with a system profile event.
 *
 * @details
 * | Enumerator                  | Description                                          |
 * |-----------------------------|------------------------------------------------------|
 * | `None`                      | No specific operation type.                          |
 * | `BackendExecute`            | Backend synchronous execute operation.               |
 * | `BackendFinalize`           | Backend finalize operation.                          |
 * | `BackendExecuteAsync`       | Backend asynchronous execute operation.              |
 * | `BackendCreateFromBinary`   | Backend create-from-binary operation.                |
 * | `BackendDeinit`             | Backend deinitialization operation.                  |
 * | `AppContextCreate`          | Application context creation operation.              |
 * | `AppComposeGraphs`          | Application compose-graphs operation.                |
 * | `AppExecuteIps`             | Application execute-IPS operation.                   |
 * | `BackendGraphComponent`     | Backend graph component operation.                   |
 * | `BackendLibLoad`            | Application backend library load operation.          |
 * | `BackendApplyBinarySection` | Backend apply-binary-section operation.              |
 * | `ContextFinalize`           | Context finalize operation.                          |
 * | `ContextGetBinarySize`      | Context get-binary-size operation.                   |
 * | `ContextGetBinary`          | Context get-binary operation.                        |
 * | `ContextGetBinarySectionSize` | Context get-binary-section-size operation.         |
 * | `BackendFinalizeTensorUpdates` | Backend finalize operation after tensor updates.  |
 * | `BackendUpdateBinarySection` | Backend update-binary-section operation.            |
 */
QAIRT_CPPAPI_DEFINE_ENUM(
    SystemProfileMethodType,
    QairtSystem_Profile_MethodType_t,
    None                         = QAIRT_SYSTEM_PROFILE_METHOD_TYPE_NONE,
    BackendExecute               = QAIRT_SYSTEM_PROFILE_METHOD_TYPE_BACKEND_EXECUTE,
    BackendFinalize              = QAIRT_SYSTEM_PROFILE_METHOD_TYPE_BACKEND_FINALIZE,
    BackendExecuteAsync          = QAIRT_SYSTEM_PROFILE_METHOD_TYPE_BACKEND_EXECUTE_ASYNC,
    BackendCreateFromBinary      = QAIRT_SYSTEM_PROFILE_METHOD_TYPE_BACKEND_CREATE_FROM_BINARY,
    BackendDeinit                = QAIRT_SYSTEM_PROFILE_METHOD_TYPE_BACKEND_DEINIT,
    AppContextCreate             = QAIRT_SYSTEM_PROFILE_METHOD_TYPE_APP_CONTEXT_CREATE,
    AppComposeGraphs             = QAIRT_SYSTEM_PROFILE_METHOD_TYPE_APP_COMPOSE_GRAPHS,
    AppExecuteIps                = QAIRT_SYSTEM_PROFILE_METHOD_TYPE_APP_EXECUTE_IPS,
    BackendGraphComponent        = QAIRT_SYSTEM_PROFILE_METHOD_TYPE_BACKEND_GRAPH_COMPONENT,
    BackendLibLoad               = QAIRT_SYSTEM_PROFILE_METHOD_TYPE_APP_BACKEND_LIB_LOAD,
    BackendApplyBinarySection    = QAIRT_SYSTEM_PROFILE_METHOD_TYPE_BACKEND_APPLY_BINARY_SECTION,
    ContextFinalize              = QAIRT_SYSTEM_PROFILE_METHOD_TYPE_CONTEXT_FINALIZE,
    ContextGetBinarySize         = QAIRT_SYSTEM_PROFILE_METHOD_TYPE_CONTEXT_GET_BINARY_SIZE,
    ContextGetBinary             = QAIRT_SYSTEM_PROFILE_METHOD_TYPE_CONTEXT_GET_BINARY,
    ContextGetBinarySectionSize  = QAIRT_SYSTEM_PROFILE_METHOD_TYPE_CONTEXT_GET_BINARY_SECTION_SIZE,
    BackendFinalizeTensorUpdates = QAIRT_SYSTEM_PROFILE_METHOD_TYPE_BACKEND_FINALIZE_TENSOR_UPDATES,
    BackendUpdateBinarySection   = QAIRT_SYSTEM_PROFILE_METHOD_TYPE_BACKEND_UPDATE_BINARY_SECTION)

/**
 * @brief Format of the data payload carried by a system profile event.
 *
 * @details
 * | Enumerator          | Description                                  |
 * |---------------------|----------------------------------------------|
 * | `EventData`         | Standard event data payload.                 |
 * | `ExtendedEventData` | Extended event data with additional fields.  |
 * | `Undefined`         | Undefined or unrecognized data type.         |
 */
QAIRT_CPPAPI_DEFINE_ENUM(SystemProfileEventDataType,
                         QairtSystem_Profile_EventDataType_t,
                         EventData         = QAIRT_SYSTEM_PROFILE_EVENT_DATA,
                         ExtendedEventData = QAIRT_SYSTEM_PROFILE_EXTENDED_EVENT_DATA,
                         Undefined         = QAIRT_SYSTEM_PROFILE_EVENT_DATA_UNDEFINED)

/**
 * @brief Version of the profile data structure.
 *
 * @details
 * | Enumerator  | Description                                   |
 * |-------------|-----------------------------------------------|
 * | `V1`        | Profile data conforms to version 1 layout.    |
 * | `Undefined` | Undefined or unrecognized data version.       |
 */
QAIRT_CPPAPI_DEFINE_ENUM(SystemProfileDataVersion,
                         QairtSystem_Profile_DataVersion_t,
                         V1        = QAIRT_SYSTEM_PROFILE_DATA_VERSION_1,
                         Undefined = QAIRT_SYSTEM_PROFILE_DATA_VERSION_UNDEFINED)

/**
 * @brief Handle for a single system profile event header.
 *
 *        Obtained via `SystemApi::make<SystemProfileHeaderV1>()`.
 */
class SystemProfileHeaderV1
    : public ApiType<SystemProfileHeaderV1, QairtSystem_Profile_HeaderV1_t, SystemApiTable> {
  friend SystemApi;
  friend Api;

  /// @brief Graph name string, managed across the C API boundary.
  detail::crossable<std::string, &interface_type::getGraphName, &interface_type::setGraphName>
      m_graphName;

 public:
  using ApiType::ApiType;
  using ApiType::operator=;

  /**
   * @brief Get the profiling interval start timestamp.
   *
   * @return Start time in microseconds.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtSystem_Profile_Header_getStartTime
   */
  uint64_t getStartTime() const { return callGetter<&interface_type::getStartTime>(); }

  /**
   * @brief Set the profiling interval start timestamp.
   *
   * @param[in] startTime  Start time in microseconds.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtSystem_Profile_Header_setStartTime
   */
  void setStartTime(uint64_t startTime) { callSetter<&interface_type::setStartTime>(startTime); }

  /**
   * @brief Get the profiling interval stop timestamp.
   *
   * @return Stop time in microseconds.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtSystem_Profile_Header_getStopTime
   */
  uint64_t getStopTime() const { return callGetter<&interface_type::getStopTime>(); }

  /**
   * @brief Set the profiling interval stop timestamp.
   *
   * @param[in] stopTime  Stop time in microseconds.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtSystem_Profile_Header_setStopTime
   */
  void setStopTime(uint64_t stopTime) { callSetter<&interface_type::setStopTime>(stopTime); }

  /**
   * @brief Get the memory usage at the start of the profiling interval.
   *
   * @return Start memory in bytes.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtSystem_Profile_Header_getStartMem
   */
  uint64_t getStartMem() const { return callGetter<&interface_type::getStartMem>(); }

  /**
   * @brief Set the memory usage at the start of the profiling interval.
   *
   * @param[in] startMem  Start memory in bytes.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtSystem_Profile_Header_setStartMem
   */
  void setStartMem(uint64_t startMem) { callSetter<&interface_type::setStartMem>(startMem); }

  /**
   * @brief Get the memory usage at the end of the profiling interval.
   *
   * @return Stop memory in bytes.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtSystem_Profile_Header_getStopMem
   */
  uint64_t getStopMem() const { return callGetter<&interface_type::getStopMem>(); }

  /**
   * @brief Set the memory usage at the end of the profiling interval.
   *
   * @param[in] stopMem  Stop memory in bytes.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtSystem_Profile_Header_setStopMem
   */
  void setStopMem(uint64_t stopMem) { callSetter<&interface_type::setStopMem>(stopMem); }

  /**
   * @brief Get the operation type associated with this profiling interval.
   *
   * @return The method type enumerator for this header.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtSystem_Profile_Header_getMethodType
   */
  SystemProfileMethodType getMethodType() const {
    return static_cast<SystemProfileMethodType>(callGetter<&interface_type::getMethodType>());
  }

  /**
   * @brief Set the operation type associated with this profiling interval.
   *
   * @param[in] methodType  Method type enumerator.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtSystem_Profile_Header_setMethodType
   */
  void setMethodType(SystemProfileMethodType methodType) {
    callSetter<&interface_type::setMethodType>(
        static_cast<QairtSystem_Profile_MethodType_t>(methodType));
  }

  /**
   * @brief Get the visibility classification for this profiling interval.
   *
   * @return The visibility enumerator for this header.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtSystem_Profile_Header_getVisibility
   */
  SystemProfileVisibility getVisibility() const {
    return static_cast<SystemProfileVisibility>(callGetter<&interface_type::getVisibility>());
  }

  /**
   * @brief Set the visibility classification for this profiling interval.
   *
   * @param[in] visibility  Visibility enumerator.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtSystem_Profile_Header_setVisibility
   */
  void setVisibility(SystemProfileVisibility visibility) {
    callSetter<&interface_type::setVisibility>(
        static_cast<QairtSystem_Profile_Visibility_t>(visibility));
  }

  /**
   * @brief Get the graph name associated with this profiling interval.
   *
   * @return The graph name string.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtSystem_Profile_Header_getGraphName
   */
  std::string& getGraphName() { return m_graphName.get(); }

  /**
   * @brief Wrapper which allows for const access to the graph name string.
   *
   * @see SystemProfileHeaderV1::getGraphName()
   */
  const std::string& getGraphName() const { return m_graphName.get(); }

  /**
   * @brief Set the graph name associated with this profiling interval.
   *
   * @param[in] graphName  Name of the graph.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtSystem_Profile_Header_setGraphName
   */
  void setGraphName(const std::string& graphName) { m_graphName = graphName; }

 private:
  void prepareToCross() const {
    // Only prepare graph name if it's been set (non-empty)
    const std::string& graphName = m_graphName.get();
    if (!graphName.empty()) {
      m_graphName.prepareToCross(*this);
    }
  }
  void updateAfterCross() const { m_graphName.updateAfterCross(*this); }

  explicit SystemProfileHeaderV1(const std::shared_ptr<SystemApiTable>& apiTable)
      : ApiType(apiTable) {
    construct();
  }
};

/**
 * @brief handle for a single system profile event record.
 *
 *        Obtained via `SystemApi::make<SystemProfileEventV1>()`. Carries an
 *        event data payload and an optional list of nested sub-events. The
 */
class SystemProfileEventV1
    : public ApiType<SystemProfileEventV1, QairtSystem_Profile_EventV1_t, SystemApiTable> {
  friend SystemApi;

  /// @brief Profile event data associated with this system profile event.
  detail::crossable<detail::non_owning<ProfileEventData>,
                    &interface_type::getEventData,
                    &interface_type::setEventData>
      m_eventData;

  /// @brief Nested sub-events belonging to this event.
  mutable std::vector<SystemProfileEventV1> m_subEvents;

 public:
  using ApiType::ApiType;
  using ApiType::operator=;

  // Constructor that takes ownership of an existing handle (for getSubEventAt/getEventAt)
  /**
   * @brief Construct a system profile event from an existing C API event handle.
   *
   * Used internally to wrap handles returned by getSubEventAt() and getEventAt().
   *
   * @param[in] apiTable  System API table for this event.
   * @param[in] handle    Existing C API event handle to adopt.
   */
  SystemProfileEventV1(const std::shared_ptr<SystemApiTable>& apiTable,
                       QairtSystem_Profile_EventHandle_t handle)
      : ApiType(apiTable) {
    setHandle(handle);
  }

  /**
   * @brief Get the event data format classification for this profile event.
   *
   * @return The event data type enumerator.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtSystem_Profile_Event_getEventDataType
   */
  SystemProfileEventDataType getEventDataType() const {
    return static_cast<SystemProfileEventDataType>(callGetter<&interface_type::getEventDataType>());
  }

  /**
   * @brief Set the event data payload for this profile event.
   *
   * @param[in] eventData  Profile event data to associate with this event.
   *
   * @throws qairt::Exception on invalid handle or invalid argument.
   *
   * @see QairtSystem_Profile_Event_setEventData
   */
  void setEventData(const ProfileEventData& eventData) {
    QAIRT_NO_RETURN m_eventData.setNonOwning(*this, eventData);
  }

  /**
   * @brief Get the event data payload associated with this profile event.
   *
   * @return Reference to the associated ProfileEventData.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtSystem_Profile_Event_getEventData
   */
  ProfileEventData& getEventData() { return m_eventData.bindNonOwning(*this); }

  /**
   * @brief Wrapper which allows for const access to the event data payload.
   *
   * @see SystemProfileEventV1::getEventData()
   */
  const ProfileEventData& getEventData() const { return m_eventData.bindNonOwning(*this); }

  /**
   * @brief Append a sub-event to this profile event's sub-event list.
   *
   * @param[in] subEvent  Sub-event to append. Ownership is transferred.
   *
   * @see QairtSystem_Profile_Event_addSubEvent
   */
  void addSubEvent(SystemProfileEventV1&& subEvent) { m_subEvents.push_back(std::move(subEvent)); }

  /**
   * @brief Get the list of sub-events for this profile event.
   *
   * @return Reference to the sub-event vector.
   *
   * @see QairtSystem_Profile_Event_getNumSubEvents
   */
  std::vector<SystemProfileEventV1>& getSubEventData() { return m_subEvents; }

  /**
   * @brief Wrapper which allows for const access to the sub-event list.
   *
   * @see SystemProfileEventV1::getSubEventData()
   */
  const std::vector<SystemProfileEventV1>& getSubEventData() const { return m_subEvents; }

  /**
   * @brief Replace the entire sub-event list for this profile event.
   *
   * @param[in] subEvents  Replacement sub-event vector. Ownership is transferred.
   *
   * @see QairtSystem_Profile_Event_setSubEventData
   */
  void setSubEventData(std::vector<SystemProfileEventV1>&& subEvents) {
    m_subEvents = std::move(subEvents);
  }

 private:
  void prepareToCross() const {
    m_eventData.prepareToCross(*this);
    if (!m_subEvents.empty()) {
      std::vector<QairtSystem_Profile_EventHandle_t> subEventHandles;
      subEventHandles.reserve(m_subEvents.size());

      for (auto& subEvent : m_subEvents) {
        detail::QairtTypeAdmin::prepareToCross(subEvent);
        subEventHandles.push_back(detail::QairtTypeAdmin::getHandle(subEvent));
      }
      callSetter<&interface_type::setSubEventData>(subEventHandles.data(),
                                                   static_cast<uint32_t>(subEventHandles.size()));
    }
  }

  void updateAfterCross() const {
    m_eventData.updateAfterCross(*this);
    uint32_t numSubEvents{0u};
    callAndConfirm<&interface_type::getNumSubEvents>(&numSubEvents);
    if (numSubEvents > 0) {
      m_subEvents.resize(numSubEvents);
      for (uint32_t i = 0; i < numSubEvents; i++) {
        QairtSystem_Profile_EventHandle_t subEventHandle = nullptr;
        callAndConfirm<&interface_type::getSubEventAt>(i, &subEventHandle);
        m_subEvents[i] = create<SystemProfileEventV1>(subEventHandle);
        detail::QairtTypeAdmin::updateAfterCross(m_subEvents[i]);
      }
    }
  }

  explicit SystemProfileEventV1(const std::shared_ptr<SystemApiTable>& apiTable)
      : ApiType(apiTable) {
    construct();
  }
};

/**
 * @brief handle for a version-1 system profile data record.
 *
 *        Obtained via `SystemApi::make<SystemProfileDataV1>()`. Aggregates a
 *        SystemProfileHeaderV1 and an ordered list of SystemProfileEventV1 records
 *        for one profiling session.
 */
class SystemProfileDataV1
    : public ApiType<SystemProfileDataV1, QairtSystem_Profile_ProfileDataV1_t, SystemApiTable> {
  friend SystemApi;
  friend Api;

  /// @brief Profile header associated with this profile data record.
  detail::crossable<detail::non_owning<SystemProfileHeaderV1>,
                    &interface_type::getHeader,
                    &interface_type::setHeader>
      m_header;

  /// @brief Profiling events contained in this profile data record.
  mutable std::vector<SystemProfileEventV1> m_events;

 public:
  using ApiType::ApiType;
  using ApiType::operator=;

  /**
   * @brief Set the profile header for this profile data record.
   *
   * @param[in] header  Profile header to associate with this record.
   *
   * @throws qairt::Exception on invalid handle or invalid argument.
   *
   * @see QairtSystem_Profile_ProfileData_setHeader
   */
  void setHeader(const SystemProfileHeaderV1& header) {
    QAIRT_NO_RETURN m_header.setNonOwning(*this, header);
  }

  /**
   * @brief Get the profile header associated with this profile data record.
   *
   * @return Reference to the associated SystemProfileHeaderV1.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtSystem_Profile_ProfileData_getHeader
   */
  SystemProfileHeaderV1& getHeader() { return m_header.bindNonOwning(*this); }

  /**
   * @brief Wrapper which allows for const access to the profile header.
   *
   * @see SystemProfileDataV1::getHeader()
   */
  const SystemProfileHeaderV1& getHeader() const { return m_header.bindNonOwning(*this); }

  /**
   * @brief Replace the entire profiling event list for this profile data record.
   *
   * @param[in] events  Replacement event vector. Ownership is transferred.
   *
   * @see QairtSystem_Profile_ProfileData_setEvents
   */
  void setEvents(std::vector<SystemProfileEventV1>&& events) { m_events = std::move(events); }

  /**
   * @brief Get the list of profiling events for this profile data record.
   *
   * @return Reference to the event vector.
   *
   * @note Releasing this profile data handle invalidates memory returned by getEvents().
   *
   * @see QairtSystem_Profile_ProfileData_getNumProfilingEvents
   */
  std::vector<SystemProfileEventV1>& getEvents() { return m_events; }

  /**
   * @brief Wrapper which allows for const access to the profiling event list.
   *
   * @see SystemProfileDataV1::getEvents()
   */
  const std::vector<SystemProfileEventV1>& getEvents() const { return m_events; }

  /**
   * @brief Append a profiling event to this profile data record.
   *
   * @param[in] event  Profile event to append. Ownership is transferred.
   *
   * @see QairtSystem_Profile_ProfileData_addEvent
   */
  void addEvent(SystemProfileEventV1&& event) { m_events.push_back(std::move(event)); }

 private:
  void prepareToCross() const {
    m_header.prepareToCross(*this);
    if (!m_events.empty()) {
      std::vector<QairtSystem_Profile_EventHandle_t> eventHandles;
      eventHandles.reserve(m_events.size());

      for (auto& event : m_events) {
        detail::QairtTypeAdmin::prepareToCross(event);
        eventHandles.push_back(detail::QairtTypeAdmin::getHandle(event));
      }
      callSetter<&interface_type::setEvents>(eventHandles.data(),
                                             static_cast<uint32_t>(eventHandles.size()));
    }
  }
  void updateAfterCross() const {
    m_header.updateAfterCross(*this);
    uint32_t numProfilingEvents{0u};
    callAndConfirm<&interface_type::getNumProfilingEvents>(&numProfilingEvents);
    if (numProfilingEvents > 0) {
      m_events.resize(numProfilingEvents);
      for (uint32_t i = 0; i < numProfilingEvents; i++) {
        QairtSystem_Profile_EventHandle_t eventHandle = nullptr;
        callAndConfirm<&interface_type::getEventAt>(i, &eventHandle);
        m_events[i] = create<SystemProfileEventV1>(eventHandle);
        detail::QairtTypeAdmin::updateAfterCross(m_events[i]);
      }
    }
  }

  explicit SystemProfileDataV1(const std::shared_ptr<SystemApiTable>& apiTable)
      : ApiType(apiTable) {
    construct();
  }
};

/**
 * @brief versioned container for profile data payloads.
 *
 *        Obtained via `SystemApi::make<SystemProfileData>()`. Wraps a versioned
 *        profile data record (currently SystemProfileDataV1) for transport to a
 *        SystemProfileSerializationTarget. The container handle is freed
 *        automatically when this object is destroyed.
 */
class SystemProfileData
    : public ApiType<SystemProfileData, QairtSystem_Profile_DataV1_t, SystemApiTable> {
  friend SystemApi;

  /// @brief Version 1 profile data payload carried by this data container.
  detail::crossable<detail::non_owning<SystemProfileDataV1>,
                    &interface_type::getProfileDataV1,
                    &interface_type::setProfileDataV1>
      m_profileDataV1;

 public:
  using ApiType::ApiType;
  using ApiType::operator=;

  /**
   * @brief Get the profile data version carried by this container.
   *
   * @return The data version enumerator.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtSystem_Profile_Data_getVersion
   */
  SystemProfileDataVersion getVersion() const {
    return static_cast<SystemProfileDataVersion>(callGetter<&interface_type::getVersion>());
  }

  /**
   * @brief Set the version 1 profile data payload for this container.
   *
   * @param[in] profileData  Version 1 profile data record to associate.
   *
   * @throws qairt::Exception on invalid handle or invalid argument.
   *
   * @see QairtSystem_Profile_Data_setProfileDataV1
   */
  void setProfileDataV1(const SystemProfileDataV1& profileData) {
    QAIRT_NO_RETURN m_profileDataV1.setNonOwning(*this, profileData);
  }

  /**
   * @brief Get the version 1 profile data payload from this container.
   *
   * @return Reference to the associated SystemProfileDataV1 record.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtSystem_Profile_Data_getProfileDataV1
   */
  SystemProfileDataV1& getProfileDataV1() { return m_profileDataV1.bindNonOwning(*this); }

  /**
   * @brief Wrapper which allows for const access to the version 1 profile data payload.
   *
   * @see SystemProfileData::getProfileDataV1()
   */
  const SystemProfileDataV1& getProfileDataV1() const {
    return m_profileDataV1.bindNonOwning(*this);
  }

 private:
  void prepareToCross() const { m_profileDataV1.prepareToCross(*this); }
  void updateAfterCross() const { m_profileDataV1.updateAfterCross(*this); }

  explicit SystemProfileData(const std::shared_ptr<SystemApiTable>& apiTable) : ApiType(apiTable) {
    construct();
  }
};

/**
 * @brief handle for the file-level metadata written at the top of a serialized profile file.
 *
 *        Obtained via `SystemApi::make<SystemProfileSerializationFileHeader>()`. Stores the
 *        application name, application version, and backend version strings that appear
 *        in the profile output file. The handle is
 */
class SystemProfileSerializationFileHeader
    : public ApiType<SystemProfileSerializationFileHeader,
                     QairtSystem_Profile_SerializationFileHeaderV1_t,
                     SystemApiTable> {
  friend SystemApi;
  friend Api;

  /// @brief Application name written into the serialization file header.
  detail::crossable<std::string, &interface_type::getAppName, &interface_type::setAppName>
      m_appName;
  /// @brief Application version string written into the serialization file header.
  detail::crossable<std::string, &interface_type::getAppVersion, &interface_type::setAppVersion>
      m_appVersion;
  /// @brief Backend version string written into the serialization file header.
  detail::
      crossable<std::string, &interface_type::getBackendVersion, &interface_type::setBackendVersion>
          m_backendVersion;

 public:
  using ApiType::ApiType;
  using ApiType::operator=;

  /**
   * @brief Get the application name stored in this serialization file header.
   *
   * @return The application name string.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtSystem_Profile_SerializationFileHeader_getAppName
   */
  const std::string& getAppName() const { return m_appName.get(); }

  /**
   * @brief Set the application name in this serialization file header.
   *
   * @param[in] appName  Application name string.
   *
   * @throws qairt::Exception on invalid handle or NULL argument.
   *
   * @see QairtSystem_Profile_SerializationFileHeader_setAppName
   */
  void setAppName(const std::string& appName) { m_appName = appName; }

  /**
   * @brief Get the application version stored in this serialization file header.
   *
   * @return The application version string.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtSystem_Profile_SerializationFileHeader_getAppVersion
   */
  const std::string& getAppVersion() const { return m_appVersion.get(); }

  /**
   * @brief Set the application version in this serialization file header.
   *
   * @param[in] appVersion  Application version string.
   *
   * @throws qairt::Exception on invalid handle or NULL argument.
   *
   * @see QairtSystem_Profile_SerializationFileHeader_setAppVersion
   */
  void setAppVersion(const std::string& appVersion) { m_appVersion = appVersion; }

  /**
   * @brief Get the backend version stored in this serialization file header.
   *
   * @return The backend version string.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtSystem_Profile_SerializationFileHeader_getBackendVersion
   */
  const std::string& getBackendVersion() const { return m_backendVersion.get(); }

  /**
   * @brief Set the backend version in this serialization file header.
   *
   * @param[in] backendVersion  Backend version string.
   *
   * @throws qairt::Exception on invalid handle or NULL argument.
   *
   * @see QairtSystem_Profile_SerializationFileHeader_setBackendVersion
   */
  void setBackendVersion(const std::string& backendVersion) { m_backendVersion = backendVersion; }

 private:
  void prepareToCross() const {
    m_appName.prepareToCross(*this);
    m_appVersion.prepareToCross(*this);
    m_backendVersion.prepareToCross(*this);
  }

  void updateAfterCross() const {
    m_appName.updateAfterCross(*this);
    m_appVersion.updateAfterCross(*this);
    m_backendVersion.updateAfterCross(*this);
  }

  explicit SystemProfileSerializationFileHeader(const std::shared_ptr<SystemApiTable>& apiTable)
      : ApiType(apiTable) {
    construct();
  }
};

/**
 * @brief handle for the configuration options of a profile serialization target.
 *
 *        Obtained via `SystemApi::make<SystemProfileSerializationTargetConfiguration>()`.
 *        Specifies the maximum number of messages and the file header for a
 *        serialization target. The handle is
 */
class SystemProfileSerializationTargetConfiguration
    : public ApiType<SystemProfileSerializationTargetConfiguration,
                     QairtSystem_Profile_SerializationTargetConfigurationV1_t,
                     SystemApiTable> {
  friend SystemApi;
  friend Api;

  /// @brief Serialization file header associated with this target configuration.
  detail::crossable<detail::non_owning<SystemProfileSerializationFileHeader>,
                    &interface_type::getFileHeader,
                    &interface_type::setFileHeader>
      m_fileHeader;

 public:
  using ApiType::ApiType;
  using ApiType::operator=;

  /**
   * @brief Set the maximum number of messages this serialization target will write.
   *
   * @param[in] maxNumMessages  Maximum number of messages.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtSystem_Profile_SerializationTargetConfiguration_setMaxNumMessages
   */
  void setMaxNumMessages(uint32_t maxNumMessages) {
    callSetter<&interface_type::setMaxNumMessages>(maxNumMessages);
  }

  /**
   * @brief Get the maximum number of messages this serialization target will write.
   *
   * @return Maximum message count.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtSystem_Profile_SerializationTargetConfiguration_getMaxNumMessages
   */
  uint32_t getMaxNumMessages() const { return callGetter<&interface_type::getMaxNumMessages>(); }

  /**
   * @brief Set the serialization file header for this target configuration.
   *
   * @param[in] fileHeader  Serialization file header to associate.
   *
   * @throws qairt::Exception on invalid handle or invalid argument.
   *
   * @see QairtSystem_Profile_SerializationTargetConfiguration_setFileHeader
   */
  void setFileHeader(const SystemProfileSerializationFileHeader& fileHeader) {
    QAIRT_NO_RETURN m_fileHeader.setNonOwning(*this, fileHeader);
  }

  /**
   * @brief Get the serialization file header associated with this target configuration.
   *
   * @return Reference to the associated SystemProfileSerializationFileHeader.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtSystem_Profile_SerializationTargetConfiguration_getFileHeader
   */
  SystemProfileSerializationFileHeader& getFileHeader() {
    return m_fileHeader.bindNonOwning(*this);
  }

  /**
   * @brief Wrapper which allows for const access to the serialization file header.
   *
   * @see SystemProfileSerializationTargetConfiguration::getFileHeader()
   */
  const SystemProfileSerializationFileHeader& getFileHeader() const {
    return m_fileHeader.bindNonOwning(*this);
  }

 private:
  void prepareToCross() const { m_fileHeader.prepareToCross(*this); }

  void updateAfterCross() const { m_fileHeader.updateAfterCross(*this); }

  explicit SystemProfileSerializationTargetConfiguration(
      const std::shared_ptr<SystemApiTable>& apiTable)
      : ApiType(apiTable) {
    construct();
  }
};

/**
 * @brief handle for a profile output file descriptor used by a serialization target.
 *
 *        Obtained via `SystemApi::make<SystemProfileSerializationTargetFile>()`.
 *        Stores the output file name and directory for a serialized profile file.
 */
class SystemProfileSerializationTargetFile
    : public ApiType<SystemProfileSerializationTargetFile,
                     QairtSystem_Profile_SerializationTargetFileV1_t,
                     SystemApiTable> {
  friend SystemApi;
  friend Api;

  /// @brief Output file name for the serialization target.
  detail::crossable<std::string, &interface_type::getFileName, &interface_type::setFileName>
      m_fileName;
  /// @brief Output directory for the serialization target file.
  detail::
      crossable<std::string, &interface_type::getFileDirectory, &interface_type::setFileDirectory>
          m_fileDirectory;

 public:
  using ApiType::ApiType;
  using ApiType::operator=;

  /**
   * @brief Get the output file name stored in this serialization target file.
   *
   * @return The output file name string.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtSystem_Profile_SerializationTargetFile_getFileName
   */
  const std::string& getFileName() const { return m_fileName.get(); }

  /**
   * @brief Set the output file name for this serialization target file.
   *
   * @param[in] fileName  Output file name string.csv".
   *
   * @throws qairt::Exception on invalid handle or NULL argument.
   *
   * @see QairtSystem_Profile_SerializationTargetFile_setFileName
   */
  void setFileName(const std::string& fileName) { m_fileName = fileName; }

  /**
   * @brief Get the output directory stored in this serialization target file.
   *
   * @return The output directory path string.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtSystem_Profile_SerializationTargetFile_getFileDirectory
   */
  const std::string& getFileDirectory() const { return m_fileDirectory.get(); }

  /**
   * @brief Set the output directory for this serialization target file.
   *
   * @param[in] fileDirectory  Output directory path string.
   *
   * @throws qairt::Exception on invalid handle or NULL argument.
   *
   * @see QairtSystem_Profile_SerializationTargetFile_setFileDirectory
   */
  void setFileDirectory(const std::string& fileDirectory) { m_fileDirectory = fileDirectory; }

 private:
  void prepareToCross() const {
    m_fileName.prepareToCross(*this);
    m_fileDirectory.prepareToCross(*this);
  }

  void updateAfterCross() const {
    m_fileName.updateAfterCross(*this);
    m_fileDirectory.updateAfterCross(*this);
  }

  explicit SystemProfileSerializationTargetFile(const std::shared_ptr<SystemApiTable>& apiTable)
      : ApiType(apiTable) {
    construct();
  }
};

/**
 * @brief handle for the type and output destination of a profile serialization target.
 *
 *        Obtained via `SystemApi::make<SystemProfileSerializationTargetInfo>()`.
 *        Associates a serialization type with a SystemProfileSerializationTargetFile.
 */
class SystemProfileSerializationTargetInfo
    : public ApiType<SystemProfileSerializationTargetInfo,
                     QairtSystem_Profile_SerializationTargetInfoV1_t,
                     SystemApiTable> {
  friend SystemApi;
  friend Api;

  /// @brief Target file descriptor associated with this serialization target info.
  detail::crossable<detail::non_owning<SystemProfileSerializationTargetFile>,
                    &interface_type::getTargetFile,
                    &interface_type::setTargetFile>
      m_targetFile;

 public:
  using ApiType::ApiType;
  using ApiType::operator=;

  /**
   * @brief Set the target file descriptor for this serialization target info.
   *
   * @param[in] targetFile  Serialization target file to associate.
   *
   * @throws qairt::Exception on invalid handle or invalid argument.
   *
   * @see QairtSystem_Profile_SerializationTargetInfo_setTargetFile
   */
  void setTargetFile(const SystemProfileSerializationTargetFile& targetFile) {
    QAIRT_NO_RETURN m_targetFile.setNonOwning(*this, targetFile);
  }

  /**
   * @brief Get the target file descriptor associated with this serialization target info.
   *
   * @return Reference to the associated SystemProfileSerializationTargetFile.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtSystem_Profile_SerializationTargetInfo_getTargetFile
   */
  SystemProfileSerializationTargetFile& getTargetFile() {
    return m_targetFile.bindNonOwning(*this);
  }

  /**
   * @brief Wrapper which allows for const access to the target file descriptor.
   *
   * @see SystemProfileSerializationTargetInfo::getTargetFile()
   */
  const SystemProfileSerializationTargetFile& getTargetFile() const {
    return m_targetFile.bindNonOwning(*this);
  }

 private:
  void prepareToCross() const { m_targetFile.prepareToCross(*this); }

  void updateAfterCross() const { m_targetFile.updateAfterCross(*this); }

  explicit SystemProfileSerializationTargetInfo(const std::shared_ptr<SystemApiTable>& apiTable)
      : ApiType(apiTable) {
    construct();
  }
};

/**
 * @brief handle for a profile data serialization target.
 *
 *        Obtained via `SystemApi::make<SystemProfileSerializationTarget>(info, config)`.
 *        Call serializeEventData() to write a collection of SystemProfileData records
 *        to the configured output destination.
 */
class SystemProfileSerializationTarget : public ApiType<SystemProfileSerializationTarget,
                                                        QairtSystem_Profile_SerializationTargetV1_t,
                                                        SystemApiTable> {
  friend SystemApi;

 public:
  using ApiType::ApiType;
  using ApiType::operator=;

  /**
   * @brief Serialize a collection of profile data records to the configured output destination.
   *
   * Writes each SystemProfileData record in _eventData_ to the destination
   * specified when this target was created. The output format and destination
   * are determined by the SystemProfileSerializationTargetInfo and
   * SystemProfileSerializationTargetConfiguration passed to the constructor.
   *
   * @param[in] eventData  Profile data records to serialize. Must be non-empty.
   *
   * @throws qairt::Exception on:
   *          - invalid handle
   *          - empty or NULL event data array
   *          - serialization failure
   *
   * @see QairtSystem_Profile_SerializationTarget_serializeEventData
   */
  void serializeEventData(std::vector<SystemProfileData>& eventData) {
    std::vector<QairtSystem_Profile_DataHandle_t> eventDataHandles;
    eventDataHandles.reserve(eventData.size());

    for (auto& event : eventData) {
      detail::QairtTypeAdmin::prepareToCross(event);
      eventDataHandles.push_back(detail::QairtTypeAdmin::getHandle(event));
    }

    callAndConfirm<&interface_type::serializeEventData>(
        eventDataHandles.data(), static_cast<uint32_t>(eventDataHandles.size()));
  }

 private:
  void prepareToCross() const {}

  void updateAfterCross() const {}

  SystemProfileSerializationTarget(const std::shared_ptr<SystemApiTable>& apiTable,
                                   const SystemProfileSerializationTargetInfo& info,
                                   const SystemProfileSerializationTargetConfiguration& config)
      : ApiType(apiTable) {
    detail::QairtTypeAdmin::prepareToCross(info);
    detail::QairtTypeAdmin::prepareToCross(config);
    construct(getHandle(info), getHandle(config));
  }
};

}  // namespace qairt

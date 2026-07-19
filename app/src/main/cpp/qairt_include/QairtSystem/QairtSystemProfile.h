//==============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//==============================================================================

/**
 * @file
 * @brief   QAIRT System Profile component API
 *
 *          A header which contains the QAIRT System Profile
 *          components and the interfaces it implements.
 */

#ifndef QAIRT_SYSTEM_PROFILE_H
#define QAIRT_SYSTEM_PROFILE_H

#include "QairtCommon/QairtCommon.h"
#include "QairtProfile/QairtProfile.h"
#include "QairtProfile/QairtProfileEvent.h"
#include "QairtSystem/QairtSystemCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
// Macros
//=============================================================================
/**
 * @brief ID for System Profile Serialization Target V1 interface
 */
#define QAIRT_SYSTEM_PROFILE_SERIALIZATION_TARGET_V1_ID QAIRT_MIN_ID_SYSTEM_PROFILE
/**
 * @brief ID for System Profile Header V1 interface
 */
#define QAIRT_SYSTEM_PROFILE_HEADER_V1_ID QAIRT_MIN_ID_SYSTEM_PROFILE + 1
/**
 * @brief ID for System Profile Event V1 interface
 */
#define QAIRT_SYSTEM_PROFILE_EVENT_V1_ID QAIRT_MIN_ID_SYSTEM_PROFILE + 2
/**
 * @brief ID for System Profile Profile Data V1 interface
 */
#define QAIRT_SYSTEM_PROFILE_PROFILE_DATA_V1_ID QAIRT_MIN_ID_SYSTEM_PROFILE + 3
/**
 * @brief ID for System Profile Data V1 interface
 */
#define QAIRT_SYSTEM_PROFILE_DATA_V1_ID QAIRT_MIN_ID_SYSTEM_PROFILE + 4
/**
 * @brief ID for System Profile Serialization File Header V1 interface
 */
#define QAIRT_SYSTEM_PROFILE_SERIALIZATION_FILE_HEADER_V1_ID QAIRT_MIN_ID_SYSTEM_PROFILE + 5
/**
 * @brief ID for System Profile Serialization Target Configuration V1 interface
 */
#define QAIRT_SYSTEM_PROFILE_SERIALIZATION_TARGET_CONFIGURATION_V1_ID \
  QAIRT_MIN_ID_SYSTEM_PROFILE + 6
/**
 * @brief ID for System Profile Serialization Target File V1 interface
 */
#define QAIRT_SYSTEM_PROFILE_SERIALIZATION_TARGET_FILE_V1_ID QAIRT_MIN_ID_SYSTEM_PROFILE + 7
/**
 * @brief ID for System Profile Serialization Target Info V1 interface
 */
#define QAIRT_SYSTEM_PROFILE_SERIALIZATION_TARGET_INFO_V1_ID QAIRT_MIN_ID_SYSTEM_PROFILE + 8

//=============================================================================
// Data Types
//=============================================================================
/**
 * @brief Handle to a System Profile Serialization Target object
 */
typedef struct _QairtSystem_Profile_SerializationTargetHandle_t*
    QairtSystem_Profile_SerializationTargetHandle_t;

/**
 * @brief Handle to a System Profile Header object
 */
typedef struct _QairtSystem_Profile_HeaderHandle_t* QairtSystem_Profile_HeaderHandle_t;

/**
 * @brief Handle to a System Profile Event object
 */
typedef struct _QairtSystem_Profile_EventHandle_t* QairtSystem_Profile_EventHandle_t;

/**
 * @brief Handle to a System Profile Profile Data object
 */
typedef struct _QairtSystem_Profile_ProfileDataHandle_t*
    QairtSystem_Profile_ProfileDataHandle_t;

/**
 * @brief Handle to a System Profile Data object
 */
typedef struct _QairtSystem_Profile_DataHandle_t* QairtSystem_Profile_DataHandle_t;

/**
 * @brief Handle to a System Profile Serialization File Header object
 */
typedef struct _QairtSystem_Profile_SerializationFileHeaderHandle_t*
    QairtSystem_Profile_SerializationFileHeaderHandle_t;

/**
 * @brief Handle to a System Profile Serialization Target Configuration object
 */
typedef struct _QairtSystem_Profile_SerializationTargetConfigurationHandle_t*
    QairtSystem_Profile_SerializationTargetConfigurationHandle_t;

/**
 * @brief Handle to a System Profile Serialization Target File object
 */
typedef struct _QairtSystem_Profile_SerializationTargetFileHandle_t*
    QairtSystem_Profile_SerializationTargetFileHandle_t;

/**
 * @brief Handle to a System Profile Serialization Target Info object
 */
typedef struct _QairtSystem_Profile_SerializationTargetInfoHandle_t*
    QairtSystem_Profile_SerializationTargetInfoHandle_t;

/**
 * @brief QAIRT System Profile API result / error codes.
 */
typedef enum {
  QAIRT_SYSTEM_PROFILE_MINERROR = QAIRT_SYSTEM_PROFILE_MIN_ERROR,
  //////////////////////////////////////////

  /// QAIRT System Profile success
  QAIRT_SYSTEM_PROFILE_NO_ERROR = QAIRT_SYSTEM_COMMON_NO_ERROR,
  /// QAIRT System Profile API is not supported yet
  QAIRT_SYSTEM_PROFILE_ERROR_UNSUPPORTED_FEATURE = QAIRT_SYSTEM_COMMON_ERROR_UNSUPPORTED_FEATURE,
  /// QAIRT System Profile invalid handle
  QAIRT_SYSTEM_PROFILE_ERROR_INVALID_HANDLE = QAIRT_SYSTEM_COMMON_ERROR_INVALID_HANDLE,
  /// One or more arguments to a System Profile API is/are NULL/invalid.
  QAIRT_SYSTEM_PROFILE_ERROR_INVALID_ARGUMENT = QAIRT_SYSTEM_COMMON_ERROR_INVALID_ARGUMENT,
  /// QAIRT System Profile writer could not allocate memory properly
  QAIRT_SYSTEM_PROFILE_ERROR_MEM_ALLOC = 31000,
  /// QAIRT System Profile serialization failed
  QAIRT_SYSTEM_PROFILE_ERROR_SERIALIZATION_FAILED = 31001,

  //////////////////////////////////////////
  QAIRT_SYSTEM_PROFILE_MAXERROR = QAIRT_SYSTEM_PROFILE_MAX_ERROR
} QairtSystemProfile_Error_t;

/**
 * @brief Visibility type for system profile events
 */
typedef enum {
  /// Public visibility - event is visible to all consumers
  QAIRT_SYSTEM_PROFILE_VISIBILITY_PUBLIC = 0,
  /// Private visibility - event is internal/private
  QAIRT_SYSTEM_PROFILE_VISIBILITY_PRIVATE = 1,
  /// Unused, present to ensure 32 bits
  QAIRT_SYSTEM_PROFILE_VISIBILITY_UNDEFINED = 0x7FFFFFFF
} QairtSystem_Profile_Visibility_t;

/**
 * @brief Method type for system profile events indicating the operation being profiled
 */
typedef enum {
  /// No specific method type
  QAIRT_SYSTEM_PROFILE_METHOD_TYPE_NONE = 0,
  /// Backend execute operation
  QAIRT_SYSTEM_PROFILE_METHOD_TYPE_BACKEND_EXECUTE = 1,
  /// Backend finalize operation
  QAIRT_SYSTEM_PROFILE_METHOD_TYPE_BACKEND_FINALIZE = 2,
  /// Backend asynchronous execute operation
  QAIRT_SYSTEM_PROFILE_METHOD_TYPE_BACKEND_EXECUTE_ASYNC = 3,
  /// Backend create from binary operation
  QAIRT_SYSTEM_PROFILE_METHOD_TYPE_BACKEND_CREATE_FROM_BINARY = 4,
  /// Backend deinitialization operation
  QAIRT_SYSTEM_PROFILE_METHOD_TYPE_BACKEND_DEINIT = 5,
  /// Application context creation operation
  QAIRT_SYSTEM_PROFILE_METHOD_TYPE_APP_CONTEXT_CREATE = 6,
  /// Application compose graphs operation
  QAIRT_SYSTEM_PROFILE_METHOD_TYPE_APP_COMPOSE_GRAPHS = 7,
  /// Application execute IPS operation
  QAIRT_SYSTEM_PROFILE_METHOD_TYPE_APP_EXECUTE_IPS = 8,
  /// Backend graph component operation
  QAIRT_SYSTEM_PROFILE_METHOD_TYPE_BACKEND_GRAPH_COMPONENT = 9,
  /// Application backend library load operation
  QAIRT_SYSTEM_PROFILE_METHOD_TYPE_APP_BACKEND_LIB_LOAD = 10,
  /// Backend apply binary section operation
  QAIRT_SYSTEM_PROFILE_METHOD_TYPE_BACKEND_APPLY_BINARY_SECTION = 11,
  /// Context finalize operation
  QAIRT_SYSTEM_PROFILE_METHOD_TYPE_CONTEXT_FINALIZE = 12,
  /// Context get binary size operation
  QAIRT_SYSTEM_PROFILE_METHOD_TYPE_CONTEXT_GET_BINARY_SIZE = 13,
  /// Context get binary operation
  QAIRT_SYSTEM_PROFILE_METHOD_TYPE_CONTEXT_GET_BINARY = 14,
  /// Context get binary section size operation
  QAIRT_SYSTEM_PROFILE_METHOD_TYPE_CONTEXT_GET_BINARY_SECTION_SIZE = 15,
  /// Backend finalize operation performed after tensor updates
  QAIRT_SYSTEM_PROFILE_METHOD_TYPE_BACKEND_FINALIZE_TENSOR_UPDATES = 16,
  /// Backend update binary section operation
  QAIRT_SYSTEM_PROFILE_METHOD_TYPE_BACKEND_UPDATE_BINARY_SECTION = 17,
  /// Unused, present to ensure 32 bits
  QAIRT_SYSTEM_PROFILE_METHOD_TYPE_UNDEFINED = 0x7FFFFFFF
} QairtSystem_Profile_MethodType_t;

/**
 * @brief Event data type indicating the format of event data
 */
typedef enum {
  /// Standard event data
  QAIRT_SYSTEM_PROFILE_EVENT_DATA = 0,
  /// Extended event data with additional fields
  QAIRT_SYSTEM_PROFILE_EXTENDED_EVENT_DATA = 1,
  /// Unused, present to ensure 32 bits
  QAIRT_SYSTEM_PROFILE_EVENT_DATA_UNDEFINED = 0x7FFFFFFF
} QairtSystem_Profile_EventDataType_t;

/**
 * @brief Profile data version type
 */
typedef enum {
  /// Type containing profile data V1
  QAIRT_SYSTEM_PROFILE_DATA_VERSION_1 = 1,
  /// Unused, present to ensure 32 bits.
  QAIRT_SYSTEM_PROFILE_DATA_VERSION_UNDEFINED = 0x7FFFFFFF
} QairtSystem_Profile_DataVersion_t;

/**
 * @brief Serialization target type indicating where profile data is written
 */
typedef enum {
  /// Serialize to a file
  QAIRT_SYSTEM_PROFILE_SERIALIZATION_TARGET_TYPE_FILE = 0,
  /// Unused, present to ensure 32 bits
  QAIRT_SYSTEM_PROFILE_SERIALIZATION_TARGET_TYPE_UNDEFINED = 0x7FFFFFFF
} QairtSystem_Profile_SerializationTargetType_t;

//=============================================================================
// Public Functions
//=============================================================================

/**
 * @brief Create a system profile header object.
 *
 * @param[out] headerHandle A handle to the created system profile header.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Header was successfully created.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_ARGUMENT: _headerHandle_ is NULL.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_MEM_ALLOC: Memory allocation failed.
 *
 * @note Use corresponding API through QairtSystem_Profile_HeaderV1_t.
 */
Qairt_Status_t QairtSystem_Profile_Header_create(
    QairtSystem_Profile_HeaderHandle_t* headerHandle);

/**
 * @brief Free a system profile header and associated resources.
 *
 * @param[in] headerHandle A handle to a created system profile header.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Header was successfully freed.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_HANDLE: _headerHandle_ is not a valid handle.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_MEM_ALLOC: Memory deallocation error.
 *
 * @note Use corresponding API through QairtSystem_Profile_HeaderV1_t.
 */
Qairt_Status_t QairtSystem_Profile_Header_free(
    QairtSystem_Profile_HeaderHandle_t headerHandle);

/**
 * @brief Get the start time from a system profile header.
 *
 * @param[in] headerHandle A handle to a system profile header.
 *
 * @param[out] startTime The start time value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Start time was successfully retrieved.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_HANDLE: _headerHandle_ is not a valid handle.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_ARGUMENT: _startTime_ is NULL.
 *
 * @note Use corresponding API through QairtSystem_Profile_HeaderV1_t.
 */
Qairt_Status_t QairtSystem_Profile_Header_getStartTime(
    QairtSystem_Profile_HeaderHandle_t headerHandle, uint64_t* startTime);

/**
 * @brief Set the start time in a system profile header.
 *
 * @param[in] headerHandle A handle to a system profile header.
 *
 * @param[in] startTime The start time value to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Start time was successfully set.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_HANDLE: _headerHandle_ is not a valid handle.
 *
 * @note Use corresponding API through QairtSystem_Profile_HeaderV1_t.
 */
Qairt_Status_t QairtSystem_Profile_Header_setStartTime(
    QairtSystem_Profile_HeaderHandle_t headerHandle, uint64_t startTime);

/**
 * @brief Get the stop time from a system profile header.
 *
 * @param[in] headerHandle A handle to a system profile header.
 *
 * @param[out] stopTime The stop time value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Stop time was successfully retrieved.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_HANDLE: _headerHandle_ is not a valid handle.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_ARGUMENT: _stopTime_ is NULL.
 *
 * @note Use corresponding API through QairtSystem_Profile_HeaderV1_t.
 */
Qairt_Status_t QairtSystem_Profile_Header_getStopTime(
    QairtSystem_Profile_HeaderHandle_t headerHandle, uint64_t* stopTime);

/**
 * @brief Set the stop time in a system profile header.
 *
 * @param[in] headerHandle A handle to a system profile header.
 *
 * @param[in] stopTime The stop time value to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Stop time was successfully set.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_HANDLE: _headerHandle_ is not a valid handle.
 *
 * @note Use corresponding API through QairtSystem_Profile_HeaderV1_t.
 */
Qairt_Status_t QairtSystem_Profile_Header_setStopTime(
    QairtSystem_Profile_HeaderHandle_t headerHandle, uint64_t stopTime);

/**
 * @brief Get the start memory value from a system profile header.
 *
 * @param[in] headerHandle A handle to a system profile header.
 *
 * @param[out] startMem The start memory value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Start memory was successfully retrieved.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_HANDLE: _headerHandle_ is not a valid handle.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_ARGUMENT: _startMem_ is NULL.
 *
 * @note Use corresponding API through QairtSystem_Profile_HeaderV1_t.
 */
Qairt_Status_t QairtSystem_Profile_Header_getStartMem(
    QairtSystem_Profile_HeaderHandle_t headerHandle, uint64_t* startMem);

/**
 * @brief Set the start memory value in a system profile header.
 *
 * @param[in] headerHandle A handle to a system profile header.
 *
 * @param[in] startMem The start memory value to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Start memory was successfully set.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_HANDLE: _headerHandle_ is not a valid handle.
 *
 * @note Use corresponding API through QairtSystem_Profile_HeaderV1_t.
 */
Qairt_Status_t QairtSystem_Profile_Header_setStartMem(
    QairtSystem_Profile_HeaderHandle_t headerHandle, uint64_t startMem);

/**
 * @brief Get the stop memory value from a system profile header.
 *
 * @param[in] headerHandle A handle to a system profile header.
 *
 * @param[out] stopMem The stop memory value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Stop memory was successfully retrieved.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_HANDLE: _headerHandle_ is not a valid handle.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_ARGUMENT: _stopMem_ is NULL.
 *
 * @note Use corresponding API through QairtSystem_Profile_HeaderV1_t.
 */
Qairt_Status_t QairtSystem_Profile_Header_getStopMem(
    QairtSystem_Profile_HeaderHandle_t headerHandle, uint64_t* stopMem);

/**
 * @brief Set the stop memory value in a system profile header.
 *
 * @param[in] headerHandle A handle to a system profile header.
 *
 * @param[in] stopMem The stop memory value to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Stop memory was successfully set.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_HANDLE: _headerHandle_ is not a valid handle.
 *
 * @note Use corresponding API through QairtSystem_Profile_HeaderV1_t.
 */
Qairt_Status_t QairtSystem_Profile_Header_setStopMem(
    QairtSystem_Profile_HeaderHandle_t headerHandle, uint64_t stopMem);

/**
 * @brief Get the method type from a system profile header.
 *
 * @param[in] headerHandle A handle to a system profile header.
 *
 * @param[out] methodType The method type value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Method type was successfully retrieved.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_HANDLE: _headerHandle_ is not a valid handle.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_ARGUMENT: _methodType_ is NULL.
 *
 * @note Use corresponding API through QairtSystem_Profile_HeaderV1_t.
 */
Qairt_Status_t QairtSystem_Profile_Header_getMethodType(
    QairtSystem_Profile_HeaderHandle_t headerHandle,
    QairtSystem_Profile_MethodType_t* methodType);

/**
 * @brief Set the method type in a system profile header.
 *
 * @param[in] headerHandle A handle to a system profile header.
 *
 * @param[in] methodType The method type value to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Method type was successfully set.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_HANDLE: _headerHandle_ is not a valid handle.
 *
 * @note Use corresponding API through QairtSystem_Profile_HeaderV1_t.
 */
Qairt_Status_t QairtSystem_Profile_Header_setMethodType(
    QairtSystem_Profile_HeaderHandle_t headerHandle, QairtSystem_Profile_MethodType_t methodType);

/**
 * @brief Get the visibility from a system profile header.
 *
 * @param[in] headerHandle A handle to a system profile header.
 *
 * @param[out] visibility The visibility value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Visibility was successfully retrieved.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_HANDLE: _headerHandle_ is not a valid handle.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_ARGUMENT: _visibility_ is NULL.
 *
 * @note Use corresponding API through QairtSystem_Profile_HeaderV1_t.
 */
Qairt_Status_t QairtSystem_Profile_Header_getVisibility(
    QairtSystem_Profile_HeaderHandle_t headerHandle,
    QairtSystem_Profile_Visibility_t* visibility);

/**
 * @brief Set the visibility in a system profile header.
 *
 * @param[in] headerHandle A handle to a system profile header.
 *
 * @param[in] visibility The visibility value to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Visibility was successfully set.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_HANDLE: _headerHandle_ is not a valid handle.
 *
 * @note Use corresponding API through QairtSystem_Profile_HeaderV1_t.
 */
Qairt_Status_t QairtSystem_Profile_Header_setVisibility(
    QairtSystem_Profile_HeaderHandle_t headerHandle, QairtSystem_Profile_Visibility_t visibility);

/**
 * @brief Get the graph name from a system profile header.
 *
 * @param[in] headerHandle A handle to a system profile header.
 *
 * @param[out] graphName The graph name string.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Graph name was successfully retrieved.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_HANDLE: _headerHandle_ is not a valid handle.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_ARGUMENT: _graphName_ is NULL.
 *
 * @note Use corresponding API through QairtSystem_Profile_HeaderV1_t.
 */
Qairt_Status_t QairtSystem_Profile_Header_getGraphName(
    QairtSystem_Profile_HeaderHandle_t headerHandle, const char** graphName);

/**
 * @brief Set the graph name in a system profile header.
 *
 * @param[in] headerHandle A handle to a system profile header.
 *
 * @param[in] graphName The graph name string to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Graph name was successfully set.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_HANDLE: _headerHandle_ is not a valid handle.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_ARGUMENT: _graphName_ is NULL.
 *
 * @note Use corresponding API through QairtSystem_Profile_HeaderV1_t.
 */
Qairt_Status_t QairtSystem_Profile_Header_setGraphName(
    QairtSystem_Profile_HeaderHandle_t headerHandle, const char* graphName);

/**
 * @brief Create a system profile event object.
 *
 * @param[out] eventHandle A handle to the created system profile event.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Event was successfully created.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_ARGUMENT: _eventHandle_ is NULL.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_MEM_ALLOC: Memory allocation failed.
 *
 * @note Use corresponding API through QairtSystem_Profile_EventV1_t.
 */
Qairt_Status_t QairtSystem_Profile_Event_create(
    QairtSystem_Profile_EventHandle_t* eventHandle);

/**
 * @brief Free a system profile event and associated resources.
 *
 * @param[in] eventHandle A handle to a created system profile event.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Event was successfully freed.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_HANDLE: _eventHandle_ is not a valid handle.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_MEM_ALLOC: Memory deallocation error.
 *
 * @note Use corresponding API through QairtSystem_Profile_EventV1_t.
 */
Qairt_Status_t QairtSystem_Profile_Event_free(QairtSystem_Profile_EventHandle_t eventHandle);

/**
 * @brief Get the event data type from a system profile event.
 *
 * @param[in] eventHandle A handle to a system profile event.
 *
 * @param[out] eventDataType The event data type.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Event data type was successfully retrieved.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_HANDLE: _eventHandle_ is not a valid handle.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_ARGUMENT: _eventDataType_ is NULL.
 *
 * @note Use corresponding API through QairtSystem_Profile_EventV1_t.
 */
Qairt_Status_t QairtSystem_Profile_Event_getEventDataType(
    QairtSystem_Profile_EventHandle_t eventHandle,
    QairtSystem_Profile_EventDataType_t* eventDataType);

/**
 * @brief Set the event data in a system profile event.
 *
 * @param[in] eventHandle A handle to a system profile event.
 *
 * @param[in] profileEventHandle A handle to a profile event to associate with this system profile
 * event.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Event data was successfully set.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_HANDLE: _eventHandle_ or _profileEventHandle_ is not
 * a valid handle.
 *
 * @note Use corresponding API through QairtSystem_Profile_EventV1_t.
 */
Qairt_Status_t QairtSystem_Profile_Event_setEventData(
    QairtSystem_Profile_EventHandle_t eventHandle,
    QairtProfile_EventHandle_t profileEventHandle);

/**
 * @brief Get the event data from a system profile event.
 *
 * @param[in] event A handle to a system profile event.
 *
 * @param[out] profileEventHandle A handle to the associated profile event.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Event data was successfully retrieved.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_HANDLE: _event_ is not a valid handle.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_ARGUMENT: _profileEventHandle_ is NULL.
 *
 * @note Use corresponding API through QairtSystem_Profile_EventV1_t.
 */
Qairt_Status_t QairtSystem_Profile_Event_getEventData(
    QairtSystem_Profile_EventHandle_t event, QairtProfile_EventHandle_t* profileEventHandle);

/**
 * @brief Get the number of sub-events in a system profile event.
 *
 * @param[in] eventHandle A handle to a system profile event.
 *
 * @param[out] numSubEvents The number of sub-events.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Number of sub-events was successfully retrieved.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_HANDLE: _eventHandle_ is not a valid handle.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_ARGUMENT: _numSubEvents_ is NULL.
 *
 * @note Use corresponding API through QairtSystem_Profile_EventV1_t.
 */
Qairt_Status_t QairtSystem_Profile_Event_getNumSubEvents(
    QairtSystem_Profile_EventHandle_t eventHandle, uint32_t* numSubEvents);

/**
 * @brief Get a sub-event at a specific index from a system profile event.
 *
 * @param[in] eventHandle A handle to a system profile event.
 *
 * @param[in] index The index of the sub-event to retrieve.
 *
 * @param[out] subEventHandle A handle to the sub-event at the specified index.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Sub-event was successfully retrieved.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_HANDLE: _eventHandle_ is not a valid handle.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_ARGUMENT: _subEventHandle_ is NULL or _index_ is out
 * of bounds.
 *
 * @note Use corresponding API through QairtSystem_Profile_EventV1_t.
 */
Qairt_Status_t QairtSystem_Profile_Event_getSubEventAt(
    QairtSystem_Profile_EventHandle_t eventHandle,
    uint32_t index,
    QairtSystem_Profile_EventHandle_t* subEventHandle);

/**
 * @brief Set the sub-events in a system profile event.
 *
 * @param[in] eventHandle A handle to a system profile event.
 *
 * @param[in] subEventHandles An array of sub-event handles to set.
 *
 * @param[in] numSubEvents The number of sub-events in the array.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Sub-events were successfully set.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_HANDLE: _eventHandle_ is not a valid handle.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_ARGUMENT: _subEventHandles_ is NULL or
 * _numSubEvents_ is invalid.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_MEM_ALLOC: Memory allocation failed.
 *
 * @note Use corresponding API through QairtSystem_Profile_EventV1_t.
 */
Qairt_Status_t QairtSystem_Profile_Event_setSubEventData(
    QairtSystem_Profile_EventHandle_t eventHandle,
    QairtSystem_Profile_EventHandle_t* subEventHandles,
    uint32_t numSubEvents);

/**
 * @brief Add a sub-event to a system profile event.
 *
 * @param[in] eventHandle A handle to a system profile event.
 *
 * @param[in] subEventHandle A handle to the sub-event to add.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Sub-event was successfully added.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_HANDLE: _eventHandle_ or _subEventHandle_ is not a
 * valid handle.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_MEM_ALLOC: Memory allocation failed.
 *
 * @note Use corresponding API through QairtSystem_Profile_EventV1_t.
 */
Qairt_Status_t QairtSystem_Profile_Event_addSubEvent(
    QairtSystem_Profile_EventHandle_t eventHandle,
    QairtSystem_Profile_EventHandle_t subEventHandle);

/**
 * @brief Create a system profile profile data object.
 *
 * @param[out] profileDataHandle A handle to the created system profile profile data.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Profile data was successfully created.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_ARGUMENT: _profileDataHandle_ is NULL.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_MEM_ALLOC: Memory allocation failed.
 *
 * @note Use corresponding API through QairtSystem_Profile_ProfileDataV1_t.
 */
Qairt_Status_t QairtSystem_Profile_ProfileData_create(
    QairtSystem_Profile_ProfileDataHandle_t* profileDataHandle);

/**
 * @brief Free a system profile profile data object and associated resources.
 *
 * @param[in] profileDataHandle A handle to a created system profile profile data.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Profile data was successfully freed.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_HANDLE: _profileDataHandle_ is not a valid handle.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_MEM_ALLOC: Memory deallocation error.
 *
 * @note Use corresponding API through QairtSystem_Profile_ProfileDataV1_t.
 */
Qairt_Status_t QairtSystem_Profile_ProfileData_free(
    QairtSystem_Profile_ProfileDataHandle_t profileDataHandle);

/**
 * @brief Set the header in a system profile profile data object.
 *
 * @param[in] profileDataHandle A handle to a system profile profile data.
 *
 * @param[in] headerHandle A handle to the header to associate with this profile data.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Header was successfully set.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_HANDLE: _profileDataHandle_ or _headerHandle_ is not
 * a valid handle.
 *
 * @note Use corresponding API through QairtSystem_Profile_ProfileDataV1_t.
 */
Qairt_Status_t QairtSystem_Profile_ProfileData_setHeader(
    QairtSystem_Profile_ProfileDataHandle_t profileDataHandle,
    QairtSystem_Profile_HeaderHandle_t headerHandle);

/**
 * @brief Get the header from a system profile profile data object.
 *
 * @param[in] profileDataHandle A handle to a system profile profile data.
 *
 * @param[out] headerHandle A handle to the associated header.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Header was successfully retrieved.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_HANDLE: _profileDataHandle_ is not a valid handle.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_ARGUMENT: _headerHandle_ is NULL.
 *
 * @note Use corresponding API through QairtSystem_Profile_ProfileDataV1_t.
 */
Qairt_Status_t QairtSystem_Profile_ProfileData_getHeader(
    QairtSystem_Profile_ProfileDataHandle_t profileDataHandle,
    QairtSystem_Profile_HeaderHandle_t* headerHandle);

/**
 * @brief Get the number of profiling events in a system profile profile data object.
 *
 * @param[in] profileDataHandle A handle to a system profile profile data.
 *
 * @param[out] numEvents The number of profiling events.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Number of events was successfully retrieved.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_HANDLE: _profileDataHandle_ is not a valid handle.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_ARGUMENT: _numEvents_ is NULL.
 *
 * @note Use corresponding API through QairtSystem_Profile_ProfileDataV1_t.
 */
Qairt_Status_t QairtSystem_Profile_ProfileData_getNumProfilingEvents(
    QairtSystem_Profile_ProfileDataHandle_t profileDataHandle, uint32_t* numEvents);

/**
 * @brief Get an event at a specific index from a system profile profile data object.
 *
 * @param[in] profileDataHandle A handle to a system profile profile data.
 *
 * @param[in] index The index of the event to retrieve.
 *
 * @param[out] eventHandle A handle to the event at the specified index.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Event was successfully retrieved.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_HANDLE: _profileDataHandle_ is not a valid handle.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_ARGUMENT: _eventHandle_ is NULL or _index_ is out of
 * bounds.
 *
 * @note Use corresponding API through QairtSystem_Profile_ProfileDataV1_t.
 */
Qairt_Status_t QairtSystem_Profile_ProfileData_getEventAt(
    QairtSystem_Profile_ProfileDataHandle_t profileDataHandle,
    uint32_t index,
    QairtSystem_Profile_EventHandle_t* eventHandle);

/**
 * @brief Set the events in a system profile profile data object.
 *
 * @param[in] profileDataHandle A handle to a system profile profile data.
 *
 * @param[in] eventHandles An array of event handles to set.
 *
 * @param[in] numEvents The number of events in the array.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Events were successfully set.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_HANDLE: _profileDataHandle_ is not a valid handle.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_ARGUMENT: _eventHandles_ is NULL or _numEvents_ is
 * invalid.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_MEM_ALLOC: Memory allocation failed.
 *
 * @note Use corresponding API through QairtSystem_Profile_ProfileDataV1_t.
 */
Qairt_Status_t QairtSystem_Profile_ProfileData_setEvents(
    QairtSystem_Profile_ProfileDataHandle_t profileDataHandle,
    QairtSystem_Profile_EventHandle_t* eventHandles,
    uint32_t numEvents);

/**
 * @brief Add an event to a system profile profile data object.
 *
 * @param[in] profileDataHandle A handle to a system profile profile data.
 *
 * @param[in] eventHandle A handle to the event to add.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Event was successfully added.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_HANDLE: _profileDataHandle_ or _eventHandle_ is not
 * a valid handle.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_MEM_ALLOC: Memory allocation failed.
 *
 * @note Use corresponding API through QairtSystem_Profile_ProfileDataV1_t.
 */
Qairt_Status_t QairtSystem_Profile_ProfileData_addEvent(
    QairtSystem_Profile_ProfileDataHandle_t profileDataHandle,
    QairtSystem_Profile_EventHandle_t eventHandle);

/**
 * @brief Create a system profile data object.
 *
 * @param[out] dataHandle A handle to the created system profile data.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Data was successfully created.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_ARGUMENT: _dataHandle_ is NULL.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_MEM_ALLOC: Memory allocation failed.
 *
 * @note Use corresponding API through QairtSystem_Profile_DataV1_t.
 */
Qairt_Status_t QairtSystem_Profile_Data_create(QairtSystem_Profile_DataHandle_t* dataHandle);

/**
 * @brief Free a system profile data object and associated resources.
 *
 * @param[in] dataHandle A handle to a created system profile data.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Data was successfully freed.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_HANDLE: _dataHandle_ is not a valid handle.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_MEM_ALLOC: Memory deallocation error.
 *
 * @note Use corresponding API through QairtSystem_Profile_DataV1_t.
 */
Qairt_Status_t QairtSystem_Profile_Data_free(QairtSystem_Profile_DataHandle_t dataHandle);

/**
 * @brief Get the version from a system profile data object.
 *
 * @param[in] dataHandle A handle to a system profile data.
 *
 * @param[out] version The profile data version.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Version was successfully retrieved.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_HANDLE: _dataHandle_ is not a valid handle.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_ARGUMENT: _version_ is NULL.
 *
 * @note Use corresponding API through QairtSystem_Profile_DataV1_t.
 */
Qairt_Status_t QairtSystem_Profile_Data_getVersion(
    QairtSystem_Profile_DataHandle_t dataHandle, QairtSystem_Profile_DataVersion_t* version);

/**
 * @brief Set the profile data V1 in a system profile data object.
 *
 * @param[in] dataHandle A handle to a system profile data.
 *
 * @param[in] profileDataHandle A handle to the profile data V1 to associate with this data.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Profile data V1 was successfully set.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_HANDLE: _dataHandle_ or _profileDataHandle_ is not a
 * valid handle.
 *
 * @note Use corresponding API through QairtSystem_Profile_DataV1_t.
 */
Qairt_Status_t QairtSystem_Profile_Data_setProfileDataV1(
    QairtSystem_Profile_DataHandle_t dataHandle,
    QairtSystem_Profile_ProfileDataHandle_t profileDataHandle);

/**
 * @brief Get the profile data V1 from a system profile data object.
 *
 * @param[in] dataHandle A handle to a system profile data.
 *
 * @param[out] profileDataHandle A handle to the associated profile data V1.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Profile data V1 was successfully retrieved.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_HANDLE: _dataHandle_ is not a valid handle.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_ARGUMENT: _profileDataHandle_ is NULL.
 *
 * @note Use corresponding API through QairtSystem_Profile_DataV1_t.
 */
Qairt_Status_t QairtSystem_Profile_Data_getProfileDataV1(
    QairtSystem_Profile_DataHandle_t dataHandle,
    QairtSystem_Profile_ProfileDataHandle_t* profileDataHandle);

/**
 * @brief Create a serialization file header object.
 *
 * @param[out] fileHeaderHandle A handle to the created serialization file header.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: File header was successfully created.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_ARGUMENT: _fileHeaderHandle_ is NULL.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_MEM_ALLOC: Memory allocation failed.
 *
 * @note Use corresponding API through QairtSystem_Profile_SerializationFileHeaderV1_t.
 */
Qairt_Status_t QairtSystem_Profile_SerializationFileHeader_create(
    QairtSystem_Profile_SerializationFileHeaderHandle_t* fileHeaderHandle);

/**
 * @brief Free a serialization file header and associated resources.
 *
 * @param[in] fileHeaderHandle A handle to a created serialization file header.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: File header was successfully freed.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_HANDLE: _fileHeaderHandle_ is not a valid handle.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_MEM_ALLOC: Memory deallocation error.
 *
 * @note Use corresponding API through QairtSystem_Profile_SerializationFileHeaderV1_t.
 */
Qairt_Status_t QairtSystem_Profile_SerializationFileHeader_free(
    QairtSystem_Profile_SerializationFileHeaderHandle_t fileHeaderHandle);

/**
 * @brief Get the application name from a serialization file header.
 *
 * @param[in] fileHeaderHandle A handle to a serialization file header.
 *
 * @param[out] appName The application name string.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Application name was successfully retrieved.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_HANDLE: _fileHeaderHandle_ is not a valid handle.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_ARGUMENT: _appName_ is NULL.
 *
 * @note Use corresponding API through QairtSystem_Profile_SerializationFileHeaderV1_t.
 */
Qairt_Status_t QairtSystem_Profile_SerializationFileHeader_getAppName(
    QairtSystem_Profile_SerializationFileHeaderHandle_t fileHeaderHandle, const char** appName);

/**
 * @brief Set the application name in a serialization file header.
 *
 * @param[in] fileHeaderHandle A handle to a serialization file header.
 *
 * @param[in] appName The application name string to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Application name was successfully set.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_HANDLE: _fileHeaderHandle_ is not a valid handle.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_ARGUMENT: _appName_ is NULL.
 *
 * @note Use corresponding API through QairtSystem_Profile_SerializationFileHeaderV1_t.
 */
Qairt_Status_t QairtSystem_Profile_SerializationFileHeader_setAppName(
    QairtSystem_Profile_SerializationFileHeaderHandle_t fileHeaderHandle, const char* appName);

/**
 * @brief Get the application version from a serialization file header.
 *
 * @param[in] fileHeaderHandle A handle to a serialization file header.
 *
 * @param[out] appVersion The application version string.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Application version was successfully retrieved.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_HANDLE: _fileHeaderHandle_ is not a valid handle.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_ARGUMENT: _appVersion_ is NULL.
 *
 * @note Use corresponding API through QairtSystem_Profile_SerializationFileHeaderV1_t.
 */
Qairt_Status_t QairtSystem_Profile_SerializationFileHeader_getAppVersion(
    QairtSystem_Profile_SerializationFileHeaderHandle_t fileHeaderHandle,
    const char** appVersion);

/**
 * @brief Set the application version in a serialization file header.
 *
 * @param[in] fileHeaderHandle A handle to a serialization file header.
 *
 * @param[in] appVersion The application version string to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Application version was successfully set.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_HANDLE: _fileHeaderHandle_ is not a valid handle.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_ARGUMENT: _appVersion_ is NULL.
 *
 * @note Use corresponding API through QairtSystem_Profile_SerializationFileHeaderV1_t.
 */
Qairt_Status_t QairtSystem_Profile_SerializationFileHeader_setAppVersion(
    QairtSystem_Profile_SerializationFileHeaderHandle_t fileHeaderHandle, const char* appVersion);

/**
 * @brief Get the backend version from a serialization file header.
 *
 * @param[in] fileHeaderHandle A handle to a serialization file header.
 *
 * @param[out] backendVersion The backend version string.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Backend version was successfully retrieved.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_HANDLE: _fileHeaderHandle_ is not a valid handle.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_ARGUMENT: _backendVersion_ is NULL.
 *
 * @note Use corresponding API through QairtSystem_Profile_SerializationFileHeaderV1_t.
 */
Qairt_Status_t QairtSystem_Profile_SerializationFileHeader_getBackendVersion(
    QairtSystem_Profile_SerializationFileHeaderHandle_t fileHeaderHandle,
    const char** backendVersion);

/**
 * @brief Set the backend version in a serialization file header.
 *
 * @param[in] fileHeaderHandle A handle to a serialization file header.
 *
 * @param[in] backendVersion The backend version string to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Backend version was successfully set.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_HANDLE: _fileHeaderHandle_ is not a valid handle.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_ARGUMENT: _backendVersion_ is NULL.
 *
 * @note Use corresponding API through QairtSystem_Profile_SerializationFileHeaderV1_t.
 */
Qairt_Status_t QairtSystem_Profile_SerializationFileHeader_setBackendVersion(
    QairtSystem_Profile_SerializationFileHeaderHandle_t fileHeaderHandle,
    const char* backendVersion);

/**
 * @brief Create a serialization target configuration object.
 *
 * @param[out] configHandle A handle to the created serialization target configuration.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Configuration was successfully created.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_ARGUMENT: _configHandle_ is NULL.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_MEM_ALLOC: Memory allocation failed.
 *
 * @note Use corresponding API through QairtSystem_Profile_SerializationTargetConfigurationV1_t.
 */
Qairt_Status_t QairtSystem_Profile_SerializationTargetConfiguration_create(
    QairtSystem_Profile_SerializationTargetConfigurationHandle_t* configHandle);

/**
 * @brief Free a serialization target configuration and associated resources.
 *
 * @param[in] configHandle A handle to a created serialization target configuration.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Configuration was successfully freed.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_HANDLE: _configHandle_ is not a valid handle.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_MEM_ALLOC: Memory deallocation error.
 *
 * @note Use corresponding API through QairtSystem_Profile_SerializationTargetConfigurationV1_t.
 */
Qairt_Status_t QairtSystem_Profile_SerializationTargetConfiguration_free(
    QairtSystem_Profile_SerializationTargetConfigurationHandle_t configHandle);

/**
 * @brief Set the maximum number of messages in a serialization target configuration.
 *
 * @param[in] configHandle A handle to a serialization target configuration.
 *
 * @param[in] maxNumMessages The maximum number of messages to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Maximum number of messages was successfully set.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_HANDLE: _configHandle_ is not a valid handle.
 *
 * @note Use corresponding API through QairtSystem_Profile_SerializationTargetConfigurationV1_t.
 */
Qairt_Status_t QairtSystem_Profile_SerializationTargetConfiguration_setMaxNumMessages(
    QairtSystem_Profile_SerializationTargetConfigurationHandle_t configHandle,
    uint32_t maxNumMessages);

/**
 * @brief Get the maximum number of messages from a serialization target configuration.
 *
 * @param[in] configHandle A handle to a serialization target configuration.
 *
 * @param[out] maxNumMessages The maximum number of messages.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Maximum number of messages was successfully retrieved.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_HANDLE: _configHandle_ is not a valid handle.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_ARGUMENT: _maxNumMessages_ is NULL.
 *
 * @note Use corresponding API through QairtSystem_Profile_SerializationTargetConfigurationV1_t.
 */
Qairt_Status_t QairtSystem_Profile_SerializationTargetConfiguration_getMaxNumMessages(
    QairtSystem_Profile_SerializationTargetConfigurationHandle_t configHandle,
    uint32_t* maxNumMessages);

/**
 * @brief Set the file header in a serialization target configuration.
 *
 * @param[in] configHandle A handle to a serialization target configuration.
 *
 * @param[in] fileHeaderHandle A handle to the file header to associate with this configuration.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: File header was successfully set.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_HANDLE: _configHandle_ or _fileHeaderHandle_ is not
 * a valid handle.
 *
 * @note Use corresponding API through QairtSystem_Profile_SerializationTargetConfigurationV1_t.
 */
Qairt_Status_t QairtSystem_Profile_SerializationTargetConfiguration_setFileHeader(
    QairtSystem_Profile_SerializationTargetConfigurationHandle_t configHandle,
    QairtSystem_Profile_SerializationFileHeaderHandle_t fileHeaderHandle);

/**
 * @brief Get the file header from a serialization target configuration.
 *
 * @param[in] configHandle A handle to a serialization target configuration.
 *
 * @param[out] fileHeaderHandle A handle to the associated file header.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: File header was successfully retrieved.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_HANDLE: _configHandle_ is not a valid handle.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_ARGUMENT: _fileHeaderHandle_ is NULL.
 *
 * @note Use corresponding API through QairtSystem_Profile_SerializationTargetConfigurationV1_t.
 */
Qairt_Status_t QairtSystem_Profile_SerializationTargetConfiguration_getFileHeader(
    QairtSystem_Profile_SerializationTargetConfigurationHandle_t configHandle,
    QairtSystem_Profile_SerializationFileHeaderHandle_t* fileHeaderHandle);

/**
 * @brief Create a serialization target file object.
 *
 * @param[out] targetFileHandle A handle to the created serialization target file.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Target file was successfully created.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_ARGUMENT: _targetFileHandle_ is NULL.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_MEM_ALLOC: Memory allocation failed.
 *
 * @note Use corresponding API through QairtSystem_Profile_SerializationTargetFileV1_t.
 */
Qairt_Status_t QairtSystem_Profile_SerializationTargetFile_create(
    QairtSystem_Profile_SerializationTargetFileHandle_t* targetFileHandle);

/**
 * @brief Free a serialization target file and associated resources.
 *
 * @param[in] targetFileHandle A handle to a created serialization target file.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Target file was successfully freed.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_HANDLE: _targetFileHandle_ is not a valid handle.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_MEM_ALLOC: Memory deallocation error.
 *
 * @note Use corresponding API through QairtSystem_Profile_SerializationTargetFileV1_t.
 */
Qairt_Status_t QairtSystem_Profile_SerializationTargetFile_free(
    QairtSystem_Profile_SerializationTargetFileHandle_t targetFileHandle);

/**
 * @brief Set the file name in a serialization target file.
 *
 * @param[in] targetFileHandle A handle to a serialization target file.
 *
 * @param[in] fileName The file name string to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: File name was successfully set.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_HANDLE: _targetFileHandle_ is not a valid handle.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_ARGUMENT: _fileName_ is NULL.
 *
 * @note Use corresponding API through QairtSystem_Profile_SerializationTargetFileV1_t.
 */
Qairt_Status_t QairtSystem_Profile_SerializationTargetFile_setFileName(
    QairtSystem_Profile_SerializationTargetFileHandle_t targetFileHandle, const char* fileName);

/**
 * @brief Get the file name from a serialization target file.
 *
 * @param[in] targetFileHandle A handle to a serialization target file.
 *
 * @param[out] fileName The file name string.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: File name was successfully retrieved.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_HANDLE: _targetFileHandle_ is not a valid handle.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_ARGUMENT: _fileName_ is NULL.
 *
 * @note Use corresponding API through QairtSystem_Profile_SerializationTargetFileV1_t.
 */
Qairt_Status_t QairtSystem_Profile_SerializationTargetFile_getFileName(
    QairtSystem_Profile_SerializationTargetFileHandle_t targetFileHandle, const char** fileName);

/**
 * @brief Set the file directory in a serialization target file.
 *
 * @param[in] targetFileHandle A handle to a serialization target file.
 *
 * @param[in] fileDirectory The file directory string to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: File directory was successfully set.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_HANDLE: _targetFileHandle_ is not a valid handle.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_ARGUMENT: _fileDirectory_ is NULL.
 *
 * @note Use corresponding API through QairtSystem_Profile_SerializationTargetFileV1_t.
 */
Qairt_Status_t QairtSystem_Profile_SerializationTargetFile_setFileDirectory(
    QairtSystem_Profile_SerializationTargetFileHandle_t targetFileHandle,
    const char* fileDirectory);

/**
 * @brief Get the file directory from a serialization target file.
 *
 * @param[in] targetFileHandle A handle to a serialization target file.
 *
 * @param[out] fileDirectory The file directory string.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: File directory was successfully retrieved.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_HANDLE: _targetFileHandle_ is not a valid handle.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_ARGUMENT: _fileDirectory_ is NULL.
 *
 * @note Use corresponding API through QairtSystem_Profile_SerializationTargetFileV1_t.
 */
Qairt_Status_t QairtSystem_Profile_SerializationTargetFile_getFileDirectory(
    QairtSystem_Profile_SerializationTargetFileHandle_t targetFileHandle,
    const char** fileDirectory);

/**
 * @brief Create a serialization target info object.
 *
 * @param[out] targetInfoHandle A handle to the created serialization target info.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Target info was successfully created.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_ARGUMENT: _targetInfoHandle_ is NULL.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_MEM_ALLOC: Memory allocation failed.
 *
 * @note Use corresponding API through QairtSystem_Profile_SerializationTargetInfoV1_t.
 */
Qairt_Status_t QairtSystem_Profile_SerializationTargetInfo_create(
    QairtSystem_Profile_SerializationTargetInfoHandle_t* targetInfoHandle);

/**
 * @brief Free a serialization target info and associated resources.
 *
 * @param[in] targetInfoHandle A handle to a created serialization target info.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Target info was successfully freed.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_HANDLE: _targetInfoHandle_ is not a valid handle.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_MEM_ALLOC: Memory deallocation error.
 *
 * @note Use corresponding API through QairtSystem_Profile_SerializationTargetInfoV1_t.
 */
Qairt_Status_t QairtSystem_Profile_SerializationTargetInfo_free(
    QairtSystem_Profile_SerializationTargetInfoHandle_t targetInfoHandle);

/**
 * @brief Get the type from a serialization target info.
 *
 * @param[in] targetInfoHandle A handle to a serialization target info.
 *
 * @param[out] type The serialization target type.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Type was successfully retrieved.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_HANDLE: _targetInfoHandle_ is not a valid handle.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_ARGUMENT: _type_ is NULL.
 *
 * @note Use corresponding API through QairtSystem_Profile_SerializationTargetInfoV1_t.
 */
Qairt_Status_t QairtSystem_Profile_SerializationTargetInfo_getType(
    QairtSystem_Profile_SerializationTargetInfoHandle_t targetInfoHandle,
    QairtSystem_Profile_SerializationTargetType_t* type);

/**
 * @brief Set the target file in a serialization target info.
 *
 * @param[in] targetInfoHandle A handle to a serialization target info.
 *
 * @param[in] targetFileHandle A handle to the target file to associate with this target info.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Target file was successfully set.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_HANDLE: _targetInfoHandle_ or _targetFileHandle_ is
 * not a valid handle.
 *
 * @note Use corresponding API through QairtSystem_Profile_SerializationTargetInfoV1_t.
 */
Qairt_Status_t QairtSystem_Profile_SerializationTargetInfo_setTargetFile(
    QairtSystem_Profile_SerializationTargetInfoHandle_t targetInfoHandle,
    QairtSystem_Profile_SerializationTargetFileHandle_t targetFileHandle);

/**
 * @brief Get the target file from a serialization target info.
 *
 * @param[in] targetInfoHandle A handle to a serialization target info.
 *
 * @param[out] targetFileHandle A handle to the associated target file.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Target file was successfully retrieved.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_HANDLE: _targetInfoHandle_ is not a valid handle.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_ARGUMENT: _targetFileHandle_ is NULL.
 *
 * @note Use corresponding API through QairtSystem_Profile_SerializationTargetInfoV1_t.
 */
Qairt_Status_t QairtSystem_Profile_SerializationTargetInfo_getTargetFile(
    QairtSystem_Profile_SerializationTargetInfoHandle_t targetInfoHandle,
    QairtSystem_Profile_SerializationTargetFileHandle_t* targetFileHandle);

/**
 * @brief Create a serialization target object.
 *
 * @param[in] targetInfoHandle A handle to the serialization target info.
 *
 * @param[in] configHandle A handle to the serialization target configuration.
 *
 * @param[out] serializationTargetHandle A handle to the created serialization target.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Serialization target was successfully created.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_HANDLE: _targetInfoHandle_ or _configHandle_ is not
 * a valid handle.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_ARGUMENT: _serializationTargetHandle_ is NULL.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_MEM_ALLOC: Memory allocation failed.
 *
 * @note Use corresponding API through QairtSystem_Profile_SerializationTargetV1_t.
 */
Qairt_Status_t QairtSystem_Profile_SerializationTarget_create(
    QairtSystem_Profile_SerializationTargetInfoHandle_t targetInfoHandle,
    QairtSystem_Profile_SerializationTargetConfigurationHandle_t configHandle,
    QairtSystem_Profile_SerializationTargetHandle_t* serializationTargetHandle);

/**
 * @brief Free a serialization target and associated resources.
 *
 * @param[in] serializationTargetHandle A handle to a created serialization target.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Serialization target was successfully freed.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_HANDLE: _serializationTargetHandle_ is not a valid
 * handle.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_MEM_ALLOC: Memory deallocation error.
 *
 * @note Use corresponding API through QairtSystem_Profile_SerializationTargetV1_t.
 */
Qairt_Status_t QairtSystem_Profile_SerializationTarget_free(
    QairtSystem_Profile_SerializationTargetHandle_t serializationTargetHandle);

/**
 * @brief Serialize event data to the serialization target.
 *
 * @param[in] serializationTargetHandle A handle to a serialization target.
 *
 * @param[in] eventData An array of data handles containing events to serialize.
 *
 * @param[in] numEvents The number of data handles in the array.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Event data was successfully serialized.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_HANDLE: _serializationTargetHandle_ is not a valid
 * handle.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_INVALID_ARGUMENT: _eventData_ is NULL or _numEvents_ is
 * invalid.
 *         - QAIRT_SYSTEM_PROFILE_ERROR_SERIALIZATION_FAILED: Serialization operation failed.
 *
 * @note Use corresponding API through QairtSystem_Profile_SerializationTargetV1_t.
 */
Qairt_Status_t QairtSystem_Profile_SerializationTarget_serializeEventData(
    QairtSystem_Profile_SerializationTargetHandle_t serializationTargetHandle,
    QairtSystem_Profile_DataHandle_t* eventData,
    uint32_t numEvents);

//=============================================================================
// Function Pointer Typedefs
//=============================================================================

// Header V1
typedef Qairt_Status_t (*QairtSystem_Profile_Header_CreateFn_t)(
    QairtSystem_Profile_HeaderHandle_t*);
typedef Qairt_Status_t (*QairtSystem_Profile_Header_FreeFn_t)(
    QairtSystem_Profile_HeaderHandle_t);
typedef Qairt_Status_t (*QairtSystem_Profile_Header_GetStartTimeFn_t)(
    QairtSystem_Profile_HeaderHandle_t, uint64_t*);
typedef Qairt_Status_t (*QairtSystem_Profile_Header_SetStartTimeFn_t)(
    QairtSystem_Profile_HeaderHandle_t, uint64_t);
typedef Qairt_Status_t (*QairtSystem_Profile_Header_GetStopTimeFn_t)(
    QairtSystem_Profile_HeaderHandle_t, uint64_t*);
typedef Qairt_Status_t (*QairtSystem_Profile_Header_SetStopTimeFn_t)(
    QairtSystem_Profile_HeaderHandle_t, uint64_t);
typedef Qairt_Status_t (*QairtSystem_Profile_Header_GetStartMemFn_t)(
    QairtSystem_Profile_HeaderHandle_t, uint64_t*);
typedef Qairt_Status_t (*QairtSystem_Profile_Header_SetStartMemFn_t)(
    QairtSystem_Profile_HeaderHandle_t, uint64_t);
typedef Qairt_Status_t (*QairtSystem_Profile_Header_GetStopMemFn_t)(
    QairtSystem_Profile_HeaderHandle_t, uint64_t*);
typedef Qairt_Status_t (*QairtSystem_Profile_Header_SetStopMemFn_t)(
    QairtSystem_Profile_HeaderHandle_t, uint64_t);
typedef Qairt_Status_t (*QairtSystem_Profile_Header_GetMethodTypeFn_t)(
    QairtSystem_Profile_HeaderHandle_t, QairtSystem_Profile_MethodType_t*);
typedef Qairt_Status_t (*QairtSystem_Profile_Header_SetMethodTypeFn_t)(
    QairtSystem_Profile_HeaderHandle_t, QairtSystem_Profile_MethodType_t);
typedef Qairt_Status_t (*QairtSystem_Profile_Header_GetVisibilityFn_t)(
    QairtSystem_Profile_HeaderHandle_t, QairtSystem_Profile_Visibility_t*);
typedef Qairt_Status_t (*QairtSystem_Profile_Header_SetVisibilityFn_t)(
    QairtSystem_Profile_HeaderHandle_t, QairtSystem_Profile_Visibility_t);
typedef Qairt_Status_t (*QairtSystem_Profile_Header_GetGraphNameFn_t)(
    QairtSystem_Profile_HeaderHandle_t, const char**);
typedef Qairt_Status_t (*QairtSystem_Profile_Header_SetGraphNameFn_t)(
    QairtSystem_Profile_HeaderHandle_t, const char*);

// Event V1
typedef Qairt_Status_t (*QairtSystem_Profile_Event_CreateFn_t)(
    QairtSystem_Profile_EventHandle_t*);
typedef Qairt_Status_t (*QairtSystem_Profile_Event_FreeFn_t)(
    QairtSystem_Profile_EventHandle_t);
typedef Qairt_Status_t (*QairtSystem_Profile_Event_GetEventDataTypeFn_t)(
    QairtSystem_Profile_EventHandle_t, QairtSystem_Profile_EventDataType_t*);
typedef Qairt_Status_t (*QairtSystem_Profile_Event_SetEventDataFn_t)(
    QairtSystem_Profile_EventHandle_t, QairtProfile_EventHandle_t);
typedef Qairt_Status_t (*QairtSystem_Profile_Event_GetEventDataFn_t)(
    QairtSystem_Profile_EventHandle_t, QairtProfile_EventHandle_t*);
typedef Qairt_Status_t (*QairtSystem_Profile_Event_GetNumSubEventsFn_t)(
    QairtSystem_Profile_EventHandle_t, uint32_t*);
typedef Qairt_Status_t (*QairtSystem_Profile_Event_GetSubEventAtFn_t)(
    QairtSystem_Profile_EventHandle_t, uint32_t, QairtSystem_Profile_EventHandle_t*);
typedef Qairt_Status_t (*QairtSystem_Profile_Event_SetSubEventDataFn_t)(
    QairtSystem_Profile_EventHandle_t, QairtSystem_Profile_EventHandle_t*, uint32_t);
typedef Qairt_Status_t (*QairtSystem_Profile_Event_AddSubEventFn_t)(
    QairtSystem_Profile_EventHandle_t, QairtSystem_Profile_EventHandle_t);

// Profile Data V1
typedef Qairt_Status_t (*QairtSystem_Profile_ProfileData_CreateFn_t)(
    QairtSystem_Profile_ProfileDataHandle_t*);
typedef Qairt_Status_t (*QairtSystem_Profile_ProfileData_FreeFn_t)(
    QairtSystem_Profile_ProfileDataHandle_t);
typedef Qairt_Status_t (*QairtSystem_Profile_ProfileData_SetHeaderFn_t)(
    QairtSystem_Profile_ProfileDataHandle_t, QairtSystem_Profile_HeaderHandle_t);
typedef Qairt_Status_t (*QairtSystem_Profile_ProfileData_GetHeaderFn_t)(
    QairtSystem_Profile_ProfileDataHandle_t, QairtSystem_Profile_HeaderHandle_t*);
typedef Qairt_Status_t (*QairtSystem_Profile_ProfileData_GetNumProfilingEventsFn_t)(
    QairtSystem_Profile_ProfileDataHandle_t, uint32_t*);
typedef Qairt_Status_t (*QairtSystem_Profile_ProfileData_GetEventAtFn_t)(
    QairtSystem_Profile_ProfileDataHandle_t, uint32_t, QairtSystem_Profile_EventHandle_t*);
typedef Qairt_Status_t (*QairtSystem_Profile_ProfileData_SetEventsFn_t)(
    QairtSystem_Profile_ProfileDataHandle_t, QairtSystem_Profile_EventHandle_t*, uint32_t);
typedef Qairt_Status_t (*QairtSystem_Profile_ProfileData_AddEventFn_t)(
    QairtSystem_Profile_ProfileDataHandle_t, QairtSystem_Profile_EventHandle_t);

// Data V1
typedef Qairt_Status_t (*QairtSystem_Profile_Data_CreateFn_t)(
    QairtSystem_Profile_DataHandle_t*);
typedef Qairt_Status_t (*QairtSystem_Profile_Data_FreeFn_t)(
    QairtSystem_Profile_DataHandle_t);
typedef Qairt_Status_t (*QairtSystem_Profile_Data_GetVersionFn_t)(
    QairtSystem_Profile_DataHandle_t, QairtSystem_Profile_DataVersion_t*);
typedef Qairt_Status_t (*QairtSystem_Profile_Data_SetProfileDataV1Fn_t)(
    QairtSystem_Profile_DataHandle_t, QairtSystem_Profile_ProfileDataHandle_t);
typedef Qairt_Status_t (*QairtSystem_Profile_Data_GetProfileDataV1Fn_t)(
    QairtSystem_Profile_DataHandle_t, QairtSystem_Profile_ProfileDataHandle_t*);

// Serialization File Header V1
typedef Qairt_Status_t (*QairtSystem_Profile_SerializationFileHeader_CreateFn_t)(
    QairtSystem_Profile_SerializationFileHeaderHandle_t*);
typedef Qairt_Status_t (*QairtSystem_Profile_SerializationFileHeader_FreeFn_t)(
    QairtSystem_Profile_SerializationFileHeaderHandle_t);
typedef Qairt_Status_t (*QairtSystem_Profile_SerializationFileHeader_GetAppNameFn_t)(
    QairtSystem_Profile_SerializationFileHeaderHandle_t, const char**);
typedef Qairt_Status_t (*QairtSystem_Profile_SerializationFileHeader_SetAppNameFn_t)(
    QairtSystem_Profile_SerializationFileHeaderHandle_t, const char*);
typedef Qairt_Status_t (*QairtSystem_Profile_SerializationFileHeader_GetAppVersionFn_t)(
    QairtSystem_Profile_SerializationFileHeaderHandle_t, const char**);
typedef Qairt_Status_t (*QairtSystem_Profile_SerializationFileHeader_SetAppVersionFn_t)(
    QairtSystem_Profile_SerializationFileHeaderHandle_t, const char*);
typedef Qairt_Status_t (*QairtSystem_Profile_SerializationFileHeader_GetBackendVersionFn_t)(
    QairtSystem_Profile_SerializationFileHeaderHandle_t, const char**);
typedef Qairt_Status_t (*QairtSystem_Profile_SerializationFileHeader_SetBackendVersionFn_t)(
    QairtSystem_Profile_SerializationFileHeaderHandle_t, const char*);

// Serialization Target Configuration V1
typedef Qairt_Status_t (*QairtSystem_Profile_SerializationTargetConfiguration_CreateFn_t)(
    QairtSystem_Profile_SerializationTargetConfigurationHandle_t*);
typedef Qairt_Status_t (*QairtSystem_Profile_SerializationTargetConfiguration_FreeFn_t)(
    QairtSystem_Profile_SerializationTargetConfigurationHandle_t);
typedef Qairt_Status_t (
    *QairtSystem_Profile_SerializationTargetConfiguration_SetMaxNumMessagesFn_t)(
    QairtSystem_Profile_SerializationTargetConfigurationHandle_t, uint32_t);
typedef Qairt_Status_t (
    *QairtSystem_Profile_SerializationTargetConfiguration_GetMaxNumMessagesFn_t)(
    QairtSystem_Profile_SerializationTargetConfigurationHandle_t, uint32_t*);
typedef Qairt_Status_t (
    *QairtSystem_Profile_SerializationTargetConfiguration_SetFileHeaderFn_t)(
    QairtSystem_Profile_SerializationTargetConfigurationHandle_t,
    QairtSystem_Profile_SerializationFileHeaderHandle_t);
typedef Qairt_Status_t (
    *QairtSystem_Profile_SerializationTargetConfiguration_GetFileHeaderFn_t)(
    QairtSystem_Profile_SerializationTargetConfigurationHandle_t,
    QairtSystem_Profile_SerializationFileHeaderHandle_t*);

// Serialization Target File V1
typedef Qairt_Status_t (*QairtSystem_Profile_SerializationTargetFile_CreateFn_t)(
    QairtSystem_Profile_SerializationTargetFileHandle_t*);
typedef Qairt_Status_t (*QairtSystem_Profile_SerializationTargetFile_FreeFn_t)(
    QairtSystem_Profile_SerializationTargetFileHandle_t);
typedef Qairt_Status_t (*QairtSystem_Profile_SerializationTargetFile_SetFileNameFn_t)(
    QairtSystem_Profile_SerializationTargetFileHandle_t, const char*);
typedef Qairt_Status_t (*QairtSystem_Profile_SerializationTargetFile_GetFileNameFn_t)(
    QairtSystem_Profile_SerializationTargetFileHandle_t, const char**);
typedef Qairt_Status_t (*QairtSystem_Profile_SerializationTargetFile_SetFileDirectoryFn_t)(
    QairtSystem_Profile_SerializationTargetFileHandle_t, const char*);
typedef Qairt_Status_t (*QairtSystem_Profile_SerializationTargetFile_GetFileDirectoryFn_t)(
    QairtSystem_Profile_SerializationTargetFileHandle_t, const char**);

// Serialization Target Info V1
typedef Qairt_Status_t (*QairtSystem_Profile_SerializationTargetInfo_CreateFn_t)(
    QairtSystem_Profile_SerializationTargetInfoHandle_t*);
typedef Qairt_Status_t (*QairtSystem_Profile_SerializationTargetInfo_FreeFn_t)(
    QairtSystem_Profile_SerializationTargetInfoHandle_t);
typedef Qairt_Status_t (*QairtSystem_Profile_SerializationTargetInfo_GetTypeFn_t)(
    QairtSystem_Profile_SerializationTargetInfoHandle_t,
    QairtSystem_Profile_SerializationTargetType_t*);
typedef Qairt_Status_t (*QairtSystem_Profile_SerializationTargetInfo_SetTargetFileFn_t)(
    QairtSystem_Profile_SerializationTargetInfoHandle_t,
    QairtSystem_Profile_SerializationTargetFileHandle_t);
typedef Qairt_Status_t (*QairtSystem_Profile_SerializationTargetInfo_GetTargetFileFn_t)(
    QairtSystem_Profile_SerializationTargetInfoHandle_t,
    QairtSystem_Profile_SerializationTargetFileHandle_t*);

// Serialization Target V1
typedef Qairt_Status_t (*QairtSystem_Profile_SerializationTarget_CreateFn_t)(
    QairtSystem_Profile_SerializationTargetInfoHandle_t,
    QairtSystem_Profile_SerializationTargetConfigurationHandle_t,
    QairtSystem_Profile_SerializationTargetHandle_t*);
typedef Qairt_Status_t (*QairtSystem_Profile_SerializationTarget_FreeFn_t)(
    QairtSystem_Profile_SerializationTargetHandle_t);
typedef Qairt_Status_t (*QairtSystem_Profile_SerializationTarget_SerializeEventDataFn_t)(
    QairtSystem_Profile_SerializationTargetHandle_t,
    QairtSystem_Profile_DataHandle_t*,
    uint32_t);

//=============================================================================
// Public Interfaces
//=============================================================================

typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtSystem_Profile_Header_CreateFn_t create;
  QairtSystem_Profile_Header_FreeFn_t free;
  QairtSystem_Profile_Header_GetStartTimeFn_t getStartTime;
  QairtSystem_Profile_Header_SetStartTimeFn_t setStartTime;
  QairtSystem_Profile_Header_GetStopTimeFn_t getStopTime;
  QairtSystem_Profile_Header_SetStopTimeFn_t setStopTime;
  QairtSystem_Profile_Header_GetStartMemFn_t getStartMem;
  QairtSystem_Profile_Header_SetStartMemFn_t setStartMem;
  QairtSystem_Profile_Header_GetStopMemFn_t getStopMem;
  QairtSystem_Profile_Header_SetStopMemFn_t setStopMem;
  QairtSystem_Profile_Header_GetMethodTypeFn_t getMethodType;
  QairtSystem_Profile_Header_SetMethodTypeFn_t setMethodType;
  QairtSystem_Profile_Header_GetVisibilityFn_t getVisibility;
  QairtSystem_Profile_Header_SetVisibilityFn_t setVisibility;
  QairtSystem_Profile_Header_GetGraphNameFn_t getGraphName;
  QairtSystem_Profile_Header_SetGraphNameFn_t setGraphName;
} QairtSystem_Profile_HeaderV1_t;

typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtSystem_Profile_Event_CreateFn_t create;
  QairtSystem_Profile_Event_FreeFn_t free;
  QairtSystem_Profile_Event_GetEventDataTypeFn_t getEventDataType;
  QairtSystem_Profile_Event_SetEventDataFn_t setEventData;
  QairtSystem_Profile_Event_GetEventDataFn_t getEventData;
  QairtSystem_Profile_Event_GetNumSubEventsFn_t getNumSubEvents;
  QairtSystem_Profile_Event_GetSubEventAtFn_t getSubEventAt;
  QairtSystem_Profile_Event_SetSubEventDataFn_t setSubEventData;
  QairtSystem_Profile_Event_AddSubEventFn_t addSubEvent;
} QairtSystem_Profile_EventV1_t;

typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtSystem_Profile_ProfileData_CreateFn_t create;
  QairtSystem_Profile_ProfileData_FreeFn_t free;
  QairtSystem_Profile_ProfileData_SetHeaderFn_t setHeader;
  QairtSystem_Profile_ProfileData_GetHeaderFn_t getHeader;
  QairtSystem_Profile_ProfileData_GetNumProfilingEventsFn_t getNumProfilingEvents;
  QairtSystem_Profile_ProfileData_GetEventAtFn_t getEventAt;
  QairtSystem_Profile_ProfileData_SetEventsFn_t setEvents;
  QairtSystem_Profile_ProfileData_AddEventFn_t addEvent;
} QairtSystem_Profile_ProfileDataV1_t;

typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtSystem_Profile_Data_CreateFn_t create;
  QairtSystem_Profile_Data_FreeFn_t free;
  QairtSystem_Profile_Data_GetVersionFn_t getVersion;
  QairtSystem_Profile_Data_SetProfileDataV1Fn_t setProfileDataV1;
  QairtSystem_Profile_Data_GetProfileDataV1Fn_t getProfileDataV1;
} QairtSystem_Profile_DataV1_t;

typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtSystem_Profile_SerializationFileHeader_CreateFn_t create;
  QairtSystem_Profile_SerializationFileHeader_FreeFn_t free;
  QairtSystem_Profile_SerializationFileHeader_GetAppNameFn_t getAppName;
  QairtSystem_Profile_SerializationFileHeader_SetAppNameFn_t setAppName;
  QairtSystem_Profile_SerializationFileHeader_GetAppVersionFn_t getAppVersion;
  QairtSystem_Profile_SerializationFileHeader_SetAppVersionFn_t setAppVersion;
  QairtSystem_Profile_SerializationFileHeader_GetBackendVersionFn_t getBackendVersion;
  QairtSystem_Profile_SerializationFileHeader_SetBackendVersionFn_t setBackendVersion;
} QairtSystem_Profile_SerializationFileHeaderV1_t;

typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtSystem_Profile_SerializationTargetConfiguration_CreateFn_t create;
  QairtSystem_Profile_SerializationTargetConfiguration_FreeFn_t free;
  QairtSystem_Profile_SerializationTargetConfiguration_SetMaxNumMessagesFn_t setMaxNumMessages;
  QairtSystem_Profile_SerializationTargetConfiguration_GetMaxNumMessagesFn_t getMaxNumMessages;
  QairtSystem_Profile_SerializationTargetConfiguration_SetFileHeaderFn_t setFileHeader;
  QairtSystem_Profile_SerializationTargetConfiguration_GetFileHeaderFn_t getFileHeader;
} QairtSystem_Profile_SerializationTargetConfigurationV1_t;

typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtSystem_Profile_SerializationTargetFile_CreateFn_t create;
  QairtSystem_Profile_SerializationTargetFile_FreeFn_t free;
  QairtSystem_Profile_SerializationTargetFile_SetFileNameFn_t setFileName;
  QairtSystem_Profile_SerializationTargetFile_GetFileNameFn_t getFileName;
  QairtSystem_Profile_SerializationTargetFile_SetFileDirectoryFn_t setFileDirectory;
  QairtSystem_Profile_SerializationTargetFile_GetFileDirectoryFn_t getFileDirectory;
} QairtSystem_Profile_SerializationTargetFileV1_t;

typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtSystem_Profile_SerializationTargetInfo_CreateFn_t create;
  QairtSystem_Profile_SerializationTargetInfo_FreeFn_t free;
  QairtSystem_Profile_SerializationTargetInfo_GetTypeFn_t getType;
  QairtSystem_Profile_SerializationTargetInfo_SetTargetFileFn_t setTargetFile;
  QairtSystem_Profile_SerializationTargetInfo_GetTargetFileFn_t getTargetFile;
} QairtSystem_Profile_SerializationTargetInfoV1_t;

typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtSystem_Profile_SerializationTarget_CreateFn_t create;
  QairtSystem_Profile_SerializationTarget_FreeFn_t free;
  QairtSystem_Profile_SerializationTarget_SerializeEventDataFn_t serializeEventData;
} QairtSystem_Profile_SerializationTargetV1_t;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // QAIRT_SYSTEM_PROFILE_H

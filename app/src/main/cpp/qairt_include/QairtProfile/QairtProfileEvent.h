//=============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//=============================================================================

#ifndef QAIRT_PROFILEEVENT_H
#define QAIRT_PROFILEEVENT_H

#include "QairtCommon/QairtCommon.h"
#include "QairtCommon/QairtScalar.h"

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
// Macros
//=============================================================================
/**
 * @brief QairtProfile_EventType_t definition to get stats related to creation of
 *        context and graphs. If supported, this profile data captures stats
 *        starting with the context creation (QairtContext_create) and ending with
 *        graph finalize (QairtGraph_finalize). Alternatively, in case of loading
 *        a cached context, it captures stats for creating context from the
 *        cache (QairtContext_createFromBinary).
 *
 * @note init information maybe available on both QAIRT_PROFILE_LEVEL_BASIC and
 *       QAIRT_PROFILE_LEVEL_DETAILED levels
 *
 * @note If unit information is not available, the value should be interpreted
 *       as time in microseconds.
 */
#define QAIRT_PROFILE_EVENTTYPE_INIT 100
/**
 * @brief QairtProfile_EventType_t definition to get stats related to finalize
 *        operation on graphs in a context. If supported, this profile data
 *        captures stats for graph finalize (QairtGraph_finalize).
 *
 * @note finalize information maybe available on both QAIRT_PROFILE_LEVEL_BASIC and
 *       QAIRT_PROFILE_LEVEL_DETAILED levels
 *
 * @note If unit information is not available, the value should be interpreted
 *       as time in microseconds.
 */
#define QAIRT_PROFILE_EVENTTYPE_FINALIZE 300
/**
 * @brief QairtProfile_EventType_t definition to get stats related to execution
 *        of graphs in a context (QairtGraph_execute or QairtGraph_executeAsync).
 *        Basic level might include stats related to execution of entire graphs.
 *        In addition, detailed level can include stats related to individual
 *        nodes in graphs as sub-events.
 *
 * @note execute information maybe available on both QAIRT_PROFILE_LEVEL_BASIC and
 *       QAIRT_PROFILE_LEVEL_DETAILED levels
 *
 * @note If unit information is not available, the value should be interpreted
 *       as time in microseconds.
 */
#define QAIRT_PROFILE_EVENTTYPE_EXECUTE 400
/**
 * @brief QairtProfile_EventType_t definition to get data related to execution of
 *        an operation. This value can be interpreted appropriately in conjunction
 *        with the unit.
 *
 * @note node specific information is available on QAIRT_PROFILE_LEVEL_DETAILED level
 *
 * @note This is a sub-event of the QAIRT_PROFILE_EVENTTYPE_EXECUTE event.
 */
#define QAIRT_PROFILE_EVENTTYPE_NODE 404
/**
 * @brief QairtProfile_EventType_t definition to get stats related to time spent
 *        waiting in a queue when executing a graph.
 *
 * @note execute enqueue information maybe available on both QAIRT_PROFILE_LEVEL_BASIC
 *       and QAIRT_PROFILE_LEVEL_DETAILED levels
 *
 * @note This is a sub-event of the QAIRT_PROFILE_EVENTTYPE_EXECUTE event.
 */
#define QAIRT_PROFILE_EVENTTYPE_EXECUTE_QUEUE_WAIT 405
/**
 * @brief QairtProfile_EventType_t definition to get stats related to time spent
 *        pre-processing in preparation of executing a graph.
 *
 * @note execute preprocess information maybe available on both QAIRT_PROFILE_LEVEL_BASIC
 *       and QAIRT_PROFILE_LEVEL_DETAILED levels
 *
 * @note This is a sub-event of the QAIRT_PROFILE_EVENTTYPE_EXECUTE event.
 */
#define QAIRT_PROFILE_EVENTTYPE_EXECUTE_PREPROCESS 406
/**
 * @brief QairtProfile_EventType_t definition to get stats related to time spent
 *        on-device executing a graph.
 *
 * @note execute device information maybe available on both QAIRT_PROFILE_LEVEL_BASIC
 *       and QAIRT_PROFILE_LEVEL_DETAILED levels
 *
 * @note This is a sub-event of the QAIRT_PROFILE_EVENTTYPE_EXECUTE event.
 */
#define QAIRT_PROFILE_EVENTTYPE_EXECUTE_DEVICE 407
/**
 * @brief QairtProfile_EventType_t definition to get stats related to time spent
 *        post-processing after execution of a graph.
 *
 * @note execute postprocess information maybe available on both QAIRT_PROFILE_LEVEL_BASIC
 *       and QAIRT_PROFILE_LEVEL_DETAILED levels
 *
 * @note This is a sub-event of the QAIRT_PROFILE_EVENTTYPE_EXECUTE event.
 */
#define QAIRT_PROFILE_EVENTTYPE_EXECUTE_POSTPROCESS 408
/**
 * @brief QairtProfile_EventType_t definition to get stats related to deinit
 *        graphs and free context operation. This profile data captures stats
 *        for QairtContext_free.
 *
 * @note deinit information maybe available on both QAIRT_PROFILE_LEVEL_BASIC and
 *       QAIRT_PROFILE_LEVEL_DETAILED levels
 *
 * @note If unit information is not available, the value should be interpreted
 *       as time in microseconds.
 */
#define QAIRT_PROFILE_EVENTTYPE_DEINIT 500
/**
 * @brief QairtProfile_EventType_t definition to get traces related to graph
 *        preparation and execution steps. This profile data captures stats
 *        for QairtGraph_execute.
 *
 * @note trace information is available on QAIRT_PROFILE_LEVEL_DETAILED
 *       level only.
 */
#define QAIRT_PROFILE_EVENTTYPE_TRACE 600
/**
 * @brief QairtProfile_EventType_t definition reserved for each back end to define
 *        and extend
 *
 * @note The client should consult the backend-specific SDK documentation for
 *       information regarding interpretation of unit, value and identifier.
 */
#define QAIRT_PROFILE_EVENTTYPE_BACKEND 1000
/**
 * @brief QairtProfile_EventUnit_t definition to provide profiling measurement as
 *        time in microseconds
 */
#define QAIRT_PROFILE_EVENTUNIT_MICROSEC 1
/**
 * @brief QairtProfile_EventUnit_t definition to provide profiling measurement as
 *        memory in bytes
 */
#define QAIRT_PROFILE_EVENTUNIT_BYTES 2
/**
 * @brief QairtProfile_EventUnit_t definition to provide profiling measurement as
 *        time in cycles
 */
#define QAIRT_PROFILE_EVENTUNIT_CYCLES 3
/**
 * @brief QairtProfile_EventUnit_t definition to provide profiling measurement as
 *        a count
 */
#define QAIRT_PROFILE_EVENTUNIT_COUNT 4
/**
 * @brief QairtProfile_EventUnit_t definition to provide profiling measurement as
 *        an opaque object
 */
#define QAIRT_PROFILE_EVENTUNIT_OBJECT 5
/**
 * @brief QairtProfile_EventUnit_t definition to provide profiling measurement with
 *        no unit
 */
#define QAIRT_PROFILE_EVENTUNIT_NONE 6
/**
 * @brief QairtProfile_EventUnit_t definition reserved for each back end to define
 *        and extend
 */
#define QAIRT_PROFILE_EVENTUNIT_BACKEND 1000

//=============================================================================
// Data Types
//=============================================================================
typedef struct _QairtProfile_EventHandle_t* QairtProfile_EventHandle_t;

typedef uint32_t QairtProfile_EventType_t;
typedef uint32_t QairtProfile_EventUnit_t;

//=============================================================================
// Public Functions
//=============================================================================

/**
 * @brief Create a profile event
 *
 * @param[out] eventHandle A handle to the created profile event.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Event was successfully created.
 *         - QAIRT_PROFILE_ERROR_INVALID_HANDLE: Event handle is invalid.
 *         - QAIRT_PROFILE_ERROR_MEM_ALLOC: Event creation failed due to memory/resource allocation
 *                                          issues.
 *
 * @note Use corresponding API through QairtProfile_EventV1_t.
 */

Qairt_Status_t QairtProfile_Event_create(QairtProfile_EventHandle_t* eventHandle);

/**
 * @brief Free a profile event handle and associated resources.
 *
 * @param[in] eventHandle A handle to a created profile event.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Profile event was successfully freed.
 *         - QAIRT_PROFILE_ERROR_INVALID_HANDLE: _eventHandle_ is not a valid handle.
 *         - QAIRT_PROFILE_ERROR_MEM_ALLOC: Memory deallocation error.
 *
 * @note Use corresponding API through QairtProfile_EventV1_t.
 */
Qairt_Status_t QairtProfile_Event_free(QairtProfile_EventHandle_t eventHandle);

/**
 * @brief A function to get whether a profile event supports extended event fields.
 *
 * @param[in] eventHandle A profile event handle.
 *
 * @param[out] supportsExtendedEvent Whether this profile event supports extended event fields
 *                                   This includes timestamp and backendOpaqueObject support
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_PROFILE_ERROR_INVALID_HANDLE: _eventHandle_ is not a valid handle
 *
 * @note Use corresponding API through QairtProfile_EventV1_t.
 */
Qairt_Status_t QairtProfile_Event_supportsExtendedEvent(
    QairtProfile_EventHandle_t eventHandle, uint8_t* value);

/**
 * @brief A function to set the event type of a profile event.
 *
 * @param[in] eventHandle A profile event handle.
 *
 * @param[in] eventType The event type for this profiling event
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_PROFILE_ERROR_INVALID_HANDLE: _eventHandle_ is not a valid handle
 *         - QAIRT_PROFILE_ERROR_INVALID_ARGUMENT: _eventType_ is invalid
 *
 * @note Use corresponding API through QairtProfile_EventV1_t.
 */
Qairt_Status_t QairtProfile_Event_setEventType(QairtProfile_EventHandle_t eventHandle,
                                                    QairtProfile_EventType_t eventType);

/**
 * @brief A function to get the event type of a profile event.
 *
 * @param[in] eventHandle A profile event handle.
 *
 * @param[out] eventType The event type for this profiling event
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_PROFILE_ERROR_INVALID_HANDLE: _eventHandle_ is not a valid handle
 *
 * @note Use corresponding API through QairtProfile_EventV1_t.
 */
Qairt_Status_t QairtProfile_Event_getEventType(QairtProfile_EventHandle_t eventHandle,
                                                    QairtProfile_EventType_t* eventType);

/**
 * @brief A function to set the unit of a profile event.
 *
 * @param[in] eventHandle A profile event handle.
 *
 * @param[in] unit The unit for this profiling event
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_PROFILE_ERROR_INVALID_HANDLE: _eventHandle_ is not a valid handle
 *         - QAIRT_PROFILE_ERROR_INVALID_ARGUMENT: _unit_ is invalid
 *
 * @note Use corresponding API through QairtProfile_EventV1_t.
 */
Qairt_Status_t QairtProfile_Event_setUnit(QairtProfile_EventHandle_t eventHandle,
                                               QairtProfile_EventUnit_t unit);

/**
 * @brief A function to get the unit of a profile event.
 *
 * @param[in] eventHandle A profile event handle.
 *
 * @param[out] unit The unit for this profiling event
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_PROFILE_ERROR_INVALID_HANDLE: _eventHandle_ is not a valid handle
 *
 * @note Use corresponding API through QairtProfile_EventV1_t.
 */
Qairt_Status_t QairtProfile_Event_getUnit(QairtProfile_EventHandle_t eventHandle,
                                               QairtProfile_EventUnit_t* unit);

/**
 * @brief A function to set the identifier of a profile event.
 *
 * @param[in] eventHandle A profile event handle.
 *
 * @param[in] identifier The identifier for this profiling event
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_PROFILE_ERROR_INVALID_HANDLE: _eventHandle_ is not a valid handle
 *         - QAIRT_PROFILE_ERROR_INVALID_ARGUMENT: _identifier_ is invalid
 *
 * @note Use corresponding API through QairtProfile_EventV1_t.
 */
Qairt_Status_t QairtProfile_Event_setIdentifier(QairtProfile_EventHandle_t eventHandle,
                                                     const char* identifier);

/**
 * @brief A function to get the identifier of a profile event.
 *
 * @param[in] eventHandle A profile event handle.
 *
 * @param[out] identifier The identifier for this profiling event
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_PROFILE_ERROR_INVALID_HANDLE: _eventHandle_ is not a valid handle
 *         - QAIRT_PROFILE_ERROR_INVALID_ARGUMENT: _identifier_ is invalid
 *
 * @note Use corresponding API through QairtProfile_EventV1_t.
 */
Qairt_Status_t QairtProfile_Event_getIdentifier(QairtProfile_EventHandle_t eventHandle,
                                                     const char** identifier);

/**
 * @brief A function to set the timestamp of a profile event.
 *
 * @param[in] eventHandle A profile event handle.
 *
 * @param[in] timestamp The timestamp for this profiling event
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_PROFILE_ERROR_INVALID_HANDLE: _eventHandle_ is not a valid handle
 *         - QAIRT_PROFILE_ERROR_INCOMPATIBLE_EVENT: event does not support timestamp
 *
 * @note Use corresponding API through QairtProfile_EventV1_t.
 */
Qairt_Status_t QairtProfile_Event_setTimestamp(QairtProfile_EventHandle_t eventHandle,
                                                    uint64_t timestamp);

/**
 * @brief A function to get the timestamp of a profile event.
 *
 * @param[in] eventHandle A profile event handle.
 *
 * @param[out] timestamp The timestamp for this profiling event. If timestamp information is not
 *                       available, 0 will be returned.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_PROFILE_ERROR_INVALID_HANDLE: _eventHandle_ is not a valid handle
 *         - QAIRT_PROFILE_ERROR_INVALID_ARGUMENT: _timestamp_ is invalid
 *         - QAIRT_PROFILE_ERROR_DATA_NOT_AVAILABLE: data is not available for this event
 *
 * @note Use corresponding API through QairtProfile_EventV1_t.
 */
Qairt_Status_t QairtProfile_Event_getTimestamp(QairtProfile_EventHandle_t eventHandle,
                                                    uint64_t* timestamp);

/**
 * @brief A function to set the scalar value of a profile event.
 *
 * @param[in] eventHandle A profile event handle.
 *
 * @param[in] scalarHandle A handle to a scalar value for this profiling event. If this event does
 *                         not support extended data fields, the scalar value will be cast to a
 *                         uint64_t before being stored.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_PROFILE_ERROR_INVALID_HANDLE: _eventHandle_ or _scalarHandle_ is not a valid
 *           handle
 *
 * @note Use corresponding API through QairtProfile_EventV1_t.
 */
Qairt_Status_t QairtProfile_Event_setScalar(QairtProfile_EventHandle_t eventHandle,
                                                 QairtScalar_Handle_t scalarHandle);

/**
 * @brief A function to get the scalar value of a profile event.
 *
 * @param[in] eventHandle A profile event handle.
 *
 * @param[out] scalarHandle A handle to store the scalar value for this profiling event.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_PROFILE_ERROR_INVALID_HANDLE: _eventHandle_ is not a valid handle
 *         - QAIRT_PROFILE_ERROR_INVALID_ARGUMENT: _scalarHandle_ is invalid
 *         - QAIRT_PROFILE_ERROR_DATA_NOT_AVAILABLE: data is not available for this event
 *
 * @note Use corresponding API through QairtProfile_EventV1_t.
 */
Qairt_Status_t QairtProfile_Event_getScalar(QairtProfile_EventHandle_t eventHandle,
                                                 QairtScalar_Handle_t* scalarHandle);

/**
 * @brief A function to set the backend opaque object of a profile event.
 *
 * @param[in] eventHandle A profile event handle.
 *
 * @param[in] backendOpaqueObjectHandle A handle to a backend opaque object for this profiling
 *                                      event. If this event does not support extended data fields,
 *                                      an error will be returned.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_PROFILE_ERROR_INVALID_HANDLE: _eventHandle_ or _backendOpaqueObjectHandle_ is not
 *           a valid handle
 *         - QAIRT_PROFILE_ERROR_INCOMPATIBLE_EVENT: This event does not support backend opaque
 *           objects
 *
 * @note Use corresponding API through QairtProfile_EventV1_t.
 */
Qairt_Status_t QairtProfile_Event_setBackendOpaqueObject(
    QairtProfile_EventHandle_t eventHandle,
    QairtProfile_BackendOpaqueObjectHandle_t backendOpaqueObjectHandle);

/**
 * @brief A function to get the backend opaque object of a profile event if supported.
 *
 * @param[in] eventHandle A profile event handle.
 *
 * @param[out] backendOpaqueObjectHandle A handle to a backend opaque object for this profiling
 *                                       event. If this event does not support extended data fields,
 *                                       an error will be returned.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_PROFILE_ERROR_INVALID_HANDLE: _eventHandle_ is not a valid handle
 *         - QAIRT_PROFILE_ERROR_INVALID_ARGUMENT: _backendOpaqueObjectHandle_ is invalid
 *         - QAIRT_PROFILE_ERROR_DATA_NOT_AVAILABLE: data is not available for this event
 *
 * @note Use corresponding API through QairtProfile_EventV1_t.
 */
Qairt_Status_t QairtProfile_Event_getBackendOpaqueObject(
    QairtProfile_EventHandle_t eventHandle,
    QairtProfile_BackendOpaqueObjectHandle_t* backendOpaqueObjectHandle);

typedef Qairt_Status_t (*QairtProfile_Event_CreateFn_t)(QairtProfile_EventHandle_t*);

typedef Qairt_Status_t (*QairtProfile_Event_FreeFn_t)(QairtProfile_EventHandle_t);

typedef Qairt_Status_t (*QairtProfile_Event_SupportsExtendedEventFn_t)(
    QairtProfile_EventHandle_t, uint8_t*);

typedef Qairt_Status_t (*QairtProfile_Event_SetEventTypeFn_t)(QairtProfile_EventHandle_t,
                                                                   QairtProfile_EventType_t);

typedef Qairt_Status_t (*QairtProfile_Event_GetEventTypeFn_t)(QairtProfile_EventHandle_t,
                                                                   QairtProfile_EventType_t*);

typedef Qairt_Status_t (*QairtProfile_Event_SetUnitFn_t)(QairtProfile_EventHandle_t,
                                                              QairtProfile_EventUnit_t);

typedef Qairt_Status_t (*QairtProfile_Event_GetUnitFn_t)(QairtProfile_EventHandle_t,
                                                              QairtProfile_EventUnit_t*);

typedef Qairt_Status_t (*QairtProfile_Event_SetIdentifierFn_t)(QairtProfile_EventHandle_t,
                                                                    const char*);

typedef Qairt_Status_t (*QairtProfile_Event_GetIdentifierFn_t)(QairtProfile_EventHandle_t,
                                                                    const char**);

typedef Qairt_Status_t (*QairtProfile_Event_SetTimestampFn_t)(QairtProfile_EventHandle_t,
                                                                   uint64_t);

typedef Qairt_Status_t (*QairtProfile_Event_GetTimestampFn_t)(QairtProfile_EventHandle_t,
                                                                   uint64_t*);

typedef Qairt_Status_t (*QairtProfile_Event_SetScalarFn_t)(QairtProfile_EventHandle_t,
                                                                QairtScalar_Handle_t);

typedef Qairt_Status_t (*QairtProfile_Event_GetScalarFn_t)(QairtProfile_EventHandle_t,
                                                                QairtScalar_Handle_t*);

typedef Qairt_Status_t (*QairtProfile_Event_SetBackendOpaqueObjectFn_t)(
    QairtProfile_EventHandle_t, QairtProfile_BackendOpaqueObjectHandle_t);

typedef Qairt_Status_t (*QairtProfile_Event_GetBackendOpaqueObjectFn_t)(
    QairtProfile_EventHandle_t, QairtProfile_BackendOpaqueObjectHandle_t*);

//=============================================================================
//  Public Interfaces
//=============================================================================

typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtProfile_Event_CreateFn_t create;
  QairtProfile_Event_FreeFn_t free;
  QairtProfile_Event_SupportsExtendedEventFn_t supportsExtendedEvent;
  QairtProfile_Event_SetEventTypeFn_t setEventType;
  QairtProfile_Event_GetEventTypeFn_t getEventType;
  QairtProfile_Event_SetUnitFn_t setUnit;
  QairtProfile_Event_GetUnitFn_t getUnit;
  QairtProfile_Event_SetIdentifierFn_t setIdentifier;
  QairtProfile_Event_GetIdentifierFn_t getIdentifier;
  QairtProfile_Event_SetTimestampFn_t setTimestamp;
  QairtProfile_Event_GetTimestampFn_t getTimestamp;
  QairtProfile_Event_SetScalarFn_t setScalar;
  QairtProfile_Event_GetScalarFn_t getScalar;
  QairtProfile_Event_SetBackendOpaqueObjectFn_t setBackendOpaqueObject;
  QairtProfile_Event_GetBackendOpaqueObjectFn_t getBackendOpaqueObject;
} QairtProfile_EventV1_t;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // QAIRT_PROFILEEVENT_H

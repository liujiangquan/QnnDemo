//=============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//=============================================================================

#ifndef QAIRT_PROFILE_H
#define QAIRT_PROFILE_H

#include "QairtCommon/QairtCommon.h"
#include "QairtProfileBackendOpaqueObject.h"
#include "QairtProfileConfig.h"
#include "QairtProfileEvent.h"

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
// Macros
//=============================================================================
#define QAIRT_PROFILE_V1_ID                       QAIRT_MIN_ID_PROFILE
#define QAIRT_PROFILE_EVENT_V1_ID                 QAIRT_MIN_ID_PROFILE + 1
#define QAIRT_PROFILE_CONFIG_V1_ID                QAIRT_MIN_ID_PROFILE + 2
#define QAIRT_PROFILE_BACKEND_OPAQUE_OBJECT_V1_ID QAIRT_MIN_ID_PROFILE + 3

/**
 * @brief Basic QairtProfile_Level_t definition that allows to collect performance
 *        metrics for graph finalization and execution stages.
 */
#define QAIRT_PROFILE_LEVEL_BASIC 1
/**
 * @brief Detailed QairtProfile_Level_t definition that allows to collect performance
 *        metrics for each operation in the graph
 */
#define QAIRT_PROFILE_LEVEL_DETAILED 2
/**
 * @brief QairtProfile_Level_t definition reserved for each back end to define and
 *        extend
 */
#define QAIRT_PROFILE_LEVEL_BACKEND 1000

//=============================================================================
// Data Types
//=============================================================================
typedef struct _QairtProfile_Handle_t* QairtProfile_Handle_t;
typedef uint32_t QairtProfile_Level_t;

typedef enum {
  QAIRT_PROFILE_MIN_ERROR = QAIRT_MIN_ERROR_PROFILE,
  ////////////////////////////////////////////
  /// Qairt Profile success
  QAIRT_PROFILE_NO_ERROR = QAIRT_SUCCESS,
  /// Backend does not support requested functionality
  QAIRT_PROFILE_ERROR_UNSUPPORTED = QAIRT_COMMON_ERROR_NOT_SUPPORTED,
  /// Invalid function argument
  QAIRT_PROFILE_ERROR_INVALID_ARGUMENT = QAIRT_COMMON_ERROR_INVALID_ARGUMENT,
  /// General error relating to memory allocation in Profile API
  QAIRT_PROFILE_ERROR_MEM_ALLOC = QAIRT_COMMON_ERROR_MEM_ALLOC,
  /// Invalid/NULL QAIRT profile handle
  QAIRT_PROFILE_ERROR_INVALID_HANDLE = 12000,
  /// Attempt to free or reconfigure a profile handle that is in-use
  QAIRT_PROFILE_ERROR_HANDLE_IN_USE = 12001,
  /// Event is incompatible with API
  QAIRT_PROFILE_ERROR_INCOMPATIBLE_EVENT = 12002,
  /// Event does not have requested data field
  QAIRT_PROFILE_ERROR_DATA_NOT_AVAILABLE = 12003,
  ////////////////////////////////////////////
  QAIRT_PROFILE_MAX_ERROR = QAIRT_MAX_ERROR_PROFILE,
  // Unused, present to ensure 32 bits.
  QAIRT_PROFILE_ERROR_UNDEFINED = 0x7FFFFFFF
} QairtProfile_Error_t;

//=============================================================================
// Public Functions
//=============================================================================

/**
 * @brief Free a profile handle and associated resources.
 *
 * @param[in] profileHandle A handle to a created profile.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Profile was successfully freed.
 *         - QAIRT_PROFILE_ERROR_INVALID_HANDLE: _profileHandle_ is not a valid handle.
 *         - QAIRT_PROFILE_ERROR_MEM_ALLOC: Memory deallocation error.
 *
 * @note Use corresponding API through QairtProfile_V1_t.
 */
Qairt_Status_t QairtProfile_free(QairtProfile_Handle_t profileHandle);

/**
 * @brief A function to set/modify configuration options on an already created profile.
 *
 * @param[in] profileHandle A profile handle.
 *
 * @param[in] configHandle A created profile config handle. NULL is allowed and indicates no config
 *                         options are provided. All config options have default value, in case not
 *                         provided. If same config option type is provided multiple times, the
 *                         last option value will be used. If a backend cannot support all provided
 *                         configs it will fail.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_PROFILE_ERROR_INVALID_HANDLE: _profileHandle_ or _configHandle_ is not a valid
 *                                              handle
 *         - QAIRT_PROFILE_ERROR_INVALID_ARGUMENT: at least one config option is invalid
 *         - QAIRT_PROFILE_ERROR_HANDLE_IN_USE: when attempting to reconfigure a profile handle
 *         - QAIRT_PROFILE_ERROR_UNSUPPORTED: at least one valid config option is not
 *                                                    supported
 *
 * @note Use corresponding API through QairtProfile_V1_t.
 */
Qairt_Status_t QairtProfile_setConfig(QairtProfile_Handle_t profileHandle,
                                           QairtProfile_ConfigHandle_t configHandle);

/**
 * @brief Get the number of profile events collected on the profile handle.
 *
 * @param[in] profileHandle A profile handle.
 *
 * @param[out] numEvents Number of profile events.
 *
 * @note profileEvents parameter: profile event memory is associated with the profile object and
 *       released on profile object release in QairtProfile_free().
 *
 * @return Error code
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_PROFILE_ERROR_INVALID_HANDLE: _profileHandle_ is not a valid handle.
 *         - QAIRT_PROFILE_ERROR_INVALID_ARGUMENT: _numEvents_ is NULL.
 *
 * @note Use corresponding API through QairtProfile_V1_t.
 */
Qairt_Status_t QairtProfile_getNumEvents(QairtProfile_Handle_t profileHandle,
                                              uint32_t* numEvents);

/**
 * @brief Get profile events collected on the profile handle at a specific index.
 *
 * @param[in] profileHandle A profile handle.
 *
 * @param[in] index index of event to return.
 *
 * @param[out] eventHandle Returns handle to profile event at specified index
 *
 * @note profileEvents parameter: profile event memory is associated with the profile object and
 *       released on profile object release in QairtProfile_free().
 *
 * @return Error code
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_PROFILE_ERROR_INVALID_HANDLE: _profileHandle_ is not a valid handle.
 *         - QAIRT_PROFILE_ERROR_INVALID_ARGUMENT: _eventHandles_ or _numEvents_ is NULL.
 *
 * @note Use corresponding API through QairtProfile_V1_t.
 */
Qairt_Status_t QairtProfile_getEventAt(QairtProfile_Handle_t profileHandle,
                                            uint32_t index,
                                            QairtProfile_EventHandle_t* eventHandle);

/**
 * @brief Get the number of profile sub-events collected on the profile event handle.
 *
 * @param[in] profileHandle A profile handle.
 *
 * @param[in] eventHandle A profile event handle.
 *
 * @param[out] numSubEvents The number of subevents.
 *
 * @note profileEvents parameter: profile event memory is associated with the profile object and
 *       released on profile object release in QairtProfile_free().
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_PROFILE_ERROR_INVALID_HANDLE: _profileHandle_ or _eventHandle_ is not a valid
 *                                               handle
 *         - QAIRT_PROFILE_ERROR_INVALID_ARGUMENT: _numSubEvents_ is invalid
 *
 * @note Use corresponding API through QairtProfile_EventV1_t.
 */
Qairt_Status_t QairtProfile_getNumSubEvents(QairtProfile_Handle_t profileHandle,
                                                 QairtProfile_EventHandle_t eventHandle,
                                                 uint64_t* numSubEvents);

/**
 * @brief Get profile events collected on the profile handle at a specific index.
 *
 * @param[in] profileHandle A profile handle.
 *
 * @param[in] eventHandle A profile event handle.
 *
 * @param[out] index Index of sub-event to return.
 *
 * @param[out] subEventHandle Returns handle to profile sub-event at specified index
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_PROFILE_ERROR_INVALID_HANDLE: _profileHandle_ or _eventHandle_ is not a valid
 *                                               handle
 *         - QAIRT_PROFILE_ERROR_INVALID_ARGUMENT: _numSubEvents_ is invalid
 *
 * @note Use corresponding API through QairtProfile_EventV1_t.
 */
Qairt_Status_t QairtProfile_getSubEventAt(QairtProfile_Handle_t profileHandle,
                                               QairtProfile_EventHandle_t eventHandle,
                                               uint32_t index,
                                               QairtProfile_EventHandle_t* subEventHandle);

typedef Qairt_Status_t (*QairtProfile_FreeFn_t)(QairtProfile_Handle_t);

typedef Qairt_Status_t (*QairtProfile_SetConfigFn_t)(QairtProfile_Handle_t,
                                                          QairtProfile_ConfigHandle_t);

typedef Qairt_Status_t (*QairtProfile_GetNumEventsFn_t)(QairtProfile_Handle_t, uint32_t*);

typedef Qairt_Status_t (*QairtProfile_GetEventAtFn_t)(QairtProfile_Handle_t,
                                                           uint32_t,
                                                           QairtProfile_EventHandle_t*);

typedef Qairt_Status_t (*QairtProfile_GetNumSubEventsFn_t)(QairtProfile_Handle_t,
                                                                QairtProfile_EventHandle_t,
                                                                uint64_t*);

typedef Qairt_Status_t (*QairtProfile_GetSubEventAtFn_t)(QairtProfile_Handle_t,
                                                              QairtProfile_EventHandle_t,
                                                              uint32_t,
                                                              QairtProfile_EventHandle_t*);

//=============================================================================
// Public Interfaces
//=============================================================================

typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtProfile_FreeFn_t free;
  QairtProfile_SetConfigFn_t setConfig;
  QairtProfile_GetNumEventsFn_t getNumEvents;
  QairtProfile_GetEventAtFn_t getEventAt;
  QairtProfile_GetNumSubEventsFn_t getNumSubEvents;
  QairtProfile_GetSubEventAtFn_t getSubEventAt;
} QairtProfile_V1_t;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // QAIRT_PROFILE_H

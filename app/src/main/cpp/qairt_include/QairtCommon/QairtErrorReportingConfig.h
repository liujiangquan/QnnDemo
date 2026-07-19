//==============================================================================
//
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// All rights reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//==============================================================================

/**
 * @file
 * @brief   Error Reporting Config component API
 *
 *          A header which contains the QAIRT Error Reporting Config component
 *          and the interfaces it implements.
 */

#ifndef QAISW_QAIRT_ERROR_REPORTING_CONFIG_H
#define QAISW_QAIRT_ERROR_REPORTING_CONFIG_H

#include "QairtCommon/QairtCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
// Macros
//=============================================================================
#define QAIRT_ERROR_REPORTING_CONFIG_V1_ID QAIRT_MIN_ID_ERROR

//=============================================================================
// Datatypes
//=============================================================================

/**
 * @brief QAIRT Error Reporting Config API result / error codes.
 */
typedef enum {
  QAIRT_ERROR_REPORTING_CONFIG_MIN_ERROR = QAIRT_MIN_ERROR_ERROR,
  ////////////////////////////////////

  /// Qairt Error Reporting Config success
  QAIRT_ERROR_REPORTING_CONFIG_NO_ERROR = QAIRT_SUCCESS,
  /// General error relating to memory allocation in Error Reporting Config API
  QAIRT_ERROR_REPORTING_CONFIG_ERROR_MEM_ALLOC = QAIRT_COMMON_ERROR_MEM_ALLOC,
  /// Unable to initialize error reporting config
  QAIRT_ERROR_REPORTING_CONFIG_ERROR_INITIALIZATION = 10002,
  /// Invalid argument passed
  QAIRT_ERROR_REPORTING_CONFIG_ERROR_INVALID_ARGUMENT = 10003,
  /// Invalid error reporting config handle passed
  QAIRT_ERROR_REPORTING_CONFIG_ERROR_INVALID_HANDLE = 10004,
  ////////////////////////////////////
  QAIRT_ERROR_REPORTING_CONFIG_MAX_ERROR = QAIRT_MAX_ERROR_ERROR,
  // Unused, present to ensure 32 bits.
  QAIRT_ERROR_REPORTING_CONFIG_ERROR_UNDEFINED = 0x7FFFFFFF
} QairtErrorReporting_Config_Error_t;

/**
 * @brief An enum to describe reporting levels for the error handling API
 * QAIRT_ERROR_REPORTING_LEVEL_BRIEF: get basic information about an error
 * QAIRT_ERROR_REPORTING_LEVEL_DETAILED: get detailed information about an error
 * in memory-based object forms
 */
typedef enum {
  QAIRT_ERROR_REPORTING_LEVEL_BRIEF    = 0,
  QAIRT_ERROR_REPORTING_LEVEL_DETAILED = 1,
  // Unused, present to ensure 32 bits.
  QAIRT_ERROR_REPORTING_LEVEL_UNDEFINED = 0x7FFFFFFF
} QairtErrorReporting_Config_Level_t;

/**
 * @brief   Error Reporting Config handle used to interface with Error Reporting Config APIs
 *
 *          The handle itself is created and freed by the client.
 */
typedef struct _QairtErrorReporting_ConfigHandle_t* QairtErrorReporting_ConfigHandle_t;

//=============================================================================
// Public Functions
//=============================================================================

/**
 * @brief Create a handle to an error reporting config object.
 *
 * @param[in] reportingLevel Error reporting level to be used.
 * @param[in] storageLimit Amount of memory to be reserved for error information.
 *                         Specified in KB.
 * @param[out] configHandle The created error reporting config handle.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: if error reporting config is successfully created.
 *         - QAIRT_COMMON_ERROR_NOT_SUPPORTED: error reporting config is not supported.
 *         - QAIRT_ERROR_REPORTING_CONFIG_ERROR_INVALID_ARGUMENT: if one or more arguments is
 *           invalid.
 *         - QAIRT_ERROR_REPORTING_CONFIG_ERROR_MEM_ALLOC: for memory allocation errors.
 *         - QAIRT_ERROR_REPORTING_CONFIG_ERROR_INITIALIZATION: config init failed.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurence
 *           (successful recovery)
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurence
 *           (unsuccessful recovery)
 *
 * @note Use corresponding API through QairtErrorReporting_Config_V1_t.
 */
Qairt_Status_t QairtErrorReporting_Config_create(
    QairtErrorReporting_Config_Level_t reportingLevel,
    uint32_t storageLimit,
    QairtErrorReporting_ConfigHandle_t* configHandle);

/**
 * @brief Frees an error reporting config handle
 *
 * @param[in] configHandle A handle to a created error reporting config.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered, config is freed.
 *         - QAIRT_ERROR_REPORTING_CONFIG_ERROR_MEM_ALLOC: error related to memory
 *           deallocation
 *         - QAIRT_ERROR_REPORTING_CONFIG_ERROR_INVALID_HANDLE: _configHandle_ is not a valid
 *           handle
 *
 * @note Use corresponding API through QairtErrorReporting_Config_V1_t.
 */
Qairt_Status_t QairtErrorReporting_Config_free(
    QairtErrorReporting_ConfigHandle_t configHandle);

/**
 * @brief Get the reporting level from an error reporting config handle.
 *
 * @param[in] configHandle A created error reporting config handle.
 * @param[out] reportingLevel The current reporting level.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: if the reporting level is retrieved successfully.
 *         - QAIRT_ERROR_REPORTING_CONFIG_ERROR_INVALID_ARGUMENT: if reportingLevel is NULL.
 *         - QAIRT_ERROR_REPORTING_CONFIG_ERROR_INVALID_HANDLE: _configHandle_ is not a valid
 *           handle
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurence
 *           (successful recovery)
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurence
 *           (unsuccessful recovery)
 *
 * @note Use corresponding API through QairtErrorReporting_Config_V1_t.
 */
Qairt_Status_t QairtErrorReporting_Config_getReportingLevel(
    QairtErrorReporting_ConfigHandle_t configHandle,
    QairtErrorReporting_Config_Level_t* reportingLevel);

/**
 * @brief Set the reporting level for an error reporting config handle.
 *
 * @param[in] configHandle A created error reporting config handle.
 * @param[in] reportingLevel New reporting level.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: if the reporting level is set successfully.
 *         - QAIRT_ERROR_REPORTING_CONFIG_ERROR_INVALID_ARGUMENT: if reportingLevel is not a
 *           valid QairtErrorReporting_Config_Level_t level.
 *         - QAIRT_ERROR_REPORTING_CONFIG_ERROR_INVALID_HANDLE: _configHandle_ is not a valid
 *           handle
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurence
 *           (successful recovery)
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurence
 *           (unsuccessful recovery)
 *
 * @note Use corresponding API through QairtErrorReporting_Config_V1_t.
 */
Qairt_Status_t QairtErrorReporting_Config_setReportingLevel(
    QairtErrorReporting_ConfigHandle_t configHandle,
    QairtErrorReporting_Config_Level_t reportingLevel);

/**
 * @brief Get the storage limit from an error reporting config handle.
 *
 * @param[in] configHandle A created error reporting config handle.
 * @param[out] storageLimit The current storage limit in KB.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: if the storage limit is retrieved successfully.
 *         - QAIRT_ERROR_REPORTING_CONFIG_ERROR_INVALID_ARGUMENT: if storageLimit is NULL.
 *         - QAIRT_ERROR_REPORTING_CONFIG_ERROR_INVALID_HANDLE: _configHandle_ is not a valid
 *           handle
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurence
 *           (successful recovery)
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurence
 *           (unsuccessful recovery)
 *
 * @note Use corresponding API through QairtErrorReporting_Config_V1_t.
 */
Qairt_Status_t QairtErrorReporting_Config_getStorageLimit(
    QairtErrorReporting_ConfigHandle_t configHandle, uint32_t* storageLimit);

/**
 * @brief Set the storage limit for an error reporting config handle.
 *
 * @param[in] configHandle A created error reporting config handle.
 * @param[in] storageLimit New storage limit in KB.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: if the storage limit is set successfully.
 *         - QAIRT_ERROR_REPORTING_CONFIG_ERROR_INVALID_ARGUMENT: if storageLimit is invalid.
 *         - QAIRT_ERROR_REPORTING_CONFIG_ERROR_INVALID_HANDLE: _configHandle_ is not a valid
 *           handle
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurence
 *           (successful recovery)
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurence
 *           (unsuccessful recovery)
 *
 * @note Use corresponding API through QairtErrorReporting_Config_V1_t.
 */
Qairt_Status_t QairtErrorReporting_Config_setStorageLimit(
    QairtErrorReporting_ConfigHandle_t configHandle, uint32_t storageLimit);

//=============================================================================
// Function Pointer Types
//=============================================================================

typedef Qairt_Status_t (*QairtErrorReporting_Config_CreateFn_t)(
    QairtErrorReporting_Config_Level_t, uint32_t, QairtErrorReporting_ConfigHandle_t*);

typedef Qairt_Status_t (*QairtErrorReporting_Config_FreeFn_t)(
    QairtErrorReporting_ConfigHandle_t);

typedef Qairt_Status_t (*QairtErrorReporting_Config_GetReportingLevelFn_t)(
    QairtErrorReporting_ConfigHandle_t, QairtErrorReporting_Config_Level_t*);

typedef Qairt_Status_t (*QairtErrorReporting_Config_SetReportingLevelFn_t)(
    QairtErrorReporting_ConfigHandle_t, QairtErrorReporting_Config_Level_t);

typedef Qairt_Status_t (*QairtErrorReporting_Config_GetStorageLimitFn_t)(
    QairtErrorReporting_ConfigHandle_t, uint32_t*);

typedef Qairt_Status_t (*QairtErrorReporting_Config_SetStorageLimitFn_t)(
    QairtErrorReporting_ConfigHandle_t, uint32_t);

//=============================================================================
// Public Interfaces
//=============================================================================

/**
 * @struct QairtErrorReporting_Config_V1_t
 * @brief This struct contains the functions that define the V1 interface for
 *        QAIRT Error Reporting Config. This interface can be retrieved through
 *        the id QAIRT_ERROR_REPORTING_CONFIG_V1_ID.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtErrorReporting_Config_CreateFn_t create;
  QairtErrorReporting_Config_FreeFn_t free;
  QairtErrorReporting_Config_GetReportingLevelFn_t getReportingLevel;
  QairtErrorReporting_Config_SetReportingLevelFn_t setReportingLevel;
  QairtErrorReporting_Config_GetStorageLimitFn_t getStorageLimit;
  QairtErrorReporting_Config_SetStorageLimitFn_t setStorageLimit;
} QairtErrorReporting_Config_V1_t;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // QAISW_QAIRT_ERROR_REPORTING_CONFIG_H

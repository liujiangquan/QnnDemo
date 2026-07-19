//==============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All Rights Reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//==============================================================================

/**
 * @file
 * @brief   QAIRT System Common API component
 *
 *          A header which contains common types shared by QAIRT system components.
 *          This simplifies the cross-inclusion of headers.
 */

#ifndef QAIRT_SYSTEM_COMMON_H
#define QAIRT_SYSTEM_COMMON_H

#include "QairtCommon/QairtCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
// Error Codes
//=============================================================================

#define QAIRT_SYSTEM_COMMON_MIN_ERROR  QAIRT_MIN_ERROR_SYSTEM
#define QAIRT_SYSTEM_COMMON_MAX_ERROR  QAIRT_MIN_ERROR_SYSTEM + 999
#define QAIRT_SYSTEM_PROFILE_MIN_ERROR QAIRT_MIN_ERROR_SYSTEM + 1000
#define QAIRT_SYSTEM_PROFILE_MAX_ERROR QAIRT_MIN_ERROR_SYSTEM + 1999

/**
 * @brief QAIRT System Profile API result / error codes.
 */
typedef enum {
  /// QAIRT System success
  QAIRT_SYSTEM_COMMON_NO_ERROR = QAIRT_SUCCESS,
  /// There is an API component that is not supported yet.
  QAIRT_SYSTEM_COMMON_ERROR_UNSUPPORTED_FEATURE = QAIRT_COMMON_ERROR_NOT_SUPPORTED,
  /// QAIRT System invalid handle
  QAIRT_SYSTEM_COMMON_ERROR_INVALID_HANDLE = 30000,
  /// One or more arguments to a System API is/are NULL/invalid.
  QAIRT_SYSTEM_COMMON_ERROR_INVALID_ARGUMENT = 30001
} QairtSystemCommon_Error_t;

//=============================================================================
// Macros
//=============================================================================

// QAIRT System library interface provider name
#define QAIRT_SYSTEM_INTERFACE_PROVIDER_NAME "SYSTEM_QTI_AISW"

// Macro controlling visibility of QAIRT_SYSTEM API
#ifndef QAIRT_SYSTEM_API
#define QAIRT_SYSTEM_API
#endif

// Provide values to use for API version.
#define QAIRT_SYSTEM_API_VERSION_MAJOR 0
#define QAIRT_SYSTEM_API_VERSION_MINOR 1
#define QAIRT_SYSTEM_API_VERSION_PATCH 0

// Error code space assigned to system API components
#define QAIRT_SYSTEM_CONTEXT_MIN_ERROR QAIRT_MIN_ERROR_SYSTEM
#define QAIRT_SYSTEM_CONTEXT_MAX_ERROR (QAIRT_SYSTEM_CONTEXT_MIN_ERROR + 999)

//=============================================================================
// Data Types
//=============================================================================

//=============================================================================
// Public Functions
//=============================================================================

/**
 * @brief Retrieve a system interface by its unique identifier.
 *
 * This is the entry-point symbol exported by the QAIRT System library
 * (libQairtSystem.so).
 *
 * @param[in] id The unique identifier for the interface to retrieve.
 * @param[out] interfaceOut A pointer to be populated with the requested interface.
 * @return Error code:
 *         - QAIRT_SUCCESS: Interface was successfully retrieved.
 *         - QAIRT_INTERFACE_ERROR_NOT_SUPPORTED: The requested interface
 *           ID is not supported.
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _interfaceOut_ is NULL.
 **/
#ifndef QAIRT_SYSTEM_INTERFACE
#define QAIRT_SYSTEM_INTERFACE
#endif

QAIRT_SYSTEM_INTERFACE
Qairt_Status_t QairtSystemInterface_getInterface(uint64_t id, void** interfaceOut);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // QAIRT_SYSTEM_COMMON_H
//==============================================================================
//
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// All rights reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//==============================================================================

/**
 * @file
 * @brief   QAIRT Info API Component
 *
 *          A header which contains the QAIRT info component and the interfaces
 *          it implements.
 */

#ifndef QAISW_QAIRTINFO_H
#define QAISW_QAIRTINFO_H

#include "QairtBackendCapabilityKeys.h"
#include "QairtCommon/QairtCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
// Macros
//=============================================================================
#define QAIRT_INFO_V1_ID QAIRT_MIN_ID_INFO

//=============================================================================
// Datatypes
//=============================================================================
/**
 * @brief QAIRT API result / error codes.
 */
typedef enum {
  QAIRT_CAPABILITY_MIN_ERROR = QAIRT_MIN_ERROR_CAPABILITY,
  //////////////////////////////////////////////

  QAIRT_CAPABILITY_NO_ERROR = QAIRT_SUCCESS,
  /// Capability in question is supported
  QAIRT_CAPABILITY_SUPPORTED = QAIRT_SUCCESS,
  /// Capability in question not supported.
  QAIRT_CAPABILITY_NOT_SUPPORTED = QAIRT_COMMON_ERROR_NOT_SUPPORTED,

  // Remaining values signal errors.

  /// Backend did not recognize the capability key.
  QAIRT_CAPABILITY_ERROR_UNKNOWN_KEY = 2000,

  //////////////////////////////////////////////
  QAIRT_CAPABILITY_MAX_ERROR = QAIRT_MAX_ERROR_CAPABILITY,
  // Unused, present to ensure 32 bits.
  QAIRT_CAPABILITY_ERROR_UNDEFINED = 0x7FFFFFFF
} QairtCapability_Error_t;

//=============================================================================
// Public Functions
//=============================================================================
/**
 * @brief Gets the backend API version. Can be retrieved without creating a
 *        backend.
 *
 * @param[out] major The major version value
 *
 * @param[out] minor The minor version value
 *
 * @param[out] patch The patch version value
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: if _major_,_minor_, or
 *           _patch_ was NULL
 *
 * @note Use corresponding API through QairtInfo_V1_t.
 */
Qairt_Status_t QairtInfo_getBackendApiVersion(uint32_t* major, uint32_t* minor, uint32_t* patch);

/**
 * @brief Gets the core API version. Can be retrieved without creating a
 *        backend.
 *
 * @param[out] major The major version value
 *
 * @param[out] minor The minor version value
 *
 * @param[out] patch The patch version value
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: if _major_,_minor_, or
 *           _patch_ was NULL
 *
 * @note Use corresponding API through QairtInfo_V1_t.
 */
Qairt_Status_t QairtInfo_getCoreApiVersion(uint32_t* major, uint32_t* minor, uint32_t* patch);

/**
 * @brief Get build id for backend library.
 *
 * @note Safe to call any time, backend does not have to be created.
 *
 * @param[out] id Pointer to string containing the build id.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_COMMON_ERROR_NOT_SUPPORTED: No build ID is available
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: if _id_ is NULL
 *
 * @note Use corresponding API through QairtInfo_V1_t.
 */
Qairt_Status_t QairtInfo_getBuildId(const char** id);

/**
 * @brief Queries a capability of the backend.
 *
 * @note Safe to call any time, backend does not have to be created.
 * @param[in] key Key which identifies the capability within group.
 *
 * @return Error code:
 *         - QAIRT_CAPABILITY_SUPPORTED: if the backend supports capability.
 *         - QAIRT_CAPABILITY_ERROR_UNKNOWN_KEY: The provided key is not
 *           valid.
 *         - QAIRT_CAPABILITY_NOT_SUPPORTED: if the backend does not support
 *           capability.
 *
 * @note Use corresponding API through QairtInfo_V1_t.
 */
Qairt_Status_t QairtInfo_hasCapability(QairtCapability_Key_t key);

typedef Qairt_Status_t (*QairtInfo_GetBackendApiVersionFn_t)(uint32_t*, uint32_t*, uint32_t*);

typedef Qairt_Status_t (*QairtInfo_GetCoreApiVersionFn_t)(uint32_t*, uint32_t*, uint32_t*);

typedef Qairt_Status_t (*QairtInfo_GetBuildIdFn_t)(const char**);

typedef Qairt_Status_t (*QairtInfo_HasCapabilityFn_t)(QairtCapability_Key_t);

//=============================================================================
// Public Interfaces
//=============================================================================
/**
 * @struct QairtInfo_V1_t
 * @brief This struct contains the functions that define the V1 interface for
 *        QAIRT Info. This interface can be retrieved through the id
 *        QAIRT_INFO_V1_ID.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtInfo_GetBackendApiVersionFn_t getBackendApiVersion;
  QairtInfo_GetCoreApiVersionFn_t getCoreApiVersion;
  QairtInfo_GetBuildIdFn_t getBuildId;
  QairtInfo_HasCapabilityFn_t hasCapability;
} QairtInfo_V1_t;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // QAISW_QAIRTINFO_H

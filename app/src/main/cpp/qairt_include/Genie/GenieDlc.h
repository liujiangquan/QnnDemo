//=============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All Rights Reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//=============================================================================

/**
 *  @file
 *  @brief  DLC API providing model loading functionality.
 */

#ifndef GENIE_DLC_H
#define GENIE_DLC_H

#include "GenieCommon.h"
#include "GenieLog.h"

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
// Data Types
//=============================================================================

/**
 * @brief A handle for DLC configuration instances.
 */
typedef const struct _GenieDlcConfig_Handle_t* GenieDlcConfig_Handle_t;

/**
 * @brief A handle for DLC instances.
 */
typedef const struct _GenieDlc_Handle_t* GenieDlc_Handle_t;

//=============================================================================
// Functions
//=============================================================================

/**
* @brief A function to create a DLC configuration from a source.
*
* @param[in] dlcSource The source DLC.
*
* @param[in] jsonStr The global configuration string.
*
* @param[out] configHandle A handle to the created config. Must not be NULL.
*
* @return Status code:
*         - GENIE_STATUS_SUCCESS: API call was successful.
*         - GENIE_STATUS_ERROR_INVALID_ARGUMENT: At least one argument is invalid.
*         - GENIE_STATUS_ERROR_MEM_ALLOC: Memory allocation failure.
*         - GENIE_STATUS_ERROR_INVALID_CONFIG: At least one configuration option is invalid.
*/
GENIE_API
Genie_Status_t GenieDlcConfig_create(const char* dlcSource,
                                     const char* jsonStr,
                                     GenieDlcConfig_Handle_t* configHandle);

/**
 * @brief A function to free a DLC config.
 *
 * @param[in] configHandle A config handle.
 *
 * @return Status code:
 *         - GENIE_STATUS_SUCCESS: API call was successful.
 *         - GENIE_STATUS_ERROR_INVALID_HANDLE: DLC handle is invalid.
 *         - GENIE_STATUS_ERROR_MEM_ALLOC: Memory (de)allocation failure.
 */
GENIE_API
Genie_Status_t GenieDlcConfig_free(const GenieDlcConfig_Handle_t configHandle);

/**
 * @brief A function to create a DLC.
 *
 * @param[in] configHandle A handle to a valid config. Must not be NULL.
 *
 * @param[out] dlcHandle A handle to the created DLC. Must not be NULL.
 *
 * @return Status code:
 *         - GENIE_STATUS_SUCCESS: API call was successful.
 *         - GENIE_STATUS_ERROR_INVALID_HANDLE: Config handle is invalid.
 *         - GENIE_STATUS_ERROR_INVALID_ARGUMENT: At least one argument is invalid.
 *         - GENIE_STATUS_ERROR_MEM_ALLOC: Memory allocation failure.
 */
GENIE_API
Genie_Status_t GenieDlc_create(const GenieDlcConfig_Handle_t configHandle,
                               GenieDlc_Handle_t* dlcHandle);

/**
 * @brief A function to get all supported use cases from a DLC. The provided
 *        callback will be called for the client to provide memory allocation
 *        on which the use case strings will be copied.
 *
 * @param[in] dlcHandle A DLC handle.
 *
 * @param[in] callback A callback function handle. Must not be NULL.
 *
 * @param[out] useCases Information about the supported use cases. The associated
 *                      buffer was allocated in the client defined allocation callback
 *                      and the memory needs to be managed by the client.
 *
 * @return Status code:
 *         - GENIE_STATUS_SUCCESS: API call was successful.
 *         - GENIE_STATUS_ERROR_INVALID_HANDLE: DLC handle is invalid.
 *         - GENIE_STATUS_ERROR_INVALID_ARGUMENT: At least one argument is invalid.
 */
GENIE_API
Genie_Status_t GenieDlc_getUseCases(const GenieDlc_Handle_t dlcHandle,
                                    Genie_AllocCallback_t callback,
                                    const char** useCases);

/**
 * @brief A function to free a DLC.
 *
 * @param[in] dlcHandle A DLC handle.
 *
 * @return Status code:
 *         - GENIE_STATUS_SUCCESS: API call was successful.
 *         - GENIE_STATUS_ERROR_INVALID_HANDLE: DLC handle is invalid.
 *         - GENIE_STATUS_ERROR_MEM_ALLOC: Memory (de)allocation failure.
 */
GENIE_API
Genie_Status_t GenieDlc_free(const GenieDlc_Handle_t dlcHandle);

/**
 * @brief A function to bind a log handle to DLC config. The log handle
 *        will also be bound to any DLC handle created from this config handle.
 *
 * @param[in] configHandle A handle to a valid config.
 *
 * @param[in] logHandle The log handle using which logs are recorded and
 *                      outputted. Cannot be NULL.
 *
 * @return Status code:
 *         - GENIE_STATUS_SUCCESS: API call was successful.
 *         - GENIE_STATUS_ERROR_INVALID_HANDLE: Config handle or log handle is invalid.
 */
GENIE_API
Genie_Status_t GenieDlcConfig_bindLogger(const GenieDlcConfig_Handle_t configHandle,
                                         const GenieLog_Handle_t logHandle);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // GENIE_DLC_H
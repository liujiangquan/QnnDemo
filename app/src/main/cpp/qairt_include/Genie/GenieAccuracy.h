//=============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All Rights Reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//=============================================================================

/**
 *  @file
 *  @brief  API providing accuracy metrics functionality.
 */

#ifndef GENIE_ACCURACY_H
#define GENIE_ACCURACY_H

#include "GenieCommon.h"
#include "GenieDialog.h"
#include "GenieLog.h"
#include "GenieNode.h"

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
// Data Types
//=============================================================================

/**
 * @brief A handle for accuracy metrics configuration instances.
 */
typedef const struct _GenieAccuracyConfig_Handle_t* GenieAccuracyConfig_Handle_t;

/**
 * @brief A handle for an accuracy metrics instance.
 */
typedef const struct _GenieAccuracy_Handle_t* GenieAccuracy_Handle_t;

//=============================================================================
// Functions
//=============================================================================

/**
 * @brief A function to create an accuracy configuration from a JSON string.
 *
 * @param[in] str A configuration string. Must not be NULL.
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
Genie_Status_t GenieAccuracyConfig_createFromJson(const char* str,
                                                  GenieAccuracyConfig_Handle_t* configHandle);

/**
 * @brief A function to free an accuracy config.
 *
 * @param[in] configHandle A config handle.
 *
 * @return Status code:
 *         - GENIE_STATUS_SUCCESS: API call was successful.
 *         - GENIE_STATUS_ERROR_INVALID_HANDLE: Accuracy config handle is invalid.
 *         - GENIE_STATUS_ERROR_MEM_ALLOC: Memory (de)allocation failure.
 */
GENIE_API
Genie_Status_t GenieAccuracyConfig_free(const GenieAccuracyConfig_Handle_t configHandle);

/**
 * @brief A function to bind a log handle to accuracy config. The log handle
 *        will also be bound to any accuracy handle created from this config handle.
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
Genie_Status_t GenieAccuracyConfig_bindLogger(const GenieAccuracyConfig_Handle_t configHandle,
                                              const GenieLog_Handle_t logHandle);

/**
 * @brief A function to create an accuracy handle from an existing dialog.
 *
 * @param[in] configHandle A handle to a valid config. Must not be NULL.
 *
 * @param[in] dialogHandle A reference to the dialog handle which will be bound to the created
 *                         accuracy handle. Must not be NULL.
 *
 * @param[out] accuracyHandle A reference to the created accuracy handle. Must not be NULL.
 *
 * @return Status code:
 *         - GENIE_STATUS_SUCCESS: API call was successful.
 *         - GENIE_STATUS_ERROR_INVALID_ARGUMENT: At least one argument is invalid.
 *         - GENIE_STATUS_ERROR_INVALID_HANDLE: one of the handles provided in the arguments is
 *                                              invalid.
 *         - GENIE_STATUS_ERROR_MEM_ALLOC: Memory allocation failure.
 */
GENIE_API
Genie_Status_t GenieAccuracy_createFromDialog(const GenieAccuracyConfig_Handle_t configHandle,
                                              const GenieDialog_Handle_t dialogHandle,
                                              GenieAccuracy_Handle_t* accuracyHandle);

/**
 * @brief A function to create an accuracy handle from an existing node.
 *
 * @note Currently, only TextGenerators are supported for the node type.
 *
 * @param[in] configHandle A handle to a valid config. Can be NULL which indicates that
 *                         perplexity will be calculated.
 *
 * @param[in] nodeHandle A reference to the node handle which will be bound to the created
 *                         accuracy handle. Must not be NULL.
 *
 * @param[out] accuracyHandle A reference to the created accuracy handle. Must not be NULL.
 *
 * @return Status code:
 *         - GENIE_STATUS_SUCCESS: API call was successful.
 *         - GENIE_STATUS_ERROR_INVALID_ARGUMENT: At least one argument is invalid.
 *         - GENIE_STATUS_ERROR_INVALID_HANDLE: one of the handles provided in the arguments is
 *                                              invalid.
 *         - GENIE_STATUS_ERROR_MEM_ALLOC: Memory allocation failure.
 */
GENIE_API
Genie_Status_t GenieAccuracy_createFromNode(const GenieAccuracyConfig_Handle_t configHandle,
                                            const GenieNode_Handle_t nodeHandle,
                                            GenieAccuracy_Handle_t* accuracyHandle);

/**
 * @brief A function to compute an accuracy metric. The provided callback will be called for the
 *        client to provide memory allocation on which the JSON object will be copied.
 *
 * @note Running GenieAccuracy_compute resets the dialog that is bound to the accuracy handle, along
 *       with the associated engine. Ideally, this API should be ran on a newly created dialog,
 *       instead of in between dialog queries.
 *
 * @param[in] accuracyHandle Handle to the previously created accuracy handle, to which a dialog is
 *                           bound, which specifies which accuracy metrics to report.
 *
 * @param[in] callback A callback function handle. Must not be NULL.
 *
 * @param[out] jsonData The output accuracy data. The associated buffer was
 *                      allocated in the client defined allocation callback and
 *                      the memory needs to be managed by the client.
 *
 * @return Status code:
 *         - GENIE_STATUS_SUCCESS: API call was successful.
 *         - GENIE_STATUS_ERROR_INVALID_ARGUMENT: At least one argument is invalid.
 *         - GENIE_STATUS_ERROR_INVALID_HANDLE: one of the handles provided in the arguments is
 *                                              invalid.
 *         - GENIE_STATUS_ERROR_MEM_ALLOC: Memory allocation failure.
 */
GENIE_API
Genie_Status_t GenieAccuracy_compute(const GenieAccuracy_Handle_t accuracyHandle,
                                     Genie_AllocCallback_t callback,
                                     const char** jsonData);

/**
 * @brief A function to free an accuracy handle.
 *
 * @param[in] accuracyHandle An accuracy handle.
 *
 * @return Status code:
 *         - GENIE_STATUS_SUCCESS: API call was successful.
 *         - GENIE_STATUS_ERROR_INVALID_HANDLE: one of the handles provided in the arguments is
 *                                              invalid.
 */
GENIE_API
Genie_Status_t GenieAccuracy_free(const GenieAccuracy_Handle_t accuracyHandle);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // GENIE_ACCURACY_H
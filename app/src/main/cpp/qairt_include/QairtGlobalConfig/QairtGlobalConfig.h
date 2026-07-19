//==============================================================================
//
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// All rights reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//==============================================================================

/**
 * @file
 * @brief   Global Config component API
 *
 *          A header which contains the QAIRT Global Config component and the
 *          interfaces it implements. The Global Config API is process-global
 *          and intended for use only on x86 host environments where the
 *          underlying simulation backend cannot auto-discover the SoC model.
 *          The selected SoC model must be set before any other QAIRT API
 *          call that reads SoC information; subsequent calls are rejected.
 */

#ifndef QAISW_QAIRT_GLOBAL_CONFIG_H
#define QAISW_QAIRT_GLOBAL_CONFIG_H

#include "QairtCommon/QairtCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
// Macros
//=============================================================================
#define QAIRT_GLOBAL_CONFIG_V1_ID QAIRT_MIN_ID_GLOBAL_CONFIG

//=============================================================================
// Datatypes
//=============================================================================

/**
 * @brief QAIRT Global Config API result / error codes.
 */
typedef enum {
  QAIRT_GLOBAL_CONFIG_MIN_ERROR = QAIRT_MIN_ERROR_GLOBAL_CONFIG,
  ////////////////////////////////////

  /// QAIRT Global Config success
  QAIRT_GLOBAL_CONFIG_NO_ERROR = QAIRT_SUCCESS,
  /// Invalid config (e.g. unrecognized SoC model, or config attempted on a
  /// platform that does not support it).
  QAIRT_GLOBAL_CONFIG_ERROR_INVALID_CONFIG = 18000,
  ////////////////////////////////////
  QAIRT_GLOBAL_CONFIG_MAX_ERROR = QAIRT_MAX_ERROR_GLOBAL_CONFIG,
  // Unused, present to ensure 32 bits.
  QAIRT_GLOBAL_CONFIG_ERROR_UNDEFINED = 0x7FFFFFFF
} QairtGlobalConfig_Error_t;

//=============================================================================
// Public Functions
//=============================================================================

/**
 * @brief Set the SoC model to be used by simulation backends.
 *
 *        Process-global. Must be called before any other QAIRT API call that
 *        reads SoC information. Subsequent calls are rejected with
 *        QAIRT_COMMON_ERROR_OPERATION_NOT_PERMITTED. Supported only on x86
 *        host environments; on target platforms the underlying backend reports
 *        QAIRT_GLOBAL_CONFIG_ERROR_INVALID_CONFIG.
 *
 * @param[in] socModel SoC model identifier. Opaque numeric ID; a value of 0
 *                     corresponds to "unknown".
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: SoC model was accepted.
 *         - QAIRT_GLOBAL_CONFIG_ERROR_INVALID_CONFIG: socModel is invalid or
 *           the platform does not support setting it.
 *         - QAIRT_COMMON_ERROR_OPERATION_NOT_PERMITTED: SoC model has already
 *           been set or read by another module.
 *         - QAIRT_COMMON_ERROR_NOT_SUPPORTED: the loaded backend does not
 *           expose Global Config.
 *
 * @note Use corresponding API through QairtGlobalConfig_V1_t.
 */
Qairt_Status_t QairtGlobalConfig_setSocModel(uint32_t socModel);

//=============================================================================
// Function Pointer Types
//=============================================================================

typedef Qairt_Status_t (*QairtGlobalConfig_SetSocModelFn_t)(uint32_t);

//=============================================================================
// Public Interfaces
//=============================================================================

/**
 * @struct QairtGlobalConfig_V1_t
 * @brief This struct contains the functions that define the V1 interface for
 *        QAIRT Global Config. This interface can be retrieved through the id
 *        QAIRT_GLOBAL_CONFIG_V1_ID.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtGlobalConfig_SetSocModelFn_t setSocModel;
} QairtGlobalConfig_V1_t;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // QAISW_QAIRT_GLOBAL_CONFIG_H

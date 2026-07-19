//=============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//=============================================================================

#ifndef QAISW_QAIRTMEMCOMMON_H
#define QAISW_QAIRTMEMCOMMON_H
#include "QairtCommon/QairtCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
// Data Types
//=============================================================================

/**
 * @brief QAIRT Mem(ory) API result / error codes.
 */
typedef enum {
  QAIRT_MEM_MIN_ERROR = QAIRT_MIN_ERROR_MEM,
  ////////////////////////////////////

  /// Qairt Memory success
  QAIRT_MEM_NO_ERROR = QAIRT_SUCCESS,
  /// Backend does not support requested functionality
  QAIRT_MEM_ERROR_NOT_SUPPORTED = QAIRT_COMMON_ERROR_NOT_SUPPORTED,
  /// Invalid function argument
  QAIRT_MEM_ERROR_INVALID_ARGUMENT = 8000,
  /// Invalid memory handle
  QAIRT_MEM_ERROR_INVALID_HANDLE = 8001,
  /// Provided memory has already been registered
  QAIRT_MEM_ERROR_ALREADY_REGISTERED = 8002,
  /// Error in memory mapping
  QAIRT_MEM_ERROR_MAPPING = 8003,
  /// Invalid memory shape based on a backend's memory restrictions (e.g.
  /// alignment incompatibility)
  QAIRT_MEM_ERROR_INVALID_SHAPE = 8004,
  /// Backend does not support requested memory type
  QAIRT_MEM_ERROR_UNSUPPORTED_MEMTYPE = 8005,

  ////////////////////////////////////
  QAIRT_MEM_MAX_ERROR = QAIRT_MAX_ERROR_MEM,
  // Unused, present to ensure 32 bits.
  QAIRT_MEM_ERROR_UNDEFINED = 0x7FFFFFFF
} QairtMem_Error_t;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // QAISW_QAIRTMEMCOMMON_H

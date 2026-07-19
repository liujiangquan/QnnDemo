//==============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//==============================================================================

/**
 * @file
 * @brief   Device Common definitions
 *
 *          Common error codes and definitions for the Device component.
 */

#ifndef QAISW_QAIRTDEVICECOMMON_H
#define QAISW_QAIRTDEVICECOMMON_H

#include "QairtCommon/QairtCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
// Datatypes
//=============================================================================
/**
 * @brief QAIRT Device API result / error codes.
 */
typedef enum {
  QAIRT_DEVICE_MIN_ERROR = QAIRT_MIN_ERROR_DEVICE,
  ////////////////////////////////////////////

  /// Qairt Device success
  QAIRT_DEVICE_NO_ERROR = QAIRT_SUCCESS,
  /// There is optional API component that is not supported yet. See
  /// QairtCapability.
  QAIRT_DEVICE_ERROR_UNSUPPORTED_FEATURE = QAIRT_COMMON_ERROR_NOT_SUPPORTED,
  /// Memory allocation/deallocation failure
  QAIRT_DEVICE_ERROR_MEM_ALLOC = QAIRT_COMMON_ERROR_MEM_ALLOC,
  /// Invalid function argument
  QAIRT_DEVICE_ERROR_INVALID_ARGUMENT = QAIRT_COMMON_ERROR_INVALID_ARGUMENT,
  /// Requested element is out of range
  QAIRT_DEVICE_ERROR_OUT_OF_RANGE = QAIRT_COMMON_ERROR_OUT_OF_RANGE,
  /// Invalid handle
  QAIRT_DEVICE_ERROR_INVALID_HANDLE = 14000,
  /// Invalid config values
  QAIRT_DEVICE_ERROR_INVALID_CONFIG = 14001,
  /// Hardware unavailable
  QAIRT_DEVICE_ERROR_HARDWARE_UNAVAILABLE = 14002,
  /// Device is associated to a context
  QAIRT_DEVICE_ERROR_ASSOCIATED_TO_CONTEXT = 14003,

  ////////////////////////////////////////////
  QAIRT_DEVICE_MAX_ERROR = QAIRT_MAX_ERROR_DEVICE,
  // Unused, present to ensure 32 bits.
  QAIRT_DEVICE_ERROR_UNDEFINED = 0x7FFFFFFF
} QairtDevice_Error_t;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // QAISW_QAIRTDEVICECOMMON_H

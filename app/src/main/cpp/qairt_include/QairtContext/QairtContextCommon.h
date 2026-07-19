//=============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//=============================================================================

#ifndef QAISW_QAIRTCONTEXTCOMMON_H
#define QAISW_QAIRTCONTEXTCOMMON_H

#include "QairtCommon/QairtCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
// Data Types
//=============================================================================
/**
 * @brief QAIRT Context API result / error codes.
 */
typedef enum {
  QAIRT_CONTEXT_MIN_ERROR = QAIRT_MIN_ERROR_CONTEXT,
  ////////////////////////////////////////////

  /// QAIRT context success
  QAIRT_CONTEXT_NO_ERROR = QAIRT_SUCCESS,
  /// There is optional API component that is not supported yet. See
  /// QairtCapability
  QAIRT_CONTEXT_ERROR_UNSUPPORTED_FEATURE = QAIRT_COMMON_ERROR_NOT_SUPPORTED,
  /// Context-specific memory allocation/deallocation failure
  QAIRT_CONTEXT_ERROR_MEM_ALLOC = QAIRT_COMMON_ERROR_MEM_ALLOC,
  /// An argument to QAIRT context API is deemed invalid by a backend
  QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT = 5000,
  /// A QAIRT context has not yet been created in the backend
  QAIRT_CONTEXT_ERROR_CTX_DOES_NOT_EXIST = 5001,
  /// Invalid/NULL QAIRT context handle
  QAIRT_CONTEXT_ERROR_INVALID_HANDLE = 5002,
  /// Attempting an operation when graphs in a context haven't been finalized
  QAIRT_CONTEXT_ERROR_NOT_FINALIZED = 5003,
  /// Attempt to access context binary with an incompatible version
  QAIRT_CONTEXT_ERROR_BINARY_VERSION = 5004,
  /// Failure to create context from binary
  QAIRT_CONTEXT_ERROR_CREATE_FROM_BINARY = 5005,
  /// Failure to get size of a QAIRT serialized context
  QAIRT_CONTEXT_ERROR_GET_BINARY_SIZE_FAILED = 5006,
  /// Failure to generate a QAIRT serialized context
  QAIRT_CONTEXT_ERROR_GET_BINARY_FAILED = 5007,
  /// Invalid context binary configuration
  QAIRT_CONTEXT_ERROR_BINARY_CONFIGURATION = 5008,
  /// Failure to set profile
  QAIRT_CONTEXT_ERROR_SET_PROFILE = 5009,
  /// Invalid config
  QAIRT_CONTEXT_ERROR_INVALID_CONFIG = 5010,
  /// Attempt to create a context from suboptimal binary
  QAIRT_CONTEXT_ERROR_BINARY_SUBOPTIMAL = 5011,
  /// Call aborted early due to a QairtSignal_trigger call issued
  /// to the observed signal object.
  QAIRT_CONTEXT_ERROR_ABORTED = 5012,
  /// Call aborted early due to a QairtSignal timeout
  QAIRT_CONTEXT_ERROR_TIMED_OUT = 5013,
  /// Incremental Binary Buffer was not allocated by backend
  QAIRT_CONTEXT_ERROR_INCREMENT_INVALID_BUFFER = 5014,
  /// Memory has already been registered with the context
  QAIRT_CONTEXT_ERROR_MEM_ALREADY_REGISTERED = 5015,
  /// Backend does not support the requested memory type
  QAIRT_CONTEXT_ERROR_MEM_UNSUPPORTED_TYPE = 5016,
  /// Failed to map between memory file descriptor and memory address
  QAIRT_CONTEXT_ERROR_MEM_MAPPING = 5017,
  /// Invalid memory shape based on backend's memory restrictions
  QAIRT_CONTEXT_ERROR_MEM_INVALID_SHAPE = 5018,
  ////////////////////////////////////////////
  QAIRT_CONTEXT_MAX_ERROR = QAIRT_MAX_ERROR_CONTEXT,
  /// Unused, present to ensure 32 bits.
  QAIRT_CONTEXT_ERROR_UNDEFINED = 0x7FFFFFFF
} QairtContext_Error_t;

/**
 * @brief Enum to distinguish type of binary section to retrieve.
 */
typedef enum {
  /// Portion of the context binary containing recent updates applied through
  /// QairtTensor_updateGraphTensors() or QairtTensor_updateContextTensors()
  QAIRT_CONTEXT_SECTION_UPDATABLE = 1,
  /// Portion of the context binary containing recent static data updates applied through
  /// QairtTensor_updateGraphTensors() or QairtTensor_updateContextTensors()
  QAIRT_CONTEXT_SECTION_UPDATABLE_WEIGHTS = 2,
  /// Portion of the context binary containing recent quantization updates applied through
  /// QairtTensor_updateGraphTensors() or QairtTensor_updateContextTensors()
  QAIRT_CONTEXT_SECTION_UPDATABLE_QUANT_PARAMS = 3,
  /// Unused, present to ensure 32 bits.
  QAIRT_CONTEXT_SECTION_UNDEFINED = 0x7FFFFFFF
} QairtContext_SectionType_t;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // QAISW_QAIRTCONTEXTCOMMON_H

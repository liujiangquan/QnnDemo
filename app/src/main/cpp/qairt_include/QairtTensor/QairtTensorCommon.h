//=============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//=============================================================================
#ifndef QAISW_QAIRTTENSORCOMMON_H
#define QAISW_QAIRTTENSORCOMMON_H
#include "QairtCommon/QairtCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
// Data Types
//=============================================================================
typedef void* QairtSparseParams_Handle_t;

/**
 * @brief QAIRT Tensor API result / error codes.
 */
typedef enum {
  QAIRT_TENSOR_MIN_ERROR = QAIRT_MIN_ERROR_TENSOR,
  //////////////////////////////////////////

  /// Success.
  QAIRT_TENSOR_NO_ERROR = QAIRT_SUCCESS,
  /// Invalid context/graph handle in creating tensor.
  QAIRT_TENSOR_ERROR_INVALID_HANDLE = 7001,
  /// Tensor with specified credentials not registered with a context/graph.
  QAIRT_TENSOR_ERROR_DOES_NOT_EXIST = 7002,
  /// Invalid tensor param.
  QAIRT_TENSOR_ERROR_INVALID_TENSOR_PARAM = 7004,
  /// This tensor param is currently unsupported.
  QAIRT_TENSOR_ERROR_UNSUPPORTED_TENSOR_PARAM = 7005,
  /// Tensor provided for update is invalid.
  QAIRT_TENSOR_ERROR_INCOMPATIBLE_TENSOR_UPDATE = 7007,
  /// Invalid context/graph handle in creating tensor.
  QAIRT_TENSOR_ERROR_INVALID_ARGUMENT = 7008,
  /// Memory allocation error during tensor memory operations.
  QAIRT_TENSOR_ERROR_MEM_ALLOC = 7009,
  /// Tensor memory type mismatch for requested operation.
  QAIRT_TENSOR_ERROR_MEMORY_TYPE_MISMATCH = 7010,
  /// There is optional API component that is not supported yet. See
  /// QairtProperty.
  QAIRT_TENSOR_ERROR_UNSUPPORTED_FEATURE = QAIRT_COMMON_ERROR_NOT_SUPPORTED,

  //////////////////////////////////////////
  QAIRT_TENSOR_MAX_ERROR = QAIRT_MAX_ERROR_TENSOR,
  // Unused, present to ensure 32 bits.
  QAIRT_TENSOR_ERROR_UNDEFINED = 0x7FFFFFFF
} QairtTensor_Error_t;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // QAISW_QAIRTTENSORCOMMON_H

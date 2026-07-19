//=============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//=============================================================================

#ifndef QAIRT_LPAI_COMMON_H
#define QAIRT_LPAI_COMMON_H

#ifdef __cplusplus
#include <cstdint>
#else
#include <stdint.h>
#endif

#include "QairtCommon/QairtCommon.h"

//=============================================================================
// Data Types
//=============================================================================

/**
 * @brief Memory types used for LPAI buffers.
 */
typedef enum {
  /// Double Data Rate (DDR) memory
  QAIRT_LPAI_MEM_TYPE_DDR = 1,
  /// Last Level Cache (LLC) memory
  QAIRT_LPAI_MEM_TYPE_LLC = 2,
  /// Tightly Coupled Memory (TCM)
  QAIRT_LPAI_MEM_TYPE_TCM = 3,
  /// Pseudo-Static RAM (PSRAM)
  QAIRT_LPAI_MEM_TYPE_PSRAM = 4,
  // Unused, present to ensure 32 bits.
  QAIRT_LPAI_MEM_TYPE_UNDEFINED = 0x7FFFFFFF
} QairtLpai_MemType_t;

#endif  // QAIRT_LPAI_COMMON_H

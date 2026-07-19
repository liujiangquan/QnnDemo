//==============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//==============================================================================

/**
 * @file
 * @brief   Interface component API
 *
 *          A header which contains the interface to a QAIRT Backend library.
 */

#ifndef QAISW_QAIRTINTERFACE_H
#define QAISW_QAIRTINTERFACE_H
#include "QairtCommon/QairtCommon.h"
#ifdef __cplusplus
#include <cstddef>
#include <cstdint>
extern "C" {
#else
#include <stddef.h>
#include <stdint.h>
#endif

//=============================================================================
// Macros
//=============================================================================
// Macro controlling visibility of QAIRT Interface API
#ifndef QAIRT_INTERFACE
#define QAIRT_INTERFACE
#endif

//=============================================================================
// Data Types
//=============================================================================
typedef enum {
  QAIRT_INTERFACE_MIN_ERROR = QAIRT_MIN_ERROR_INTERFACE,
  //////////////////////////////////////////

  /// API or feature is not supported by implementation.
  QAIRT_INTERFACE_ERROR_NOT_SUPPORTED = 15000,

  //////////////////////////////////////////
  QAIRT_INTERFACE_MAX_ERROR = QAIRT_MAX_ERROR_INTERFACE,
  // Unused, present to ensure 32 bits.
  QAIRT_INTERFACE_ERROR_UNDEFINED = 0x7FFFFFFF
} QairtInterface_Error_t;

//=============================================================================
// Public Functions
//=============================================================================
/**
 * @brief Retrieve an interface by its unique identifier.
 *
 * @param[in] id The unique identifier for the interface to retrieve. Interface
 *               IDs are defined in component headers (e.g.,
 *               QAIRT_BACKEND_V1_ID, QAIRT_GRAPH_V1_ID).
 * @param[out] interfaceOut A pointer to be populated with the requested
 *                          interface. The interface is a struct containing
 *                          function pointers for the component's API.
 * @return Error code:
 *         - QAIRT_SUCCESS: Interface was successfully retrieved.
 *         - QAIRT_INTERFACE_ERROR_NOT_SUPPORTED: The requested interface
 *           ID is not supported.
 *         - QAIRT_COMMON_ERROR_INVALID_ARGUMENT: _interfaceOut_ is NULL.
 **/
QAIRT_INTERFACE
Qairt_Status_t QairtInterface_getInterface(uint64_t id, void** interfaceOut);

#ifdef __cplusplus
}  // extern "C"
#endif
#endif  // QAISW_QAIRTINTERFACE_H

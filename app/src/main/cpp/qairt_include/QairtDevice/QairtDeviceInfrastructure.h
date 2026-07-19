//==============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//==============================================================================

/**
 * @file
 * @brief   Device Infrastructure component API
 *
 *          Provides interfaces for accessing backend-specific device
 *          infrastructure.
 */

#ifndef QAISW_QAIRTDEVICEINFRASTRUCTURE_H
#define QAISW_QAIRTDEVICEINFRASTRUCTURE_H

#include "QairtCommon/QairtCommon.h"
#include "QairtDeviceCommon.h"
#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
// Macros
//=============================================================================

//=============================================================================
// Datatypes
//=============================================================================
/**
 * @brief Backend specific opaque infrastructure object
 *
 *        Please refer to the documentation provided by the backend for usage
 *        information.
 */
typedef struct _QairtDevice_InfrastructureHandle_t* QairtDevice_InfrastructureHandle_t;

//=============================================================================
// Public Functions
//=============================================================================

/**
 * @brief Get device hardware infrastructure interface object
 *
 * This is optional capability, support is advertised via QairtCapability. If
 * supported, please refer to documentation and/or header file provided by the
 * backend for usage information.
 *
 * @param[out] deviceInfraHandle Pointer to infrastructure interface object. The
 *                               pointer returned is a backend owned memory.
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_DEVICE_ERROR_UNSUPPORTED_FEATURE: API is not supported
 *         - QAIRT_DEVICE_ERROR_INVALID_ARGUMENT: _deviceInfraHandle_ is
 *           NULL
 *         - QAIRT_DEVICE_ERROR_MEM_ALLOC: Insufficient memory to return
 *           _deviceInfraHandle_
 *
 * @note Use corresponding API through QairtDevice_InfrastructureProviderV1_t.
 */
Qairt_Status_t QairtDevice_InfrastructureProvider_getInfrastructure(
    QairtDevice_InfrastructureHandle_t* deviceInfraHandle);

typedef Qairt_Status_t (*QairtDevice_InfrastructureProvider_GetInfrastructureFn_t)(
    QairtDevice_InfrastructureHandle_t*);

//=============================================================================
// Public Interfaces
//=============================================================================
/**
 * @struct QairtDevice_InfrastructureProviderV1_t
 * @brief This struct contains the functions that define the V1 interface for
 *        QAIRT Device Infrastructure Provider. This interface can be
 *        retrieved through the id QAIRT_DEVICE_INRFA_PROVIDER_V1_ID.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtDevice_InfrastructureProvider_GetInfrastructureFn_t getDeviceInfra;
} QairtDevice_InfrastructureProviderV1_t;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // QAISW_QAIRTDEVICEINFRASTRUCTURE_H

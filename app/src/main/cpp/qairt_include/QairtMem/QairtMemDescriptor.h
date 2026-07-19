//=============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//=============================================================================

#ifndef QAISW_QAIRTMEMDESCRIPTOR_H
#define QAISW_QAIRTMEMDESCRIPTOR_H

#include "QairtCommon/QairtScalar.h"
#include "QairtMemShape.h"

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
// Macros
//=============================================================================
#define QAIRT_MEM_DESCRIPTOR_V1_ID QAIRT_MIN_ID_MEM
#define QAIRT_MEM_SHAPE_V1_ID      QAIRT_MIN_ID_MEM + 2

//=============================================================================
// Data Types
//=============================================================================
typedef struct _QairtMem_Handle_t* QairtMem_Handle_t;
typedef struct _QairtMem_DescriptorHandle_t* QairtMem_DescriptorHandle_t;
typedef struct _QairtMem_CustomInfoHandle_t* QairtMem_CustomInfoHandle_t;

/**
 * @brief An enumeration of memory types which may be used to provide data for a
 *        QAIRT tensor.
 */
typedef enum {
  QAIRT_UNSUPPORTED = 1,
  /// Memory allocated by a custom backend mechanism.
  QAIRT_MEM_TYPE_CUSTOM = 2,
  /// Memory allocated by DMA-BUF subsystem.
  QAIRT_MEM_TYPE_DMA_BUF = 3,
  // Unused, present to ensure 32 bits.
  QAIRT_MEM_TYPE_UNDEFINED = 0x7FFFFFFF
} Qairt_MemType_t;

//=============================================================================
// Public Functions
//=============================================================================
/**
 * @brief Creates a Mem Descriptor handle.
 *
 * @param[out] descriptorHandle A handle to the created mem descriptor.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_MEM_ERROR_INVALID_HANDLE: _descriptorHandle_ is not an
 *           assignable handle
 *
 * @note Use corresponding API through QairtMem_DescriptorV1_t.
 */
Qairt_Status_t QairtMem_Descriptor_create(QairtMem_DescriptorHandle_t* descriptorHandle);

/**
 * @brief Frees a Mem Descriptor handle.
 *
 * @param[in] descriptorHandle A handle to the created mem descriptor.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_MEM_ERROR_INVALID_HANDLE: _descriptorHandle_ is not an
 *           assignable handle
 *
 * @note Use corresponding API through QairtMem_DescriptorV1_t.
 */
Qairt_Status_t QairtMem_Descriptor_free(QairtMem_DescriptorHandle_t descriptorHandle);

/**
 * @brief Sets a shape handle onto a descriptor handle.
 *
 * @param[in] descriptorHandle A handle to the created mem descriptor.
 *
 * @param[in] memShapeHandle A mem shape handle, created and configured via the
 *                           Mem Shape component APIs. Once set onto the
 *                           descriptor, the Mem Shape handle may be freed.
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_MEM_ERROR_INVALID_HANDLE: _descriptorHandle_ or
 *           _memShapeHandle_ are not a valid handle
 *         - QAIRT_MEM_ERROR_INVALID_ARGUMENT: at least one argument is
 *           invalid
 *
 * @note Use corresponding API through QairtMem_DescriptorV1_t.
 */
Qairt_Status_t QairtMem_Descriptor_setMemShape(QairtMem_DescriptorHandle_t descriptorHandle,
                                                    QairtMem_ShapeHandle_t memShapeHandle);

/**
 * @brief Gets a shape handle onto a descriptor handle.
 *
 * @param[in] descriptorHandle A handle to the created mem descriptor.
 *                             Once set, the Mem Shape handle may be freed.
 * @param[out] memShapeHandle A referenced memShape handle. Reference handles
 *                            should not be freed. Referenced handles are
 *                            invalidated if modification operations are performed from the same
 * component API.
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_MEM_ERROR_INVALID_HANDLE: _backend_ is not a valid
 *           handle
 *         - QAIRT_MEM_ERROR_INVALID_ARGUMENT: at least one argument is
 *           invalid
 *
 * @note Use corresponding API through QairtMem_DescriptorV1_t.
 */
Qairt_Status_t QairtMem_Descriptor_getMemShape(QairtMem_DescriptorHandle_t descriptorHandle,
                                                    QairtMem_ShapeHandle_t* memShapeHandle);

/**
 * @brief Sets a data type on a descriptor handle.
 *
 * @param[in] descriptorHandle A handle to the created mem descriptor.
 *
 * @param[in] dataType The datatype to set
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_MEM_ERROR_INVALID_HANDLE: _descriptorHandle_ is not a
 *           valid handle
 *         - QAIRT_MEM_ERROR_INVALID_ARGUMENT: at least one argument is
 *           invalid
 */
Qairt_Status_t QairtMem_Descriptor_setDataType(QairtMem_DescriptorHandle_t descriptorHandle,
                                                    Qairt_DataType_t dataType);

/**
 * @brief Gets the data type from a descriptor handle.
 *
 * @param[in] descriptorHandle A handle to the created mem descriptor.
 *
 * @param[out] dataType The datatype value associated with the descriptor
 *                      handle.
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_MEM_ERROR_INVALID_HANDLE: _descriptorHandle_ is not a
 *           valid handle
 *         - QAIRT_MEM_ERROR_INVALID_ARGUMENT: at least one argument is
 *           invalid
 *
 * @note Use corresponding API through QairtMem_DescriptorV1_t.
 */
Qairt_Status_t QairtMem_Descriptor_getDataType(QairtMem_DescriptorHandle_t descriptorHandle,
                                                    Qairt_DataType_t* dataType);

/**
 * @brief Gets the mem type from a descriptor handle.
 *
 * @param[in] descriptorHandle A handle to the created mem descriptor.
 *
 * @param[out] memType The memType value associated with the descriptor handle.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_MEM_ERROR_INVALID_HANDLE: _descriptorHandle_ is not a
 *           valid handle
 *         - QAIRT_MEM_ERROR_INVALID_ARGUMENT: at least one argument is
 *           invalid
 *
 * @note Use corresponding API through QairtMem_DescriptorV1_t.
 */
Qairt_Status_t QairtMem_Descriptor_getMemType(QairtMem_DescriptorHandle_t descriptorHandle,
                                                   Qairt_MemType_t* memType);

/**
 * @brief Sets a custom info handle onto a descriptor handle.
 *
 * @param[in] descriptorHandle A handle to the created mem descriptor.
 *
 * @param[in] customInfoHandle A QairtMem custom info handle, created and
 *                             configured via the QairtMem Custom Info component
 *                             APIs, as defined by the backend. Once set onto the descriptor, the
 * custom info handle may be freed.
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_MEM_ERROR_INVALID_HANDLE: _descriptorHandle_ or
 *           _customInfoHandle_ is not a valid handle
 *         - QAIRT_MEM_ERROR_INVALID_ARGUMENT: at least one argument is
 *           invalid
 *
 * @note Use corresponding API through QairtMem_DescriptorV1_t.
 */
Qairt_Status_t QairtMem_Descriptor_setCustomInfo(
    QairtMem_DescriptorHandle_t descriptorHandle, QairtMem_CustomInfoHandle_t customInfoHandle);

/**
 * @brief Gets the custom info handle associated with the descriptor handle.
 *
 * @param[in] descriptorHandle A handle to the created mem descriptor.
 *
 * @param[out] customInfoHandle A referenced QairtMem custom info handle,
 *                              created and configured via the QairtMem Custom
 *                              Info component APIs, as defined by the backend. Reference handles
 * should not be freed. Referenced handles are invalidated if modification operations are performed
 * from the same component API.
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_MEM_ERROR_INVALID_HANDLE: _descriptorHandle_ or
 *           _customInfoHandle_ is not a valid handle
 *         - QAIRT_MEM_ERROR_INVALID_ARGUMENT: at least one argument is
 *           invalid
 *
 * @note Use corresponding API through QairtMem_DescriptorV1_t.
 */
Qairt_Status_t QairtMem_Descriptor_getCustomInfo(
    QairtMem_DescriptorHandle_t descriptorHandle,
    QairtMem_CustomInfoHandle_t* customInfoHandle);

/**
 * @brief Sets the dma buf on a descriptor handle.
 *
 * @param[in] descriptorHandle A handle to the created mem descriptor.
 *
 * @param[in] fd The file descriptor for memory. Must be set to
 *               QAIRT_MEM_INVALID_FD if not applicable.
 * @param[in] data Data pointer, created by app, using mmap on the above file
 *                 descriptor.
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_MEM_ERROR_INVALID_HANDLE: _descriptorHandle_ is not a
 *           valid handle
 *         - QAIRT_MEM_ERROR_INVALID_ARGUMENT: at least one argument is
 *           invalid
 *
 * @note Use corresponding API through QairtMem_DescriptorV1_t.
 */
Qairt_Status_t QairtMem_Descriptor_setDmaBuf(QairtMem_DescriptorHandle_t descriptorHandle,
                                                  int32_t fd,
                                                  void* data);

/**
 * @brief Gets the dma buf associated with the descriptor handle.
 *
 * @param[in] descriptorHandle A handle to the created mem descriptor.
 *
 * @param[out] fd The file descriptor for memory. Must be set to
 *                QAIRT_MEM_INVALID_FD if not applicable.
 * @param[out] data Data pointer, created by app, using mmap on the above file
 *                  descriptor.
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_MEM_ERROR_INVALID_HANDLE: _descriptorHandle_ is not a
 *           valid handle
 *         - QAIRT_MEM_ERROR_INVALID_ARGUMENT: at least one argument is
 *           invalid
 *
 * @note Use corresponding API through QairtMem_DescriptorV1_t.
 */
Qairt_Status_t QairtMem_Descriptor_getDmaBuf(QairtMem_DescriptorHandle_t descriptorHandle,
                                                  int32_t* fd,
                                                  void** data);

/**
 * @brief Deregister a memory handle which was registered via QairtMem_register
 *        and invalidates memHandle for the given backend handle.
 *
 * @param[in] memHandlesHandle Array of memory handles to be deregistered.
 *
 * @param[in] numHandles Number of memory handles in the array.
 *
 * @note memHandles parameter: Upon failure, all valid handles within
         _memHandlesHandle_ will still be de-registered.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: memory was successfully de-registered
 *         - QAIRT_MEM_ERROR_NOT_SUPPORTED: backend does not support this
 *           API
 *         - QAIRT_MEM_ERROR_INVALID_ARGUMENT: _memHandles_ is NULL
 *         - QAIRT_MEM_ERROR_INVALID_HANDLE: a handle within _memHandles_
 *           is NULL/invalid
 *
 * @note Use corresponding API through QairtMem_DescriptorV1_t.
 */
Qairt_Status_t QairtMem_Descriptor_deregisterMem(QairtMem_Handle_t* memHandles,
                                                      uint32_t numHandles);

typedef Qairt_Status_t (*QairtMem_Descriptor_CreateFn_t)(QairtMem_DescriptorHandle_t*);
typedef Qairt_Status_t (*QairtMem_Descriptor_FreeFn_t)(QairtMem_DescriptorHandle_t);

typedef Qairt_Status_t (*QairtMem_Descriptor_SetMemShapeFn_t)(QairtMem_DescriptorHandle_t,
                                                                   QairtMem_ShapeHandle_t);
typedef Qairt_Status_t (*QairtMem_Descriptor_GetMemShapeFn_t)(QairtMem_DescriptorHandle_t,
                                                                   QairtMem_ShapeHandle_t*);

typedef Qairt_Status_t (*QairtMem_Descriptor_SetDataTypeFn_t)(QairtMem_DescriptorHandle_t,
                                                                   Qairt_DataType_t);
typedef Qairt_Status_t (*QairtMem_Descriptor_GetDataTypeFn_t)(QairtMem_DescriptorHandle_t,
                                                                   Qairt_DataType_t*);

typedef Qairt_Status_t (*QairtMem_Descriptor_GetMemTypeFn_t)(QairtMem_DescriptorHandle_t,
                                                                  Qairt_MemType_t*);

typedef Qairt_Status_t (*QairtMem_Descriptor_SetCustomInfoFn_t)(QairtMem_DescriptorHandle_t,
                                                                     QairtMem_CustomInfoHandle_t);
typedef Qairt_Status_t (*QairtMem_Descriptor_GetCustomInfoFn_t)(
    QairtMem_DescriptorHandle_t, QairtMem_CustomInfoHandle_t*);

typedef Qairt_Status_t (*QairtMem_Descriptor_SetDmaBufFn_t)(QairtMem_DescriptorHandle_t,
                                                                 int32_t,
                                                                 void*);
typedef Qairt_Status_t (*QairtMem_Descriptor_GetDmaBufFn_t)(QairtMem_DescriptorHandle_t,
                                                                 int32_t*,
                                                                 void**);

typedef Qairt_Status_t (*QairtMem_Descriptor_DeregisterMemFn_t)(
    QairtMem_Handle_t* memHandles, uint32_t numHandles);
//=============================================================================
// Public Interfaces
//=============================================================================
/**
 * @struct QairtMem_DescriptorV1_t
 * @brief This struct contains the functions that define the V1 interface for
 *        QAIRT Mem Descriptor. This interface can be retrieved through the
 *        id QAIRT_MEM_DESCRIPTOR_V1_ID.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtMem_Descriptor_CreateFn_t create;
  QairtMem_Descriptor_FreeFn_t free;
  QairtMem_Descriptor_SetMemShapeFn_t setMemShape;
  QairtMem_Descriptor_GetMemShapeFn_t getMemShape;
  QairtMem_Descriptor_SetDataTypeFn_t setDataType;
  QairtMem_Descriptor_GetDataTypeFn_t getDataType;
  QairtMem_Descriptor_GetMemTypeFn_t getMemType;
  QairtMem_Descriptor_SetCustomInfoFn_t setCustomInfo;
  QairtMem_Descriptor_GetCustomInfoFn_t getCustomInfo;
  QairtMem_Descriptor_SetDmaBufFn_t setDmaBuf;
  QairtMem_Descriptor_GetDmaBufFn_t getDmaBuf;
  QairtMem_Descriptor_DeregisterMemFn_t deregisterMem;
} QairtMem_DescriptorV1_t;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // QAISW_QAIRTMEMDESCRIPTOR_H

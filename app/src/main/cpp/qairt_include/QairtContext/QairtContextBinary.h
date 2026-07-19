//=============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//=============================================================================

/**
 * @file
 * @brief   Context Binary component API
 *
 *          A header which contains the QAIRT context binary component and the
 *          interfaces it implements.
 */

#ifndef QAISW_QAIRTCONTEXTBINARY_H
#define QAISW_QAIRTCONTEXTBINARY_H

#ifdef __cplusplus
extern "C" {
#include <cstddef>
#include <cstdint>
#else
#include <stddef.h>
#include <stdint.h>
#endif

#include "QairtContextCommon.h"
//=============================================================================
// Macros
//=============================================================================

//=============================================================================
// Data Types
//=============================================================================
/// Define Context Binary Buffer
typedef struct _QairtContext_BinaryBufferHandle_t* QairtContext_BinaryBufferHandle_t;
/// Define Context Binary
typedef struct _QairtContext_BinaryHandle_t* QairtContext_BinaryHandle_t;

/// Define Qairt Context Binary Compatibility Type
typedef enum {
  QAIRT_CONTEXT_BINARY_TYPE_RAW        = 0,
  QAIRT_CONTEXT_BINARY_TYPE_MEM_HANDLE = 1,
  // Unused, present to ensure 32 bits
  QAIRT_CONTEXT_BINARY_TYPE_UNDEFINED = 0x7FFFFFF
} QairtContext_BinaryType_t;

//=============================================================================
// Public Functions
//=============================================================================

/**
 * @brief Create a binary buffer handle.
 *
 * @param[out] binaryBufferHandle A handle to the created binary buffer.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Binary buffer was successfully created.
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: _binaryBufferHandle_ is
 *           NULL.
 *         - QAIRT_CONTEXT_ERROR_MEM_ALLOC: Memory allocation error while
 *           creating binary buffer.
 *
 * @note Use corresponding API through QairtContext_BinaryBufferV1_t.
 */
Qairt_Status_t QairtContext_BinaryBuffer_create(
    QairtContext_BinaryBufferHandle_t* binaryBufferHandle);

/**
 * @brief Free a binary buffer handle and associated resources.
 *
 * @param[in] binaryBufferHandle A handle to a created binary buffer.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Binary buffer was successfully freed.
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: _binaryBufferHandle_ is
 *           not a valid handle.
 *         - QAIRT_CONTEXT_ERROR_MEM_ALLOC: Memory deallocation error.
 *
 * @note Use corresponding API through QairtContext_BinaryBufferV1_t.
 */
Qairt_Status_t QairtContext_BinaryBuffer_free(QairtContext_BinaryBufferHandle_t binaryBufferHandle);

/**
 * @brief Set data pointer for a binary buffer.
 *
 * @param[in] binaryBufferHandle A handle to a created binary buffer.
 *
 * @param[in] data Pointer to the data to be associated with the binary buffer.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Data was successfully set.
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: _binaryBufferHandle_ is
 *           not a valid handle.
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: _data_ is NULL.
 *
 * @note Use corresponding API through QairtContext_BinaryBufferV1_t.
 */
Qairt_Status_t QairtContext_BinaryBuffer_setData(
    QairtContext_BinaryBufferHandle_t binaryBufferHandle, void* data);

/**
 * @brief Set size for a binary buffer.
 *
 * @param[in] binaryBufferHandle A handle to a created binary buffer.
 *
 * @param[in] size Size of the binary buffer in bytes.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Size was successfully set.
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: _binaryBufferHandle_ is
 *           not a valid handle.
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: _size_ is invalid.
 *
 * @note Use corresponding API through QairtContext_BinaryBufferV1_t.
 */
Qairt_Status_t QairtContext_BinaryBuffer_setSize(
    QairtContext_BinaryBufferHandle_t binaryBufferHandle, uint64_t size);

/**
 * @brief Get data pointer from a binary buffer.
 *
 * @param[in] binaryBufferHandle A handle to a created binary buffer.
 *
 * @param[out] data Pointer to retrieve the data associated with the binary
 *                  buffer.
 * @return Error code:
 *         - QAIRT_SUCCESS: Data was successfully retrieved.
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: _binaryBufferHandle_ is
 *           not a valid handle.
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: _data_ is NULL.
 *
 * @note Use corresponding API through QairtContext_BinaryBufferV1_t.
 */
Qairt_Status_t QairtContext_BinaryBuffer_getData(
    QairtContext_BinaryBufferHandle_t binaryBufferHandle, void** data);

/**
 * @brief Get size from a binary buffer.
 *
 * @param[in] binaryBufferHandle A handle to a created binary buffer.
 *
 * @param[out] size Pointer to retrieve the size of the binary buffer in bytes.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Size was successfully retrieved.
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: _binaryBufferHandle_ is
 *           not a valid handle.
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: _size_ is NULL.
 *
 * @note Use corresponding API through QairtContext_BinaryBufferV1_t.
 */
Qairt_Status_t QairtContext_BinaryBuffer_getSize(
    QairtContext_BinaryBufferHandle_t binaryBufferHandle, uint64_t* size);

/**
 * @brief Create a binary handle.
 *
 * @param[out] binaryHandle A handle to the created binary.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Binary was successfully created.
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: _binaryHandle_ is NULL.
 *         - QAIRT_CONTEXT_ERROR_MEM_ALLOC: Memory allocation error while
 *           creating binary.
 *
 * @note Use corresponding API through QairtContext_BinaryV1_t.
 */
Qairt_Status_t QairtContext_Binary_create(QairtContext_BinaryHandle_t* binaryHandle);

/**
 * @brief Free a binary handle and associated resources.
 *
 * @param[in] binaryHandle A handle to a created binary.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Binary was successfully freed.
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: _binaryHandle_ is not a
 *           valid handle.
 *         - QAIRT_CONTEXT_ERROR_MEM_ALLOC: Memory deallocation error.
 *
 * @note Use corresponding API through QairtContext_BinaryV1_t.
 */
Qairt_Status_t QairtContext_Binary_free(QairtContext_BinaryHandle_t binaryHandle);

/**
 * @brief Get the type of a binary.
 *
 * @param[in] binaryHandle A handle to a created binary.
 *
 * @param[out] type Pointer to retrieve the binary type.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Type was successfully retrieved.
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: _binaryHandle_ is not a
 *           valid handle.
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: _type_ is NULL.
 *
 * @note Use corresponding API through QairtContext_BinaryV1_t.
 */
Qairt_Status_t QairtContext_binaryGetType(QairtContext_BinaryHandle_t binaryHandle,
                                          QairtContext_BinaryType_t* type);

/**
 * @brief Set binary buffer for a binary handle.
 *
 * @param[in] binaryHandle A handle to a created binary.
 *
 * @param[in] binaryBufferHandle A handle to a binary buffer to associate with
 *                               the binary.
 * @return Error code:
 *         - QAIRT_SUCCESS: Binary buffer was successfully set.
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: _binaryHandle_ or
 *           _binaryBufferHandle_ is not a valid handle.
 *
 * @note Use corresponding API through QairtContext_BinaryV1_t.
 */
Qairt_Status_t QairtContext_binarySetBuffer(QairtContext_BinaryHandle_t binaryHandle,
                                            QairtContext_BinaryBufferHandle_t binaryBufferHandle);

/**
 * @brief Get binary buffer from a binary handle.
 *
 * @param[in] binaryHandle A handle to a created binary.
 *
 * @param[out] binaryBufferHandle Pointer to retrieve the binary buffer handle.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Binary buffer was successfully retrieved.
 *         - QAIRT_CONTEXT_ERROR_INVALID_HANDLE: _binaryHandle_ is not a
 *           valid handle.
 *         - QAIRT_CONTEXT_ERROR_INVALID_ARGUMENT: _binaryBufferHandle_ is
 *           NULL.
 *
 * @note Use corresponding API through QairtContext_BinaryV1_t.
 */
Qairt_Status_t QairtContext_binaryGetBuffer(QairtContext_BinaryHandle_t binaryHandle,
                                            QairtContext_BinaryBufferHandle_t* binaryBufferHandle);

/// Binary Buffer
typedef Qairt_Status_t (*QairtContext_BinaryBuffer_CreateFn_t)(QairtContext_BinaryBufferHandle_t*);
typedef Qairt_Status_t (*QairtContext_BinaryBuffer_FreeFn_t)(QairtContext_BinaryBufferHandle_t);
typedef Qairt_Status_t (*QairtContext_BinaryBuffer_SetDataFn_t)(QairtContext_BinaryBufferHandle_t,
                                                                void*);
typedef Qairt_Status_t (*QairtContext_BinaryBuffer_GetDataFn_t)(QairtContext_BinaryBufferHandle_t,
                                                                void**);
typedef Qairt_Status_t (*QairtContext_BinaryBuffer_SetSizeFn_t)(QairtContext_BinaryBufferHandle_t,
                                                                uint64_t);
typedef Qairt_Status_t (*QairtContext_BinaryBuffer_GetSizeFn_t)(QairtContext_BinaryBufferHandle_t,
                                                                uint64_t*);

/// Binary
typedef Qairt_Status_t (*QairtContext_Binary_CreateFn_t)(QairtContext_BinaryHandle_t*);
typedef Qairt_Status_t (*QairtContext_Binary_FreeFn_t)(QairtContext_BinaryHandle_t);
typedef Qairt_Status_t (*QairtContext_Binary_GetTypeFn_t)(QairtContext_BinaryHandle_t,
                                                          QairtContext_BinaryType_t*);
typedef Qairt_Status_t (*QairtContext_Binary_SetBinaryBufferFn_t)(
    QairtContext_BinaryHandle_t, QairtContext_BinaryBufferHandle_t);
typedef Qairt_Status_t (*QairtContext_Binary_GetBinaryBufferFn_t)(
    QairtContext_BinaryHandle_t, QairtContext_BinaryBufferHandle_t*);

//=============================================================================
// Public Interfaces
//=============================================================================

/**
 * @struct QairtContext_BinaryBufferV1_t
 * @brief This struct contains the functions that define the V1 interface for
 *        QAIRT Context Binary Buffer.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtContext_BinaryBuffer_CreateFn_t create;
  QairtContext_BinaryBuffer_FreeFn_t free;
  QairtContext_BinaryBuffer_SetDataFn_t setData;
  QairtContext_BinaryBuffer_GetDataFn_t getData;
  QairtContext_BinaryBuffer_SetSizeFn_t setSize;
  QairtContext_BinaryBuffer_GetSizeFn_t getSize;
} QairtContext_BinaryBufferV1_t;

/**
 * @struct QairtContext_BinaryV1_t
 * @brief This struct contains the functions that define the V1 interface for
 *        QAIRT Context Binary.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtContext_Binary_CreateFn_t create;
  QairtContext_Binary_FreeFn_t free;
  QairtContext_Binary_GetTypeFn_t getType;
  QairtContext_Binary_SetBinaryBufferFn_t setBuffer;
  QairtContext_Binary_GetBinaryBufferFn_t getBuffer;
} QairtContext_BinaryV1_t;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // QAISW_QAIRTCONTEXTBINARY_H

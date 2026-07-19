//=============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//=============================================================================

/**
 * @file
 * @brief   Tensor Memory component API
 *
 *          A header which contains the QAIRT Tensor Memory component and the
 *          interfaces it implements.
 */

#ifndef QAISW_QAIRTTENSORMEMORY_H
#define QAISW_QAIRTTENSORMEMORY_H

#include "QairtMem/QairtMemDescriptor.h"
#include "QairtTensorCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
// Macros
//=============================================================================

//=============================================================================
// Data Types
//=============================================================================
typedef struct _QairtTensor_MemoryHandle_t* QairtTensor_MemoryHandle_t;
typedef struct _QairtTensor_ClientBufferHandle_t* QairtTensor_ClientBufferHandle_t;

// Forward declarations for handle types used in callbacks
typedef struct _QairtContext_Handle_t* QairtContext_Handle_t;
typedef struct _QairtGraph_Handle_t* QairtGraph_Handle_t;
/**
 * @brief An enum specifying memory types of tensor data.
 */
typedef enum {
  /// Raw memory pointer
  QAIRT_TENSORMEMTYPE_RAW = 0,
  /// Memory object, provide capability for memory sharing in between QAIRT
  /// accelerator backends.
  QAIRT_TENSORMEMTYPE_MEMHANDLE = 1,
  /// Callback to retrieve a raw memory pointer
  QAIRT_TENSORMEMTYPE_RETRIEVE_RAW = 2,
  // Unused, present to ensure 32 bits.
  QAIRT_TENSORMEMTYPE_UNDEFINED = 0x7FFFFFFF
} QairtTensor_MemoryType_t;

/**
 * @brief A client-defined function used to obtain tensor data when the tensor memory type is
 *        QAIRT_TENSORMEMTYPE_RETRIEVE_RAW. Qairt_GetTensorRawDataFn_t may be called multiple times
 *        for the same tensor. Each call to Qairt_GetTensorRawDataFn_t must be accompanied by a call
 *        to Qairt_FreeTensorRawDataFn_t to free any allocated data for that tensor.
 *
 * @param[in] context The context to which the tensor is associated
 * @param[in] graph The graph to which the context is associated. For context tensors this field
 *                  should be null.
 * @param[in] id The tensor ID.
 * @param[out] clientBuf A handle to a client buffer that should be populated with the tensor's
 *                       raw data pointer and size.
 * @param[in] cookie User-supplied context pointer passed to callbacks (may be NULL).
 *                   This is the same cookie value provided via
 *                   QairtTensor_Memory_setRawRetrieveCallbacks().
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Client Buffer data successfully provided.
 *         - QAIRT_TENSOR_ERROR_DOES_NOT_EXIST: Tensor with id does not exist or was not created
 *           as QAIRT_TENSORMEMTYPE_RETRIEVE_RAW.
 *         - QAIRT_TENSOR_ERROR_INVALID_ARGUMENT: clientBuf is NULL
 *         - QAIRT_TENSOR_ERROR_MEM_ALLOC: Requested tensor data cannot be allocated.
 */
typedef Qairt_Status_t (*Qairt_GetTensorRawDataFn_t)(
    QairtContext_Handle_t context,
    QairtGraph_Handle_t graph,
    uint64_t id,
    QairtTensor_ClientBufferHandle_t* clientBuf,
    void* cookie);

/**
 * @brief A client-defined function used to free tensor data previously obtained by
 *        Qairt_GetTensorRawDataFn_t. After the call to Qairt_FreeTensorRawDataFn_t the data
 *        provided in the client buffer should be considered invalid. If Qairt_GetTensorRawDataFn_t
 *        has been called multiple times for the same tensor then Qairt_FreeTensorRawDataFn_t must
 *        be called an equivalent number of times to free all allocated data for this tensor.
 *
 * @param[in] context The context to which the tensor is associated
 * @param[in] graph The graph to which the context is associated. For context tensors this field
 *                  should be null.
 * @param[in] id The tensor ID.
 * @param[in] cookie User-supplied context pointer passed to callbacks (may be NULL).
 *                   This is the same cookie value provided via
 *                   QairtTensor_Memory_setRawRetrieveCallbacks().
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Client Buffer data successfully freed.
 *         - QAIRT_TENSOR_ERROR_DOES_NOT_EXIST: Tensor with id does not exist, was not created
 *           as QAIRT_TENSORMEMTYPE_RETRIEVE_RAW, or has already been freed.
 */
typedef Qairt_Status_t (*Qairt_FreeTensorRawDataFn_t)(QairtContext_Handle_t context,
                                                           QairtGraph_Handle_t graph,
                                                           uint64_t id,
                                                           void* cookie);

//=============================================================================
// Public Functions
//=============================================================================

/**
 * @brief Create a client buffer handle.
 *
 * @param[out] clientBufferHandle A handle to the created client buffer.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Client buffer was successfully created.
 *         - QAIRT_TENSOR_ERROR_INVALID_ARGUMENT: _clientBufferHandle_ is NULL.
 *         - QAIRT_TENSOR_ERROR_MEM_ALLOC: Memory allocation error while creating client buffer.
 *
 * @note Use corresponding API through QairtTensor_ClientBufferV1_t.
 */
Qairt_Status_t QairtTensor_ClientBuffer_create(
    QairtTensor_ClientBufferHandle_t* clientBufferHandle);

/**
 * @brief Free a client buffer handle and associated resources.
 *
 * @param[in] clientBufferHandle A handle to a created client buffer.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Client buffer was successfully freed.
 *         - QAIRT_TENSOR_ERROR_INVALID_HANDLE: _clientBufferHandle_ is not a valid handle.
 *         - QAIRT_TENSOR_ERROR_MEM_ALLOC: Memory deallocation error.
 *
 * @note Use corresponding API through QairtTensor_ClientBufferV1_t.
 */
Qairt_Status_t QairtTensor_ClientBuffer_free(
    QairtTensor_ClientBufferHandle_t clientBufferHandle);

/**
 * @brief Set data pointer for a client buffer.
 *
 * @param[in] clientBufferHandle A handle to a created client buffer.
 * @param[in] data Pointer to the data to be associated with the client buffer.
 *                 May be NULL for zero-size buffers or to reset/detach the buffer.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Data was successfully set.
 *         - QAIRT_TENSOR_ERROR_INVALID_HANDLE: _clientBufferHandle_ is not a valid handle.
 *
 * @note Use corresponding API through QairtTensor_ClientBufferV1_t.
 * @note NULL data pointers are valid for empty buffers (size 0) or when resetting a buffer.
 *       This allows proper cleanup and initialization of tensor buffers.
 */
Qairt_Status_t QairtTensor_ClientBuffer_setData(
    QairtTensor_ClientBufferHandle_t clientBufferHandle, void* data);

/**
 * @brief Get data pointer from a client buffer.
 *
 * @param[in] clientBufferHandle A handle to a created client buffer.
 * @param[out] data Pointer to retrieve the data associated with the client buffer.
 *                  May be NULL if the buffer has no data or has been reset.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Data was successfully retrieved.
 *         - QAIRT_TENSOR_ERROR_INVALID_HANDLE: _clientBufferHandle_ is not a valid handle.
 *         - QAIRT_TENSOR_ERROR_INVALID_ARGUMENT: _data_ is NULL.
 *
 * @note Use corresponding API through QairtTensor_ClientBufferV1_t.
 * @note The retrieved data pointer may be NULL for empty or reset buffers.
 */
Qairt_Status_t QairtTensor_ClientBuffer_getData(
    QairtTensor_ClientBufferHandle_t clientBufferHandle, void** data);

/**
 * @brief Set data size for a client buffer.
 *
 * @param[in] clientBufferHandle A handle to a created client buffer.
 * @param[in] dataSize Data size of the client buffer in bytes. May be 0 for empty buffers.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: dataSize was successfully set.
 *         - QAIRT_TENSOR_ERROR_INVALID_HANDLE: _clientBufferHandle_ is not a valid handle.
 *
 * @note Use corresponding API through QairtTensor_ClientBufferV1_t.
 * @note A size of 0 is valid and indicates an empty buffer.
 */
Qairt_Status_t QairtTensor_ClientBuffer_setDataSize(
    QairtTensor_ClientBufferHandle_t clientBufferHandle, uint32_t dataSize);

/**
 * @brief Get data size from a client buffer.
 *
 * @param[in] clientBufferHandle A handle to a created client buffer.
 * @param[out] dataSize Pointer to retrieve the data size of the client buffer in bytes.
 *                      May be 0 for empty buffers.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: dataSize was successfully retrieved.
 *         - QAIRT_TENSOR_ERROR_INVALID_HANDLE: _clientBufferHandle_ is not a valid handle.
 *         - QAIRT_TENSOR_ERROR_INVALID_ARGUMENT: _dataSize_ is NULL.
 *
 * @note Use corresponding API through QairtTensor_ClientBufferV1_t.
 * @note The retrieved size may be 0 for empty buffers.
 */
Qairt_Status_t QairtTensor_ClientBuffer_getDataSize(
    QairtTensor_ClientBufferHandle_t clientBufferHandle, uint32_t* dataSize);

/**
 * @brief Create a tensor memory handle.
 *
 * @param[out] tensorMemoryHandle A handle to the created tensor memory.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Tensor memory was successfully created.
 *         - QAIRT_TENSOR_ERROR_INVALID_ARGUMENT: _tensorMemoryHandle_ is NULL.
 *         - QAIRT_TENSOR_ERROR_MEM_ALLOC: Memory allocation error while creating tensor memory.
 *
 * @note Use corresponding API through QairtTensor_MemoryV1_t.
 */
Qairt_Status_t QairtTensor_Memory_create(
    QairtTensor_MemoryHandle_t* tensorMemoryHandle);

/**
 * @brief Free a tensor memory handle and associated resources.
 *
 * @param[in] tensorMemoryHandle A handle to a created tensor memory.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Tensor memory was successfully freed.
 *         - QAIRT_TENSOR_ERROR_INVALID_HANDLE: _tensorMemoryHandle_ is not a valid handle.
 *         - QAIRT_TENSOR_ERROR_MEM_ALLOC: Memory deallocation error.
 *
 * @note Use corresponding API through QairtTensor_MemoryV1_t.
 */
Qairt_Status_t QairtTensor_Memory_free(QairtTensor_MemoryHandle_t tensorMemoryHandle);

/**
 * @brief Get the memory type of a tensor memory handle.
 *
 * @param[in] tensorMemoryHandle A handle to a created tensor memory.
 * @param[out] type Pointer to retrieve the memory type.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Memory type was successfully retrieved.
 *         - QAIRT_TENSOR_ERROR_INVALID_HANDLE: _tensorMemoryHandle_ is not a valid handle.
 *         - QAIRT_TENSOR_ERROR_INVALID_ARGUMENT: _type_ is invalid.
 *
 * @note Use corresponding API through QairtTensor_MemoryV1_t.
 */
Qairt_Status_t QairtTensor_Memory_getMemoryType(
    QairtTensor_MemoryHandle_t tensorMemoryHandle, QairtTensor_MemoryType_t* type);

/**
 * @brief Set a memory handle for a tensor memory handle.
 *        Creates a memory handle memory type within the tensor memory if not already present,
 *        then associates the specified memory handle with the tensor memory.
 *
 * @param[in] tensorMemory A handle to a created tensor memory.
 * @param[in] memHandle A memory handle to associate with the tensor memory.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Memory handle was successfully set.
 *         - QAIRT_TENSOR_ERROR_INVALID_HANDLE: _tensorMemory_ or _memHandle_ is not a valid handle.
 *
 * @note Use corresponding API through QairtTensor_MemoryV1_t.
 */
Qairt_Status_t QairtTensor_Memory_setMemHandle(QairtTensor_MemoryHandle_t tensorMemory,
                                                          QairtMem_Handle_t memHandle);

/**
 * @brief Get memory handle from a tensor memory handle.
 *
 * @param[in] tensorMemory A handle to a created tensor memory.
 * @param[out] memHandle Pointer to retrieve the memory handle.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Memory handle was successfully retrieved.
 *         - QAIRT_TENSOR_ERROR_INVALID_HANDLE: _tensorMemory_ is not a valid handle.
 *         - QAIRT_TENSOR_ERROR_INVALID_ARGUMENT: _memHandle_ is NULL.
 *         - QAIRT_TENSOR_ERROR_MEMORY_TYPE_MISMATCH: Tensor memory type is not
 *           QAIRT_TENSORMEMTYPE_MEMHANDLE.
 *
 * @note Use corresponding API through QairtTensor_MemoryV1_t.
 */
Qairt_Status_t QairtTensor_Memory_getMemHandle(QairtTensor_MemoryHandle_t tensorMemory,
                                                          QairtMem_Handle_t* memHandle);

/**
 * @brief Set client buffer for a tensor memory handle.
 *        Creates a buffer memory type within the tensor memory if not already present,
 *        then associates the specified client buffer with the tensor memory.
 *
 * @param[in] tensorMemory A handle to a created tensor memory.
 * @param[in] clientBuffer A client buffer handle to associate with the tensor memory.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Client buffer was successfully set.
 *         - QAIRT_TENSOR_ERROR_INVALID_HANDLE: _tensorMemory_ is not a valid handle.
 *         - QAIRT_TENSOR_ERROR_INVALID_ARGUMENT: _clientBuffer_ is NULL.
 *
 * @note Use corresponding API through QairtTensor_MemoryV1_t.
 */
Qairt_Status_t QairtTensor_Memory_setClientBuffer(
    QairtTensor_MemoryHandle_t tensorMemory, QairtTensor_ClientBufferHandle_t clientBuffer);

/**
 * @brief Get client buffer from a tensor memory handle.
 *
 * @param[in] tensorMemory A handle to a created tensor memory.
 * @param[out] clientBuffer Pointer to retrieve the client buffer handle.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Client buffer was successfully retrieved.
 *         - QAIRT_TENSOR_ERROR_INVALID_HANDLE: _tensorMemory_ is not a valid handle.
 *         - QAIRT_TENSOR_ERROR_INVALID_ARGUMENT: _clientBuffer_ is NULL.
 *         - QAIRT_TENSOR_ERROR_MEMORY_TYPE_MISMATCH: Tensor memory type is not
 *           QAIRT_TENSORMEMTYPE_RAW.
 *
 * @note Use corresponding API through QairtTensor_MemoryV1_t.
 */
Qairt_Status_t QairtTensor_Memory_getClientBuffer(
    QairtTensor_MemoryHandle_t tensorMemory, QairtTensor_ClientBufferHandle_t* clientBuffer);

/**
 * @brief Set raw retrieve callbacks for a tensor memory handle.
 *        Creates a raw retrieve memory type within the tensor memory if not already present,
 *        then sets the specified callback functions and cookie.
 *
 * @param[in] tensorMemory A handle to a created tensor memory.
 * @param[in] getCallback Callback function to get tensor raw data.
 * @param[in] freeCallback Callback function to free tensor raw data.
 * @param[in] cookie User-supplied context pointer to pass to callbacks (may be NULL).
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Raw retrieve callbacks were successfully set.
 *         - QAIRT_TENSOR_ERROR_INVALID_HANDLE: _tensorMemory_ is not a valid handle.
 *         - QAIRT_TENSOR_ERROR_INVALID_ARGUMENT: _getCallback_ or _freeCallback_ is NULL.
 *
 * @note Use corresponding API through QairtTensor_MemoryV1_t.
 */
Qairt_Status_t QairtTensor_Memory_setRawRetrieveCallbacks(
    QairtTensor_MemoryHandle_t tensorMemory,
    Qairt_GetTensorRawDataFn_t getCallback,
    Qairt_FreeTensorRawDataFn_t freeCallback,
    void* cookie);

/**
 * @brief Get callbacks and associated cookie from a tensor memory handle's raw retrieve.
 *
 * @param[in] tensorMemory A handle to a created tensor memory.
 * @param[out] getCallback Pointer to retrieve the get tensor raw data callback.
 * @param[out] freeCallback Pointer to retrieve the free tensor raw data callback.
 * @param[out] cookie Pointer to retrieve the user-supplied context pointer passed to callbacks.
 *                    (The pointer value will be set to NULL if no cookie was set.)
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Callbacks were successfully retrieved.
 *         - QAIRT_TENSOR_ERROR_INVALID_HANDLE: _tensorMemory_ is not a valid handle.
 *         - QAIRT_TENSOR_ERROR_INVALID_ARGUMENT: _getCallback_, _freeCallback_, or _cookie_ is
 *           NULL.
 *         - QAIRT_TENSOR_ERROR_MEMORY_TYPE_MISMATCH: Tensor memory type is not
 *           QAIRT_TENSORMEMTYPE_RETRIEVE_RAW.
 *
 * @note Use corresponding API through QairtTensor_MemoryV1_t.
 */
Qairt_Status_t QairtTensor_Memory_getRawRetrieveCallbacks(
    QairtTensor_MemoryHandle_t tensorMemory,
    Qairt_GetTensorRawDataFn_t* getCallback,
    Qairt_FreeTensorRawDataFn_t* freeCallback,
    void** cookie);

/// Client Buffer
typedef Qairt_Status_t (*QairtTensor_ClientBuffer_CreateFn_t)(
    QairtTensor_ClientBufferHandle_t*);
typedef Qairt_Status_t (*QairtTensor_ClientBuffer_FreeFn_t)(
    QairtTensor_ClientBufferHandle_t);
typedef Qairt_Status_t (*QairtTensor_ClientBuffer_SetDataFn_t)(
    QairtTensor_ClientBufferHandle_t, void*);
typedef Qairt_Status_t (*QairtTensor_ClientBuffer_GetDataFn_t)(
    QairtTensor_ClientBufferHandle_t, void**);
typedef Qairt_Status_t (*QairtTensor_ClientBuffer_SetDataSizeFn_t)(
    QairtTensor_ClientBufferHandle_t, uint32_t);
typedef Qairt_Status_t (*QairtTensor_ClientBuffer_GetDataSizeFn_t)(
    QairtTensor_ClientBufferHandle_t, uint32_t*);

/// Tensor Memory
typedef Qairt_Status_t (*QairtTensor_Memory_CreateFn_t)(QairtTensor_MemoryHandle_t*);
typedef Qairt_Status_t (*QairtTensor_Memory_FreeFn_t)(QairtTensor_MemoryHandle_t);
typedef Qairt_Status_t (*QairtTensor_Memory_GetMemoryTypeFn_t)(
    QairtTensor_MemoryHandle_t, QairtTensor_MemoryType_t*);

typedef Qairt_Status_t (*QairtTensor_Memory_SetMemHandleFn_t)(
    QairtTensor_MemoryHandle_t, QairtMem_Handle_t);
typedef Qairt_Status_t (*QairtTensor_Memory_GetMemHandleFn_t)(
    QairtTensor_MemoryHandle_t, QairtMem_Handle_t*);

typedef Qairt_Status_t (*QairtTensor_Memory_SetClientBufferFn_t)(
    QairtTensor_MemoryHandle_t, QairtTensor_ClientBufferHandle_t);
typedef Qairt_Status_t (*QairtTensor_Memory_GetClientBufferFn_t)(
    QairtTensor_MemoryHandle_t, QairtTensor_ClientBufferHandle_t*);

typedef Qairt_Status_t (*QairtTensor_Memory_SetRawRetrieveCallbacksFn_t)(
    QairtTensor_MemoryHandle_t,
    Qairt_GetTensorRawDataFn_t,
    Qairt_FreeTensorRawDataFn_t,
    void* cookie);
typedef Qairt_Status_t (*QairtTensor_Memory_GetRawRetrieveCallbacksFn_t)(
    QairtTensor_MemoryHandle_t,
    Qairt_GetTensorRawDataFn_t*,
    Qairt_FreeTensorRawDataFn_t*,
    void** cookie);

//=============================================================================
// Public Interfaces
//=============================================================================

/**
 * @struct QairtTensor_ClientBufferV1_t
 * @brief This struct contains the functions that define the V1 interface for
 *        QAIRT Client Buffer.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtTensor_ClientBuffer_CreateFn_t create;
  QairtTensor_ClientBuffer_FreeFn_t free;
  QairtTensor_ClientBuffer_SetDataFn_t setData;
  QairtTensor_ClientBuffer_GetDataFn_t getData;
  QairtTensor_ClientBuffer_SetDataSizeFn_t setDataSize;
  QairtTensor_ClientBuffer_GetDataSizeFn_t getDataSize;
} QairtTensor_ClientBufferV1_t;

/**
 * @struct QairtTensor_MemoryV1_t
 * @brief This struct contains the functions that define the V1 interface for
 *        QAIRT Memory. This interface can be retrieved through the id
 *        QAIRT_TENSOR_MEMORY_V1_ID.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtTensor_Memory_CreateFn_t create;
  QairtTensor_Memory_FreeFn_t free;
  QairtTensor_Memory_GetMemoryTypeFn_t getMemoryType;
  QairtTensor_Memory_SetMemHandleFn_t setMemHandle;
  QairtTensor_Memory_GetMemHandleFn_t getMemHandle;
  QairtTensor_Memory_SetClientBufferFn_t setClientBuffer;
  QairtTensor_Memory_GetClientBufferFn_t getClientBuffer;
  QairtTensor_Memory_SetRawRetrieveCallbacksFn_t setRawRetrieveCallbacks;
  QairtTensor_Memory_GetRawRetrieveCallbacksFn_t getRawRetrieveCallbacks;
} QairtTensor_MemoryV1_t;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // QAISW_QAIRTTENSORMEMORY_H

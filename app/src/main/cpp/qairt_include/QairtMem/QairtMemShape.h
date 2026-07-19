//=============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//=============================================================================

#ifndef QAISW_QAIRTMEMSHAPE_H
#define QAISW_QAIRTMEMSHAPE_H

#include "QairtCommon/QairtCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
// Data Types
//=============================================================================
/**
 * @brief   MemShape handle used to interface with MemShape APIs
 *
 *          The handle itself is created and freed by the client.
 */
typedef struct _QairtMem_ShapeHandle_t* QairtMem_ShapeHandle_t;

//=============================================================================
// Public Functions
//=============================================================================
/**
 * @brief Creates a Mem Shape handle.
 *
 * @param[out] shapeHandle A handle to the created mem shape.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_MEM_ERROR_INVALID_HANDLE: _shapeHandle_ is not an
 *           assignable handle
 *
 * @note Use corresponding API through QairtMem_ShapeV1_t.
 */
Qairt_Status_t QairtMem_Shape_create(QairtMem_ShapeHandle_t* shapeHandle);

/**
 * @brief Frees a Mem Shape handle.
 *
 * @param[in] shapeHandle A handle to the created mem shape.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_MEM_ERROR_INVALID_HANDLE: _shapeHandle_ is not a valid
 *           handle
 *
 * @note Use corresponding API through QairtMem_ShapeV1_t.
 */
Qairt_Status_t QairtMem_Shape_free(QairtMem_ShapeHandle_t shapeHandle);

/**
 * @brief Gets the rank from a shape handle.
 *
 * @param[in] shapeHandle A handle to the created mem shape.
 *
 * @param[out] rank The rank associated with the shape handle.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_MEM_ERROR_INVALID_HANDLE: _shapeHandle_ is not a valid
 *           handle
 *         - QAIRT_MEM_ERROR_INVALID_ARGUMENT: at least one argument is
 *           invalid
 *
 * @note Use corresponding API through QairtMem_ShapeV1_t.
 */
Qairt_Status_t QairtMem_Shape_getRank(QairtMem_ShapeHandle_t shapeHandle, uint32_t* rank);

/**
 * @brief Sets the dimensions on a shape handle.
 *
 * @param[in] shapeHandle A handle to the created mem shape.
 *
 * @param[in] dimensions An array of dimensions, with length rank.
 *
 * @param[in] rank The number of dimensions in the dimensions array.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_MEM_ERROR_INVALID_HANDLE: _shapeHandle_ is not a valid
 *           handle
 *         - QAIRT_MEM_ERROR_INVALID_ARGUMENT: at least one argument is
 *           invalid
 *
 * @note Use corresponding API through QairtMem_ShapeV1_t.
 */
Qairt_Status_t QairtMem_Shape_setDimensions(QairtMem_ShapeHandle_t shapeHandle,
                                                 uint32_t* dimensions,
                                                 const uint32_t rank);

/**
 * @brief Gets the dimension at an index.
 *
 * @param[in] shapeHandle A handle to the created mem shape.
 *
 * @param[in] index The index of the dimension to retrieve. Must be less than
 *                  rank.
 * @param[out] dim An array of dimensions, with length rank.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_MEM_ERROR_INVALID_HANDLE: _shapeHandle_ is not a valid
 *           handle
 *         - QAIRT_MEM_ERROR_INVALID_ARGUMENT: at least one argument is
 *           invalid
 *
 * @note Use corresponding API through QairtMem_ShapeV1_t.
 */
Qairt_Status_t QairtMem_Shape_getDimensionAt(QairtMem_ShapeHandle_t shapeHandle,
                                                  uint32_t index,
                                                  uint32_t* dim);

/**
 * @brief Sets the shape config on a shape handle.
 *
 * @param[in] shapeHandle A handle to the created mem shape.
 *
 * @param[in] config The configuration to set. Allowed to be NULL.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_MEM_ERROR_INVALID_HANDLE: _shapeHandle_ is not a valid
 *           handle
 *         - QAIRT_MEM_ERROR_INVALID_ARGUMENT: at least one argument is
 *           invalid
 *
 * @note Use corresponding API through QairtMem_ShapeV1_t.
 */
Qairt_Status_t QairtMem_Shape_setShapeConfig(QairtMem_ShapeHandle_t shapeHandle,
                                                  const char* config);

/**
 * @brief Gets the shape config associated with the shape handle.
 *
 * @param[in] shapeHandle A handle to the created mem shape.
 *
 * @param[out] config The configuration associated with the handle. Allowed to
 *                    be NULL.
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_MEM_ERROR_INVALID_HANDLE: _shapeHandle_ is not a valid
 *           handle
 *         - QAIRT_MEM_ERROR_INVALID_ARGUMENT: at least one argument is
 *           invalid
 *
 * @note Use corresponding API through QairtMem_ShapeV1_t.
 */
Qairt_Status_t QairtMem_Shape_getShapeConfig(QairtMem_ShapeHandle_t shapeHandle,
                                                  const char** config);

typedef Qairt_Status_t (*QairtMem_Shape_CreateFn_t)(QairtMem_ShapeHandle_t*);
typedef Qairt_Status_t (*QairtMem_Shape_FreeFn_t)(QairtMem_ShapeHandle_t);

typedef Qairt_Status_t (*QairtMem_Shape_GetRankFn_t)(QairtMem_ShapeHandle_t, uint32_t*);
typedef Qairt_Status_t (*QairtMem_Shape_SetDimensionsFn_t)(QairtMem_ShapeHandle_t,
                                                                uint32_t*,
                                                                const uint32_t);
typedef Qairt_Status_t (*QairtMem_Shape_GetDimensionAtFn_t)(QairtMem_ShapeHandle_t,
                                                                 uint32_t,
                                                                 uint32_t*);

typedef Qairt_Status_t (*QairtMem_Shape_SetShapeConfigFn_t)(QairtMem_ShapeHandle_t,
                                                                 const char*);
typedef Qairt_Status_t (*QairtMem_Shape_GetShapeConfigFn_t)(QairtMem_ShapeHandle_t,
                                                                 const char**);
//=============================================================================
// Public Interfaces
//=============================================================================
/**
 * @struct QairtMem_ShapeV1_t
 * @brief This struct contains the functions that define the V1 interface for
 *        QAIRT Mem Shape. This interface can be retrieved through the id
 *        QAIRT_MEM_SHAPE_V1_ID.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtMem_Shape_CreateFn_t create;
  QairtMem_Shape_FreeFn_t free;
  QairtMem_Shape_GetRankFn_t getRank;
  QairtMem_Shape_SetDimensionsFn_t setDimensions;
  QairtMem_Shape_GetDimensionAtFn_t getDimensionAt;
  QairtMem_Shape_SetShapeConfigFn_t setShapeConfig;
  QairtMem_Shape_GetShapeConfigFn_t getShapeConfig;
} QairtMem_ShapeV1_t;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // QAISW_QAIRTMEMSHAPE_H

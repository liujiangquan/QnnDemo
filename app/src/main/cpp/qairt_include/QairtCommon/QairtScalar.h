//==============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//==============================================================================

/**
 * @file
 * @brief   Scalar component API
 *
 *          A header which contains the QAIRT Scalar component and the
 *          interfaces it implements.
 */

#ifndef QAISW_QAIRTSCALAR_H
#define QAISW_QAIRTSCALAR_H

#include "QairtCommon/QairtCommon.h"

#ifdef __cplusplus
extern "C" {
#include <cstdint>
#else
#include <stdint.h>
#endif

//=============================================================================
// Datatypes
//=============================================================================
/**
 * @brief   Scalar handle used to interface with Scalar APIs
 *
 *          The handle itself is created and freed by the client.
 */
typedef struct _QairtScalar_Handle_t* QairtScalar_Handle_t;

//=============================================================================
// Public Functions
//=============================================================================
/**
 * @brief Create a scalar handle.
 *
 * @param[out] scalarHandle A handle to the created scalar.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_OPCONFIG_ERROR_INVALID_ARGUMENT: _scalarHandle_ is NULL
 *         - QAIRT_OPCONFIG_ERROR_MEM_ALLOC: Memory allocation error while
 *           creating scalar
 *
 * @note Use corresponding API through QairtScalar_V1_t.
 */
Qairt_Status_t QairtScalar_create(QairtScalar_Handle_t* scalarHandle);

/**
 * @brief Free a scalar handle.
 *
 * @param[in] scalarHandle A handle to the scalar to be freed.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_OPCONFIG_ERROR_INVALID_HANDLE: _scalarHandle_ is not a
 *           valid handle
 *         - QAIRT_OPCONFIG_ERROR_MEM_ALLOC: Memory deallocation error
 *
 * @note Use corresponding API through QairtScalar_V1_t.
 */
Qairt_Status_t QairtScalar_free(QairtScalar_Handle_t scalarHandle);

/**
 * @brief Get the data type of a scalar.
 *
 * @param[in] scalarHandle A handle to the scalar.
 * @param[out] value The retrieved data type.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_OPCONFIG_ERROR_INVALID_HANDLE: _scalarHandle_ is not a
 *           valid handle
 *         - QAIRT_OPCONFIG_ERROR_INVALID_ARGUMENT: _value_ is NULL
 *
 * @note Use corresponding API through QairtScalar_V1_t.
 */
Qairt_Status_t QairtScalar_getDataType(QairtScalar_Handle_t scalarHandle,
                                            Qairt_DataType_t* value);

/**
 * @brief Get the float value from a scalar.
 *
 * @param[in] scalarHandle A handle to the scalar.
 *
 * @param[out] value The retrieved float value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_OPCONFIG_ERROR_INVALID_HANDLE: _scalarHandle_ is not a
 *           valid handle
 *         - QAIRT_OPCONFIG_ERROR_INVALID_ARGUMENT: _value_ is NULL or
 *           scalar is not float type
 *
 * @note Use corresponding API through QairtScalar_V1_t.
 */
Qairt_Status_t QairtScalar_getFloatValue(QairtScalar_Handle_t scalarHandle, float* value);

/**
 * @brief Get the double value from a scalar.
 *
 * @param[in] scalarHandle A handle to the scalar.
 *
 * @param[out] value The retrieved double value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_OPCONFIG_ERROR_INVALID_HANDLE: _scalarHandle_ is not a
 *           valid handle
 *         - QAIRT_OPCONFIG_ERROR_INVALID_ARGUMENT: _value_ is NULL or
 *           scalar is not double type
 *
 * @note Use corresponding API through QairtScalar_V1_t.
 */
Qairt_Status_t QairtScalar_getDoubleValue(QairtScalar_Handle_t scalarHandle, double* value);

/**
 * @brief Get the uint64 value from a scalar.
 *
 * @param[in] scalarHandle A handle to the scalar.
 *
 * @param[out] value The retrieved uint64 value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_OPCONFIG_ERROR_INVALID_HANDLE: _scalarHandle_ is not a
 *           valid handle
 *         - QAIRT_OPCONFIG_ERROR_INVALID_ARGUMENT: _value_ is NULL or
 *           scalar is not uint64 type
 *
 * @note Use corresponding API through QairtScalar_V1_t.
 */
Qairt_Status_t QairtScalar_getUint64Value(QairtScalar_Handle_t scalarHandle,
                                               uint64_t* value);

/**
 * @brief Get the uint32 value from a scalar.
 *
 * @param[in] scalarHandle A handle to the scalar.
 *
 * @param[out] value The retrieved uint32 value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_OPCONFIG_ERROR_INVALID_HANDLE: _scalarHandle_ is not a
 *           valid handle
 *         - QAIRT_OPCONFIG_ERROR_INVALID_ARGUMENT: _value_ is NULL or
 *           scalar is not uint32 type
 *
 * @note Use corresponding API through QairtScalar_V1_t.
 */
Qairt_Status_t QairtScalar_getUint32Value(QairtScalar_Handle_t scalarHandle,
                                               uint32_t* value);

/**
 * @brief Get the uint16 value from a scalar.
 *
 * @param[in] scalarHandle A handle to the scalar.
 *
 * @param[out] value The retrieved uint16 value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_OPCONFIG_ERROR_INVALID_HANDLE: _scalarHandle_ is not a
 *           valid handle
 *         - QAIRT_OPCONFIG_ERROR_INVALID_ARGUMENT: _value_ is NULL or
 *           scalar is not uint16 type
 *
 * @note Use corresponding API through QairtScalar_V1_t.
 */
Qairt_Status_t QairtScalar_getUint16Value(QairtScalar_Handle_t scalarHandle,
                                               uint16_t* value);

/**
 * @brief Get the uint8 value from a scalar.
 *
 * @param[in] scalarHandle A handle to the scalar.
 *
 * @param[out] value The retrieved uint8 value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_OPCONFIG_ERROR_INVALID_HANDLE: _scalarHandle_ is not a
 *           valid handle
 *         - QAIRT_OPCONFIG_ERROR_INVALID_ARGUMENT: _value_ is NULL or
 *           scalar is not uint8 type
 *
 * @note Use corresponding API through QairtScalar_V1_t.
 */
Qairt_Status_t QairtScalar_getUint8Value(QairtScalar_Handle_t scalarHandle, uint8_t* value);

/**
 * @brief Get the int64 value from a scalar.
 *
 * @param[in] scalarHandle A handle to the scalar.
 *
 * @param[out] value The retrieved int64 value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_OPCONFIG_ERROR_INVALID_HANDLE: _scalarHandle_ is not a
 *           valid handle
 *         - QAIRT_OPCONFIG_ERROR_INVALID_ARGUMENT: _value_ is NULL or
 *           scalar is not int64 type
 *
 * @note Use corresponding API through QairtScalar_V1_t.
 */
Qairt_Status_t QairtScalar_getInt64Value(QairtScalar_Handle_t scalarHandle, int64_t* value);

/**
 * @brief Get the int32 value from a scalar.
 *
 * @param[in] scalarHandle A handle to the scalar.
 *
 * @param[out] value The retrieved int32 value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_OPCONFIG_ERROR_INVALID_HANDLE: _scalarHandle_ is not a
 *           valid handle
 *         - QAIRT_OPCONFIG_ERROR_INVALID_ARGUMENT: _value_ is NULL or
 *           scalar is not int32 type
 *
 * @note Use corresponding API through QairtScalar_V1_t.
 */
Qairt_Status_t QairtScalar_getInt32Value(QairtScalar_Handle_t scalarHandle, int32_t* value);

/**
 * @brief Get the int16 value from a scalar.
 *
 * @param[in] scalarHandle A handle to the scalar.
 *
 * @param[out] value The retrieved int16 value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_OPCONFIG_ERROR_INVALID_HANDLE: _scalarHandle_ is not a
 *           valid handle
 *         - QAIRT_OPCONFIG_ERROR_INVALID_ARGUMENT: _value_ is NULL or
 *           scalar is not int16 type
 *
 * @note Use corresponding API through QairtScalar_V1_t.
 */
Qairt_Status_t QairtScalar_getInt16Value(QairtScalar_Handle_t scalarHandle, int16_t* value);

/**
 * @brief Get the int8 value from a scalar.
 *
 * @param[in] scalarHandle A handle to the scalar.
 *
 * @param[out] value The retrieved int8 value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_OPCONFIG_ERROR_INVALID_HANDLE: _scalarHandle_ is not a
 *           valid handle
 *         - QAIRT_OPCONFIG_ERROR_INVALID_ARGUMENT: _value_ is NULL or
 *           scalar is not int8 type
 *
 * @note Use corresponding API through QairtScalar_V1_t.
 */
Qairt_Status_t QairtScalar_getInt8Value(QairtScalar_Handle_t scalarHandle, int8_t* value);

/**
 * @brief Get the string value from a scalar.
 *
 * @param[in] scalarHandle A handle to the scalar.
 *
 * @param[out] value The retrieved string value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_OPCONFIG_ERROR_INVALID_HANDLE: _scalarHandle_ is not a
 *           valid handle
 *         - QAIRT_OPCONFIG_ERROR_INVALID_ARGUMENT: _value_ is NULL or
 *           scalar is not string type
 *
 * @note Use corresponding API through QairtScalar_V1_t.
 */
Qairt_Status_t QairtScalar_getStringValue(QairtScalar_Handle_t scalarHandle,
                                               const char** value);

/**
 * @brief Get the boolean value from a scalar.
 *
 * @param[in] scalarHandle A handle to the scalar.
 *
 * @param[out] value The retrieved boolean value.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_OPCONFIG_ERROR_INVALID_HANDLE: _scalarHandle_ is not a
 *           valid handle
 *         - QAIRT_OPCONFIG_ERROR_INVALID_ARGUMENT: _value_ is NULL or
 *           scalar is not boolean type
 *
 * @note Use corresponding API through QairtScalar_V1_t.
 */
Qairt_Status_t QairtScalar_getBoolValue(QairtScalar_Handle_t scalarHandle, uint8_t* value);

/**
 * @brief Set the float value of a scalar.
 *
 * @param[in] scalarHandle A handle to the scalar.
 *
 * @param[in] value The float value to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_OPCONFIG_ERROR_INVALID_HANDLE: _scalarHandle_ is not a
 *           valid handle
 *
 * @note Use corresponding API through QairtScalar_V1_t.
 */
Qairt_Status_t QairtScalar_setFloatValue(QairtScalar_Handle_t scalarHandle,
                                              const float value);

/**
 * @brief Set the double value of a scalar.
 *
 * @param[in] scalarHandle A handle to the scalar.
 *
 * @param[in] value The double value to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_OPCONFIG_ERROR_INVALID_HANDLE: _scalarHandle_ is not a
 *           valid handle
 *
 * @note Use corresponding API through QairtScalar_V1_t.
 */
Qairt_Status_t QairtScalar_setDoubleValue(QairtScalar_Handle_t scalarHandle,
                                               const double value);

/**
 * @brief Set the uint64 value of a scalar.
 *
 * @param[in] scalarHandle A handle to the scalar.
 *
 * @param[in] value The uint64 value to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_OPCONFIG_ERROR_INVALID_HANDLE: _scalarHandle_ is not a
 *           valid handle
 *
 * @note Use corresponding API through QairtScalar_V1_t.
 */
Qairt_Status_t QairtScalar_setUint64Value(QairtScalar_Handle_t scalarHandle,
                                               const uint64_t value);

/**
 * @brief Set the uint32 value of a scalar.
 *
 * @param[in] scalarHandle A handle to the scalar.
 *
 * @param[in] value The uint32 value to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_OPCONFIG_ERROR_INVALID_HANDLE: _scalarHandle_ is not a
 *           valid handle
 *
 * @note Use corresponding API through QairtScalar_V1_t.
 */
Qairt_Status_t QairtScalar_setUint32Value(QairtScalar_Handle_t scalarHandle,
                                               const uint32_t value);

/**
 * @brief Set the uint16 value of a scalar.
 *
 * @param[in] scalarHandle A handle to the scalar.
 *
 * @param[in] value The uint16 value to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_OPCONFIG_ERROR_INVALID_HANDLE: _scalarHandle_ is not a
 *           valid handle
 *
 * @note Use corresponding API through QairtScalar_V1_t.
 */
Qairt_Status_t QairtScalar_setUint16Value(QairtScalar_Handle_t scalarHandle,
                                               const uint16_t value);

/**
 * @brief Set the uint8 value of a scalar.
 *
 * @param[in] scalarHandle A handle to the scalar.
 *
 * @param[in] value The uint8 value to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_OPCONFIG_ERROR_INVALID_HANDLE: _scalarHandle_ is not a
 *           valid handle
 *
 * @note Use corresponding API through QairtScalar_V1_t.
 */
Qairt_Status_t QairtScalar_setUint8Value(QairtScalar_Handle_t scalarHandle,
                                              const uint8_t value);

/**
 * @brief Set the int64 value of a scalar.
 *
 * @param[in] scalarHandle A handle to the scalar.
 *
 * @param[in] value The int64 value to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_OPCONFIG_ERROR_INVALID_HANDLE: _scalarHandle_ is not a
 *           valid handle
 *
 * @note Use corresponding API through QairtScalar_V1_t.
 */
Qairt_Status_t QairtScalar_setInt64Value(QairtScalar_Handle_t scalarHandle,
                                              const int64_t value);

/**
 * @brief Set the int32 value of a scalar.
 *
 * @param[in] scalarHandle A handle to the scalar.
 *
 * @param[in] value The int32 value to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_OPCONFIG_ERROR_INVALID_HANDLE: _scalarHandle_ is not a
 *           valid handle
 *
 * @note Use corresponding API through QairtScalar_V1_t.
 */
Qairt_Status_t QairtScalar_setInt32Value(QairtScalar_Handle_t scalarHandle,
                                              const int32_t value);

/**
 * @brief Set the int16 value of a scalar.
 *
 * @param[in] scalarHandle A handle to the scalar.
 *
 * @param[in] value The int16 value to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_OPCONFIG_ERROR_INVALID_HANDLE: _scalarHandle_ is not a
 *           valid handle
 *
 * @note Use corresponding API through QairtScalar_V1_t.
 */
Qairt_Status_t QairtScalar_setInt16Value(QairtScalar_Handle_t scalarHandle,
                                              const int16_t value);

/**
 * @brief Set the int8 value of a scalar.
 *
 * @param[in] scalarHandle A handle to the scalar.
 *
 * @param[in] value The int8 value to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_OPCONFIG_ERROR_INVALID_HANDLE: _scalarHandle_ is not a
 *           valid handle
 *
 * @note Use corresponding API through QairtScalar_V1_t.
 */
Qairt_Status_t QairtScalar_setInt8Value(QairtScalar_Handle_t scalarHandle,
                                             const int8_t value);

/**
 * @brief Set the string value of a scalar.
 *
 * @param[in] scalarHandle A handle to the scalar.
 *
 * @param[in] value The string value to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_OPCONFIG_ERROR_INVALID_HANDLE: _scalarHandle_ is not a
 *           valid handle
 *         - QAIRT_OPCONFIG_ERROR_INVALID_ARGUMENT: _value_ is NULL
 *         - QAIRT_OPCONFIG_ERROR_MEM_ALLOC: Memory allocation error while
 *           setting string value
 *
 * @note Use corresponding API through QairtScalar_V1_t.
 */
Qairt_Status_t QairtScalar_setStringValue(QairtScalar_Handle_t scalarHandle,
                                               const char* value);

/**
 * @brief Set the boolean value of a scalar.
 *
 * @param[in] scalarHandle A handle to the scalar.
 *
 * @param[in] value The boolean value to set.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_OPCONFIG_ERROR_INVALID_HANDLE: _scalarHandle_ is not a
 *           valid handle
 *
 * @note Use corresponding API through QairtScalar_V1_t.
 */
Qairt_Status_t QairtScalar_setBoolValue(QairtScalar_Handle_t scalarHandle,
                                             const uint8_t value);

typedef Qairt_Status_t (*QairtScalar_CreateFn_t)(QairtScalar_Handle_t*);

typedef Qairt_Status_t (*QairtScalar_FreeFn_t)(QairtScalar_Handle_t);

typedef Qairt_Status_t (*QairtScalar_GetDataTypeFn_t)(QairtScalar_Handle_t,
                                                           Qairt_DataType_t*);

typedef Qairt_Status_t (*QairtScalar_GetFloatValueFn_t)(QairtScalar_Handle_t, float*);

typedef Qairt_Status_t (*QairtScalar_GetDoubleValueFn_t)(QairtScalar_Handle_t, double*);

typedef Qairt_Status_t (*QairtScalar_GetUint64ValueFn_t)(QairtScalar_Handle_t, uint64_t*);

typedef Qairt_Status_t (*QairtScalar_GetUint32ValueFn_t)(QairtScalar_Handle_t, uint32_t*);

typedef Qairt_Status_t (*QairtScalar_GetUint16ValueFn_t)(QairtScalar_Handle_t, uint16_t*);

typedef Qairt_Status_t (*QairtScalar_GetUint8ValueFn_t)(QairtScalar_Handle_t, uint8_t*);

typedef Qairt_Status_t (*QairtScalar_GetInt64ValueFn_t)(QairtScalar_Handle_t, int64_t*);

typedef Qairt_Status_t (*QairtScalar_GetInt32ValueFn_t)(QairtScalar_Handle_t, int32_t*);

typedef Qairt_Status_t (*QairtScalar_GetInt16ValueFn_t)(QairtScalar_Handle_t, int16_t*);

typedef Qairt_Status_t (*QairtScalar_GetInt8ValueFn_t)(QairtScalar_Handle_t, int8_t*);

typedef Qairt_Status_t (*QairtScalar_GetStringValueFn_t)(QairtScalar_Handle_t, const char**);

typedef Qairt_Status_t (*QairtScalar_GetBoolValueFn_t)(QairtScalar_Handle_t, uint8_t*);

typedef Qairt_Status_t (*QairtScalar_SetFloatValueFn_t)(QairtScalar_Handle_t, const float);

typedef Qairt_Status_t (*QairtScalar_SetDoubleValueFn_t)(QairtScalar_Handle_t, const double);

typedef Qairt_Status_t (*QairtScalar_SetUint64ValueFn_t)(QairtScalar_Handle_t,
                                                              const uint64_t);

typedef Qairt_Status_t (*QairtScalar_SetUint32ValueFn_t)(QairtScalar_Handle_t,
                                                              const uint32_t);

typedef Qairt_Status_t (*QairtScalar_SetUint16ValueFn_t)(QairtScalar_Handle_t,
                                                              const uint16_t);

typedef Qairt_Status_t (*QairtScalar_SetUint8ValueFn_t)(QairtScalar_Handle_t, const uint8_t);

typedef Qairt_Status_t (*QairtScalar_SetInt64ValueFn_t)(QairtScalar_Handle_t, const int64_t);

typedef Qairt_Status_t (*QairtScalar_SetInt32ValueFn_t)(QairtScalar_Handle_t, const int32_t);

typedef Qairt_Status_t (*QairtScalar_SetInt16ValueFn_t)(QairtScalar_Handle_t, const int16_t);

typedef Qairt_Status_t (*QairtScalar_SetInt8ValueFn_t)(QairtScalar_Handle_t, const int8_t);

typedef Qairt_Status_t (*QairtScalar_SetStringValueFn_t)(QairtScalar_Handle_t, const char*);

typedef Qairt_Status_t (*QairtScalar_SetBoolValueFn_t)(QairtScalar_Handle_t, const uint8_t);

//=============================================================================
// Public Interfaces
//=============================================================================
/**
 * @struct QairtScalar_V1_t
 * @brief This struct contains the functions that define the V1 interface for
 *        QAIRT Scalar. This interface can be retrieved through the id
 *        QAIRT_SCALAR_V1_ID.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtScalar_CreateFn_t create;
  QairtScalar_FreeFn_t free;
  QairtScalar_GetDataTypeFn_t getDataType;
  QairtScalar_GetFloatValueFn_t getFloatValue;
  QairtScalar_GetDoubleValueFn_t getDoubleValue;
  QairtScalar_GetUint64ValueFn_t getUint64Value;
  QairtScalar_GetUint32ValueFn_t getUint32Value;
  QairtScalar_GetUint16ValueFn_t getUint16Value;
  QairtScalar_GetUint8ValueFn_t getUint8Value;
  QairtScalar_GetInt64ValueFn_t getInt64Value;
  QairtScalar_GetInt32ValueFn_t getInt32Value;
  QairtScalar_GetInt16ValueFn_t getInt16Value;
  QairtScalar_GetInt8ValueFn_t getInt8Value;
  QairtScalar_GetStringValueFn_t getStringValue;
  QairtScalar_GetBoolValueFn_t getBoolValue;
  QairtScalar_SetFloatValueFn_t setFloatValue;
  QairtScalar_SetDoubleValueFn_t setDoubleValue;
  QairtScalar_SetUint64ValueFn_t setUint64Value;
  QairtScalar_SetUint32ValueFn_t setUint32Value;
  QairtScalar_SetUint16ValueFn_t setUint16Value;
  QairtScalar_SetUint8ValueFn_t setUint8Value;
  QairtScalar_SetInt64ValueFn_t setInt64Value;
  QairtScalar_SetInt32ValueFn_t setInt32Value;
  QairtScalar_SetInt16ValueFn_t setInt16Value;
  QairtScalar_SetInt8ValueFn_t setInt8Value;
  QairtScalar_SetStringValueFn_t setStringValue;
  QairtScalar_SetBoolValueFn_t setBoolValue;
} QairtScalar_V1_t;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // QAISW_QAIRTSCALAR_H
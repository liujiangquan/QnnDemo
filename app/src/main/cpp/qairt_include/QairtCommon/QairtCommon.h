//==============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//==============================================================================

/**
 * @file
 * @brief   Common component API
 *
 *          A header which contains common definitions across all components.
 */

#ifndef QAISW_QAIRTCOMMON_H
#define QAISW_QAIRTCOMMON_H

#ifdef __cplusplus
#include <cstddef>
#include <cstdint>
extern "C" {
#else
#include <stddef.h>
#include <stdint.h>
#endif

/**
 * @brief An enum to specify scalar data types
 */
typedef enum {
  // Signed Int: 0x00XX
  /// 2-bit integer type
  QAIRT_DATATYPE_INT_2 = 0x0002,
  /// 4-bit integer type
  QAIRT_DATATYPE_INT_4 = 0x0004,
  /// 8-bit integer type
  QAIRT_DATATYPE_INT_8 = 0x0008,
  /// 16-bit integer type
  QAIRT_DATATYPE_INT_16 = 0x0016,
  /// 32-bit integer type
  QAIRT_DATATYPE_INT_32 = 0x0032,
  /// 64-bit integer type
  QAIRT_DATATYPE_INT_64 = 0x0064,

  // Unsigned Int: 0x01XX
  QAIRT_DATATYPE_UINT_2  = 0x0102,
  QAIRT_DATATYPE_UINT_4  = 0x0104,
  QAIRT_DATATYPE_UINT_8  = 0x0108,
  QAIRT_DATATYPE_UINT_16 = 0x0116,
  QAIRT_DATATYPE_UINT_32 = 0x0132,
  QAIRT_DATATYPE_UINT_64 = 0x0164,

  // Float: 0x02XX
  QAIRT_DATATYPE_FLOAT_4   = 0x0204,
  QAIRT_DATATYPE_FLOAT_8   = 0x0208,
  QAIRT_DATATYPE_FLOAT_16  = 0x0216,
  QAIRT_DATATYPE_BFLOAT_16 = 0x0226,
  QAIRT_DATATYPE_FLOAT_32  = 0x0232,
  QAIRT_DATATYPE_FLOAT_64  = 0x0264,

  // Signed Fixed Point: 0x03XX
  QAIRT_DATATYPE_SFIXED_POINT_2  = 0x0302,
  QAIRT_DATATYPE_SFIXED_POINT_4  = 0x0304,
  QAIRT_DATATYPE_SFIXED_POINT_8  = 0x0308,
  QAIRT_DATATYPE_SFIXED_POINT_16 = 0x0316,
  QAIRT_DATATYPE_SFIXED_POINT_32 = 0x0332,

  // Unsigned Fixed Point: 0x04XX
  QAIRT_DATATYPE_UFIXED_POINT_2  = 0x0402,
  QAIRT_DATATYPE_UFIXED_POINT_4  = 0x0404,
  QAIRT_DATATYPE_UFIXED_POINT_8  = 0x0408,
  QAIRT_DATATYPE_UFIXED_POINT_16 = 0x0416,
  QAIRT_DATATYPE_UFIXED_POINT_32 = 0x0432,

  // Bool: 0x05XX
  /// 8-bit boolean type, 0 = false, any non-zero value = true
  QAIRT_DATATYPE_BOOL_8 = 0x0508,

  // String: 0x06xx
  QAIRT_DATATYPE_STRING = 0x0608,

  // Unused, present to ensure 32 bits.
  QAIRT_DATATYPE_UNDEFINED = 0x7FFFFFFF
} Qairt_DataType_t;
//=============================================================================
// Macros
//=============================================================================
#define QAIRT_SUCCESS 0

/**
 * @brief Call a function through a versioned QAIRT interface struct, returning
 *        QAIRT_COMMON_ERROR_NOT_SUPPORTED if the function is unavailable.
 *
 * QAIRT interface structs carry a @c size field reflecting how many bytes the
 * library populated.  This macro checks that the requested field lies within
 * that reported size and is non-NULL before calling it, returning
 * QAIRT_COMMON_ERROR_NOT_SUPPORTED otherwise.  This guards against calling a
 * function that does not exist in the loaded library without requiring a
 * prior capability query, though QairtInfo_hasCapability() should still be
 * used for semantic feature detection.
 *
 * The pointer-to-char casts used in the size check are permitted under the
 * MISRA C:2012/C:2023 Rule 11.3 exception for conversions to char types.
 *
 * @param iface_ptr  Pointer to a QAIRT interface struct (must not be NULL).
 * @param member     The function-pointer field to call (unquoted identifier).
 * @param ...        Arguments forwarded verbatim to the function.
 *
 * @return The return value of the function, or
 *         @c QAIRT_COMMON_ERROR_NOT_SUPPORTED if the function is unavailable.
 *
 * Example:
 * @code
 *   if (QairtInfo_hasCapability(QAIRT_CAPABILITY_FOO) == QAIRT_CAPABILITY_SUPPORTED) {
 *       status = QAIRT_SAFE_CALL(fooInterface, someNewFunction, arg1, arg2);
 *   }
 * @endcode
 */
#define QAIRT_SAFE_CALL(iface_ptr, member, ...)                        \
  (((iface_ptr) != NULL &&                                             \
    (const char*)&(iface_ptr)->member + sizeof((iface_ptr)->member) <= \
        (const char*)(iface_ptr) + (iface_ptr)->size &&                \
    (iface_ptr)->member != NULL)                                       \
       ? (iface_ptr)->member(__VA_ARGS__)                              \
       : ((Qairt_Status_t)QAIRT_COMMON_ERROR_NOT_SUPPORTED))

// Macro controlling visibility of QAIRT API
#ifndef QAIRT_API
#define QAIRT_API
#endif
// Values to use for API version.
#define QAIRT_API_VERSION_MAJOR 0
#define QAIRT_API_VERSION_MINOR 1
#define QAIRT_API_VERSION_PATCH 0

/// NULL backend identifier.
#define QAIRT_BACKEND_ID_NULL 0

// Define ranges for interface IDs
#define QAIRT_MIN_ID_COMMON              1000
#define QAIRT_MAX_ID_COMMON              1999
#define QAIRT_MIN_ID_CAPABILITY          2000
#define QAIRT_MAX_ID_CAPABILITY          2999
#define QAIRT_MIN_ID_OP_PACKAGE          3000
#define QAIRT_MAX_ID_OP_PACKAGE          3999
#define QAIRT_MIN_ID_BACKEND             4000
#define QAIRT_MIN_ID_BACKEND_SAVER       4950
#define QAIRT_MAX_ID_BACKEND_SAVER       4998
#define QAIRT_MAX_ID_BACKEND             4999
#define QAIRT_MIN_ID_CONTEXT             5000
#define QAIRT_MAX_ID_CONTEXT             5999
#define QAIRT_MIN_ID_GRAPH               6000
#define QAIRT_MAX_ID_GRAPH               6999
#define QAIRT_MIN_ID_TENSOR              7000
#define QAIRT_MAX_ID_TENSOR              7999
#define QAIRT_MIN_ID_MEM                 8000
#define QAIRT_MAX_ID_MEM                 8999
#define QAIRT_MIN_ID_SIGNAL              9000
#define QAIRT_MAX_ID_SIGNAL              9999
#define QAIRT_MIN_ID_ERROR               10000
#define QAIRT_MAX_ID_ERROR               10999
#define QAIRT_MIN_ID_LOG                 11000
#define QAIRT_MAX_ID_LOG                 11999
#define QAIRT_MIN_ID_PROFILE             12000
#define QAIRT_MAX_ID_PROFILE             12999
#define QAIRT_MIN_ID_PERF_INFRASTRUCTURE 13000
#define QAIRT_MAX_ID_PERF_INFRASTRUCTURE 13999
#define QAIRT_MIN_ID_DEVICE              14000
#define QAIRT_MAX_ID_DEVICE              14999
#define QAIRT_MIN_ID_INFO                15000
#define QAIRT_MAX_ID_INFO                15999
#define QAIRT_MIN_ID_OPCONFIG            16000
#define QAIRT_MAX_ID_OPCONFIG            16999
#define QAIRT_MIN_ID_QUANTIZE_PARAMS     17000
#define QAIRT_MAX_ID_QUANTIZE_PARAMS     17999
#define QAIRT_MIN_ID_GLOBAL_CONFIG       18000
#define QAIRT_MAX_ID_GLOBAL_CONFIG       18999
#define QAIRT_MIN_ID_SYSTEM              30000
#define QAIRT_MIN_ID_SYSTEM_LOG          30100
#define QAIRT_MIN_ID_SYSTEM_DLC          35000
#define QAIRT_MAX_ID_SYSTEM_DLC          39999
#define QAIRT_MIN_ID_SYSTEM_PROFILE      40000
#define QAIRT_MAX_ID_SYSTEM              49999

#define QAIRT_MIN_ID_HTP 100000
#define QAIRT_MAX_ID_HTP 199999

#define QAIRT_MIN_ID_GPU 200000
#define QAIRT_MAX_ID_GPU 299999

#define QAIRT_MIN_ID_DSP 300000
#define QAIRT_MAX_ID_DSP 399999

#define QAIRT_MIN_ID_CPU 400000
#define QAIRT_MAX_ID_CPU 499999

#define QAIRT_MIN_ID_LPAI 500000
#define QAIRT_MAX_ID_LPAI 599999

// Define ranges for error codes
#define QAIRT_MIN_ERROR_COMMON              1000
#define QAIRT_MAX_ERROR_COMMON              1999
#define QAIRT_MIN_ERROR_CAPABILITY          2000
#define QAIRT_MAX_ERROR_CAPABILITY          2999
#define QAIRT_MIN_ERROR_OP_PACKAGE          3000
#define QAIRT_MAX_ERROR_OP_PACKAGE          3999
#define QAIRT_MIN_ERROR_BACKEND             4000
#define QAIRT_MIN_ERROR_BACKEND_SAVER       4950
#define QAIRT_MAX_ERROR_BACKEND_SAVER       4998
#define QAIRT_MAX_ERROR_BACKEND             4999
#define QAIRT_MIN_ERROR_CONTEXT             5000
#define QAIRT_MAX_ERROR_CONTEXT             5999
#define QAIRT_MIN_ERROR_GRAPH               6000
#define QAIRT_MAX_ERROR_GRAPH               6999
#define QAIRT_MIN_ERROR_TENSOR              7000
#define QAIRT_MAX_ERROR_TENSOR              7999
#define QAIRT_MIN_ERROR_MEM                 8000
#define QAIRT_MAX_ERROR_MEM                 8999
#define QAIRT_MIN_ERROR_SIGNAL              9000
#define QAIRT_MAX_ERROR_SIGNAL              9999
#define QAIRT_MIN_ERROR_ERROR               10000
#define QAIRT_MAX_ERROR_ERROR               10999
#define QAIRT_MIN_ERROR_LOG                 11000
#define QAIRT_MAX_ERROR_LOG                 11999
#define QAIRT_MIN_ERROR_PROFILE             12000
#define QAIRT_MAX_ERROR_PROFILE             12999
#define QAIRT_MIN_ERROR_PERF_INFRASTRUCTURE 13000
#define QAIRT_MAX_ERROR_PERF_INFRASTRUCTURE 13999
#define QAIRT_MIN_ERROR_DEVICE              14000
#define QAIRT_MAX_ERROR_DEVICE              14999
#define QAIRT_MIN_ERROR_INTERFACE           15000
#define QAIRT_MAX_ERROR_INTERFACE           15999
#define QAIRT_MIN_ERROR_OPCONFIG            16000
#define QAIRT_MAX_ERROR_OPCONFIG            16999
#define QAIRT_MIN_ERROR_GLOBAL_CONFIG       18000
#define QAIRT_MAX_ERROR_GLOBAL_CONFIG       18999

// QAIRT System
#define QAIRT_MIN_ERROR_SYSTEM     30000
#define QAIRT_MIN_ERROR_SYSTEM_DLC 35000
#define QAIRT_MAX_ERROR_SYSTEM_DLC 39999
#define QAIRT_MAX_ERROR_SYSTEM     49999

// Backend-specific error ranges
#define QAIRT_MIN_ERROR_HTP 100000
#define QAIRT_MAX_ERROR_HTP 199999

#define QAIRT_MIN_ERROR_GPU 200000
#define QAIRT_MAX_ERROR_GPU 299999

#define QAIRT_MIN_ERROR_CPU 400000
#define QAIRT_MAX_ERROR_CPU 499999

#define QAIRT_MIN_ERROR_LPAI 500000
#define QAIRT_MAX_ERROR_LPAI 599999
#define QAIRT_MIN_ERROR_DSP  300000
#define QAIRT_MAX_ERROR_DSP  399999

//=============================================================================
// Data Types
//=============================================================================

/// Status returned by QAIRT API calls. Compare to QAIRT_SUCCESS to test for success.
typedef uint64_t Qairt_Status_t;

/// Define Qairt Priority
typedef enum {
  /// Usage of the priorities apart from QAIRT_PRIORITY_LOW may be restricted
  QAIRT_PRIORITY_LOWEST = 25,
  /// QAIRT_PRIORITY_LOW is always available for use.
  QAIRT_PRIORITY_LOW = QAIRT_PRIORITY_LOWEST,
  /// Between Low and Normal priorities
  QAIRT_PRIORITY_NORMAL_LOW = 50,
  /// Between Normal Low and Normal High priorities
  QAIRT_PRIORITY_NORMAL  = 100,
  QAIRT_PRIORITY_DEFAULT = QAIRT_PRIORITY_NORMAL,
  /// Between Normal and High Priorities
  QAIRT_PRIORITY_NORMAL_HIGH = 150,
  /// Between Normal High and High Plus Priorities
  QAIRT_PRIORITY_HIGH = 200,
  /// Between High and Critical Priorities
  QAIRT_PRIORITY_HIGH_PLUS = 300,
  /// Between High Plus and Critical Plus Priorities
  QAIRT_PRIORITY_CRITICAL = 400,
  /// Highest available priority
  QAIRT_PRIORITY_CRITICAL_PLUS = 500,
  QAIRT_PRIORITY_HIGHEST       = QAIRT_PRIORITY_CRITICAL_PLUS,
  // Unused, present to ensure 32 bits.
  QAIRT_PRIORITY_UNDEFINED = 0x7FFFFFFF
} Qairt_Priority_t;

/// Define Qairt Precision
typedef enum {
  /// 32-bit floating point precision.
  QAIRT_PRECISION_FLOAT32 = 0,
  /// 16-bit floating point precision.
  QAIRT_PRECISION_FLOAT16 = 1,
  // Unused, present to ensure 32 bits.
  QAIRT_PRECISION_UNDEFINED = 0x7FFFFFFF
} Qairt_Precision_t;

/**
 * @brief a typedef for a function to retrieve interface extensions from an existing interface.
 */
typedef Qairt_Status_t (*Qairt_GetInterfaceFn_t)(uint64_t, void**);

/**
 * @brief An enum which defines error codes commonly used across API components.
 */
typedef enum {
  QAIRT_COMMON_MIN_ERROR = QAIRT_MIN_ERROR_COMMON,
  //////////////////////////////////////////

  /// API or feature is not supported by implementation.
  QAIRT_COMMON_ERROR_NOT_SUPPORTED = 1000,
  /// Memory allocation related error.
  QAIRT_COMMON_ERROR_MEM_ALLOC = 1002,
  /// System level error, such as related to platform / OS services
  QAIRT_COMMON_ERROR_SYSTEM = 1003,
  /// Invalid function argument
  QAIRT_COMMON_ERROR_INVALID_ARGUMENT = 1004,
  /// Illegal operation or sequence of operations
  QAIRT_COMMON_ERROR_OPERATION_NOT_PERMITTED = 1005,
  /// Attempt to use QAIRT API on an unsupported platform
  QAIRT_COMMON_ERROR_PLATFORM_NOT_SUPPORTED = 1006,
  /// Communication errors with platform / OS service (service is recoverable)
  QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION = 1007,
  /// Loaded libraries are of incompatible versions
  QAIRT_COMMON_ERROR_INCOMPATIBLE_BINARIES = 1008,
  /// Attempt to reload library already loaded in this process
  QAIRT_COMMON_ERROR_LOADING_BINARIES = 1009,
  /// Resource allocation related error.
  QAIRT_COMMON_ERROR_RESOURCE_UNAVAILABLE = 1010,
  /// Communication errors with platform / OS service (service is
  /// non-recoverable)
  QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL = 1011,
  /// Invalid function argument
  QAIRT_COMMON_ERROR_INVALID_HANDLE = 1012,
  // Requested element is out of range
  QAIRT_COMMON_ERROR_OUT_OF_RANGE = 1013,
  /// General error, which has not been identified as any other error type.
  QAIRT_COMMON_ERROR_GENERAL = 1100,

  //////////////////////////////////////////
  QAIRT_COMMON_MAX_ERROR = QAIRT_MAX_ERROR_COMMON,
  // Unused, present to ensure 32 bits.
  QAIRT_COMMON_ERROR_UNDEFINED = 0x7FFFFFFF
} QairtCommon_Error_t;

#ifdef __cplusplus
}  // extern "C"
#endif
#endif  // QAISW_QAIRTCOMMON_H

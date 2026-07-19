//==============================================================================
//
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// All rights reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//==============================================================================

/**
 * @file
 * @brief   Backend component API
 *
 *          A header which contains the QAIRT Backend component and the
 *          interfaces it implements.
 */

#ifndef QAISW_QAIRTBACKEND_H
#define QAISW_QAIRTBACKEND_H

#include "QairtBackendConfig.h"
#include "QairtBackendOperationName.h"
#include "QairtCommon/QairtBackendCapabilityKeys.h"
#include "QairtLog/QairtLog.h"
#include "QairtOpConfig/QairtOpConfig.h"
#include "QairtProfile/QairtProfile.h"

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
// Macros
//=============================================================================
#define QAIRT_BACKEND_V1_ID                QAIRT_MIN_ID_BACKEND
#define QAIRT_BACKEND_CONFIG_V1_ID         QAIRT_MIN_ID_BACKEND + 1
#define QAIRT_BACKEND_OPERATION_NAME_V1_ID QAIRT_MIN_ID_BACKEND + 2

//=============================================================================
// Datatypes
//=============================================================================
/**
 * @brief QAIRT Backend API result / error codes.
 */
typedef enum {
  QAIRT_BACKEND_MIN_ERROR = QAIRT_MIN_ERROR_BACKEND,
  ////////////////////////////////////////////

  /// Qairt Backend success
  QAIRT_BACKEND_NO_ERROR = QAIRT_SUCCESS,
  /// General error relating to memory allocation in Backend API
  QAIRT_BACKEND_ERROR_MEM_ALLOC = QAIRT_COMMON_ERROR_MEM_ALLOC,
  /// Backend attempted to be created on an unsupported platform
  QAIRT_BACKEND_ERROR_UNSUPPORTED_PLATFORM = QAIRT_COMMON_ERROR_PLATFORM_NOT_SUPPORTED,
  /// Backend failed to initialize
  QAIRT_BACKEND_ERROR_CANNOT_INITIALIZE = 4000,
  /// Failed to free allocated resources during termination
  QAIRT_BACKEND_ERROR_TERMINATE_FAILED = 4002,
  /// Backend does not support requested functionality
  QAIRT_BACKEND_ERROR_NOT_SUPPORTED = 4003,
  /// Invalid function argument
  QAIRT_BACKEND_ERROR_INVALID_ARGUMENT = 4004,
  /// Could not find specified op package
  QAIRT_BACKEND_ERROR_OP_PACKAGE_NOT_FOUND = 4005,
  /// Could not load interface provider from op package library
  QAIRT_BACKEND_ERROR_OP_PACKAGE_IF_PROVIDER_NOT_FOUND = 4006,
  /// Failed to register op package
  QAIRT_BACKEND_ERROR_OP_PACKAGE_REGISTRATION_FAILED = 4007,
  /// Backend does not support the op config's interface version
  QAIRT_BACKEND_ERROR_OP_PACKAGE_UNSUPPORTED_VERSION = 4008,
  /// An Op with the same package name and op name was already registered
  QAIRT_BACKEND_ERROR_OP_PACKAGE_DUPLICATE = 4009,
  /// Inconsistent backend configuration
  QAIRT_BACKEND_ERROR_INCONSISTENT_CONFIG = 4010,
  /// Invalid backend handle
  QAIRT_BACKEND_ERROR_INVALID_HANDLE = 4011,
  /// Invalid config
  QAIRT_BACKEND_ERROR_INVALID_CONFIG = 4012,
  ////////////////////////////////////////////
  QAIRT_BACKEND_MAX_ERROR = QAIRT_MAX_ERROR_BACKEND,
  // Unused, present to ensure 32 bits.
  QAIRT_BACKEND_ERROR_UNDEFINED = 0x7FFFFFFF
} QairtBackend_Error_t;

/**
 * @brief   Backend handle used to interface with Backend APIs
 *
 *          The handle itself is created and freed by the client.
 */
typedef struct _QairtBackend_Handle_t* QairtBackend_Handle_t;

//=============================================================================
// Public Functions
//=============================================================================
/**
 * @brief Initialize a backend library and create a backend handle. Function is
 *        re-entrant and thread-safe.
 *
 * @param[in] logHandle A handle to the logger. Use a NULL handle to disable
 *                      logging. QairtBackend does not manage the lifecycle of
 *                      the logger and must be freed by using QairtLog_free().
 * @param[in] configHandle A handle to a backend configuration, which is managed
 *                         from the BackendConfig component APIs. NULL is
 *                         allowed and indicates no config options are provided. This handle may be
 *                         freed once the backend is created.
 * @param[out] backendHandle A handle to the created backend.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_BACKEND_ERROR_UNSUPPORTED_PLATFORM: Backend attempted
 *           to be created on unsupported platform
 *         - QAIRT_BACKEND_ERROR_INCONSISTENT_CONFIG: One or more backend
 *           configurations are inconsistent between multiple create calls.
 *           Refer to backend headers for which configuration options must be consistent.
 *         - QAIRT_BACKEND_ERROR_CANNOT_INITIALIZE: backend failed to
 *           initialize
 *         - QAIRT_BACKEND_ERROR_MEM_ALLOC: error related to memory
 *           allocation
 *         - QAIRT_BACKEND_ERROR_INVALID_HANDLE: _logger_ is not a valid
 *           handle
 *         - QAIRT_BACKEND_ERROR_INVALID_CONFIG: one or more config values
 *           is invalid
 *         - QAIRT_BACKEND_ERROR_NOT_SUPPORTED: an optional feature is not
 *           supported
 *
 * @note Use corresponding API through QairtBackend_V1_t.
 */
Qairt_Status_t QairtBackend_create(QairtLog_Handle_t logHandle,
                                   QairtBackend_ConfigHandle_t configHandle,
                                   QairtBackend_Handle_t* backendHandle);

/**
 * @brief Frees a backend handle
 *
 * @param[in] backendHandle A handle to a created backend.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered.
 *         - QAIRT_BACKEND_ERROR_MEM_ALLOC: error related to memory
 *           deallocation
 *         - QAIRT_BACKEND_ERROR_TERMINATE_FAILED: indicates failure to
 *           free resources or failure to invalidate handles and pointers allocated by the
 *           library
 *         - QAIRT_BACKEND_ERROR_INVALID_HANDLE: _backend_ is not a valid
 *           handle
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurence
 *           (successful recovery)
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurence
 *           (unsuccessful recovery)
 *
 * @note Use corresponding API through QairtBackend_V1_t.
 */
Qairt_Status_t QairtBackend_free(QairtBackend_Handle_t backendHandle);

/**
 * @brief A function to set/modify configuration options on an already generated
 *        backend.
 *
 * @param[in] backendHandle A handle to a created backend.
 *
 * @param[in] configHandle A handle to a backend configuration, which is managed
 *                         from the BackendConfig component APIs.
 * @return Error code:
 *         - QAIRT_SUCCESS: no error is encountered
 *         - QAIRT_BACKEND_ERROR_INVALID_HANDLE: _backend_ is not a valid
 *           handle
 *         - QAIRT_BACKEND_ERROR_INVALID_CONFIG: at least one config
 *           option is invalid
 *         - QAIRT_BACKEND_ERROR_NOT_SUPPORTED: an optional feature is not
 *           supported
 *
 * @note Use corresponding API through QairtBackend_V1_t.
 */
Qairt_Status_t QairtBackend_setConfig(QairtBackend_Handle_t backendHandle,
                                      QairtBackend_ConfigHandle_t configHandle);

/**
 * @brief Register an operation package with the backend handle.
 *
 * @param[in] backendHandle A handle to a created backend.
 *
 * @param[in] packagePath Path on disk to the op package library to load.
 *
 * @param[in] interfaceProvider The name of a function in the op package library
 *                              which satisfies the
 *                              QairtOpPackage_InterfaceProvider_t interface. The backend will use
 * this function to retrieve the op package's interface.
 * @param[in] target An optional parameter specifying the target platform on
 *                   which the backend must register the op package. Required in
 *                   scenarios where an op package is to be loaded on a processing unit that is
 *                   different from the target on which the backend runs. Ex: loading a DSP op
 *                   package on ARM for optional online context caching. Refer to additional
 *                   documentation for a list of permissible target names.
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_BACKEND_ERROR_INVALID_ARGUMENT: if _packagePath_ or
 *           _interfaceProvider_ is NULL
 *         - QAIRT_BACKEND_ERROR_OP_PACKAGE_NOT_FOUND: Could not open
 *           _packagePath_
 *         - QAIRT_BACKEND_ERROR_OP_PACKAGE_IF_PROVIDER_NOT_FOUND: Could
 *           not find _interfaceProvider_ symbol in package library
 *         - QAIRT_BACKEND_ERROR_OP_PACKAGE_REGISTRATION_FAILED: Op
 *           package registration failed
 *         - QAIRT_BACKEND_ERROR_OP_PACKAGE_UNSUPPORTED_VERSION: Op
 *           package has interface version not supported by this backend
 *         - QAIRT_BACKEND_ERROR_NOT_SUPPORTED: Op package registration is
 *           not supported.
 *         - QAIRT_BACKEND_ERROR_INVALID_HANDLE: _backend_ is not a valid
 *           handle
 *         - QAIRT_BACKEND_ERROR_OP_PACKAGE_DUPLICATE:
 *           OpPackageName+OpName must be unique. Op package content information can be be
 *           obtained with QairtOpPackage interface. Indicates that an Op with the same
 *           package name and op name was already registered.
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurence
 *           (successful recovery)
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurence
 *           (unsuccessful recovery)
 *
 * @note Use corresponding API through QairtBackend_V1_t.
 */
Qairt_Status_t QairtBackend_registerOpPackage(QairtBackend_Handle_t backendHandle,
                                              const char* packagePath,
                                              const char* interfaceProvider,
                                              const char* target);

/**
 * @brief Get the supported operations registered to a backend handle including
 *        built-in ops.
 *
 * @param[in] backendHandle A created backend handle. Can be NULL to obtain the
 *                          built-in op package.
 * @param[out] operationsHandle Array of operation handles, which are managed
 *                              from the Backend Operation Name component APIs.
 * @param[out] numOperations Number of supported operations.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_BACKEND_ERROR_INVALID_ARGUMENT: if _numOperations_ or
 *           _operationsHandle_ is NULL
 *         - QAIRT_BACKEND_ERROR_INVALID_HANDLE: _backendHandle_ is not a
 *           valid handle
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION: SSR occurence
 *           (successful recovery)
 *         - QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL: SSR occurence
 *           (unsuccessful recovery)
 *
 * @note Use corresponding API through QairtBackend_V1_t.
 */
Qairt_Status_t QairtBackend_getSupportedOperations(
    QairtBackend_Handle_t backendHandle,
    const QairtBackend_OperationNameHandle_t** operationsHandle,
    uint32_t* numOperations);

/**
 * @brief A method to validate op config with an appropriate op package. This is
 *        a wrapper API around the actual OpPackage interface method that
 *        performs op validation. The Backend may pick an appropriate op package among
 *        ones that are registered with it for validation based on the attributes of
 *        the op configuration.
 *
 * @param[in] backendHandle A created backend handle.
 *
 * @param[in] opConfigHandle A created opConfig handle, which is managed by the
 *                           OpConfig component APIs.
 * @return Error code
 *         - QAIRT_SUCCESS if validation is successful
 *         - QAIRT_OP_PACKAGE_ERROR_VALIDATION_FAILURE: op config
 *           validation failed
 *         - QAIRT_BACKEND_ERROR_NOT_SUPPORTED: Validation API not
 *           supported
 *         - QAIRT_BACKEND_ERROR_OP_PACKAGE_NOT_FOUND: No op package with
 *           matching op config attributes found.
 *         - QAIRT_BACKEND_ERROR_INVALID_HANDLE: _backend_ is not a valid
 *           handle
 * @note Use corresponding API through QairtBackend_V1_t.
 */
Qairt_Status_t QairtBackend_validateOpConfig(QairtBackend_Handle_t backendHandle,
                                             QairtOpConfig_Handle_t opConfigHandle);

/**
 * @brief Create a handle to a profile object.
 *
 * @param[in] backendHandle A backend handle.
 *
 * @param[in] level Granularity level at which the profile should collect events.
 *
 * @param[out] profileHandle A profile handle.
 *
 * @return Error code
 *         - QAIRT_SUCCESS: No error encountered
 *         - QAIRT_BACKEND_ERROR_INVALID_ARGUMENT: _profile_ is NULL or _level_ is invalid.
 *         - QAIRT_BACKEND_ERROR_NOT_SUPPORTED: Profiling is unsupported on a backend.
 *         - QAIRT_BACKEND_ERROR_MEM_ALLOC: Error in allocating memory when creating profile handle
 *         - QAIRT_BACKEND_ERROR_INVALID_HANDLE: _backend_ is not a valid handle
 *
 * @note Use corresponding API through QairtInterface_t.
 */
Qairt_Status_t QairtBackend_createProfile(QairtBackend_Handle_t backendHandle,
                                          QairtProfile_Level_t level,
                                          QairtProfile_Handle_t* profileHandle);

typedef Qairt_Status_t (*QairtBackend_CreateFn_t)(QairtLog_Handle_t,
                                                  QairtBackend_ConfigHandle_t,
                                                  QairtBackend_Handle_t*);

typedef Qairt_Status_t (*QairtBackend_FreeFn_t)(QairtBackend_Handle_t);

typedef Qairt_Status_t (*QairtBackend_SetConfigFn_t)(QairtBackend_Handle_t,
                                                     QairtBackend_ConfigHandle_t);

typedef Qairt_Status_t (*QairtBackend_RegisterOpPackageFn_t)(QairtBackend_Handle_t,
                                                             const char*,
                                                             const char*,
                                                             const char*);

typedef Qairt_Status_t (*QairtBackend_GetSupportedOperationsFn_t)(
    QairtBackend_Handle_t, const QairtBackend_OperationNameHandle_t**, uint32_t*);

typedef Qairt_Status_t (*QairtBackend_ValidateOpConfigFn_t)(QairtBackend_Handle_t,
                                                            QairtOpConfig_Handle_t);

typedef Qairt_Status_t (*QairtBackend_CreateProfileFn_t)(QairtBackend_Handle_t,
                                                         QairtProfile_Level_t,
                                                         QairtProfile_Handle_t*);

//=============================================================================
// Public Interfaces
//=============================================================================
/**
 * @struct QairtBackend_V1_t
 * @brief This struct contains the functions that define the V1 interface for
 *        QAIRT Backend. This interface can be retrieved through the id
 *        QAIRT_BACKEND_V1_ID.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;
  QairtBackend_CreateFn_t create;
  QairtBackend_FreeFn_t free;
  QairtBackend_SetConfigFn_t setConfig;
  QairtBackend_RegisterOpPackageFn_t registerOpPackage;
  QairtBackend_GetSupportedOperationsFn_t getSupportedOperations;
  QairtBackend_ValidateOpConfigFn_t validateOpConfig;
  QairtBackend_CreateProfileFn_t createProfile;
} QairtBackend_V1_t;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // QAISW_QAIRTBACKEND_H

//==============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//==============================================================================

/**
 * @file
 * @brief   System Builder component API
 *
 *          A header which contains the QAIRT System Builder
 *          component for configuring and building Qairt instances.
 */

#ifndef QAISW_QAIRTSYSTEMBUILDER_H
#define QAISW_QAIRTSYSTEMBUILDER_H

#include "QairtContext/QairtContextConfig.h"
#include "QairtLog/QairtLog.h"
#include "QairtProfile/QairtProfile.h"
#include "QairtSystemCommon.h"
#include "QairtSystemDlc.h"

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
// Macros
//=============================================================================

/**
 * @brief Interface ID for QairtSystemBuilder V1
 */
#define QAIRT_SYSTEM_BUILDER_V1_ID (QAIRT_MIN_ID_SYSTEM + 10)

/**
 * @brief Sentinel value for QairtSystemBuilder_setBackendType() indicating "all ops" (catch-all).
 *
 * Pass as both opStartIndex and opEndIndex to designate this backend as the
 * fallback for all ops not covered by an explicit range (or as the sole backend
 * for single-backend inference).
 */
#define QAIRT_PARTITION_ALL_OPS  0xffffffffU

// /**
//  * @brief Interface ID for Qairt V1
//  */
// #define QAIRT_V1_ID (QAIRT_MIN_ID_SYSTEM + 11)

//=============================================================================
// Data Types
//=============================================================================

/**
 * @brief Handle to a System Builder instance
 */
typedef void* QairtSystemBuilder_Handle_t;

/**
 * @brief Handle to a Qairt runtime instance
 */
typedef void* Qairt_Handle_t;

/**
 * @brief Handle to a SystemApi instance
 */
typedef void* QairtSystemApi_Handle_t;

/**
 * @brief Backend type enum for selecting inference backend
 */
typedef enum {
  QAIRT_BACKEND_TYPE_CPU = 0,  ///< CPU backend (libQairtCpu.so / QairtCpu.dll) - SUPPORTED
  QAIRT_BACKEND_TYPE_GPU = 1,  ///< GPU backend (libQairtGpu.so / QairtGpu.dll) - SUPPORTED
  QAIRT_BACKEND_TYPE_DSP =
      2,  ///< DSP backend / HexNN v2 (libQairtDsp.so / QairtDsp.dll) - NOT SUPPORTED
  QAIRT_BACKEND_TYPE_HTP =
      3,  ///< HTP backend / HexNN v3 (libQairtHtp.so / QairtHtp.dll) - SUPPORTED
  QAIRT_BACKEND_TYPE_HTA = 4,  ///< HTA backend (libQairtHta.so / QairtHta.dll) - NOT SUPPORTED
  QAIRT_BACKEND_TYPE_AIP = 5,  ///< AIP backend (libQairtAip.so / QairtAip.dll) - NOT SUPPORTED
  QAIRT_BACKEND_TYPE_LPAI =
      6,  ///< LPAI/EAI backend (libQairtLpai.so / QairtLpai.dll) - NOT SUPPORTED
  QAIRT_BACKEND_TYPE_EAI       = 6,  ///< Alias for LPAI
  QAIRT_BACKEND_TYPE_UNDEFINED = 0x7FFFFFFF
} QairtSystem_BackendType_t;

/**
 * @brief Performance profile enum for power/performance trade-off
 */
typedef enum {
  QAIRT_PERF_PROFILE_DEFAULT                    = 0,          ///< Backend default
  QAIRT_PERF_PROFILE_LOW_BALANCED               = 1,          ///< Low balanced mode
  QAIRT_PERF_PROFILE_BALANCED                   = 2,          ///< Balanced power/performance
  QAIRT_PERF_PROFILE_HIGH_PERFORMANCE           = 3,          ///< Maximum performance
  QAIRT_PERF_PROFILE_SUSTAINED_HIGH_PERFORMANCE = 4,          ///< Sustained high performance
  QAIRT_PERF_PROFILE_BURST                      = 5,          ///< Short burst mode
  QAIRT_PERF_PROFILE_EXTREME_POWER_SAVER        = 6,          ///< Extreme power saving
  QAIRT_PERF_PROFILE_LOW_POWER_SAVER            = 7,          ///< Low power saver
  QAIRT_PERF_PROFILE_POWER_SAVER                = 8,          ///< Power saver mode
  QAIRT_PERF_PROFILE_HIGH_POWER_SAVER           = 9,          ///< High power saver
  QAIRT_PERF_PROFILE_SYSTEM_SETTINGS            = 10,         ///< Use system settings
  QAIRT_PERF_PROFILE_NO_USER_INPUT              = 11,         ///< No preference specified
  QAIRT_PERF_PROFILE_UNDEFINED                  = 0x7FFFFFFF  ///< Undefined value
} QairtSystem_PerfProfile_t;

/**
 * @brief System Builder API error codes
 */
typedef enum {
  QAIRT_SYSTEM_BUILDER_MIN_ERROR = QAIRT_MIN_ERROR_SYSTEM + 100,
  //////////////////////////////////////////

  /// System Builder success
  QAIRT_SYSTEM_BUILDER_NO_ERROR = QAIRT_SUCCESS,
  /// Invalid/NULL System Builder handle
  QAIRT_SYSTEM_BUILDER_ERROR_INVALID_HANDLE = 30100,
  /// Invalid function argument
  QAIRT_SYSTEM_BUILDER_ERROR_INVALID_ARGUMENT = 30101,
  /// Memory allocation error
  QAIRT_SYSTEM_BUILDER_ERROR_MEM_ALLOC = 30102,
  /// Configuration validation failed
  QAIRT_SYSTEM_BUILDER_ERROR_VALIDATION_FAILED = 30103,
  /// Build process failed
  QAIRT_SYSTEM_BUILDER_ERROR_BUILD_FAILED = 30104,
  /// Backend library not found
  QAIRT_SYSTEM_BUILDER_ERROR_BACKEND_NOT_FOUND = 30105,
  /// Backend type not supported in QAIRT
  QAIRT_SYSTEM_BUILDER_ERROR_BACKEND_NOT_SUPPORTED = 30106,

  //////////////////////////////////////////
  QAIRT_SYSTEM_BUILDER_MAX_ERROR = QAIRT_MIN_ERROR_SYSTEM + 199,
  /// Unused, present to ensure 32 bits
  QAIRT_SYSTEM_BUILDER_ERROR_UNDEFINED = 0x7FFFFFFF
} QairtSystemBuilder_Error_t;

//=============================================================================
// Public Functions
//=============================================================================

/**
 * @brief Create a system builder handle
 *
 * Creates a new QairtSystemBuilder instance for configuring and building
 * Qairt runtime instances.
 *
 * @param[out] builderHandle A handle to the created builder
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Builder was successfully created
 *         - QAIRT_SYSTEM_BUILDER_ERROR_INVALID_ARGUMENT: _builderHandle_ is NULL
 *         - QAIRT_SYSTEM_BUILDER_ERROR_MEM_ALLOC: Memory allocation failed
 *
 * @note Use corresponding API through QairtSystemBuilder_V1_t
 *
 * Example:
 * @code
 *   QairtSystemBuilder_Handle_t builder;
 *   Qairt_Status_t status = QairtSystemBuilder_create(&builder);
 *   if (status != QAIRT_SUCCESS) {
 *     // Handle error
 *   }
 * @endcode
 */
Qairt_Status_t QairtSystemBuilder_create(QairtSystemBuilder_Handle_t* builderHandle);

/**
 * @brief Free a system builder handle
 *
 * Destroys the QairtSystemBuilder instance and releases all associated resources.
 *
 * @param[in] builderHandle Handle to the builder to free
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Builder was successfully freed
 *         - QAIRT_SYSTEM_BUILDER_ERROR_INVALID_HANDLE: Invalid builder handle
 *
 * @note Use corresponding API through QairtSystemBuilder_V1_t
 */
Qairt_Status_t QairtSystemBuilder_free(QairtSystemBuilder_Handle_t builderHandle);

/**
 * @brief Set the DLC (Deep Learning Container) for the model
 *
 * Specifies the SystemDlc object containing the model to execute.
 *
 * @param[in] builderHandle Handle to the builder
 * @param[in] dlcHandle Handle to the SystemDlc object
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: DLC was successfully set
 *         - QAIRT_SYSTEM_BUILDER_ERROR_INVALID_HANDLE: Invalid builder or DLC handle
 *         - QAIRT_SYSTEM_BUILDER_ERROR_INVALID_ARGUMENT: _dlcHandle_ is NULL
 *
 * @note Use corresponding API through QairtSystemBuilder_V1_t
 */
Qairt_Status_t QairtSystemBuilder_setDlc(QairtSystemBuilder_Handle_t builderHandle,
                                         QairtSystemDlc_Handle_t dlcHandle);

/**
 * @brief Set the DLC via a raw pointer that carries a valid API table. For C++ API use only.
 *
 * The standard setDlc path accepts a QairtSystemDlc_Handle_t from the handle
 * manager, which is a shell without an API table. This variant accepts the raw
 * qairt::SystemDlc* (as void*) so the SystemApiTable is preserved through the
 * CAPI boundary. The builder stores a non-owning reference; the caller must
 * keep the original shared_ptr alive for the lifetime of the builder and the
 * resulting Qairt instance.
 *
 * @param[in] builderHandle  Handle to the builder.
 * @param[in] dlcPtr         Raw qairt::SystemDlc* cast to void*. Must not be NULL.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: DLC was successfully set
 *         - QAIRT_SYSTEM_BUILDER_ERROR_INVALID_ARGUMENT: dlcPtr is NULL
 *         - QAIRT_SYSTEM_BUILDER_ERROR_INVALID_HANDLE: Invalid builder handle
 *
 * @note This function is intended to be called from the C++ QairtSystemBuilder
 *       wrapper only. C users should use QairtSystemBuilder_setDlc instead.
 */
Qairt_Status_t QairtSystemBuilder_setDlcPtr(QairtSystemBuilder_Handle_t builderHandle,
                                            void* dlcPtr);

/**
 * @brief Set the logging level
 *
 * Controls the verbosity of logging output.
 *
 * @param[in] builderHandle Handle to the builder
 * @param[in] level Log level (e.g., QAIRT_LOG_LEVEL_ERROR, QAIRT_LOG_LEVEL_WARN,
 *                  QAIRT_LOG_LEVEL_INFO, QAIRT_LOG_LEVEL_VERBOSE, QAIRT_LOG_LEVEL_DEBUG)
 *                  Default is QAIRT_LOG_LEVEL_ERROR
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Log level was successfully set
 *         - QAIRT_SYSTEM_BUILDER_ERROR_INVALID_HANDLE: Invalid builder handle
 *
 * @note Use corresponding API through QairtSystemBuilder_V1_t
 */
Qairt_Status_t QairtSystemBuilder_setLogLevel(QairtSystemBuilder_Handle_t builderHandle,
                                              QairtLog_Level_t level);

/**
 * @brief Set the profiling level
 *
 * Controls the level of performance profiling.
 *
 * @param[in] builderHandle Handle to the builder
 * @param[in] level Profiling level:
 *                  - 0: OFF (no profiling)
 *                  - QAIRT_PROFILE_LEVEL_BASIC (1): Basic profiling
 *                  - QAIRT_PROFILE_LEVEL_DETAILED (2): Detailed profiling
 *                  - QAIRT_PROFILE_LEVEL_BACKEND (1000+): Backend-specific profiling
 *                  Default is 0 (OFF)
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Profiling level was successfully set
 *         - QAIRT_SYSTEM_BUILDER_ERROR_INVALID_HANDLE: Invalid builder handle
 *
 * @note Use corresponding API through QairtSystemBuilder_V1_t
 *
 * Example:
 * @code
 *   QairtSystemBuilder_setProfilingLevel(builder, QAIRT_PROFILE_LEVEL_BASIC);
 * @endcode
 */
Qairt_Status_t QairtSystemBuilder_setProfilingLevel(QairtSystemBuilder_Handle_t builderHandle,
                                                    QairtProfile_Level_t level);

/**
 * @brief Set the profiling option
 *
 * Enables specific profiling features.
 *
 * @param[in] builderHandle Handle to the builder
 * @param[in] option Profiling option (bitfield):
 *                   - 0: NONE (no special profiling)
 *                   - 1: OPTRACE (enable OpTrace profiling)
 *                   Default is 0 (NONE)
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Profiling option was successfully set
 *         - QAIRT_SYSTEM_BUILDER_ERROR_INVALID_HANDLE: Invalid builder handle
 *
 * @note Use corresponding API through QairtSystemBuilder_V1_t
 */
Qairt_Status_t QairtSystemBuilder_setProfilingOption(QairtSystemBuilder_Handle_t builderHandle,
                                                     uint32_t option);

/**
 * @brief Set the performance profile
 *
 * Controls the power/performance trade-off for execution.
 *
 * @param[in] builderHandle Handle to the builder
 * @param[in] profile Performance profile:
 *                    - QAIRT_PERF_PROFILE_DEFAULT: Backend default
 *                    - QAIRT_PERF_PROFILE_HIGH_PERFORMANCE: Maximum performance
 *                    - QAIRT_PERF_PROFILE_POWER_SAVER: Power saving mode
 *                    - QAIRT_PERF_PROFILE_BALANCED: Balance power and performance
 *                    - QAIRT_PERF_PROFILE_BURST: Short burst of high performance
 *                    - QAIRT_PERF_PROFILE_SUSTAINED_HIGH_PERFORMANCE: Sustained high performance
 *                    - QAIRT_PERF_PROFILE_NO_USER_INPUT: No preference (default)
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Performance profile was successfully set
 *         - QAIRT_SYSTEM_BUILDER_ERROR_INVALID_HANDLE: Invalid builder handle
 *
 * @note Use corresponding API through QairtSystemBuilder_V1_t
 *
 * Example:
 * @code
 *   QairtSystemBuilder_setPerfProfile(builder, QAIRT_PERF_PROFILE_HIGH_PERFORMANCE);
 * @endcode
 */
Qairt_Status_t QairtSystemBuilder_setPerfProfile(QairtSystemBuilder_Handle_t builderHandle,
                                                 QairtSystem_PerfProfile_t profile);

/**
 * @brief Set platform-specific options
 *
 * Configures platform-specific settings as a string.
 *
 * @param[in] builderHandle Handle to the builder
 * @param[in] options Platform options string (format: "key1:value1,key2:value2")
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Platform options were successfully set
 *         - QAIRT_SYSTEM_BUILDER_ERROR_INVALID_HANDLE: Invalid builder handle
 *         - QAIRT_SYSTEM_BUILDER_ERROR_INVALID_ARGUMENT: _options_ is NULL
 *
 * @note Use corresponding API through QairtSystemBuilder_V1_t
 */
Qairt_Status_t QairtSystemBuilder_setPlatformOptions(QairtSystemBuilder_Handle_t builderHandle,
                                                     const char* options);

/**
 * @brief Set device-specific options
 *
 * Configures device-specific settings as a string.
 *
 * @param[in] builderHandle Handle to the builder
 * @param[in] options Device options string (format: "deviceId:0,coreId:1")
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Device options were successfully set
 *         - QAIRT_SYSTEM_BUILDER_ERROR_INVALID_HANDLE: Invalid builder handle
 *         - QAIRT_SYSTEM_BUILDER_ERROR_INVALID_ARGUMENT: _options_ is NULL
 *
 * @note Use corresponding API through QairtSystemBuilder_V1_t
 */
Qairt_Status_t QairtSystemBuilder_setDeviceOptions(QairtSystemBuilder_Handle_t builderHandle,
                                                   const char* options);

/**
 * @brief Add an operator package specification
 *
 * Registers a custom operator package for use during inference.
 *
 * @param[in] builderHandle Handle to the builder
 * @param[in] path Path to the operator package library
 * @param[in] interfaceProvider Interface provider name (can be NULL for default)
 * @param[in] target Target platform (e.g., "CPU", "GPU", "DSP", can be NULL for default)
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Operator package was successfully added
 *         - QAIRT_SYSTEM_BUILDER_ERROR_INVALID_HANDLE: Invalid builder handle
 *         - QAIRT_SYSTEM_BUILDER_ERROR_INVALID_ARGUMENT: _path_ is NULL
 *
 * @note Use corresponding API through QairtSystemBuilder_V1_t
 */
Qairt_Status_t QairtSystemBuilder_addOpPackage(QairtSystemBuilder_Handle_t builderHandle,
                                               const char* path,
                                               const char* interfaceProvider,
                                               const char* target);

/**
 * @brief Set the backend type for inference execution
 *
 * Converts the backend type enum to the appropriate library name.
 * The library will be loaded later by the underlying backend initialization.
 *
 * @param[in] builderHandle Handle to the builder
 * @param[in] backendType Backend type to use. Valid values:
 *
 *                        **SUPPORTED BACKENDS:**
 *                        - QAIRT_BACKEND_TYPE_CPU (0): CPU backend
 *                          Library: libQairtCpu.so (Linux) / QairtCpu.dll (Windows)
 *
 *                        - QAIRT_BACKEND_TYPE_GPU (1): GPU backend
 *                          Library: libQairtGpu.so (Linux) / QairtGpu.dll (Windows)
 *
 *                        - QAIRT_BACKEND_TYPE_HTP (3): HTP backend (HexNN v3)
 *                          Library: libQairtHtp.so (Linux) / QairtHtp.dll (Windows)
 *
 *                        **NOT SUPPORTED IN QAIRT:**
 *                        - QAIRT_BACKEND_TYPE_DSP (2): DSP backend (HexNN v2)
 *                        - QAIRT_BACKEND_TYPE_HTA (4): HTA backend
 *                        - QAIRT_BACKEND_TYPE_AIP (5): AIP backend
 *                        - QAIRT_BACKEND_TYPE_LPAI (6): LPAI/EAI backend
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Backend type was successfully set
 *         - QAIRT_SYSTEM_BUILDER_ERROR_INVALID_HANDLE: Invalid builder handle
 *         - QAIRT_SYSTEM_BUILDER_ERROR_INVALID_ARGUMENT: Invalid backend type
 *         - QAIRT_SYSTEM_BUILDER_ERROR_BACKEND_NOT_SUPPORTED: Backend not supported in QAIRT
 *
 * @note Use corresponding API through QairtSystemBuilder_V1_t
 * @note Only CPU, GPU, and HTP backends are currently supported in QAIRT
 * @note The library must be in LD_LIBRARY_PATH for successful loading during initialization
 *
 * Example:
 * @code
 *   // Single backend (all ops on HTP):
 *   QairtSystemBuilder_setBackendType(builder, QAIRT_BACKEND_TYPE_HTP);
 * @endcode
 */
Qairt_Status_t QairtSystemBuilder_setBackendType(QairtSystemBuilder_Handle_t builderHandle,
                                                  QairtSystem_BackendType_t   backendType);

/**
 * @brief Add a multi-backend partition: ops [opStartIndex, opEndIndex] run on @p backendType.
 *
 * Pass @c QAIRT_PARTITION_ALL_OPS for both opStartIndex and opEndIndex to create a catch-all
 * partition (equivalent to QairtSystemBuilder_setBackendType()).
 * May be called multiple times to build a multi-backend partition specification.
 * Mutually exclusive with QairtSystemBuilder_setBackendOrder().
 *
 * @param[in] builderHandle  Handle to the builder
 * @param[in] backendType    Backend to assign to this range
 * @param[in] opStartIndex   Inclusive start op index (0-based); QAIRT_PARTITION_ALL_OPS = catch-all
 * @param[in] opEndIndex     Inclusive end op index; QAIRT_PARTITION_ALL_OPS = catch-all
 * @param[in] priority       Execution priority for this partition
 * @param[in] perfProfile    Performance profile for this partition
 *
 * Example:
 * @code
 *   // Multi-backend (ops 0-100 on HTP, rest on CPU):
 *   QairtSystemBuilder_setBackendPartition(b, QAIRT_BACKEND_TYPE_HTP, 0, 100,
 *                                          QAIRT_PRIORITY_HIGH, QAIRT_PERF_PROFILE_BURST);
 *   QairtSystemBuilder_setBackendType(b, QAIRT_BACKEND_TYPE_CPU);
 * @endcode
 */
Qairt_Status_t QairtSystemBuilder_setBackendPartition(QairtSystemBuilder_Handle_t builderHandle,
                                                       QairtSystem_BackendType_t   backendType,
                                                       uint32_t                    opStartIndex,
                                                       uint32_t                    opEndIndex,
                                                       Qairt_Priority_t            priority,
                                                       QairtSystem_PerfProfile_t   perfProfile);

/**
 * @brief Set backends in priority order for automatic op-level partitioning.
 *
 * For each op in the graph, QAIRT tries backends in the given order and assigns
 * the op to the first backend that supports it. Consecutive ops on the same
 * backend are grouped into a single subnet.
 *
 * Mutually exclusive with QairtSystemBuilder_setBackendType(); an error is
 * returned by QairtSystemBuilder_validate() if both are set.
 *
 * @param[in] builderHandle  Handle to the builder
 * @param[in] backends       Array of backend types in descending priority order
 * @param[in] numBackends    Number of entries in the @p backends array (>= 1)
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Backend order was set successfully
 *         - QAIRT_SYSTEM_BUILDER_ERROR_INVALID_HANDLE: Invalid builder handle
 *         - QAIRT_SYSTEM_BUILDER_ERROR_INVALID_ARGUMENT: @p backends is NULL or @p numBackends is 0
 *
 * Example:
 * @code
 *   QairtSystem_BackendType_t order[] = {QAIRT_BACKEND_TYPE_HTP, QAIRT_BACKEND_TYPE_CPU};
 *   QairtSystemBuilder_setBackendOrder(builder, order, 2);
 * @endcode
 */
Qairt_Status_t QairtSystemBuilder_setBackendOrder(QairtSystemBuilder_Handle_t      builderHandle,
                                                   const QairtSystem_BackendType_t* backends,
                                                   uint32_t                         numBackends);

/**
 * @brief Set the priority for context creation
 *
 * Sets the execution priority that will be applied to the context.
 * Higher priority contexts may receive preferential scheduling and resource allocation.
 *
 * @param[in] builderHandle Handle to the builder
 * @param[in] priority Priority value. Valid values:
 *                     - QAIRT_PRIORITY_LOW : Always available for use
 *                     - QAIRT_PRIORITY_LOWEST : Alias for QAIRT_PRIORITY_LOW
 *                     - QAIRT_PRIORITY_NORMAL_LOW : Between Low and Normal priorities
 *                     - QAIRT_PRIORITY_NORMAL : Default priority
 *                     - QAIRT_PRIORITY_DEFAULT : Alias for QAIRT_PRIORITY_NORMAL
 *                     - QAIRT_PRIORITY_NORMAL_HIGH : Between Normal and High priorities
 *                     - QAIRT_PRIORITY_HIGH : Between Normal High and High Plus priorities
 *                     - QAIRT_PRIORITY_HIGH_PLUS : Between High and Critical priorities
 *                     - QAIRT_PRIORITY_CRITICAL : Between High Plus and Critical Plus priorities
 *                     - QAIRT_PRIORITY_CRITICAL_PLUS : Highest available priority
 *                     - QAIRT_PRIORITY_HIGHEST : Alias for QAIRT_PRIORITY_CRITICAL_PLUS
 *
 *                     Default: QAIRT_PRIORITY_NORMAL
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Priority was successfully set
 *         - QAIRT_SYSTEM_BUILDER_ERROR_INVALID_HANDLE: Invalid builder handle
 *         - QAIRT_SYSTEM_BUILDER_ERROR_INVALID_ARGUMENT: Invalid priority value
 *
 * @note Use corresponding API through QairtSystemBuilder_V1_t
 * @note Usage of priorities apart from QAIRT_PRIORITY_LOW may be restricted on some platforms
 * @note Currently not usable, would be effective once backend extensions implemented for Qairt
 *
 * Example:
 * @code
 *   QairtSystemBuilder_setPriority(builder, QAIRT_PRIORITY_HIGH);
 * @endcode
 */
Qairt_Status_t QairtSystemBuilder_setPriority(QairtSystemBuilder_Handle_t builderHandle,
                                              Qairt_Priority_t priority);

/**
 * @brief Set list of graphs to enable in context
 *
 * Specifies which graphs from the DLC should be enabled during context creation.
 * If not set, all graphs in the DLC will be enabled by default.
 *
 * @param[in] builderHandle Handle to the builder
 * @param[in] graphNames Array of null-terminated graph name strings.
 *                       Each string should match a graph name in the DLC.
 * @param[in] numGraphs Number of graph names in the array. Must be > 0.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Enabled graphs were successfully set
 *         - QAIRT_SYSTEM_BUILDER_ERROR_INVALID_HANDLE: Invalid builder handle
 *         - QAIRT_SYSTEM_BUILDER_ERROR_INVALID_ARGUMENT: graphNames is NULL or numGraphs is 0
 *
 * @note Use corresponding API through QairtSystemBuilder_V1_t
 * @note Graph names must exactly match those in the DLC
 * @note If a specified graph name doesn't exist in the DLC, context creation may fail
 * @note Currently not usable, would be effective once backend extensions implemented for Qairt
 *
 * Example:
 * @code
 *   const char* graphs[] = {"model1", "model2"};
 *   QairtSystemBuilder_setEnabledGraphs(builder, graphs, 2);
 * @endcode
 */
Qairt_Status_t QairtSystemBuilder_setEnabledGraphs(QairtSystemBuilder_Handle_t builderHandle,
                                                   const char* const* graphNames,
                                                   uint32_t numGraphs);

/**
 * @brief Set memory limit hint for context
 *
 * Provides a hint to the backend about the maximum memory that should be used
 * for this context. The backend may use this to optimize memory allocation.
 *
 * @param[in] builderHandle Handle to the builder
 * @param[in] memoryLimitHint Memory limit in bytes.
 *                            - 0: No limit (default)
 *                            - >0: Suggested maximum memory usage in bytes
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Memory limit hint was successfully set
 *         - QAIRT_SYSTEM_BUILDER_ERROR_INVALID_HANDLE: Invalid builder handle
 *
 * @note Use corresponding API through QairtSystemBuilder_V1_t
 * @note This is a hint; the backend may not strictly enforce this limit
 * @note Actual memory usage may exceed this value depending on model requirements
 * @note Currently not usable, would be effective once backend extensions implemented for Qairt
 *
 * Example:
 * @code
 *   // Set 1GB memory limit hint
 *   QairtSystemBuilder_setMemoryLimitHint(builder, 1073741824);
 * @endcode
 */
Qairt_Status_t QairtSystemBuilder_setMemoryLimitHint(QairtSystemBuilder_Handle_t builderHandle,
                                                     uint64_t memoryLimitHint);

/**
 * @brief Set whether binary should be persistent
 *
 * Controls whether the context binary cache should persist across application
 * restarts. When enabled, the binary cache may be saved to persistent storage
 * for faster subsequent loads.
 *
 * @param[in] builderHandle Handle to the builder
 * @param[in] isPersistent Persistent binary flag:
 *                         - 0: Non-persistent (default) - binary cache is temporary
 *                         - 1: Persistent - binary cache may be saved to disk
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Persistent binary flag was successfully set
 *         - QAIRT_SYSTEM_BUILDER_ERROR_INVALID_HANDLE: Invalid builder handle
 *
 * @note Use corresponding API through QairtSystemBuilder_V1_t
 * @note Backend support for persistent binaries varies by platform
 * @note Persistent binaries may improve subsequent load times
 * @note Currently not usable, would be effective once backend extensions implemented for Qairt
 *
 * Example:
 * @code
 *   QairtSystemBuilder_setIsPersistentBinary(builder, 1);  // Enable persistence
 * @endcode
 */
Qairt_Status_t QairtSystemBuilder_setIsPersistentBinary(QairtSystemBuilder_Handle_t builderHandle,
                                                        uint8_t isPersistent);

/**
 * @brief Set cache compatibility mode
 *
 * Controls how strictly the binary cache compatibility is checked when loading
 * a cached context binary. This affects whether a cached binary is considered
 * compatible with the current hardware/software configuration.
 *
 * @param[in] builderHandle Handle to the builder
 * @param[in] compatibilityMode Compatibility mode. Valid values:
 *                              - QAIRT_CONTEXT_BINARY_COMPATIBILITY_PERMISSIVE
 *                              - QAIRT_CONTEXT_BINARY_COMPATIBILITY_STRICT
 *                              Default: QAIRT_CONTEXT_BINARY_COMPATIBILITY_PERMISSIVE (0)
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Cache compatibility mode was successfully set
 *         - QAIRT_SYSTEM_BUILDER_ERROR_INVALID_HANDLE: Invalid builder handle
 *         - QAIRT_SYSTEM_BUILDER_ERROR_INVALID_ARGUMENT: Invalid compatibility mode
 *
 * @note Use corresponding API through QairtSystemBuilder_V1_t
 * @note STRICT mode ensures optimal performance but may reject more cached binaries
 * @note PERMISSIVE mode maximizes cache reuse but may not achieve peak performance
 * @note Currently not usable, would be effective once backend extensions implemented for Qairt
 * Example:
 * @code
 *   // Use strict mode for optimal performance
 *   QairtSystemBuilder_setCacheCompatibilityMode(builder,
 *       QAIRT_CONTEXT_BINARY_COMPATIBILITY_STRICT);
 * @endcode
 */
Qairt_Status_t QairtSystemBuilder_setCacheCompatibilityMode(
    QairtSystemBuilder_Handle_t builderHandle,
    QairtContext_BinaryCompatibilityType_t compatibilityMode);

/**
 * @brief Set maximum number of profile events to capture
 *
 * Limits the number of profiling events that will be recorded during execution.
 * This can be used to control memory usage when profiling is enabled.
 *
 * @param[in] builderHandle Handle to the builder
 * @param[in] maxEvents Maximum number of profile events to record:
 *                      - 0: No limit (default) - record all events
 *                      - >0: Maximum number of events to capture
 *
 *                      Once the limit is reached, older events may be discarded
 *                      or new events may not be recorded, depending on backend
 *                      implementation.
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Profile max events was successfully set
 *         - QAIRT_SYSTEM_BUILDER_ERROR_INVALID_HANDLE: Invalid builder handle
 *
 * @note Use corresponding API through QairtSystemBuilder_V1_t
 * @note This setting only takes effect when profiling is enabled
 * @note Backend support for this feature varies by platform
 * @note Setting a limit can help prevent excessive memory usage during profiling
 * @note Currently not usable, would be effective once backend extensions implemented for Qairt
 *
 * Example:
 * @code
 *   // Limit to 1000 profile events
 *   QairtSystemBuilder_setProfileMaxEvents(builder, 1000);
 * @endcode
 */
Qairt_Status_t QairtSystemBuilder_setProfileMaxEvents(QairtSystemBuilder_Handle_t builderHandle,
                                                      uint64_t maxEvents);

/**
 * @brief Set input tensor dimensions for dynamic resizing
 *
 * @param[in] builderHandle Handle to the builder
 * @param[in] graphName     Name of the graph ("" for the first/only graph)
 * @param[in] inputNames    Array of input tensor names
 * @param[in] inputDims     Array of dimension arrays (one per tensor)
 * @param[in] numDimsPerInput Array of dimension counts (one per tensor)
 * @param[in] numInputs     Number of tensors in inputNames/inputDims arrays
 *
 * @return QAIRT_SUCCESS on success, error code otherwise
 */
Qairt_Status_t QairtSystemBuilder_setInputDimensions(QairtSystemBuilder_Handle_t builderHandle,
                                                     const char* graphName,
                                                     const char** inputNames,
                                                     const size_t** inputDims,
                                                     const uint32_t* numDimsPerInput,
                                                     uint32_t numInputs);

/**
 * @brief Validate the current configuration
 *
 * Checks if the current builder configuration is valid and ready for building.
 *
 * @param[in] builderHandle Handle to the builder
 * @param[out] errorMsg Buffer to receive error message if validation fails (can be NULL)
 * @param[in] errorMsgSize Size of the error message buffer
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Configuration is valid
 *         - QAIRT_SYSTEM_BUILDER_ERROR_INVALID_HANDLE: Invalid builder handle
 *         - QAIRT_SYSTEM_BUILDER_ERROR_VALIDATION_FAILED: Configuration validation failed
 *
 * @note Use corresponding API through QairtSystemBuilder_V1_t
 * @note If _errorMsg_ is NULL, validation still occurs but no error message is returned
 *
 * Example:
 * @code
 *   char errorMsg[256];
 *   Qairt_Status_t status = QairtSystemBuilder_validate(builder, errorMsg, sizeof(errorMsg));
 *   if (status != QAIRT_SUCCESS) {
 *     printf("Validation failed: %s\n", errorMsg);
 *   }
 * @endcode
 */
Qairt_Status_t QairtSystemBuilder_validate(QairtSystemBuilder_Handle_t builderHandle,
                                           char* errorMsg,
                                           size_t errorMsgSize);

/**
 * @brief Build a Qairt instance with the current configuration
 *
 * Creates and initializes a Qairt runtime instance based on the current
 * builder configuration. This performs full initialization including:
 * - Backend creation and configuration
 * - Operator package registration
 * - Device creation (if supported)
 * - Context creation from DLC
 * - Graph loading
 *
 * If initialization fails, the function returns an error and no Qairt
 * instance is created.
 *
 * @param[in] builderHandle Handle to the builder
 * @param[out] qairtHandle Handle to the created Qairt instance
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Qairt instance was successfully created and initialized
 *         - QAIRT_SYSTEM_BUILDER_ERROR_INVALID_HANDLE: Invalid builder handle
 *         - QAIRT_SYSTEM_BUILDER_ERROR_INVALID_ARGUMENT: _qairtHandle_ is NULL
 *         - QAIRT_SYSTEM_BUILDER_ERROR_VALIDATION_FAILED: Configuration validation failed
 *         - QAIRT_SYSTEM_BUILDER_ERROR_BUILD_FAILED: Build or initialization failed
 *
 * @note Use corresponding API through QairtSystemBuilder_V1_t
 * @note The returned Qairt instance is ready to use immediately for inference
 * @note The builder can be freed after successful build
 *
 * Example:
 * @code
 *   Qairt_Handle_t qairt;
 *   Qairt_Status_t status = QairtSystemBuilder_build(builder, &qairt);
 *   if (status == QAIRT_SUCCESS) {
 *     // Qairt is ready for inference
 *     // Can now free the builder
 *     QairtSystemBuilder_free(builder);
 *   }
 * @endcode
 */
Qairt_Status_t QairtSystemBuilder_build(QairtSystemBuilder_Handle_t builderHandle,
                                        Qairt_Handle_t* qairtHandle);

//=============================================================================
// Function Pointer Typedefs
//=============================================================================

typedef Qairt_Status_t (*QairtSystemBuilder_CreateFn_t)(QairtSystemBuilder_Handle_t*);

typedef Qairt_Status_t (*QairtSystemBuilder_FreeFn_t)(QairtSystemBuilder_Handle_t);

typedef Qairt_Status_t (*QairtSystemBuilder_SetDlcFn_t)(QairtSystemBuilder_Handle_t,
                                                        QairtSystemDlc_Handle_t);

/// Function pointer type for setDlcPtr — accepts a raw SystemDlc* (as void*)
/// to preserve its API table across the CAPI boundary. Used by the C++ wrapper.
typedef Qairt_Status_t (*QairtSystemBuilder_SetDlcPtrFn_t)(QairtSystemBuilder_Handle_t, void*);

typedef Qairt_Status_t (*QairtSystemBuilder_SetSystemApiFn_t)(QairtSystemBuilder_Handle_t,
                                                              QairtSystemApi_Handle_t);

typedef Qairt_Status_t (*QairtSystemBuilder_SetLogLevelFn_t)(QairtSystemBuilder_Handle_t,
                                                             QairtLog_Level_t);

typedef Qairt_Status_t (*QairtSystemBuilder_SetProfilingLevelFn_t)(QairtSystemBuilder_Handle_t,
                                                                   QairtProfile_Level_t);

typedef Qairt_Status_t (*QairtSystemBuilder_SetProfilingOptionFn_t)(QairtSystemBuilder_Handle_t,
                                                                    uint32_t);

typedef Qairt_Status_t (*QairtSystemBuilder_SetPerfProfileFn_t)(QairtSystemBuilder_Handle_t,
                                                                QairtSystem_PerfProfile_t);

typedef Qairt_Status_t (*QairtSystemBuilder_SetPlatformOptionsFn_t)(QairtSystemBuilder_Handle_t,
                                                                    const char*);

typedef Qairt_Status_t (*QairtSystemBuilder_SetDeviceOptionsFn_t)(QairtSystemBuilder_Handle_t,
                                                                  const char*);

typedef Qairt_Status_t (*QairtSystemBuilder_AddOpPackageFn_t)(QairtSystemBuilder_Handle_t,
                                                              const char*,
                                                              const char*,
                                                              const char*);

typedef Qairt_Status_t (*QairtSystemBuilder_SetBackendTypeFn_t)(QairtSystemBuilder_Handle_t,
                                                                 QairtSystem_BackendType_t);

typedef Qairt_Status_t (*QairtSystemBuilder_SetBackendPartitionFn_t)(QairtSystemBuilder_Handle_t,
                                                                      QairtSystem_BackendType_t,
                                                                      uint32_t,
                                                                      uint32_t,
                                                                      Qairt_Priority_t,
                                                                      QairtSystem_PerfProfile_t);

typedef Qairt_Status_t (*QairtSystemBuilder_SetBackendOrderFn_t)(QairtSystemBuilder_Handle_t,
                                                                  const QairtSystem_BackendType_t*,
                                                                  uint32_t);

typedef Qairt_Status_t (*QairtSystemBuilder_SetPriorityFn_t)(QairtSystemBuilder_Handle_t,
                                                             Qairt_Priority_t);

typedef Qairt_Status_t (*QairtSystemBuilder_SetEnabledGraphsFn_t)(QairtSystemBuilder_Handle_t,
                                                                  const char* const*,
                                                                  uint32_t);

typedef Qairt_Status_t (*QairtSystemBuilder_SetMemoryLimitHintFn_t)(QairtSystemBuilder_Handle_t,
                                                                    uint64_t);

typedef Qairt_Status_t (*QairtSystemBuilder_SetIsPersistentBinaryFn_t)(QairtSystemBuilder_Handle_t,
                                                                       uint8_t);

typedef Qairt_Status_t (*QairtSystemBuilder_SetCacheCompatibilityModeFn_t)(
    QairtSystemBuilder_Handle_t, QairtContext_BinaryCompatibilityType_t);

typedef Qairt_Status_t (*QairtSystemBuilder_SetProfileMaxEventsFn_t)(QairtSystemBuilder_Handle_t,
                                                                     uint64_t);

typedef Qairt_Status_t (*QairtSystemBuilder_SetInputDimensionsFn_t)(QairtSystemBuilder_Handle_t,
                                                                    const char*,
                                                                    const char**,
                                                                    const size_t**,
                                                                    const uint32_t*,
                                                                    uint32_t);

typedef Qairt_Status_t (*QairtSystemBuilder_ValidateFn_t)(QairtSystemBuilder_Handle_t,
                                                          char*,
                                                          size_t);

typedef Qairt_Status_t (*QairtSystemBuilder_BuildFn_t)(QairtSystemBuilder_Handle_t,
                                                       Qairt_Handle_t*);

//=============================================================================
// Public Interfaces
//=============================================================================

/**
 * @brief QairtSystemBuilder V1 interface struct
 *
 * This struct contains the functions that define the V1 interface for
 * QAIRT System Builder. This interface can be retrieved through the
 * id QAIRT_SYSTEM_BUILDER_V1_ID.
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;

  // Lifecycle
  QairtSystemBuilder_CreateFn_t create;
  QairtSystemBuilder_FreeFn_t free;

  // Core Configuration
  QairtSystemBuilder_SetDlcFn_t setDlc;        ///< Set the DLC for the builder
  QairtSystemBuilder_SetDlcPtrFn_t setDlcPtr;  // Deprecated
  QairtSystemBuilder_SetSystemApiFn_t setSystemApi;

  // Logging & Profiling
  QairtSystemBuilder_SetLogLevelFn_t setLogLevel;
  QairtSystemBuilder_SetProfilingLevelFn_t setProfilingLevel;
  QairtSystemBuilder_SetProfilingOptionFn_t setProfilingOption;
  QairtSystemBuilder_SetPerfProfileFn_t setPerfProfile;

  // Platform & Device
  QairtSystemBuilder_SetPlatformOptionsFn_t setPlatformOptions;
  QairtSystemBuilder_SetDeviceOptionsFn_t setDeviceOptions;
  QairtSystemBuilder_AddOpPackageFn_t addOpPackage;
  QairtSystemBuilder_SetBackendTypeFn_t setBackendType;

  // Runtime Configuration (Backend extensions setters)
  QairtSystemBuilder_SetPriorityFn_t setPriority;
  QairtSystemBuilder_SetEnabledGraphsFn_t setEnabledGraphs;
  QairtSystemBuilder_SetMemoryLimitHintFn_t setMemoryLimitHint;
  QairtSystemBuilder_SetIsPersistentBinaryFn_t setIsPersistentBinary;
  QairtSystemBuilder_SetCacheCompatibilityModeFn_t setCacheCompatibilityMode;
  QairtSystemBuilder_SetProfileMaxEventsFn_t setProfileMaxEvents;
  QairtSystemBuilder_SetInputDimensionsFn_t setInputDimensions;

  // Validation & Build
  QairtSystemBuilder_ValidateFn_t validate;
  QairtSystemBuilder_BuildFn_t build;

  //Multi Backend
  QairtSystemBuilder_SetBackendOrderFn_t     setBackendOrder;
  QairtSystemBuilder_SetBackendPartitionFn_t setBackendPartition;
} QairtSystemBuilder_V1_t;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // QAISW_QAIRTSYSTEMBUILDER_H

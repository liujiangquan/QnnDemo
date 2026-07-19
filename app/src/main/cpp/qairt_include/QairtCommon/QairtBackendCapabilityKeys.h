//==============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//==============================================================================

#ifndef QAISW_QAIRTBACKENDCAPABILITYKEYS_H
#define QAISW_QAIRTBACKENDCAPABILITYKEYS_H

#ifdef __cplusplus
#include <climits>
#include <cstddef>
#include <cstdint>
extern "C" {
#else
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#endif

///
/// Definition of QAIRT_CAPABILITY_GROUP_CORE CAPABILITY group.
///

/**
 * @brief CAPABILITY group for the QAIRT core CAPABILITY group.
 */
#define QAIRT_CAPABILITY_GROUP_CORE 0x00000001

///
/// Definition of QAIRT_CAPABILITY_GROUP_BACKEND CAPABILITY group. This group is
/// Core (non-optional) API.
///

/**
 * @brief CAPABILITY group for the QAIRT Backend API CAPABILITY group. This is a
 *        non-optional API component and cannot be used as a
 *        CAPABILITY key.
 */
#define QAIRT_CAPABILITY_GROUP_BACKEND (QAIRT_CAPABILITY_GROUP_CORE + 100)

/**
 * @brief CAPABILITY key for determining if a backend supports
 *        QAIRTBackend_registerOpPackage.
 */
#define QAIRT_CAPABILITY_BACKEND_SUPPORT_OP_PACKAGE (QAIRT_CAPABILITY_GROUP_BACKEND + 4)

/**
 * @brief CAPABILITY key for determining whether a backend supports the
 *        QAIRT_BACKEND_CONFIG_OPTION_PLATFORM configuration.
 */
#define QAIRT_CAPABILITY_BACKEND_SUPPORT_PLATFORM_OPTIONS (QAIRT_CAPABILITY_GROUP_BACKEND + 5)

/**
 * @brief CAPABILITY key for determining whether a backend supports graph
 *        composition. The following are considered graph composition
 *        APIs:
 *        - QAIRTContext_create
 *        - QAIRTGraph_create
 *        - QAIRTGraph_addNode
 *        - QAIRTGraph_finalize
 *        - QAIRTTensor_createContextTensor
 *        - QAIRTTensor_createGraphTensor
 *        - QAIRTBackend_validateOpConfig
 */
#define QAIRT_CAPABILITY_BACKEND_SUPPORT_COMPOSITION (QAIRT_CAPABILITY_GROUP_BACKEND + 6)

/**
 * @brief CAPABILITY key for determining whether a backend supports setting
 *        QAIRT_BACKEND_CAPABILITY_OPTION_CUSTOM as a CAPABILITY
 *        option.
 */
#define QAIRT_CAPABILITY_BACKEND_SUPPORT_CUSTOM_CAPABILITY (QAIRT_CAPABILITY_GROUP_BACKEND + 7)

///
/// Definition of QAIRT_CAPABILITY_GROUP_CONTEXT CAPABILITY group. This group is
/// Core (non-optional) API.
///

/**
 * @brief CAPABILITY group for the QAIRT Context API CAPABILITY group. This is a
 *        non-optional API component and cannot be used as a
 *        CAPABILITY key.
 */
#define QAIRT_CAPABILITY_GROUP_CONTEXT (QAIRT_CAPABILITY_GROUP_CORE + 200)

/**
 * @brief CAPABILITY key for determining whether a backend supports context
 *        binaries. It determines supports for the following APIs:
 *        - QAIRTContext_getBinarySize
 *        - QAIRTContext_getBinary
 *        - QAIRTContext_createFromBinary
 */
#define QAIRT_CAPABILITY_CONTEXT_SUPPORT_CACHING (QAIRT_CAPABILITY_GROUP_CONTEXT + 1)

/**
 * @brief CAPABILITY key for determining whether a backend supports the
 *        QAIRTContext_Config_t data structure.
 */
#define QAIRT_CAPABILITY_CONTEXT_SUPPORT_CONFIGURATION (QAIRT_CAPABILITY_GROUP_CONTEXT + 4)

/**
 * @brief CAPABILITY key for determining whether a backend supports graph
 *        enablement in a context. See
 *        QAIRT_CONTEXT_CONFIG_ENABLE_GRAPHS.
 */
#define QAIRT_CAPABILITY_CONTEXT_SUPPORT_CONFIG_ENABLE_GRAPHS (QAIRT_CAPABILITY_GROUP_CONTEXT + 5)

/**
 * @brief CAPABILITY key for determining whether a backend supports memory
 *        limits in a context. See QAIRT_CONTEXT_CONFIG_MEMORY_LIMIT.
 */
#define QAIRT_CAPABILITY_CONTEXT_SUPPORT_CONFIG_MEMORY_LIMIT_HINT \
  (QAIRT_CAPABILITY_GROUP_CONTEXT + 6)

/**
 * @brief CAPABILITY key for determining whether a backend supports context
 *        binaries that are readable throughout the lifetime of the
 *        context. See QAIRT_CONTEXT_CONFIG_PERSISTENT_BINARY.
 */
#define QAIRT_CAPABILITY_CONTEXT_SUPPORT_CONFIG_PERSISTENT_BINARY \
  (QAIRT_CAPABILITY_GROUP_CONTEXT + 7)

/**
 * @brief CAPABILITY key for determining whether a backend supports binary
 *        compatibility control in a context. See
 *        QAIRT_CONTEXT_CONFIG_BINARY_COMPATIBILITY.
 */
#define QAIRT_CAPABILITY_CONTEXT_SUPPORT_CONFIG_BINARY_COMPATIBILITY_TYPE \
  (QAIRT_CAPABILITY_GROUP_CONTEXT + 8)

/**
 * @brief CAPABILITY key for determining whether a backend supports validation
 *        of a stored binary. It determines support for
 *        QAIRTContext_validateBinary.
 */
#define QAIRT_CAPABILITY_CONTEXT_SUPPORT_VALIDATE_BINARY (QAIRT_CAPABILITY_GROUP_CONTEXT + 9)

/**
 * @brief CAPABILITY key for determining whether a backend supports creating a
 *        context from a stored binary, which supports control
 *        signals. It determines support for QAIRTContext_createFromBinaryWithSignal.
 */
#define QAIRT_CAPABILITY_CONTEXT_SUPPORT_CREATE_FROM_BINARY_WITH_SIGNALS \
  (QAIRT_CAPABILITY_GROUP_CONTEXT + 10)

/**
 * @brief CAPABILITY key for determining whether a backend supports creating
 *        multiple contexts from binaries in a single API call. It
 *        determines support for QAIRTContext_createFromBinaryListAsync.
 */
#define QAIRT_CAPABILITY_CONTEXT_SUPPORT_CREATE_FROM_BINARY_LIST_ASYNC \
  (QAIRT_CAPABILITY_GROUP_CONTEXT + 11)

/**
 * @brief CAPABILITY key for determining whether a backend supports creation and
 *        application of updates for an existing context binary. This
 *        determines support for QAIRTContext_getBinarySectionSize(),
 *        QAIRTContext_retrieveBinarySection(), and
 *        QAIRTContext_applyBinarySection() with QAIRT_CONTEXT_SECTION_UPDATABLE.
 */
#define QAIRT_CAPABILITY_CONTEXT_SUPPORT_BINARY_UPDATES (QAIRT_CAPABILITY_GROUP_CONTEXT + 12)

/**
 * @brief CAPABILITY key for determining whether a backend supports use of
 *        binary sections without the
 *        __graph__ argument provided.
 */
#define QAIRT_CAPABILITY_CONTEXT_SUPPORT_BINARY_SECTION_FULL_CONTEXT \
  (QAIRT_CAPABILITY_GROUP_CONTEXT + 13)

/**
 * @brief CAPABILITY key for determining whether a backend supports setting
 *        QAIRT_CONTEXT_CAPABILITY_OPTION_CUSTOM as a CAPABILITY
 *        option.
 */
#define QAIRT_CAPABILITY_CONTEXT_SUPPORT_CUSTOM_CAPABILITY (QAIRT_CAPABILITY_GROUP_CONTEXT + 14)

/**
 * @brief CAPABILITY key for determining whether a backend supports
 *        QAIRTContext_getIncrementalBinary and
 *        QAIRTContext_releaseIncrementalBinary.
 */
#define QAIRT_CAPABILITY_CONTEXT_SUPPORT_INCREMENTAL_BINARY (QAIRT_CAPABILITY_GROUP_CONTEXT + 15)

/**
 * @brief CAPABILITY key for determining whether a backend supports deferred
 *        graph initialization during context creation. See
 *        QAIRT_CONTEXT_CONFIG_OPTION_DEFER_GRAPH_INIT.
 */
#define QAIRT_CAPABILITY_CONTEXT_SUPPORT_DEFERRED_GRAPH_INIT (QAIRT_CAPABILITY_GROUP_CONTEXT + 16)

/**
 * @brief CAPABILITY key for determining whether a backend supports creation and
 *        application of weight only updates for an existing context
 *        binary. This determines support for QAIRTContext_getBinarySectionSize(),
 *        QAIRTContext_retrieveBinarySection(), and
 *        QAIRTContext_applyBinarySection() with
 *        QAIRT_CONTEXT_SECTION_UPDATABLE_WEIGHTS.
 */
#define QAIRT_CAPABILITY_CONTEXT_SUPPORT_BINARY_WEIGHT_ONLY_UPDATES \
  (QAIRT_CAPABILITY_GROUP_CONTEXT + 17)

/**
 * @brief CAPABILITY key for determining whether a backend supports creation and
 *        application of quant param only updates for an existing
 *        context binary. This determines support for
 *        QAIRTContext_getBinarySectionSize(), QAIRTContext_retrieveBinarySection(),
 *        and QAIRTContext_applyBinarySection() with
 *        QAIRT_CONTEXT_SECTION_UPDATABLE_QUANT_PARAMS.
 */
#define QAIRT_CAPABILITY_CONTEXT_SUPPORT_BINARY_QUANT_ONLY_UPDATES \
  (QAIRT_CAPABILITY_GROUP_CONTEXT + 18)

/**
 * @brief CAPABILITY key for determining whether a backend supports creating
 *        contexts with a client callback function. It determines
 *        support for QAIRTContext_createFromBinaryWithCallback.
 */
#define QAIRT_CAPABILITY_CONTEXT_SUPPORT_CREATE_FROM_BINARY_WITH_CALLBACK \
  (QAIRT_CAPABILITY_GROUP_CONTEXT + 19)

/**
 * @brief CAPABILITY key for determining whether a backend supports multi
 *        switching. See QAIRT_CONTEXT_CONFIG_GRAPH_RETENTION_ORDER
 */
#define QAIRT_CAPABILITY_CONTEXT_SUPPORT_GRAPH_RETENTION_ORDER (QAIRT_CAPABILITY_GROUP_CONTEXT + 20)

///
/// Definition of QAIRT_CAPABILITY_GROUP_GRAPH CAPABILITY group. This group is
/// Core (non-optional) API.
///

/**
 * @brief CAPABILITY group for the QAIRT Graph API CAPABILITY group. This is a
 *        non-optional API component and cannot be used as a
 *        CAPABILITY key.
 */
#define QAIRT_CAPABILITY_GROUP_GRAPH (QAIRT_CAPABILITY_GROUP_CORE + 300)

/**
 * @brief CAPABILITY key for determining whether a backend supports graph
 *        configuration. It determines support for
 *        QAIRTGraph_setConfig.
 */
#define QAIRT_CAPABILITY_GRAPH_SUPPORT_CONFIG (QAIRT_CAPABILITY_GROUP_GRAPH + 1)

/**
 * @brief CAPABILITY key for determining whether a backend supports signals.
 * @note This capability is equivalent to all of
 *       QAIRT_CAPABILITY_GRAPH_SUPPORT_FINALIZE_SIGNAL,
 *       QAIRT_CAPABILITY_GRAPH_SUPPORT_EXECUTE_SIGNAL, and
 *       QAIRT_CAPABILITY_GRAPH_SUPPORT_EXECUTE_ASYNC_SIGNAL having
 *       support.
 * @note DEPRECATED: Use QAIRT_CAPABILITY_GRAPH_SUPPORT_FINALIZE_SIGNAL,
 *       QAIRT_CAPABILITY_GRAPH_SUPPORT_EXECUTE_SIGNAL, or
 *       QAIRT_CAPABILITY_GRAPH_SUPPORT_EXECUTE_ASYNC_SIGNAL for
 *       QAIRTGraph API support for QAIRTSignal.
 */
#define QAIRT_CAPABILITY_GRAPH_SUPPORT_SIGNALS (QAIRT_CAPABILITY_GROUP_GRAPH + 2)

/**
 * @brief CAPABILITY key for determining whether a backend supports asynchronous
 *        graph execution. It determines support for
 *        QAIRTGraph_executeAsync.
 */
#define QAIRT_CAPABILITY_GRAPH_SUPPORT_ASYNC_EXECUTION (QAIRT_CAPABILITY_GROUP_GRAPH + 3)

/**
 * @brief CAPABILITY key for determining whether a backend supports execution of
 *        graphs with null inputs. This implies that the graph will
 *        contain no APP_WRITE tensors.
 */
#define QAIRT_CAPABILITY_GRAPH_SUPPORT_NULL_INPUTS (QAIRT_CAPABILITY_GROUP_GRAPH + 4)

/**
 * @brief CAPABILITY key for determining whether a backend supports priority
 *        control of graphs within a context. See
 *        QAIRT_GRAPH_CONFIG_OPTION_PRIORITY.
 */
#define QAIRT_CAPABILITY_GRAPH_SUPPORT_PRIORITY_CONTROL (QAIRT_CAPABILITY_GROUP_GRAPH + 5)

/**
 * @brief CAPABILITY key for determining whether a backend supports QAIRTSignal
 *        for QAIRTGraph_finalize.
 */
#define QAIRT_CAPABILITY_GRAPH_SUPPORT_FINALIZE_SIGNAL (QAIRT_CAPABILITY_GROUP_GRAPH + 6)

/**
 * @brief CAPABILITY key for determining whether a backend supports QAIRTSignal
 *        for QAIRTGraph_execute.
 */
#define QAIRT_CAPABILITY_GRAPH_SUPPORT_EXECUTE_SIGNAL (QAIRT_CAPABILITY_GROUP_GRAPH + 7)

/**
 * @brief CAPABILITY key for determining whether a backend supports QAIRTSignal
 *        for QAIRTGraph_executeAsync.
 */
#define QAIRT_CAPABILITY_GRAPH_SUPPORT_EXECUTE_ASYNC_SIGNAL (QAIRT_CAPABILITY_GROUP_GRAPH + 8)

/**
 * @brief CAPABILITY key for determining whether a backend supports graph-level
 *        continuous profiling. See
 *        QAIRT_GRAPH_CONFIG_OPTION_PROFILE_HANDLE.
 */
#define QAIRT_CAPABILITY_GRAPH_SUPPORT_CONTINUOUS_PROFILING (QAIRT_CAPABILITY_GROUP_GRAPH + 9)

/**
 * @brief CAPABILITY key for determining whether a backend supports graph
 *        execution. It determines support for QAIRTGraph_execute.
 */
#define QAIRT_CAPABILITY_GRAPH_SUPPORT_EXECUTE (QAIRT_CAPABILITY_GROUP_GRAPH + 10)

/**
 * @brief CAPABILITY key for determining whether a backend supports batch
 *        multiplier.
 */
#define QAIRT_CAPABILITY_GRAPH_SUPPORT_BATCH_MULTIPLE (QAIRT_CAPABILITY_GROUP_GRAPH + 11)

/**
 * @brief CAPABILITY key for determining whether a backend supports per-API
 *        profiling data for graph execution.
 */
#define QAIRT_CAPABILITY_GRAPH_SUPPORT_EXECUTE_PER_API_PROFILING (QAIRT_CAPABILITY_GROUP_GRAPH + 12)

/**
 * @brief CAPABILITY key for determining whether a backend supports subgraphs.
 *        It determines support for QAIRTGraph_createSubgraph.
 */
#define QAIRT_CAPABILITY_GRAPH_SUPPORT_SUBGRAPH (QAIRT_CAPABILITY_GROUP_GRAPH + 13)

/**
 * @brief CAPABILITY key for determining whether a backend supports graph
 *        profiling state. See
 *        QAIRT_GRAPH_CONFIG_OPTION_SET_PROFILING_STATE.
 */
#define QAIRT_CAPABILITY_GRAPH_SUPPORT_PROFILING_STATE (QAIRT_CAPABILITY_GROUP_GRAPH + 14)

/**
 * @brief CAPABILITY key for determining whether a backend supports controlling
 *        the number of profiling executions of a graph. See
 *        QAIRT_GRAPH_CONFIG_OPTION_SET_PROFILING_NUM_EXECUTIONS.
 */
#define QAIRT_CAPABILITY_GRAPH_SUPPORT_SET_PROFILING_NUM_EXECUTIONS \
  (QAIRT_CAPABILITY_GROUP_GRAPH + 15)

/**
 * @brief CAPABILITY key for determining whether a backend supports the
 *        QAIRT_GRAPH_EXECUTE_ENVIRONMENT_OPTION_BIND_MEM_HANDLES
 *        execution environment option for binding client allocated mem handles to a
 *        graph.
 */
#define QAIRT_CAPABILITY_GRAPH_SUPPORT_ENV_OPTION_BIND_MEM_HANDLES \
  (QAIRT_CAPABILITY_GROUP_GRAPH + 16)

/**
 * @brief CAPABILITY key for determining whether a backend supports the
 *        QAIRT_GRAPH_EXECUTE_ENVIRONMENT_OPTION_POPULATE_CLIENT_BUFS
 *        execution environment option for populating client buffers with backend
 *        allocated memory.
 */
#define QAIRT_CAPABILITY_GRAPH_SUPPORT_ENV_OPTION_POPULATE_CLIENT_BUFS \
  (QAIRT_CAPABILITY_GROUP_GRAPH + 17)

/**
 * @brief CAPABILITY key for determining whether a backend supports finalizing
 *        (QAIRTGraph_finalize) a graph retrieved from a context
 *        binary.
 */
#define QAIRT_CAPABILITY_GRAPH_SUPPORT_FINALIZE_DESERIALIZED_GRAPH \
  (QAIRT_CAPABILITY_GROUP_GRAPH + 18)

/**
 * @brief CAPABILITY key for determining whether a backend supports setting
 *        QAIRT_GRAPH_CAPABILITY_OPTION_CUSTOM as a CAPABILITY
 *        option.
 */
#define QAIRT_CAPABILITY_GRAPH_SUPPORT_CUSTOM_CAPABILITY (QAIRT_CAPABILITY_GROUP_GRAPH + 19)

/**
 * @brief CAPABILITY key for determining whether a backend supports early
 *        termination of graph execution.
 */
#define QAIRT_CAPABILITY_GRAPH_SUPPORT_EARLY_TERMINATION (QAIRT_CAPABILITY_GROUP_GRAPH + 20)

/**
 * @brief CAPABILITY key for determining whether a backend supports online
 *        preparation of graphs.
 */
#define QAIRT_CAPABILITY_GRAPH_SUPPORT_ONLINE_PREPARE (QAIRT_CAPABILITY_GROUP_GRAPH + 21)

///
/// Definition of QAIRT_CAPABILITY_GROUP_OP_PACKAGE CAPABILITY group. This group
/// is Optional portion of API.
///

/**
 * @brief CAPABILITY group for the QAIRT Op Package API CAPABILITY group. This
 *        can be used as a key to check if Op Package API is
 *        supported by a backend.
 */
#define QAIRT_CAPABILITY_GROUP_OP_PACKAGE (QAIRT_CAPABILITY_GROUP_CORE + 400)

/**
 * @brief CAPABILITY key for determining whether an op package supports
 *        validation.
 */
#define QAIRT_CAPABILITY_OP_PACKAGE_SUPPORTS_VALIDATION (QAIRT_CAPABILITY_GROUP_OP_PACKAGE + 1)

/**
 * @brief CAPABILITY key for determining whether an op package supports op
 *        implementation creation and freeing.
 */
#define QAIRT_CAPABILITY_OP_PACKAGE_SUPPORTS_OP_IMPLS (QAIRT_CAPABILITY_GROUP_OP_PACKAGE + 2)

/**
 * @brief CAPABILITY key for determining whether an op package supports
 *        duplication of operation names, such that there are
 *        duplicated op_package_name::op_name combinations.
 */
#define QAIRT_CAPABILITY_OP_PACKAGE_SUPPORTS_DUPLICATE_NAMES (QAIRT_CAPABILITY_GROUP_OP_PACKAGE + 3)

///
/// Definition of QAIRT_CAPABILITY_GROUP_TENSOR CAPABILITY group. This group is
/// Core (non-optional) API.
///

/**
 * @brief CAPABILITY group for the QAIRT Tensor API CAPABILITY group. This is a
 *        non-optional API component and cannot be used as a
 *        CAPABILITY key.
 */
#define QAIRT_CAPABILITY_GROUP_TENSOR (QAIRT_CAPABILITY_GROUP_CORE + 500)

/**
 * @brief CAPABILITY key to determine whether a backend supports
 *        QAIRT_MemHandle_t type tensors.
 */
#define QAIRT_CAPABILITY_TENSOR_SUPPORT_MEMHANDLE_TYPE (QAIRT_CAPABILITY_GROUP_TENSOR + 1)

/**
 * @brief CAPABILITY key to determine whether a backend supports creating
 *        context tensors. It determines support for
 *        QAIRTTensor_createContextTensor.
 */
#define QAIRT_CAPABILITY_TENSOR_SUPPORT_CONTEXT_TENSORS (QAIRT_CAPABILITY_GROUP_TENSOR + 2)

/**
 * @brief CAPABILITY key to determine whether a backend supports dynamic tensor
 *        dimensions.
 */
#define QAIRT_CAPABILITY_TENSOR_SUPPORT_DYNAMIC_DIMENSIONS (QAIRT_CAPABILITY_GROUP_TENSOR + 3)

/**
 * @brief CAPABILITY key to determine whether a backend supports tensor
 *        sparsity.
 */
#define QAIRT_CAPABILITY_TENSOR_SUPPORT_SPARSITY (QAIRT_CAPABILITY_GROUP_TENSOR + 4)

/**
 * @brief CAPABILITY key to determine whether a backend supports updating static
 *        tensor weight data and quantization encodings, if
 *        applicable.
 */
#define QAIRT_CAPABILITY_TENSOR_SUPPORT_UPDATEABLE_STATIC_TENSORS \
  (QAIRT_CAPABILITY_GROUP_TENSOR + 5)

/**
 * @brief CAPABILITY key to determine whether a backend supports updating
 *        quantization tensor encodings for UPDATABLE_NATIVE tensors.
 */
#define QAIRT_CAPABILITY_TENSOR_SUPPORT_UPDATEABLE_NATIVE_TENSORS \
  (QAIRT_CAPABILITY_GROUP_TENSOR + 6)

/**
 * @brief CAPABILITY key to determine whether a backend supports updating
 *        quantization tensor encodings for UPDATABLE_APP_READ,
 *        UPDATABLE_APP_WRITE, and UPDATABLE_APP_READWRITE tensors.
 */
#define QAIRT_CAPABILITY_TENSOR_SUPPORT_UPDATEABLE_APP_TENSORS (QAIRT_CAPABILITY_GROUP_TENSOR + 7)

/**
 * @brief CAPABILITY key to determine whether a backend supports scale-offset
 *        quantization encodings. See
 *        QAIRT_QUANTIZATION_ENCODING_SCALE_OFFSET.
 */
#define QAIRT_CAPABILITY_TENSOR_SUPPORT_QUANTIZATION_ENCODING_SCALE_OFFSET \
  (QAIRT_CAPABILITY_GROUP_TENSOR + 8)

/**
 * @brief CAPABILITY key to determine whether a backend supports axis
 *        scale-offset quantization encodings. See
 *        QAIRT_QUANTIZATION_ENCODING_AXIS_SCALE_OFFSET.
 */
#define QAIRT_CAPABILITY_TENSOR_SUPPORT_QUANTIZATION_ENCODING_AXIS_SCALE_OFFSET \
  (QAIRT_CAPABILITY_GROUP_TENSOR + 9)

/**
 * @brief CAPABILITY key to determine whether a backend supports bit-width
 *        scale-offset quantization encodings. See
 *        QAIRT_QUANTIZATION_ENCODING_BW_SCALE_OFFSET.
 */
#define QAIRT_CAPABILITY_TENSOR_SUPPORT_QUANTIZATION_ENCODING_BW_SCALE_OFFSET \
  (QAIRT_CAPABILITY_GROUP_TENSOR + 10)

/**
 * @brief CAPABILITY key to determine whether a backend supports bit-width axis
 *        scale-offset quantization encodings. See
 *        QAIRT_QUANTIZATION_ENCODING_BW_AXIS_SCALE_OFFSET.
 */
#define QAIRT_CAPABILITY_TENSOR_SUPPORT_QUANTIZATION_ENCODING_BW_AXIS_SCALE_OFFSET \
  (QAIRT_CAPABILITY_GROUP_TENSOR + 11)

/**
 * @brief CAPABILITY key to determine whether a backend supports block
 *        quantization encodings. See
 *        QAIRT_QUANTIZATION_ENCODING_BLOCK.
 */
#define QAIRT_CAPABILITY_TENSOR_SUPPORT_QUANTIZATION_ENCODING_BLOCK \
  (QAIRT_CAPABILITY_GROUP_TENSOR + 12)

/**
 * @brief CAPABILITY key to determine whether a backend supports blockwise
 *        expansion quantization encodings. See
 *        QAIRT_QUANTIZATION_ENCODING_BLOCKWISE_EXPANSION.
 */
#define QAIRT_CAPABILITY_TENSOR_SUPPORT_QUANTIZATION_ENCODING_BLOCKWISE_EXPANSION \
  (QAIRT_CAPABILITY_GROUP_TENSOR + 13)

/**
 * @brief CAPABILITY key to determine whether a backend supports vector
 *        quantization encodings. See
 *        QAIRT_QUANTIZATION_ENCODING_VECTOR.
 */
#define QAIRT_CAPABILITY_TENSOR_SUPPORT_QUANTIZATION_ENCODING_VECTOR \
  (QAIRT_CAPABILITY_GROUP_TENSOR + 14)

/**
 * @brief CAPABILITY key to determine whether a backend supports deferred
 *        loading of raw tensor data through a callback. See
 *        QAIRT_TENSORMEMTYPE_RETRIEVE_RAW.
 */
#define QAIRT_CAPABILITY_TENSOR_SUPPORT_RETRIEVE_RAW (QAIRT_CAPABILITY_GROUP_TENSOR + 15)

/**
 * @brief CAPABILITY key for determining whether a backend supports optional
 *        application writable tensors.
 */
#define QAIRT_CAPABILITY_TENSOR_SUPPORT_OPTIONAL_APP_WRITE (QAIRT_CAPABILITY_GROUP_TENSOR + 16)
/**
 * @brief CAPABILITY key for determining whether a backend supports optional
 *        application readable tensors.
 */
#define QAIRT_CAPABILITY_TENSOR_SUPPORT_OPTIONAL_APP_READ (QAIRT_CAPABILITY_GROUP_TENSOR + 17)
/**
 * @brief CAPABILITY key for determining whether a backend supports optional
 *        application readable/writable tensors.
 */
#define QAIRT_CAPABILITY_TENSOR_SUPPORT_OPTIONAL_APP_READWRITE (QAIRT_CAPABILITY_GROUP_TENSOR + 18)

/**
 * @brief CAPABILITY key for determining whether a backend supports MX data
 *        format
 */
#define QAIRT_CAPABILITY_TENSOR_SUPPORT_MX_DATA_FORMAT (QAIRT_CAPABILITY_GROUP_TENSOR + 19)

/**
 *  @brief CAPABILITY key for determining whether a backend supports
 *         QAIRT_TENSOR_DATA_FORMAT_UBWC_RGBA8888 data format.
 */
#define QAIRT_CAPABILITY_TENSOR_SUPPORT_UBWC_RGBA8888 (QAIRT_CAPABILITY_GROUP_TENSOR + 20)

/**
 * @brief CAPABILITY key for determining whether a backend supports
 *        QAIRT_TENSOR_DATA_FORMAT_UBWC_NV12 data format.
 */
#define QAIRT_CAPABILITY_TENSOR_SUPPORT_UBWC_NV12 (QAIRT_CAPABILITY_GROUP_TENSOR + 21)

/**
 * @brief CAPABILITY key for determining whether a backend supports
 *        QAIRT_TENSOR_DATA_FORMAT_UBWC_NV12_Y data format.
 */
#define QAIRT_CAPABILITY_TENSOR_SUPPORT_UBWC_NV12_Y (QAIRT_CAPABILITY_GROUP_TENSOR + 22)

/**
 * @brief CAPABILITY key for determining whether a backend supports
 *        QAIRT_TENSOR_DATA_FORMAT_UBWC_NV12_UV data format.
 */
#define QAIRT_CAPABILITY_TENSOR_SUPPORT_UBWC_NV12_UV (QAIRT_CAPABILITY_GROUP_TENSOR + 23)

/**
 * @brief CAPABILITY key for determining whether a backend supports
 *        QAIRT_TENSOR_DATA_FORMAT_UBWC_NV124R data format.
 */
#define QAIRT_CAPABILITY_TENSOR_SUPPORT_UBWC_NV124R (QAIRT_CAPABILITY_GROUP_TENSOR + 24)

/**
 * @brief CAPABILITY key for determining whether a backend supports
 *        QAIRT_TENSOR_DATA_FORMAT_UBWC_NV124R_Y data format.
 */
#define QAIRT_CAPABILITY_TENSOR_SUPPORT_UBWC_NV124R_Y (QAIRT_CAPABILITY_GROUP_TENSOR + 25)

/**
 * @brief CAPABILITY key for determining whether a backend supports
 *        QAIRT_TENSOR_DATA_FORMAT_UBWC_NV124R_UV data format.
 */
#define QAIRT_CAPABILITY_TENSOR_SUPPORT_UBWC_NV124R_UV (QAIRT_CAPABILITY_GROUP_TENSOR + 26)

/**
 * @brief CAPABILITY key to determine whether a backend supports float block quantization encodings.
 *        See QAIRT_QUANTIZATION_ENCODING_FLOAT_BLOCK.
 */
#define QAIRT_CAPABILITY_TENSOR_SUPPORT_QUANTIZATION_ENCODING_FLOAT_BLOCK \
  (QAIRT_CAPABILITY_GROUP_TENSOR + 27)

/**
 * @brief CAPABILITY key to determine whether a backend supports bit-width axis scale-offset
 *        mapped quantization encodings. See
 *        QAIRT_QUANTIZATION_ENCODING_BW_AXIS_SCALE_OFFSET_MAPPED.
 */
#define QAIRT_CAPABILITY_TENSOR_SUPPORT_QUANTIZATION_ENCODING_BW_AXIS_SCALE_OFFSET_MAPPED \
  (QAIRT_CAPABILITY_GROUP_TENSOR + 28)

/**
 * @brief CAPABILITY key to determine whether a backend supports bit-width block scale-offset
 *        mapped quantization encodings. See
 *        QAIRT_QUANTIZATION_ENCODING_BW_BLOCK_MAPPED.
 */
#define QAIRT_CAPABILITY_TENSOR_SUPPORT_QUANTIZATION_ENCODING_BW_BLOCK_SCALE_OFFSET_MAPPED \
  (QAIRT_CAPABILITY_GROUP_TENSOR + 29)

/**
 * @brief CAPABILITY key to determine whether a backend supports bit-width blockwise expansion
 *        mapped quantization encodings. See
 *        QAIRT_QUANTIZATION_ENCODING_BW_BLOCKWISE_EXPANSION_MAPPED.
 */
#define QAIRT_CAPABILITY_TENSOR_SUPPORT_QUANTIZATION_ENCODING_BW_BLOCKWISE_EXPANSION_MAPPED \
  (QAIRT_CAPABILITY_GROUP_TENSOR + 30)

/**
 * @brief CAPABILITY key to determine whether a backend supports bit-width float block
 *        quantization encodings. See QAIRT_QUANTIZATION_ENCODING_BW_FLOAT_BLOCK.
 */
#define QAIRT_CAPABILITY_TENSOR_SUPPORT_QUANTIZATION_ENCODING_BW_FLOAT_BLOCK \
  (QAIRT_CAPABILITY_GROUP_TENSOR + 31)

/**
 * @brief CAPABILITY key to determine whether a backend supports Microscaling quantization
 *        encodings. See QAIRT_QUANTIZATION_ENCODING_MICROSCALING.
 */
#define QAIRT_CAPABILITY_TENSOR_SUPPORT_QUANTIZATION_ENCODING_MICROSCALING \
  (QAIRT_CAPABILITY_GROUP_TENSOR + 32)

/**
 * @brief CAPABILITY key for determining whether a backend supports
 *        QAIRT_TENSOR_DATA_FORMAT_UBWC_P010 data format.
 */
#define QAIRT_CAPABILITY_TENSOR_SUPPORT_UBWC_P010 (QAIRT_CAPABILITY_GROUP_TENSOR + 33)

/**
 * @brief CAPABILITY key for determining whether a backend supports
 *        QAIRT_TENSOR_DATA_FORMAT_UBWC_P010_Y data format.
 */
#define QAIRT_CAPABILITY_TENSOR_SUPPORT_UBWC_P010_Y (QAIRT_CAPABILITY_GROUP_TENSOR + 34)

/**
 * @brief CAPABILITY key for determining whether a backend supports
 *        QAIRT_TENSOR_DATA_FORMAT_UBWC_P010_UV data format.
 */
#define QAIRT_CAPABILITY_TENSOR_SUPPORT_UBWC_P010_UV (QAIRT_CAPABILITY_GROUP_TENSOR + 35)

/**
 * @brief CAPABILITY key for determining whether a backend supports
 *        QAIRT_TENSOR_DATA_FORMAT_UBWC_P016 data format.
 */
#define QAIRT_CAPABILITY_TENSOR_SUPPORT_UBWC_P016 (QAIRT_CAPABILITY_GROUP_TENSOR + 36)

/**
 * @brief CAPABILITY key for determining whether a backend supports
 *        QAIRT_TENSOR_DATA_FORMAT_UBWC_P016_Y data format.
 */
#define QAIRT_CAPABILITY_TENSOR_SUPPORT_UBWC_P016_Y (QAIRT_CAPABILITY_GROUP_TENSOR + 37)

/**
 * @brief CAPABILITY key for determining whether a backend supports
 *        QAIRT_TENSOR_DATA_FORMAT_UBWC_P016_UV data format.
 */
#define QAIRT_CAPABILITY_TENSOR_SUPPORT_UBWC_P016_UV (QAIRT_CAPABILITY_GROUP_TENSOR + 38)

/**
 * @brief CAPABILITY key for determining whether a backend supports
 *        QAIRT_TENSOR_DATA_FORMAT_UBWC_TP10 data format.
 */
#define QAIRT_CAPABILITY_TENSOR_SUPPORT_UBWC_TP10 (QAIRT_CAPABILITY_GROUP_TENSOR + 39)

/**
 * @brief CAPABILITY key for determining whether a backend supports
 *        QAIRT_TENSOR_DATA_FORMAT_UBWC_TP10_Y data format.
 */
#define QAIRT_CAPABILITY_TENSOR_SUPPORT_UBWC_TP10_Y (QAIRT_CAPABILITY_GROUP_TENSOR + 40)

/**
 * @brief CAPABILITY key for determining whether a backend supports
 *        QAIRT_TENSOR_DATA_FORMAT_UBWC_TP10_UV data format.
 */
#define QAIRT_CAPABILITY_TENSOR_SUPPORT_UBWC_TP10_UV (QAIRT_CAPABILITY_GROUP_TENSOR + 41)

/**
 * @brief CAPABILITY key for determining whether a backend supports
 *        QAIRT_TENSOR_DATA_FORMAT_UBWC_TBAYER10_1X1 data format.
 */
#define QAIRT_CAPABILITY_TENSOR_SUPPORT_UBWC_TBAYER10_1X1 (QAIRT_CAPABILITY_GROUP_TENSOR + 42)

/**
 * @brief CAPABILITY key for determining whether a backend supports
 *        QAIRT_TENSOR_DATA_FORMAT_UBWC_TBAYER10_2X2 data format.
 */
#define QAIRT_CAPABILITY_TENSOR_SUPPORT_UBWC_TBAYER10_2X2 (QAIRT_CAPABILITY_GROUP_TENSOR + 43)

/**
 * @brief CAPABILITY key for determining whether a backend supports
 *        QAIRT_TENSOR_DATA_FORMAT_UBWC_TBAYER10_3X3 data format.
 */
#define QAIRT_CAPABILITY_TENSOR_SUPPORT_UBWC_TBAYER10_3X3 (QAIRT_CAPABILITY_GROUP_TENSOR + 44)

/**
 * @brief CAPABILITY key for determining whether a backend supports
 *        QAIRT_TENSOR_DATA_FORMAT_UBWC_TBAYER10_4X4 data format.
 */
#define QAIRT_CAPABILITY_TENSOR_SUPPORT_UBWC_TBAYER10_4X4 (QAIRT_CAPABILITY_GROUP_TENSOR + 45)

/**
 * @brief CAPABILITY key for determining whether a backend supports
 *        QAIRT_TENSOR_DATA_FORMAT_UBWC_BAYER16_10BPS_1X1 data format.
 */
#define QAIRT_CAPABILITY_TENSOR_SUPPORT_UBWC_BAYER16_10BPS_1X1 (QAIRT_CAPABILITY_GROUP_TENSOR + 46)

/**
 * @brief CAPABILITY key for determining whether a backend supports
 *        QAIRT_TENSOR_DATA_FORMAT_UBWC_BAYER16_10BPS_2X2 data format.
 */
#define QAIRT_CAPABILITY_TENSOR_SUPPORT_UBWC_BAYER16_10BPS_2X2 (QAIRT_CAPABILITY_GROUP_TENSOR + 47)

/**
 * @brief CAPABILITY key for determining whether a backend supports
 *        QAIRT_TENSOR_DATA_FORMAT_UBWC_BAYER16_10BPS_4X4 data format.
 */
#define QAIRT_CAPABILITY_TENSOR_SUPPORT_UBWC_BAYER16_10BPS_4X4 (QAIRT_CAPABILITY_GROUP_TENSOR + 48)

/**
 * @brief CAPABILITY key for determining whether a backend supports
 *        QAIRT_TENSOR_DATA_FORMAT_UBWC_BAYER16_12BPS_1X1 data format.
 */
#define QAIRT_CAPABILITY_TENSOR_SUPPORT_UBWC_BAYER16_12BPS_1X1 (QAIRT_CAPABILITY_GROUP_TENSOR + 49)

/**
 * @brief CAPABILITY key for determining whether a backend supports
 *        QAIRT_TENSOR_DATA_FORMAT_UBWC_BAYER16_12BPS_2X2 data format.
 */
#define QAIRT_CAPABILITY_TENSOR_SUPPORT_UBWC_BAYER16_12BPS_2X2 (QAIRT_CAPABILITY_GROUP_TENSOR + 50)

/**
 * @brief CAPABILITY key for determining whether a backend supports
 *        QAIRT_TENSOR_DATA_FORMAT_UBWC_BAYER16_12BPS_4X4 data format.
 */
#define QAIRT_CAPABILITY_TENSOR_SUPPORT_UBWC_BAYER16_12BPS_4X4 (QAIRT_CAPABILITY_GROUP_TENSOR + 51)

/**
 * @brief CAPABILITY key for determining whether a backend supports
 *        QAIRT_TENSOR_DATA_FORMAT_UBWC_BAYER16_14BPS_1X1 data format.
 */
#define QAIRT_CAPABILITY_TENSOR_SUPPORT_UBWC_BAYER16_14BPS_1X1 (QAIRT_CAPABILITY_GROUP_TENSOR + 52)

/**
 * @brief CAPABILITY key for determining whether a backend supports
 *        QAIRT_TENSOR_DATA_FORMAT_UBWC_BAYER16_14BPS_2X2 data format.
 */
#define QAIRT_CAPABILITY_TENSOR_SUPPORT_UBWC_BAYER16_14BPS_2X2 (QAIRT_CAPABILITY_GROUP_TENSOR + 53)

/**
 * @brief CAPABILITY key for determining whether a backend supports
 *        QAIRT_TENSOR_DATA_FORMAT_UBWC_BAYER16_14BPS_4X4 data format.
 */
#define QAIRT_CAPABILITY_TENSOR_SUPPORT_UBWC_BAYER16_14BPS_4X4 (QAIRT_CAPABILITY_GROUP_TENSOR + 54)

/**
 * @brief CAPABILITY key for determining whether a backend supports
 *        QAIRT_TENSOR_DATA_FORMAT_UBWC_BAYER16_16BPS_1X1 data format.
 */
#define QAIRT_CAPABILITY_TENSOR_SUPPORT_UBWC_BAYER16_16BPS_1X1 (QAIRT_CAPABILITY_GROUP_TENSOR + 55)

/**
 * @brief CAPABILITY key for determining whether a backend supports
 *        QAIRT_TENSOR_DATA_FORMAT_UBWC_BAYER16_16BPS_2X2 data format.
 */
#define QAIRT_CAPABILITY_TENSOR_SUPPORT_UBWC_BAYER16_16BPS_2X2 (QAIRT_CAPABILITY_GROUP_TENSOR + 56)

/**
 * @brief CAPABILITY key for determining whether a backend supports
 *        QAIRT_TENSOR_DATA_FORMAT_UBWC_BAYER16_16BPS_4X4 data format.
 */
#define QAIRT_CAPABILITY_TENSOR_SUPPORT_UBWC_BAYER16_16BPS_4X4 (QAIRT_CAPABILITY_GROUP_TENSOR + 57)

///
/// Definition of QAIRT_CAPABILITY_GROUP_ERROR CAPABILITY group. This group is
/// Optional portion of API.
///

/**
 * @brief CAPABILITY key for the QAIRT Error API CAPABILITY group. This can be
 *        used as a key to check if Error API is supported by a
 *        backend.
 */
#define QAIRT_CAPABILITY_GROUP_ERROR (QAIRT_CAPABILITY_GROUP_CORE + 1000)

/**
 * @brief CAPABILITY key to determine whether a backend supports retrieving
 *        verbose string descriptors of errorHandles. It determines
 *        support for QAIRTError_getVerboseMessage.
 */
#define QAIRT_CAPABILITY_ERROR_GET_VERBOSE_MESSAGE (QAIRT_CAPABILITY_GROUP_ERROR + 1)

///
/// Definition of QAIRT_CAPABILITY_GROUP_MEMORY CAPABILITY group. This group is
/// an optional API.
///

/**
 * @brief CAPABILITY group for the QAIRT Memory API CAPABILITY group. This can
 *        be used as a key to check if Memory API is supported by a
 *        backend.
 */
#define QAIRT_CAPABILITY_GROUP_MEMORY (QAIRT_CAPABILITY_GROUP_CORE + 1100)

/**
 * @brief CAPABILITY key to determine whether a backend supports ion memory
 *        type.
 */
#define QAIRT_CAPABILITY_MEMORY_SUPPORT_MEM_TYPE_ION (QAIRT_CAPABILITY_GROUP_MEMORY + 1)

/**
 * @brief CAPABILITY key to determine whether a backend supports custom memory
 *        type.
 */
#define QAIRT_CAPABILITY_MEMORY_SUPPORT_MEM_TYPE_CUSTOM (QAIRT_CAPABILITY_GROUP_MEMORY + 2)

/**
 * @brief CAPABILITY key to determine whether a backend supports DMA-BUF memory
 *        type.
 */
#define QAIRT_CAPABILITY_MEMORY_SUPPORT_MEM_TYPE_DMA_BUF (QAIRT_CAPABILITY_GROUP_MEMORY + 3)

/**
 * @brief CAPABILITY key to determine whether a backend supports DX12 memory
 *        type.
 */
#define QAIRT_CAPABILITY_MEMORY_SUPPORT_MEM_TYPE_DX12 (QAIRT_CAPABILITY_GROUP_MEMORY + 4)

///
/// Definition of QAIRT_CAPABILITY_GROUP_SIGNAL CAPABILITY group. This group is
/// an optional API.
///

/**
 * @brief CAPABILITY group for signal support. This can be used as a key to
 *        check if Signal API is supported by a backend.
 */
#define QAIRT_CAPABILITY_GROUP_SIGNAL (QAIRT_CAPABILITY_GROUP_CORE + 1200)

/**
 * @brief CAPABILITY key to determine whether a backend supports abort signals.
 */
#define QAIRT_CAPABILITY_SIGNAL_SUPPORT_ABORT QAIRT_CAPABILITY_GROUP_SIGNAL + 1

/**
 * @brief CAPABILITY key to determine whether a backend supports timeout
 *        signals.
 */
#define QAIRT_CAPABILITY_SIGNAL_SUPPORT_TIMEOUT QAIRT_CAPABILITY_GROUP_SIGNAL + 2

///
/// Definition of QAIRT_CAPABILITY_GROUP_LOG CAPABILITY group. This group is an
/// optional API.
///

/**
 * @brief CAPABILITY group for log support. This can be used as a key to
 *        check if Log API is supported by a backend.
 */
#define QAIRT_CAPABILITY_GROUP_LOG (QAIRT_CAPABILITY_GROUP_CORE + 1300)

/**
 * @brief CAPABILITY key for determining whether a backend supports logging with
 *        the system's default stream (callback=NULL).
 */
#define QAIRT_CAPABILITY_LOG_SUPPORTS_DEFAULT_STREAM (QAIRT_CAPABILITY_GROUP_LOG + 1)

///
/// Definition of QAIRT_CAPABILITY_GROUP_PROFILE CAPABILITY group. This group is
/// an optional API.
///

/**
 * @brief CAPABILITY group for profile support. This can be used as a key to
 *        check if Profile API is supported by a backend.
 */
#define QAIRT_CAPABILITY_GROUP_PROFILE (QAIRT_CAPABILITY_GROUP_CORE + 1400)

/**
 * @brief CAPABILITY key for determining whether a backend supports the
 *        QAIRT_PROFILE_CONFIG_OPTION_CUSTOM config option.
 */
#define QAIRT_CAPABILITY_PROFILE_SUPPORT_CUSTOM_CONFIG (QAIRT_CAPABILITY_GROUP_PROFILE + 1)

/**
 * @brief CAPABILITY key for determining whether a backend supports the
 *        QAIRT_PROFILE_CONFIG_OPTION_MAX_EVENTS config option.
 */
#define QAIRT_CAPABILITY_PROFILE_SUPPORT_MAX_EVENTS_CONFIG (QAIRT_CAPABILITY_GROUP_PROFILE + 2)

/**
 * @brief CAPABILITY key for determining whether a backend supports querying
 *        extended event data. It determines support for
 *        QAIRTProfile_getExtendedEventData.
 */
#define QAIRT_CAPABILITY_PROFILE_SUPPORTS_EXTENDED_EVENT (QAIRT_CAPABILITY_GROUP_PROFILE + 3)

/**
 * @brief CAPABILITY key for determining whether a backend supports the
 *        QAIRT_PROFILE_CONFIG_OPTION_ENABLE_OPTRACE config option.
 */
#define QAIRT_CAPABILITY_PROFILE_SUPPORT_OPTRACE_CONFIG (QAIRT_CAPABILITY_GROUP_PROFILE + 4)

/**
 * @brief CAPABILITY group for device support. This can be used as a key to
 *        check if Device API is supported by a backend.
 */
#define QAIRT_CAPABILITY_GROUP_DEVICE (QAIRT_CAPABILITY_GROUP_CORE + 1500)

/**
 * @brief CAPABILITY key for determining if a backend supports
 *        QAIRTDevice_getInfrastructure.
 */
#define QAIRT_CAPABILITY_DEVICE_SUPPORT_INFRASTRUCTURE (QAIRT_CAPABILITY_GROUP_DEVICE + 1)

///
/// Definition of QAIRT_CAPABILITY_GROUP_CUSTOM CAPABILITY group. This group
/// represents backend defined properties.
///

/**
 * @brief CAPABILITY group for custom backend properties.
 */
#define QAIRT_CAPABILITY_GROUP_CUSTOM (QAIRT_CAPABILITY_GROUP_CORE + 2000)

typedef uint32_t QairtCapability_Key_t;

#ifdef __cplusplus
};
#endif

#endif  // QAISW_QAIRTBACKENDCAPABILITYKEYS_H
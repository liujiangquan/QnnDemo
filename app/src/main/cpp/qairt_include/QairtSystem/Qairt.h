//==============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//==============================================================================

/**
 * @file
 * @brief   Qairt component API
 *
 *          A header which contains the QAIRT component
 *          for executing inference with user-managed tensors.
 */

#ifndef QAISW_QAIRT_H
#define QAISW_QAIRT_H

#include "QairtSystemCommon.h"
#include "QairtSystemContext.h"
#include "QairtTensor/QairtTensor.h"

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
// Macros
//=============================================================================

/**
 * @brief Interface ID for Qairt V1
 *
 * @note This is also defined in QairtSystemBuilder.h
 */
#ifndef QAIRT_V1_ID
#define QAIRT_V1_ID (QAIRT_MIN_ID_SYSTEM + 11)
#endif

//=============================================================================
// Data Types
//=============================================================================

/**
 * @brief Handle to a Qairt runtime instance
 *
 * @note This is also defined in QairtSystemBuilder.h
 */
#ifndef QAIRT_HANDLE_V1_T_DEFINED
#define QAIRT_HANDLE_V1_T_DEFINED
typedef void* Qairt_Handle_t;
#endif

/**
 * @brief Qairt runtime API error codes
 */
typedef enum {
  QAIRT_MIN_ERROR = QAIRT_MIN_ERROR_SYSTEM + 200,
  //////////////////////////////////////////

  /// Qairt runtime success
  QAIRT_NO_ERROR = QAIRT_SUCCESS,
  /// Invalid/NULL Qairt handle
  QAIRT_ERROR_INVALID_HANDLE = 30200,
  /// Invalid function argument
  QAIRT_ERROR_INVALID_ARGUMENT = 30201,
  /// Graph not found
  QAIRT_ERROR_CONTEXT_NOT_FOUND = 30202,
  /// Execution failed
  QAIRT_ERROR_EXECUTION_FAILED = 30203,

  //////////////////////////////////////////
  QAIRT_MAX_ERROR = QAIRT_MIN_ERROR_SYSTEM + 299,
  /// Unused, present to ensure 32 bits
  QAIRT_ERROR_UNDEFINED = 0x7FFFFFFF
} Qairt_Error_t;

//=============================================================================
// Public Functions
//=============================================================================

/**
 * @brief Free a Qairt runtime instance
 *
 * Destroys the Qairt instance and releases all associated resources including
 * backend, context, and graphs.
 *
 * @param[in] qairtHandle Handle to the Qairt instance to free
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Qairt instance was successfully freed
 *         - QAIRT_ERROR_INVALID_HANDLE: Invalid Qairt handle
 *
 * @note Use corresponding API through Qairt_V1_t
 * @note There is no create function - Qairt instances are created via QairtSystemBuilder_build()
 */
Qairt_Status_t Qairt_free(Qairt_Handle_t qairtHandle);

/**
 * @brief Execute a specific graph with user-provided tensor info and data buffers
 ** Executes inference on the specified graph using separate tensor metadata and data buffers.
 * Users are responsible for:
 * - Obtaining tensor info from graph info (via Qairt_getNumGraphInfos and Qairt_getGraphInfoAt)
 * - Allocating and filling input data buffers
 * - Allocating output data buffers
 * - Managing buffer memory lifecycle
 *
 * @param[in] qairtHandle Handle to Qairt instance
 * @param[in] graphName Name of the graph to execute
 * @param[in] inputTensorInfos Array of input tensor info handles
 * @param[in] inputDataBuffers Array of input data buffer pointers (must match inputTensorInfos
 *order)
 * @param[in] numInputs Number of inputs
 * @param[in] outputTensorInfos Array of output tensor info handles
 * @param[in,out] outputDataBuffers Array of output data buffer pointers (must match
 *outputTensorInfos order)
 * @param[in] numOutputs Number of outputs
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Execution successful
 *         - QAIRT_ERROR_INVALID_HANDLE: Invalid qairt handle
 *         - QAIRT_ERROR_INVALID_ARGUMENT: Invalid parameters (NULL pointers or invalid counts)
 *         - QAIRT_ERROR_CONTEXT_NOT_FOUND: Graph not found
 *         - QAIRT_ERROR_EXECUTION_FAILED: Execution failed
 *
 * @note Use corresponding API through Qairt_V1_t
 * @note Tensor info handles are obtained from graph info, not created by users
 ** Example:
 * @code
 *   // Get graph information
 *   uint32_t numGraphs;
 *   Qairt_getNumGraphInfos(qairt, &numGraphs);
 *
 *   QairtSystem_Context_GraphInfoHandle_t graphInfo;
 *   Qairt_getGraphInfoAt(qairt, 0, &graphInfo);
 *
 *   // Get tensor infos for the first graph
 *   QairtSystem_Context_TensorInfoHandle_t* inputTensorInfos;
 *   uint32_t numInputs;
 *   QairtSystem_Context_GraphInfo_getInputAt(graphInfo, 0, &inputTensorInfos);
 *
 *   // Allocate and fill input buffers
 *   uint8_t* inputBuffers[numInputs];
 *   // ... allocate and fill buffers based on tensor info ...
 *
 *   // Get output tensor infos and allocate buffers
 *   QairtSystem_Context_TensorInfoHandle_t* outputTensorInfos;
 *   uint32_t numOutputs;
 *   QairtSystem_Context_GraphInfo_getOutputAt(graphInfo, 0, &outputTensorInfos);
 *   uint8_t* outputBuffers[numOutputs];
 *   // ... allocate output buffers ...
 *
 *   // Execute
 *   const char* graphName;
 *   QairtSystem_Context_GraphInfo_getName(graphInfo, &graphName);
 *   Qairt_Status_t status = Qairt_executeGraph(qairt, graphName,
 *                                                    inputTensorInfos, inputBuffers, numInputs,
 *                                                    outputTensorInfos, outputBuffers, numOutputs);
 * @endcode
 */
Qairt_Status_t Qairt_executeGraph(Qairt_Handle_t qairtHandle,
                                       const char* graphName,
                                       QairtSystem_Context_TensorInfoHandle_t* inputTensorInfos,
                                       uint8_t** inputDataBuffers,
                                       uint32_t numInputs,
                                       QairtSystem_Context_TensorInfoHandle_t* outputTensorInfos,
                                       uint8_t** outputDataBuffers,
                                       uint32_t numOutputs);

/**
 * @brief Execute the default graph with user-provided tensor info and data buffers
 *
 * Convenience function that executes the first/default graph without
 * needing to specify the graph name. Equivalent to calling Qairt_executeGraph
 * with the first graph's name.
 *
 * @param[in] qairtHandle Handle to Qairt instance
 * @param[in] inputTensorInfos Array of input tensor info handles
 * @param[in] inputDataBuffers Array of input data buffer pointers (must match inputTensorInfos
 * order)
 * @param[in] numInputs Number of inputs
 * @param[in] outputTensorInfos Array of output tensor info handles
 * @param[in,out] outputDataBuffers Array of output data buffer pointers (must match
 * outputTensorInfos order)
 * @param[in] numOutputs Number of outputs
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Execution successful
 *         - QAIRT_ERROR_INVALID_HANDLE: Invalid qairt handle
 *         - QAIRT_ERROR_INVALID_ARGUMENT: Invalid parameters (NULL pointers or invalid counts)
 *         - QAIRT_ERROR_EXECUTION_FAILED: Execution failed
 *
 * @note Use corresponding API through Qairt_V1_t
 * @note Tensor info handles are obtained from graph info, not created by users
 *
 * Example:
 * @code
 *   // Execute default graph
 *   Qairt_Status_t status = Qairt_execute(qairt,
 *                                               inputTensorInfos, inputBuffers, numInputs,
 *                                               outputTensorInfos, outputBuffers, numOutputs);
 * @endcode
 */
Qairt_Status_t Qairt_execute(Qairt_Handle_t qairtHandle,
                                  QairtSystem_Context_TensorInfoHandle_t* inputTensorInfos,
                                  uint8_t** inputDataBuffers,
                                  uint32_t numInputs,
                                  QairtSystem_Context_TensorInfoHandle_t* outputTensorInfos,
                                  uint8_t** outputDataBuffers,
                                  uint32_t numOutputs);

/**
 * @brief Get the number of graphs in the model
 *
 * Returns the count of graphs available in the loaded model.
 * This should be called before Qairt_getGraphInfoAt() to determine
 * how many graph info handles to allocate.
 *
 * @param[in] qairtHandle Handle to Qairt instance
 * @param[out] count Number of graphs in the model
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Count retrieved successfully
 *         - QAIRT_ERROR_INVALID_HANDLE: Invalid qairt handle
 *         - QAIRT_ERROR_INVALID_ARGUMENT: _count_ is NULL
 *
 * @note Use corresponding API through Qairt_V1_t
 ** Example:
 * @code
 *   uint32_t numGraphs;
 *   Qairt_Status_t status = Qairt_getNumGraphInfos(qairt, &numGraphs);
 *   if (status == QAIRT_SUCCESS) {
 *     printf("Model contains %u graphs\n", numGraphs);
 *   }
 * @endcode
 */
Qairt_Status_t Qairt_getNumGraphInfos(Qairt_Handle_t qairtHandle, uint32_t* count);

/**
 * @brief Get graph information at a specific index
 *
 * Retrieves the graph info handle for the graph at the specified index.
 * The index must be in the range [0, count-1] where count is obtained
 * from Qairt_getNumGraphInfos().
 *
 * @param[in] qairtHandle Handle to Qairt instance
 * @param[in] index Index of the graph (0-based, must be < count from Qairt_getNumGraphInfos)
 * @param[out] graphInfo Handle to the graph info at the specified index
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Graph info retrieved successfully
 *         - QAIRT_ERROR_INVALID_HANDLE: Invalid qairt handle
 *         - QAIRT_ERROR_INVALID_ARGUMENT: _graphInfo_ is NULL or _index_ is out of bounds
 *
 * @note Use corresponding API through Qairt_V1_t
 * @note The returned graph info handle is owned by the Qairt instance and should not be freed
 * @note Graph info handles remain valid until the Qairt instance is freed
 *
 * Example:
 * @code
 *   // Get count and allocate array
 *   uint32_t numGraphs;
 *   Qairt_getNumGraphInfos(qairt, &numGraphs);
 *   QairtSystem_Context_GraphInfoHandle_t* graphInfos =
 *       malloc(numGraphs * sizeof(QairtSystem_Context_GraphInfoHandle_t));
 *
 *   // Retrieve each graph info
 *   for (uint32_t i = 0; i < numGraphs; i++) {
 *     Qairt_Status_t status = Qairt_getGraphInfoAt(qairt, i, &graphInfos[i]);
 *   }
 *
 *   // User is responsible for freeing the array
 *   free(graphInfos);
 * @endcode
 */
Qairt_Status_t Qairt_getGraphInfoAt(Qairt_Handle_t qairtHandle,
                                         uint32_t index,
                                         QairtSystem_Context_GraphInfoHandle_t* graphInfo);

/**
 * @brief Get the last error message
 *
 * Returns a detailed error message for the last operation that failed.
 * Useful for debugging and error reporting.
 *
 * @param[in] qairtHandle Handle to Qairt instance
 * @param[out] errorMsg Pointer to error message string
 *
 * @return Error code:
 *         - QAIRT_SUCCESS: Error message retrieved (may be empty if no error)
 *         - QAIRT_ERROR_INVALID_HANDLE: Invalid qairt handle
 *         - QAIRT_ERROR_INVALID_ARGUMENT: _errorMsg_ is NULL
 *
 * @note Use corresponding API through Qairt_V1_t
 * @note The returned string is owned by the Qairt instance and should not be freed
 * @note The error message remains valid until the next operation or until the Qairt instance is
 * freed
 * @note Returns NULL or empty string if no error has occurred
 *
 * Example:
 * @code
 *   const char* errorMsg;
 *   Qairt_Status_t status = Qairt_execute(qairt, inputs, 2, outputs, 1);
 *   if (status != QAIRT_SUCCESS) {
 *     Qairt_getLastError(qairt, &errorMsg);
 *     printf("Execution failed: %s\n", errorMsg ? errorMsg : "Unknown error");
 *   }
 * @endcode
 */
Qairt_Status_t Qairt_getLastError(Qairt_Handle_t qairtHandle, const char** errorMsg);

/** Return number of true model input tensors (no transfer tensors). */
Qairt_Status_t Qairt_getNumModelInputInfos(Qairt_Handle_t qairtHandle, uint32_t* count);
/** Return pointer to i-th true model input SystemContextTensorInfo (as void*). */
Qairt_Status_t Qairt_getModelInputInfoAt(Qairt_Handle_t qairtHandle, uint32_t index, void** out);
/** Return number of true model output tensors. */
Qairt_Status_t Qairt_getNumModelOutputInfos(Qairt_Handle_t qairtHandle, uint32_t* count);
/** Return pointer to i-th true model output SystemContextTensorInfo (as void*). */
Qairt_Status_t Qairt_getModelOutputInfoAt(Qairt_Handle_t qairtHandle, uint32_t index, void** out);

//=============================================================================
// Function Pointer Typedefs
//=============================================================================

typedef Qairt_Status_t (*Qairt_FreeFn_t)(Qairt_Handle_t);

typedef Qairt_Status_t (*Qairt_ExecuteGraphFn_t)(Qairt_Handle_t,
                                                      const char*,
                                                      QairtSystem_Context_TensorInfoHandle_t*,
                                                      uint8_t**,
                                                      uint32_t,
                                                      QairtSystem_Context_TensorInfoHandle_t*,
                                                      uint8_t**,
                                                      uint32_t);

typedef Qairt_Status_t (*Qairt_ExecuteFn_t)(Qairt_Handle_t,
                                                 QairtSystem_Context_TensorInfoHandle_t*,
                                                 uint8_t**,
                                                 uint32_t,
                                                 QairtSystem_Context_TensorInfoHandle_t*,
                                                 uint8_t**,
                                                 uint32_t);

typedef Qairt_Status_t (*Qairt_GetNumGraphInfosFn_t)(Qairt_Handle_t, uint32_t*);

typedef Qairt_Status_t (*Qairt_GetGraphInfoAtFn_t)(Qairt_Handle_t,
                                                        uint32_t,
                                                        QairtSystem_Context_GraphInfoHandle_t*);

typedef Qairt_Status_t (*Qairt_GetLastErrorFn_t)(Qairt_Handle_t, const char**);

typedef Qairt_Status_t (*Qairt_GetNumModelInputInfosFn_t)(Qairt_Handle_t, uint32_t*);
typedef Qairt_Status_t (*Qairt_GetModelInputInfoAtFn_t)(Qairt_Handle_t, uint32_t, void**);
typedef Qairt_Status_t (*Qairt_GetNumModelOutputInfosFn_t)(Qairt_Handle_t, uint32_t*);
typedef Qairt_Status_t (*Qairt_GetModelOutputInfoAtFn_t)(Qairt_Handle_t, uint32_t, void**);

//=============================================================================
// Public Interfaces
//=============================================================================

/**
 * @brief Qairt V1 interface struct
 *
 * This struct contains the functions that define the V1 interface for
 * QAIRT Runtime. This interface can be retrieved through the id QAIRT_V1_ID.
 *
 * @note Qairt instances are created via QairtSystemBuilder_build(), not directly
 */
typedef struct {
  uint64_t size;
  Qairt_GetInterfaceFn_t getInterface;

  // Lifecycle
  Qairt_FreeFn_t free;

  // Execution
  Qairt_ExecuteGraphFn_t executeGraph;
  Qairt_ExecuteFn_t execute;

  // Information
  Qairt_GetNumGraphInfosFn_t getNumGraphInfos;
  Qairt_GetGraphInfoAtFn_t getGraphInfoAt;

  // State
  Qairt_GetLastErrorFn_t getLastError;

  // Unified model I/O — transparent for both single-backend and multi-backend.
  // Returns count and pointers (as void*) to SystemContextTensorInfo elements.
  // For multi-backend: inputs from first subnet (no transfer tensors),
  // outputs from last subnet (true final outputs only).
  Qairt_GetNumModelInputInfosFn_t  getNumModelInputInfos;
  Qairt_GetModelInputInfoAtFn_t    getModelInputInfoAt;
  Qairt_GetNumModelOutputInfosFn_t getNumModelOutputInfos;
  Qairt_GetModelOutputInfoAtFn_t   getModelOutputInfoAt;
} Qairt_V1_t;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // QAISW_QAIRT_H

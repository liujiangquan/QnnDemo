//==============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//==============================================================================

#pragma once

#include <string>
#include <vector>
#include <utility>

#include "../QairtApi.hpp"
#include "QairtSystem/Qairt.h"
#include "QairtCppApi/System/QairtSystemContext.hpp"

namespace qairt {

// Forward declaration
class QairtSystemBuilder;

/**
 * @brief C++ wrapper for the QAIRT High Level Runtime.
 *
 * Qairt provides a single-object interface for running neural network inference.
 * It owns the backend, context, and compiled graphs for a loaded DLC model.
 * Users supply their own input/output buffers; Qairt handles all internal tensor
 * and memory management.
 *
 * ## Lifecycle
 * Instances are created exclusively through QairtSystemBuilder::build(), which
 * performs the full initialization sequence:
 *   1. Backend library load
 *   2. Context creation from the DLC
 *   3. Graph composition and finalization
 *
 * A default-constructed Qairt has no API table and must NOT be used for any
 * calls until a fully initialized instance is move-assigned into it:
 * @code
 *   qairt::Qairt obj;           // default-constructed — not usable yet
 *   obj = builder.build();      // move-assign a fully initialized instance
 * @endcode
 * Qairt is **move-only** (copy constructor is deleted).
 *
 * ## Execution Model
 * Users are responsible for:
 * - Obtaining tensor metadata via getGraphInfoAt(), then calling
 *   getGraphInputs() / getGraphOutputs() on the returned SystemContextGraphInfo
 * - Allocating input and output data buffers correctly sized for each tensor
 * - Filling input buffers before each execute call
 * - Reading output buffers after each execute call
 * - Freeing all buffers (Qairt never takes ownership)
 *
 * Qairt is responsible for:
 * - Running inference and writing results into caller-provided output buffers
 * - Managing the lifetime of SystemContextTensorInfo objects (valid for the
 *   lifetime of the Qairt instance)
 *
 * ## Complete Usage Example
 * @code
 *   // 1. Build Qairt (via QairtSystemBuilder)
 *   auto qairt = builder.build();
 *
 *   // 2. Get graph information — use auto& (copy constructor is deleted)
 *   uint32_t numGraphs = qairt.getNumGraphInfos();
 *   auto& graphInfo = qairt.getGraphInfoAt(0);
 *   std::string graphName = graphInfo.getGraphName();  // e.g. "inception_v3"
 *
 *   // 3. Get tensor information
 *   auto& inputs = graphInfo.getGraphInputs();
 *   auto& outputs = graphInfo.getGraphOutputs();
 *
 *   // 4. Prepare input buffers
 *   std::vector<std::pair<qairt::SystemContextTensorInfo*, uint8_t*>> inputData;
 *   for (auto& tensorInfo : inputs) {
 *     uint8_t* buffer = allocateAndFillBuffer(tensorInfo);
 *     inputData.emplace_back(&tensorInfo, buffer);
 *   }
 *
 *   // 5. Prepare output buffers
 *   std::vector<std::pair<qairt::SystemContextTensorInfo*, uint8_t*>> outputData;
 *   for (auto& tensorInfo : outputs) {
 *     uint8_t* buffer = allocateBuffer(tensorInfo);
 *     outputData.emplace_back(&tensorInfo, buffer);
 *   }
 *
 *   // 6. Execute inference
 *   qairt.executeGraph(graphName, inputData, outputData);
 *
 *   // 7. Process outputs (user's responsibility)
 *   // Output data is now in the buffers pointed to by outputData vector
 *
 *   // 8. Clean up buffers (user's responsibility)
 *   for (auto& pair : inputData) delete[] pair.second;
 *   for (auto& pair : outputData) delete[] pair.second;
 * @endcode
 */
class Qairt : public ApiType<Qairt, Qairt_V1_t, SystemApiTable> {
  friend SystemApi;
  friend class QairtSystemBuilder;

  // Mirrors the m_inputTensorInfos pattern in SystemContextGraphInfo.
  // The crossable vector mechanism calls constructNonOwning + updateAfterCross_()
  // on each element, ensuring crossable fields like m_name are populated so that
  // getGraphName() returns the correct value instead of empty string.
  mutable detail::crossable<std::vector<detail::non_owning<SystemContextGraphInfo>>,
                            &interface_type::getGraphInfoAt,
                            &interface_type::getNumGraphInfos>
      m_graphInfosCache;

 public:
  using base_type::base_type;
  using base_type::operator=;

  Qairt() noexcept                       = default;
  Qairt(const Qairt&)                    = delete;
  Qairt(Qairt&&) noexcept                = default;
  Qairt& operator=(const Qairt&)         = delete;
  Qairt& operator=(Qairt&&) noexcept     = default;

  // ============================================================================
  // Execution Methods
  // ============================================================================

  /**
   * @brief Execute a named graph with user-provided input and output buffers.
   *
   * Performs a synchronous inference pass on the graph identified by
   * @p graphName. For each input tensor the caller provides a pre-filled buffer;
   * for each output tensor the caller provides a pre-allocated buffer that will
   * be overwritten with inference results.
   *
   * @param graphName  Name of the graph to execute, obtained from
   *                   getGraphInfoAt(i).getGraphName(). Must not be empty.
   * @param inputs     Ordered vector of (SystemContextTensorInfo*, data buffer) pairs
   *                   for input tensors.
   * @param outputs    Ordered vector of (SystemContextTensorInfo*, data buffer) pairs
   *                   for output tensors.
   * @throws qairt::Exception  If execution fails, the graph is not found, or any
   *                            argument is invalid. On failure, call getLastError()
   *                            for a detailed description of the root cause.
   *
   * @note **TensorInfo pointer semantics:**
   *       - Each SystemContextTensorInfo* must point to an element obtained from
   *         getGraphInfoAt(i).getGraphInputs() or getGraphOutputs() — i.e., it
   *         must live inside the Qairt instance's internal graph info cache.
   *       - Do NOT pass a pointer to a separately constructed or heap-allocated
   *         SystemContextTensorInfo; doing so causes undefined behavior in the
   *         CAPI layer, which casts the pointer back to a concrete type.
   *       - These pointers remain valid for the lifetime of the Qairt instance.
   *         Never delete them — ownership belongs to Qairt.
   *       - The pairs in @p inputs and @p outputs must be in the same order as
   *         the tensors returned by getGraphInputs() and getGraphOutputs().
   *
   * @note **Buffer requirements:**
   *       - Input buffers must be fully filled with valid data before this call.
   *       - Output buffers must be allocated with at least the number of bytes
   *         required for the tensor's dimensions and data type. Use
   *         RuntimeApi::calculateBufferSize(dims, dataType) to compute this.
   *       - Both input and output buffers must remain alive for the entire
   *         duration of this call.
   *       - Buffer memory is not owned by Qairt; the caller is responsible for
   *         allocation and deallocation.
   *       - For quantized models the buffers hold native quantized data (e.g.
   *         uint8). Dequantization to float is the caller's responsibility.
   *
   * Example:
   * @code
   *   // Get graph info — bind to a reference (copy constructor is deleted)
   *   auto& graphInfo = qairt.getGraphInfoAt(0);
   *   std::string graphName = graphInfo.getGraphName();
   *
   *   // Prepare inputs
   *   std::vector<std::pair<qairt::SystemContextTensorInfo*, uint8_t*>> inputs;
   *   for (auto& tensorInfo : graphInfo.getGraphInputs()) {
   *     uint8_t* buffer = allocateAndFillBuffer(tensorInfo);
   *     inputs.emplace_back(&tensorInfo, buffer);
   *   }
   *
   *   // Prepare outputs
   *   std::vector<std::pair<qairt::SystemContextTensorInfo*, uint8_t*>> outputs;
   *   for (auto& tensorInfo : graphInfo.getGraphOutputs()) {
   *     uint8_t* buffer = allocateBuffer(tensorInfo);
   *     outputs.emplace_back(&tensorInfo, buffer);
   *   }
   *
   *   // Execute
   *   qairt.executeGraph(graphName, inputs, outputs);
   * @endcode
   */
  void executeGraph(
      const std::string& graphName,
      const std::vector<std::pair<SystemContextTensorInfo*, uint8_t*>>& inputs,
      std::vector<std::pair<SystemContextTensorInfo*, uint8_t*>>& outputs) {

    // Convert vectors to C arrays
    std::vector<QairtSystem_Context_TensorInfoHandle_t> inputHandles;
    std::vector<uint8_t*> inputBuffers;
    inputHandles.reserve(inputs.size());
    inputBuffers.reserve(inputs.size());

    for (const auto& pair : inputs) {
      // Pass the C++ wrapper pointer itself as the handle value so the C bridge
      // can cast it back to SystemContextTensorInfo* without type confusion.
      // getHandle() would return the inner C-struct handle, which has a different
      // memory layout and cannot be reinterpret_cast back to SystemContextTensorInfo*.
      inputHandles.push_back(reinterpret_cast<QairtSystem_Context_TensorInfoHandle_t>(pair.first));
      inputBuffers.push_back(pair.second);
    }

    std::vector<QairtSystem_Context_TensorInfoHandle_t> outputHandles;
    std::vector<uint8_t*> outputBuffers;
    outputHandles.reserve(outputs.size());
    outputBuffers.reserve(outputs.size());

    for (const auto& pair : outputs) {
      outputHandles.push_back(reinterpret_cast<QairtSystem_Context_TensorInfoHandle_t>(pair.first));
      outputBuffers.push_back(pair.second);
    }
    // Call C API
    callAndConfirm<&Qairt_V1_t::executeGraph>(
        graphName.c_str(),
        inputHandles.data(),
        inputBuffers.data(),
        static_cast<uint32_t>(inputs.size()),
        outputHandles.data(),
        outputBuffers.data(),
        static_cast<uint32_t>(outputs.size()));
  }

  /**
   * @brief Execute the first (default) graph with user-provided input and output buffers.
   *
   * Convenience overload that resolves the graph name internally. Use this when
   * the DLC contains a single graph and you do not need to reference the graph
   * by name. For DLCs with multiple graphs use executeGraph() with an explicit
   * name to avoid ambiguity.
   *
   * All pointer semantics and buffer requirements are identical to executeGraph();
   * refer to that method for full documentation.
   *
   * @param inputs   Ordered vector of (SystemContextTensorInfo*, data buffer) pairs
   *                 for input tensors. Same rules as executeGraph().
   * @param outputs  Ordered vector of (SystemContextTensorInfo*, data buffer) pairs
   *                 for output tensors. Same rules as executeGraph().
   * @throws qairt::Exception  If execution fails or any argument is invalid.
   *
   * @note All pointer and buffer requirements documented in executeGraph() apply here.
   * @note The graph name is resolved from the first graph in the loaded DLC.
   *       If the DLC was loaded with multiple graphs, only the first graph runs.
   *
   * Example:
   * @code
   *   // Prepare inputs and outputs exactly as for executeGraph()
   *   // ...
   *
   *   // Execute the default (first) graph — no need to look up the graph name
   *   qairt.execute(inputs, outputs);
   * @endcode
   */
  void execute(
      const std::vector<std::pair<SystemContextTensorInfo*, uint8_t*>>& inputs,
      std::vector<std::pair<SystemContextTensorInfo*, uint8_t*>>& outputs) {
    // Convert vectors to C arrays
    std::vector<QairtSystem_Context_TensorInfoHandle_t> inputHandles;
    std::vector<uint8_t*> inputBuffers;
    inputHandles.reserve(inputs.size());
    inputBuffers.reserve(inputs.size());
    for (const auto& pair : inputs) {
      inputHandles.push_back(reinterpret_cast<QairtSystem_Context_TensorInfoHandle_t>(pair.first));
      inputBuffers.push_back(pair.second);
    }
    std::vector<QairtSystem_Context_TensorInfoHandle_t> outputHandles;
    std::vector<uint8_t*> outputBuffers;
    outputHandles.reserve(outputs.size());
    outputBuffers.reserve(outputs.size());
    for (const auto& pair : outputs) {
      outputHandles.push_back(reinterpret_cast<QairtSystem_Context_TensorInfoHandle_t>(pair.first));
      outputBuffers.push_back(pair.second);
    }
    // Call C API
    callAndConfirm<&Qairt_V1_t::execute>(
        inputHandles.data(),
        inputBuffers.data(),
        static_cast<uint32_t>(inputs.size()),
        outputHandles.data(),
        outputBuffers.data(),
        static_cast<uint32_t>(outputs.size()));
  }

  // ============================================================================
  // Information Methods
  // ============================================================================

  /**
   * @brief Return the number of graphs compiled into the loaded DLC.
   *
   * @return  Number of available graphs. Always ≥ 1 after a successful
   *          QairtSystemBuilder::build(); zero only if the DLC contained no
   *          graphs (which would also cause build() to fail).
   * @throws qairt::Exception if the underlying CAPI call fails.
   *
   * @note Use this together with getGraphInfoAt() to iterate over all graphs:
   * @code
   *   for (uint32_t i = 0; i < qairt.getNumGraphInfos(); ++i) {
   *     auto& gi = qairt.getGraphInfoAt(i);
   *     std::cout << gi.getGraphName() << "\n";
   *   }
   * @endcode
   */
  uint32_t getNumGraphInfos() const {
    return callGetter<&Qairt_V1_t::getNumGraphInfos>();
  }

  /**
   * @brief Return a reference to the graph info for the graph at the given index.
   *
   * The returned reference points into an internal cache owned by this Qairt
   * instance. The cache is populated on first call and remains stable for the
   * lifetime of the Qairt instance. The SystemContextGraphInfo exposes:
   * - getGraphName()      — the graph's string identifier (e.g. "inception_v3")
   * - getGraphInputs()    — ordered list of input tensor metadata
   * - getGraphOutputs()   — ordered list of output tensor metadata
   *
   * @param index  Zero-based graph index. Must be in [0, getNumGraphInfos()).
   * @return       Non-const lvalue reference to the graph info. The reference
   *               is stable for the lifetime of this Qairt instance.
   * @throws qairt::Exception  If @p index is out of bounds or the CAPI call fails.
   *
   * @note **Bind to a reference, not a value.** SystemContextGraphInfo has a
   *       deleted copy constructor. Writing `auto gi = qairt.getGraphInfoAt(0)`
   *       will not compile — use `auto& gi = qairt.getGraphInfoAt(0)` instead.
   * @note The SystemContextTensorInfo pointers inside getGraphInputs() and
   *       getGraphOutputs() are valid for the lifetime of this Qairt instance.
   *       Do not store them beyond that lifetime and do not delete them.
   *
   * Example:
   * @code
   *   auto& graphInfo = qairt.getGraphInfoAt(0);
   *   std::string name = graphInfo.getGraphName();
   *   for (auto& ti : graphInfo.getGraphInputs()) {
   *     // use ti.getName(), ti.getDimensions(), ti.getDataType(), etc. after proper casting
   *   }
   * @endcode
   */
  SystemContextGraphInfo& getGraphInfoAt(uint32_t index) const {
    return m_graphInfosCache.get(*this)[index];
  }

  /**
   * @brief Return the last error message recorded by the Qairt runtime.
   *
   * After any failed executeGraph() / execute() call the runtime stores a
   *  description of the root cause (e.g."Exception during graph execution:
   * CAPI call failed calling QairtGraph_V1_t::execute: <error_code>").
   *  Calling this immediately after catching a qairt::Exception gives the most
   * specific failure detail available, beyond what e.what() alone provides.
   *
   * @return  Descriptive error string. Empty string if the last operation
   *          succeeded or if no operation has been attempted yet.
   * @throws qairt::Exception  If this Qairt object has no valid API table —
   *                            for example when called on a default-constructed
   *                            instance before build() has been move-assigned.
   *                            Always wrap in a try-catch when calling from an
   *                            exception handler where the Qairt object may not
   *                            be fully initialized:
   * @code
   *   try { std::cerr << qairt.getLastError(); } catch (...) {}
   * @endcode
   *
   * @note The stored message is overwritten on every failed operation and
   *       cleared (set to empty) on every successful one.
   * @note This call itself goes through the CAPI. If the Qairt handle is
   *       invalid, it will throw rather than return an empty string.
   *
   * Example:
   * @code
   *   try {
   *     qairt.executeGraph(graphName, inputs, outputs);
   *   } catch (const qairt::Exception& e) {
   *     std::cerr << "Execution failed: " << e.what() << "\n";
   *     try {
   *       std::cerr << "Detail: " << qairt.getLastError() << "\n";
   *     } catch (...) {}
   *   }
   * @endcode
   */
  std::string getLastError() const {
    const char* errorMsg = nullptr;
    static_cast<void>(call<&Qairt_V1_t::getLastError>(&errorMsg));
    return errorMsg ? std::string(errorMsg) : std::string();
  }

  /**
   * @brief True model inputs
   * Returns pointers to SystemContextTensorInfo objects
   * For multi-backend: first subnet's inputs only
   */
  std::vector<SystemContextTensorInfo*> getModelInputInfos() const {
    uint32_t n = 0;
    callAndConfirm<&Qairt_V1_t::getNumModelInputInfos>(&n);
    std::vector<SystemContextTensorInfo*> result;
    result.reserve(n);
    for (uint32_t i = 0; i < n; ++i) {
      void* ptr = nullptr;
      callAndConfirm<&Qairt_V1_t::getModelInputInfoAt>(i, &ptr);
      result.push_back(reinterpret_cast<SystemContextTensorInfo*>(ptr));
    }
    return result;
  }

  /**
   * @brief True model outputs
   * * Returns pointers to SystemContextTensorInfo objects
   * For multi-backend: last subnet's outputs only (no transfer tensors). ✓
   */
  std::vector<SystemContextTensorInfo*> getModelOutputInfos() const {
    uint32_t n = 0;
    callAndConfirm<&Qairt_V1_t::getNumModelOutputInfos>(&n);
    std::vector<SystemContextTensorInfo*> result;
    result.reserve(n);
    for (uint32_t i = 0; i < n; ++i) {
      void* ptr = nullptr;
      callAndConfirm<&Qairt_V1_t::getModelOutputInfoAt>(i, &ptr);
      result.push_back(reinterpret_cast<SystemContextTensorInfo*>(ptr));
    }
    return result;
  }

 private:
  /**
   * @brief Private constructor called by QairtSystemBuilder::build()
   * @param apiTable Shared pointer to SystemApiTable
   * @param handle Non-owning handle to Qairt instance
   */
  Qairt(const std::shared_ptr<SystemApiTable>& apiTable, Qairt_Handle_t handle)
      : base_type(apiTable) {
    setHandle(handle);
  }

  void prepareToCross() const {}
  void updateAfterCross() const {}
};

}  // namespace qairt

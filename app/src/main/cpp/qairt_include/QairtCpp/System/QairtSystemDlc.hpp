//==============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All Rights Reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//==============================================================================

/**
 * @file
 * @brief C++ wrapper for the QAIRT System DLC API.
 */

#pragma once

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "../QairtBackend.hpp"
#include "../QairtContext.hpp"
#include "../QairtGraph.hpp"
#include "QairtSystemApi.hpp"
#include "QairtSystemContext.hpp"
#include "QairtSystemLog.hpp"

namespace qairt {

/**
 * @brief Error codes returned by QAIRT System DLC operations.
 *
 * @details
 * | Enumerator          | Description                                              |
 * |---------------------|----------------------------------------------------------|
 * | `MinError`          | Lower sentinel for the error code range.                 |
 * | `NoError`           | Operation succeeded.                                     |
 * | `UnsupportedFeature`| The requested feature is not yet supported.              |
 * | `MemAlloc`          | Memory allocation failure.                               |
 * | `General`           | An unclassified System DLC error occurred.               |
 * | `InvalidHandle`     | The provided System DLC handle is not valid.             |
 * | `InvalidArgument`   | One or more arguments to the operation are NULL/invalid. |
 * | `OperationFailed`   | The System DLC operation could not be completed.         |
 * | `MalformedBinary`   | The DLC binary is malformed or corrupted.                |
 * | `MaxError`          | Upper sentinel for the error code range.                 |
 */
QAIRT_CPPAPI_DEFINE_ENUM(SystemDlcError,
                         QairtSystemDlc_Error_t,
                         MinError           = QAIRT_SYSTEM_DLC_MIN_ERROR,
                         NoError            = QAIRT_SYSTEM_DLC_NO_ERROR,
                         UnsupportedFeature = QAIRT_SYSTEM_DLC_ERROR_UNSUPPORTED_FEATURE,
                         MemAlloc           = QAIRT_SYSTEM_DLC_ERROR_MEM_ALLOC,
                         General            = QAIRT_SYSTEM_DLC_ERROR_GENERAL,
                         InvalidHandle      = QAIRT_SYSTEM_DLC_ERROR_INVALID_HANDLE,
                         InvalidArgument    = QAIRT_SYSTEM_DLC_ERROR_INVALID_ARGUMENT,
                         OperationFailed    = QAIRT_SYSTEM_DLC_ERROR_OPERATION_FAILED,
                         MalformedBinary    = QAIRT_SYSTEM_DLC_ERROR_MALFORMED_BINARY,
                         MaxError           = QAIRT_SYSTEM_DLC_MAX_ERROR)

// Forward declarations
class SystemDlcGraphConfigInfo;
class SystemDlc;

/**
 * @brief Per-graph configuration options for a SystemDlc::composeGraphs() call.
 */
class SystemDlcGraphConfigInfo : public ApiType<SystemDlcGraphConfigInfo,
                                                QairtSystem_Dlc_GraphConfigInfoV1_t,
                                                SystemApiTable> {
  friend SystemApi;
  friend Api;

  /// @brief Manages the graph name string, synchronized with the underlying C API handle.
  detail::crossable<std::string, &interface_type::getGraphName, &interface_type::setGraphName>
      m_graphName;
  /// @brief Manages the graph configuration handle passed to the underlying C API (write-only).
  detail::crossable<detail::set_only<GraphConfiguration>, nullptr, &interface_type::setGraphConfig>
      m_graphConfig;

 public:
  using base_type::base_type;
  using base_type::operator=;

  SystemDlcGraphConfigInfo()                                               = default;
  SystemDlcGraphConfigInfo(const SystemDlcGraphConfigInfo&)                = delete;
  SystemDlcGraphConfigInfo(SystemDlcGraphConfigInfo&&) noexcept            = default;
  SystemDlcGraphConfigInfo& operator=(const SystemDlcGraphConfigInfo&)     = delete;
  SystemDlcGraphConfigInfo& operator=(SystemDlcGraphConfigInfo&&) noexcept = default;

  // Stores the graph config handle set via setGraphConfiguration(). Kept as a raw handle
  // rather than a crossable getter since GraphConfiguration is write-only from the C++ API.
  /// @brief Raw graph configuration handle stored by setGraphConfiguration().
  QairtGraph_ConfigHandle_t m_graphConfigHandle{};

  /**
   * @brief Get the graph name for this configuration entry.
   *
   * @return The configured graph name string.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtSystemDlc_GraphConfigInfo_getGraphName
   */
  std::string& getGraphName() { return m_graphName.get(); }

  /// @see SystemDlcGraphConfigInfo::getGraphName()
  const std::string& getGraphName() const { return m_graphName.get(); }

  /**
   * @brief Set the graph name for this configuration entry.
   *
   * @param[in] graphName  Name of the target graph.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtSystemDlc_GraphConfigInfo_setGraphName
   */
  void setGraphName(const std::string& graphName) { m_graphName = graphName; }

  /**
   * @brief Set the backend-specific graph configuration for this entry.
   *
   * @param[in] graphConfig  Handle to the backend-specific graph configuration.
   *
   * @throws qairt::Exception on invalid handle or invalid argument.
   *
   * @see QairtSystemDlc_GraphConfigInfo_setGraphConfig
   */
  void setGraphConfiguration(const GraphConfiguration& graphConfig) {
    QAIRT_NO_RETURN m_graphConfig.setNonOwning(*this,
                                               detail::QairtTypeAdmin::getHandle(graphConfig));
  }

 private:
  explicit SystemDlcGraphConfigInfo(const std::shared_ptr<SystemApiTable>& apiTable)
      : ApiType(apiTable) {
    construct<&interface_type::create>();
  }

  void prepareToCross() const {
    m_graphName.prepareToCross(static_cast<const SystemDlcGraphConfigInfo&>(*this));
  }

  void updateAfterCross() const { updateMultipleAfterCross(m_graphName); }
};

/**
 * @brief Handle for a QAIRT Deep Learning Container (DLC) model file.
 *
 *        Obtained via `SystemApi::makeShared<SystemDlc>(systemLog, path)`.
 */
class SystemDlc : public ApiType<SystemDlc, QairtSystem_DlcV1_t, SystemApiTable> {
  friend SystemApi;
  friend Api;

 public:
  using base_type::base_type;
  using base_type::operator=;

  ~SystemDlc()                               = default;
  SystemDlc() noexcept                       = default;
  SystemDlc(const SystemDlc&)                = delete;
  SystemDlc(SystemDlc&&) noexcept            = default;
  SystemDlc& operator=(const SystemDlc&)     = delete;
  SystemDlc& operator=(SystemDlc&&) noexcept = default;

  /**
   * @brief Compose graphs from this DLC onto a backend and context.
   *
   * @param[in] graphConfigInfos  Per-graph configuration entries. Optional.
   * @param[in] backend           Backend on which to compose the graphs.
   * @param[in] context           Context on which to compose the graphs.
   * @param[in] backendApi        Backend API object providing the
   *                              QairtInterface_getInterface function pointer.
   * @param[in] systemApi         SystemApi providing the system API table.
   *
   * @return Handle set representing the graphs created on the backend.
   *
   * @throws qairt::Exception on:
   *          - invalid handle
   *          - memory allocation failure
   *          - unsupported feature
   *          - graph composition failure
   *
   * @see QairtSystemDlc_composeGraphs
   */
  SystemContextGraphInfoSet composeGraphs(
      const std::vector<SystemDlcGraphConfigInfo>& graphConfigInfos,
      Backend& backend,
      Context& context,
      const Api& backendApi,
      const SystemApi& systemApi) {
    std::vector<QairtSystemDlc_GraphConfigInfoHandle_t> configHandles(graphConfigInfos.size());

    QAIRT_NO_RETURN std::transform(graphConfigInfos.begin(),
                                   graphConfigInfos.end(),
                                   configHandles.begin(),
                                   [](const SystemDlcGraphConfigInfo& configInfo) {
                                     detail::QairtTypeAdmin::prepareToCross(configInfo);
                                     return getHandle(configInfo);
                                   });

    const QairtSystemDlc_GraphConfigInfoHandle_t* configHandlesPtr =
        configHandles.empty() ? nullptr : configHandles.data();

    // qairtInterface must be the backend's QairtInterface_getInterface function pointer
    const void* qairtInterfacePtr = reinterpret_cast<const void*>(backendApi.m_getInterfaceFn);

    QairtSystem_Context_GraphInfoSetHandle_t graphInfoSetHandle{};

    callAndConfirm<&QairtSystem_DlcV1_t::composeGraphs>(configHandlesPtr,
                                                        static_cast<uint32_t>(configHandles.size()),
                                                        detail::QairtTypeAdmin::getHandle(backend),
                                                        detail::QairtTypeAdmin::getHandle(context),
                                                        qairtInterfacePtr,
                                                        &graphInfoSetHandle);

    return SystemContextGraphInfoSet(systemApi.m_systemApiTable, graphInfoSetHandle);
  }

  /// @see SystemDlc::composeGraphs(const std::vector<SystemDlcGraphConfigInfo>&, Backend&,
  /// Context&, const Api&, const SystemApi&)
  SystemContextGraphInfoSet composeGraphs(Backend& backend,
                                          Context& context,
                                          const Api& backendApi,
                                          const SystemApi& systemApi) {
    return composeGraphs(
        std::vector<SystemDlcGraphConfigInfo>{}, backend, context, backendApi, systemApi);
  }

 private:
  SystemDlc(const std::shared_ptr<SystemApiTable>& apiTable,
            const std::string& dlcPath,
            ApiTypeRef<const SystemLog&> systemLog = {})
      : ApiType(apiTable) {
    construct<&interface_type::createFromFile>(detail::getHandle(systemLog), dlcPath.c_str());
  }

  SystemDlc(const std::shared_ptr<SystemApiTable>& apiTable,
            ApiTypeRef<SystemLog>&& systemLog,
            const std::string& dlcPath)
      : ApiType(apiTable) {
    construct<&interface_type::createFromFile>(detail::getHandle(systemLog), dlcPath.c_str());
  }

  // Private constructor for creating from binary
  SystemDlc(const std::shared_ptr<SystemApiTable>& apiTable,
            const uint8_t* buffer,
            uint64_t bufferSize,
            ApiTypeRef<const SystemLog&> systemLog = {})
      : ApiType(apiTable) {
    construct<&interface_type::createFromBinary>(detail::getHandle(systemLog), buffer, bufferSize);
  }

  SystemDlc(const std::shared_ptr<SystemApiTable>& apiTable,
            ApiTypeRef<const SystemLog&> systemLog,
            const uint8_t* buffer,
            uint64_t bufferSize)
      : ApiType(apiTable) {
    construct<&interface_type::createFromBinary>(detail::getHandle(systemLog), buffer, bufferSize);
  }

  // For use in QairtSystemBuilder
  SystemDlc(const std::shared_ptr<SystemApiTable>& apiTable,
            detail::non_owning_handle<handle_type> existingHandle)
      : ApiType(apiTable) {
    setHandle(existingHandle.m_handle);
    setNonOwning(true);
  }

  void prepareToCross() const {}
  void updateAfterCross() const {}
};

}  // namespace qairt

//=============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//=============================================================================
#pragma once

#include "LPAI/QairtLpaiBackend.h"
#include "LPAI/QairtLpaiContext.h"
#include "LPAI/QairtLpaiGraph.h"
#include "QairtCppApi/QairtApi.hpp"

namespace qairt {

class LpaiBackendExtensionTable : public BackendExtensionTable<LpaiBackendExtensionTable> {
  friend class Api;
  using BackendExtensionTable::BackendExtensionTable;

 public:
  ~LpaiBackendExtensionTable() = default;
  LpaiBackendExtensionTable()  = delete;
  explicit LpaiBackendExtensionTable(const std::shared_ptr<ApiTable>& apiTable)
      : BackendExtensionTable(apiTable) {}
  LpaiBackendExtensionTable(const LpaiBackendExtensionTable&)                = delete;
  LpaiBackendExtensionTable(LpaiBackendExtensionTable&&) noexcept            = default;
  LpaiBackendExtensionTable& operator=(const LpaiBackendExtensionTable&)     = delete;
  LpaiBackendExtensionTable& operator=(LpaiBackendExtensionTable&&) noexcept = default;

  template <typename T>
  static void handle_type_tag(T*);

  template <auto Value>
  struct apitable_p2m {
    static constexpr auto value = Value;
  };
  template <typename T>
  struct p2m_to_api_member : apitable_p2m<nullptr> {};

#define QAIRT_INTERFACE_DECLARE_MEMBER(type, handle, name) \
  static handle handle_type_tag(type*);                    \
  type name;                                               \
  template <typename T>                                    \
  struct p2m_to_api_member<T type::*> : apitable_p2m<&LpaiBackendExtensionTable::name> {};

  QAIRT_INTERFACE_DECLARE_MEMBER(QairtLpaiBackend_HwInfoV1_t,
                                 QairtLpaiBackend_HwInfoHandle_t,
                                 lpaiBackendHwInfo)
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtLpaiBackend_CustomConfigV1_t,
                                 QairtLpaiBackend_CustomConfigHandle_t,
                                 lpaiBackendCustomConfig)
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtLpaiContext_CustomConfigV1_t,
                                 QairtLpaiContext_CustomConfigHandle_t,
                                 lpaiContextCustomConfig)
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtLpaiGraph_MemConfigV1_t,
                                 QairtLpaiGraph_MemConfigHandle_t,
                                 lpaiGraphMemConfig)
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtLpaiGraph_PerfCfgV1_t,
                                 QairtLpaiGraph_PerfCfgHandle_t,
                                 lpaiGraphPerfCfg)
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtLpaiGraph_CoreAffinityV1_t,
                                 QairtLpaiGraph_CoreAffinityHandle_t,
                                 lpaiGraphCoreAffinity)
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtLpaiGraph_PrepareCfgV1_t,
                                 QairtLpaiGraph_PrepareCfgHandle_t,
                                 lpaiGraphPrepareCfg)
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtLpaiGraph_CustomConfigV1_t,
                                 QairtLpaiGraph_CustomConfigHandle_t,
                                 lpaiGraphCustomConfig)

#undef QAIRT_INTERFACE_DECLARE_MEMBER

  template <typename T>
  struct interface_handle_type {
    using handle_type = decltype(handle_type_tag(std::declval<T*>()));
  };

  template <typename T>
  using interface_handle_type_t = typename interface_handle_type<T>::handle_type;

  template <auto P2m>
  struct p2mv_to_api_member : p2m_to_api_member<decltype(P2m)> {};

  template <auto P2m>
  inline static constexpr auto p2mv_to_api_member_v = p2mv_to_api_member<P2m>::value;

  template <auto P2m_InterfaceFn>
  const auto& getMemberInterface() const {
    static_assert(
        detail::ensure_not_nullptr_v<p2m_to_api_member<decltype(P2m_InterfaceFn)>::value>);
    return this->*p2mv_to_api_member_v<P2m_InterfaceFn>;
  }

  static LpaiBackendExtensionTable create(const std::shared_ptr<ApiTable>& apiTable,
                                          decltype(QairtInterface_getInterface) getInterfaceFn) {
    using detail::populateInterface;

    LpaiBackendExtensionTable lbe(apiTable);

    lbe.lpaiBackendHwInfo =
        populateInterface<QAIRT_LPAI_BACKEND_HW_INFO_ID,
                          &QairtLpaiBackend_HwInfoV1_t::create,
                          &QairtLpaiBackend_HwInfoV1_t::free,
                          &QairtLpaiBackend_HwInfoV1_t::setTarget,
                          &QairtLpaiBackend_HwInfoV1_t::getTarget,
                          &QairtLpaiBackend_HwInfoV1_t::setHwVersion,
                          &QairtLpaiBackend_HwInfoV1_t::getHwVersion>(getInterfaceFn);

    lbe.lpaiBackendCustomConfig =
        populateInterface<QAIRT_LPAI_BACKEND_CUSTOM_CONFIG_ID,
                          &QairtLpaiBackend_CustomConfigV1_t::create,
                          &QairtLpaiBackend_CustomConfigV1_t::free,
                          &QairtLpaiBackend_CustomConfigV1_t::setHwInfo,
                          &QairtLpaiBackend_CustomConfigV1_t::getHwInfo>(getInterfaceFn);

    lbe.lpaiContextCustomConfig =
        populateInterface<QAIRT_LPAI_CONTEXT_CUSTOM_CONFIG_ID,
                          &QairtLpaiContext_CustomConfigV1_t::create,
                          &QairtLpaiContext_CustomConfigV1_t::free,
                          &QairtLpaiContext_CustomConfigV1_t::setModelBufferMemType,
                          &QairtLpaiContext_CustomConfigV1_t::getModelBufferMemType,
                          &QairtLpaiContext_CustomConfigV1_t::setEnableIsland,
                          &QairtLpaiContext_CustomConfigV1_t::getEnableIsland>(getInterfaceFn);

    lbe.lpaiGraphMemConfig =
        populateInterface<QAIRT_LPAI_GRAPH_MEM_CONFIG_ID,
                          &QairtLpaiGraph_MemConfigV1_t::create,
                          &QairtLpaiGraph_MemConfigV1_t::free,
                          &QairtLpaiGraph_MemConfigV1_t::setMemType,
                          &QairtLpaiGraph_MemConfigV1_t::getMemType,
                          &QairtLpaiGraph_MemConfigV1_t::setSize,
                          &QairtLpaiGraph_MemConfigV1_t::getSize,
                          &QairtLpaiGraph_MemConfigV1_t::setAddr,
                          &QairtLpaiGraph_MemConfigV1_t::getAddr>(getInterfaceFn);

    lbe.lpaiGraphPerfCfg =
        populateInterface<QAIRT_LPAI_GRAPH_PERF_CFG_ID,
                          &QairtLpaiGraph_PerfCfgV1_t::create,
                          &QairtLpaiGraph_PerfCfgV1_t::free,
                          &QairtLpaiGraph_PerfCfgV1_t::setFps,
                          &QairtLpaiGraph_PerfCfgV1_t::getFps,
                          &QairtLpaiGraph_PerfCfgV1_t::setFtrtRatio,
                          &QairtLpaiGraph_PerfCfgV1_t::getFtrtRatio,
                          &QairtLpaiGraph_PerfCfgV1_t::setClientType,
                          &QairtLpaiGraph_PerfCfgV1_t::getClientType>(getInterfaceFn);

    lbe.lpaiGraphCoreAffinity =
        populateInterface<QAIRT_LPAI_GRAPH_CORE_AFFINITY_ID,
                          &QairtLpaiGraph_CoreAffinityV1_t::create,
                          &QairtLpaiGraph_CoreAffinityV1_t::free,
                          &QairtLpaiGraph_CoreAffinityV1_t::setAffinity,
                          &QairtLpaiGraph_CoreAffinityV1_t::getAffinity,
                          &QairtLpaiGraph_CoreAffinityV1_t::setCoreSelection,
                          &QairtLpaiGraph_CoreAffinityV1_t::getCoreSelection>(getInterfaceFn);

    lbe.lpaiGraphPrepareCfg =
        populateInterface<QAIRT_LPAI_GRAPH_PREPARE_CFG_ID,
                          &QairtLpaiGraph_PrepareCfgV1_t::create,
                          &QairtLpaiGraph_PrepareCfgV1_t::free,
                          &QairtLpaiGraph_PrepareCfgV1_t::setEnablePerLayer,
                          &QairtLpaiGraph_PrepareCfgV1_t::getEnablePerLayer,
                          &QairtLpaiGraph_PrepareCfgV1_t::setEnableCoreSelection,
                          &QairtLpaiGraph_PrepareCfgV1_t::getEnableCoreSelection>(getInterfaceFn);

    lbe.lpaiGraphCustomConfig =
        populateInterface<QAIRT_LPAI_GRAPH_CUSTOM_CONFIG_ID,
                          &QairtLpaiGraph_CustomConfigV1_t::create,
                          &QairtLpaiGraph_CustomConfigV1_t::free,
                          &QairtLpaiGraph_CustomConfigV1_t::setScratchMem,
                          &QairtLpaiGraph_CustomConfigV1_t::getScratchMem,
                          &QairtLpaiGraph_CustomConfigV1_t::setPersistentMem,
                          &QairtLpaiGraph_CustomConfigV1_t::getPersistentMem,
                          &QairtLpaiGraph_CustomConfigV1_t::setPerfCfg,
                          &QairtLpaiGraph_CustomConfigV1_t::getPerfCfg,
                          &QairtLpaiGraph_CustomConfigV1_t::setCoreAffinity,
                          &QairtLpaiGraph_CustomConfigV1_t::getCoreAffinity,
                          &QairtLpaiGraph_CustomConfigV1_t::setPauseExecution,
                          &QairtLpaiGraph_CustomConfigV1_t::setResumeExecution,
                          &QairtLpaiGraph_CustomConfigV1_t::setPrepareCfg,
                          &QairtLpaiGraph_CustomConfigV1_t::getPrepareCfg>(getInterfaceFn);

    return lbe;
  }
};

using LpaiBackendExtension = BackendExtension<LpaiBackendExtensionTable>;
static_assert(detail::is_backend_extension_v<LpaiBackendExtension>);

template <typename T, typename U>
using LpaiApiType = ApiType<T, U, LpaiBackendExtensionTable>;

}  // namespace qairt

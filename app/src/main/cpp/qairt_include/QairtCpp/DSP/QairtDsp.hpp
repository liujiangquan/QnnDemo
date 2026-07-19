//=============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//=============================================================================
#pragma once

#include "QairtCppApi/QairtApi.hpp"
#include "QairtDspBackend.h"
#include "QairtDspContext.h"
#include "QairtDspGraph.h"

namespace qairt {

class DspBackendExtensionTable : public BackendExtensionTable<DspBackendExtensionTable> {
  friend class Api;
  using BackendExtensionTable::BackendExtensionTable;

 public:
  ~DspBackendExtensionTable() = default;
  DspBackendExtensionTable()  = delete;
  explicit DspBackendExtensionTable(const std::shared_ptr<ApiTable>& apiTable)
      : BackendExtensionTable(apiTable) {}
  DspBackendExtensionTable(const DspBackendExtensionTable&)                = delete;
  DspBackendExtensionTable(DspBackendExtensionTable&&) noexcept            = default;
  DspBackendExtensionTable& operator=(const DspBackendExtensionTable&)     = delete;
  DspBackendExtensionTable& operator=(DspBackendExtensionTable&&) noexcept = default;

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
  struct p2m_to_api_member<T type::*> : apitable_p2m<&DspBackendExtensionTable::name> {};

  QAIRT_INTERFACE_DECLARE_MEMBER(QairtDspBackend_CustomConfigV1_t,
                                 QairtDspBackend_CustomConfigHandle_t,
                                 backendCustomConfig);
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtDspContext_CustomConfigV1_t,
                                 QairtDspContext_CustomConfigHandle_t,
                                 contextCustomConfig);
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtDspGraph_CustomConfigV1_t,
                                 QairtDspGraph_CustomConfigHandle_t,
                                 graphCustomConfig);

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

  static DspBackendExtensionTable create(const std::shared_ptr<ApiTable>& apiTable,
                                         decltype(QairtInterface_getInterface) getInterfaceFn) {
    using detail::populateInterface;

    DspBackendExtensionTable dbe(apiTable);

    dbe.backendCustomConfig =
        populateInterface<QAIRT_DSP_BACKEND_CUSTOM_CONFIG_ID,
                          &QairtDspBackend_CustomConfigV1_t::create,
                          &QairtDspBackend_CustomConfigV1_t::free,
                          &QairtDspBackend_CustomConfigV1_t::setFoldReluActivationIntoConvOff,
                          &QairtDspBackend_CustomConfigV1_t::getFoldReluActivationIntoConvOff,
                          &QairtDspBackend_CustomConfigV1_t::setShortDepthConvOnHmxOff,
                          &QairtDspBackend_CustomConfigV1_t::getShortDepthConvOnHmxOff,
                          &QairtDspBackend_CustomConfigV1_t::setUseSignedProcessDomain,
                          &QairtDspBackend_CustomConfigV1_t::getUseSignedProcessDomain,
                          &QairtDspBackend_CustomConfigV1_t::setArch,
                          &QairtDspBackend_CustomConfigV1_t::getArch>(getInterfaceFn);

    dbe.contextCustomConfig =
        populateInterface<QAIRT_DSP_CONTEXT_CUSTOM_CONFIG_ID,
                          &QairtDspContext_CustomConfigV1_t::create,
                          &QairtDspContext_CustomConfigV1_t::free,
                          &QairtDspContext_CustomConfigV1_t::setFileReadMemoryBudget,
                          &QairtDspContext_CustomConfigV1_t::getFileReadMemoryBudget>(
            getInterfaceFn);

    dbe.graphCustomConfig =
        populateInterface<QAIRT_DSP_GRAPH_CUSTOM_CONFIG_ID,
                          &QairtDspGraph_CustomConfigV1_t::create,
                          &QairtDspGraph_CustomConfigV1_t::free,
                          &QairtDspGraph_CustomConfigV1_t::setOptimization,
                          &QairtDspGraph_CustomConfigV1_t::getOptimization,
                          &QairtDspGraph_CustomConfigV1_t::setEncoding,
                          &QairtDspGraph_CustomConfigV1_t::getEncoding,
                          &QairtDspGraph_CustomConfigV1_t::setPriority,
                          &QairtDspGraph_CustomConfigV1_t::getPriority,
                          &QairtDspGraph_CustomConfigV1_t::setPrecision,
                          &QairtDspGraph_CustomConfigV1_t::getPrecision>(getInterfaceFn);

    return dbe;
  }
};

using DspBackendExtension = BackendExtension<DspBackendExtensionTable>;
static_assert(detail::is_backend_extension_v<DspBackendExtension>);

template <typename T, typename U>
using DspApiType = ApiType<T, U, DspBackendExtensionTable>;

}  // namespace qairt

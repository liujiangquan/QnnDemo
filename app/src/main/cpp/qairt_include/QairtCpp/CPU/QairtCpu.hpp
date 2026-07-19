//==============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//==============================================================================
#pragma once

#include "CPU/QairtCpuContext.h"
#include "CPU/QairtCpuGraph.h"
#include "QairtCppApi/QairtApi.hpp"

namespace qairt {

class CpuBackendExtensionTable : public BackendExtensionTable<CpuBackendExtensionTable> {
  friend class Api;
  using BackendExtensionTable::BackendExtensionTable;

 public:
  ~CpuBackendExtensionTable() = default;
  CpuBackendExtensionTable()  = delete;
  explicit CpuBackendExtensionTable(const std::shared_ptr<ApiTable>& apiTable)
      : BackendExtensionTable(apiTable) {}
  CpuBackendExtensionTable(const CpuBackendExtensionTable&)                = delete;
  CpuBackendExtensionTable(CpuBackendExtensionTable&&) noexcept            = default;
  CpuBackendExtensionTable& operator=(const CpuBackendExtensionTable&)     = delete;
  CpuBackendExtensionTable& operator=(CpuBackendExtensionTable&&) noexcept = default;

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
  struct p2m_to_api_member<T type::*> : apitable_p2m<&CpuBackendExtensionTable::name> {};

  QAIRT_INTERFACE_DECLARE_MEMBER(QairtCpuContext_CustomConfigV1_t,
                                 QairtCpuContext_CustomConfigHandle_t,
                                 cpuContextCustomConfig);
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtCpuGraph_CustomConfigV1_t,
                                 QairtCpuGraph_CustomConfigHandle_t,
                                 cpuGraphCustomConfig);

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

  static CpuBackendExtensionTable create(const std::shared_ptr<ApiTable>& apiTable,
                                         decltype(QairtInterface_getInterface) getInterfaceFn) {
    using detail::populateInterface;

    CpuBackendExtensionTable cbe(apiTable);

    cbe.cpuContextCustomConfig =
        populateInterface<QAIRT_CPU_CONTEXT_CUSTOM_CONFIG_ID,
                          &QairtCpuContext_CustomConfigV1_t::create,
                          &QairtCpuContext_CustomConfigV1_t::free,
                          &QairtCpuContext_CustomConfigV1_t::setUseQmx,
                          &QairtCpuContext_CustomConfigV1_t::getUseQmx>(getInterfaceFn);

    cbe.cpuGraphCustomConfig =
        populateInterface<QAIRT_CPU_GRAPH_CUSTOM_CONFIG_ID,
                          &QairtCpuGraph_CustomConfigV1_t::create,
                          &QairtCpuGraph_CustomConfigV1_t::free,
                          &QairtCpuGraph_CustomConfigV1_t::setUseQmx,
                          &QairtCpuGraph_CustomConfigV1_t::getUseQmx>(getInterfaceFn);

    return cbe;
  }
};

using CpuBackendExtension = BackendExtension<CpuBackendExtensionTable>;
static_assert(detail::is_backend_extension_v<CpuBackendExtension>);

template <typename T, typename U>
using CpuApiType = ApiType<T, U, CpuBackendExtensionTable>;

}  // namespace qairt

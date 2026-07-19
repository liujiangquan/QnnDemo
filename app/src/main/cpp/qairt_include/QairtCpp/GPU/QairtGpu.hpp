//=============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//=============================================================================
#pragma once

#include "GPU/QairtGpuBackend.h"
#include "GPU/QairtGpuContext.h"
#include "GPU/QairtGpuGraph.h"
#include "QairtCppApi/QairtApi.hpp"

namespace qairt {

class GpuBackendExtensionTable : public BackendExtensionTable<GpuBackendExtensionTable> {
  friend class Api;
  using BackendExtensionTable::BackendExtensionTable;

 public:
  ~GpuBackendExtensionTable() = default;
  GpuBackendExtensionTable()  = delete;
  explicit GpuBackendExtensionTable(const std::shared_ptr<ApiTable>& apiTable)
      : BackendExtensionTable(apiTable) {}
  GpuBackendExtensionTable(const GpuBackendExtensionTable&)                = delete;
  GpuBackendExtensionTable(GpuBackendExtensionTable&&) noexcept            = default;
  GpuBackendExtensionTable& operator=(const GpuBackendExtensionTable&)     = delete;
  GpuBackendExtensionTable& operator=(GpuBackendExtensionTable&&) noexcept = default;

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
  struct p2m_to_api_member<T type::*> : apitable_p2m<&GpuBackendExtensionTable::name> {};

  QAIRT_INTERFACE_DECLARE_MEMBER(QairtGpuContext_CustomConfigV1_t,
                                 QairtGpuContext_CustomConfigHandle_t,
                                 gpuContextCustomConfig);
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtGpuBackend_CustomConfigV1_t,
                                 QairtGpuBackend_CustomConfigHandle_t,
                                 gpuBackendCustomConfig);
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtGpuGraph_FenceConfigV1_t,
                                 QairtGpuGraph_FenceConfigHandle_t,
                                 gpuGraphFenceConfig);
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtGpuGraph_CustomConfigV1_t,
                                 QairtGpuGraph_CustomConfigHandle_t,
                                 gpuGraphCustomConfig);

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

  static GpuBackendExtensionTable create(const std::shared_ptr<ApiTable>& apiTable,
                                         decltype(QairtInterface_getInterface) getInterfaceFn) {
    using detail::populateInterface;

    GpuBackendExtensionTable gbe(apiTable);

    gbe.gpuContextCustomConfig =
        populateInterface<QAIRT_GPU_CONTEXT_CUSTOM_CONFIG_ID,
                          &QairtGpuContext_CustomConfigV1_t::create,
                          &QairtGpuContext_CustomConfigV1_t::free,
                          &QairtGpuContext_CustomConfigV1_t::setPerfHint,
                          &QairtGpuContext_CustomConfigV1_t::getPerfHint,
                          &QairtGpuContext_CustomConfigV1_t::setUseGLBuffers,
                          &QairtGpuContext_CustomConfigV1_t::getUseGLBuffers,
                          &QairtGpuContext_CustomConfigV1_t::setKernelRepoDir,
                          &QairtGpuContext_CustomConfigV1_t::getKernelRepoDir,
                          &QairtGpuContext_CustomConfigV1_t::setInvalidateKernelRepo,
                          &QairtGpuContext_CustomConfigV1_t::getInvalidateKernelRepo>(
            getInterfaceFn);

    gbe.gpuBackendCustomConfig =
        populateInterface<QAIRT_GPU_BACKEND_CUSTOM_CONFIG_ID,
                          &QairtGpuBackend_CustomConfigV1_t::create,
                          &QairtGpuBackend_CustomConfigV1_t::free,
                          &QairtGpuBackend_CustomConfigV1_t::setEnableTuningMode,
                          &QairtGpuBackend_CustomConfigV1_t::getEnableTuningMode,
                          &QairtGpuBackend_CustomConfigV1_t::setPerformanceCacheDir,
                          &QairtGpuBackend_CustomConfigV1_t::getPerformanceCacheDir,
                          &QairtGpuBackend_CustomConfigV1_t::setInvalidatePerformanceCache,
                          &QairtGpuBackend_CustomConfigV1_t::getInvalidatePerformanceCache,
                          &QairtGpuBackend_CustomConfigV1_t::setWeightSharingEnabled,
                          &QairtGpuBackend_CustomConfigV1_t::getWeightSharingEnabled,
                          &QairtGpuBackend_CustomConfigV1_t::setDisableKernelProfiling,
                          &QairtGpuBackend_CustomConfigV1_t::getDisableKernelProfiling>(
            getInterfaceFn);

    gbe.gpuGraphFenceConfig =
        populateInterface<QAIRT_GPU_GRAPH_FENCE_CONFIG_ID,
                          &QairtGpuGraph_FenceConfigV1_t::create,
                          &QairtGpuGraph_FenceConfigV1_t::free,
                          &QairtGpuGraph_FenceConfigV1_t::setNumInputFences,
                          &QairtGpuGraph_FenceConfigV1_t::getNumInputFences,
                          &QairtGpuGraph_FenceConfigV1_t::setInputFenceHandles,
                          &QairtGpuGraph_FenceConfigV1_t::getInputFenceHandles,
                          &QairtGpuGraph_FenceConfigV1_t::setEnableOutputFence,
                          &QairtGpuGraph_FenceConfigV1_t::getEnableOutputFence,
                          &QairtGpuGraph_FenceConfigV1_t::setOutputFence,
                          &QairtGpuGraph_FenceConfigV1_t::getOutputFence>(getInterfaceFn);

    gbe.gpuGraphCustomConfig =
        populateInterface<QAIRT_GPU_GRAPH_CUSTOM_CONFIG_ID,
                          &QairtGpuGraph_CustomConfigV1_t::create,
                          &QairtGpuGraph_CustomConfigV1_t::free,
                          &QairtGpuGraph_CustomConfigV1_t::setPrecision,
                          &QairtGpuGraph_CustomConfigV1_t::getPrecision,
                          &QairtGpuGraph_CustomConfigV1_t::setDisableMemoryOptimizations,
                          &QairtGpuGraph_CustomConfigV1_t::getDisableMemoryOptimizations,
                          &QairtGpuGraph_CustomConfigV1_t::setDisableNodeOptimizations,
                          &QairtGpuGraph_CustomConfigV1_t::getDisableNodeOptimizations,
                          &QairtGpuGraph_CustomConfigV1_t::setDisableQueueRecording,
                          &QairtGpuGraph_CustomConfigV1_t::getDisableQueueRecording,
                          &QairtGpuGraph_CustomConfigV1_t::setFenceConfig,
                          &QairtGpuGraph_CustomConfigV1_t::getFenceConfig>(getInterfaceFn);

    return gbe;
  }
};

using GpuBackendExtension = BackendExtension<GpuBackendExtensionTable>;
static_assert(detail::is_backend_extension_v<GpuBackendExtension>);

template <typename T, typename U>
using GpuApiType = ApiType<T, U, GpuBackendExtensionTable>;

}  // namespace qairt

//=============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//=============================================================================
#pragma once

#include "QairtCppApi/QairtApi.hpp"
#include "QairtHtpContext.h"
#include "QairtHtpDevice.h"
#include "QairtHtpGraph.h"

namespace qairt {

class HtpBackendExtensionTable : public BackendExtensionTable<HtpBackendExtensionTable> {
  friend class Api;
  using BackendExtensionTable::BackendExtensionTable;

 public:
  ~HtpBackendExtensionTable() = default;
  HtpBackendExtensionTable()  = delete;
  explicit HtpBackendExtensionTable(const std::shared_ptr<ApiTable>& apiTable)
      : BackendExtensionTable(apiTable) {}
  HtpBackendExtensionTable(const HtpBackendExtensionTable&)                = delete;
  HtpBackendExtensionTable(HtpBackendExtensionTable&&) noexcept            = default;
  HtpBackendExtensionTable& operator=(const HtpBackendExtensionTable&)     = delete;
  HtpBackendExtensionTable& operator=(HtpBackendExtensionTable&&) noexcept = default;

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
  struct p2m_to_api_member<T type::*> : apitable_p2m<&HtpBackendExtensionTable::name> {};

  QAIRT_INTERFACE_DECLARE_MEMBER(QairtHtpContext_GroupRegistrationV1_t,
                                 QairtHtpContext_GroupRegistrationHandle_t,
                                 groupRegistration);
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtHtpContext_CustomConfigV1_t,
                                 QairtHtpContext_CustomConfigHandle_t,
                                 customConfig);
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtHtpGraph_CustomConfigV1_t,
                                 QairtHtpGraph_CustomConfigHandle_t,
                                 graphCustomConfig);
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtHtpGraph_OptimizationOptionV1_t,
                                 QairtHtpGraph_OptimizationOptionHandle_t,
                                 graphOptimizationOption);
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtHtp_HmxBoundingInfoV1_t,
                                 QairtHtp_HmxBoundingInfoHandle_t,
                                 hmxBoundingInfo);
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtHtpDevice_CustomConfigV1_t,
                                 QairtHtpDevice_CustomConfigHandle_t,
                                 deviceCustomConfig);

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

  static HtpBackendExtensionTable create(const std::shared_ptr<ApiTable>& apiTable,
                                         decltype(QairtInterface_getInterface) getInterfaceFn) {
    using detail::populateInterface;

    HtpBackendExtensionTable hbe(apiTable);
    hbe.groupRegistration =
        populateInterface<QAIRT_HTP_CONTEXT_GROUP_REGISTRATION_ID,
                          &QairtHtpContext_GroupRegistrationV1_t::create,
                          &QairtHtpContext_GroupRegistrationV1_t::free,
                          &QairtHtpContext_GroupRegistrationV1_t::setFirstGroupHandle,
                          &QairtHtpContext_GroupRegistrationV1_t::getFirstGroupHandle,
                          &QairtHtpContext_GroupRegistrationV1_t::setMaxSpillFill,
                          &QairtHtpContext_GroupRegistrationV1_t::getMaxSpillFill>(getInterfaceFn);

    hbe.customConfig =
        populateInterface<QAIRT_HTP_CONTEXT_CUSTOM_CONFIG_ID,
                          &QairtHtpContext_CustomConfigV1_t::create,
                          &QairtHtpContext_CustomConfigV1_t::free,
                          &QairtHtpContext_CustomConfigV1_t::setEnableWeightSharing,
                          &QairtHtpContext_CustomConfigV1_t::getEnableWeightSharing,
                          &QairtHtpContext_CustomConfigV1_t::setGroupRegistration,
                          &QairtHtpContext_CustomConfigV1_t::getGroupRegistration,
                          &QairtHtpContext_CustomConfigV1_t::setFileReadMemoryBudget,
                          &QairtHtpContext_CustomConfigV1_t::getFileReadMemoryBudget,
                          &QairtHtpContext_CustomConfigV1_t::setEnableDspMemoryProfiling,
                          &QairtHtpContext_CustomConfigV1_t::getEnableDspMemoryProfiling,
                          &QairtHtpContext_CustomConfigV1_t::setShareResources,
                          &QairtHtpContext_CustomConfigV1_t::getShareResources,
                          &QairtHtpContext_CustomConfigV1_t::setIoMemEstimation,
                          &QairtHtpContext_CustomConfigV1_t::getIoMemEstimation,
                          &QairtHtpContext_CustomConfigV1_t::setIsPrepareOnly,
                          &QairtHtpContext_CustomConfigV1_t::getIsPrepareOnly,
                          &QairtHtpContext_CustomConfigV1_t::setInitAcceleration,
                          &QairtHtpContext_CustomConfigV1_t::getInitAcceleration,
                          &QairtHtpContext_CustomConfigV1_t::setSkipBinarySectionValidation,
                          &QairtHtpContext_CustomConfigV1_t::getSkipBinarySectionValidation,
                          &QairtHtpContext_CustomConfigV1_t::setShareResourcesOptimizationType,
                          &QairtHtpContext_CustomConfigV1_t::getShareResourcesOptimizationType,
                          &QairtHtpContext_CustomConfigV1_t::setUseExtendedUdma,
                          &QairtHtpContext_CustomConfigV1_t::getUseExtendedUdma,
                          &QairtHtpContext_CustomConfigV1_t::setConcurrentGroupRegistration,
                          &QairtHtpContext_CustomConfigV1_t::getConcurrentGroupRegistration,
                          &QairtHtpContext_CustomConfigV1_t::setLoraWeightSharingEnabled,
                          &QairtHtpContext_CustomConfigV1_t::getLoraWeightSharingEnabled>(
            getInterfaceFn);

    hbe.graphCustomConfig =
        populateInterface<QAIRT_HTP_GRAPH_CUSTOM_CONFIG_ID,
                          &QairtHtpGraph_CustomConfigV1_t::create,
                          &QairtHtpGraph_CustomConfigV1_t::free,
                          &QairtHtpGraph_CustomConfigV1_t::setOptimizationOption,
                          &QairtHtpGraph_CustomConfigV1_t::getOptimizationOption,
                          &QairtHtpGraph_CustomConfigV1_t::setPrecision,
                          &QairtHtpGraph_CustomConfigV1_t::getPrecision,
                          &QairtHtpGraph_CustomConfigV1_t::setVtcmSizeInMB,
                          &QairtHtpGraph_CustomConfigV1_t::getVtcmSizeInMB,
                          &QairtHtpGraph_CustomConfigV1_t::setVtcmSizeInBytes,
                          &QairtHtpGraph_CustomConfigV1_t::getVtcmSizeInBytes,
                          &QairtHtpGraph_CustomConfigV1_t::setFoldReluActivationIntoConvOff,
                          &QairtHtpGraph_CustomConfigV1_t::getFoldReluActivationIntoConvOff,
                          &QairtHtpGraph_CustomConfigV1_t::setShortDepthConvOnHmxOff,
                          &QairtHtpGraph_CustomConfigV1_t::getShortDepthConvOnHmxOff,
                          &QairtHtpGraph_CustomConfigV1_t::setNumHvxThreads,
                          &QairtHtpGraph_CustomConfigV1_t::getNumHvxThreads,
                          &QairtHtpGraph_CustomConfigV1_t::setNumCores,
                          &QairtHtpGraph_CustomConfigV1_t::getNumCores,
                          &QairtHtpGraph_CustomConfigV1_t::setHmxBoundingInfo,
                          &QairtHtpGraph_CustomConfigV1_t::getHmxBoundingInfo,
                          &QairtHtpGraph_CustomConfigV1_t::setWeightsPacking,
                          &QairtHtpGraph_CustomConfigV1_t::getWeightsPacking,
                          &QairtHtpGraph_CustomConfigV1_t::setAssumeSameQuant,
                          &QairtHtpGraph_CustomConfigV1_t::getAssumeSameQuant,
                          &QairtHtpGraph_CustomConfigV1_t::setShareIOBuffer,
                          &QairtHtpGraph_CustomConfigV1_t::getShareIOBuffer,
                          &QairtHtpGraph_CustomConfigV1_t::setAdvancedActivationFusion,
                          &QairtHtpGraph_CustomConfigV1_t::getAdvancedActivationFusion,
                          &QairtHtpGraph_CustomConfigV1_t::setHighPrecisionSigmoid,
                          &QairtHtpGraph_CustomConfigV1_t::getHighPrecisionSigmoid,
                          &QairtHtpGraph_CustomConfigV1_t::setMonolithicLstm,
                          &QairtHtpGraph_CustomConfigV1_t::getMonolithicLstm>(getInterfaceFn);

    hbe.graphOptimizationOption =
        populateInterface<QAIRT_HTP_GRAPH_OPTIMIZATION_OPTION_ID,
                          &QairtHtpGraph_OptimizationOptionV1_t::create,
                          &QairtHtpGraph_OptimizationOptionV1_t::free,
                          &QairtHtpGraph_OptimizationOptionV1_t::setType,
                          &QairtHtpGraph_OptimizationOptionV1_t::getType,
                          &QairtHtpGraph_OptimizationOptionV1_t::setFloatValue,
                          &QairtHtpGraph_OptimizationOptionV1_t::getFloatValue>(getInterfaceFn);

    hbe.hmxBoundingInfo =
        populateInterface<QAIRT_HTP_GRAPH_HMX_BOUNDING_INFO_ID,
                          &QairtHtp_HmxBoundingInfoV1_t::create,
                          &QairtHtp_HmxBoundingInfoV1_t::free,
                          &QairtHtp_HmxBoundingInfoV1_t::setTargetHmxFreqHz,
                          &QairtHtp_HmxBoundingInfoV1_t::getTargetHmxFreqHz,
                          &QairtHtp_HmxBoundingInfoV1_t::setTargetDspCoreFreq,
                          &QairtHtp_HmxBoundingInfoV1_t::getTargetDspCoreFreq>(getInterfaceFn);

    hbe.deviceCustomConfig =
        populateInterface<QAIRT_HTP_DEVICE_CUSTOM_CONFIG_ID,
                          &QairtHtpDevice_CustomConfigV1_t::create,
                          &QairtHtpDevice_CustomConfigV1_t::free,
                          &QairtHtpDevice_CustomConfigV1_t::setSocModel,
                          &QairtHtpDevice_CustomConfigV1_t::getSocModel,
                          &QairtHtpDevice_CustomConfigV1_t::setArch,
                          &QairtHtpDevice_CustomConfigV1_t::getArch,
                          &QairtHtpDevice_CustomConfigV1_t::setUseSignedProcessDomain,
                          &QairtHtpDevice_CustomConfigV1_t::getUseSignedProcessDomain,
                          &QairtHtpDevice_CustomConfigV1_t::setUseSecureProcessDomain,
                          &QairtHtpDevice_CustomConfigV1_t::getUseSecureProcessDomain>(
            getInterfaceFn);

    return hbe;
  }
};

using HtpBackendExtension = BackendExtension<HtpBackendExtensionTable>;
static_assert(detail::is_backend_extension_v<HtpBackendExtension>);

template <typename T, typename U>
using HtpApiType = ApiType<T, U, HtpBackendExtensionTable>;

}  // namespace qairt

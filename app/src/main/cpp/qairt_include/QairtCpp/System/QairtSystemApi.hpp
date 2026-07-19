//==============================================================================
//
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//==============================================================================

#pragma once

/**
 * @file
 * @brief C++ API for loading the QAIRT System library and accessing its interfaces.
 */

#include "../QairtApi.hpp"
#include "QairtSystem/Qairt.h"
#include "QairtSystem/QairtSystemBuilder.h"
#include "QairtSystem/QairtSystemCommon.h"
#include "QairtSystem/QairtSystemContext.h"
#include "QairtSystem/QairtSystemDlc.h"
#include "QairtSystem/QairtSystemLog.h"
#include "QairtSystem/QairtSystemProfile.h"

namespace qairt {

/**
 * @brief Interface table for the QAIRT System library.
 *
 *        Populated by SystemApi on construction via QairtSystemInterface_getInterface.
 *        All system-level C++ wrappers (SystemContext, SystemDlc, SystemLog,
 *        QairtSystemBuilder, Qairt) dispatch their C API calls through this table.
 *
 * @note Instances are not constructed directly; obtain one via SystemApi.
 */
struct SystemApiTable {
  /// @brief Shared ownership handle for the dynamically loaded system library.
  std::shared_ptr<DlHandleHolder> m_dlHandle;

  template <typename T>
  static void handle_type_tag(T*);

  template <auto Value>
  struct apitable_p2m {
    static constexpr auto value = Value;
  };
  template <typename T>
  struct p2m_to_api_member : apitable_p2m<nullptr> {};

#define QAIRT_SYSTEM_INTERFACE_DECLARE_MEMBER(type, handle, name) \
  static handle handle_type_tag(type*);                           \
  type name;                                                      \
  template <typename T>                                           \
  struct p2m_to_api_member<T type::*> : apitable_p2m<&SystemApiTable::name> {};

  QAIRT_SYSTEM_INTERFACE_DECLARE_MEMBER(QairtSystem_Context_QuantParamInfoV1_t,
                                        QairtSystem_Context_QuantInfoHandle_t,
                                        sysCtxQuantInfo)
  QAIRT_SYSTEM_INTERFACE_DECLARE_MEMBER(QairtSystem_Context_QuantizeParams_ScaleOffsetV1_t,
                                        QairtSystem_Context_QuantizeParams_ScaleOffsetHandle_t,
                                        sysCtxQuantizeParamsScaleOffset)
  QAIRT_SYSTEM_INTERFACE_DECLARE_MEMBER(QairtSystem_Context_QuantizeParams_AxisScaleOffsetV1_t,
                                        QairtSystem_Context_QuantizeParams_AxisScaleOffsetHandle_t,
                                        sysCtxQuantizeParamsAxisScaleOffset)
  QAIRT_SYSTEM_INTERFACE_DECLARE_MEMBER(QairtSystem_Context_QuantizeParams_BwScaleOffsetV1_t,
                                        QairtSystem_Context_QuantizeParams_BwScaleOffsetHandle_t,
                                        sysCtxQuantizeParamsBwScaleOffset)
  QAIRT_SYSTEM_INTERFACE_DECLARE_MEMBER(
      QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetV1_t,
      QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetHandle_t,
      sysCtxQuantizeParamsBwAxisScaleOffset)
  QAIRT_SYSTEM_INTERFACE_DECLARE_MEMBER(
      QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMappedV1_t,
      QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMappedHandle_t,
      sysCtxQuantizeParamsBwAxisScaleOffsetMapped)

  QAIRT_SYSTEM_INTERFACE_DECLARE_MEMBER(QairtSystem_Context_TensorInfoV1_t,
                                        QairtSystem_Context_TensorInfoHandle_t,
                                        sysCtxTensorInfo)
  QAIRT_SYSTEM_INTERFACE_DECLARE_MEMBER(QairtSystem_Context_GraphInfoV1_t,
                                        QairtSystem_Context_GraphInfoHandle_t,
                                        sysCtxGraphInfo)
  QAIRT_SYSTEM_INTERFACE_DECLARE_MEMBER(QairtSystem_Context_GraphInfoSetV1_t,
                                        QairtSystem_Context_GraphInfoSetHandle_t,
                                        sysCtxGraphInfoSet)
  QAIRT_SYSTEM_INTERFACE_DECLARE_MEMBER(QairtSystem_Context_BinaryInfoV1_t,
                                        QairtSystem_Context_BinaryInfoHandle_t,
                                        sysCtxBinaryInfo)
  QAIRT_SYSTEM_INTERFACE_DECLARE_MEMBER(QairtSystem_ContextV1_t,
                                        QairtSystem_Context_Handle_t,
                                        sysCtx)
  QAIRT_SYSTEM_INTERFACE_DECLARE_MEMBER(QairtSystem_DlcV1_t, QairtSystemDlc_Handle_t, sysDlc)
  QAIRT_SYSTEM_INTERFACE_DECLARE_MEMBER(QairtSystem_Dlc_GraphConfigInfoV1_t,
                                        QairtSystemDlc_GraphConfigInfoHandle_t,
                                        sysDlcGraphConfigInfo)
  QAIRT_SYSTEM_INTERFACE_DECLARE_MEMBER(QairtSystemLog_V1_t, QairtSystemLog_Handle_t, sysLog)
  QAIRT_SYSTEM_INTERFACE_DECLARE_MEMBER(QairtSystem_Profile_HeaderV1_t,
                                        QairtSystem_Profile_HeaderHandle_t,
                                        sysProfileHeader)
  QAIRT_SYSTEM_INTERFACE_DECLARE_MEMBER(QairtSystem_Profile_EventV1_t,
                                        QairtSystem_Profile_EventHandle_t,
                                        sysProfileEvent)
  QAIRT_SYSTEM_INTERFACE_DECLARE_MEMBER(QairtSystem_Profile_ProfileDataV1_t,
                                        QairtSystem_Profile_ProfileDataHandle_t,
                                        sysProfileData)
  QAIRT_SYSTEM_INTERFACE_DECLARE_MEMBER(QairtSystem_Profile_DataV1_t,
                                        QairtSystem_Profile_DataHandle_t,
                                        sysProfileDataVersion)
  QAIRT_SYSTEM_INTERFACE_DECLARE_MEMBER(QairtSystem_Profile_SerializationFileHeaderV1_t,
                                        QairtSystem_Profile_SerializationFileHeaderHandle_t,
                                        sysProfileFileHeader)
  QAIRT_SYSTEM_INTERFACE_DECLARE_MEMBER(
      QairtSystem_Profile_SerializationTargetConfigurationV1_t,
      QairtSystem_Profile_SerializationTargetConfigurationHandle_t,
      sysProfileTargetConfig)
  QAIRT_SYSTEM_INTERFACE_DECLARE_MEMBER(QairtSystem_Profile_SerializationTargetFileV1_t,
                                        QairtSystem_Profile_SerializationTargetFileHandle_t,
                                        sysProfileTargetFile)
  QAIRT_SYSTEM_INTERFACE_DECLARE_MEMBER(QairtSystem_Profile_SerializationTargetInfoV1_t,
                                        QairtSystem_Profile_SerializationTargetInfoHandle_t,
                                        sysProfileTargetInfo)
  QAIRT_SYSTEM_INTERFACE_DECLARE_MEMBER(QairtSystem_Profile_SerializationTargetV1_t,
                                        QairtSystem_Profile_SerializationTargetHandle_t,
                                        sysProfileSerializationTarget)
  QAIRT_SYSTEM_INTERFACE_DECLARE_MEMBER(QairtSystemBuilder_V1_t,
                                        QairtSystemBuilder_Handle_t,
                                        sysBuilder)
  QAIRT_SYSTEM_INTERFACE_DECLARE_MEMBER(Qairt_V1_t, Qairt_Handle_t, qairt)

#undef QAIRT_SYSTEM_INTERFACE_DECLARE_MEMBER

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

  static SystemApiTable create(void* dlHandle,
                               decltype(QairtSystemInterface_getInterface) getInterfaceFn) {
    using detail::populateInterface;
    SystemApiTable api;
    api.m_dlHandle = std::make_shared<DlHandleHolder>(dlHandle);

    api.sysCtxQuantInfo =
        populateInterface<QAIRT_SYSTEM_CONTEXT_QUANT_INFO_V1_ID,
                          &QairtSystem_Context_QuantParamInfoV1_t::getQuantizationType,
                          &QairtSystem_Context_QuantParamInfoV1_t::getScaleOffset,
                          &QairtSystem_Context_QuantParamInfoV1_t::getAxisScaleOffset,
                          &QairtSystem_Context_QuantParamInfoV1_t::getBwScaleOffset,
                          &QairtSystem_Context_QuantParamInfoV1_t::getBwAxisScaleOffset,
                          &QairtSystem_Context_QuantParamInfoV1_t::getBwAxisScaleOffsetMapped>(
            getInterfaceFn);

    api.sysCtxQuantizeParamsScaleOffset =
        populateInterface<QAIRT_SYSTEM_CONTEXT_QUANTIZE_PARAMS_SCALE_OFFSET_V1_ID,
                          &QairtSystem_Context_QuantizeParams_ScaleOffsetV1_t::create,
                          &QairtSystem_Context_QuantizeParams_ScaleOffsetV1_t::free,
                          &QairtSystem_Context_QuantizeParams_ScaleOffsetV1_t::getScale,
                          &QairtSystem_Context_QuantizeParams_ScaleOffsetV1_t::getOffset>(
            getInterfaceFn);

    api.sysCtxQuantizeParamsAxisScaleOffset = populateInterface<
        QAIRT_SYSTEM_CONTEXT_QUANTIZE_PARAMS_AXIS_SCALE_OFFSET_V1_ID,
        &QairtSystem_Context_QuantizeParams_AxisScaleOffsetV1_t::create,
        &QairtSystem_Context_QuantizeParams_AxisScaleOffsetV1_t::free,
        &QairtSystem_Context_QuantizeParams_AxisScaleOffsetV1_t::getNumScaleOffsets,
        &QairtSystem_Context_QuantizeParams_AxisScaleOffsetV1_t::getScaleOffsetAt,
        &QairtSystem_Context_QuantizeParams_AxisScaleOffsetV1_t::getAxis>(getInterfaceFn);

    api.sysCtxQuantizeParamsBwScaleOffset =
        populateInterface<QAIRT_SYSTEM_CONTEXT_QUANTIZE_PARAMS_BW_SCALE_OFFSET_V1_ID,
                          &QairtSystem_Context_QuantizeParams_BwScaleOffsetV1_t::create,
                          &QairtSystem_Context_QuantizeParams_BwScaleOffsetV1_t::free,
                          &QairtSystem_Context_QuantizeParams_BwScaleOffsetV1_t::getBw,
                          &QairtSystem_Context_QuantizeParams_BwScaleOffsetV1_t::getScale,
                          &QairtSystem_Context_QuantizeParams_BwScaleOffsetV1_t::getOffset>(
            getInterfaceFn);

    api.sysCtxQuantizeParamsBwAxisScaleOffset =
        populateInterface<QAIRT_SYSTEM_CONTEXT_QUANTIZE_PARAMS_BW_AXIS_SCALE_OFFSET_V1_ID,
                          &QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetV1_t::create,
                          &QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetV1_t::free,
                          &QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetV1_t::getBw,
                          &QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetV1_t::getNumOffsets,
                          &QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetV1_t::getOffsetAt,
                          &QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetV1_t::getNumScales,
                          &QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetV1_t::getScaleAt,
                          &QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetV1_t::getAxis>(
            getInterfaceFn);

    api.sysCtxQuantizeParamsBwAxisScaleOffsetMapped = populateInterface<
        QAIRT_SYSTEM_CONTEXT_QUANTIZE_PARAMS_BW_AXIS_SCALE_OFFSET_MAPPED_V1_ID,
        &QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMappedV1_t::create,
        &QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMappedV1_t::free,
        &QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMappedV1_t::getBw,
        &QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMappedV1_t::getNumOffsets,
        &QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMappedV1_t::getOffsetAt,
        &QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMappedV1_t::getNumScales,
        &QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMappedV1_t::getScaleAt,
        &QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMappedV1_t::getAxis,
        &QairtSystem_Context_QuantizeParams_BwAxisScaleOffsetMappedV1_t::getMapping>(
        getInterfaceFn);

    api.sysCtxTensorInfo =
        populateInterface<QAIRT_SYSTEM_CONTEXT_TENSOR_INFO_V1_ID,
                          &QairtSystem_Context_TensorInfoV1_t::getId,
                          &QairtSystem_Context_TensorInfoV1_t::getName,
                          &QairtSystem_Context_TensorInfoV1_t::getRank,
                          &QairtSystem_Context_TensorInfoV1_t::getDimensions,
                          &QairtSystem_Context_TensorInfoV1_t::getDatatype,
                          &QairtSystem_Context_TensorInfoV1_t::getQuantInfo>(getInterfaceFn);
    api.sysCtxGraphInfo =
        populateInterface<QAIRT_SYSTEM_CONTEXT_GRAPH_INFO_V1_ID,
                          &QairtSystem_Context_GraphInfoV1_t::getName,
                          &QairtSystem_Context_GraphInfoV1_t::getNumInputs,
                          &QairtSystem_Context_GraphInfoV1_t::getInputAt,
                          &QairtSystem_Context_GraphInfoV1_t::getNumOutputs,
                          &QairtSystem_Context_GraphInfoV1_t::getOutputAt,
                          &QairtSystem_Context_GraphInfoV1_t::getNumUpdateables,
                          &QairtSystem_Context_GraphInfoV1_t::getUpdateableAt,
                          &QairtSystem_Context_GraphInfoV1_t::getGraphInfoBlobSize,
                          &QairtSystem_Context_GraphInfoV1_t::getGraphInfoBlob,
                          &QairtSystem_Context_GraphInfoV1_t::getStartOpIndex,
                          &QairtSystem_Context_GraphInfoV1_t::getEndOpIndex>(getInterfaceFn);
    api.sysCtxGraphInfoSet =
        populateInterface<QAIRT_SYSTEM_CONTEXT_GRAPH_INFO_SET_V1_ID,
                          &QairtSystem_Context_GraphInfoSetV1_t::getNumGraphInfos,
                          &QairtSystem_Context_GraphInfoSetV1_t::getGraphInfoAt>(getInterfaceFn);
    api.sysCtxBinaryInfo =
        populateInterface<QAIRT_SYSTEM_CONTEXT_BINARY_INFO_V1_ID,
                          &QairtSystem_Context_BinaryInfoV1_t::getBackendId,
                          &QairtSystem_Context_BinaryInfoV1_t::getBuildId,
                          &QairtSystem_Context_BinaryInfoV1_t::getApiVersion,
                          &QairtSystem_Context_BinaryInfoV1_t::getBackendApiVersion,
                          &QairtSystem_Context_BinaryInfoV1_t::getNumSocVersions,
                          &QairtSystem_Context_BinaryInfoV1_t::getSocVersionAt,
                          &QairtSystem_Context_BinaryInfoV1_t::getNumGraphInfos,
                          &QairtSystem_Context_BinaryInfoV1_t::getGraphInfoAt,
                          &QairtSystem_Context_BinaryInfoV1_t::getDevicePlatformInfo,
                          &QairtSystem_Context_BinaryInfoV1_t::getSocModel>(getInterfaceFn);
    api.sysCtx = populateInterface<QAIRT_SYSTEM_CONTEXT_V1_ID,
                                   &QairtSystem_ContextV1_t::create,
                                   &QairtSystem_ContextV1_t::getBinaryInfo,
                                   &QairtSystem_ContextV1_t::free>(getInterfaceFn);
    api.sysDlc = populateInterface<QAIRT_SYSTEM_DLC_V1_ID,
                                   &QairtSystem_DlcV1_t::createFromFile,
                                   &QairtSystem_DlcV1_t::createFromBinary,
                                   &QairtSystem_DlcV1_t::composeGraphs,
                                   &QairtSystem_DlcV1_t::getOpMappings,
                                   &QairtSystem_DlcV1_t::free>(getInterfaceFn);
    api.sysDlcGraphConfigInfo =
        populateInterface<QAIRT_SYSTEM_DLC_GRAPH_CONFIG_INFO_V1_ID,
                          &QairtSystem_Dlc_GraphConfigInfoV1_t::create,
                          &QairtSystem_Dlc_GraphConfigInfoV1_t::free,
                          &QairtSystem_Dlc_GraphConfigInfoV1_t::setGraphName,
                          &QairtSystem_Dlc_GraphConfigInfoV1_t::getGraphName,
                          &QairtSystem_Dlc_GraphConfigInfoV1_t::setGraphConfig>(getInterfaceFn);
    api.sysLog = populateInterface<QAIRT_SYSTEM_LOG_V1_ID,
                                   &QairtSystemLog_V1_t::create,
                                   &QairtSystemLog_V1_t::setLogLevel,
                                   &QairtSystemLog_V1_t::free>(getInterfaceFn);
    api.sysProfileHeader =
        populateInterface<QAIRT_SYSTEM_PROFILE_HEADER_V1_ID,
                          &QairtSystem_Profile_HeaderV1_t::create,
                          &QairtSystem_Profile_HeaderV1_t::free,
                          &QairtSystem_Profile_HeaderV1_t::getStartTime,
                          &QairtSystem_Profile_HeaderV1_t::setStartTime,
                          &QairtSystem_Profile_HeaderV1_t::getStopTime,
                          &QairtSystem_Profile_HeaderV1_t::setStopTime,
                          &QairtSystem_Profile_HeaderV1_t::getStartMem,
                          &QairtSystem_Profile_HeaderV1_t::setStartMem,
                          &QairtSystem_Profile_HeaderV1_t::getStopMem,
                          &QairtSystem_Profile_HeaderV1_t::setStopMem,
                          &QairtSystem_Profile_HeaderV1_t::getMethodType,
                          &QairtSystem_Profile_HeaderV1_t::setMethodType,
                          &QairtSystem_Profile_HeaderV1_t::getVisibility,
                          &QairtSystem_Profile_HeaderV1_t::setVisibility,
                          &QairtSystem_Profile_HeaderV1_t::getGraphName,
                          &QairtSystem_Profile_HeaderV1_t::setGraphName>(getInterfaceFn);
    api.sysProfileEvent =
        populateInterface<QAIRT_SYSTEM_PROFILE_EVENT_V1_ID,
                          &QairtSystem_Profile_EventV1_t::create,
                          &QairtSystem_Profile_EventV1_t::free,
                          &QairtSystem_Profile_EventV1_t::getEventDataType,
                          &QairtSystem_Profile_EventV1_t::setEventData,
                          &QairtSystem_Profile_EventV1_t::getEventData,
                          &QairtSystem_Profile_EventV1_t::getNumSubEvents,
                          &QairtSystem_Profile_EventV1_t::getSubEventAt,
                          &QairtSystem_Profile_EventV1_t::setSubEventData,
                          &QairtSystem_Profile_EventV1_t::addSubEvent>(getInterfaceFn);
    api.sysProfileData =
        populateInterface<QAIRT_SYSTEM_PROFILE_PROFILE_DATA_V1_ID,
                          &QairtSystem_Profile_ProfileDataV1_t::create,
                          &QairtSystem_Profile_ProfileDataV1_t::free,
                          &QairtSystem_Profile_ProfileDataV1_t::setHeader,
                          &QairtSystem_Profile_ProfileDataV1_t::getHeader,
                          &QairtSystem_Profile_ProfileDataV1_t::getNumProfilingEvents,
                          &QairtSystem_Profile_ProfileDataV1_t::getEventAt,
                          &QairtSystem_Profile_ProfileDataV1_t::setEvents,
                          &QairtSystem_Profile_ProfileDataV1_t::addEvent>(getInterfaceFn);
    api.sysProfileDataVersion =
        populateInterface<QAIRT_SYSTEM_PROFILE_DATA_V1_ID,
                          &QairtSystem_Profile_DataV1_t::create,
                          &QairtSystem_Profile_DataV1_t::free,
                          &QairtSystem_Profile_DataV1_t::getVersion,
                          &QairtSystem_Profile_DataV1_t::setProfileDataV1,
                          &QairtSystem_Profile_DataV1_t::getProfileDataV1>(getInterfaceFn);
    api.sysProfileFileHeader =
        populateInterface<QAIRT_SYSTEM_PROFILE_SERIALIZATION_FILE_HEADER_V1_ID,
                          &QairtSystem_Profile_SerializationFileHeaderV1_t::create,
                          &QairtSystem_Profile_SerializationFileHeaderV1_t::free,
                          &QairtSystem_Profile_SerializationFileHeaderV1_t::getAppName,
                          &QairtSystem_Profile_SerializationFileHeaderV1_t::setAppName,
                          &QairtSystem_Profile_SerializationFileHeaderV1_t::getAppVersion,
                          &QairtSystem_Profile_SerializationFileHeaderV1_t::setAppVersion,
                          &QairtSystem_Profile_SerializationFileHeaderV1_t::getBackendVersion,
                          &QairtSystem_Profile_SerializationFileHeaderV1_t::setBackendVersion>(
            getInterfaceFn);
    api.sysProfileTargetConfig = populateInterface<
        QAIRT_SYSTEM_PROFILE_SERIALIZATION_TARGET_CONFIGURATION_V1_ID,
        &QairtSystem_Profile_SerializationTargetConfigurationV1_t::create,
        &QairtSystem_Profile_SerializationTargetConfigurationV1_t::free,
        &QairtSystem_Profile_SerializationTargetConfigurationV1_t::setMaxNumMessages,
        &QairtSystem_Profile_SerializationTargetConfigurationV1_t::getMaxNumMessages,
        &QairtSystem_Profile_SerializationTargetConfigurationV1_t::setFileHeader,
        &QairtSystem_Profile_SerializationTargetConfigurationV1_t::getFileHeader>(getInterfaceFn);
    api.sysProfileTargetFile =
        populateInterface<QAIRT_SYSTEM_PROFILE_SERIALIZATION_TARGET_FILE_V1_ID,
                          &QairtSystem_Profile_SerializationTargetFileV1_t::create,
                          &QairtSystem_Profile_SerializationTargetFileV1_t::free,
                          &QairtSystem_Profile_SerializationTargetFileV1_t::setFileName,
                          &QairtSystem_Profile_SerializationTargetFileV1_t::getFileName,
                          &QairtSystem_Profile_SerializationTargetFileV1_t::setFileDirectory,
                          &QairtSystem_Profile_SerializationTargetFileV1_t::getFileDirectory>(
            getInterfaceFn);
    api.sysProfileTargetInfo =
        populateInterface<QAIRT_SYSTEM_PROFILE_SERIALIZATION_TARGET_INFO_V1_ID,
                          &QairtSystem_Profile_SerializationTargetInfoV1_t::create,
                          &QairtSystem_Profile_SerializationTargetInfoV1_t::free,
                          &QairtSystem_Profile_SerializationTargetInfoV1_t::getType,
                          &QairtSystem_Profile_SerializationTargetInfoV1_t::setTargetFile,
                          &QairtSystem_Profile_SerializationTargetInfoV1_t::getTargetFile>(
            getInterfaceFn);
    api.sysProfileSerializationTarget =
        populateInterface<QAIRT_SYSTEM_PROFILE_SERIALIZATION_TARGET_V1_ID,
                          &QairtSystem_Profile_SerializationTargetV1_t::create,
                          &QairtSystem_Profile_SerializationTargetV1_t::free,
                          &QairtSystem_Profile_SerializationTargetV1_t::serializeEventData>(
            getInterfaceFn);

    // Populate QairtSystemBuilder interface
    api.sysBuilder = populateInterface<QAIRT_SYSTEM_BUILDER_V1_ID,
                                       &QairtSystemBuilder_V1_t::create,
                                       &QairtSystemBuilder_V1_t::free,
                                       &QairtSystemBuilder_V1_t::setDlc,
                                       &QairtSystemBuilder_V1_t::setDlcPtr,
                                       &QairtSystemBuilder_V1_t::setLogLevel,
                                       &QairtSystemBuilder_V1_t::setProfilingLevel,
                                       &QairtSystemBuilder_V1_t::setProfilingOption,
                                       &QairtSystemBuilder_V1_t::setPerfProfile,
                                       &QairtSystemBuilder_V1_t::setPlatformOptions,
                                       &QairtSystemBuilder_V1_t::setDeviceOptions,
                                       &QairtSystemBuilder_V1_t::addOpPackage,
                                       &QairtSystemBuilder_V1_t::setBackendType,
                                       &QairtSystemBuilder_V1_t::setPriority,
                                       &QairtSystemBuilder_V1_t::setEnabledGraphs,
                                       &QairtSystemBuilder_V1_t::setMemoryLimitHint,
                                       &QairtSystemBuilder_V1_t::setIsPersistentBinary,
                                       &QairtSystemBuilder_V1_t::setCacheCompatibilityMode,
                                       &QairtSystemBuilder_V1_t::setProfileMaxEvents,
                                       &QairtSystemBuilder_V1_t::setInputDimensions,
                                       &QairtSystemBuilder_V1_t::validate,
                                       &QairtSystemBuilder_V1_t::build,
                                       &QairtSystemBuilder_V1_t::setBackendOrder,
                                       &QairtSystemBuilder_V1_t::setBackendPartition>(getInterfaceFn);

    // Populate Qairt interface
    api.qairt = populateInterface<QAIRT_V1_ID,
                                  &Qairt_V1_t::free,
                                  &Qairt_V1_t::executeGraph,
                                  &Qairt_V1_t::execute,
                                  &Qairt_V1_t::getNumGraphInfos,
                                  &Qairt_V1_t::getGraphInfoAt,
                                  &Qairt_V1_t::getLastError,
                                  &Qairt_V1_t::getNumModelInputInfos,
                                  &Qairt_V1_t::getModelInputInfoAt,
                                  &Qairt_V1_t::getNumModelOutputInfos,
                                  &Qairt_V1_t::getModelOutputInfoAt>(getInterfaceFn);

    return api;
  }

  template <auto p2m>
  static constexpr bool ensureNotNullptr() {
    static_assert(p2m != nullptr);
    return p2m != nullptr;
  }

  template <auto P2m_InterfaceFn, typename... Args>
  auto call(Args&&... args) const {
    static_assert(ensureNotNullptr<P2m_InterfaceFn>());
    static_assert(ensureNotNullptr<p2m_to_api_member<decltype(P2m_InterfaceFn)>::value>());
    static_assert(ensureNotNullptr<p2mv_to_api_member_v<P2m_InterfaceFn>>());

    if (this->*p2mv_to_api_member_v<P2m_InterfaceFn>.*P2m_InterfaceFn == nullptr) {
      throw std::runtime_error("A function has not been wired up to the system api table.");
    }

#if defined(__clang__) //Checks if the compiler is Clang
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsign-conversion"
#endif
    return (this->*p2mv_to_api_member_v<P2m_InterfaceFn>.*
            P2m_InterfaceFn)(std::forward<Args>(args)...);
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
  }

  template <auto P2m_InterfaceFn, typename... Args>
  auto getFunction() const {
    static_assert(ensureNotNullptr<P2m_InterfaceFn>());
    static_assert(ensureNotNullptr<p2m_to_api_member<decltype(P2m_InterfaceFn)>::value>());
    static_assert(ensureNotNullptr<p2mv_to_api_member_v<P2m_InterfaceFn>>());

    return this->*p2mv_to_api_member_v<P2m_InterfaceFn>.*P2m_InterfaceFn;
  }
};

/**
 * @brief Entry point for loading the QAIRT System library and creating system objects.
 *
 *        Construct by passing a path to the system shared library (e.g.
 *        `"libQairtSystem.so"`), a static function pointer, or a
 *        StaticBackendTag / QairtSystemBackendTag for specialized lookup modes.
 *        Use make(), makeUnique(), or makeShared() to instantiate
 *        SystemLog, SystemDlc, SystemContext, QairtSystemBuilder, and Qairt objects.
 */
class SystemApi {
  template <typename, typename, typename>
  friend class ApiType;

 public:
  /// @brief Shared pointer to the populated QAIRT System API interface table.
  std::shared_ptr<SystemApiTable> m_systemApiTable;

  /**
   * @brief Tag type for static backend construction (RTLD_DEFAULT / compile-time linkage).
   *
   * When QAIRT_SYSTEM_STATIC_LINK is defined, falls back to the directly linked
   * QairtSystemInterface_getInterface symbol if the RTLD_DEFAULT lookup fails.
   */
  struct StaticBackendTag {};
  static constexpr StaticBackendTag StaticBackend{};

  /**
   * @brief Construct SystemApi by dynamically loading a system library.
   *
   * @param libSoFilePath Path to the system shared library (e.g. "libQairtSystem.so").
   * @param openFlags     dlopen flags (default: RTLD_NOW).
   */
  // Default flags: RTLD_NOW | RTLD_GLOBAL.
  // RTLD_GLOBAL ensures that symbols exported by the system library are visible
  // to subsequently dlopen'd libraries (and vice versa).
  explicit SystemApi(const char* libSoFilePath,
                     unsigned int openFlags = dynamicloading::dl_now | dynamicloading::dl_global) {
    void* handle = dynamicloading::dlopen(libSoFilePath, openFlags);
    if (!handle) {
      std::string errMsg = "Failed to dlopen system library: " + std::string(libSoFilePath);
#ifndef _WIN32
      const char* dlErr = dlerror();
      if (dlErr) {
        errMsg += ": ";
        errMsg += dlErr;
      }
#endif
      throw std::runtime_error(errMsg);
    }

    void* getInterfaceFn = dynamicloading::dlsym(handle, "QairtSystemInterface_getInterface");
    if (!getInterfaceFn) {
      throw std::runtime_error(
          std::string("Failed to dlsym QairtSystemInterface_getInterface from: ") + libSoFilePath);
    }

    m_systemApiTable = std::make_shared<SystemApiTable>(SystemApiTable::create(
        handle, reinterpret_cast<decltype(QairtSystemInterface_getInterface)*>(getInterfaceFn)));
  }

  /**
   * @brief Construct SystemApi from a statically-linked getInterface function pointer.
   *
   * Use this when the system library is linked statically into the binary.
   *
   * @param getInterfaceFn Pointer to QairtSystemInterface_getInterface from the system library.
   */
  explicit SystemApi(decltype(QairtSystemInterface_getInterface) getInterfaceFn)
      : m_systemApiTable(
            std::make_shared<SystemApiTable>(SystemApiTable::create(nullptr, getInterfaceFn))) {}

  /**
   * @brief Tag type for constructing SystemApi from the underlying system library
   * directly instead of the QAIRT System library (QairtSystemInterface_getInterface).
   *
   * This is used by the InternalSystemContextBridge to avoid infinite recursion:
   * the QAIRT System C API implementations call the bridge, and the bridge needs
   * to call the underlying system library directly without going back through the
   * QAIRT System C API function pointers.
   */
  struct QairtSystemBackendTag {};
  static constexpr QairtSystemBackendTag QairtSystemBackend{};

  /**
   * @brief Construct SystemApi from the underlying system interface provider.
   *
   * Looks up the system interface provider via RTLD_DEFAULT, gets the
   * system interface, and creates a SystemApiTable by wrapping the
   * system functions in QAIRT-compatible shims.
   */
  explicit SystemApi(QairtSystemBackendTag);

  /**
   * @brief Construct SystemApi using RTLD_DEFAULT symbol lookup.
   *
   * Searches for QairtSystemInterface_getInterface in the already-loaded symbols.
   * If QAIRT_SYSTEM_STATIC_LINK is defined and the lookup fails, falls back to
   * the directly linked symbol.
   */
  explicit SystemApi(StaticBackendTag) {
    auto* getInterfaceFn = reinterpret_cast<decltype(QairtSystemInterface_getInterface)*>(
        dynamicloading::dlsym(dynamicloading::dl_default, "QairtSystemInterface_getInterface"));

#ifdef QAIRT_SYSTEM_STATIC_LINK
    if (!getInterfaceFn) {
      getInterfaceFn = &QairtSystemInterface_getInterface;
    }
#endif

    if (!getInterfaceFn) {
      throw std::runtime_error(
          "Failed to find QairtSystemInterface_getInterface for system library. "
          "Ensure the system library is loaded or define QAIRT_SYSTEM_STATIC_LINK.");
    }

    m_systemApiTable =
        std::make_shared<SystemApiTable>(SystemApiTable::create(nullptr, getInterfaceFn));
  }

  /**
   * @brief Construct SystemApi from an existing, pre-populated API table.
   *
   * Use this when the SystemApiTable has already been constructed externally
   * (e.g. by an internal bridge) and you need to wrap it without re-loading
   * the library.
   *
   * @param[in] systemApiTable  Shared pointer to a fully populated SystemApiTable.
   *                             Must not be null.
   */
  explicit SystemApi(std::shared_ptr<SystemApiTable> systemApiTable)
      : m_systemApiTable(std::move(systemApiTable)) {}

  SystemApi(const SystemApi&)                = delete;
  SystemApi(SystemApi&&) noexcept            = default;
  SystemApi& operator=(const SystemApi&)     = delete;
  SystemApi& operator=(SystemApi&&) noexcept = default;

  /**
   * @brief Create a QAIRT system object bound to this SystemApi's interface table.
   *
   * @tparam QairtClass  A class derived from ApiType.
   * @tparam Args        Constructor argument types forwarded to QairtClass.
   * @param[in] args     Constructor arguments forwarded to QairtClass.
   * @return             A newly constructed QairtClass instance.
   * @throws std::runtime_error  If a required interface function pointer is null in the table.
   */
  template <typename QairtClass, typename... Args>
  typename std::enable_if_t<detail::is_api_type_v<QairtClass>, QairtClass> make(
      Args&&... args) const {
    return QairtClass(m_systemApiTable, std::forward<Args>(args)...);
  }

  /**
   * @brief Wrapper which allows for a raw-pointer result type instead of a value type.
   *
   * @see SystemApi::make(Args&&...)
   */
  template <typename QairtClass, typename... Args>
  std::enable_if_t<std::is_pointer_v<QairtClass> &&
                       detail::is_api_type_v<std::remove_pointer_t<QairtClass>>,
                   QairtClass>
  make(Args&&... args) const {
    return new std::remove_pointer_t<QairtClass>(m_systemApiTable, std::forward<Args>(args)...);
  }

  /**
   * @brief Create a QAIRT system object wrapped in a std::unique_ptr.
   *
   * @tparam QairtClass  A class derived from ApiType.
   * @tparam Args        Constructor argument types forwarded to QairtClass.
   * @param[in] args     Constructor arguments forwarded after the SystemApiTable pointer.
   * @return             std::unique_ptr<QairtClass> owning the newly constructed object.
   * @throws std::runtime_error  If a required interface function pointer is null in the table.
   */
  template <typename QairtClass, typename... Args>
  std::enable_if_t<detail::is_api_type_v<QairtClass>, std::unique_ptr<QairtClass>> makeUnique(
      Args&&... args) const {
    return std::unique_ptr<QairtClass>(
        new QairtClass(m_systemApiTable, std::forward<Args>(args)...));
  }

  /**
   * @brief Create a QAIRT system object wrapped in a std::shared_ptr.
   *
   * @tparam QairtClass  A class derived from ApiType.
   * @tparam Args        Constructor argument types forwarded to QairtClass.
   * @param[in] args     Constructor arguments forwarded after the SystemApiTable pointer.
   * @return             std::shared_ptr<QairtClass> owning the newly constructed object.
   * @throws std::runtime_error  If a required interface function pointer is null in the table.
   */
  template <typename QairtClass, typename... Args>
  std::enable_if_t<detail::is_api_type_v<QairtClass>, std::shared_ptr<QairtClass>> makeShared(
      Args&&... args) const {
    return std::shared_ptr<QairtClass>(
        new QairtClass(m_systemApiTable, std::forward<Args>(args)...));
  }
};

}  // namespace qairt

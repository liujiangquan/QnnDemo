//==============================================================================
//
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// All rights reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//==============================================================================

#pragma once

#include <algorithm>
#include <functional>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "QairtBackend/QairtBackend.h"
#include "QairtBackend/QairtBackendConfig.h"
#include "QairtCommon/QairtCommon.h"
#include "QairtCommon/QairtInfo.h"
#include "QairtContext/QairtContext.h"
#include "QairtContext/QairtContextConfig.h"
#include "QairtDevice/QairtDevice.h"
#include "QairtGlobalConfig/QairtGlobalConfig.h"
#include "QairtGraph/QairtGraph.h"
#include "QairtGraph/QairtGraphConfig.h"
#include "QairtInterface/QairtInterface.h"
#include "QairtLog/QairtLog.h"
#include "QairtMem/QairtMemCommon.h"
#include "QairtMem/QairtMemDescriptor.h"
#include "QairtMem/QairtMemShape.h"
#include "QairtOpConfig/QairtOpConfig.h"
#include "QairtQuantizeParams/QairtQuantizeParams.h"
#include "QairtSignal/QairtSignal.h"
#include "QairtSignal/QairtSignalConfig.h"
#include "QairtTensor/QairtTensor.h"
#include "QairtTypeTraits.hpp"

// clang-format off
#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
// clang-format on

#include <set>

inline std::set<HMODULE> sg_modHandles;
inline thread_local char* sg_lastErrMsg = "";

namespace qairt::dynamicloading {
inline constexpr bool is_windows        = true;
inline constexpr unsigned int dl_now    = 0x1;
inline constexpr unsigned int dl_local  = 0x2;
inline constexpr unsigned int dl_global = 0x4;
inline constexpr unsigned int dl_noload = 0x8;

struct dl_default_t {};
inline constexpr dl_default_t dl_default{};

}  // namespace qairt::dynamicloading
#else
#include <dlfcn.h>
namespace qairt::dynamicloading {
inline constexpr bool is_windows        = false;
inline constexpr unsigned int dl_now    = RTLD_NOW;
inline constexpr unsigned int dl_local  = RTLD_LOCAL;
inline constexpr unsigned int dl_global = RTLD_GLOBAL;
inline constexpr unsigned int dl_noload = RTLD_NOLOAD;

struct dl_default_t {};
inline constexpr dl_default_t dl_default{};

}  // namespace qairt::dynamicloading
#endif

// Dynamic Loading Support
namespace qairt::dynamicloading {

inline void* dlopen(const char* filename, unsigned int flags) {
#ifdef _WIN32
  HMODULE mod;
  HANDLE cur_proc;
  DWORD as_is, to_be;
  bool loadedBefore = false;

  if (!filename || ::strlen(filename) == 0) {
    mod = GetModuleHandleA(NULL);
    if (!mod) {
      sg_lastErrMsg = "getting own handle failed";
      return NULL;
    }
    return static_cast<void*>(mod);
  }

  if (!(flags & dl_now)) {
    sg_lastErrMsg = "flags must either include dl_now or only specify dl_noload";
    return NULL;
  }

  if (flags & dl_noload) {
    mod = GetModuleHandleA(filename);
    return static_cast<void*>(mod);
  }

  cur_proc = GetCurrentProcess();

  if (EnumProcessModules(cur_proc, NULL, 0, &as_is) == 0) {
    sg_lastErrMsg = "enumerate modules failed before loading module";
    return NULL;
  }

  // search from system lib path first
  mod = LoadLibraryExA(filename, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
  if (!mod) {
    sg_lastErrMsg = "load library failed";
    return NULL;
  }

  if (EnumProcessModules(cur_proc, NULL, 0, &to_be) == 0) {
    sg_lastErrMsg = "enumerate modules failed after loading module";
    FreeLibrary(mod);
    return NULL;
  }

  if (as_is == to_be) {
    loadedBefore = true;
  }

  if ((!loadedBefore) && (flags & dl_local)) {
    sg_modHandles.insert(mod);
  }

  if (flags & dl_global) {
    sg_modHandles.erase(mod);
  }

  return static_cast<void*>(mod);
#else
  return ::dlopen(filename, static_cast<int>(flags));
#endif
}

inline void* dlsym(void* handle, const char* symbol) {
#ifdef _WIN32
  FARPROC sym_addr = NULL;
  HANDLE cur_proc;
  DWORD size, size_needed;
  HMODULE* mod_list;
  HMODULE mod = 0;

  if ((!handle) || (!symbol)) {
    return NULL;
  }

  cur_proc = GetCurrentProcess();

  if (EnumProcessModules(cur_proc, NULL, 0, &size) == 0) {
    sg_lastErrMsg = "enumerate modules failed before memory allocation";
    return NULL;
  }

  mod_list = static_cast<HMODULE*>(malloc(size));
  if (!mod_list) {
    sg_lastErrMsg = "malloc failed";
    return NULL;
  }

  if (EnumProcessModules(cur_proc, mod_list, size, &size_needed) == 0) {
    sg_lastErrMsg = "enumerate modules failed after memory allocation";
    free(mod_list);
    return NULL;
  }

  if (handle == (void*)0x4) {
    for (size_t i = 0; i < (size / sizeof(HMODULE)); i++) {
      auto iter = sg_modHandles.find(mod_list[i]);
      if (iter != sg_modHandles.end()) {
        continue;
      }
      sym_addr = GetProcAddress(mod_list[i], symbol);
      if (sym_addr) {
        free(mod_list);
        return *(void**)(&sym_addr);
      }
    }
  } else {
    mod = static_cast<HMODULE>(handle);
  }

  free(mod_list);
  sym_addr = GetProcAddress(mod, symbol);
  if (!sym_addr) {
    sg_lastErrMsg = "can't resolve symbol";
    return NULL;
  }

  return *(void**)(&sym_addr);
#else
  return ::dlsym(handle, symbol);
#endif
}

inline void* dlsym(dl_default_t, const char* symbol) {
#ifdef _WIN32
  return dlsym((void*)0x4, symbol);
#else
  return dlsym(RTLD_DEFAULT, symbol);
#endif
}

inline int dlclose(void* handle) {
  if (!handle) {
    return 0;
  }
#ifdef _WIN32
  HMODULE mod = static_cast<HMODULE>(handle);

  if (FreeLibrary(mod) == 0) {
    sg_lastErrMsg = "free library failed";
    return -1;
  }

  sg_modHandles.erase(mod);

  return 0;
#else
  return ::dlclose(handle);
#endif
}

}  // namespace qairt::dynamicloading

namespace qairt {

namespace detail {

// Compile time check on a p2m
template <auto p2m>
static constexpr bool ensure_not_nullptr() {
  static_assert(p2m != nullptr);
  return p2m != nullptr;
}
template <auto P2m>
inline constexpr bool ensure_not_nullptr_v = ensure_not_nullptr<P2m>();
}  // namespace detail

#define QAIRT_CPPAPI_DEFINE_ENUM(CppName, CapiName, ...) \
  enum class CppName : std::underlying_type_t<CapiName> { __VA_ARGS__ };

QAIRT_CPPAPI_DEFINE_ENUM(CommonError,
                         QairtCommon_Error_t,
                         MinError                 = QAIRT_COMMON_MIN_ERROR,
                         NotSupported             = QAIRT_COMMON_ERROR_NOT_SUPPORTED,
                         MemAlloc                 = QAIRT_COMMON_ERROR_MEM_ALLOC,
                         System                   = QAIRT_COMMON_ERROR_SYSTEM,
                         InvalidArgument          = QAIRT_COMMON_ERROR_INVALID_ARGUMENT,
                         OperationNotPermitted    = QAIRT_COMMON_ERROR_OPERATION_NOT_PERMITTED,
                         PlatformNotSupported     = QAIRT_COMMON_ERROR_PLATFORM_NOT_SUPPORTED,
                         SystemCommunication      = QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION,
                         IncompatibleBinaries     = QAIRT_COMMON_ERROR_INCOMPATIBLE_BINARIES,
                         LoadingBinaries          = QAIRT_COMMON_ERROR_LOADING_BINARIES,
                         ResourceUnavailable      = QAIRT_COMMON_ERROR_RESOURCE_UNAVAILABLE,
                         SystemCommunicationFatal = QAIRT_COMMON_ERROR_SYSTEM_COMMUNICATION_FATAL,
                         InvalidHandle            = QAIRT_COMMON_ERROR_INVALID_HANDLE,
                         OutOfRange               = QAIRT_COMMON_ERROR_OUT_OF_RANGE,
                         General                  = QAIRT_COMMON_ERROR_GENERAL,
                         Undefined                = QAIRT_COMMON_ERROR_UNDEFINED)

class Exception : public std::exception {
  template <typename T>
  inline static constexpr bool is_valid_error_enum_v = std::is_enum_v<T>&&
      std::is_same_v<std::underlying_type_t<T>, std::underlying_type_t<CommonError>>;

  template <typename T>
  inline static constexpr CommonError as_common_error(const T& t) {
    static_assert(is_valid_error_enum_v<T>);
    return static_cast<CommonError>(static_cast<std::underlying_type_t<CommonError>>(t));
  }

  static constexpr CommonError default_error = CommonError::MinError;
  CommonError m_error;
  mutable std::string m_whatArg = "";

 public:
  explicit Exception(const std::string& whatArg, CommonError error = default_error)
      : m_error(error), m_whatArg(whatArg) {}
  explicit Exception(const char* whatArg, CommonError error = default_error)
      : m_error(error), m_whatArg(whatArg) {}

  Exception(Exception&&) noexcept            = default;
  Exception(const Exception&)                = default;
  Exception& operator=(Exception&&) noexcept = default;
  Exception& operator=(const Exception&)     = default;

  template <typename T, typename std::enable_if_t<is_valid_error_enum_v<T>, int> = 0>
  Exception(const std::string& whatArg, const T& error)
      : m_error(as_common_error(error)), m_whatArg(whatArg) {}
  template <typename T, typename std::enable_if_t<is_valid_error_enum_v<T>, int> = 0>
  Exception(const char* whatArg, const T& error)
      : m_error(as_common_error(error)), m_whatArg(whatArg) {}

  // Allow comparison between types
  template <typename T>
  std::enable_if_t<std::is_enum_v<T>, bool> operator==(const T& t) const noexcept {
    static_assert(is_valid_error_enum_v<T>,
                  "Cannot make enum comparisons with different underlying types.");
    return static_cast<std::underlying_type_t<CommonError>>(m_error) ==
           static_cast<std::underlying_type_t<CommonError>>(t);
  }
  template <typename T>
  std::enable_if_t<std::is_enum_v<T>, bool> operator!=(const T& t) const noexcept {
    return !operator==(t);
  }

  const char* what() const noexcept override { return m_whatArg.c_str(); }
};

inline constexpr auto stub_exception_string = "QAIRT C++API: This function is not yet implemented.";

}  // namespace qairt

#ifdef __clang__
namespace qairt {
namespace detail {
inline std::string get_stub_string(const char* func) {
  return std::string("QAIRT C++API: This function is not yet implemented: ") + func;
}
}  // namespace detail
}  // namespace qairt
#define QAIRT_CPPAPI_STUB_EXCEPTION                                         \
  ::qairt::Exception(::qairt::detail::get_stub_string(__PRETTY_FUNCTION__), \
                     ::qairt::CommonError::NotSupported)
#else
#define QAIRT_CPPAPI_STUB_EXCEPTION \
  ::qairt::Exception(::qairt::stub_exception_string, ::qairt::CommonError::NotSupported)
#endif

namespace qairt {

// This section handles populating stubs in the event that an API is not supported
namespace detail {

template <typename T_Ret, typename... T_Args>
struct stub_maker {
  static inline T_Ret stub(T_Args...) {
    throw Exception("Function not supported by current QAIRT Version", CommonError::NotSupported);
  }
};

template <typename T_Ret, typename... T_Args>
void populateStub(T_Ret (*&ptr)(T_Args...)) {
  ptr = stub_maker<T_Ret, T_Args...>::stub;
}

template <auto P2m>
void populate_(const p2mv_parent_type_t<P2m>* retrieved, p2mv_parent_type_t<P2m>& iface) {
  // Ensure it's a reasonable member size for a function pointer
  static_assert(sizeof(void (*)()) == sizeof(iface.*P2m));

  const uint64_t offset = static_cast<uint64_t>(reinterpret_cast<ptrdiff_t>(&(iface.*P2m)) -
                                                reinterpret_cast<ptrdiff_t>(&iface));

  if (retrieved == nullptr || offset + (sizeof(retrieved->*P2m) - 1) >= retrieved->size ||
      retrieved->*P2m == nullptr) {
    populateStub(iface.*P2m);
  } else {
    iface.*P2m = retrieved->*P2m;
  }
}

// Populate interface function pointers with stubs in case they are outside the range of function
// pointers returned by the getInterface function call.
template <auto... P2ms>
void populate(const p2mv_parent_type_t<first_param_v<P2ms...>>* retrieved,
              p2mv_parent_type_t<first_param_v<P2ms...>>& iface) {
  static_assert(all_are_same_v<p2mv_parent_type_t<P2ms>...>);
  (populate_<P2ms>(retrieved, iface), ...);
}

template <typename T_Interface, int InterfaceID>
inline T_Interface* getInterface(decltype(QairtInterface_getInterface) getInterfaceFn) {
  void* iface_ptr = nullptr;
  auto status     = (getInterfaceFn)(InterfaceID, &iface_ptr);
  if (status != QAIRT_SUCCESS) {
    return {};
  }

  if (!iface_ptr) {
    throw std::runtime_error("nullptr returned from getInterface alongside QAIRT_SUCCESS");
  }

  return static_cast<T_Interface*>(iface_ptr);
}

template <int InterfaceID, auto P2m_First, auto... P2m_Rest>
inline p2mv_parent_type_t<P2m_First> populateInterface(
    decltype(QairtInterface_getInterface) getInterfaceFn) {
  using interface_type = p2mv_parent_type_t<P2m_First>;
  static_assert(all_are_same_v<interface_type, p2mv_parent_type_t<P2m_Rest>...>);

  const interface_type* retrieved = getInterface<interface_type, InterfaceID>(getInterfaceFn);

  interface_type toret{};

  // Copy over the size and getInterface function
  if (retrieved) {
    toret.size         = retrieved->size;
    toret.getInterface = retrieved->getInterface;
  } else {
    toret.size         = 0;
    toret.getInterface = nullptr;
  }

  populate<P2m_First, P2m_Rest...>(retrieved, toret);

  return toret;
}
}  // namespace detail

template <typename>
struct Interface;

// A type to ensure a shared lib is not closed until all instances of it are no longer used
struct DlHandleHolder {
  void* m_dlHandle{nullptr};
  ~DlHandleHolder() noexcept {
    if (m_dlHandle) {
      QAIRT_NO_RETURN dynamicloading::dlclose(m_dlHandle);
      m_dlHandle = nullptr;
    }
  }
  explicit DlHandleHolder(void* dlHandle) noexcept : m_dlHandle{dlHandle} {}
  DlHandleHolder() noexcept             = default;
  DlHandleHolder(const DlHandleHolder&) = delete;
  DlHandleHolder(DlHandleHolder&& other) noexcept
      : m_dlHandle(std::exchange(other.m_dlHandle, nullptr)) {}
  DlHandleHolder& operator=(const DlHandleHolder&) = delete;
  DlHandleHolder& operator=(DlHandleHolder&& other) noexcept {
    if (this != &other) {
      std::swap(m_dlHandle, other.m_dlHandle);
    }
    return *this;
  }
};

struct ApiTable {
  std::shared_ptr<DlHandleHolder> m_dlHandle;

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
  struct p2m_to_api_member<T type::*> : apitable_p2m<&ApiTable::name> {};

  // BACKEND
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtBackend_V1_t, QairtBackend_Handle_t, backend)
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtBackend_ConfigV1_t,
                                 QairtBackend_ConfigHandle_t,
                                 backendConfig)
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtBackend_OperationNameV1_t,
                                 QairtBackend_OperationNameHandle_t,
                                 backendOperationName)

  // LOG
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtLog_CallbackV1_t, QairtLog_CallbackHandle_t, logCallback)
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtLog_V1_t, QairtLog_Handle_t, log)

  // TENSOR
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtTensor_PropertiesV1_t,
                                 QairtTensor_PropertiesHandle_t,
                                 tensorProperties)
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtTensor_ClientBufferV1_t,
                                 QairtTensor_ClientBufferHandle_t,
                                 tensorClientBuffer)
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtTensor_MemoryV1_t, QairtTensor_MemoryHandle_t, tensorMemory)
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtTensor_V1_t, QairtTensor_Handle_t, tensor)

  // QUANTIZEPARAMS
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtQuantizeParams_ScaleOffsetV1_t,
                                 QairtQuantizeParams_ScaleOffsetHandle_t,
                                 scaleOffset)
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtQuantizeParams_AxisScaleOffsetV1_t,
                                 QairtQuantizeParams_AxisScaleOffsetHandle_t,
                                 axisScaleOffset)
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtQuantizeParams_BwScaleOffsetV1_t,
                                 QairtQuantizeParams_BwScaleOffsetHandle_t,
                                 bwScaleOffset)
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtQuantizeParams_BwAxisScaleOffsetV1_t,
                                 QairtQuantizeParams_BwAxisScaleOffsetHandle_t,
                                 bwAxisScaleOffset)
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtQuantizeParams_BwAxisScaleOffsetMappedV1_t,
                                 QairtQuantizeParams_BwAxisScaleOffsetMappedHandle_t,
                                 bwAxisScaleOffsetMapped)
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtQuantizeParams_MicroscalingEncodingV1_t,
                                 QairtQuantizeParams_MicroscalingEncodingHandle_t,
                                 microscalingEncoding)
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtQuantizeParams_FloatScaleOffsetV1_t,
                                 QairtQuantizeParams_FloatScaleOffsetHandle_t,
                                 floatScaleOffset)
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtQuantizeParams_BwFloatBlockEncodingV1_t,
                                 QairtQuantizeParams_BwFloatBlockEncodingHandle_t,
                                 bwFloatBlockEncoding)
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtQuantizeParams_FloatBlockEncodingV1_t,
                                 QairtQuantizeParams_FloatBlockEncodingHandle_t,
                                 floatBlockEncoding)
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtQuantizeParams_BwBlockMappedV1_t,
                                 QairtQuantizeParams_BwBlockMappedHandle_t,
                                 bwBlockMapped)
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtQuantizeParams_BwBlockwiseExpansionMappedV1_t,
                                 QairtQuantizeParams_BwBlockwiseExpansionMappedHandle_t,
                                 bwBlockwiseExpansionMapped)
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtQuantizeParams_BlockEncodingV1_t,
                                 QairtQuantizeParams_BlockEncodingHandle_t,
                                 blockEncoding)
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtQuantizeParams_VectorEncodingV1_t,
                                 QairtQuantizeParams_VectorEncodingHandle_t,
                                 vectorEncoding)
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtQuantizeParams_BlockwiseExpansionV1_t,
                                 QairtQuantizeParams_BlockwiseExpansionHandle_t,
                                 blockwiseExpansion)
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtQuantizeParams_V1_t,
                                 QairtQuantizeParams_Handle_t,
                                 quantizeParams)

  // OPCONFIG
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtScalar_V1_t, QairtScalar_Handle_t, scalar)
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtOpConfig_ParamV1_t, QairtParam_Handle_t, param)
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtOpConfig_V1_t, QairtOpConfig_Handle_t, opConfig)

  // MEM
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtMem_DescriptorV1_t,
                                 QairtMem_DescriptorHandle_t,
                                 memDescriptor)
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtMem_ShapeV1_t, QairtMem_ShapeHandle_t, memShape)

  // DEVICE
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtDevice_V1_t, QairtDevice_Handle_t, device)
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtDevice_ConfigV1_t, QairtDevice_ConfigHandle_t, deviceConfig)
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtDevice_InfrastructureProviderV1_t,
                                 QairtDevice_InfrastructureHandle_t,
                                 deviceInfrastructureProvider)
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtDevice_PlatformInfoProviderV1_t,
                                 QairtDevice_PlatformInfoProviderHandle_t,
                                 devicePlatformInfoProvider)
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtDevice_PlatformInfoV1_t,
                                 QairtDevice_PlatformInfoHandle_t,
                                 devicePlatformInfo)
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtDevice_HardwareDeviceInfoV1_t,
                                 QairtDevice_HardwareDeviceInfoHandle_t,
                                 deviceHardwareDeviceInfo)
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtDevice_HardwareCoreInfoV1_t,
                                 QairtDevice_HardwareCoreInfoHandle_t,
                                 deviceHardwareCoreInfo)

  // GRAPH
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtGraph_V1_t, QairtGraph_Handle_t, graph)
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtGraph_ConfigV1_t, QairtGraph_ConfigHandle_t, graphConfig)

  // CONTEXT
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtContext_V1_t, QairtContext_Handle_t, context)
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtContext_AsyncExecutionDepthV1_t,
                                 QairtContext_AsyncExecutionDepthHandle_t,
                                 contextAsyncExecutionDepth)
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtContext_ConfigV1_t,
                                 QairtContext_ConfigHandle_t,
                                 contextConfig)
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtContext_BinaryBufferV1_t,
                                 QairtContext_BinaryBufferHandle_t,
                                 contextBinaryBuffer)
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtContext_BinaryV1_t,
                                 QairtContext_BinaryHandle_t,
                                 contextBinary)

  // PROFILE
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtProfile_V1_t, QairtProfile_Handle_t, profile)
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtProfile_ConfigV1_t,
                                 QairtProfile_ConfigHandle_t,
                                 profileConfig)
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtProfile_EventV1_t, QairtProfile_EventHandle_t, profileEvent)
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtProfile_BackendOpaqueObjectV1_t,
                                 QairtProfile_BackendOpaqueObjectHandle_t,
                                 profileBackendOpaqueObject)

  // SIGNAL
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtSignal_V1_t, QairtSignal_Handle_t, signal)
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtSignal_ConfigV1_t, QairtSignal_ConfigHandle_t, signalConfig)

  QAIRT_INTERFACE_DECLARE_MEMBER(QairtInfo_V1_t, void*, info)

  // GLOBAL CONFIG
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtGlobalConfig_V1_t, void*, globalConfig)

  // ERROR REPORTING CONFIG
  QAIRT_INTERFACE_DECLARE_MEMBER(QairtErrorReporting_Config_V1_t,
                                 QairtErrorReporting_ConfigHandle_t,
                                 errorReportingConfig)

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

  static ApiTable create(void* dlHandle, decltype(QairtInterface_getInterface) getInterfaceFn) {
    using detail::populateInterface;
    ApiTable api;
    api.m_dlHandle = std::make_shared<DlHandleHolder>(dlHandle);

    api.backend = populateInterface<QAIRT_BACKEND_V1_ID,
                                    &QairtBackend_V1_t::create,
                                    &QairtBackend_V1_t::free,
                                    &QairtBackend_V1_t::setConfig,
                                    &QairtBackend_V1_t::registerOpPackage,
                                    &QairtBackend_V1_t::getSupportedOperations,
                                    &QairtBackend_V1_t::validateOpConfig,
                                    &QairtBackend_V1_t::createProfile>(getInterfaceFn);

    api.backendConfig =
        populateInterface<QAIRT_BACKEND_CONFIG_V1_ID,
                          &QairtBackend_ConfigV1_t::create,
                          &QairtBackend_ConfigV1_t::free,
                          &QairtBackend_ConfigV1_t::setCustomConfigs,
                          &QairtBackend_ConfigV1_t::getNumCustomConfigs,
                          &QairtBackend_ConfigV1_t::getCustomConfigAt,
                          &QairtBackend_ConfigV1_t::setErrorReportingConfig,
                          &QairtBackend_ConfigV1_t::getErrorReportingConfig,
                          &QairtBackend_ConfigV1_t::getNumPlatformOptions,
                          &QairtBackend_ConfigV1_t::setPlatformOptions,
                          &QairtBackend_ConfigV1_t::getPlatformOptionAt>(getInterfaceFn);

    api.backendOperationName =
        populateInterface<QAIRT_BACKEND_OPERATION_NAME_V1_ID,
                          &QairtBackend_OperationNameV1_t::getPackageName,
                          &QairtBackend_OperationNameV1_t::getName,
                          &QairtBackend_OperationNameV1_t::getTarget>(getInterfaceFn);

    api.logCallback = populateInterface<QAIRT_LOG_CALLBACK_V1_ID,
                                        &QairtLog_CallbackV1_t::create,
                                        &QairtLog_CallbackV1_t::free>(getInterfaceFn);
    api.log         = populateInterface<QAIRT_LOG_V1_ID,
                                &QairtLog_V1_t::create,
                                &QairtLog_V1_t::free,
                                &QairtLog_V1_t::setLogLevel>(getInterfaceFn);

    api.tensorProperties =
        populateInterface<QAIRT_TENSOR_PROPERTIES_V1_ID,
                          &QairtTensor_PropertiesV1_t::create,
                          &QairtTensor_PropertiesV1_t::free,
                          &QairtTensor_PropertiesV1_t::setIsInput,
                          &QairtTensor_PropertiesV1_t::getIsInput,
                          &QairtTensor_PropertiesV1_t::setIsOutput,
                          &QairtTensor_PropertiesV1_t::getIsOutput,
                          &QairtTensor_PropertiesV1_t::setIsNative,
                          &QairtTensor_PropertiesV1_t::getIsNative,
                          &QairtTensor_PropertiesV1_t::setIsNull,
                          &QairtTensor_PropertiesV1_t::getIsNull,
                          &QairtTensor_PropertiesV1_t::setIsStatic,
                          &QairtTensor_PropertiesV1_t::getIsStatic,
                          &QairtTensor_PropertiesV1_t::setIsOptional,
                          &QairtTensor_PropertiesV1_t::getIsOptional,
                          &QairtTensor_PropertiesV1_t::setIsUpdatable,
                          &QairtTensor_PropertiesV1_t::getIsUpdatable>(getInterfaceFn);

    api.tensorClientBuffer =
        populateInterface<QAIRT_TENSOR_CLIENT_BUFFER_V1_ID,
                          &QairtTensor_ClientBufferV1_t::create,
                          &QairtTensor_ClientBufferV1_t::free,
                          &QairtTensor_ClientBufferV1_t::setData,
                          &QairtTensor_ClientBufferV1_t::getData,
                          &QairtTensor_ClientBufferV1_t::setDataSize,
                          &QairtTensor_ClientBufferV1_t::getDataSize>(getInterfaceFn);

    api.tensorMemory =
        populateInterface<QAIRT_TENSOR_MEMORY_V1_ID,
                          &QairtTensor_MemoryV1_t::create,
                          &QairtTensor_MemoryV1_t::free,
                          &QairtTensor_MemoryV1_t::getMemoryType,
                          &QairtTensor_MemoryV1_t::setMemHandle,
                          &QairtTensor_MemoryV1_t::getMemHandle,
                          &QairtTensor_MemoryV1_t::setClientBuffer,
                          &QairtTensor_MemoryV1_t::getClientBuffer,
                          &QairtTensor_MemoryV1_t::setRawRetrieveCallbacks,
                          &QairtTensor_MemoryV1_t::getRawRetrieveCallbacks>(getInterfaceFn);

    api.tensor = populateInterface<QAIRT_TENSOR_V1_ID,
                                   &QairtTensor_V1_t::create,
                                   &QairtTensor_V1_t::free,
                                   &QairtTensor_V1_t::setId,
                                   &QairtTensor_V1_t::getId,
                                   &QairtTensor_V1_t::getIsProduced,
                                   &QairtTensor_V1_t::setName,
                                   &QairtTensor_V1_t::getName,
                                   &QairtTensor_V1_t::setTensorProperties,
                                   &QairtTensor_V1_t::getTensorProperties,
                                   &QairtTensor_V1_t::setDataFormat,
                                   &QairtTensor_V1_t::getDataFormat,
                                   &QairtTensor_V1_t::setDataType,
                                   &QairtTensor_V1_t::getDataType,
                                   &QairtTensor_V1_t::getRank,
                                   &QairtTensor_V1_t::setDims,
                                   &QairtTensor_V1_t::getDims,
                                   &QairtTensor_V1_t::setTensorMemory,
                                   &QairtTensor_V1_t::getTensorMemory,
                                   &QairtTensor_V1_t::setIsDynamicDimensions,
                                   &QairtTensor_V1_t::getIsDynamicDimensions,
                                   &QairtTensor_V1_t::setSparseParams,
                                   &QairtTensor_V1_t::getSparseParams,
                                   &QairtTensor_V1_t::setQuantizeParams,
                                   &QairtTensor_V1_t::getQuantizeParams>(getInterfaceFn);

    api.scalar   = populateInterface<QAIRT_SCALAR_V1_ID,
                                   &QairtScalar_V1_t::create,
                                   &QairtScalar_V1_t::free,
                                   &QairtScalar_V1_t::getDataType,
                                   &QairtScalar_V1_t::getFloatValue,
                                   &QairtScalar_V1_t::getDoubleValue,
                                   &QairtScalar_V1_t::getUint64Value,
                                   &QairtScalar_V1_t::getUint32Value,
                                   &QairtScalar_V1_t::getUint16Value,
                                   &QairtScalar_V1_t::getUint8Value,
                                   &QairtScalar_V1_t::getInt64Value,
                                   &QairtScalar_V1_t::getInt32Value,
                                   &QairtScalar_V1_t::getInt16Value,
                                   &QairtScalar_V1_t::getInt8Value,
                                   &QairtScalar_V1_t::getStringValue,
                                   &QairtScalar_V1_t::getBoolValue,
                                   &QairtScalar_V1_t::setFloatValue,
                                   &QairtScalar_V1_t::setDoubleValue,
                                   &QairtScalar_V1_t::setUint64Value,
                                   &QairtScalar_V1_t::setUint32Value,
                                   &QairtScalar_V1_t::setUint16Value,
                                   &QairtScalar_V1_t::setUint8Value,
                                   &QairtScalar_V1_t::setInt64Value,
                                   &QairtScalar_V1_t::setInt32Value,
                                   &QairtScalar_V1_t::setInt16Value,
                                   &QairtScalar_V1_t::setInt8Value,
                                   &QairtScalar_V1_t::setStringValue,
                                   &QairtScalar_V1_t::setBoolValue>(getInterfaceFn);
    api.param    = populateInterface<QAIRT_PARAM_V1_ID,
                                  &QairtOpConfig_ParamV1_t::create,
                                  &QairtOpConfig_ParamV1_t::free,
                                  &QairtOpConfig_ParamV1_t::getParamType,
                                  &QairtOpConfig_ParamV1_t::getName,
                                  &QairtOpConfig_ParamV1_t::getScalarParam,
                                  &QairtOpConfig_ParamV1_t::getTensorParam,
                                  &QairtOpConfig_ParamV1_t::setName,
                                  &QairtOpConfig_ParamV1_t::setScalarParam,
                                  &QairtOpConfig_ParamV1_t::setTensorParam>(getInterfaceFn);
    api.opConfig = populateInterface<QAIRT_OPCONFIG_V1_ID,
                                     &QairtOpConfig_V1_t::create,
                                     &QairtOpConfig_V1_t::free,
                                     &QairtOpConfig_V1_t::setName,
                                     &QairtOpConfig_V1_t::getName,
                                     &QairtOpConfig_V1_t::setPackageName,
                                     &QairtOpConfig_V1_t::getPackageName,
                                     &QairtOpConfig_V1_t::setTypeName,
                                     &QairtOpConfig_V1_t::getTypeName,
                                     &QairtOpConfig_V1_t::setParams,
                                     &QairtOpConfig_V1_t::getNumParams,
                                     &QairtOpConfig_V1_t::getParamAt,
                                     &QairtOpConfig_V1_t::setInputTensors,
                                     &QairtOpConfig_V1_t::getNumInputTensors,
                                     &QairtOpConfig_V1_t::getInputTensorAt,
                                     &QairtOpConfig_V1_t::setOutputTensors,
                                     &QairtOpConfig_V1_t::getNumOutputTensors,
                                     &QairtOpConfig_V1_t::getOutputTensorAt>(getInterfaceFn);

    api.scaleOffset =
        populateInterface<QAIRT_SCALE_OFFSET_V1_ID,
                          &QairtQuantizeParams_ScaleOffsetV1_t::create,
                          &QairtQuantizeParams_ScaleOffsetV1_t::free,
                          &QairtQuantizeParams_ScaleOffsetV1_t::setScale,
                          &QairtQuantizeParams_ScaleOffsetV1_t::getScale,
                          &QairtQuantizeParams_ScaleOffsetV1_t::setOffset,
                          &QairtQuantizeParams_ScaleOffsetV1_t::getOffset>(getInterfaceFn);

    api.axisScaleOffset =
        populateInterface<QAIRT_AXIS_SCALE_OFFSET_V1_ID,
                          &QairtQuantizeParams_AxisScaleOffsetV1_t::create,
                          &QairtQuantizeParams_AxisScaleOffsetV1_t::free,
                          &QairtQuantizeParams_AxisScaleOffsetV1_t::setScaleOffsets,
                          &QairtQuantizeParams_AxisScaleOffsetV1_t::getNumScaleOffsets,
                          &QairtQuantizeParams_AxisScaleOffsetV1_t::getScaleOffsetAt,
                          &QairtQuantizeParams_AxisScaleOffsetV1_t::setAxis,
                          &QairtQuantizeParams_AxisScaleOffsetV1_t::getAxis>(getInterfaceFn);

    api.bwScaleOffset =
        populateInterface<QAIRT_BW_SCALE_OFFSET_V1_ID,
                          &QairtQuantizeParams_BwScaleOffsetV1_t::create,
                          &QairtQuantizeParams_BwScaleOffsetV1_t::free,
                          &QairtQuantizeParams_BwScaleOffsetV1_t::setScale,
                          &QairtQuantizeParams_BwScaleOffsetV1_t::getScale,
                          &QairtQuantizeParams_BwScaleOffsetV1_t::setOffset,
                          &QairtQuantizeParams_BwScaleOffsetV1_t::getOffset,
                          &QairtQuantizeParams_BwScaleOffsetV1_t::setBw,
                          &QairtQuantizeParams_BwScaleOffsetV1_t::getBw>(getInterfaceFn);

    api.bwAxisScaleOffset =
        populateInterface<QAIRT_BW_AXIS_SCALE_OFFSET_V1_ID,
                          &QairtQuantizeParams_BwAxisScaleOffsetV1_t::create,
                          &QairtQuantizeParams_BwAxisScaleOffsetV1_t::free,
                          &QairtQuantizeParams_BwAxisScaleOffsetV1_t::setScales,
                          &QairtQuantizeParams_BwAxisScaleOffsetV1_t::getNumScales,
                          &QairtQuantizeParams_BwAxisScaleOffsetV1_t::getScaleAt,
                          &QairtQuantizeParams_BwAxisScaleOffsetV1_t::setOffsets,
                          &QairtQuantizeParams_BwAxisScaleOffsetV1_t::getNumOffsets,
                          &QairtQuantizeParams_BwAxisScaleOffsetV1_t::getOffsetAt,
                          &QairtQuantizeParams_BwAxisScaleOffsetV1_t::setAxis,
                          &QairtQuantizeParams_BwAxisScaleOffsetV1_t::getAxis,
                          &QairtQuantizeParams_BwAxisScaleOffsetV1_t::setBw,
                          &QairtQuantizeParams_BwAxisScaleOffsetV1_t::getBw>(getInterfaceFn);

    api.bwAxisScaleOffsetMapped =
        populateInterface<QAIRT_BW_AXIS_SCALE_OFFSET_MAPPED_V1_ID,
                          &QairtQuantizeParams_BwAxisScaleOffsetMappedV1_t::create,
                          &QairtQuantizeParams_BwAxisScaleOffsetMappedV1_t::free,
                          &QairtQuantizeParams_BwAxisScaleOffsetMappedV1_t::setScales,
                          &QairtQuantizeParams_BwAxisScaleOffsetMappedV1_t::getNumScales,
                          &QairtQuantizeParams_BwAxisScaleOffsetMappedV1_t::getScaleAt,
                          &QairtQuantizeParams_BwAxisScaleOffsetMappedV1_t::setOffsets,
                          &QairtQuantizeParams_BwAxisScaleOffsetMappedV1_t::getNumOffsets,
                          &QairtQuantizeParams_BwAxisScaleOffsetMappedV1_t::getOffsetAt,
                          &QairtQuantizeParams_BwAxisScaleOffsetMappedV1_t::setAxis,
                          &QairtQuantizeParams_BwAxisScaleOffsetMappedV1_t::getAxis,
                          &QairtQuantizeParams_BwAxisScaleOffsetMappedV1_t::setBw,
                          &QairtQuantizeParams_BwAxisScaleOffsetMappedV1_t::getBw,
                          &QairtQuantizeParams_BwAxisScaleOffsetMappedV1_t::setMapping,
                          &QairtQuantizeParams_BwAxisScaleOffsetMappedV1_t::getMapping>(
            getInterfaceFn);

    api.microscalingEncoding =
        populateInterface<QAIRT_MICROSCALING_ENCODING_V1_ID,
                          &QairtQuantizeParams_MicroscalingEncodingV1_t::create,
                          &QairtQuantizeParams_MicroscalingEncodingV1_t::free,
                          &QairtQuantizeParams_MicroscalingEncodingV1_t::setValueEncoding,
                          &QairtQuantizeParams_MicroscalingEncodingV1_t::getValueEncoding,
                          &QairtQuantizeParams_MicroscalingEncodingV1_t::setBlockDimensions,
                          &QairtQuantizeParams_MicroscalingEncodingV1_t::getNumBlockDimensions,
                          &QairtQuantizeParams_MicroscalingEncodingV1_t::getBlockDimensionAt,
                          &QairtQuantizeParams_MicroscalingEncodingV1_t::getNumBlockScales,
                          &QairtQuantizeParams_MicroscalingEncodingV1_t::setScaleDataType,
                          &QairtQuantizeParams_MicroscalingEncodingV1_t::getScaleDataType,
                          &QairtQuantizeParams_MicroscalingEncodingV1_t::setBlockScales8,
                          &QairtQuantizeParams_MicroscalingEncodingV1_t::getBlockScale8At,
                          &QairtQuantizeParams_MicroscalingEncodingV1_t::setBlockScalesFloat,
                          &QairtQuantizeParams_MicroscalingEncodingV1_t::getBlockScaleFloatAt>(
            getInterfaceFn);

    api.floatScaleOffset =
        populateInterface<QAIRT_FLOAT_SCALE_OFFSET_V1_ID,
                          &QairtQuantizeParams_FloatScaleOffsetV1_t::create,
                          &QairtQuantizeParams_FloatScaleOffsetV1_t::free,
                          &QairtQuantizeParams_FloatScaleOffsetV1_t::setScale,
                          &QairtQuantizeParams_FloatScaleOffsetV1_t::getScale,
                          &QairtQuantizeParams_FloatScaleOffsetV1_t::setOffset,
                          &QairtQuantizeParams_FloatScaleOffsetV1_t::getOffset>(getInterfaceFn);

    api.bwFloatBlockEncoding =
        populateInterface<QAIRT_BW_FLOAT_BLOCK_ENCODING_V1_ID,
                          &QairtQuantizeParams_BwFloatBlockEncodingV1_t::create,
                          &QairtQuantizeParams_BwFloatBlockEncodingV1_t::free,
                          &QairtQuantizeParams_BwFloatBlockEncodingV1_t::setBw,
                          &QairtQuantizeParams_BwFloatBlockEncodingV1_t::getBw,
                          &QairtQuantizeParams_BwFloatBlockEncodingV1_t::setBlockSize,
                          &QairtQuantizeParams_BwFloatBlockEncodingV1_t::getNumBlockSizes,
                          &QairtQuantizeParams_BwFloatBlockEncodingV1_t::getBlockSizeAt,
                          &QairtQuantizeParams_BwFloatBlockEncodingV1_t::setFloatScaleOffsets,
                          &QairtQuantizeParams_BwFloatBlockEncodingV1_t::getNumFloatScaleOffsets,
                          &QairtQuantizeParams_BwFloatBlockEncodingV1_t::getFloatScaleOffsetAt>(
            getInterfaceFn);

    api.floatBlockEncoding =
        populateInterface<QAIRT_FLOAT_BLOCK_ENCODING_V1_ID,
                          &QairtQuantizeParams_FloatBlockEncodingV1_t::create,
                          &QairtQuantizeParams_FloatBlockEncodingV1_t::free,
                          &QairtQuantizeParams_FloatBlockEncodingV1_t::setBlockSize,
                          &QairtQuantizeParams_FloatBlockEncodingV1_t::getNumBlockSizes,
                          &QairtQuantizeParams_FloatBlockEncodingV1_t::getBlockSizeAt,
                          &QairtQuantizeParams_FloatBlockEncodingV1_t::setFloatScaleOffsets,
                          &QairtQuantizeParams_FloatBlockEncodingV1_t::getNumFloatScaleOffsets,
                          &QairtQuantizeParams_FloatBlockEncodingV1_t::getFloatScaleOffsetAt>(
            getInterfaceFn);

    api.bwBlockMapped =
        populateInterface<QAIRT_BW_BLOCK_MAPPED_V1_ID,
                          &QairtQuantizeParams_BwBlockMappedV1_t::create,
                          &QairtQuantizeParams_BwBlockMappedV1_t::free,
                          &QairtQuantizeParams_BwBlockMappedV1_t::setBw,
                          &QairtQuantizeParams_BwBlockMappedV1_t::getBw,
                          &QairtQuantizeParams_BwBlockMappedV1_t::setMapping,
                          &QairtQuantizeParams_BwBlockMappedV1_t::getMapping,
                          &QairtQuantizeParams_BwBlockMappedV1_t::setBlockSize,
                          &QairtQuantizeParams_BwBlockMappedV1_t::getNumBlockSizes,
                          &QairtQuantizeParams_BwBlockMappedV1_t::getBlockSizeAt,
                          &QairtQuantizeParams_BwBlockMappedV1_t::setScaleOffsets,
                          &QairtQuantizeParams_BwBlockMappedV1_t::getNumScaleOffsets,
                          &QairtQuantizeParams_BwBlockMappedV1_t::getScaleOffsetAt>(getInterfaceFn);

    api.bwBlockwiseExpansionMapped = populateInterface<
        QAIRT_BW_BLOCKWISE_EXPANSION_MAPPED_V1_ID,
        &QairtQuantizeParams_BwBlockwiseExpansionMappedV1_t::create,
        &QairtQuantizeParams_BwBlockwiseExpansionMappedV1_t::free,
        &QairtQuantizeParams_BwBlockwiseExpansionMappedV1_t::setBw,
        &QairtQuantizeParams_BwBlockwiseExpansionMappedV1_t::getBw,
        &QairtQuantizeParams_BwBlockwiseExpansionMappedV1_t::setMapping,
        &QairtQuantizeParams_BwBlockwiseExpansionMappedV1_t::getMapping,
        &QairtQuantizeParams_BwBlockwiseExpansionMappedV1_t::setAxis,
        &QairtQuantizeParams_BwBlockwiseExpansionMappedV1_t::getAxis,
        &QairtQuantizeParams_BwBlockwiseExpansionMappedV1_t::setScaleOffsets,
        &QairtQuantizeParams_BwBlockwiseExpansionMappedV1_t::getNumScaleOffsets,
        &QairtQuantizeParams_BwBlockwiseExpansionMappedV1_t::getScaleOffsetAt,
        &QairtQuantizeParams_BwBlockwiseExpansionMappedV1_t::setNumBlocksPerAxis,
        &QairtQuantizeParams_BwBlockwiseExpansionMappedV1_t::getNumBlocksPerAxis,
        &QairtQuantizeParams_BwBlockwiseExpansionMappedV1_t::setBlockScaleBitwidth,
        &QairtQuantizeParams_BwBlockwiseExpansionMappedV1_t::getBlockScaleBitwidth,
        &QairtQuantizeParams_BwBlockwiseExpansionMappedV1_t::setBlocksScale8,
        &QairtQuantizeParams_BwBlockwiseExpansionMappedV1_t::getNumBlocksScale8,
        &QairtQuantizeParams_BwBlockwiseExpansionMappedV1_t::getBlocksScale8At,
        &QairtQuantizeParams_BwBlockwiseExpansionMappedV1_t::setBlocksScale16,
        &QairtQuantizeParams_BwBlockwiseExpansionMappedV1_t::getNumBlocksScale16,
        &QairtQuantizeParams_BwBlockwiseExpansionMappedV1_t::getBlocksScale16At>(getInterfaceFn);

    api.blockEncoding =
        populateInterface<QAIRT_BLOCK_ENCODING_V1_ID,
                          &QairtQuantizeParams_BlockEncodingV1_t::create,
                          &QairtQuantizeParams_BlockEncodingV1_t::free,
                          &QairtQuantizeParams_BlockEncodingV1_t::setBlockSize,
                          &QairtQuantizeParams_BlockEncodingV1_t::getNumBlockSize,
                          &QairtQuantizeParams_BlockEncodingV1_t::getBlockSizeAt,
                          &QairtQuantizeParams_BlockEncodingV1_t::setScaleOffsets,
                          &QairtQuantizeParams_BlockEncodingV1_t::getNumScaleOffsets,
                          &QairtQuantizeParams_BlockEncodingV1_t::getScaleOffsetAt>(getInterfaceFn);

    api.vectorEncoding =
        populateInterface<QAIRT_VECTOR_ENCODING_V1_ID,
                          &QairtQuantizeParams_VectorEncodingV1_t::create,
                          &QairtQuantizeParams_VectorEncodingV1_t::free,
                          &QairtQuantizeParams_VectorEncodingV1_t::setBwAxisScaleOffset,
                          &QairtQuantizeParams_VectorEncodingV1_t::getBwAxisScaleOffset,
                          &QairtQuantizeParams_VectorEncodingV1_t::setRowsPerBlock,
                          &QairtQuantizeParams_VectorEncodingV1_t::getRowsPerBlock,
                          &QairtQuantizeParams_VectorEncodingV1_t::setColsPerBlock,
                          &QairtQuantizeParams_VectorEncodingV1_t::getColsPerBlock,
                          &QairtQuantizeParams_VectorEncodingV1_t::setVectorStride,
                          &QairtQuantizeParams_VectorEncodingV1_t::getVectorStride,
                          &QairtQuantizeParams_VectorEncodingV1_t::setVectorDimension,
                          &QairtQuantizeParams_VectorEncodingV1_t::getVectorDimension,
                          &QairtQuantizeParams_VectorEncodingV1_t::setIndexBw,
                          &QairtQuantizeParams_VectorEncodingV1_t::getIndexBw>(getInterfaceFn);

    api.blockwiseExpansion =
        populateInterface<QAIRT_BLOCKWISE_EXPANSION_V1_ID,
                          &QairtQuantizeParams_BlockwiseExpansionV1_t::create,
                          &QairtQuantizeParams_BlockwiseExpansionV1_t::free,
                          &QairtQuantizeParams_BlockwiseExpansionV1_t::setAxis,
                          &QairtQuantizeParams_BlockwiseExpansionV1_t::getAxis,
                          &QairtQuantizeParams_BlockwiseExpansionV1_t::setScaleOffsets,
                          &QairtQuantizeParams_BlockwiseExpansionV1_t::getNumScaleOffsets,
                          &QairtQuantizeParams_BlockwiseExpansionV1_t::getScaleOffsetAt,
                          &QairtQuantizeParams_BlockwiseExpansionV1_t::setNumBlocksPerAxis,
                          &QairtQuantizeParams_BlockwiseExpansionV1_t::getNumBlocksPerAxis,
                          &QairtQuantizeParams_BlockwiseExpansionV1_t::setBlockScaleBw,
                          &QairtQuantizeParams_BlockwiseExpansionV1_t::getBlockScaleBw,
                          &QairtQuantizeParams_BlockwiseExpansionV1_t::setBlockScale8,
                          &QairtQuantizeParams_BlockwiseExpansionV1_t::getNumBlockScale8,
                          &QairtQuantizeParams_BlockwiseExpansionV1_t::getBlockScale8At,
                          &QairtQuantizeParams_BlockwiseExpansionV1_t::setBlockScale16,
                          &QairtQuantizeParams_BlockwiseExpansionV1_t::getNumBlockScale16,
                          &QairtQuantizeParams_BlockwiseExpansionV1_t::getBlockScale16At,
                          &QairtQuantizeParams_BlockwiseExpansionV1_t::getStorageType>(
            getInterfaceFn);

    api.quantizeParams =
        populateInterface<QAIRT_QUANTIZE_PARAMS_V1_ID,
                          &QairtQuantizeParams_V1_t::create,
                          &QairtQuantizeParams_V1_t::free,
                          &QairtQuantizeParams_V1_t::setScaleOffset,
                          &QairtQuantizeParams_V1_t::getScaleOffset,
                          &QairtQuantizeParams_V1_t::setAxisScaleOffset,
                          &QairtQuantizeParams_V1_t::getAxisScaleOffset,
                          &QairtQuantizeParams_V1_t::setBwScaleOffset,
                          &QairtQuantizeParams_V1_t::getBwScaleOffset,
                          &QairtQuantizeParams_V1_t::setBwAxisScaleOffset,
                          &QairtQuantizeParams_V1_t::getBwAxisScaleOffset,
                          &QairtQuantizeParams_V1_t::setBlockEncoding,
                          &QairtQuantizeParams_V1_t::getBlockEncoding,
                          &QairtQuantizeParams_V1_t::setBlockwiseExpansion,
                          &QairtQuantizeParams_V1_t::getBlockwiseExpansion,
                          &QairtQuantizeParams_V1_t::setVectorEncoding,
                          &QairtQuantizeParams_V1_t::getVectorEncoding,
                          &QairtQuantizeParams_V1_t::getQuantEncoding,
                          &QairtQuantizeParams_V1_t::setBwAxisScaleOffsetMapped,
                          &QairtQuantizeParams_V1_t::getBwAxisScaleOffsetMapped,
                          &QairtQuantizeParams_V1_t::setMicroscalingEncoding,
                          &QairtQuantizeParams_V1_t::getMicroscalingEncoding,
                          &QairtQuantizeParams_V1_t::setBwFloatBlockEncoding,
                          &QairtQuantizeParams_V1_t::getBwFloatBlockEncoding,
                          &QairtQuantizeParams_V1_t::setBwBlockMapped,
                          &QairtQuantizeParams_V1_t::getBwBlockMapped,
                          &QairtQuantizeParams_V1_t::setBwBlockwiseExpansionMapped,
                          &QairtQuantizeParams_V1_t::getBwBlockwiseExpansionMapped,
                          &QairtQuantizeParams_V1_t::setFloatBlockEncoding,
                          &QairtQuantizeParams_V1_t::getFloatBlockEncoding>(getInterfaceFn);

    api.memDescriptor = populateInterface<QAIRT_MEM_DESCRIPTOR_V1_ID,
                                          &QairtMem_DescriptorV1_t::create,
                                          &QairtMem_DescriptorV1_t::free,
                                          &QairtMem_DescriptorV1_t::setMemShape,
                                          &QairtMem_DescriptorV1_t::getMemShape,
                                          &QairtMem_DescriptorV1_t::setDataType,
                                          &QairtMem_DescriptorV1_t::getDataType,
                                          &QairtMem_DescriptorV1_t::getMemType,
                                          &QairtMem_DescriptorV1_t::setCustomInfo,
                                          &QairtMem_DescriptorV1_t::getCustomInfo,
                                          &QairtMem_DescriptorV1_t::setDmaBuf,
                                          &QairtMem_DescriptorV1_t::getDmaBuf,
                                          &QairtMem_DescriptorV1_t::deregisterMem>(getInterfaceFn);

    api.memShape = populateInterface<QAIRT_MEM_SHAPE_V1_ID,
                                     &QairtMem_ShapeV1_t::create,
                                     &QairtMem_ShapeV1_t::free,
                                     &QairtMem_ShapeV1_t::getRank,
                                     &QairtMem_ShapeV1_t::setDimensions,
                                     &QairtMem_ShapeV1_t::getDimensionAt,
                                     &QairtMem_ShapeV1_t::setShapeConfig,
                                     &QairtMem_ShapeV1_t::getShapeConfig>(getInterfaceFn);

    api.device = populateInterface<QAIRT_DEVICE_V1_ID,
                                   &QairtDevice_V1_t::create,
                                   &QairtDevice_V1_t::free,
                                   &QairtDevice_V1_t::setConfig,
                                   &QairtDevice_V1_t::getConfig>(getInterfaceFn);

    api.deviceConfig =
        populateInterface<QAIRT_DEVICE_CONFIG_V1_ID,
                          &QairtDevice_ConfigV1_t::create,
                          &QairtDevice_ConfigV1_t::free,
                          &QairtDevice_ConfigV1_t::setPlatformInfo,
                          &QairtDevice_ConfigV1_t::getPlatformInfo,
                          &QairtDevice_ConfigV1_t::setCustomConfig,
                          &QairtDevice_ConfigV1_t::getNumCustomConfig,
                          &QairtDevice_ConfigV1_t::getCustomConfigAt,
                          &QairtDevice_ConfigV1_t::clearCustomConfigs>(getInterfaceFn);

    api.deviceInfrastructureProvider =
        populateInterface<QAIRT_DEVICE_INRFA_PROVIDER_V1_ID,
                          &QairtDevice_InfrastructureProviderV1_t::getDeviceInfra>(getInterfaceFn);

    api.devicePlatformInfoProvider =
        populateInterface<QAIRT_DEVICE_PLATFORM_INFO_PROVIDER_V1_ID,
                          &QairtDevice_PlatformInfoProviderV1_t::create,
                          &QairtDevice_PlatformInfoProviderV1_t::getPlatformInfo,
                          &QairtDevice_PlatformInfoProviderV1_t::free>(getInterfaceFn);

    api.devicePlatformInfo =
        populateInterface<QAIRT_DEVICE_PLATFORM_INFO_V1_ID,
                          &QairtDevice_PlatformInfoV1_t::getNumHwDeviceInfo,
                          &QairtDevice_PlatformInfoV1_t::getHwDeviceInfoAt>(getInterfaceFn);

    api.deviceHardwareDeviceInfo =
        populateInterface<QAIRT_DEVICE_HW_DEVICE_INFO_V1_ID,
                          &QairtDevice_HardwareDeviceInfoV1_t::getDeviceId,
                          &QairtDevice_HardwareDeviceInfoV1_t::getDeviceType,
                          &QairtDevice_HardwareDeviceInfoV1_t::getNumCoreInfos,
                          &QairtDevice_HardwareDeviceInfoV1_t::getHardwareCoreInfoAt,
                          &QairtDevice_HardwareDeviceInfoV1_t::getHardwareDeviceExtendedInfo>(
            getInterfaceFn);

    api.deviceHardwareCoreInfo =
        populateInterface<QAIRT_DEVICE_HW_CORE_INFO_V1_ID,
                          &QairtDevice_HardwareCoreInfoV1_t::getCoreId,
                          &QairtDevice_HardwareCoreInfoV1_t::getCoreType,
                          &QairtDevice_HardwareCoreInfoV1_t::getHardwareCoreExtendedInfo>(
            getInterfaceFn);

    api.graph = populateInterface<QAIRT_GRAPH_V1_ID,
                                  &QairtGraph_V1_t::free,
                                  &QairtGraph_V1_t::createSubgraph,
                                  &QairtGraph_V1_t::setConfig,
                                  &QairtGraph_V1_t::addNode,
                                  &QairtGraph_V1_t::finalize,
                                  &QairtGraph_V1_t::createGraphTensor,
                                  &QairtGraph_V1_t::execute,
                                  &QairtGraph_V1_t::executeAsync>(getInterfaceFn);

    api.graphConfig =
        populateInterface<QAIRT_GRAPH_CONFIG_V1_ID,
                          &QairtGraph_ConfigV1_t::create,
                          &QairtGraph_ConfigV1_t::free,
                          &QairtGraph_ConfigV1_t::getNumCustomConfigs,
                          &QairtGraph_ConfigV1_t::getCustomConfigAt,
                          &QairtGraph_ConfigV1_t::getPriority,
                          &QairtGraph_ConfigV1_t::getProfileHandle,
                          &QairtGraph_ConfigV1_t::getProfilingState,
                          &QairtGraph_ConfigV1_t::getNumProfilingExecutions,
                          &QairtGraph_ConfigV1_t::setCustomConfigs,
                          &QairtGraph_ConfigV1_t::setPriority,
                          &QairtGraph_ConfigV1_t::setProfileHandle,
                          &QairtGraph_ConfigV1_t::setProfilingState,
                          &QairtGraph_ConfigV1_t::setNumProfilingExecutions>(getInterfaceFn);

    api.context = populateInterface<QAIRT_CONTEXT_V1_ID,
                                    &QairtContext_V1_t::create,
                                    &QairtContext_V1_t::free,
                                    &QairtContext_V1_t::setConfig,
                                    &QairtContext_V1_t::getBinarySize,
                                    &QairtContext_V1_t::getBinary,
                                    &QairtContext_V1_t::validateBinary,
                                    &QairtContext_V1_t::createFromBinary,
                                    &QairtContext_V1_t::finalize,
                                    &QairtContext_V1_t::createGraph,
                                    &QairtContext_V1_t::retrieveGraph>(getInterfaceFn);

    api.contextAsyncExecutionDepth =
        populateInterface<QAIRT_CONTEXT_ASYNC_EXEC_DEPTH_V1_ID,
                          &QairtContext_AsyncExecutionDepthV1_t::create,
                          &QairtContext_AsyncExecutionDepthV1_t::free,
                          &QairtContext_AsyncExecutionDepthV1_t::setAsyncExecutionDepth,
                          &QairtContext_AsyncExecutionDepthV1_t::getAsyncExecutionDepth>(
            getInterfaceFn);

    api.contextConfig =
        populateInterface<QAIRT_CONTEXT_CONFIG_V1_ID,
                          &QairtContext_ConfigV1_t::create,
                          &QairtContext_ConfigV1_t::free,
                          &QairtContext_ConfigV1_t::setCustomConfigs,
                          &QairtContext_ConfigV1_t::getNumCustomConfigs,
                          &QairtContext_ConfigV1_t::getCustomConfigAt,
                          &QairtContext_ConfigV1_t::setPriority,
                          &QairtContext_ConfigV1_t::getPriority,
                          &QairtContext_ConfigV1_t::setOemKey,
                          &QairtContext_ConfigV1_t::getOemKey,
                          &QairtContext_ConfigV1_t::setAsyncQueueDepth,
                          &QairtContext_ConfigV1_t::getAsyncQueueDepth,
                          &QairtContext_ConfigV1_t::setEnableGraph,
                          &QairtContext_ConfigV1_t::getNumEnabledGraphs,
                          &QairtContext_ConfigV1_t::getEnabledGraphAt,
                          &QairtContext_ConfigV1_t::setMemoryLimitHint,
                          &QairtContext_ConfigV1_t::getMemoryLimitHint,
                          &QairtContext_ConfigV1_t::setIsPersistentBinary,
                          &QairtContext_ConfigV1_t::getIsPersistentBinary,
                          &QairtContext_ConfigV1_t::setBinaryCompatibilityType,
                          &QairtContext_ConfigV1_t::getBinaryCompatibilityType>(getInterfaceFn);

    api.contextBinaryBuffer =
        populateInterface<QAIRT_CONTEXT_BINARY_BUFFER_V1_ID,
                          &QairtContext_BinaryBufferV1_t::create,
                          &QairtContext_BinaryBufferV1_t::free,
                          &QairtContext_BinaryBufferV1_t::setData,
                          &QairtContext_BinaryBufferV1_t::getData,
                          &QairtContext_BinaryBufferV1_t::setSize,
                          &QairtContext_BinaryBufferV1_t::getSize>(getInterfaceFn);

    api.contextBinary = populateInterface<QAIRT_CONTEXT_BINARY_V1_ID,
                                          &QairtContext_BinaryV1_t::getType,
                                          &QairtContext_BinaryV1_t::setBuffer,
                                          &QairtContext_BinaryV1_t::getBuffer>(getInterfaceFn);

    api.profile = populateInterface<QAIRT_PROFILE_V1_ID,
                                    &QairtProfile_V1_t::free,
                                    &QairtProfile_V1_t::setConfig,
                                    &QairtProfile_V1_t::getNumEvents,
                                    &QairtProfile_V1_t::getEventAt,
                                    &QairtProfile_V1_t::getNumSubEvents,
                                    &QairtProfile_V1_t::getSubEventAt>(getInterfaceFn);

    api.profileConfig =
        populateInterface<QAIRT_PROFILE_CONFIG_V1_ID,
                          &QairtProfile_ConfigV1_t::create,
                          &QairtProfile_ConfigV1_t::free,
                          &QairtProfile_ConfigV1_t::setEnableOpTrace,
                          &QairtProfile_ConfigV1_t::getEnableOpTrace,
                          &QairtProfile_ConfigV1_t::setMaxEvents,
                          &QairtProfile_ConfigV1_t::getMaxEvents,
                          &QairtProfile_ConfigV1_t::setCustomConfig,
                          &QairtProfile_ConfigV1_t::getCustomConfig>(getInterfaceFn);

    api.profileEvent =
        populateInterface<QAIRT_PROFILE_EVENT_V1_ID,
                          &QairtProfile_EventV1_t::create,
                          &QairtProfile_EventV1_t::free,
                          &QairtProfile_EventV1_t::supportsExtendedEvent,
                          &QairtProfile_EventV1_t::setEventType,
                          &QairtProfile_EventV1_t::getEventType,
                          &QairtProfile_EventV1_t::setUnit,
                          &QairtProfile_EventV1_t::getUnit,
                          &QairtProfile_EventV1_t::setIdentifier,
                          &QairtProfile_EventV1_t::getIdentifier,
                          &QairtProfile_EventV1_t::setTimestamp,
                          &QairtProfile_EventV1_t::getTimestamp,
                          &QairtProfile_EventV1_t::setScalar,
                          &QairtProfile_EventV1_t::getScalar,
                          &QairtProfile_EventV1_t::setBackendOpaqueObject,
                          &QairtProfile_EventV1_t::getBackendOpaqueObject>(getInterfaceFn);

    api.profileBackendOpaqueObject =
        populateInterface<QAIRT_PROFILE_BACKEND_OPAQUE_OBJECT_V1_ID,
                          &QairtProfile_BackendOpaqueObjectV1_t::create,
                          &QairtProfile_BackendOpaqueObjectV1_t::free,
                          &QairtProfile_BackendOpaqueObjectV1_t::setData,
                          &QairtProfile_BackendOpaqueObjectV1_t::getData,
                          &QairtProfile_BackendOpaqueObjectV1_t::setFileName,
                          &QairtProfile_BackendOpaqueObjectV1_t::getFileName>(getInterfaceFn);

    api.signal = populateInterface<QAIRT_SIGNAL_V1_ID,
                                   &QairtSignal_V1_t::create,
                                   &QairtSignal_V1_t::free,
                                   &QairtSignal_V1_t::setConfig,
                                   &QairtSignal_V1_t::trigger>(getInterfaceFn);

    api.signalConfig =
        populateInterface<QAIRT_SIGNAL_CONFIG_V1_ID,
                          &QairtSignal_ConfigV1_t::create,
                          &QairtSignal_ConfigV1_t::free,
                          &QairtSignal_ConfigV1_t::setCustomConfig,
                          &QairtSignal_ConfigV1_t::getCustomConfig,
                          &QairtSignal_ConfigV1_t::setTimeoutDuration,
                          &QairtSignal_ConfigV1_t::getTimeoutDuration>(getInterfaceFn);

    api.info = populateInterface<QAIRT_INFO_V1_ID,
                                 &QairtInfo_V1_t::getBackendApiVersion,
                                 &QairtInfo_V1_t::getCoreApiVersion,
                                 &QairtInfo_V1_t::getBuildId,
                                 &QairtInfo_V1_t::hasCapability>(getInterfaceFn);
    api.globalConfig = populateInterface<QAIRT_GLOBAL_CONFIG_V1_ID,
                                         &QairtGlobalConfig_V1_t::setSocModel>(getInterfaceFn);
    api.errorReportingConfig =
        populateInterface<QAIRT_ERROR_REPORTING_CONFIG_V1_ID,
                          &QairtErrorReporting_Config_V1_t::create,
                          &QairtErrorReporting_Config_V1_t::free,
                          &QairtErrorReporting_Config_V1_t::getReportingLevel,
                          &QairtErrorReporting_Config_V1_t::setReportingLevel,
                          &QairtErrorReporting_Config_V1_t::getStorageLimit,
                          &QairtErrorReporting_Config_V1_t::setStorageLimit>(getInterfaceFn);

    return api;
  }

  template <auto P2m_InterfaceFn, typename... Args>
  auto call(Args&&... args) const {
    static_assert(detail::ensure_not_nullptr_v<P2m_InterfaceFn>);
    static_assert(
        detail::ensure_not_nullptr_v<p2m_to_api_member<decltype(P2m_InterfaceFn)>::value>);

    static_assert(detail::ensure_not_nullptr_v<p2mv_to_api_member_v<P2m_InterfaceFn>>);

    if (this->*p2mv_to_api_member_v<P2m_InterfaceFn>.*P2m_InterfaceFn == nullptr) {
      throw std::runtime_error("A function has not been wired up to the api table.");
    }

    return (this->*p2mv_to_api_member_v<P2m_InterfaceFn>.*
            P2m_InterfaceFn)(std::forward<Args>(args)...);
  }
};

template <typename T_Table, typename T_Interface>
using table_handle_type_t = typename T_Table::template interface_handle_type_t<T_Interface>;

class QairtApiTypeBase;

template <typename, typename, typename = ApiTable>
class ApiType;

class Api;

namespace detail {

class DummyNotCrossable {
  friend class ::qairt::Api;
};

class DummyCrossable {
  friend class ::qairt::Api;
  void prepareToCross() const {}
  void updateAfterCross() const {}
};

template <typename T, typename U, typename V>
inline constexpr std::true_type is_api_type_(const ApiType<T, U, V>&);
inline constexpr std::false_type is_api_type_(...);

template <typename T>
struct is_api_type : decltype(is_api_type_(std::declval<T>())) {};

template <typename T>
inline constexpr bool is_api_type_v = is_api_type<T>::value;

}  // namespace detail

template <typename T_BETType>
class BackendExtensionTable {
 public:
  using backend_extension_table_type = T_BETType;

  std::weak_ptr<ApiTable> m_apiTable;
  explicit BackendExtensionTable(const std::shared_ptr<ApiTable>& apiTable)
      : m_apiTable(apiTable) {}

  template <auto P2m_InterfaceFn, typename... Args>
  auto call(Args&&... args) const {
    static_assert(detail::ensure_not_nullptr_v<P2m_InterfaceFn>);

    auto& memberInterface = static_cast<const backend_extension_table_type&>(*this)
                                .template getMemberInterface<P2m_InterfaceFn>();

    if (memberInterface.*P2m_InterfaceFn == nullptr) {
      throw std::runtime_error("A function has not been wired up to the api table.");
    }

    return (memberInterface.*P2m_InterfaceFn)(std::forward<Args>(args)...);
  }
};

namespace detail {

template <typename T>
inline constexpr std::true_type is_backend_extension_table_(const BackendExtensionTable<T>&);
inline constexpr std::false_type is_backend_extension_table_(...);

template <typename T>
struct is_backend_extension_table : decltype(is_backend_extension_table_(std::declval<T>())) {};

template <typename T>
inline constexpr bool is_backend_extension_table_v = is_backend_extension_table<T>::value;

}  // namespace detail

template <typename T_BETType>
class BackendExtension;

namespace detail {

struct TempWorkaround {
  template <typename T, typename... Args>
  static T construct(Args&&... args) {
    return T(std::forward<Args>(args)...);
  }
};

template <typename T_BETType>
class TabularBEType {
 protected:
  using base_type                    = TabularBEType<T_BETType>;
  using backend_extension_table_type = T_BETType;

  std::weak_ptr<backend_extension_table_type> m_backendExtensionTable{};

 public:
  TabularBEType() = default;
  explicit TabularBEType(std::weak_ptr<backend_extension_table_type> backendExtensionTable)
      : m_backendExtensionTable{std::move(backendExtensionTable)} {}
  TabularBEType(const TabularBEType&)                = default;
  TabularBEType(TabularBEType&&) noexcept            = default;
  TabularBEType& operator=(const TabularBEType&)     = default;
  TabularBEType& operator=(TabularBEType&&) noexcept = default;

 protected:
  template <typename T, typename... Args>
  std::enable_if_t<is_api_type_v<T>, T> make(Args&&... args) const {
    static_assert(is_api_type_v<T>);

    return TempWorkaround::construct<T>(m_backendExtensionTable, std::forward<Args>(args)...);
  }
};

template <typename T>
inline constexpr std::true_type is_tabular_be_type_(const TabularBEType<T>&);
inline constexpr std::false_type is_tabular_be_type_(...);

template <typename T>
struct is_tabular_be_type : decltype(is_tabular_be_type_(std::declval<T>())) {};

template <typename T>
inline constexpr bool is_tabular_be_type_v = is_tabular_be_type<T>::value;

}  // namespace detail

template <typename T_BETType>
class BackendExtension {
  template <typename, typename, typename>
  friend class ApiType;

  using api_table_type = T_BETType;

 public:
  using backend_extension_table_type = T_BETType;
  std::shared_ptr<backend_extension_table_type> m_backendExtensionTable;

  static BackendExtension create(const std::shared_ptr<ApiTable>& apiTable,
                                 decltype(QairtInterface_getInterface)* getInterfaceFn) {
    BackendExtension toret;
    toret.m_backendExtensionTable = std::make_shared<backend_extension_table_type>(
        backend_extension_table_type::create(apiTable, getInterfaceFn));
    return toret;
  }

  template <typename T_BackendExtensionType, typename... Args>
  std::enable_if_t<detail::is_api_type_v<T_BackendExtensionType>, T_BackendExtensionType> make(
      Args&&... args) {
    return T_BackendExtensionType(
        static_cast<std::weak_ptr<backend_extension_table_type>>(m_backendExtensionTable),
        std::forward<Args>(args)...);
  }

  template <typename T>
  std::enable_if_t<detail::is_tabular_be_type_v<T>, T> make() {
    return T(static_cast<std::weak_ptr<backend_extension_table_type>>(m_backendExtensionTable));
  }
};

namespace detail {

template <typename T_HandleType>
struct non_owning_handle {
  T_HandleType m_handle;
};
template <typename T_HandleType>
non_owning_handle(T_HandleType) -> non_owning_handle<T_HandleType>;

template <typename... Ts>
inline constexpr std::true_type is_backend_extension_(const BackendExtension<Ts...>&);
inline constexpr std::false_type is_backend_extension_(...);

template <typename T>
struct is_backend_extension : decltype(is_backend_extension_(std::declval<T>())) {};

template <typename T>
inline constexpr bool is_backend_extension_v = is_backend_extension<T>::value;
}  // namespace detail

class Api {
  using T_Specialization = void;
  template <typename, typename, typename>
  friend class ApiType;

  template <typename T>
  static auto test_prepareToCross(int)
      -> decltype(std::declval<const T&>().prepareToCross(), std::true_type{});
  template <typename T>
  static std::false_type test_prepareToCross(...);

  template <typename T>
  static auto test_updateAfterCross(int)
      -> decltype(std::declval<const T&>().updateAfterCross(), std::true_type{});
  template <typename T>
  static std::false_type test_updateAfterCross(...);

  template <typename T>
  struct has_prepareToCross : decltype(test_prepareToCross<T>(0)) {};

  template <typename T>
  struct has_updateAfterCross : decltype(test_updateAfterCross<T>(0)) {};

  static_assert(Api::has_prepareToCross<detail::DummyCrossable>::value);
  static_assert(Api::has_updateAfterCross<detail::DummyCrossable>::value);

  static_assert(!Api::has_prepareToCross<detail::DummyNotCrossable>::value);
  static_assert(!Api::has_updateAfterCross<detail::DummyNotCrossable>::value);

  static_assert(has_updateAfterCross<detail::DummyCrossable>::value);
  static_assert(has_prepareToCross<detail::DummyCrossable>::value);
  static_assert(!has_updateAfterCross<detail::DummyNotCrossable>::value);
  static_assert(!has_prepareToCross<detail::DummyNotCrossable>::value);

  template <typename T>
  inline static void callPrepareToCross(T& t) {
    t.prepareToCross();
  }
  template <typename T>
  inline static void callUpdateAfterCross(T& t) {
    t.updateAfterCross();
  }

  using api_table_type = ApiTable;

 public:
  // The QairtInterface_getInterface function pointer for this backend.
  // Required by system APIs (e.g. composeGraphs) that need to call backend
  // interfaces internally.
  decltype(QairtInterface_getInterface)* m_getInterfaceFn{};

  std::shared_ptr<api_table_type> m_apiTable;

 public:
  /**
   * @brief Tag type for static/RTLD_DEFAULT backend construction.
   *
   * Searches for QairtInterface_getInterface in the already-loaded symbols
   * (RTLD_DEFAULT).  Use this inside shared libraries (e.g. libQairtSystem.so)
   * where the backend library has already been loaded by the caller with
   * RTLD_GLOBAL, making QairtInterface_getInterface globally visible without
   * requiring a direct link-time dependency on the backend interface provider.
   *
   * If QAIRT_STATIC_LINK is defined and the RTLD_DEFAULT lookup fails, falls
   * back to the directly-linked QairtInterface_getInterface symbol.
   */
  struct StaticBackendTag {};
  static constexpr StaticBackendTag StaticBackend{};

  explicit Api(StaticBackendTag) {
    auto* getInterfaceFn = reinterpret_cast<decltype(QairtInterface_getInterface)*>(
        dynamicloading::dlsym(dynamicloading::dl_default, "QairtInterface_getInterface"));

#ifdef QAIRT_STATIC_LINK
    if (!getInterfaceFn) {
      getInterfaceFn = &QairtInterface_getInterface;
    }
#endif

    if (!getInterfaceFn) {
      throw std::runtime_error(
          "Failed to find QairtInterface_getInterface via RTLD_DEFAULT. "
          "Ensure the backend library is loaded with RTLD_GLOBAL before constructing Api, "
          "or define QAIRT_STATIC_LINK.");
    }

    m_getInterfaceFn = getInterfaceFn;
    m_apiTable       = std::make_shared<ApiTable>(ApiTable::create(nullptr, m_getInterfaceFn));
  }

  // Default flags: RTLD_NOW | RTLD_GLOBAL.
  // RTLD_GLOBAL is required so that symbols exported by the backend library
  // (e.g. QairtInterface_getInterface) are visible to subsequently dlopen'd
  // libraries such as libQairtSystem.so.
  explicit Api(const char* libSoFilePath,
               unsigned int openFlags = dynamicloading::dl_now | dynamicloading::dl_global) {
    void* handle = dynamicloading::dlopen(libSoFilePath, openFlags);
    if (!handle) {
      std::string errMsg = "Failed to dlopen " + std::string(libSoFilePath);
#ifndef _WIN32
      const char* dlErr = dlerror();
      if (dlErr) {
        errMsg += ": ";
        errMsg += dlErr;
      }
#endif
      throw std::runtime_error(errMsg);
    }

    void* getInterfaceFn = dynamicloading::dlsym(handle, "QairtInterface_getInterface");
    if (!getInterfaceFn) {
      throw std::runtime_error("failed to dlsym QairtInterface_getInterface");
    }

    m_getInterfaceFn = reinterpret_cast<decltype(QairtInterface_getInterface)*>(getInterfaceFn);
    m_apiTable = std::make_shared<api_table_type>(api_table_type::create(handle, m_getInterfaceFn));
  }
  explicit Api(decltype(QairtInterface_getInterface) getInterfaceFn)
      : m_getInterfaceFn(getInterfaceFn),
        m_apiTable(
            std::make_shared<api_table_type>(api_table_type::create(nullptr, getInterfaceFn))) {}
  explicit Api(std::shared_ptr<api_table_type> apiTable) : m_apiTable(std::move(apiTable)) {}

  Api(const Api&)                = delete;
  Api(Api&&) noexcept            = default;
  Api& operator=(const Api&)     = delete;
  Api& operator=(Api&&) noexcept = default;

  template <typename T_Extension>
  typename std::enable_if_t<detail::is_backend_extension_v<T_Extension>, T_Extension> make() {
    return T_Extension::create(m_apiTable, m_getInterfaceFn);
  };

  template <typename QairtClass, typename... Args>
  typename std::enable_if_t<detail::is_api_type_v<QairtClass>, QairtClass> make(
      Args&&... args) const {
    return QairtClass(m_apiTable, std::forward<Args>(args)...);
  }
  template <typename QairtClass, typename... Args>
  std::enable_if_t<std::is_pointer_v<QairtClass> &&
                       detail::is_api_type_v<std::remove_pointer_t<QairtClass>>,
                   QairtClass>
  make(Args&&... args) const {
    return new std::remove_pointer_t<QairtClass>(m_apiTable, std::forward<Args>(args)...);
  }
  template <typename QairtClass, typename... Args>
  std::enable_if_t<detail::is_api_type_v<QairtClass>, std::unique_ptr<QairtClass>> makeUnique(
      Args&&... args) const {
    return std::unique_ptr<QairtClass>(new QairtClass(m_apiTable, std::forward<Args>(args)...));
  }
  template <typename QairtClass, typename... Args>
  std::enable_if_t<detail::is_api_type_v<QairtClass>, std::shared_ptr<QairtClass>> makeShared(
      Args&&... args) const {
    return std::shared_ptr<QairtClass>(new QairtClass(m_apiTable, std::forward<Args>(args)...));
  }

  template <auto P2m_InterfaceFn, typename... Args>
  auto call(Args&&... args) {
    return m_apiTable->template call<P2m_InterfaceFn>(std::forward<Args>(args)...);
  }
};

namespace detail {

// Allow friend struct crossable
template <typename T,
          auto P2m_Getter,
          auto P2m_Setter = nullptr,
          auto P2m_GetNum = nullptr,
          typename        = void>
struct crossable;

struct QairtTypeAdmin;

template <typename T_ApiType>
struct ApiTypeRef;

}  // namespace detail

template <typename T_Derived, typename T_Interface, typename T_Table>
class ApiType {
  template <typename>
  friend struct detail::ApiTypeRef;
  QAIRT_STATIC_ASSERT(detail::has_size_member_v<T_Interface>);

 protected:
  using handle_type = table_handle_type_t<T_Table, T_Interface>;

  using api_table_type = T_Table;

 private:
  const api_table_type* m_rawTable{};
  std::weak_ptr<api_table_type> m_weakTable;
  handle_type m_handle{};
  bool m_nonOwning{false};

  template <typename, auto, auto, auto, typename>
  friend struct detail::crossable;

  friend struct detail::QairtTypeAdmin;

  template <typename, typename, typename>
  friend class ApiType;

 protected:
  using base_type    = ApiType<T_Derived, T_Interface, T_Table>;
  using derived_type = T_Derived;

  ApiType copyNonOnwing() const { return ApiType{m_rawTable, m_weakTable, m_handle}; }

  template <typename T>
  std::enable_if_t<detail::is_api_type_v<T>, T> createNonOwning(void* otherHandle) const {
    (void)otherHandle;
    return T{m_rawTable, nullptr, m_handle};
  }

  template <typename T, typename... T_Args>
  std::enable_if_t<detail::is_api_type_v<T>, T> create(T_Args&&... args) const {
    if (auto locked = m_weakTable.lock()) {
      return T(locked, std::forward<T_Args>(args)...);
    } else {
      throw Exception("Constructing after API is destroyed");
    }
  }

 protected:
  //  using base_type = ApiType<T_Interface>;
  using interface_type = T_Interface;
  template <typename T, typename U, typename V>
  static typename T_Table::template interface_handle_type<U>::handle_type getHandle(
      const ApiType<T, U, V>& other) noexcept {
    return other.m_handle;
  }

  template <typename, typename, typename>
  friend class ApiType;

  template <typename T, typename T_Handle>
  std::enable_if_t<detail::is_api_type_v<T>, T> constructNonOwning(T_Handle childHandle) const {
    using child_handle_type =
        typename T_Table::template interface_handle_type<typename T::interface_type>::handle_type;
    return T(
        *this,
        detail::non_owning_handle<child_handle_type>{static_cast<child_handle_type>(childHandle)});
  }
  // Non-owning constructor: accept any parent table type.
  // Only copy the table if parent and child share the same table type.
  template <typename T, typename U, typename V>
  ApiType(const ApiType<T, U, V>& parent, detail::non_owning_handle<handle_type> noh)
      : m_handle{noh.m_handle} {
    if constexpr (std::is_same_v<V, T_Table>) {
      m_rawTable  = parent.m_rawTable;
      m_weakTable = parent.m_weakTable;
    }
  }
  template <typename T, typename U, typename V>
  void bindNonOwning(const ApiType<T, U, V>& parent, detail::non_owning_handle<handle_type> noh) {
    if (!m_rawTable) {
      if constexpr (std::is_same_v<V, T_Table>) {
        m_rawTable  = parent.m_rawTable;
        m_weakTable = parent.m_weakTable;
      }
    }
    m_handle = noh.m_handle;
  }

  // Check if a type has a customFree() member function, useful for handling cases
  // where the free function does not simply take a handle to the corresponding type
  template <typename, typename = void>
  struct has_customFree : std::false_type {};
  template <typename T>
  struct has_customFree<T, std::void_t<decltype(&T::customFree)>> : std::true_type {};

 public:
  ~ApiType() {
    if constexpr (detail::has_free_member_v<T_Interface>) {
      if (m_handle && !m_nonOwning) {
        if constexpr (has_customFree<T_Derived>::value) {
          static_cast<T_Derived&>(*this).customFree(m_handle);
        } else {
          QAIRT_NO_RETURN callNoHandle<&T_Interface::free>(m_handle);
        }
        m_handle = {};
      }
    }
  }

  explicit ApiType(const std::shared_ptr<T_Table>& apiTable)
      : m_rawTable(apiTable.get()), m_weakTable(apiTable) {}

 protected:
  struct copy_table_tag_t {};
  inline static constexpr copy_table_tag_t copy_table_tag{};
  ApiType(copy_table_tag_t, const ApiType& other)
      : m_rawTable(other.m_rawTable), m_weakTable(other.m_weakTable) {}

 public:
  ApiType() noexcept                 = default;
  ApiType(const ApiType&)            = delete;
  ApiType& operator=(const ApiType&) = delete;

  ApiType(ApiType&&) noexcept            = default;
  ApiType& operator=(ApiType&&) noexcept = default;
  handle_type getHandle() const noexcept { return m_handle; }

 protected:
  void setHandle(handle_type handle) noexcept { m_handle = handle; }

  template <auto P2m_InterfaceFn, typename... Args>
  auto callNoHandle(Args&&... args) const noexcept(detail::p2mv_is_free_member_v<P2m_InterfaceFn>) {
    if constexpr (detail::p2mv_is_free_member_v<P2m_InterfaceFn>) {
      if (const auto lockedTable = m_weakTable.lock(); lockedTable) {
        return lockedTable->template call<P2m_InterfaceFn>(std::forward<Args>(args)...);
      } else {
        return decltype(lockedTable->template call<P2m_InterfaceFn>(std::forward<Args>(args)...)){};
      }
    } else {
      if (!m_rawTable) {
        std::string errorMsg = "Attempting to use ApiType function without a ApiTable. ";
        errorMsg += "Interface: ";
        errorMsg += detail::type_string_v<T_Interface>;
        errorMsg += ", Function: ";
        errorMsg += detail::type_string_v<decltype(P2m_InterfaceFn)>;
        errorMsg += ", Argument count: ";
        errorMsg += std::to_string(sizeof...(Args));
        throw std::runtime_error(errorMsg);
      }
      return m_rawTable->template call<P2m_InterfaceFn>(std::forward<Args>(args)...);
    }
  }

  template <auto P2m_InterfaceFn, typename... Args>
  auto call(Args&&... args) const {
    static_assert(!detail::p2mv_is_free_member_v<P2m_InterfaceFn>);
    return callNoHandle<P2m_InterfaceFn>(m_handle, std::forward<Args>(args)...);
  }

  template <auto P2m_InterfaceFn, typename... Args>
  void callAndConfirm(Args&&... args) const {
    static_assert(!detail::p2mv_is_free_member_v<P2m_InterfaceFn>);
    const auto status = call<P2m_InterfaceFn>(std::forward<Args>(args)...);
    if (QAIRT_SUCCESS != status) {
      throw Exception(std::string("CAPI call failed calling ") +
                          std::string(detail::type_string_v<decltype(P2m_InterfaceFn)>),
                      static_cast<CommonError>(status));
    }
  }

  template <auto P2m_InterfaceFn, typename... Args>
  void callNoHandleAndConfirm(Args&&... args) const {
    static_assert(!detail::p2mv_is_free_member_v<P2m_InterfaceFn>);
    const auto status = callNoHandle<P2m_InterfaceFn>(std::forward<Args>(args)...);
    if (QAIRT_SUCCESS != status) {
      throw Exception("CAPI call failed", static_cast<CommonError>(status));
    }
  }

  template <auto P2m_InterfaceFn, typename... Args>
  detail::p2mv_getter_type_t<P2m_InterfaceFn> callGetter(Args&&... args) const {
    detail::p2mv_getter_type_t<P2m_InterfaceFn> toret;
    callAndConfirm<P2m_InterfaceFn>(std::forward<Args>(args)..., &toret);
    return toret;
  }

  template <auto P2m_InterfaceFn, typename... Args>
  void callSetter(Args&&... args) const {
    callAndConfirm<P2m_InterfaceFn>(std::forward<Args>(args)...);
  }

  void setNonOwning(bool nonOwning) noexcept { m_nonOwning = nonOwning; }

  template <typename... T_Args>
  void construct(T_Args&&... args) {
    if (m_handle) {
      throw Exception("construct called with existing handle");
    }
    auto status = callNoHandle<&T_Interface::create>(std::forward<T_Args>(args)..., &m_handle);
    if (status != QAIRT_SUCCESS) {
      throw Exception("Failed to construct", static_cast<CommonError>(status));
    }
  }
  template <auto P2m_CreateFn, typename... T_Args>
  void construct(T_Args&&... args) {
    auto status = callNoHandle<P2m_CreateFn>(std::forward<T_Args>(args)..., &m_handle);
    if (status != QAIRT_SUCCESS) {
      throw Exception("Failed to construct", static_cast<CommonError>(status));
    }
  }

  void prepareToCross_() const {
    if constexpr (Api::template has_prepareToCross<derived_type>::value) {
      Api::callPrepareToCross(static_cast<const derived_type&>(*this));
    }
  }
  void updateAfterCross_() const {
    if constexpr (Api::has_updateAfterCross<derived_type>::value) {
      Api::callUpdateAfterCross(static_cast<const derived_type&>(*this));
    }
  }

  template <typename... T_Crossables>
  void prepareMultipleToCross(T_Crossables&... crossables) const {
    (crossables.prepareToCross(static_cast<const derived_type&>(*this)), ...);
  }
  template <typename... T_Crossables>
  void updateMultipleAfterCross(T_Crossables&... crossables) const {
    (crossables.updateAfterCross(static_cast<const derived_type&>(*this)), ...);
  }
};

namespace detail {

template <typename T, auto P2m_Getter, auto P2m_Setter, auto P2m_GetNum, typename>
struct crossable;

template <auto P2m_Getter, auto P2m_Setter>
struct crossable<std::string, P2m_Getter, P2m_Setter> {
  using getset_type = p2mv_getset_type_t<P2m_Getter, P2m_Setter>;

  mutable std::unique_ptr<std::string> m_str{};

  ~crossable() = default;

  crossable() noexcept                       = default;
  crossable(const crossable&)                = delete;
  crossable(crossable&&) noexcept            = default;
  crossable& operator=(const crossable&)     = delete;
  crossable& operator=(crossable&&) noexcept = default;

  explicit crossable(const std::string& str) : m_str(new std::string(str)) {}
  explicit crossable(std::string&& str) : m_str(new std::string(std::move(str))) {}

  template <typename T>
  void assignString(T&& t) const {
    if (m_str) {
      *m_str = std::forward<T>(t);
    } else {
      m_str = std::make_unique<std::string>(std::forward<T>(t));
    }
  }
  crossable& operator=(const std::string& str) {
    assignString(str);
    return *this;
  }
  crossable& operator=(std::string&& str) {
    assignString(std::move(str));
    return *this;
  }

  const char* c_str() const {
    if (m_str) {
      if (m_str->empty()) {
        return "";
      }
      return m_str->c_str();
    } else {
      return {};
    }
  }

  std::string& get() {
    if (!m_str) {
      m_str = std::make_unique<std::string>();
    }
    return *m_str;
  }
  const std::string& get() const {
    if (!m_str) {
      m_str = std::make_unique<std::string>();
    }
    return *m_str;
  }

  void clear() {
    if (m_str) {
      m_str->clear();
    }
  }

  template <typename T, typename V>
  void prepareToCross(const ApiType<T, p2mv_parent_type_t<P2m_Getter>, V>& parent) const {
    if constexpr (P2m_Setter != nullptr) {
      parent.template callSetter<P2m_Setter>(c_str());
    }
  }
  template <typename T, typename V>
  void updateAfterCross(const ApiType<T, p2mv_parent_type_t<P2m_Getter>, V>& parent) const {
    const char* current = parent.template callGetter<P2m_Getter>();
    if (c_str() != current) {
      if (current == nullptr) {
        m_str.reset();
      } else {
        assignString(current);
      }
    }
  }
};

static_assert(std::is_same_v<QairtTensor_PropertiesHandle_t,
                             p2mv_getter_type_t<&QairtTensor_V1_t::getTensorProperties>>);

template <typename T>
struct non_owning {
  QAIRT_STATIC_ASSERT(is_api_type_v<T>);
  using type = T;
};

template <typename T>
struct set_only {
  QAIRT_STATIC_ASSERT(is_api_type_v<T>);
  using type = T;
};

template <typename T_ApiType, auto P2m_Setter>
struct crossable<set_only<T_ApiType>,
                 nullptr,
                 P2m_Setter,
                 nullptr,
                 std::enable_if_t<is_api_type_v<T_ApiType>>> {
  mutable T_ApiType m_apiType;

  ~crossable() { m_apiType.m_handle = {}; }

  crossable() noexcept                       = default;
  crossable(const crossable&)                = delete;
  crossable(crossable&&) noexcept            = default;
  crossable& operator=(const crossable&)     = delete;
  crossable& operator=(crossable&&) noexcept = default;

  template <typename T, typename U, typename V>
  T_ApiType& setNonOwning(const ApiType<T, U, V>& parent,
                          p2mv_setter_type_t<P2m_Setter> childHandle) {
    parent.template callSetter<P2m_Setter>(childHandle);
    return m_apiType;
  }

  template <typename T, typename V>
  void prepareToCross(const ApiType<T, p2mv_parent_type_t<P2m_Setter>, V>& parent) const {
    (void)parent;
    m_apiType.prepareToCross();
  }
  template <typename T, typename V>
  void updateAfterCross(const ApiType<T, p2mv_parent_type_t<P2m_Setter>, V>& parent) const {
    (void)parent;
    m_apiType.updateAfterCross();
  }
};

template <typename T_ApiType, auto P2m_Getter, auto P2m_Setter>
struct crossable<non_owning<T_ApiType>,
                 P2m_Getter,
                 P2m_Setter,
                 nullptr,
                 std::enable_if_t<is_api_type_v<T_ApiType>>> {
  mutable T_ApiType m_apiType;

  ~crossable() { m_apiType.m_handle = {}; }

  crossable() noexcept                       = default;
  crossable(const crossable&)                = delete;
  crossable(crossable&&) noexcept            = default;
  crossable& operator=(const crossable&)     = delete;
  crossable& operator=(crossable&&) noexcept = default;

  template <typename T, typename U, typename V>
  T_ApiType& bindNonOwning(const ApiType<T, U, V>& parent) const {
    using child_handle_type = typename T_ApiType::handle_type;
    m_apiType.bindNonOwning(
        parent, non_owning_handle<child_handle_type>{parent.template callGetter<P2m_Getter>()});
    m_apiType.updateAfterCross_();
    return m_apiType;
  }

  template <typename T, typename U, typename V, typename W, typename X, typename Y>
  T_ApiType& setNonOwning(const ApiType<T, U, V>& parent, const ApiType<W, X, Y>& child) {
    child.prepareToCross_();
    parent.template callSetter<P2m_Setter>(ApiType<W, X, Y>::getHandle(child));
    return m_apiType;
  }

  template <typename T, typename V>
  void prepareToCross(const ApiType<T, p2mv_parent_type_t<P2m_Getter>, V>& parent) const {
    if (!m_apiType.m_rawTable) {
      QAIRT_NO_RETURN bindNonOwning(parent);
    }
    m_apiType.prepareToCross_();
  }
  template <typename T, typename V>
  void updateAfterCross(const ApiType<T, p2mv_parent_type_t<P2m_Getter>, V>& parent) const {
    if (!m_apiType.m_rawTable) {
      QAIRT_NO_RETURN bindNonOwning(parent);
    }
    m_apiType.updateAfterCross_();
  }
};

namespace detail {
template <auto P2m_Setter>
struct is_vector_setter {
  static constexpr bool value =
      std::is_same_v<std::tuple_element_t<1, p2mv_setter_type_t<P2m_Setter>>, uint32_t> &&
      std::is_pointer_v<std::tuple_element_t<0, p2mv_setter_type_t<P2m_Setter>>>;
};
template <>
struct is_vector_setter<nullptr> {
  static constexpr bool value = true;
};

template <auto P2m_Setter>
inline constexpr bool is_vector_setter_v = is_vector_setter<P2m_Setter>::value;
}  // namespace detail

template <auto P2m_Getter, auto P2m_Setter>
struct crossable<std::vector<std::string>, P2m_Getter, P2m_Setter> {
  mutable std::vector<std::string> m_vec;

  ~crossable() {}

  crossable() noexcept                       = default;
  crossable(const crossable&)                = delete;
  crossable(crossable&&) noexcept            = default;
  crossable& operator=(const crossable&)     = delete;
  crossable& operator=(crossable&&) noexcept = default;
};

template <typename T_ApiType, auto P2m_Getter, auto P2m_GetNum, auto P2m_Setter>
struct crossable<std::vector<non_owning<T_ApiType>>,
                 P2m_Getter,
                 P2m_GetNum,
                 P2m_Setter,
                 std::enable_if_t<is_api_type_v<T_ApiType>>> {
  QAIRT_STATIC_ASSERT(
      std::is_same_v<uint32_t, std::tuple_element_t<0, p2mv_getter_type_t<P2m_Getter>>>);
  QAIRT_STATIC_ASSERT(is_tuple_v<p2mv_getter_type_t<P2m_Getter>>);
  QAIRT_STATIC_ASSERT(std::is_pointer_v<std::tuple_element_t<1, p2mv_getter_type_t<P2m_Getter>>>);
  static_assert(!std::is_pointer_v<
                std::remove_pointer_t<std::tuple_element_t<1, p2mv_getter_type_t<P2m_Getter>>>>);

  QAIRT_STATIC_ASSERT(detail::is_vector_setter_v<P2m_Setter>);

  // Helper to check setter type only when not nullptr
  template <auto Setter, typename = void>
  struct check_setter_constraints {
    static constexpr bool value = true;
  };

  template <auto Setter>
  struct check_setter_constraints<Setter, std::enable_if_t<Setter != nullptr>> {
    // Ensure setter takes in handle* and uint32_t size
    QAIRT_STATIC_ASSERT(
        std::is_same_v<std::tuple_element_t<1, p2mv_setter_type_t<Setter>>, uint32_t>);
    QAIRT_STATIC_ASSERT(std::is_pointer_v<std::tuple_element_t<0, p2mv_setter_type_t<Setter>>>);
    QAIRT_STATIC_ASSERT(
        std::is_same_v<std::remove_pointer_t<std::tuple_element_t<0, p2mv_setter_type_t<Setter>>>,
                       std::tuple_element_t<1, p2mv_getter_type_t<P2m_Getter>>>);
    static constexpr bool value = true;
  };

  QAIRT_STATIC_ASSERT(check_setter_constraints<P2m_Setter>::value);

  mutable std::vector<T_ApiType> m_vec;

  ~crossable() {
    // remove the handle to prevent free calls on DTOR
    for (auto& e : m_vec) {
      e.m_handle = {};
    }
  }

  crossable() noexcept                       = default;
  crossable(const crossable&)                = delete;
  crossable(crossable&&) noexcept            = default;
  crossable& operator=(const crossable&)     = delete;
  crossable& operator=(crossable&&) noexcept = default;

  // Retrieve non-owning objects
  template <typename T, typename U, typename V>
  std::vector<T_ApiType>& get(const ApiType<T, U, V>& parent) const {
    static_assert(is_tuple_v<p2mv_getter_type_t<P2m_Getter>>,
                  "Update this after fixing p2mv_getter_type_t");
    using handle_type = std::tuple_element_t<1, p2mv_getter_type_t<P2m_Getter>>;
    static_assert(!std::is_pointer_v<std::remove_pointer_t<handle_type>>);

    // Query the size
    const auto size = parent.template callGetter<P2m_GetNum>();
    m_vec.resize(size);
    for (std::remove_const_t<decltype(size)> i = 0; i < size; ++i) {
      auto& current             = m_vec[i];
      const auto existingHandle = parent.getHandle(current);
      handle_type currentHandle{};
      parent.template callAndConfirm<P2m_Getter>(i, &currentHandle);
      if (existingHandle != currentHandle) {
        if (!currentHandle) {
          throw std::runtime_error("Null handle returned in vector?");
        }
        current = parent.template constructNonOwning<T_ApiType>(currentHandle);
        current.updateAfterCross_();
      }
    }
    return m_vec;
  }

  template <typename T, typename U, typename V>
  void set(const ApiType<T, U, V>& parent, const std::vector<T_ApiType>& vec) const {
    static_assert(is_tuple_v<p2mv_setter_type_t<P2m_Setter>>);

    using size_type = std::tuple_element_t<1, p2mv_setter_type_t<P2m_Setter>>;
    static_assert(std::is_integral_v<size_type>);

    using handle_pointer_type = std::tuple_element_t<0, p2mv_setter_type_t<P2m_Setter>>;
    using handle_type         = std::remove_pointer_t<handle_pointer_type>;
    static_assert(!std::is_pointer_v<std::remove_pointer_t<handle_type>>);

    std::vector<handle_type> handles(vec.size());
    QAIRT_NO_RETURN std::transform(
        vec.begin(), vec.end(), handles.begin(), [&](const T_ApiType& e) {
          e.prepareToCross_();
          return parent.getHandle(e);
        });

    parent.template callSetter<P2m_Setter>(handles.data(), handles.size());

    m_vec.clear();
  }

  template <typename T, typename V>
  void prepareToCross(const ApiType<T, p2mv_parent_type_t<P2m_Getter>, V>& parent) const {
    (void)parent;
    for (auto& e : m_vec) {
      e.prepareToCross_();
    }
  }
  template <typename T, typename V>
  void updateAfterCross(const ApiType<T, p2mv_parent_type_t<P2m_Getter>, V>& parent) const {
    for (auto& e : get(parent)) {
      e.updateAfterCross_();
    }
  }
};

class bool_wrapper {
  bool m_value{};

 public:
  constexpr bool_wrapper() noexcept                               = default;
  constexpr bool_wrapper(const bool_wrapper&) noexcept            = default;
  constexpr bool_wrapper(bool_wrapper&&) noexcept                 = default;
  constexpr bool_wrapper& operator=(const bool_wrapper&) noexcept = default;
  constexpr bool_wrapper& operator=(bool_wrapper&&) noexcept      = default;

  explicit constexpr bool_wrapper(const bool& value) noexcept : m_value{value} {}
  constexpr bool_wrapper& operator=(const bool& value) noexcept {
    m_value = value;
    return *this;
  }

  explicit constexpr operator const bool&() const noexcept { return m_value; }
  explicit constexpr operator bool&() noexcept { return m_value; }
};
static_assert(sizeof(bool_wrapper) == sizeof(uint8_t));

}  // namespace detail

// Allow API boundary synchronization internally
namespace detail {
struct QairtTypeAdmin {
  template <typename T, typename U, typename V>
  static void prepareToCross(const ApiType<T, U, V>& apiType) {
    apiType.prepareToCross_();
  }
  template <typename T, typename U, typename V>
  static void updateAfterCross(const ApiType<T, U, V>& apiType) {
    apiType.updateAfterCross_();
  }
  template <typename T, typename U, typename V>
  static typename V::template interface_handle_type<U>::handle_type getHandle(
      const ApiType<T, U, V>& apiType) {
    return apiType.getHandle();
  }
  template <typename T, typename U, typename V>
  static std::shared_ptr<V> lockApiTable(const ApiType<T, U, V>& apiType) {
    return apiType.m_weakTable.lock();
  }
};
}  // namespace detail

namespace detail {

template <typename T>
using optional_reference = std::optional<std::reference_wrapper<T>>;

}  // namespace detail

template <typename T>
using ApiTypeRef =
    std::enable_if_t<detail::is_api_type_v<std::remove_cv_t<std::remove_reference_t<T>>>,
                     detail::optional_reference<std::remove_reference_t<T>>>;

namespace detail {

template <typename T>
inline static auto getHandle(const optional_reference<T>& optRef) {
  if (optRef.has_value()) {
    return QairtTypeAdmin::getHandle(optRef->get());
  } else {
    return decltype(QairtTypeAdmin::getHandle(optRef->get())){};
  }
}

}  // namespace detail

class CustomConfigType {
 protected:
  CustomConfigType()                                       = default;
  CustomConfigType(const CustomConfigType&)                = default;
  CustomConfigType(CustomConfigType&&) noexcept            = default;
  CustomConfigType& operator=(const CustomConfigType&)     = default;
  CustomConfigType& operator=(CustomConfigType&&) noexcept = default;

 public:
  virtual ~CustomConfigType() = default;
};

}  // namespace qairt

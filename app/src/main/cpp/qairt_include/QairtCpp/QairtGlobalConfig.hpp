//==============================================================================
//
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// All rights reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//==============================================================================

/**
 * @file
 * @brief  C++ wrapper for the QAIRT global config API.
 *
 *         The Global Config object configures process-global parameters that
 *         simulation backends consume during their initialization. It must be
 *         set before constructing any other QAIRT object that reads SoC
 *         information (e.g. Backend, Device).
 *
 */

#pragma once

#include "QairtApi.hpp"
#include "QairtGlobalConfig/QairtGlobalConfig.h"

namespace qairt {

/**
 * @brief Result and error codes returned by QAIRT global config operations.
 *
 * @details
 * | Enumerator             | Description                                                     |
 * |------------------------|-----------------------------------------------------------------|
 * | `NoError`              | Operation completed without error.                              |
 * | `InvalidConfig`        | Provided value is invalid, or platform does not support it.     |
 * | `OperationNotPermitted`| Config has already been set or read by another module.          |
 * | `NotSupported`         | The loaded backend does not expose the global config API.       |
 * | `Undefined`            | An undefined or unknown error occurred.                         |
 */
QAIRT_CPPAPI_DEFINE_ENUM(GlobalConfigError,
                         QairtGlobalConfig_Error_t,
                         NoError               = QAIRT_GLOBAL_CONFIG_NO_ERROR,
                         InvalidConfig         = QAIRT_GLOBAL_CONFIG_ERROR_INVALID_CONFIG,
                         OperationNotPermitted = QAIRT_COMMON_ERROR_OPERATION_NOT_PERMITTED,
                         NotSupported          = QAIRT_COMMON_ERROR_NOT_SUPPORTED,
                         Undefined             = QAIRT_GLOBAL_CONFIG_ERROR_UNDEFINED)

/**
 * @brief Wrapper for the QAIRT global config interface.
 *
 *        Obtained via Api::make<GlobalConfig>(). Must be used before any
 *        backend-side initialization that reads SoC information; subsequent
 *        attempts to change the value are rejected.
 */
class GlobalConfig : public ApiType<GlobalConfig, QairtGlobalConfig_V1_t> {
  friend class Api;

 public:
  GlobalConfig()                               = default;
  GlobalConfig(const GlobalConfig&)            = delete;
  GlobalConfig(GlobalConfig&&) noexcept        = default;
  GlobalConfig& operator=(const GlobalConfig&) = delete;
  GlobalConfig& operator=(GlobalConfig&&) noexcept = default;

  /**
   * @brief Set the SoC model used by simulation backends.
   *
   * @param[in] socModel SoC model identifier (opaque numeric ID).
   *
   * @throws qairt::Exception if the value is rejected (e.g. invalid value,
   *         already set, or platform does not support setting it).
   *
   * @see QairtGlobalConfig_setSocModel
   */
  void setSocModel(uint32_t socModel) {
    callNoHandleAndConfirm<&interface_type::setSocModel>(socModel);
  }

 private:
  explicit GlobalConfig(const std::shared_ptr<ApiTable>& apiTable) : ApiType(apiTable) {}
};

}  // namespace qairt

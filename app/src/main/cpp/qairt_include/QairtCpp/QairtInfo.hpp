//==============================================================================
//
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// All rights reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//==============================================================================

/**
 * @file
 * @brief  C++ wrapper for the QAIRT info API.
 *
 *         A Backend must be created before constructing an Info object.
 *
 */

#pragma once

#include <cstdint>
#include <string>

#include "QairtApi.hpp"
#include "QairtCommon/QairtInfo.h"

namespace qairt {

/**
 * @brief Result and error codes returned by QAIRT capability query operations.
 *
 * @details
 * | Enumerator     | Description                                         |
 * |----------------|-----------------------------------------------------|
 * | `NoError`      | Operation completed without error.                  |
 * | `Supported`    | The queried capability is supported.                |
 * | `NotSupported` | The queried capability is not supported.            |
 * | `UnknownKey`   | The provided capability key was not recognized.     |
 * | `Undefined`    | An undefined or unknown result occurred.            |
 */
QAIRT_CPPAPI_DEFINE_ENUM(CapabilityError,
                         QairtCapability_Error_t,
                         NoError      = QAIRT_CAPABILITY_NO_ERROR,
                         Supported    = QAIRT_CAPABILITY_SUPPORTED,
                         NotSupported = QAIRT_CAPABILITY_NOT_SUPPORTED,
                         UnknownKey   = QAIRT_CAPABILITY_ERROR_UNKNOWN_KEY,
                         Undefined    = QAIRT_CAPABILITY_ERROR_UNDEFINED)

using CapabilityKey = QairtCapability_Key_t;

/**
 * @brief Semantic version number returned by QAIRT info queries.
 *
 *        Construct directly: Version v{major, minor, patch}.
 */
struct Version {
  /// @brief Major component of a semantic version number.
  uint32_t major;
  /// @brief Minor component of a semantic version number.
  uint32_t minor;
  /// @brief Patch component of a semantic version number.
  uint32_t patch;
};

/**
 * @brief Wrapper for the QAIRT info interface, providing version and capability queries.
 *
 *        Obtained via Api::make<Info>().
 */
class Info : public ApiType<Info, QairtInfo_V1_t> {
  friend class Api;

 public:
  Info()                           = default;
  Info(const Info&)                = delete;
  Info(Info&&) noexcept            = default;
  Info& operator=(const Info&)     = delete;
  Info& operator=(Info&&) noexcept = default;

  /**
   * @brief Get the backend API version.
   *
   * @return Version struct containing the backend API major, minor, and patch numbers.
   *
   * @throws qairt::Exception on invalid argument.
   *
   * @see QairtInfo_getBackendApiVersion
   */
  Version getBackendApiVersion() const {
    Version toret{};
    QAIRT_NO_RETURN callNoHandle<&interface_type::getBackendApiVersion>(
        &toret.major, &toret.minor, &toret.patch);
    return toret;
  }

  /**
   * @brief Get the core API version.
   *
   * @return Version struct containing the core API major, minor, and patch numbers.
   *
   * @throws qairt::Exception on invalid argument.
   *
   * @see QairtInfo_getCoreApiVersion
   */
  Version getCoreApiVersion() const {
    Version toret{};
    QAIRT_NO_RETURN callNoHandle<&interface_type::getCoreApiVersion>(
        &toret.major, &toret.minor, &toret.patch);
    return toret;
  }

  /**
   * @brief Get the build identifier string for the backend library.
   *
   * @return Build ID string, or an empty string if none is available.
   *
   * @see QairtInfo_getBuildId
   */
  std::string getBuildId() const {
    const char* str{};
    QAIRT_NO_RETURN callNoHandle<&interface_type::getBuildId>(&str);
    return std::string(str != nullptr ? str : "");
  }

  /**
   * @brief Query whether the backend supports a specific capability.
   *
   * @param[in] key  Capability identifier to query.
   *
   * @return `true` if the capability is supported, `false` if not supported.
   *
   * @throws std::invalid_argument if _key_ is not recognized by the backend.
   * @throws std::runtime_error on unexpected error.
   *
   * @see QairtInfo_hasCapability
   */
  bool hasCapability(CapabilityKey key) const {
    Qairt_Status_t result = callNoHandle<&interface_type::hasCapability>(key);
    if (result == QAIRT_CAPABILITY_SUPPORTED) {
      return true;
    } else if (result == QAIRT_CAPABILITY_NOT_SUPPORTED) {
      return false;
    } else if (result == QAIRT_CAPABILITY_ERROR_UNKNOWN_KEY) {
      throw std::invalid_argument("Invalid qairt::CapabilityKey");
    }
    throw std::runtime_error("Unknown error in hasCapability");
  }

 private:
  explicit Info(const std::shared_ptr<ApiTable>& apiTable) : ApiType(apiTable) {}
};

}  // namespace qairt

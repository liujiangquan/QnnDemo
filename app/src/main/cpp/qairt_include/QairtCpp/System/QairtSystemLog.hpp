//==============================================================================
//
//  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//  All rights reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//==============================================================================

/**
 * @file
 * @brief  C++ wrapper for the QAIRT System logging API.
 *
 *         A SystemApi must be created before constructing a SystemLog object.
 *
 */

#pragma once

#include "../QairtLog.hpp"
#include "QairtSystemApi.hpp"

namespace qairt {

/**
 * @brief Handle for a QAIRT system-library log sink.
 *
 *        Obtained via `SystemApi::makeShared<SystemLog>(callback, level)`.
 */
class SystemLog : public ApiType<SystemLog, QairtSystemLog_V1_t, SystemApiTable> {
  friend SystemApi;

  /// @brief Callback function pointer for this log instance.
  LogCallback m_callback;

 public:
  SystemLog() noexcept                       = default;
  SystemLog(const SystemLog&)                = delete;
  SystemLog(SystemLog&&) noexcept            = default;
  SystemLog& operator=(const SystemLog&)     = delete;
  SystemLog& operator=(SystemLog&&) noexcept = default;

  /**
   * @brief Set the verbosity level for this system log instance.
   *
   * @param[in] level  Maximum log verbosity.
   *
   * @throws qairt::Exception on invalid handle or invalid log level.
   *
   * @see QairtSystemLog_setLogLevel
   */
  void setLogLevel(LogLevel level) {
    callAndConfirm<&QairtSystemLog_V1_t::setLogLevel>(static_cast<QairtLog_Level_t>(level));
  }

 private:
  SystemLog(const std::shared_ptr<SystemApiTable>& apiTable,
            LogCallback&& callback,
            LogLevel level,
            void* userData = nullptr)
      : ApiType(apiTable) {
    construct<&interface_type::create>(detail::QairtTypeAdmin::getHandle(callback),
                                       static_cast<QairtLog_Level_t>(level),
                                       userData);
  }

  SystemLog(const std::shared_ptr<SystemApiTable>& apiTable,
            ApiTypeRef<LogCallback>&& callback,
            LogLevel level,
            void* userData = nullptr)
      : ApiType(apiTable) {
    construct<&interface_type::create>(
        detail::getHandle(callback), static_cast<QairtLog_Level_t>(level), userData);
  }
};

}  // namespace qairt

//==============================================================================
//
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// All rights reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//==============================================================================

/**
 * @file
 * @brief C++ wrapper for the QAIRT logging API.
 *
 * A Backend must be created before constructing a Log object.
 */

#pragma once

#include "QairtApi.hpp"
#include "QairtLog/QairtLog.h"

namespace qairt {

/**
 * @brief Error codes returned by QAIRT logging operations.
 *
 * @details
 * | Enumerator        | Description                                    |
 * |-------------------|------------------------------------------------|
 * | `NoError`         | Operation succeeded.                           |
 * | `MemAlloc`        | Memory allocation failure.                     |
 * | `Initialization`  | Log initialization failed.                     |
 * | `InvalidArgument` | An argument to the operation was invalid.      |
 * | `InvalidHandle`   | The provided handle is not valid.              |
 * | `Undefined`       | An undefined or unknown error occurred.        |
 */
QAIRT_CPPAPI_DEFINE_ENUM(LogError,
                         QairtLog_Error_t,
                         NoError         = QAIRT_LOG_NO_ERROR,
                         MemAlloc        = QAIRT_LOG_ERROR_MEM_ALLOC,
                         Initialization  = QAIRT_LOG_ERROR_INITIALIZATION,
                         InvalidArgument = QAIRT_LOG_ERROR_INVALID_ARGUMENT,
                         InvalidHandle   = QAIRT_LOG_ERROR_INVALID_HANDLE,
                         Undefined       = QAIRT_LOG_ERROR_UNDEFINED)

/**
 * @brief Verbosity levels for QAIRT log output.
 *
 * @details
 * | Enumerator | Description                                              |
 * |------------|----------------------------------------------------------|
 * | `Error`    | Error conditions only.                                   |
 * | `Warn`     | Warnings and error conditions.                           |
 * | `Info`     | Informational messages, warnings, and errors.            |
 * | `Verbose`  | Detailed trace messages and all lower-severity output.   |
 * | `Debug`    | Reserved for developer debugging; most verbose level.    |
 * | `Max`      | Sentinel value; enables all log output.                  |
 */
QAIRT_CPPAPI_DEFINE_ENUM(LogLevel,
                         QairtLog_Level_t,
                         Error   = QAIRT_LOG_LEVEL_ERROR,
                         Warn    = QAIRT_LOG_LEVEL_WARN,
                         Info    = QAIRT_LOG_LEVEL_INFO,
                         Verbose = QAIRT_LOG_LEVEL_VERBOSE,
                         Debug   = QAIRT_LOG_LEVEL_DEBUG,
                         Max     = QAIRT_LOG_LEVEL_MAX)

class Log;
class SystemLog;

/**
 * @brief Wrapper for a QAIRT log callback function handle.
 */
class LogCallback : public ApiType<LogCallback, QairtLog_CallbackV1_t> {
  friend class Api;
  friend class Log;
  friend class SystemLog;

 public:
  LogCallback() noexcept                         = default;
  LogCallback(const LogCallback&)                = delete;
  LogCallback(LogCallback&&) noexcept            = default;
  LogCallback& operator=(const LogCallback&)     = delete;
  LogCallback& operator=(LogCallback&&) noexcept = default;

 private:
  LogCallback(const std::shared_ptr<ApiTable>& apiTable, QairtLog_CallbackFn_t callbackFn)
      : ApiType(apiTable) {
    construct(callbackFn);
  }
};

/**
 * @brief Wrapper for a QAIRT Log handle.
 *
 *        Obtained via @ref Api::make<Log>().
 */
class Log : public ApiType<Log, QairtLog_V1_t> {
  friend class Api;

  /// @brief Callback function pointer for this log instance.
  LogCallback m_callback;

 public:
  Log() noexcept                 = default;
  Log(const Log&)                = delete;
  Log(Log&&) noexcept            = default;
  Log& operator=(const Log&)     = delete;
  Log& operator=(Log&&) noexcept = default;

  /**
   * @brief Set the verbosity level for this log instance.
   *
   * @param[in] level  Maximum log verbosity.
   *
   * @throws qairt::Exception on invalid handle.
   *
   * @see QairtLog_setLogLevel
   */
  void setLogLevel(QairtLog_Level_t level) { callAndConfirm<&QairtLog_V1_t::setLogLevel>(level); }

 private:
  Log(const std::shared_ptr<ApiTable>& apiTable,
      LogCallback&& callback,
      LogLevel level,
      void* userData = nullptr)
      : ApiType(apiTable), m_callback(std::move(callback)) {
    construct(getHandle(m_callback), static_cast<QairtLog_Level_t>(level), userData);
  }

  Log(const std::shared_ptr<ApiTable>& apiTable,
      QairtLog_CallbackFn_t callbackFn,
      QairtLog_Level_t level,
      void* userData = nullptr)
      : ApiType(apiTable), m_callback(apiTable, callbackFn) {
    construct(getHandle(m_callback), level, userData);
  }
};

}  // namespace qairt

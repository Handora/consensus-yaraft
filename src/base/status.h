// Copyright 2017 Wu Tao
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "base/slice.h"

#include <boost/optional.hpp>
#include <glog/logging.h>
#include <silly/status.h>

namespace consensus {

class Error {
 public:
  enum ErrorCodes {
    OK,

    IOError,
    NotFound,
    AlreadyPresent,
    NotSupported,
    Corruption,
    LogCompacted,
    OutOfBound,
    YARaftERR,
  };

 private:
  static std::string toString(unsigned int code);

 private:
  friend class silly::Status<Error, Error::ErrorCodes>;
};

typedef silly::Status<Error, Error::ErrorCodes> Status;

template <typename T>
class StatusWith {
 public:
  // for ok case
  StatusWith(T value) : status_(Status::OK()), value_(value) {}

  // for error case
  StatusWith(Status status) : status_(std::move(status)) {}

  StatusWith(Error::ErrorCodes code, const Slice &reason)
      : StatusWith(Status::Make(code, reason)) {}

  StatusWith(Error::ErrorCodes code) : StatusWith(Status::Make(code, nullptr)) {}

  const T &GetValue() const {
    LOG_ASSERT(status_.IsOK());
    return *value_;
  }

  T &GetValue() {
    LOG_ASSERT(status_.IsOK());
    return *value_;
  }

  const Status &GetStatus() const {
    return status_;
  }

  bool IsOK() const {
    return status_.IsOK();
  }

  std::string ToString() const {
    return status_.ToString();
  }

 private:
  Status status_;
  boost::optional<T> value_;
};

#define ASSIGN_IF_OK(sw, var)                                                                   \
  do {                                                                                          \
    const auto &_sw = (sw);                                                                     \
    auto &_var = (var);                                                                         \
    RETURN_NOT_OK(_sw.GetStatus());                                                             \
    static_assert(std::is_convertible<decltype(_var), decltype(_sw.GetValue())>::value == true, \
                  #var " cannot be converted to " #sw ".GetValue()");                           \
    _var = _sw.GetValue();                                                                      \
  } while (0)

}  // namespace consensus

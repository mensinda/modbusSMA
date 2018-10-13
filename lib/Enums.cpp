/*
 * Copyright (C) 2018 Daniel Mensinger
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Enums.hpp"

using namespace std;
using namespace modbusSMA;

//! Converts the State enum to a std::string.
string enum2Str::toStr(State _state) {
  switch (_state) {
    case State::CONFIGURE: return "CONFIGURE";
    case State::CONNECTED: return "CONNECTED";
    case State::INITIALIZED: return "INITIALIZED";
    case State::ERROR: return "ERROR";
    default: return "<UNKNOWN>";
  }
}

//! Converts the ErrorCode enum to a std::string.
string enum2Str::toStr(ErrorCode _errocCode) {
  switch (_errocCode) {
    case ErrorCode::OK: return "OK";
    case ErrorCode::INVALID_STATE: return "INVALID_STATE";
    case ErrorCode::INVALID_MODBUS_CONTEXT: return "INVALID_MODBUS_CONTEXT";
    case ErrorCode::MODBUS_CONNECTION_FAILED: return "MODBUS_CONNECTION_FAILED";
    case ErrorCode::INITIALIZATION_FAILED: return "INITIALIZATION_FAILED";
    case ErrorCode::FILE_NOT_FOUND: return "FILE_NOT_FOUND";
    case ErrorCode::DATA_BASE_ERROR: return "DATA_BASE_ERROR";
    case ErrorCode::ERROR: return "ERROR";
    default: return "<UNKNOWN>";
  }
}

//! Converts the ConnectionType enum to a std::string.
string enum2Str::toStr(ConnectionType _type) {
  switch (_type) {
    case ConnectionType::TCP_IP: return "TCP_IP";
    case ConnectionType::TCP_IP_PI: return "TCP_IP_PI";
    case ConnectionType::RTU: return "RTU";
    default: return "<UNKNOWN>";
  }
}

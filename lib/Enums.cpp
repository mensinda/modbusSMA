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

const size_t FNV1A_BASE  = 2166136261;
const size_t FNV1A_PRIME = 16777619;

inline size_t fnv1aHash(const char *data) {
  size_t hash = FNV1A_BASE;
  while (*data != 0) {
    hash ^= static_cast<size_t>(*(data++));
    hash *= FNV1A_PRIME;
  }
  return hash;
}

constexpr size_t fnv1aHash(const char *data, size_t n) {
  size_t hash = FNV1A_BASE;
  for (size_t i = 0; i < n; ++i) {
    hash ^= static_cast<size_t>(data[i]);
    hash *= FNV1A_PRIME;
  }
  return hash;
}

constexpr size_t operator"" _h(char const *data, size_t n) { return fnv1aHash(data, n); }


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

//! Converts the DataType enum to a std::string.
string enum2Str::toStr(DataType _type) {
  switch (_type) {
    case DataType::S16: return "S16";
    case DataType::S32: return "S32";
    case DataType::S64: return "S64";
    case DataType::STR32: return "STR32";
    case DataType::U16: return "U16";
    case DataType::U32: return "U32";
    case DataType::U64: return "U64";
    case DataType::__UNKNOWN__: return "__UNKNOWN__";
    default: return "<UNKNOWN>";
  }
}

//! Converts the DataAccess enum to a std::string.
string enum2Str::toStr(DataAccess _type) {
  switch (_type) {
    case DataAccess::RO: return "RO";
    case DataAccess::RW: return "RW";
    case DataAccess::WO: return "WO";
    case DataAccess::__UNKNOWN__: return "__UNKNOWN__";
    default: return "<UNKNOWN>";
  }
}


//! Converts the DataFormat enum to a std::string.
string enum2Str::toStr(DataFormat _type) {
  switch (_type) {
    case DataFormat::Duration: return "Duration";
    case DataFormat::DT: return "DT";
    case DataFormat::ENUM: return "ENUM";
    case DataFormat::FIX0: return "FIX0";
    case DataFormat::FIX1: return "FIX1";
    case DataFormat::FIX2: return "FIX2";
    case DataFormat::FIX3: return "FIX3";
    case DataFormat::FIX4: return "FIX4";
    case DataFormat::FW: return "FW";
    case DataFormat::HW: return "HW";
    case DataFormat::IP4: return "IP4";
    case DataFormat::RAW: return "RAW";
    case DataFormat::TM: return "TM";
    case DataFormat::UTF8: return "UTF8";
    case DataFormat::REV: return "REV";
    case DataFormat::TEMP: return "TEMP";
    case DataFormat::FUNCTION_SEC: return "FUNCTION_SEC";
    case DataFormat::__UNKNOWN__: return "__UNKNOWN__";
    default: return "<UNKNOWN>";
  }
}

//! Converts a string to the corresponding DataType
DataType enum2Str::typeFromStr(string _type) {
  switch (fnv1aHash(_type.c_str())) {
    case "S16"_h: return DataType::S16;
    case "S32"_h: return DataType::S32;
    case "S64"_h: return DataType::S64;
    case "STR32"_h: return DataType::STR32;
    case "U16"_h: return DataType::U16;
    case "U32"_h: return DataType::U32;
    case "U64"_h: return DataType::U64;
    default: return DataType::__UNKNOWN__;
  }
}

//! Converts a string to the corresponding DataFormat
DataFormat enum2Str::formatFromStr(string _type) {
  switch (fnv1aHash(_type.c_str())) {
    case "Duration"_h: return DataFormat::Duration;
    case "DT"_h: return DataFormat::DT;
    case "FUNKTION_SEC"_h:
    case "ENUM"_h: return DataFormat::ENUM;
    case "FIX0"_h: return DataFormat::FIX0;
    case "FIX1"_h: return DataFormat::FIX1;
    case "FIX2"_h: return DataFormat::FIX2;
    case "FIX3"_h: return DataFormat::FIX3;
    case "FIX4"_h: return DataFormat::FIX4;
    case "FW"_h: return DataFormat::FW;
    case "HW"_h: return DataFormat::HW;
    case "IP4"_h: return DataFormat::IP4;
    case "RAW"_h: return DataFormat::RAW;
    case "TM"_h: return DataFormat::TM;
    case "UTF8"_h: return DataFormat::UTF8;
    case "REV"_h: return DataFormat::REV;
    case "TEMP"_h: return DataFormat::TEMP;
    case "FUNCTION_SEC"_h: return DataFormat::FUNCTION_SEC;
    default: return DataFormat::__UNKNOWN__;
  }
}

//! Converts a string to the corresponding DataAccess
DataAccess enum2Str::accessFromStr(string _type) {
  switch (fnv1aHash(_type.c_str())) {
    case "RO"_h: return DataAccess::RO;
    case "RW"_h: return DataAccess::RW;
    case "W"_h:
    case "WO"_h: return DataAccess::WO;
    default: return DataAccess::__UNKNOWN__;
  }
}

//! \file
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

#pragma once

#include <string>

namespace modbusSMA {

//! The current state of the ModbusAPI.
enum class State {
  CONFIGURE   = 0b000, //!< (CFG) The ModbusAPI object is configured and ready to connect.
  CONNECTED   = 0b001, //!< (CON) The Connecction to the SMA inverter is established and can be initialized.
  INITIALIZED = 0b011, //!< (INI) The ModbusAPI is initialized.
  ERROR       = 0b100, //!< (ERR) An error occurred.
};

//! Error codes for functions used in modbusSMA.
enum class ErrorCode {
  OK = 0,                   //!< The function was successfull.
  INVALID_STATE,            //!< The function can not be executed with the current sate of the object.
  INVALID_MODBUS_CONTEXT,   //!< Creating the modbus context (IP, etc.) failed.
  MODBUS_CONNECTION_FAILED, //!< Failed to establishe the modbus connection.
  INITIALIZATION_FAILED,    //!< Failed to initialize the API.
  ERROR,                    //!< Generic error.
};

//! The modbus connection type.
enum class ConnectionType {
  TCP_IP,    //!< Normal TCP IP.
  TCP_IP_PI, //!< TCP IP protocol independant.
  RTU,       //!< RTU connection.
};

//! Connvert enums to strings
namespace enum2Str {

std::string toStr(State _state);
std::string toStr(ErrorCode _errocCode);
std::string toStr(ConnectionType _type);

} // namespace enum2Str

} // namespace modbusSMA

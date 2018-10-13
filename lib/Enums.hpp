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
  FILE_NOT_FOUND,           //!< A specified file was not found.
  DATA_BASE_ERROR,          //!< An error generated when handling the database.
  ERROR,                    //!< Generic error.
};

//! The modbus connection type.
enum class ConnectionType {
  TCP_IP,    //!< Normal TCP IP.
  TCP_IP_PI, //!< TCP IP protocol independant.
  RTU,       //!< RTU connection.
};

//! SMA modbus register data types.
enum class DataType {
  S16,   //!< A single word (16-bit).
  S32,   //!<  A signed double word (32-bit).
  STR32, //!<  32 byte data field, in UTF8 format.
  U16,   //!<  A word (16-bit).
  U32,   //!<  A double word (32-bit).
  U64,   //!<  A quadruple word (64-bit).

  __UNKNOWN__, //!< Undefined.
};

//! SMA data format of a modbus register
enum class DataFormat {
  Duration, //!< Time in seconds, in minutes or in hours, depending on the Modbus register.
  DT,       //!<  Date/time, in accordance with country setting. Transmission in seconds since 1970-01-01.
  ENUM,     //!< Coded numerical values (enum).
  FIX0,     //!< Decimal number, commercially rounded, no decimal place.
  FIX1,     //!< Decimal number, commercially rounded, one decimal place.
  FIX2,     //!< Decimal number, commercially rounded, two decimal places.
  FIX3,     //!< Decimal number, commercially rounded, three decimal places.
  FIX4,     //!< Decimal number, commercially rounded, four decimal places.
  FW,       //!< Firmware version (see Section 3.8, "SMA Firmware Data Format (FW)", 15).
  HW,       //!< Hardware version e.g. 24.
  IP4,      //!< 4-byte IP address (IPv4) of the form XXX.XXX.XXX.XXX.
  RAW,      //!< Text or number. A RAW number has no decimal places and no thousand or other separation indicators.
  TM,       //!< UTC time, in seconds.
  UTF8,     //!< Data in UTF8 format.
  REV,      //!< Outline Purchase Agreement.

  /*!
   * Temperature values are stored in special Modbus registers in degrees Celsius (°C),
   * in degrees Fahrenheit (°F), or in Kelvin K. The values are commercially rounded,
   * with one decimal place.
   */
  TEMP,

  /*!
   * The date saved in the register will be transmitted in the event of a
   * change to a function and starts this. After execution of the function,
   * no status value is set. A security question must be executed in the client
   * software prior to execution of the function.
   */
  FUNCTION_SEC,

  __UNKNOWN__, //!< Undefined.
};

//! SMA modbus register access
enum class DataAccess {
  RO = 0, //!< Read only.
  RW = 1, //!< Read and write.
  WO = 2, //!< Write only.

  __UNKNOWN__, //!< Undefined.
};

//! Connvert enums to strings
namespace enum2Str {

std::string toStr(State _state);
std::string toStr(ErrorCode _errocCode);
std::string toStr(ConnectionType _type);
std::string toStr(DataType _type);
std::string toStr(DataFormat _type);
std::string toStr(DataAccess _type);

DataType   typeFromStr(std::string _type);
DataFormat formatFromStr(std::string _type);
DataAccess accessFromStr(std::string _type);

} // namespace enum2Str

} // namespace modbusSMA

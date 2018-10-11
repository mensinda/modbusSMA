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

#include "mSMAConfig.hpp"

#include <string>

#include "Enums.hpp"

//! The main namespace of this library.
namespace modbusSMA {


/*!
 * \brief Main ModbusSMA class
 *
 * This class is the main ModbusSMA interface.
 *
 * The state machine for this class:
 * \dot
 * digraph ModbusAPI_State {
 *   rankdir=LR;
 *   size="8,5";
 *
 *   node [shape = point]; qi;
 *
 *   node [shape = circle, label="CFG"] cfg;
 *   node [shape = circle, label="CON"] con;
 *   node [shape = circle, label="INI"] init;
 *   node [shape = circle, label="ERR"] err;
 *
 *   qi   -> cfg;
 *   cfg  -> con  [label="connect()"];
 *   con  -> init [label="initialize()"];
 *   cfg  -> init [label="setup()"];
 *   con  -> cfg  [label="reset()"];
 *   init -> cfg  [label="reset()"];
 *   err  -> cfg  [label="reset()"];
 *   cfg  -> cfg  [label="reset()"];
 * }
 * \enddot
 */
class ModbusAPI {
 private:
  std::string vIP   = "127.0.0.1"; //!< Test.
  uint32_t    vPort = 512;         //!< Even more tests.

  State vState = State::CONFIGURE; //!< Bleh.

 public:
  ModbusAPI() = default;
  ModbusAPI(std::string _ip, uint32_t _port);
  virtual ~ModbusAPI();

  ErrorCode connect();
  ErrorCode initialize();
  ErrorCode setup();
  void      reset();

  ErrorCode setConnectionTCP(std::string _ip, uint32_t _port);

  inline std::string getIP() { return vIP; }       //!< Returns the IP address.
  inline uint32_t    getPort() { return vPort; }   //!< Returns the TCP port.
  inline State       getState() { return vState; } //!< Get the current state of the API.
};

} // namespace modbusSMA

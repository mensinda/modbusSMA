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

#include <memory>
#include <string>

#include "Enums.hpp"
#include "MBConnectionBase.hpp"

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
  std::unique_ptr<MBConnectionBase> mConn = nullptr;

  State mState = State::CONFIGURE;

 public:
  ModbusAPI() = delete;
  ModbusAPI(std::string _ip, uint32_t _port);
  ModbusAPI(std::string _node, std::string _service);
  ModbusAPI(std::string _device, uint32_t _baud, char _parity, int _dataBit, int _stopBit);
  virtual ~ModbusAPI();

  ModbusAPI(ModbusAPI const &) = delete;
  void operator=(ModbusAPI const &) = delete;

  ErrorCode connect();
  ErrorCode initialize();
  ErrorCode setup();
  void      reset();

  ErrorCode setConnectionTCP_IP(std::string _ip, uint32_t _port);
  ErrorCode setConnectionTCP_IP_PI(std::string _node, std::string _service);
  ErrorCode setConnectionRTU(std::string _device, uint32_t _baud, char _parity, int _dataBit, int _stopBit);

  inline State getState() { return mState; } //!< Get the current state of the API.
};

} // namespace modbusSMA

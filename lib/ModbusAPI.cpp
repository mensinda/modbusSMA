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

#include "mSMAConfig.hpp"

#include "ModbusAPI.hpp"

#include "Logging.hpp"

using namespace std;
using namespace modbusSMA;

/*!
 * \brief Initializes the ModbusAPI with a custom IP and port
 * \param _ip   The IP address to use
 * \param _port The Port to use
 */
ModbusAPI::ModbusAPI(string _ip, uint32_t _port) {
  vIP   = _ip;
  vPort = _port;
}

ModbusAPI::~ModbusAPI() { reset(); }


/*!
 * \brief Resets the state of the object (modbus connection, inverter type, etc.) to CONFIGURE
 *
 * The configuration (IP, port) is NOT reset.
 */
void ModbusAPI::reset() {
  if (vState == State::CONNECTED || vState == State::INITIALIZED) {
    //! \todo Kill the modbus connection
  }
  vState = State::CONFIGURE;
}

/*!
 * \brief Establishes the modbus connection based on the current configuration
 *
 * State change: CONFIGURE --> CONNECTED
 */
ErrorCode ModbusAPI::connect() {
  auto lLogger = log::get();
  if (vState != State::CONFIGURE) {
    lLogger->error("ModbusAPI: can not connect() -- invalid object state '{}'", enum2Str::toStr(vState));
    vState = State::ERROR;
    return ErrorCode::INVALID_STATE;
  }

  //! \todo Implement

  lLogger->info("ModbusAPI: connected");
  vState = State::CONNECTED;
  return ErrorCode::OK;
}

/*!
 * \brief Initializes the API
 *
 * State change: CONNECTED --> INITIALIZED
 */
ErrorCode ModbusAPI::initialize() {
  auto lLogger = log::get();
  if (vState != State::CONNECTED) {
    lLogger->error("ModbusAPI: can not initialize() -- invalid object state '{}'", enum2Str::toStr(vState));
    vState = State::ERROR;
    return ErrorCode::INVALID_STATE;
  }

  //! \todo Implement

  lLogger->info("ModbusAPI: initialized");
  vState = State::INITIALIZED;
  return ErrorCode::OK;
}

/*!
 * \brief Wrapper for connect() and initialize()
 *
 * State change: CONFIGURE --> INITIALIZED
 */
ErrorCode ModbusAPI::setup() {
  ErrorCode lRes = connect();
  if (lRes != ErrorCode::OK) { return lRes; }

  return initialize();
}

/*!
 * \brief Sets the TCP server connection details
 *
 * \note This function can only be called in the CONFIGURE state
 *
 * \param _ip   The IP address to use
 * \param _port The port to use
 *
 * \returns OK or INVALID_STATE
 */
ErrorCode ModbusAPI::setConnectionTCP(std::string _ip, uint32_t _port) {
  auto lLogger = log::get();
  if (vState != State::CONFIGURE) {
    lLogger->error("ModbusAPI: can not setConnectionTCP() -- invalid object state '{}'", enum2Str::toStr(vState));
    return ErrorCode::INVALID_STATE;
  }

  vIP   = _ip;
  vPort = _port;

  return ErrorCode::OK;
}

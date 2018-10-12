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
#include "MBConnectionIP.hpp"
#include "MBConnectionIP_PI.hpp"
#include "MBConnectionRTU.hpp"

using namespace std;
using namespace modbusSMA;

/*!
 * \brief Initializes the ModbusAPI with a custom TCP IP and port
 * \sa setConnectionTCP_IP
 */
ModbusAPI::ModbusAPI(string _ip, uint32_t _port) { setConnectionTCP_IP(_ip, _port); }

/*!
 * \brief Initializes the ModbusAPI with a custom IP and port
 * \sa setConnectionTCP_IP
 */
ModbusAPI::ModbusAPI(string _node, string _service) { setConnectionTCP_IP_PI(_node, _service); }

/*!
 * \brief Initializes the ModbusAPI with a RTU connectopm
 * \sa setConnectionRTU
 */
ModbusAPI::ModbusAPI(string _device, uint32_t _baud, char _parity, int _dataBit, int _stopBit) {
  setConnectionRTU(_device, _baud, _parity, _dataBit, _stopBit);
}


ModbusAPI::~ModbusAPI() { reset(); }


/*!
 * \brief Resets the state of the object (modbus connection, inverter type, etc.) to CONFIGURE
 *
 * The configuration (IP, port) is NOT reset.
 */
void ModbusAPI::reset() {
  if (mState == State::CONNECTED || mState == State::INITIALIZED) {
    mConn->disconnect();
    mConn = nullptr;
  }
  mState = State::CONFIGURE;
}

/*!
 * \brief Establishes the modbus connection based on the current configuration
 *
 * State change: CONFIGURE --> CONNECTED
 */
ErrorCode ModbusAPI::connect() {
  auto logger = log::get();
  if (mState != State::CONFIGURE) {
    logger->error("ModbusAPI: can not connect() -- invalid object state '{}'", enum2Str::toStr(mState));
    mState = State::ERROR;
    return ErrorCode::INVALID_STATE;
  }

  auto res = mConn->connect();
  if (res != ErrorCode::OK) {
    logger->error("ModbusAPI: unable to connect: '{}'", enum2Str::toStr(res));
    mState = State::ERROR;
    mConn  = nullptr;
    return res;
  }

  logger->info("ModbusAPI: connected");
  mState = State::CONNECTED;
  return ErrorCode::OK;
}

/*!
 * \brief Initializes the API
 *
 * State change: CONNECTED --> INITIALIZED
 */
ErrorCode ModbusAPI::initialize() {
  auto logger = log::get();
  if (mState != State::CONNECTED) {
    logger->error("ModbusAPI: can not initialize() -- invalid object state '{}'", enum2Str::toStr(mState));
    mState = State::ERROR;
    return ErrorCode::INVALID_STATE;
  }

  //! \todo Implement

  logger->info("ModbusAPI: initialized");
  mState = State::INITIALIZED;
  return ErrorCode::OK;
}

/*!
 * \brief Wrapper for connect() and initialize()
 *
 * State change: CONFIGURE --> INITIALIZED
 */
ErrorCode ModbusAPI::setup() {
  ErrorCode res = connect();
  if (res != ErrorCode::OK) { return res; }

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
ErrorCode ModbusAPI::setConnectionTCP_IP(string _ip, uint32_t _port) {
  if (mState != State::CONFIGURE) {
    log::get()->error("ModbusAPI: setConnectionTCP_IP() -- invalid object state '{}'", enum2Str::toStr(mState));
    return ErrorCode::INVALID_STATE;
  }

  mConn = make_unique<MBConnectionIP>(_ip, _port);
  return ErrorCode::OK;
}

/*!
 * \brief Sets the TCP IP protocol indemendant server connection details
 *
 * \note This function can only be called in the CONFIGURE state
 *
 * \param _node    The server node
 * \param _service The service
 *
 * \returns OK or INVALID_STATE
 */
ErrorCode ModbusAPI::setConnectionTCP_IP_PI(string _node, string _service) {
  if (mState != State::CONFIGURE) {
    log::get()->error("ModbusAPI: setConnectionTCP_IP_PI() -- invalid object state '{}'", enum2Str::toStr(mState));
    return ErrorCode::INVALID_STATE;
  }

  mConn = make_unique<MBConnectionIP_PI>(_node, _service);
  return ErrorCode::OK;
}

/*!
 * \brief Sets the TCP server connection details
 *
 * \note This function can only be called in the CONFIGURE state
 *
 * \param _device   The name of the serial port
 * \param _baud     The baud rate of the communication
 * \param _parity   Partity type: N = None; E = Even; O = Odd
 * \param _dataBit  The number of bits of data, the allowed values are 5, 6, 7 and 8
 * \param _stopBit  The bits of stop, the allowed values are 1 and 2
 *
 * \returns OK or INVALID_STATE
 */
ErrorCode ModbusAPI::setConnectionRTU(string _device, uint32_t _baud, char _parity, int _dataBit, int _stopBit) {
  if (mState != State::CONFIGURE) {
    log::get()->error("ModbusAPI: setConnectionRTU() -- invalid object state '{}'", enum2Str::toStr(mState));
    return ErrorCode::INVALID_STATE;
  }

  mConn = make_unique<MBConnectionRTU>(_device, _baud, _parity, _dataBit, _stopBit);
  return ErrorCode::OK;
}

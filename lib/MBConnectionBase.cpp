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

#include "MBConnectionBase.hpp"

#include <modbus/modbus.h>

#include "Logging.hpp"

using namespace std;
using namespace modbusSMA;

//! Disconnects the modbus connection if neccessary.
MBConnectionBase::~MBConnectionBase() { disconnect(); }

/*!
 * \brief Creates the modbus connection based on the backend context implementation
 * \note A previously established connection is disconnected when calling this function.
 */
ErrorCode MBConnectionBase::connect() {
  auto logger = log::get();
  if (mConnection) { disconnect(); }

  mConnection = createModbusContext();
  if (!mConnection) { return ErrorCode::INVALID_MODBUS_CONTEXT; }

  logger->debug("MBConnectionBase: Establishing the modbus connection");
  if (modbus_connect(mConnection) == -1) {
    logger->error("MBConnectionBase: Failed to establish the modbus connection: '{}'", modbus_strerror(errno));
    modbus_close(mConnection);
    modbus_free(mConnection);
    mConnection = nullptr;
    return ErrorCode::MODBUS_CONNECTION_FAILED;
  }

  return ErrorCode::OK;
}

//! Disconnects an active modbus connnection (if present, else does nothing).
void MBConnectionBase::disconnect() {
  if (!mConnection) { return; }

  log::get()->debug("MBConnectionBase: Closing the current modbus connection");
  modbus_close(mConnection);
  modbus_free(mConnection);
  mConnection = nullptr;
}

/*!
 * \brief read _num registers from the device
 *
 * The maximum number of registers is limited by MODBUS_MAX_REGISTER_COUNT
 *
 * \param _reg The starting register
 * \param _num The number of registers to read
 *
 * \returns A vector of the result OR an empty vector on error
 */
vector<uint16_t> MBConnectionBase::readRegisters(uint32_t _reg, uint32_t _num) {
  if (_num > MODBUS_MAX_REGISTER_COUNT) {
    auto logger = log::get();
    logger->error("MBConnectionBase: readRegisters(_reg = {}, _num = {}): ", _reg, _num);
    logger->error("  -- Can not request {} registers. Max register count is {}", _num, MODBUS_MAX_REGISTER_COUNT);
    return {};
  }

  vector<uint16_t> vecOut;
  vecOut.resize(_num);

  if (modbus_read_registers(mConnection, _reg, _num, vecOut.data()) < 0) {
    auto logger = log::get();
    logger->error("MBConnectionBase: readRegisters(_reg = {}, _num = {}): ", _reg, _num);
    logger->error("  -- Request failed with '{}'", modbus_strerror(errno));
    return {};
  }

  return vecOut;
}

/*!
 * \brief Sets the slave/uinit ID of the modbus connection
 *
 * \returns ErrorCode::OK on success
 *          ErrorCode::INVALID_STATE if not connected
 *          ErrorCode::ERROR when setting the id failed
 */
ErrorCode MBConnectionBase::setSlaveID(int _id) {
  if (!isConnected()) { return ErrorCode::INVALID_STATE; }

  if (modbus_set_slave(mConnection, _id) != 0) {
    log::get()->error("Failed to set the slave id to {}. Error: '{}'", _id, modbus_strerror(errno));
    return ErrorCode::ERROR;
  }

  return ErrorCode::OK;
}

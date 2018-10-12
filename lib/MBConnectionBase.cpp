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

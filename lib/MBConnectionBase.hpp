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

#include <modbus/modbus.h>
#include <string>
#include <vector>

#include "Enums.hpp"

namespace modbusSMA {

/*!
 * \brief Base class for the modbus connection
 *
 * This class handles connecting and disconnecting the modbus interface.
 * Recieving and sending raw data is also handled here.
 *
 * The modbus context creation (IP, RTU, etc.) is handled in the subclasses.
 *
 * \note The derived classes are only responsible for creating the context. All other modbus_t context handling
 *       (including destruction) is done in this class.
 */
class MBConnectionBase {
 private:
  modbus_t *mConnection = nullptr;

 protected:
  virtual modbus_t *createModbusContext() = 0; //!< Create and return the modbus context.

 public:
  MBConnectionBase() = default;
  virtual ~MBConnectionBase();

  MBConnectionBase(MBConnectionBase const &) = delete;
  void operator=(MBConnectionBase const &) = delete;

  ErrorCode connect();
  void      disconnect();
  bool      isConnected() const { return mConnection != nullptr; } //!< Returns whether a valid conection exists.

  inline modbus_t *getConnection() { return mConnection; } //!< Returns the raw connection. DO NOT close OR free it.

  virtual ConnectionType type() = 0; //!< Returns the modbus connection type
};


} // namespace modbusSMA

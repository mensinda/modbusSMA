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

#include "MBConnectionBase.hpp"

namespace modbusSMA {

//! Class for an IP based modbus connection.
class MBConnectionIP : public MBConnectionBase {
 private:
  std::string mIP;
  uint32_t    mPort;

 protected:
  modbus_t *createModbusContext() override;

 public:
  MBConnectionIP() = delete;
  MBConnectionIP(std::string _ip, uint32_t _port);

  MBConnectionIP(MBConnectionIP const &) = delete;
  void operator=(MBConnectionIP const &) = delete;

  std::string getIP() const { return mIP; }     //!< Returns the IP address used.
  uint32_t    getPort() const { return mPort; } //!< Returns the Port used.

  ConnectionType type() override { return ConnectionType::TCP_IP; }
};

} // namespace modbusSMA

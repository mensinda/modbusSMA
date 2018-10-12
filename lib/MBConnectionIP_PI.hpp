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
class MBConnectionIP_PI : public MBConnectionBase {
 private:
  std::string mNode;
  std::string mService;

 protected:
  modbus_t *createModbusContext() override;

 public:
  MBConnectionIP_PI() = delete;
  MBConnectionIP_PI(std::string _node, std::string _service);

  MBConnectionIP_PI(MBConnectionIP_PI const &) = delete;
  void operator=(MBConnectionIP_PI const &) = delete;

  std::string getNode() const { return mNode; }       //!< Returns the Node used.
  std::string getService() const { return mService; } //!< Returns the service used.

  ConnectionType type() override { return ConnectionType::TCP_IP_PI; }
};

} // namespace modbusSMA

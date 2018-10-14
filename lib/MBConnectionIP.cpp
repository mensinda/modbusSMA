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

#include "MBConnectionIP.hpp"

#include <modbus/modbus-tcp.h>

#include "Logging.hpp"

using namespace std;
using namespace modbusSMA;

//! This constructor only initializes the IP and the port
MBConnectionIP::MBConnectionIP(std::string _ip, uint32_t _port) : mIP(_ip), mPort(_port) {}

/*!
 * \brief Creates the modbus IP context
 *
 * Returns nullptr on error.
 */
modbus_t *MBConnectionIP::createModbusContext() {
  auto logger = log::get();
  logger->debug("MBConnectionIP: Creating modbus context for {} port: {}", mIP, mPort);
  modbus_t *ctx = modbus_new_tcp(mIP.c_str(), (int)mPort);

  if (!ctx) {
    logger->error("MBConnectionIP: Failed to create the modbus context: {}", modbus_strerror(errno));
    return nullptr;
  }

  return ctx;
}

string MBConnectionIP::description() { return fmt::format("TCP-IP: {}:{}", mIP, mPort); }

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

#include "MBConnectionIP_PI.hpp"

#include <modbus/modbus-tcp.h>

#include "Logging.hpp"

using namespace std;
using namespace modbusSMA;

//! This constructor only initializes the IP and the port
MBConnectionIP_PI::MBConnectionIP_PI(std::string _node, std::string _service) : mNode(_node), mService(_service) {}

/*!
 * \brief Creates the modbus IP context
 *
 * Returns nullptr on error.
 */
modbus_t *MBConnectionIP_PI::createModbusContext() {
  auto logger = log::get();
  logger->debug("MBConnectionIP_PI: Creating modbus context for Node: {} Service: {}", mNode, mService);
  modbus_t *ctx = modbus_new_tcp_pi(mNode.c_str(), mService.c_str());

  if (!ctx) {
    logger->error("MBConnectionIP_PI: Failed to create the modbus context: {}", modbus_strerror(errno));
    return nullptr;
  }

  return ctx;
}

string MBConnectionIP_PI::description() { return fmt::format("TCP Node: '{}'; Service: '{}'", mNode, mService); }

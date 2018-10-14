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

struct CFG {
  std::string db = SMA_MODBUS_DEFAULT_DB;

  struct TcpIP {
    std::string ip   = "127.0.0.1";
    uint32_t    port = 502;
  } tcpIP;

  struct TcpIP_PI {
    std::string node    = "::1";
    std::string service = "502";
  } tcpIP_PI;

  struct RTU {
    std::string device  = "/dev/null";
    uint32_t    baud    = 800;
    char        parity  = 'N';
    int         dataBit = 8;
    int         stopBit = 1;
  } rtu;
};

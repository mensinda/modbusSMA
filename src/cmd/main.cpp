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

#include <iostream>

#include "CFG.hpp"
#include "CLI11.hpp"
#include "DataBase.hpp"
#include "Logging.hpp"
#include "ModbusAPI.hpp"

using namespace std;
using namespace spdlog;
using namespace modbusSMA;

int main(int argc, char *argv[]) {
  auto      logger = log::get();
  ModbusAPI mapi("127.0.0.1", 512); // Config will be overwritten later
  DataBase  db;
  CFG       cfg;
  ErrorCode result;

  CLI::App app{"modbusSMA CLI client"};

  app.add_flag("--version",
               [logger](size_t) -> void {
                 cout << "Version " << MODBUS_SMA_VERSION << endl;
                 exit(0);
               },
               "Print the version and exit");

  app.add_option("-d,--database", cfg.db, "Path to the modbusSMA database", true)->check(CLI::ExistingFile);

  auto lFlagV = app.add_flag("-v,--verbose", "Verbose logging");
  auto lFlagQ = app.add_flag("-q,--quiet", "Will only log warnings and errors");

  CLI::App *tcpIP = app.add_subcommand("ip", "TCP IP mode")->fallthrough()->ignore_case();
  tcpIP->add_option("-I,--ip", cfg.tcpIP.ip, "IP address of the modbus server")->required();
  tcpIP->add_option("-P,--port", cfg.tcpIP.port, "Port of the modbus server", true);

  CLI::App *tcpIP_PI = app.add_subcommand("ip_pi", "TCP IP protocol independant mode")->fallthrough()->ignore_case();
  tcpIP_PI->add_option("-N,--node", cfg.tcpIP_PI.node, "The server node")->required();
  tcpIP_PI->add_option("-S,--service", cfg.tcpIP_PI.service, "Service of the modbus server")->required();

  CLI::App *rtu = app.add_subcommand("rtu", "RTU serial protocol")->fallthrough()->ignore_case();
  rtu->add_option("--device", cfg.rtu.device, "Name of the serial port handled by the OS")->required();
  rtu->add_option("--baud", cfg.rtu.baud, "Baud rate of the communication")->required();
  rtu->add_option("--parity", cfg.rtu.parity, "Parity type: (N)one (E)ven (O)dd")->required();
  rtu->add_option("--dataBit", cfg.rtu.dataBit, "The number of bits of data, the allowed values: 5-8")->required();
  rtu->add_option("--stopBit", cfg.rtu.stopBit, "The bits of stop, the allowed values are 1 and 2")->required();

  app.require_subcommand(1, 1);

  CLI11_PARSE(app, argc, argv);

  if (lFlagQ->count() > 0 && lFlagQ->count() > lFlagV->count()) { logger->set_level(level::warn); }
  if (lFlagV->count() > 0 && lFlagV->count() > lFlagQ->count()) { logger->set_level(level::debug); }

  logger->info("Loading the database: {}", cfg.db);
  result = db.connect(cfg.db);

  if (result != ErrorCode::OK) {
    logger->error("Failed to load the register database: '{}'", enum2Str::toStr(result));
    return 1;
  }


  logger->info("Starting the modbus CLI server");

  if (*tcpIP) { mapi.setConnectionTCP_IP(cfg.tcpIP.ip, cfg.tcpIP.port); }
  if (*tcpIP_PI) { mapi.setConnectionTCP_IP_PI(cfg.tcpIP_PI.node, cfg.tcpIP_PI.service); }
  if (*rtu) { mapi.setConnectionRTU(cfg.rtu.device, cfg.rtu.baud, cfg.rtu.parity, cfg.rtu.dataBit, cfg.rtu.stopBit); }

  result = mapi.setup();

  if (result != ErrorCode::OK) {
    logger->error("Client setup failed with '{}'", enum2Str::toStr(result));
    return 1;
  }

  return 0;
}

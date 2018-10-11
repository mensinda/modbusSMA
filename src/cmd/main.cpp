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
#include "Logging.hpp"
#include "ModbusAPI.hpp"

using namespace std;
using namespace spdlog;
using namespace modbusSMA;

int main(int argc, char *argv[]) {
  auto      lLogger = log::get();
  ModbusAPI lMAPI;
  CFG       lCfg;

  CLI::App lApp{"modbusSMA CLI client"};

  lApp.add_flag("--version",
                [lLogger](size_t) -> void {
                  cout << "Version " << MODBUS_SMA_VERSION << endl;
                  exit(0);
                },
                "Print the version and exit");

  auto lFlagV = lApp.add_flag("-v,--verbose", "Verbose logging");
  auto lFlagQ = lApp.add_flag("-q,--quiet", "Will only log warnings and errors");

  lApp.add_option("-I,--ip", lCfg.ip, "IP address of the modbus server");
  lApp.add_option("-P,--port", lCfg.port, "Port of the modbus server");

  CLI11_PARSE(lApp, argc, argv);

  if (lFlagQ->count() > 0 && lFlagQ->count() > lFlagV->count()) { lLogger->set_level(level::warn); }
  if (lFlagV->count() > 0 && lFlagV->count() > lFlagQ->count()) { lLogger->set_level(level::debug); }

  lLogger->info("Starting the modbus CLI server");

  lMAPI.setConnectionTCP(lCfg.ip, lCfg.port);
  auto lRes = lMAPI.setup();

  if (lRes != ErrorCode::OK) {
    lLogger->error("Client setup failed with '{}'", enum2Str::toStr(lRes));
    return 1;
  }

  return 0;
}

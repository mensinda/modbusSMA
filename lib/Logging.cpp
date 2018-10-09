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

#include "mSMAConfig.hpp"

#include "Logging.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

using namespace std;
using namespace spdlog;
using namespace modbusSMA;

log::LOGGER log::initialize(vector<sink_ptr> _sinks) {
  auto lLogger = spdlog::get(MODBUS_SMA_LOGGER_NAME);
  if (lLogger) {
    lLogger->info("Logger '{}' was already initialized", MODBUS_SMA_LOGGER_NAME);
    return lLogger;
  }

  if (!_sinks.empty()) {
    lLogger = make_shared<logger>(MODBUS_SMA_LOGGER_NAME, begin(_sinks), end(_sinks));
    register_logger(lLogger);
  } else {
    lLogger = stdout_color_mt(MODBUS_SMA_LOGGER_NAME);
  }

  return lLogger;
}

log::LOGGER log::get() {
  auto lLogger = spdlog::get(MODBUS_SMA_LOGGER_NAME);

  if (!lLogger) { lLogger = initialize(); }

  return lLogger;
}

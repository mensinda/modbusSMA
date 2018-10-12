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
using namespace modbusSMA;

/*!
 * \brief Initializes the logger for the ModbusSMA library
 *
 * If no sinks are specified, the default stdout_color_mt logger is used.
 * Does nothing if the logger already exists.
 *
 * \param _sinks Sinks to add to the logger
 * \returns the created logger
 */
log::LOGGER log::initialize(std::vector<spdlog::sink_ptr> _sinks) {
  auto logger = spdlog::get(MODBUS_SMA_LOGGER_NAME);
  if (logger) {
    logger->info("Logger '{}' was already initialized", MODBUS_SMA_LOGGER_NAME);
    return logger;
  }

  if (!_sinks.empty()) {
    logger = make_shared<spdlog::logger>(MODBUS_SMA_LOGGER_NAME, begin(_sinks), end(_sinks));
    register_logger(logger);
  } else {
    logger = spdlog::stdout_color_mt(MODBUS_SMA_LOGGER_NAME);
  }

  return logger;
}

/*!
 * \brief Returns the logger used in the modbusSMA library
 *
 * Default initializes the logger if it is not already initialized.
 */
log::LOGGER log::get() {
  auto logger = spdlog::get(MODBUS_SMA_LOGGER_NAME);

  if (!logger) { logger = initialize(); }

  return logger;
}

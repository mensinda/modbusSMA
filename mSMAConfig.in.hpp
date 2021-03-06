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

#mesondefine SMA_MODBUS_USE_EXTERNAL_FMT

#if SMA_MODBUS_USE_EXTERNAL_FMT
#  define SPDLOG_FMT_EXTERNAL 1
#endif

#include <cstdint>

#mesondefine SMA_MODBUS_SMA_VERSION
#mesondefine SMA_MODBUS_SMA_LOGGER_NAME
#mesondefine SMA_MODBUS_MAX_REGISTER_COUNT
#mesondefine SMA_MODBUS_INSTALL_PREFIX
#mesondefine SMA_MODBUS_INSTALL_DATA_DIR
#mesondefine SMA_MODBUS_DEFAULT_DB

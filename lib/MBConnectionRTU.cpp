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

#include "MBConnectionRTU.hpp"

#include <modbus/modbus-rtu.h>

#include "Logging.hpp"

using namespace std;
using namespace modbusSMA;

//! This constructor only initializes the IP and the port
MBConnectionRTU::MBConnectionRTU(string _device, uint32_t _baud, char _parity, int _dataBit, int _stopBit)
    : mDevice(_device), mBaud(_baud), mParity(_parity), mDataBit(_dataBit), mStopBit(_stopBit) {}

/*!
 * \brief Creates the modbus IP context
 *
 * Returns nullptr on error.
 */
modbus_t *MBConnectionRTU::createModbusContext() {
  auto lLogger = log::get();
  lLogger->debug("MBConnectionRTU: Creating modbus context for RTU device: {}", mDevice);
  lLogger->debug("  -- Baud:     {}", mBaud);
  lLogger->debug("  -- Parity:   {}", mParity);
  lLogger->debug("  -- Data Bit: {}", mDataBit);
  lLogger->debug("  -- Stop Bit: {}", mStopBit);
  modbus_t *lCTX = modbus_new_rtu(mDevice.c_str(), (int)mBaud, mParity, mDataBit, mStopBit);

  if (!lCTX) {
    lLogger->error("MBConnectionRTU: Failed to create the modbus context: {}", modbus_strerror(errno));
    return nullptr;
  }

  return lCTX;
}

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
class MBConnectionRTU : public MBConnectionBase {
 private:
  std::string mDevice;
  uint32_t    mBaud;
  char        mParity;
  int         mDataBit;
  int         mStopBit;

 protected:
  modbus_t *createModbusContext() override;

 public:
  MBConnectionRTU() = delete;
  MBConnectionRTU(std::string _device, uint32_t _baud, char _parity, int _dataBit, int _stopBit);

  MBConnectionRTU(MBConnectionRTU const &) = delete;
  void operator=(MBConnectionRTU const &) = delete;

  std::string getDevice() const { return mDevice; }   //!< Returns the device used.
  uint32_t    getBaud() const { return mBaud; }       //!< Returns the baud used.
  char        getParity() const { return mParity; }   //!< Returns the parity used.
  int         getDataBit() const { return mDataBit; } //!< Returns the data bit used.
  int         getStopBit() const { return mStopBit; } //!< Returns the stop bit used.

  ConnectionType type() override { return ConnectionType::RTU; }
};

} // namespace modbusSMA

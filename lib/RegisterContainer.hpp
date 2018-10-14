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

#include <functional>
#include <vector>

#include "Register.hpp"

namespace modbusSMA {

//! Container for all modbus registers.
class RegisterContainer {
 private:
  std::vector<Register> mRegisters;

 public:
  RegisterContainer() = default;

  inline size_t   size() const { return mRegisters.size(); }                   //!< The number of registers stored.
  inline bool     empty() const { return mRegisters.empty(); }                 //!< Checks if empty.
  inline Register at(uint16_t _idx) const { return mRegisters.at(_idx); }      //!< Returns the register at index _idx.
  inline Register operator[](uint16_t _idx) const { return mRegisters[_idx]; } //!< Returns the register at index _idx.

  void addRegisters(std::vector<Register> _registers);
  bool updateRegister(uint16_t _address, std::vector<uint16_t> _data);

  std::vector<Register> getRegisters(std::vector<uint16_t> _regList);
  std::vector<Register> getRegisters() const { return mRegisters; } //!< Returns a COPY of ALL registers.
};

} // namespace modbusSMA

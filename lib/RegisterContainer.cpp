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

#include "RegisterContainer.hpp"

using namespace std;
using namespace modbusSMA;

/*!
 * \brief Get a vector of Register from a list of uint16_t register ids
 *
 * \param _regList The registers to return
 * \returns A list all found registers (_regList.size() == returnedList.size() ==> all registers found)
 */
vector<Register> RegisterContainer::getRegisters(vector<uint16_t> _regList) {
  vector<Register> outRegList = {};
  outRegList.reserve(_regList.size()); // Best case: all registers are found

  for (uint16_t i : _regList) {
    auto pos = lower_bound(begin(mRegisters), end(mRegisters), i); // the register vector is sorted
    if (*pos == i) { outRegList.push_back(*pos); }
  }

  return outRegList;
}

//! Adds the registers to the register list.
void RegisterContainer::addRegisters(vector<Register> _registers) {
  mRegisters.insert(end(mRegisters), begin(_registers), end(_registers));
  stable_sort(begin(mRegisters), end(mRegisters));
  mRegisters.erase(unique(mRegisters.begin(), mRegisters.end()), mRegisters.end());
}

//! Updates already existing registers
bool RegisterContainer::updateRegister(uint16_t _address, vector<uint16_t> _data) {
  auto pos = lower_bound(begin(mRegisters), end(mRegisters), _address); // the register vector is always sorted
  if (not(*pos == _address)) { return false; }

  return pos->setRaw(_data);
}

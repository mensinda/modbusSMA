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

#include "Register.hpp"

using namespace std;
using namespace modbusSMA;

//! Initializes the register.
Register::Register(uint16_t    _reg,    //!< The starting register.
                   std::string _desc,   //!< Textual desctiption of the register.
                   std::string _unit,   //!< Unit of the data stored.
                   DataType    _type,   //!< The data type.
                   DataFormat  _format, //!< The data format.
                   DataAccess  _access  //!< How this register can be accessed.
                   )
    : mReg(_reg), mDesc(_desc), mUnit(_unit), mType(_type), mFormat(_format), mAccess(_access) {}

//! The size in 16-bit words of this register
uint32_t Register::size() const noexcept {
  switch (mType) {
    case DataType::S16:
    case DataType::U16: return 1;
    case DataType::U32:
    case DataType::S32: return 2;
    case DataType::U64: return 4;
    case DataType::STR32: return 16;
    default: return UINT32_MAX;
  }
}

//! Sets the new raw data. Returns false if the size differs from the expected size.
bool Register::setRaw(std::vector<uint16_t> _data) {
  if (_data.size() != size()) { return false; }
  mData = _data;
  return true;
}

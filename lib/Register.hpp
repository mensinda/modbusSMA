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
#include <vector>

#include "Enums.hpp"

namespace modbusSMA {

/*!
 * \brief Data and information of one SMA register
 */
class Register {
 private:
  uint16_t    mReg;
  std::string mDesc;
  std::string mUnit;
  DataType    mType;
  DataFormat  mFormat;
  DataAccess  mAccess;

  std::vector<uint16_t> mData;

 public:
  Register() = delete;
  Register(uint16_t _reg, std::string _desc, std::string _unit, DataType _type, DataFormat _format, DataAccess _access);

  inline uint16_t    reg() const noexcept { return mReg; }       //!< Returns the register (integer).
  inline std::string desc() const noexcept { return mDesc; }     //!< Returns the register description.
  inline std::string unit() const noexcept { return mUnit; }     //!< Returns the unit of the register.
  inline DataType    type() const noexcept { return mType; }     //!< Returns the data type.
  inline DataFormat  format() const noexcept { return mFormat; } //!< Returns the data format.
  inline DataAccess  access() const noexcept { return mAccess; } //!< Returns the access type.

  inline bool canRead() const noexcept { return mAccess == DataAccess::RO || mAccess == DataAccess::RW; }  //!< Read?
  inline bool canWrite() const noexcept { return mAccess == DataAccess::RW || mAccess == DataAccess::WO; } //!< Write?

  inline bool operator<(const Register &_b) { return mReg < _b.mReg; }   //!< Only compares the 16-bit register address.
  inline bool operator<(const uint16_t &_b) { return mReg < _b; }        //!< Only compares the 16-bit register address.
  inline bool operator==(const Register &_b) { return mReg == _b.mReg; } //!< Only compares the 16-bit register address.
  inline bool operator==(const uint16_t &_b) { return mReg == _b; }      //!< Only compares the 16-bit register address.

  uint32_t size() const noexcept;
};

} // namespace modbusSMA

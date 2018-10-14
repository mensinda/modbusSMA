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

#include <regex>
#include <sstream>

#include "Logging.hpp"

using namespace std;
using namespace modbusSMA;

template <typename Out>
void split(const string &s, char delim, Out result) {
  stringstream ss(s);
  string       item;
  while (getline(ss, item, delim)) { *(result++) = item; }
}

vector<string> split(const string &s, char delim) {
  vector<string> elems;
  split(s, delim, back_inserter(elems));
  return elems;
}

string trim(const string &str) {
  size_t first = str.find_first_not_of(" \n\t");
  if (string::npos == first) { return str; }
  size_t last = str.find_last_not_of(" \n\t");
  return str.substr(first, (last - first + 1));
}

//! Initializes the register.
Register::Register(uint16_t    _reg,    //!< The starting register.
                   std::string _desc,   //!< Textual desctiption of the register.
                   std::string _unit,   //!< Unit of the data stored.
                   DataType    _type,   //!< The data type.
                   DataFormat  _format, //!< The data format.
                   DataAccess  _access  //!< How this register can be accessed.
                   )
    : mReg(_reg), mDesc(_desc), mUnit(_unit), mType(_type), mFormat(_format), mAccess(_access) {
  resetData();

  vector<string> splitDesc = split(mDesc, '\n');
  if (splitDesc.size() > 1) {
    for (size_t i = 1; i < splitDesc.size(); ++i) {
      vector<string> currSplit = split(splitDesc[i], '=');
      if (currSplit.size() < 2) { continue; }

      string num  = trim(currSplit[0]);
      string name = currSplit[1];

      for (size_t j = 2; j < currSplit.size(); ++j) { name += "=" + currSplit[j]; }
      name = trim(name);

      uint32_t number;
      try {
        number = (uint32_t)stoi(num);
      } catch (...) { continue; }

      mEnumData[number] = name;
    }
  }
}

//! The size in 16-bit words of this register (returns 2 for an unknown type)
uint32_t Register::size() const noexcept {
  switch (mType) {
    case DataType::S16:
    case DataType::U16: return 1;
    case DataType::S32:
    case DataType::U32: return 2;
    case DataType::S64:
    case DataType::U64: return 4;
    case DataType::STR32: return 16;
    default: return 2;
  }
}

//! Generate NaN values for the current data type.
vector<uint16_t> Register::getNaN() {
  switch (mType) {
    case DataType::S16: return {0x8000}; break;
    case DataType::S32: return {0x8000, 0x0000}; break;
    case DataType::S64: return {0x8000, 0x0000, 0x0000, 0x0000}; break;
    case DataType::U16: return {0xFFFF}; break;
    case DataType::U32: return {0xFFFF, 0xFFFF}; break;
    case DataType::U64: return {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF}; break;
    case DataType::STR32: return {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; break;
    default: return {0x0000, 0x0000};
  }
}



//! Sets the new raw data. Returns false if the size differs from the expected size.
bool Register::setRaw(std::vector<uint16_t> _data) {
  if (_data.size() != size()) { return false; }
  mData = _data;
  return true;
}

//! Returns the value of the register as a string.
string Register::value() {
  char *raw = reinterpret_cast<char *>(mData.data());
  if (mType == DataType::STR32) {
    if (mData.size() != 16) { return "Conversion ERROR: Invalid Data size!"; }
    char data[33];
    for (uint32_t i = 0; i < 32; i += 2) {
      data[i + 0] = raw[i + 1];
      data[i + 1] = raw[i + 0];
    }
    data[32] = '\0';
    return data;
  }

  if (mData == getNaN()) { return "NaN"; }

  uint32_t numDec = UINT32_MAX;

  switch (mFormat) {
    case DataFormat::FW: {
      if (mData.size() < 2) { return "Conversion ERROR: Invalid Data size!"; }
      string suffix;
      switch ((uint)raw[2]) {
        case 0: suffix = "N"; break;
        case 1: suffix = "E"; break;
        case 2: suffix = "A"; break;
        case 3: suffix = "B"; break;
        case 4: suffix = "R"; break;
        case 5: suffix = "S"; break;
        default: suffix = to_string((uint)raw[3]); break;
      }
      return fmt::format("{}.{:0>2}.{:0>2}.{}", (uint)raw[1], (uint)raw[0], (uint)raw[3], suffix);
    }

    case DataFormat::IP4:
    case DataFormat::REV:
      if (mData.size() < 2) { return "Conversion ERROR: Invalid Data size!"; }
      return fmt::format("{}.{}.{}.{}", (uint)raw[0], (uint)raw[1], (uint)raw[2], (uint)raw[3]);

    case DataFormat::DT: {
      time_t time = (time_t)valueUInt();
      return asctime(localtime(&time));
    }

    case DataFormat::ENUM: {
      auto iter = mEnumData.find((uint32_t)valueUInt());
      if (iter == end(mEnumData)) { return fmt::format("ENUM: {}", valueUInt()); }
      return iter->second;
    }

    case DataFormat::FIX0:
    case DataFormat::Duration:
    case DataFormat::HW:
    case DataFormat::RAW:
    case DataFormat::TM:
    case DataFormat::FUNCTION_SEC: numDec = 0; [[fallthrough]];
    case DataFormat::TEMP:
    case DataFormat::FIX1: numDec = numDec == UINT32_MAX ? 1 : numDec; [[fallthrough]];
    case DataFormat::FIX2: numDec = numDec == UINT32_MAX ? 2 : numDec; [[fallthrough]];
    case DataFormat::FIX3: numDec = numDec == UINT32_MAX ? 3 : numDec; [[fallthrough]];
    case DataFormat::FIX4:
      numDec = numDec == UINT32_MAX ? 4 : numDec;

      switch (mType) {
        case DataType::S16:
        case DataType::S32:
        case DataType::S64:
          if (numDec > 0 && numDec < 8) {
            bool    wasNegative = false;
            int64_t val         = valueInt();
            if (val < 0) {
              val *= -1;
              wasNegative = true;
            }
            string valStr = fmt::format("{}", val);

            while (valStr.size() < (numDec + 1)) { valStr.insert(0, "0"); }
            valStr.insert(valStr.size() - numDec, ".");

            if (wasNegative) { valStr.insert(0, "-"); }
            return valStr;
          }
          return fmt::format("{}", valueInt());
        case DataType::U16:
        case DataType::U32:
        case DataType::U64:
          if (numDec > 0 && numDec < 8) {
            string valStr = fmt::format("{}", valueUInt());
            while (valStr.size() < (numDec + 1)) { valStr.insert(0, "0"); }
            valStr.insert(valStr.size() - numDec, ".");
            return valStr;
          }
          return fmt::format("{}", valueUInt());
        default: return "<UNKNOWN TYPE>";
      }

    default: break;
  }

  // Should never be executed
  return to_string(valueUInt());
}

//! Get the value as an signed integer. Fixed point number formats are ignored.
int64_t Register::valueInt() {
  switch (mType) {
    case DataType::S16:
    case DataType::U16: return (int16_t)mData[0];
    case DataType::S32:
    case DataType::U32: return (int32_t)(((uint32_t)mData[0] << 16) + mData[1]);
    case DataType::S64:
    case DataType::U64:
      return (int64_t)(((uint64_t)mData[0] << 48) + ((uint64_t)mData[0] << 32) + ((uint32_t)mData[0] << 16) + mData[0]);
    default: return 0;
  }
}

//! Get the value as an unsigned integer. Fixed point number formats are ignored.
uint64_t Register::valueUInt() { return (uint64_t)valueInt(); }

//! Get the value as floating point variable.
double Register::valueDouble() {
  double value = (double)valueInt();

  switch (mFormat) {
    case DataFormat::FIX1: value /= 10.0; break;
    case DataFormat::FIX2: value /= 100.0; break;
    case DataFormat::FIX3: value /= 1000.0; break;
    case DataFormat::FIX4: value /= 10000.0; break;
    default: break;
  }

  return value;
}

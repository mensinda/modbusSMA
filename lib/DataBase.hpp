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

#include <sqlite3.h>
#include <string>
#include <vector>

#include "Enums.hpp"
#include "Register.hpp"

namespace modbusSMA {

namespace internal {

//! SQL query helper class.
//! \internal
class SQL_Query final {
 private:
  sqlite3_stmt *mSTMT = nullptr;

 public:
  SQL_Query() = delete;
  SQL_Query(sqlite3 *_db, std::string _query);
  ~SQL_Query();

  inline sqlite3_stmt *get() { return mSTMT; }                //!< Returns the compiled statement.
  inline sqlite3_stmt *operator()() { return mSTMT; }         //!< Returns the compiled statement.
  inline bool          isValid() { return mSTMT != nullptr; } //!< Returns whether the statement is valid or not.
};

} // namespace internal

//! Reads the modbusSMA register definitions from a sqlite3 database.
class DataBase {
 public:
  //! Information for one supported device.
  struct DevEnum {
    uint32_t    id;    //!< The id of the device
    std::string table; //!< The name of the table where the registers are defined
    std::string name;  //!< The name of the inverter.
  };

 private:
  std::string mPath = MODBUS_DEFAULT_DB;
  sqlite3 *   mDB   = nullptr;

 public:
  DataBase() = default;
  virtual ~DataBase();

  ErrorCode connect(std::string _path = MODBUS_DEFAULT_DB);
  bool      validate();
  void      disconnect();

  std::vector<std::string> getTableList();
  std::vector<DevEnum>     getDeviceEnums();
  std::vector<Register>    getRegisters(std::string _table);

  bool isConnected() const { return mDB != nullptr; } //!< Returns whether the DB is loaded.

  DataBase(DataBase const &) = delete;
  void operator=(DataBase const &) = delete;
};

} // namespace modbusSMA

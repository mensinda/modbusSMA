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

namespace modbusSMA {

//! Reads the modbusSMA register definitions from a sqlite3 database.
class DataBase {
 public:
  //! Stores SQL queries
  struct SQL_Query {
    std::string   sql  = "";      //!< The SQL command to execute.
    sqlite3_stmt *stmt = nullptr; //!< Pointer to the precompiled sqlite3 statement.
  };

  struct DevEnum {
    int         id;
    std::string table;
    std::string name;
  };

  //! List of queries
  enum class Query {
    LIST_TABLES = 0,
    ENUMS,
  };

 private:
  std::string mPath = MODBUS_DEFAULT_DB;
  sqlite3 *   mDB   = nullptr;

  std::vector<SQL_Query> mQueries = {{"SELECT `name` FROM `sqlite_master` WHERE `type`='table';"},
                                     {"SELECT `id`, `table`, `name` FROM `DeviceENUM`;"}};

  SQL_Query *getQuery(Query _q);

 public:
  DataBase() = default;
  virtual ~DataBase();

  ErrorCode connect(std::string _path = MODBUS_DEFAULT_DB);
  bool      validate();
  void      disconnect();

  std::vector<std::string> getTableList();
  std::vector<DevEnum>     getDeviceEnums();

  bool isConnected() const { return mDB != nullptr; } //!< Returns whether the DB is loaded.

  DataBase(DataBase const &) = delete;
  void operator=(DataBase const &) = delete;
};

} // namespace modbusSMA

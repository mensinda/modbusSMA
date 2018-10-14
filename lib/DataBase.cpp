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

#include "DataBase.hpp"

#include <algorithm>
#include <filesystem>
#include <set>

#include "Logging.hpp"

using namespace std;
using namespace modbusSMA;
using namespace modbusSMA::internal;

namespace fs = std::filesystem;

//! Compile the query to a sqlite3 statement
SQL_Query::SQL_Query(sqlite3 *_db, string _query) {
  int errorCode = sqlite3_prepare_v2(_db, _query.c_str(), _query.size() + 1, &mSTMT, nullptr);
  if (errorCode != SQLITE_OK || !mSTMT) {
    auto logger = log::get();
    logger->error("SQL_Query: failed to compile SQL");
    logger->error("  -- SQL:     '{}'", _query);
    logger->error("  -- Error:   '{}'", sqlite3_errstr(errorCode));
    logger->error("  -- Message: '{}'", sqlite3_errmsg(_db));
    mSTMT = nullptr;
  }
}

SQL_Query::~SQL_Query() {
  if (!isValid()) { return; }
  sqlite3_finalize(mSTMT);
}

DataBase::DataBase(std::string _path) : mPath(_path) {} //!< Constructor. Only sets the DB path.
DataBase::~DataBase() { disconnect(); }

//! Connect to the databes.
ErrorCode DataBase::connect() {
  auto     logger    = log::get();
  int      errorCode = SQLITE_OK;
  fs::path filePath(mPath);

  if (!fs::exists(filePath)) {
    logger->error("DataBase::connect() [{}]: DB does not exist", mPath);
    return ErrorCode::FILE_NOT_FOUND;
  }

  if (!fs::is_regular_file(filePath)) {
    logger->error("DataBase::connect() [{}]: DB is not a regular file", mPath);
    return ErrorCode::FILE_NOT_FOUND;
  }

  logger->debug("DataBase::connect() [{}]: Loading register DB", mPath);
  errorCode = sqlite3_open(mPath.c_str(), &mDB);
  if (errorCode != SQLITE_OK) {
    logger->error("DataBase::connect() [{}]: unable to open the database", mPath);
    logger->error("  -- Error: '{}'", sqlite3_errstr(errorCode));
    return ErrorCode::DATA_BASE_ERROR;
  }

  if (!validate()) {
    logger->error("DataBase::connect() [{}]: validation failed", mPath);
    disconnect();
    return ErrorCode::DATA_BASE_ERROR;
  }

  return ErrorCode::OK;
}

//! Closes an open databese connection.
void DataBase::disconnect() {
  if (!mDB) { return; }

  sqlite3_close(mDB);
  mDB = nullptr;
}


//! Validates the register database.
bool DataBase::validate() {
  if (!isConnected()) { return false; }

  auto logger = log::get();
  logger->debug("Validating '{}':", mPath);

  vector<string> tables         = getTableList();
  vector<string> missingTables  = {};
  set<string>    requiredTables = {"ALL"};
  auto           itTableALL     = find(begin(tables), end(tables), "ALL");
  auto           itTableDevEnum = find(begin(tables), end(tables), "DeviceENUM");

  logger->debug("  -- Total number of tables:      {}", tables.size());
  logger->debug("    - Has table 'ALL':            {}", itTableALL != end(tables));
  logger->debug("    - Has table 'DeviceENUM':     {}", itTableDevEnum != end(tables));


  if (tables.empty() || itTableALL == end(tables) || itTableDevEnum == end(tables)) {
    logger->error("Important tables missing");
    return false;
  }

  vector<DevEnum> enums = getDeviceEnums();
  logger->debug("  -- Number of supported devices: {}", enums.size());

  if (enums.empty()) {
    logger->error("No entries in table 'DeviceENUM'");
    return false;
  }

  for (auto &i : enums) {
    if (find(begin(tables), end(tables), i.table) == end(tables)) { missingTables.push_back(i.table); }
    requiredTables.insert(i.table);
  }

  if (!missingTables.empty()) {
    logger->error("Some tables are missing:");
    for (auto i : missingTables) { logger->error("  -- table {}", i); }
    return false;
  }

  missingTables  = {};
  uint32_t count = 0;
  for (auto i : requiredTables) {
    vector<Register> registers = getRegisters(i);

    if (registers.empty()) {
      logger->error("No registers defined in table '{}'", i);
      missingTables.push_back(i);
      continue;
    }

    count += registers.size();
  }

  if (!missingTables.empty()) {
    logger->error("No registers defined in the following tables:");
    for (auto i : missingTables) { logger->error("  -- {}", i); }
    return false;
  }

  logger->debug("  -- Found {} register descriptions in {} tables", count, requiredTables.size());
  return true;
}


/*!
 * \brief Returns a vector of all tables
 *
 * An empty vector is returned on error.
 */
vector<string> DataBase::getTableList() {
  if (!isConnected()) { return {}; }
  vector<string> tableList;
  int            errorCode;

  SQL_Query qTables(mDB, "SELECT `name` FROM `sqlite_master` WHERE `type`='table';");
  if (!qTables.isValid()) { return {}; }

  while (true) {
    errorCode = sqlite3_step(qTables());

    if (errorCode == SQLITE_DONE) { break; }
    if (errorCode != SQLITE_ROW) {
      auto logger = log::get();
      logger->error("DataBase: getTableList(): error in sqlite3_step:");
      logger->error("  -- Path:    '{}'", mPath);
      logger->error("  -- Error:   '{}'", sqlite3_errstr(errorCode));
      logger->error("  -- Message: '{}'", sqlite3_errmsg(mDB));
      return {};
    }

    auto *tableName = sqlite3_column_text(qTables(), 0);
    if (!tableName) {
      log::get()->error("DataBase: getTableList(): sqlite3_column_text returned NULL (DB: {})", mPath);
      return {};
    }

    tableList.emplace_back((const char *)tableName);
  }

  return tableList;
}


/*!
 * \brief Returns a vector of all supported devices
 *
 * An empty vector is returned on error.
 */
vector<DataBase::DevEnum> DataBase::getDeviceEnums() {
  if (!isConnected()) { return {}; }
  vector<DevEnum> enumList;
  int             errorCode;

  SQL_Query query(mDB, "SELECT `id`, `table`, `name` FROM `DeviceENUM`;");
  if (!query.isValid()) { return {}; }

  while (true) {
    errorCode = sqlite3_step(query());

    if (errorCode == SQLITE_DONE) { break; }
    if (errorCode != SQLITE_ROW) {
      auto logger = log::get();
      logger->error("DataBase: getDeviceEnums(): error in sqlite3_step:");
      logger->error("  -- Path:    '{}'", mPath);
      logger->error("  -- Error:   '{}'", sqlite3_errstr(errorCode));
      logger->error("  -- Message: '{}'", sqlite3_errmsg(mDB));
      return {};
    }

    int   id    = sqlite3_column_int(query(), 0);
    auto *table = sqlite3_column_text(query(), 1);
    auto *name  = sqlite3_column_text(query(), 2);
    if (!table || !name) {
      log::get()->error("DataBase: getDeviceEnums(): sqlite3_column_text returned NULL (DB: {})", mPath);
      return {};
    }

    enumList.push_back({(uint32_t)id, (const char *)table, (const char *)name});
  }

  return enumList;
}

/*!
 * \brief Returns a vector of all registers supported by the device
 *
 * An empty vector is returned on error.
 *
 * \param _table The device table from the DevEnum
 */
vector<Register> DataBase::getRegisters(std::string _table) {
  if (!isConnected()) { return {}; }
  vector<Register> regList;
  int              errorCode;
  auto             logger = log::get();

  SQL_Query query(
      mDB,
      fmt::format("SELECT `register`, `desc`, `unit`, `type`, `format`, `access` FROM `{}` ORDER BY `register` ASC;",
                  _table));
  if (!query.isValid()) { return {}; }

  while (true) {
    errorCode = sqlite3_step(query());

    if (errorCode == SQLITE_DONE) { break; }
    if (errorCode != SQLITE_ROW) {
      logger->error("DataBase: getRegisters(_table = '{}'): error in sqlite3_step:", _table);
      logger->error("  -- Path:    '{}'", mPath);
      logger->error("  -- Error:   '{}'", sqlite3_errstr(errorCode));
      logger->error("  -- Message: '{}'", sqlite3_errmsg(mDB));
      return {};
    }

    int   rawID     = sqlite3_column_int(query(), 0);
    auto *rawDesc   = sqlite3_column_text(query(), 1);
    auto *rawUnit   = sqlite3_column_text(query(), 2);
    auto *rawType   = sqlite3_column_text(query(), 3);
    auto *rawFormat = sqlite3_column_text(query(), 4);
    auto *rawAccess = sqlite3_column_text(query(), 5);
    if (!rawDesc || !rawUnit || !rawType || !rawFormat || !rawAccess) {
      logger->error("DataBase: getRegisters(_table = '{}'): sqlite3_column_text returned NULL", _table);
      return {};
    }

    uint16_t   id        = (uint16_t)rawID;
    string     desc      = (const char *)rawDesc;
    string     unit      = (const char *)rawUnit;
    string     strType   = (const char *)rawType;
    string     strFormat = (const char *)rawFormat;
    string     strAccess = (const char *)rawAccess;
    DataType   type      = enum2Str::typeFromStr(strType);
    DataFormat format    = enum2Str::formatFromStr(strFormat);
    DataAccess access    = enum2Str::accessFromStr(strAccess);

    if (type == DataType::__UNKNOWN__) {
      logger->error("DataBase: getRegisters(_table = '{}'): Unkown data type '{}'", _table, strType);
      logger->error("  -- ID:          {}", id);
      logger->error("  -- Description: {}", desc);
      return {};
    }

    if (format == DataFormat::__UNKNOWN__) {
      logger->error("DataBase: getRegisters(_table = '{}'): Unkown data format '{}'", _table, strFormat);
      logger->error("  -- ID:          {}", id);
      logger->error("  -- Description: {}", desc);
      return {};
    }

    if (access == DataAccess::__UNKNOWN__) {
      logger->error("DataBase: getRegisters(_table = '{}'): Unkown access type '{}'", _table, strAccess);
      logger->error("  -- ID:          {}", id);
      logger->error("  -- Description: {}", desc);
      return {};
    }

    regList.emplace_back(id, desc, unit, type, format, access);
  }

  return regList;
}

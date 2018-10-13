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

namespace fs = std::filesystem;

DataBase::~DataBase() { disconnect(); }

/*!
 * \brief Connect to the databes
 * \param _path The full path to the database
 */
ErrorCode DataBase::connect(std::string _path) {
  auto logger    = log::get();
  int  errorCode = SQLITE_OK;
  mPath          = _path;
  fs::path filePath(mPath);

  if (!fs::exists(filePath)) {
    logger->error("DataBase: connect(_path={}): _path does not exist", _path);
    return ErrorCode::FILE_NOT_FOUND;
  }

  if (!fs::is_regular_file(filePath)) {
    logger->error("DataBase: connect(_path={}): _path is not a regular file", _path);
    return ErrorCode::FILE_NOT_FOUND;
  }

  logger->debug("DataBase: connect(_path={}): Loading register DB", _path);
  errorCode = sqlite3_open(_path.c_str(), &mDB);
  if (errorCode != SQLITE_OK) {
    logger->error("DataBase: connect(_path={}): unable to open the database", _path);
    logger->error("  -- Error: '{}'", sqlite3_errstr(errorCode));
    return ErrorCode::DATA_BASE_ERROR;
  }

  logger->debug("DataBase: connect(_path={}): Pre-Building {} SQL queries", _path, mQueries.size());
  for (auto &i : mQueries) {
    errorCode = sqlite3_prepare_v2(mDB, i.sql.c_str(), i.sql.size() + 1, &i.stmt, nullptr);
    if (errorCode != SQLITE_OK || !i.stmt) {
      logger->error("DataBase: connect(_path={}): failed to compile SQL", _path);
      logger->error("  -- SQL:     '{}'", i.sql);
      logger->error("  -- Error:   '{}'", sqlite3_errstr(errorCode));
      logger->error("  -- Message: '{}'", sqlite3_errmsg(mDB));
      disconnect();
      return ErrorCode::DATA_BASE_ERROR;
    }
  }

  if (!validate()) {
    logger->error("DataBase: connect(_path={}): validation failed", _path);
    disconnect();
    return ErrorCode::DATA_BASE_ERROR;
  }

  return ErrorCode::OK;
}

DataBase::SQL_Query *DataBase::getQuery(DataBase::Query _q) {
  size_t index = (size_t)_q;
  if (index >= mQueries.size()) { return nullptr; }

  SQL_Query *q = &mQueries[index];
  if (!q->stmt) { return nullptr; }

  sqlite3_reset(q->stmt);
  return q;
}


//! Validates the register database.
bool DataBase::validate() {
  if (!isConnected()) { return false; }

  auto logger = log::get();
  logger->debug("Validating '{}':", mPath);

  vector<string> tables         = getTableList();
  vector<string> missingTables  = {};
  set<string>    requiredTables = {};
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

  SQL_Query *qTables = getQuery(Query::LIST_TABLES);
  assert(qTables != nullptr); // Should always pass when connected

  while (true) {
    errorCode = sqlite3_step(qTables->stmt);

    if (errorCode == SQLITE_DONE) { break; }
    if (errorCode != SQLITE_ROW) {
      auto logger = log::get();
      logger->error("DataBase: getTableList(): error in sqlite3_step:");
      logger->error("  -- Path:    '{}'", mPath);
      logger->error("  -- Error:   '{}'", sqlite3_errstr(errorCode));
      logger->error("  -- Message: '{}'", sqlite3_errmsg(mDB));
      return {};
    }

    auto *tableName = sqlite3_column_text(qTables->stmt, 0);
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

  SQL_Query *query = getQuery(Query::ENUMS);
  assert(query != nullptr); // Should always pass when connected

  while (true) {
    errorCode = sqlite3_step(query->stmt);

    if (errorCode == SQLITE_DONE) { break; }
    if (errorCode != SQLITE_ROW) {
      auto logger = log::get();
      logger->error("DataBase: getDeviceEnums(): error in sqlite3_step:");
      logger->error("  -- Path:    '{}'", mPath);
      logger->error("  -- Error:   '{}'", sqlite3_errstr(errorCode));
      logger->error("  -- Message: '{}'", sqlite3_errmsg(mDB));
      return {};
    }

    int   id    = sqlite3_column_int(query->stmt, 0);
    auto *table = sqlite3_column_text(query->stmt, 1);
    auto *name  = sqlite3_column_text(query->stmt, 2);
    if (!table || !name) {
      log::get()->error("DataBase: getDeviceEnums(): sqlite3_column_text returned NULL (DB: {})", mPath);
      return {};
    }

    enumList.push_back({id, (const char *)table, (const char *)name});
  }

  return enumList;
}


//! Closes an open databese connection.
void DataBase::disconnect() {
  for (auto &i : mQueries) {
    if (!i.stmt) { continue; }

    sqlite3_finalize(i.stmt);
    i.stmt = nullptr;
  }

  if (!mDB) { return; }

  sqlite3_close(mDB);
  mDB = nullptr;
}

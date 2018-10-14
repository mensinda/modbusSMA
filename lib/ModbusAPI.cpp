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

#include "mSMAConfig.hpp"

#include "ModbusAPI.hpp"

#include <algorithm>

#include "Logging.hpp"
#include "MBConnectionIP.hpp"
#include "MBConnectionIP_PI.hpp"
#include "MBConnectionRTU.hpp"

using namespace std;
using namespace modbusSMA;

/*!
 * \brief Initializes the ModbusAPI with a custom TCP IP and port
 * \sa setConnectionTCP_IP
 */
ModbusAPI::ModbusAPI(string                    _ip,   //!< The IP address of the inverter.
                     uint32_t                  _port, //!< The modbus TCP port.
                     std::shared_ptr<DataBase> _db    //!< Database describing the modbus registers.
) {
  setConnectionTCP_IP(_ip, _port);
  setDataBase(_db);
}

/*!
 * \brief Initializes the ModbusAPI with a custom IP and port
 * \sa setConnectionTCP_IP
 */
ModbusAPI::ModbusAPI(string                    _node, //!< The Modbus node (IP address, DNS name, etc.) of the inverter.
                     string                    _service, //!< The service to connect to.
                     std::shared_ptr<DataBase> _db       //!< Database describing the modbus registers.

) {
  setConnectionTCP_IP_PI(_node, _service);
  setDataBase(_db);
}

/*!
 * \brief Initializes the ModbusAPI with a RTU connectopm
 * \sa setConnectionRTU
 */
ModbusAPI::ModbusAPI(string   _device,             //!< The name / path of the serial port.
                     uint32_t _baud,               //!< RTU connection baud rate.
                     char     _parity,             //!< Partity type: N = None; E = Even; O = Odd.
                     int      _dataBit,            //!< The number of bits of data, the allowed values are 5, 6, 7 and 8
                     int      _stopBit,            //!< The bits of stop, the allowed values are 1 and 2.
                     std::shared_ptr<DataBase> _db //!< Database describing the modbus registers.
) {
  setConnectionRTU(_device, _baud, _parity, _dataBit, _stopBit);
  setDataBase(_db);
}

//! Wrapper for the other constructor.
ModbusAPI::ModbusAPI(string _ip, uint32_t _port, string _dbPath)
    : ModbusAPI(_ip, _port, make_shared<DataBase>(_dbPath)) {}

//! Wrapper for the other constructor.
ModbusAPI::ModbusAPI(string _node, string _service, string _dbPath)
    : ModbusAPI(_node, _service, make_shared<DataBase>(_dbPath)) {}

//! Wrapper for the other constructor.
ModbusAPI::ModbusAPI(string _device, uint32_t _baud, char _parity, int _dataBit, int _stopBit, string _dbPath)
    : ModbusAPI(_device, _baud, _parity, _dataBit, _stopBit, make_shared<DataBase>(_dbPath)) {}


ModbusAPI::~ModbusAPI() { reset(); }


/*!
 * \brief Resets the state of the object (modbus connection, inverter type, etc.) to CONFIGURE
 *
 * The configuration (IP, port) is NOT reset.
 *
 * State change: * --> CONFIGURE
 */
void ModbusAPI::reset() {
  if (mConn) {
    mConn->disconnect();
    mConn = nullptr;
  }

  mRegisters = nullptr;
  mState     = State::CONFIGURE;
}

/*!
 * \brief Establishes the modbus connection based on the current configuration
 *
 * State change: CONFIGURE --> CONNECTED | ERROR
 */
ErrorCode ModbusAPI::connect() {
  auto logger = log::get();
  if (mState != State::CONFIGURE) {
    logger->error("ModbusAPI: can not connect() -- invalid object state '{}'", enum2Str::toStr(mState));
    mState = State::ERROR;
    return ErrorCode::INVALID_STATE;
  }

  auto res = mConn->connect();
  if (res != ErrorCode::OK) {
    logger->error("ModbusAPI: unable to connect: '{}'", enum2Str::toStr(res));
    mState = State::ERROR;
    return res;
  }

  logger->info("ModbusAPI: connected to {}", mConn->description());
  mState = State::CONNECTED;
  return ErrorCode::OK;
}

/*!
 * \brief Initializes the API
 *
 * State change: CONNECTED --> INITIALIZED | ERROR
 */
ErrorCode ModbusAPI::initialize() {
  auto      logger = log::get();
  ErrorCode result;

  if (mState != State::CONNECTED) {
    logger->error("ModbusAPI: can not initialize() -- invalid object state '{}'", enum2Str::toStr(mState));
    mState = State::ERROR;
    return ErrorCode::INVALID_STATE;
  }

  // 1st: check database
  if (!mDB->isConnected()) {
    result = mDB->connect();
    if (result != ErrorCode::OK) {
      logger->error("ModbusAPI: DataBase initialization failed with {}", enum2Str::toStr(result));
      mState = State::ERROR;
      return result;
    }
  }

  mRegisters = make_shared<RegisterContainer>();
  mRegisters->addRegisters(mDB->getRegisters("ALL"));
  if (mRegisters->empty()) {
    // This code should never be executed because of the DataBase validation
    logger->error("ModbusAPI: No Registers in table 'ALL'");
    mState = State::ERROR;
    return ErrorCode::DATA_BASE_ERROR;
  }

  // 2nd: set slave ID to 1
  result = mConn->setSlaveID(1);

  if (result != ErrorCode::OK) {
    mState = State::ERROR;
    return result;
  }

  // 3rd: request data
  vector<uint16_t> rawData = mConn->readRegisters(42109, 4);

  logger->debug("Requesting basic inverter information:");

  if (rawData.size() != 4) {
    logger->error("ModbusAPI: Failed to initialize -- requesting slave/unit ID failed");
    mState = State::ERROR;
    return ErrorCode::INITIALIZATION_FAILED;
  }

  uint32_t serialNumber = (rawData[0] << 16) + rawData[1];
  uint16_t susyID       = rawData[2];
  uint16_t unitID       = rawData[3];

  logger->debug("  -- Physical serial number: {}", serialNumber);
  logger->debug("  -- Physical SusyID:        {}", susyID);
  logger->debug("  -- Unit / Slave ID:        {}", unitID);

  // 4th: set slave ID to unitID
  result = mConn->setSlaveID(unitID);

  if (result != ErrorCode::OK) {
    mState = State::ERROR;
    return result;
  }

  // 5th: determine the inverter type
  rawData             = mConn->readRegisters(30053, 2);
  uint32_t inverterID = (rawData[0] << 16) + rawData[1];
  logger->debug("  -- Inverter type ID:       {}", inverterID);

  auto devices = mDB->getDeviceEnums();
  bool found   = false;
  for (auto i : devices) {
    if (i.id == inverterID) {
      found           = true;
      mInverterType   = i.name;
      mInverterTypeID = inverterID;
      mRegisters->addRegisters(mDB->getRegisters(i.table));
      break;
    }
  }

  if (!found) {
    logger->error("ModbusAPI: unknown inverter type {}", inverterID);
    mState = State::ERROR;
    return ErrorCode::INITIALIZATION_FAILED;
  }

  logger->debug("  -- Inverter type:          '{}'", mInverterType);
  logger->debug("  -- Number of registers:    {}", mRegisters->size());
  logger->info("ModbusAPI: initialization for {} complete", mInverterType);
  mState = State::INITIALIZED;
  return ErrorCode::OK;
}

/*!
 * \brief Wrapper for connect() and initialize()
 *
 * State change: CONFIGURE --> INITIALIZED | ERROR
 */
ErrorCode ModbusAPI::setup() {
  ErrorCode res = connect();
  if (res != ErrorCode::OK) { return res; }

  return initialize();
}



//! Internal Request helper
//! \internal
struct RegBatch {
  //! Internal Request helper
  //! \internal
  struct Reg {
    uint16_t reg;    //!< 16-bit register address.
    uint16_t offset; //!< offset in the rawData vector.
  };

  uint16_t         size;    //!< Size of the batch in number of registers.
  vector<Reg>      regs;    //!< Registers in the batch.
  vector<uint16_t> rawData; //!< Raw data vector.
};

/*!
 * \brief Updates all registers stored in _regList
 *
 * Unsupported registers in _regList are ignored.
 *
 * \note This function can only be called in the CONFIGURE state
 *
 * State change: NONE
 *
 * \param[in]  _regList    List of registers to update
 * \param[out] _numUpdated Number of updated registers
 */
ErrorCode ModbusAPI::updateRegisters(vector<Register> _regList, size_t *_numUpdated) {
  auto logger = log::get();
  if (_numUpdated) { *_numUpdated = 0; }
  if (mState != State::INITIALIZED) {
    logger->error("ModbusAPI: updateRegisters() -- invalid object state '{}'", enum2Str::toStr(mState));
    return ErrorCode::INVALID_STATE;
  }
  if (_regList.empty()) {
    logger->warn("ModbusAPI: updateRegisters() -- empty register list ==> do nothing");
    return ErrorCode::OK;
  }

  sort(begin(_regList), end(_regList)); // Ensure that the list is sorted.

  // 1st: Create batches of registers.
  std::vector<RegBatch> batches;
  batches.reserve(_regList.size()); // Worst case: every register has its own batch
  batches.push_back({0, {}, {}});

  for (Register i : _regList) {
    RegBatch *curr = &batches.back();

    if (!curr->regs.empty()) {
      if (((curr->size + i.size()) >= SMA_MODBUS_MAX_REGISTER_COUNT) || // Check if maximum request size is reached
          ((curr->regs[0].reg + curr->size) != i.reg())) {              // Check if continous
        batches.push_back({0, {}, {}});
        curr = &batches.back();
      }
    }

    curr->regs.push_back({i.reg(), curr->size});
    curr->size += i.size();
  }

  //! \todo finish this

  return ErrorCode::OK;
}




/*!
 * \brief Sets the modbus register database to use
 *
 * \note This function can only be called in the CONFIGURE state
 *
 * State change: NONE
 *
 * \param _db   shared pointer to the DataBase object
 * \returns OK or INVALID_STATE
 */
ErrorCode ModbusAPI::setDataBase(std::shared_ptr<DataBase> _db) {
  if (mState != State::CONFIGURE) {
    log::get()->error("ModbusAPI: setDataBase() -- invalid object state '{}'", enum2Str::toStr(mState));
    return ErrorCode::INVALID_STATE;
  }

  if (!_db) { _db = make_shared<DataBase>(SMA_MODBUS_DEFAULT_DB); }

  mDB = _db;
  return ErrorCode::OK;
}

/*!
 * \brief Sets the modbus register database to use
 *
 * \note This function can only be called in the CONFIGURE state
 *
 * State change: NONE
 *
 * \param _dbPath Path to the database file
 * \returns OK or INVALID_STATE
 */
ErrorCode ModbusAPI::setDataBase(string _dbPath) {
  if (mState != State::CONFIGURE) {
    log::get()->error("ModbusAPI: setDataBase() -- invalid object state '{}'", enum2Str::toStr(mState));
    return ErrorCode::INVALID_STATE;
  }

  mDB = make_shared<DataBase>(_dbPath);
  return ErrorCode::OK;
}




/*!
 * \brief Sets the TCP server connection details
 *
 * \note This function can only be called in the CONFIGURE state
 *
 * State change: NONE
 *
 * \param _ip   The IP address to use
 * \param _port The port to use
 *
 * \returns OK or INVALID_STATE
 */
ErrorCode ModbusAPI::setConnectionTCP_IP(string _ip, uint32_t _port) {
  if (mState != State::CONFIGURE) {
    log::get()->error("ModbusAPI: setConnectionTCP_IP() -- invalid object state '{}'", enum2Str::toStr(mState));
    return ErrorCode::INVALID_STATE;
  }

  mConn = make_unique<MBConnectionIP>(_ip, _port);
  return ErrorCode::OK;
}

/*!
 * \brief Sets the TCP IP protocol indemendant server connection details
 *
 * \note This function can only be called in the CONFIGURE state
 *
 * State change: NONE
 *
 * \param _node    The server node
 * \param _service The service
 *
 * \returns OK or INVALID_STATE
 */
ErrorCode ModbusAPI::setConnectionTCP_IP_PI(string _node, string _service) {
  if (mState != State::CONFIGURE) {
    log::get()->error("ModbusAPI: setConnectionTCP_IP_PI() -- invalid object state '{}'", enum2Str::toStr(mState));
    return ErrorCode::INVALID_STATE;
  }

  mConn = make_unique<MBConnectionIP_PI>(_node, _service);
  return ErrorCode::OK;
}

/*!
 * \brief Sets the TCP server connection details
 *
 * \note This function can only be called in the CONFIGURE state
 *
 * State change: NONE
 *
 * \param _device   The name of the serial port
 * \param _baud     The baud rate of the communication
 * \param _parity   Partity type: N = None; E = Even; O = Odd
 * \param _dataBit  The number of bits of data, the allowed values are 5, 6, 7 and 8
 * \param _stopBit  The bits of stop, the allowed values are 1 and 2
 *
 * \returns OK or INVALID_STATE
 */
ErrorCode ModbusAPI::setConnectionRTU(string _device, uint32_t _baud, char _parity, int _dataBit, int _stopBit) {
  if (mState != State::CONFIGURE) {
    log::get()->error("ModbusAPI: setConnectionRTU() -- invalid object state '{}'", enum2Str::toStr(mState));
    return ErrorCode::INVALID_STATE;
  }

  mConn = make_unique<MBConnectionRTU>(_device, _baud, _parity, _dataBit, _stopBit);
  return ErrorCode::OK;
}

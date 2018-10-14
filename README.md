# modbusSMA

C++ library for accessing the the modbus interface of SMA inverters.

See https://mensinda.github.io/modbusSMA/ for documentation.

# Features

 - Easy to use
 - Connect to a SMA inverter via TCP and RTU
 - Uses a sqlite3 database for the modbus register information of the inverters
 - Automatically convertes the raw modbus registers to usable formats

# Install

## Dependencies

 - A C++17 compiler
 - sqlite3 (https://www.sqlite.org/index.html)
 - libmodbus (https://libmodbus.org/)

Furthermore meson (https://mesonbuild.com/index.html) and ninja are required to build the library.
Both can be installed with `pip install ninja meson`.

## Build

```bash
git clone https://github.com/mensinda/modbusSMA
git submodule update --init --recursive
cd modbusSMA
meson build
ninja -C build
ninja -C build install # Only to install the library
```

# Code Sample

```cpp
#include <modbusSMA/ModbusAPI.hpp>
#include <vector>

using namespace modbusSMA;
using namespace std;

int main() {
  ModbusAPI mapi("127.0.0.1", 502); // Create the main API object

  // Call further configuration functions here.

  ErrorCode err = mapi.setup();        // Connect to the modbus interface with ModbusAPI::setup()
  auto      reg = mapi.getRegisters(); // Get a pointer to the RegisterContainer where all registers are stored.

  // Use the shared RegisterContainer pointer (reg) to get a list of supported registers.
  vector<uint16_t> toFetch = {30051, 30053, 30529, 30535, 30538};

  err = mapi.updateRegisters(toFetch); // Fetch the values of the registers and store them in the RegisterContainer

  // The fetched register values can now be retrieved from the shared RegisterContainer
  vector<Register> registersWithValues = reg->getRegisters(toFetch);
  return 0;
}
```

modbusSMASrc = files([
  'Enums.cpp',
  'Logging.cpp',
  'MBConnectionBase.cpp',
  'MBConnectionIP.cpp',
  'MBConnectionIP_PI.cpp',
  'MBConnectionRTU.cpp',
  'ModbusAPI.cpp'
])

modbusSMALib = library(
  'modbusSMA', modbusSMASrc,
  include_directories: includeDirs,
  link_with:           [],
  dependencies:        projectDeps,
  install:             true,
)

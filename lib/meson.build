modbusSMASrc = files([
  'Enums.cpp',
  'Logging.cpp',
  'ModbusAPI.cpp'
])

modbusSMALib = library(
  'modbusSMA', modbusSMASrc,
  include_directories: includeDirs,
  link_with:           [],
  dependencies:        projectDeps,
  install:             true,
)

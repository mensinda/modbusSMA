modbusSMASrc = [
  'Enums.cpp',
  'DataBase.cpp',
  'Logging.cpp',
  'MBConnectionBase.cpp',
  'MBConnectionIP.cpp',
  'MBConnectionIP_PI.cpp',
  'MBConnectionRTU.cpp',
  'ModbusAPI.cpp',
  'Register.cpp',
]

modbusSMAInc = []

foreach src : modbusSMASrc
  modbusSMAInc += src.split('.')[0] + '.hpp'
endforeach

modbusSMALib = library(
  'modbusSMA', modbusSMASrc,
  include_directories: includeDirs,
  link_with:           [],
  dependencies:        projectDeps,
  install:             true,
)

install_headers(modbusSMAInc, subdir: meson.project_name())

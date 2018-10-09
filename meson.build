project(
  'modbusSMA', ['cpp'],
  default_options: [
    'cpp_std=c++17',
    'warning_level=3',
    'buildtype=debugoptimized',
    'libdir=lib/modbusSMA',
    'includedir=include/modbusSMA',
    'b_colorout=always',
    'b_ndebug=if-release'
  ],
  version: '0.0.1',
  license: 'Apache 2'
)

includeDirsArray = ['.', 'lib', 'external', 'external/spdlog/include']
includeDirs      = include_directories(includeDirsArray)
compiler         = meson.get_compiler('cpp')

modbusDep   = dependency('libmodbus', required: true, version: '>=3.0')
sqlite3Dep  = dependency('sqlite3',   required: true)
threadsDep  = dependency('threads',   required: true)

# FMT library
fmtLib   = compiler.find_library('fmt', required: true)

if not compiler.has_header('fmt/format.h')
  error('Can not find fmt header files')
endif

projectDeps = [modbusDep, sqlite3Dep, threadsDep, fmtLib]

cfgData = configuration_data()
cfgData.set_quoted('MODBUS_SMA_VERSION',     meson.project_version())
cfgData.set_quoted('MODBUS_SMA_LOGGER_NAME', meson.project_name())

configure_file(
  configuration: cfgData,
  input:         'mSMAConfig.in.hpp',
  output:        'mSMAConfig.hpp'
)

subdir('lib')
subdir('src/cmd')

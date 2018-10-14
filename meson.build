project(
  'modbusSMA', ['cpp'],
  default_options: [
    'cpp_std=c++17',
    'warning_level=3',
    'buildtype=debugoptimized',
    'b_colorout=always',
    'b_ndebug=if-release'
  ],
  version: '1.0.0',
  license: 'Apache 2'
)

includeDirsArray = ['.', 'lib', 'external', 'external/spdlog/include']
includeDirs      = include_directories(includeDirsArray)
compiler         = meson.get_compiler('cpp')

modbusDep   = dependency('libmodbus', required: true, version: '>=3.0')
sqlite3Dep  = dependency('sqlite3',   required: true)
threadsDep  = dependency('threads',   required: true)
fsLib       = compiler.find_library('stdc++fs', required: true)

projectDeps = [modbusDep, sqlite3Dep, threadsDep, fsLib]

if get_option('use_external_fmt')
  fmtLib       = compiler.find_library('fmt',      required: true)
  projectDeps += [fmtLib]

  if not compiler.has_header('fmt/format.h')
    error('Can not find fmt header files')
  endif
endif


###############################
# Begin Compiler test section #
###############################

if not compiler.has_header('filesystem')
  error('Can not find std::filesystem header files')
endif

fsTestCode = '''#include <filesystem>
std::filesystem::path getPathTest() {return std::filesystem::current_path();}
int main() {return 0;}'''

if not compiler.links(fsTestCode, dependencies: projectDeps , name: 'std::filesystem check')
  error('Simple std::filesystem test program fails to compile')
endif

#############################
# Configuration header file #
#############################

cfgData = configuration_data()
cfgData.set_quoted('SMA_MODBUS_SMA_VERSION',        meson.project_version())
cfgData.set_quoted('SMA_MODBUS_SMA_LOGGER_NAME',    meson.project_name())
cfgData.set(       'SMA_MODBUS_MAX_REGISTER_COUNT', get_option('max_register_count'))
cfgData.set_quoted('SMA_MODBUS_INSTALL_PREFIX',     get_option('prefix'))
cfgData.set_quoted('SMA_MODBUS_INSTALL_DATA_DIR',   join_paths([cfgData.get_unquoted('SMA_MODBUS_INSTALL_PREFIX'),   get_option('datadir'), meson.project_name()]))
cfgData.set_quoted('SMA_MODBUS_DEFAULT_DB',         join_paths([cfgData.get_unquoted('SMA_MODBUS_INSTALL_DATA_DIR'), 'SMA_Modbus.db']))
cfgData.set10(     'SMA_MODBUS_USE_EXTERNAL_FMT',   get_option('use_external_fmt'))

cfgHead = configure_file(
  configuration: cfgData,
  input:         'mSMAConfig.in.hpp',
  output:        'mSMAConfig.hpp'
)

install_data('data/SMA_Modbus.db')
install_headers(cfgHead, subdir: meson.project_name())

subdir('lib')
subdir('src/cmd')

##############
# PKG-Config #
##############

pkg = import('pkgconfig')
pkg.generate(
  description: 'C++ library for accessing the the modbus interface of SMA inverters',
  name:        meson.project_name(),
  version:     meson.project_version(),
  url:         'https://github.com/mensinda/modbusSMA',
  requires:    ['libmodbus', 'sqlite3'],
  libraries:   [modbusSMALib],
  subdirs:     [meson.project_name()]
)

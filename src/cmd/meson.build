cmdSrc = files([
  'main.cpp'
])

executable(
  'modbusCMD', cmdSrc,
  include_directories: includeDirs,
  link_with:           [modbusSMALib],
  dependencies:        projectDeps,
  install:             true,
  install_rpath:       join_paths([get_option('prefix'), get_option('libdir')]),
)

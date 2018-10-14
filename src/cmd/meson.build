cmdSrc = files([
  'main.cpp'
])

executable(
  'modbusCMD', cmdSrc,
  include_directories: includeDirs,
  link_with:           [modbusSMALib],
  dependencies:        projectDeps,
  install:             true,
)

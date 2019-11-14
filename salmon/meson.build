compiler_deps = []

lib_compiler = static_library(
  'salmon_compiler',
  files(
    'compiler/box.cpp',
    'compiler/CountingStream.cpp',
    'compiler/compiler.cpp',
    'compiler/config.cpp',
    'compiler/package.cpp',
    'compiler/parser.cpp',
    'compiler/symbol.cpp',
    'compiler/type.cpp',
    'compiler/vm/allocateditem.cpp',
    'compiler/vm/memory.cpp',
  ),
  include_directories: salmon_inc,
  dependencies: compiler_deps,
)


salmon_sources = files(
  'main.cpp',
  'util/environment.cpp',
  )

salmon_exe_deps = [
  linenoise,
]

executable(
  'salmon',
  salmon_sources,
  include_directories: [salmon_inc],
  dependencies: salmon_exe_deps,
  link_with: lib_compiler,
  install: true
)

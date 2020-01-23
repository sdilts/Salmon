compiler_deps = []

lib_compiler = static_library(
  'salmon_compiler',
  files(
    'compiler/CountingStream.cpp',
    'compiler/vm/box.cpp',
    'compiler/compiler.cpp',
    'config.cpp',
    'compiler/parser.cpp',
    'compiler/vm/allocateditem.cpp',
    'compiler/vm/array.cpp',
    'compiler/vm/list.cpp',
    'compiler/vm/memory.cpp',
    'compiler/vm/package.cpp',
    'compiler/vm/string.cpp',
    'compiler/vm/symbol.cpp',
    'compiler/vm/type.cpp',
    'compiler/vm/vm.cpp',
  ),
  include_directories: salmon_inc,
  dependencies: compiler_deps,
)


salmon_sources = files(
  'main.cpp',
  'util/environment.cpp',
  )

salmon_exe_deps = [
  replxx_dep,
]

executable(
  'salmon',
  salmon_sources,
  include_directories: [salmon_inc],
  dependencies: salmon_exe_deps,
  link_with: lib_compiler,
  install: true
)

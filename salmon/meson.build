compiler_deps = []

lib_compiler = static_library(
  'salmon_compiler',
  files(
    'util/assert.cpp',

    'compiler/CountingStream.cpp',
    'compiler/compiler.cpp',
    'compiler/parser.cpp',
    'config.cpp',
    'vm/allocateditem.cpp',
    'vm/array.cpp',
    'vm/box.cpp',
    'vm/function.cpp',
    'vm/list.cpp',
    'vm/memory.cpp',
    'vm/package.cpp',
    'vm/string.cpp',
    'vm/symbol.cpp',
    'vm/type.cpp',
    'vm/vm.cpp',
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

compiler_deps = []

lib_compiler = static_library(
  'salmon_compiler',
  files(
    'util/assert.cpp',
    'compiler/CountingStream.cpp',
    'compiler/compiler.cpp',
    'compiler/parser.cpp',
    'vm/allocateditem.cpp',
    'vm/array.cpp',
    'vm/box.cpp',
    'vm/function.cpp',
    'vm/functionexception.cpp',
    'vm/list.cpp',
    'vm/memory.cpp',
    'vm/package.cpp',
    'vm/typespec.cpp',
    'vm/string.cpp',
    'vm/symbol.cpp',
    'vm/type.cpp',
    'vm/vm.cpp',
  ),
  include_directories: salmon_inc,
  dependencies: compiler_deps,
  cpp_pch: 'pch/pch_cpp.hpp',
)


salmon_sources = files(
  'main.cpp',
  'config.cpp',
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

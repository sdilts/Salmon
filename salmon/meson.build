salmon_sources = files(
  'main.cpp',

  'compiler/config.cpp',
  'compiler/CountingStream.cpp',
  'compiler/parser.cpp',
  'compiler/vm/memory.cpp',

  'util/environment.cpp',
  )

salmon_deps = [
  linenoise,
]

executable(
  'salmon',
  salmon_sources,
  include_directories: [salmon_inc],
  dependencies: salmon_deps,
  install: true
)

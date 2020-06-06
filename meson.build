project('salmon',
	['cpp', 'c'],
	version : '0.0.1',
	license : 'MIT',
	meson_version: '>=0.51.0',
	default_options : [
	  'cpp_std=c++17',
	  'c_std=c17',
	  'werror=true',
	  'warning_level=3',
	  'buildtype=debugoptimized',
	  'b_ndebug=if-release',
	])

cmake = import('cmake')

common_args = [
  '-Wimplicit-fallthrough',
]

add_project_arguments(common_args, language: 'cpp',)
add_project_arguments(common_args, language: 'c',)

replxx_options = [
  '-DREPLXX_BUILD_EXAMPLES=no',
  '-DCMAKE_BUILD_TYPE=Release',
  '-DREPLXX_STATIC=yes'
]
replxx_proj = cmake.subproject('replxx', cmake_options: replxx_options)
replxx_dep = replxx_proj.dependency('replxx')

salmon_inc = include_directories('include')

# subdir('include')
subdir('salmon')

if get_option('test')
  subdir('test')
endif

doxygen = find_program('doxygen', required : false)

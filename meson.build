project('salmon',
	['cpp', 'c'],
	version : '0.0.1',
	license : 'MIT',
	meson_version: '>=0.55.0',
	default_options : [
	  'cpp_std=c++2a',
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

replxx_var = cmake.subproject_options()
replxx_var.add_cmake_defines({'REPLXX_BUILD_EXAMPLES' : 0})
replxx_var.add_cmake_defines({'CMAKE_BUILD_TYPE' : 'Release'})
replxx_var.add_cmake_defines({'REPLXX_BUILD_STATIC' : 1})
replxx_proj = cmake.subproject('replxx', options: replxx_var)
replxx_dep = replxx_proj.dependency('replxx')

salmon_inc = include_directories('include')

# subdir('include')
subdir('salmon')

if get_option('test')
  subdir('test')
endif

doxygen = find_program('doxygen', required : false)

project('salmon',
	['cpp', 'c'],
	version : '0.0.1',
	license : 'MIT',
	meson_version: '>=0.51.0',
	default_options : [
	  'cpp_std=c++17',
	  'c_std=c17',
	  'werror=true',
	  'warning_level=2',
	])

cmake = import('cmake')

common_args = [
  '-Wall',
  '-Wimplicit-fallthrough',
]

add_project_arguments(common_args, language: 'cpp',)
add_project_arguments(common_args, language: 'c',)


replxx_proj = cmake.subproject('replxx', cmake_options: '-DREPLXX_BUILD_EXAMPLES=no -DCMAKE_BUILD_TYPE=Release')
replxx_dep = replxx_proj.get_variable('replxx_dep')

salmon_inc = include_directories('include')

# subdir('include')
subdir('salmon')
subdir('test')

doxygen = find_program('doxygen', required : false)

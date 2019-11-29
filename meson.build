project('salmon',
	['cpp', 'c'],
	version : '0.0.1',
	license : 'MIT',
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


replxx_proj = cmake.subproject('replxx', cmake_options: '-DREPLXX_BUILD_EXAMPLES=no')
replxx_dep = replxx_proj.get_variable('replxx_dep')

linenoise = dependency('linenoise')

salmon_inc = include_directories('include')

# subdir('include')
subdir('salmon')
subdir('test')

doxygen = find_program('doxygen', required : false)

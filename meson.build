project('salmon',
	['cpp', 'c'],
	version : '0.0.1',
	license : 'MIT',
	default_options : [
	  'cpp_std=c++17',
	  'c_std=c17',
	  'werror=true',
	  'warning_level=2',])

common_args = [
  '-Wall',
  '-Wimplicit-fallthrough',
]

add_project_arguments(common_args, language: 'cpp',)
add_project_arguments(common_args, language: 'c',)


linenoise = dependency('linenoise')
salmon_inc = include_directories('include')

# subdir('include')
subdir('salmon')


doxygen = find_program('doxygen', required : false)

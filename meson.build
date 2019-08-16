project('salmon',
	['cpp', 'c'],
	version : '0.0.1',
	license : 'MIT',
	default_options : [
	  'werror=true',
	  'warning_level=2',])

common_args = [
  '-Wall',
  '-Wimplicit-fallthrough',
]

cpp_args = [ '--std=c++17' ]
cpp_args += common_args

c_args = [ '--std=c11' ]
c_args += common_args

add_project_arguments(cpp_args, language: 'cpp',)
add_project_arguments(c_args, language: 'c',)


linenoise = dependency('linenoise')
salmon_inc = include_directories('include')

# subdir('include')
subdir('salmon')


doxygen = find_program('doxygen', required : false)

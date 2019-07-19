project('salmon', 'cpp',
	version : '0.0.1',
	license : 'MIT',
	default_options : ['cpp_std=c++17',
			   'werror=true',
			   'warning_level=2',])

add_project_arguments(
  [
    '-Wall',
  ],
  language: 'cpp',)

linenoise = dependency('linenoise')


salmon_inc = include_directories('include')

# subdir('include')
subdir('salmon')


doxygen = find_program('doxygen', required : false)

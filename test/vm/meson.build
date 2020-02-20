tests = {
	  'box_tests' : 'box_tests.cpp',
	  'package_tests' : 'package_test.cpp',
	  'symbol_tests' : 'symbol_test.cpp',
	  'vm_ptr_tests' : 'vm_ptr_tests.cpp',
	  'function_tests' : 'builtin_function_test.cpp',
	}

foreach name, file : tests
  e = executable(name, file,
		 include_directories: [salmon_inc],
		link_with: [lib_compiler, lib_catch] )
  test(name, e, suite: 'vm')
endforeach

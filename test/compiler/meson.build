tests = {
	  # 'parser_tests' : 'compiler/parser_test.cpp',
	  'box_tests' : 'vm/box_tests.cpp',
	  'package_tests' : 'vm/package_test.cpp',
	  'symbol_tests' : 'vm/symbol_test.cpp',
	  'vm_ptr_tests' : 'vm/vm_ptr_tests.cpp',
	  'function_tests' : 'vm/builtin_function_test.cpp',
	}

foreach name, file : tests
  e = executable(name, file,
		 include_directories: [salmon_inc],
		link_with: [lib_compiler, lib_catch] )
  test(name, e)
endforeach

tests = {
	  # 'parser_tests' : 'compiler/parser_test.cpp',
	  'package_tests' : 'package_test.cpp',
	  'symbol_tests' : 'symbol_test.cpp',
	  'vm_ptr_tests' : 'vm/vm_ptr_tests.cpp',
	}

foreach name, file : tests
  e = executable(name, file,
		 include_directories: [salmon_inc],
		link_with: [lib_compiler, lib_catch] )
  test(name, e)
endforeach

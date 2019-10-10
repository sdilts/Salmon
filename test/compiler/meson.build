tests = {
	  # 'parser_tests' : 'compiler/parser_test.cpp',
	  'symbol_tests' : 'symbol_test.cpp',
	}

foreach name, file : tests
  e = executable(name, file,
		 include_directories: [salmon_inc],
		link_with: lib_compiler)
  test(name, e)
endforeach

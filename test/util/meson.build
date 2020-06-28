tests = {
	  'prefixtrie_tests' : 'prefixtrie_test.cpp'
	}

foreach name, file : tests
  e = executable(name, file,
		 include_directories: [salmon_inc],
		 link_with: [lib_catch] )
  test(name, e, suite: 'util')
endforeach

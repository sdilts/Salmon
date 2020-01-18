#include <iostream>

#include <compiler/vm/string.hpp>

namespace salmon::vm {

	StaticString::StaticString(const std::string &str) :
		contents{str} { }

	void StaticString::print_debug_info() const {
		std::cerr << "string: \"" << contents << "\"" << std::endl;
	}

}

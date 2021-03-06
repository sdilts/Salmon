#include <iostream>

#include <vm/string.hpp>

namespace salmon::vm {

	StaticString::StaticString(const std::string &str) :
		contents{str} { }

	StaticString::StaticString(std::string &&str) :
		contents(str) { }

	void StaticString::print_debug_info() const {
		std::cerr << "string: \"" << contents << "\"" << std::endl;
	}

	size_t StaticString::allocated_size() const {
		return sizeof(StaticString);
	}

	std::ostream& operator<<(std::ostream &os, const StaticString &string) {
		return os << '"' << string.contents << '"';
	}
}

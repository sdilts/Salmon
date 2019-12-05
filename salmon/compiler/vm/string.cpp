#include <compiler/vm/string.hpp>

namespace salmon::vm {

	StaticString::StaticString(const std::string &str) :
		contents{str} { }

}

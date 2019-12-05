#include <compiler/string.hpp>

namespace salmon::compiler {

	StaticString::StaticString(const std::string &str) :
		contents{str} { }

}

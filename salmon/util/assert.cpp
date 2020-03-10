#include <sstream>

#include <util/assert.hpp>

namespace salmon {

	AssertionException::AssertionException(const std::string &msg,
										   const char *file,
										   unsigned int line) :
		std::logic_error(msg),
		file(file),
		line{line} { }

	void throw_assert(const char* predicate, const char *file, unsigned int line,
					  bool condition, const std::string &msg) {
		if(!condition) {
			std::stringstream out;
			out << "Assertion failed ";
			out << " at " << file << ':' << line << ' ';
			out << predicate << ": " << msg;
			throw AssertionException(out.str(), file, line);
		}
	}
}

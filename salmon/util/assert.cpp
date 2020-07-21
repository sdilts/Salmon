#include <sstream>

#include <util/assert.hpp>

namespace salmon {

	AssertionException::AssertionException(const std::string &msg,
										   std::source_location &loc) :
		std::logic_error(msg),
		loc{loc} { }

	void throw_assert(const char* predicate, bool condition, const std::string &msg,
					  std::source_location loc = std::source_location::current()) {
		if(!condition) {
			std::stringstream out;
			out << "Assertion failed ";
			out << " at " << loc.file_name() << ' '
				<< loc.function_name() << ' ' << loc.line() << ':' << loc.column() << ' ';
			out << predicate << ": " << msg;
			throw AssertionException(out.str(), loc);
		}
	}
}

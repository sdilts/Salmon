#ifndef SALMON_COMPILER_SYMBOL
#define SALMON_COMPILER_SYMBOL

#include <string>
#include <set>
#include <ostream>

namespace salmon::compiler {

	class Package;

	struct Symbol {
		const std::string name;
		std::optional<Package*> package;

		Symbol(const Symbol&) = delete;
		Symbol(Symbol &&) = default;
		void operator=(const Symbol&) = delete;
	};

	bool operator<(const std::string &first, const Symbol &second);
	bool operator<(const Symbol &first, const std::string &second);

	bool operator<(const Symbol &first, const Symbol &second);
	bool operator==(const Symbol &first, const Symbol &second);
	bool operator!=(const Symbol &first, const Symbol &second);

	std::ostream& operator<<(std::ostream &os, const Symbol &symbol);
}

#endif

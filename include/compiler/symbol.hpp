#ifndef SALMON_COMPILER_SYMBOL
#define SALMON_COMPILER_SYMBOL

#include <string>
#include <set>
#include <ostream>

namespace salmon::compiler {

	class Package;

	struct Symbol {
		const std::string name;
		Package &package;
	};

	class Package {
	public:
		Package(const std::string &name);
		Package(const std::string &name, const std::set<std::reference_wrapper<Package>> &used);

		const std::string name;

		friend const Symbol& intern_symbol(const std::string &name, Package &package);
		friend bool is_external(const Symbol &symbol, const Package &package);
	private:
		Package();

		friend std::optional<std::reference_wrapper<const Symbol>> owned_by(const std::string,
																	  const Package&);

		std::set<Symbol, std::less<>> interned;
		std::set<Symbol, std::less<>> exported;
		std::set<std::reference_wrapper<Package>> used;
	};

	bool operator<(const Package &first, const Package &second);
	bool operator<(const std::string &first, const Symbol &second);
	bool operator<(const Symbol &first, const std::string &second);
	bool operator<(const Symbol &first, const Symbol &second);

	std::ostream& operator<<(std::ostream &os, const Symbol &symbol);
	std::ostream& operator<<(std::ostream &os, const Package &package);
}



#endif

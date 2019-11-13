#ifndef SALMON_COMPILER_PACKAGE
#define SALMON_COMPILER_PACKAGE

#include <string>
#include <set>
#include <map>
#include <optional>
#include <functional>

#include <compiler/symbol.hpp>

namespace salmon::compiler {

	class Package {
	public:
		Package(const std::string &name);
		Package(const std::string &name, const std::set<std::reference_wrapper<Package>> &used);

		Package(const Package&) = delete;
		void operator=(const Package&) = delete;

		const std::string name;

		const Symbol& intern_symbol(const std::string &name);
		std::optional<std::reference_wrapper<const Symbol>> find_symbol(const std::string &name) const;
		bool is_exported(const Symbol &symbol) const;
		void export_symbol(const Symbol &symbol);

	private:
		Package();

		std::optional<std::reference_wrapper<const Symbol>>
		find_external_symbol(const std::string&) const;

		// TODO: figure out a better way to store the interned/exported symbols.
		std::map<std::string, Symbol> interned;
		std::set<std::reference_wrapper<const Symbol>, std::less<>> exported;
		std::set<std::reference_wrapper<Package>> used;
	};

	bool operator<(const Package &first, const Package &second);

	std::ostream& operator<<(std::ostream &os, const Package &package);

}
#endif

#ifndef SALMON_COMPILER_SYMBOL
#define SALMON_COMPILER_SYMBOL

#include <compare>
#include <string>
#include <set>
#include <ostream>
#include <optional>

#include <vm/allocateditem.hpp>

namespace salmon::vm {

	class Package;

	struct Symbol : public AllocatedItem {
		const std::string name;
		// Having this as a pointer is a real big footgun,
		// as it means packages can't be moved once symbols have
		// been interned in them *unless* the symbol pointers are updated.
		Package* package;

		Symbol(const std::string&, Package*);
		Symbol(const std::string&);
		Symbol(const std::string&&);
		Symbol(const std::string&&, Package*);
		Symbol(const Symbol&) = delete;
		Symbol(Symbol &&) = default;

		~Symbol();
		void operator=(const Symbol&) = delete;

		size_t allocated_size() const override;
		void print_debug_info() const override;
		std::partial_ordering operator<=>(const Symbol &) const;
	};

	bool operator==(const Symbol &first, const Symbol &second);
	std::ostream& operator<<(std::ostream &os, const Symbol &symbol);
}

#endif

#ifndef SALMON_COMPILER_SYMBOL
#define SALMON_COMPILER_SYMBOL

#include <string>
#include <set>
#include <ostream>

#include <vm/allocateditem.hpp>

namespace salmon::vm {

	class Package;

	struct Symbol : public AllocatedItem {
		const std::string name;
		std::optional<Package*> package;

		Symbol(const std::string&, const std::optional<Package*>&);
		Symbol(const std::string&);
		Symbol(const std::string&&);
		Symbol(const std::string&&, const std::optional<Package*>&);
		Symbol(const Symbol&) = delete;
		Symbol(Symbol &&) = default;

		~Symbol();
		void operator=(const Symbol&) = delete;

		size_t allocated_size() const override;
		void print_debug_info() const override;
	};

	bool operator<(const std::string &first, const Symbol &second);
	bool operator<(const Symbol &first, const std::string &second);
	bool operator>(const Symbol &first, const Symbol &second);

	bool operator<(const Symbol &first, const Symbol &second);
	bool operator==(const Symbol &first, const Symbol &second);
	bool operator!=(const Symbol &first, const Symbol &second);

	std::ostream& operator<<(std::ostream &os, const Symbol &symbol);
}

#endif

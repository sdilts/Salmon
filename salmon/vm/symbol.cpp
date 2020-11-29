#include <compare>
#include <optional>
#include <functional>
#include <iostream>

#include <assert.h>

#include <vm/symbol.hpp>
#include <vm/package.hpp>

namespace salmon::vm {

	Symbol::Symbol(const std::string &name, const std::optional<Package*> &package)
		: name{name}, package{package} {}

	Symbol::Symbol(const std::string &&name, const std::optional<Package*> &package)
		: name(name), package{package} {}

	Symbol::Symbol(const std::string &name) :
		name{name}, package{std::nullopt} {}

	Symbol::Symbol(const std::string &&name) :
		name(name), package{std::nullopt} {}

	Symbol::~Symbol() { }

	void Symbol::print_debug_info() const {
		std::cerr << this << " " << *this << std::endl;
	}

	size_t Symbol::allocated_size() const {
		return sizeof(Symbol);
	}

	bool operator==(const Symbol &first, const Symbol &second) {
		return &first == &second;
	}

	std::partial_ordering Symbol::operator<=>(const Symbol &other) const {
		if(this->package && other.package) {
			auto compare = *package <=> *(other.package);
			if(compare == std::strong_ordering::equal) {
				return this->name <=> other.name;
			} else return compare;
		} else if(package) {
			return std::partial_ordering::greater;
		} else if(other.package) {
			return std::partial_ordering::less;
		} else if(this == &other) {
			return std::partial_ordering::equivalent;
		}
		return this->name <=> other.name;
	}

	std::ostream& operator<<(std::ostream &os, const Symbol &symbol) {
		if(symbol.package) {
			os << (*symbol.package)->name;
			if((*symbol.package)->is_exported(symbol)) {
				os << ":";
			} else {
				os << "::";
			}
		} else {
			os << "#:";
		}
		return os << symbol.name;
	}
}

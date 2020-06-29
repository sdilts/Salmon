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

	bool operator<(const std::string &first, const Symbol &second) {
		return first < second.name;
	}

	bool operator<(const Symbol &first, const std::string &second) {
		return first.name < second;
	}

	bool operator<(const Symbol &first, const Symbol &second) {
		if(first.package && second.package) {
			if (*first.package == *second.package) {
				return first.name < second.name;
			}
			return **first.package < **second.package;
		} else if(first.package) {
			return false;
		} else if (second.package) {
			return true;
		}
		return first.name < second.name;
	}

	bool operator>(const Symbol &first, const Symbol &second) {
		if(first.package && second.package) {
			if (*first.package == *second.package) {
				return first.name > second.name;
			}
			return **first.package > **second.package;
		} else if(first.package) {
			return true;
		} else if (second.package) {
			return false;
		}
		return first.name > second.name;
	}

	bool operator==(const Symbol &first, const Symbol &second) {
		return &first == &second;
	}

	bool operator!=(const Symbol &first, const Symbol &second) {
		return !(first == second);
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

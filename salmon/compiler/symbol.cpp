#include <optional>
#include <functional>

#include <assert.h>

#include <compiler/symbol.hpp>

namespace salmon::compiler {

	Package::Package(const std::string &name) : name(name), interned(), exported(), used() { }

	Package::Package(const std::string &name, const std::set<std::reference_wrapper<Package>> &used) :
		name(name), interned(), exported(),used(used) { }


	std::optional<std::reference_wrapper<const Symbol>>
	Package::find_external_symbol(const std::string &name) const {
		auto result = this->exported.find(name);
		if(result == this->exported.end()) {
			for(const auto &pkg : this->used) {
				auto found = pkg.get().find_external_symbol(name);
				if(found) {
					return *found;
				}
			}
			return std::nullopt;
		}
		return *result;
	}

	const Symbol& Package::intern_symbol(const std::string &name) {

		for(const auto &pkg : this->used) {
			auto found = pkg.get().find_external_symbol(name);
			if(found) {
				return *found;
			}
		}

		auto interned_result = interned.lower_bound(name);
		if(interned_result == interned.end() || (*interned_result).name != name) {
			Symbol new_symb = {name, std::make_optional(this)};
			auto final_place = interned.insert(interned_result, std::move(new_symb));
			assert((*final_place).name == name);
			return *final_place;
		}
		return *interned_result;
	}

	std::optional<std::reference_wrapper<const Symbol>> Package::find_symbol(const std::string &name) const {
		for(const auto &pkg : this->used) {
			auto found = pkg.get().find_external_symbol(name);
			if(found) {
				return *found;
			}
		}

		auto interned_result = this->interned.find(name);
		if(interned_result == this->interned.end() || (*interned_result).name != name) {
			return *interned_result;
		}
		return std::nullopt;
	}

	void Package::export_symbol(const Symbol &symbol) {
		exported.insert(symbol);
	}

	bool Package::is_exported(const Symbol &symbol) const {
		auto found = this->exported.find(symbol);
		return found != this->exported.end();
	}

	bool operator<(const Package &first, const Package &second) {
		return first.name < second.name;
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
			// std::optional uses the package comparison operators under the hood:
			return first.package < second.package;
		} else if(first.package) {
			return false;
		} else if (second.package) {
			return true;
		}
		return first.name < second.name;
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
		} else {
			os << "#:";
		}
		return os << symbol.name;
	}

	std::ostream& operator<<(std::ostream &os, const Package &package) {
		return os << "#<PACKAGE: \"" << package.name << "\">";
	}
}

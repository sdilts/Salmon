#include <optional>
#include <functional>

#include <assert.h>

#include <compiler/symbol.hpp>

namespace salmon::compiler {

	Package::Package(const std::string &name) : name(name), interned(), exported(), used() { }

	Package::Package(const std::string &name, const std::set<std::reference_wrapper<Package>> &used) :
		name(name), interned(), exported(),used(used) { }


	std::optional<std::reference_wrapper<const Symbol>>
	owned_by(const std::string name, const Package &package) {
		auto result = package.interned.find(name);
		if(result == package.interned.end()) {
			return std::nullopt;
		}
		return std::make_optional(std::ref(*result));
	}

	const Symbol& intern_symbol(const std::string &name, Package &package) {

		for(const auto &pkg : package.used) {
			auto found = owned_by(name, pkg);
			if(found) {
				return *found;
			}
		}

		auto interned_result = package.interned.lower_bound(name);
		if(interned_result == package.interned.end() || (*interned_result).name != name) {
			auto final_place = package.interned.insert(interned_result, {name, package});
			assert((*final_place).name == name);
			return *final_place;
		}
		return *interned_result;
	}

	bool is_external(const Symbol &symbol, const Package &package) {
		auto found = package.exported.find(symbol);
		return found != package.exported.end();
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
		return first.name < second.name;
	}

	std::ostream& operator<<(std::ostream &os, const Symbol &symbol) {
		return os << symbol.package.name << "::" << symbol.name;
	}

	std::ostream& operator<<(std::ostream &os, const Package &package) {
		return os << "#<PACKAGE: \"" << package.name << "\">";
	}

}

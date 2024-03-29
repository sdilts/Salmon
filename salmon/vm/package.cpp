#include <compare>
#include <vm/package.hpp>
#include <util/assert.hpp>

namespace salmon::vm {

	Package::Package(const std::string &name, MemoryManager &mem_manager)
		: name(name),
		  mem_manager{mem_manager},
		  interned(),
		  exported(),
		  used() { }

	Package::Package(std::string &&name, MemoryManager &mem_manager)
		: name(name),
		  mem_manager{mem_manager},
		  interned(),
		  exported(),
		  used() { }

	Package::Package(const std::string &name, MemoryManager &mem_manager,
					 const std::set<Package*> &used)
		: name(name),
		  mem_manager{mem_manager},
		  interned(),
		  exported(),
		  used(used) { }


	std::optional<vm_ptr<Symbol>> Package::find_external_symbol(const std::string_view name) const {
		auto result = this->exported.find(name);
		if(result == this->exported.end()) {
			for(const auto &pkg : this->used) {
				auto found = pkg->find_external_symbol(name);
				if(found) {
					return *found;
				}
			}
			return std::nullopt;
		}
		return (*result).second;
	}

    vm_ptr<Symbol> Package::intern_symbol(std::string &&name) {
		for(const auto &pkg : this->used) {
			auto found = pkg->find_external_symbol(name);
			if(found) {
				return *found;
			}
		}

		auto interned_result = interned.lower_bound(name);
		if(interned_result == interned.end() || (*interned_result).second->name != name) {
			vm_ptr<Symbol> new_symb = mem_manager.allocate_obj<Symbol>(name, this);
			auto final_place = interned.emplace_hint(interned_result, name, std::move(new_symb));
			salmon_check((*final_place).second->name == name, "Name not added correctly");
			return (*final_place).second;
		}
		return (*interned_result).second;
	}

    vm_ptr<Symbol> Package::intern_symbol(const std::string &name) {
		for(const auto &pkg : this->used) {
			auto found = pkg->find_external_symbol(name);
			if(found) {
				return *found;
			}
		}

		auto interned_result = interned.lower_bound(name);
		if(interned_result == interned.end() || (*interned_result).second->name != name) {
			vm_ptr<Symbol> new_symb = mem_manager.allocate_obj<Symbol>(name, this);
			auto final_place = interned.emplace_hint(interned_result, name, std::move(new_symb));
			salmon_check((*final_place).second->name == name, "Name not added correctly");
			return (*final_place).second;
		}
		return (*interned_result).second;
	}

	std::optional<vm_ptr<Symbol>> Package::find_symbol(const std::string_view name) const {
		for(const auto &pkg : this->used) {
			auto found = pkg->find_external_symbol(name);
			if(found) {
				return *found;
			}
		}

		auto interned_result = this->interned.find(name);
		if(interned_result != interned.end() && (*interned_result).second->name == name) {
			return (*interned_result).second;
		}
		return std::nullopt;
	}

	void Package::export_symbol(const vm_ptr<Symbol> &symbol) {
		exported.insert({(*symbol).name, vm_ptr(symbol)});
	}

	bool Package::is_exported(const Symbol &symbol) const {
		auto found = this->exported.find(symbol.name);
		return found != this->exported.end();
	}

	std::strong_ordering Package::operator<=>(const Package &other) const {
		return this->name <=> other.name;
	}

	bool Package::operator==(const Package& other) const {
		return this-> name == other.name;
	}

	std::ostream& operator<<(std::ostream &os, const Package &package) {
		return os << "#<PACKAGE: \"" << package.name << "\">";
	}

}

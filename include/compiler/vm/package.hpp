#ifndef SALMON_COMPILER_PACKAGE
#define SALMON_COMPILER_PACKAGE

#include <string>
#include <set>
#include <map>
#include <optional>
#include <functional>

#include <compiler/vm/vm_ptr.hpp>
#include <compiler/vm/memory.hpp>
#include <compiler/vm/symbol.hpp>

namespace salmon::vm {

	class Package {
	public:
		Package(const std::string &name, MemoryManager &mem_manager);
		Package(const std::string &name, MemoryManager &mem_manager,
				const std::set<std::reference_wrapper<Package>> &used);

		// Package(const Package&) = delete;
		// void operator=(const Package&) = delete;

		const std::string name;
		salmon::vm::MemoryManager &mem_manager;

		vm_ptr<Symbol> intern_symbol(const std::string &name);
		std::optional<vm_ptr<Symbol>> find_symbol(const std::string &name) const;
		bool is_exported(const Symbol &symbol) const;
		void export_symbol(vm_ptr<Symbol> &symbol);

	private:
		Package();

		std::optional<vm_ptr<Symbol>> find_external_symbol(const std::string&) const;

        std::map<std::string, vm_ptr<Symbol>> interned;
		std::map<std::string, vm_ptr<Symbol>> exported;

		std::set<std::reference_wrapper<Package>> used;
	};

	bool operator<(const Package &first, const Package &second);

	std::ostream& operator<<(std::ostream &os, const Package &package);

}
#endif

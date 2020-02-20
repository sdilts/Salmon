#ifndef SALMON_COMPILER_PACKAGE
#define SALMON_COMPILER_PACKAGE

#include <string>
#include <set>
#include <map>
#include <optional>
#include <functional>

#include <vm/vm_ptr.hpp>
#include <vm/memory.hpp>
#include <vm/symbol.hpp>

namespace salmon::vm {

	class Package {
	public:
		Package(const std::string &name, MemoryManager &mem_manager);
		Package(const std::string &name, MemoryManager &mem_manager,
				const std::set<std::reference_wrapper<Package>> &used);

		// Class invariants mean that packages can be moved but not copied.
		Package(const Package&) = delete;
		Package& operator=(const Package&) = delete;

		Package(Package&&) = default;

		const std::string name;
		//! the memory manager that the package should use to allocate new symbols
		salmon::vm::MemoryManager &mem_manager;

		/**
		 * If a symbol doesn't exist in the package, create it and return the new symbol.
		 * If it already exists, return that symbol
		 * @param name the name of the new symbol.
		 **/
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
	bool operator>(const Package &first, const Package &second);

	std::ostream& operator<<(std::ostream &os, const Package &package);

}
#endif

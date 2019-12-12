#ifndef SALMON_COMPILER_VM_VM
#define SALMON_COMPILER_VM_VM

#include <unordered_map>
#include <string>

#include <compiler/vm/memory.hpp>
#include <compiler/vm/type.hpp>
#include <compiler/vm/package.hpp>

namespace salmon::vm {

	class VirtualMachine {
	public:
		VirtualMachine();

		MemoryManager mem_manager;

		Package& find_package(const std::string &name);
		Package& find_package(const vm_ptr<Symbol> &package);

	private:
		TypeTable type_table;
		std::unordered_map<std::string, Package> packages;
	};
}

#endif

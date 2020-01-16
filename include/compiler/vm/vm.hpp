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
		VirtualMachine(const std::string &base_package);

		MemoryManager mem_manager;

		TypeTable type_table;
		std::unordered_map<std::string, Package> packages;
	};
}

#endif

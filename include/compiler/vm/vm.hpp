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

		std::optional<std::reference_wrapper<Package>> find_package(const std::string &name);
		std::optional<std::reference_wrapper<Package>> find_package(const vm_ptr<Symbol> &name);

		MemoryManager mem_manager;
		TypeTable type_table;
		std::unordered_map<std::string, Package> packages;

		// These will be use often enough that it is work it to be able to access them
		// with a single method call.
		Type *int32_type();
		Type *float_type();

	private:
		Type *_int32_type;
		Type *_float_type;
	};
}

#endif

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
		Type *symbol_type();
		Type *list_type();
		Type *const_str_type();

	private:
		Type *_int32_type;
		Type *_float_type;
		Type *_symbol_type;
		Type *_list_type;
		Type *_const_str_type;
	};
}

#endif

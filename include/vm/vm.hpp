#ifndef SALMON_COMPILER_VM_VM
#define SALMON_COMPILER_VM_VM

#include <unordered_map>
#include <string>

#include <vm/memory.hpp>
#include <vm/type.hpp>
#include <vm/package.hpp>
#include <vm/function.hpp>
#include <salmon/config.hpp>

namespace salmon::vm {

	class VirtualMachine {
	public:
		VirtualMachine(const Config &config, const std::string &base_package);

		std::optional<std::reference_wrapper<Package>> find_package(const std::string &name);
		std::optional<std::reference_wrapper<Package>> find_package(const vm_ptr<Symbol> &name);

		// //! Call function name with args args:
		// Box dispatch_function(vm_ptr<Symbol> &name, vm_ptr<List> &args);
		// //! register the function with the given name.
		// void register_function(vm_ptr<Symbol> &name, VmFunction &function);
		// //! register the interface with the given name and lambda list:
		// void register_interface(vm_ptr<Symbol> &name, InterfaceFunction &function);

		// These will be used often enough that it is worth it to be able to access them
		// with a single method call.
		TypePtr int32_type();
		TypePtr float_type();
		TypePtr symbol_type();
		TypePtr list_type();
		TypePtr dyn_array_type();
		TypePtr empty_type();
		TypePtr const_str_type();

		MemoryManager mem_manager;
		TypeTable type_table;
		std::unordered_map<std::string, Package> packages;
	private:
		Config _config;

		// std::map<vm_ptr<Symbol>, std::unique_ptr<VmFunction>> func_table;
		// registering interface functions require different steps,
		// and we want to avoid instanceof usage, so use a different table.
		// std::map<vm_ptr<Symbol>, InterfaceFunction> interface_table;

		TypePtr _int32_type;
		TypePtr _float_type;
		TypePtr _symbol_type;
		TypePtr _list_type;
		TypePtr _dyn_array_type;
		TypePtr _empty_type;
		TypePtr _const_str_type;
	};
}

#endif

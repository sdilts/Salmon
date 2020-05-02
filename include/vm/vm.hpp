#ifndef SALMON_COMPILER_VM_VM
#define SALMON_COMPILER_VM_VM

#include <unordered_map>
#include <string>
#include <typeinfo>
#include <typeindex>

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

		template<typename T>
		Box make_boxed(vm_ptr<T> item) {
			TypePtr type = builtin_map[typeid(T)];
			Box box(item);
			box.type = type;
			return box;
		}

		template<typename T>
		Box make_boxed(T item) {
			TypePtr type = builtin_map[typeid(T)];
			Box box(mem_manager.make_vm_ptr<vm::AllocatedItem>());
			box.set_value(item);
			box.type = type;
			return box;
		}

		// //! Call function name with args args:
		// Box dispatch_function(vm_ptr<Symbol> &name, vm_ptr<List> &args);
		// //! register the function with the given name.
		// void register_function(vm_ptr<Symbol> &name, VmFunction &function);
		// //! register the interface with the given name and lambda list:
		// void register_interface(vm_ptr<Symbol> &name, InterfaceFunction &function);

		MemoryManager mem_manager;
		TypeTable type_table;
		std::unordered_map<std::string, Package> packages;
	private:
		Config _config;

		//! map used to lookup the VM types of builtin types
		std::unordered_map<std::type_index, TypePtr> builtin_map;
		// std::map<vm_ptr<Symbol>, std::unique_ptr<VmFunction>> func_table;
		// registering interface functions require different steps,
		// and we want to avoid instanceof usage, so use a different table.
		// std::map<vm_ptr<Symbol>, InterfaceFunction> interface_table;

	};
}

#endif

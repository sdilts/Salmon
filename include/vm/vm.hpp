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
		Box make_boxed(const vm_ptr<T> &item) {
			salmon_check(builtin_map.find(typeid(T)) != builtin_map.end(), "builtin type not found");
			auto type = builtin_map[typeid(T)];
			auto vm_ptr = mem_manager.make_vm_ptr<Type>();
			vm_ptr = type;
			Box box(item, vm_ptr);
			return box;
		}

		template<typename T>
		Box make_boxed(vm_ptr<T> &&item) {
			salmon_check(builtin_map.find(typeid(T)) != builtin_map.end(), "builtin type not found");
			auto type = builtin_map[typeid(T)];
			auto vm_ptr = mem_manager.make_vm_ptr<Type>();
			vm_ptr = type;
			Box box(item, vm_ptr);
			return box;
		}

		template<typename T>
		Box make_boxed(T item) {
			salmon_check(builtin_map.find(typeid(T)) != builtin_map.end(), "builtin type not found");
			auto type = builtin_map[typeid(T)];
			auto vm_ptr = mem_manager.make_vm_ptr<Type>();
			vm_ptr = type;
			Box box(item, vm_ptr);
			box.set_value(item);
			return box;
		}

		template<typename T>
		vm_ptr<Type> get_builtin_type() {
			salmon_check(builtin_map.find(typeid(T)) != builtin_map.end(), "builtin type not found");
			auto type = builtin_map[typeid(T)];
			auto vm_ptr = mem_manager.make_vm_ptr<Type>(type);
			return vm_ptr;
		}

		// //! Call function name with args args:
		// Box dispatch_function(vm_ptr<Symbol> &name, vm_ptr<List> &args);
		// //! register the function with the given name.
		// void register_function(vm_ptr<Symbol> &name, VmFunction &function);
		// //! register the interface with the given name and lambda list:
		// void register_interface(vm_ptr<Symbol> &name, InterfaceFunction &function);

		MemoryManager mem_manager;
		TypeTable type_table;
		FunctionTable fn_table;
		std::unordered_map<std::string, Package> packages;
	private:
		Config _config;

		// TODO: use one of those fancy constexpr maps:
		//! map used to lookup the VM types of builtin types
		std::unordered_map<std::type_index, Type*> builtin_map;
	};
}

#endif

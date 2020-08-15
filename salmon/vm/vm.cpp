#include <util/assert.hpp>

#include <vm/vm.hpp>
#include <vm/string.hpp>
#include <vm/builtinfunction.hpp>
#include "vmstdlib.cpp"

namespace salmon::vm {

	template<typename ... Args>
	static void add_interface_fn(VirtualMachine *vm,
							 const vm_ptr<Symbol> &name,
							 const std::string &doc,
							 const std::vector<vm_ptr<Symbol>> &lambda_list,
							 vm_ptr<Type> interface_type,
							 std::vector<std::pair<vm_ptr<Type>,typename BuiltinFunction<Args...>::FunctionType>> impls) {
		FunctionTable &fn_table = vm->fn_table;

		vm_ptr<InterfaceFunction> interface_fn =
			vm->mem_manager.allocate_obj<InterfaceFunction>(interface_type, lambda_list,
															doc, std::nullopt);
		salmon_ensure(fn_table.new_interface(name, interface_fn),
					  "Interface function not added");
		Package &base_package = vm->base_package();
		base_package.export_symbol(name);

		for(const auto &[type, fn] : impls) {
			vm_ptr<VmFunction> vm_fn(vm->mem_manager
									 .allocate_obj<BuiltinFunction<Args...>>(fn,
																			 type,
																			 lambda_list));
			salmon_ensure(vm->fn_table.add_function(name, vm_fn), "Function not added");
		}
	}

	static void init_print_fns(VirtualMachine *vm) {
		Package &base_package = vm->base_package();
		TypeTable &type_table = vm->type_table;

		vm_ptr<Symbol> obj_symb = base_package.intern_symbol("obj");
		vm_ptr<Symbol> print_symb = base_package.intern_symbol("print");
		std::vector<vm_ptr<Symbol>> lambda_list = { obj_symb };
		SpecBuilder interfaceBuilder;
		interfaceBuilder.add_parameter(obj_symb);
		vm_ptr<Type> p_interface_type = type_table.get_fn_type(interfaceBuilder.build(),
															   interfaceBuilder.build());

		std::vector<std::pair<vm_ptr<Type>,BuiltinFunction<InternalBox>::FunctionType>> to_add = {
			{ vm->get_builtin_type<Symbol>(),       print_pointer_primitive<Symbol> },
			{ vm->get_builtin_type<StaticString>(), print_pointer_primitive<StaticString> },
			{ vm->get_builtin_type<double>(),       print_primitive<double> },
			{ vm->get_builtin_type<int32_t>(),      print_primitive<int32_t> },
			{ vm->get_builtin_type<bool>(),         print_primitive<bool> },
			{ vm->get_builtin_type<Empty>(),        print_primitive<Empty> },
			{ vm->get_builtin_type<Array>(),        print_array },
			{ vm->get_builtin_type<List>(),         print_list },
		};

		for(auto &[type, fn] : to_add) {
			SpecBuilder spec;
			vm_ptr<Type> tmp_type = type;
			spec.add_type(tmp_type);
			type = vm->type_table.get_fn_type(spec.build(), spec.build());
		}

		add_interface_fn<InternalBox>(vm, print_symb,
								  "Print the readable representation of an object",
								  lambda_list, p_interface_type,
								  to_add);
	}

	static void init_stdlib(VirtualMachine *vm) {
		init_print_fns(vm);
	}

	template<typename T>
	static constexpr size_t type_size() {
		if(std::is_same<T,Empty>::value
		   || !(std::is_class<T>::value || std::is_union<T>::value)) {
			return sizeof(T);
		} else {
			T* tmp;
			return sizeof(tmp);
		}
	}

	template<typename T>
	static void init_primitive_type(Package &base_package, TypeTable &t_table,
									std::unordered_map<std::type_index, Type*> &builtin_map,
									const std::string &name, const std::string &doc) {
		vm_ptr<Symbol> name_symb = base_package.intern_symbol(name);
		base_package.export_symbol(name_symb);

		size_t size = type_size<T>();

		vm_ptr<Type> type = t_table.make_primitive(name_symb, doc, size);
		builtin_map[typeid(T)] = type.get();
	}

	static void init_types(Package &base_package, TypeTable &t_table,
					   std::unordered_map<std::type_index, Type*> &builtin_map) {
		init_primitive_type<StaticString>(base_package, t_table, builtin_map,
									  "const-string", "Constant string type used by the vm");
		init_primitive_type<List>(base_package, t_table, builtin_map,
							 "list", "Linked List used by the vm");
		init_primitive_type<Array>(base_package, t_table, builtin_map,
							   "dyn-array", "Dynamic array used by the vm");
		init_primitive_type<Symbol>(base_package, t_table, builtin_map,
								"symbol", "symbol");
		init_primitive_type<int32_t>(base_package, t_table, builtin_map,
								 "int-32", "32 bit signed integer type");
		init_primitive_type<double>(base_package, t_table, builtin_map,
								"float-64", "64 bit floating type");
		init_primitive_type<bool>(base_package, t_table, builtin_map,
							  "bool", "Boolean type");
		init_primitive_type<Empty>(base_package, t_table, builtin_map,
								"Empty", "Type representing an empty object");
	}

	VirtualMachine::VirtualMachine(const Config &config, const std::string &base_package) :
		mem_manager{},
		type_table{mem_manager},
		fn_table{},
		packages{},
		_config{config},
		base_package_name(base_package),
		builtin_map{} {
		packages.emplace(std::string(base_package), Package(base_package, mem_manager));

		Package &base_pkg = packages.find(base_package_name)->second;
		init_types(base_pkg, type_table, builtin_map);
		init_stdlib(this);
	}

	Package &VirtualMachine::base_package() {
		return packages.find(base_package_name)->second;
	}

	std::optional<std::reference_wrapper<Package>> VirtualMachine::find_package(const std::string &name) {
		auto pkg_iter = packages.find(name);
		if(pkg_iter != packages.end()) {
			return std::make_optional(std::reference_wrapper(pkg_iter->second));
		} else {
			return std::nullopt;
		}
	}
	std::optional<std::reference_wrapper<Package>> VirtualMachine::find_package(const vm_ptr<Symbol> &name) {
		return find_package((*name).name);
	}
}

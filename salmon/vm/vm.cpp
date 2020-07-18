#include <util/assert.hpp>

#include <vm/vm.hpp>
#include <vm/string.hpp>
#include <vm/builtinfunction.hpp>
#include "vmstdlib.cpp"

namespace salmon::vm {

	template<typename ... Args>
	static void add_function(VirtualMachine *vm,
							 const vm_ptr<Symbol> &name,
							 typename BuiltinFunction<Args...>::FunctionType fn,
							 const std::vector<vm_ptr<Symbol>> &lambda_list,
							 const TypeSpecification &ret, const TypeSpecification &args) {
		vm_ptr<Type> fn_type = vm->type_table.get_fn_type(ret, args);
		vm_ptr<VmFunction> vm_fn(vm->mem_manager
								 .allocate_obj<BuiltinFunction<Args...>>(fn,
																   fn_type,
																   lambda_list));
		salmon_ensure(vm->fn_table.add_function(name, vm_fn), "Function not added");
	}

	static void init_print_fns(VirtualMachine *vm) {
		FunctionTable &fn_table = vm->fn_table;
		Package &base_package = vm->base_package();
		TypeTable &type_table = vm->type_table;

		vm_ptr<Symbol> obj_symb = base_package.intern_symbol("obj");
		vm_ptr<Symbol> print_symb = base_package.intern_symbol("print");
		SpecBuilder interfaceBuilder;
		interfaceBuilder.add_parameter(obj_symb);
		vm_ptr<Type> p_interface_type = type_table.get_fn_type(interfaceBuilder.build(),
															   interfaceBuilder.build());
		std::vector<vm_ptr<Symbol>> lambda_list = {obj_symb};
		vm_ptr<InterfaceFunction> p_interface =
			vm->mem_manager.allocate_obj<InterfaceFunction>(p_interface_type, lambda_list);
		salmon_ensure(fn_table.new_interface(print_symb, p_interface),
					  "Interface function not added");
		base_package.export_symbol(print_symb);

		std::vector<std::pair<vm_ptr<Type>,BuiltinFunction<InternalBox>::FunctionType>> to_add =
		{
			{ vm->get_builtin_type<Symbol>(),       print_pointer_primitive<Symbol> },
			{ vm->get_builtin_type<StaticString>(), print_pointer_primitive<StaticString> },
			{ vm->get_builtin_type<double>(),       print_primitive<double> },
			{ vm->get_builtin_type<int32_t>(),      print_primitive<int32_t> },
			{ vm->get_builtin_type<bool>(),         print_primitive<bool> },
			{ vm->get_builtin_type<Empty>(),        print_primitive<Empty> },
			{ vm->get_builtin_type<Array>(),        print_array },
			{ vm->get_builtin_type<List>(),         print_list },
		};

		for(const auto &[type, fn] : to_add) {
			SpecBuilder spec;
			vm_ptr<Type> tmp_type = type;
			spec.add_type(tmp_type);
			add_function<InternalBox>(vm, print_symb, fn, lambda_list,
									  spec.build(), spec.build());
		}

	}

	static void init_stdlib(VirtualMachine *vm) {
		init_print_fns(vm);
	}

	static void init_types(Package &base_package, TypeTable &t_table,
						   std::unordered_map<std::type_index, Type*> &builtin_map) {
		vm_ptr<Symbol> str_symb = base_package.intern_symbol("const-string");
		vm_ptr<Symbol> list_symb = base_package.intern_symbol("list");
		vm_ptr<Symbol> dyn_arr_symb = base_package.intern_symbol("dyn-array");
		vm_ptr<Symbol> int_symb = base_package.intern_symbol("int-32");
		vm_ptr<Symbol> double_symb = base_package.intern_symbol("float-64");
		vm_ptr<Symbol> symb_symb = base_package.intern_symbol("symbol");
		vm_ptr<Symbol> bool_symb = base_package.intern_symbol("boolean");
		vm_ptr<Symbol> empty_symb = base_package.intern_symbol("empty");

		base_package.export_symbol(str_symb);
		base_package.export_symbol(list_symb);
		base_package.export_symbol(dyn_arr_symb);
		base_package.export_symbol(int_symb);
		base_package.export_symbol(double_symb);
		base_package.export_symbol(symb_symb);
		base_package.export_symbol(bool_symb);
		base_package.export_symbol(empty_symb);

		TypePtr str =  t_table.make_primitive(str_symb, "Constant string type used by the vm",
											  sizeof(vm_ptr<Symbol>));
		TypePtr list = t_table.make_primitive(list_symb, "Linked list used by the vm",
											  sizeof(vm_ptr<Symbol>));
		TypePtr dyn_arr = t_table.make_primitive(dyn_arr_symb, "Dynamic array used by the vm",
												 sizeof(vm_ptr<Symbol>));
		TypePtr int_type = t_table.make_primitive(int_symb,"32 bit integer", sizeof(vm_ptr<Symbol>));
		TypePtr double_type = t_table.make_primitive(double_symb, "32 bit floating point",
													 sizeof(vm_ptr<Symbol>));
		TypePtr symbol_type = t_table.make_primitive(symb_symb, "Symbol",
													 sizeof(vm_ptr<Symbol>));
		TypePtr bool_type = t_table.make_primitive(bool_symb, "True or false type",
												   sizeof(vm_ptr<Symbol>));
		TypePtr empty_type = t_table.make_primitive(empty_symb, "Type representing the empty object.",
													sizeof(vm_ptr<Symbol>));

		builtin_map[typeid(StaticString)] = str.get();
		builtin_map[typeid(List)] = list.get();
		builtin_map[typeid(Array)] = dyn_arr.get();
		builtin_map[typeid(int32_t)] = int_type.get();
		builtin_map[typeid(double)] = double_type.get();
		builtin_map[typeid(Symbol)] = symbol_type.get();
		builtin_map[typeid(bool)] = bool_type.get();
		builtin_map[typeid(Empty)] = empty_type.get();
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

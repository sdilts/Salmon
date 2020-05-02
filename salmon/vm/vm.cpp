#include <util/assert.hpp>

#include <vm/vm.hpp>
#include <vm/string.hpp>

namespace salmon::vm {

	static void init_types(Package &base_package, TypeTable &t_table,
						   std::unordered_map<std::type_index, TypePtr> &builtin_map) {
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

		builtin_map[typeid(StaticString)] = str;
		builtin_map[typeid(List)] = list;
		builtin_map[typeid(Array)] = dyn_arr;
		builtin_map[typeid(int32_t)] = int_type;
		builtin_map[typeid(double)] = double_type;
		builtin_map[typeid(Symbol)] = symbol_type;
		builtin_map[typeid(bool)] = bool_type;
		builtin_map[typeid(Empty)] = empty_type;
	}

	VirtualMachine::VirtualMachine(const Config &config, const std::string &base_package) :
		mem_manager{},
		type_table{},
		packages{},
		_config{config} {
		packages.emplace(std::string(base_package), Package(base_package, mem_manager));

		Package &base_pkg = packages.find(base_package)->second;
	    init_types(base_pkg, type_table, builtin_map);
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

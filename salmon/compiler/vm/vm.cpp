#include <cassert>

#include <compiler/vm/vm.hpp>

namespace salmon::vm {

	static void init_types(Package &base_package, TypeTable &t_table) {
		vm_ptr<Symbol> str_symb = base_package.intern_symbol("const-string");
		vm_ptr<Symbol> list_symb = base_package.intern_symbol("list");
		vm_ptr<Symbol> int_symb = base_package.intern_symbol("int-32");
		vm_ptr<Symbol> double_symb = base_package.intern_symbol("float-32");
		vm_ptr<Symbol> symb_symb = base_package.intern_symbol("symbol");
		vm_ptr<Symbol> bool_symb = base_package.intern_symbol("boolean");
		vm_ptr<Symbol> empty_symb = base_package.intern_symbol("empty");

		base_package.export_symbol(str_symb);
		base_package.export_symbol(list_symb);
		base_package.export_symbol(int_symb);
		base_package.export_symbol(double_symb);
		base_package.export_symbol(symb_symb);
		base_package.export_symbol(bool_symb);
		base_package.export_symbol(empty_symb);

		Type str =  { str_symb,  {}, "Constant string type used by the vm" };
		Type list = { list_symb, {}, "Linked list used by the vm" };
		Type int_type =    {int_symb, {}, "32 bit integer" };
		Type double_type = {double_symb, {}, "32 bit floating point"};
		Type symbol_type = {symb_symb, {}, "Symbol"};
		Type bool_type = {bool_symb, {}, "True or false type"};
		Type empty_type = {empty_symb, {}, "Type representing an object containing nothing."};

		t_table.insert(str);
		t_table.insert(list);
		t_table.insert(int_type);
		t_table.insert(double_type);
		t_table.insert(symbol_type);
		t_table.insert(bool_type);
		t_table.insert(empty_type);
	}

	VirtualMachine::VirtualMachine(const Config &config, const std::string &base_package) :
		mem_manager{},
		type_table{},
		packages{},
		_config{config} {
		packages.emplace(std::string(base_package), Package(base_package, mem_manager));

		Package &base_pkg = packages.find(base_package)->second;
	    init_types(base_pkg, type_table);

		_int32_type = *type_table.get_type(*base_pkg.find_symbol("int-32"));
		_float_type = *type_table.get_type(*base_pkg.find_symbol("float-32"));
		_list_type = *type_table.get_type(*base_pkg.find_symbol("list"));
		_empty_type = *type_table.get_type(*base_pkg.find_symbol("empty"));
		_const_str_type = *type_table.get_type(*base_pkg.find_symbol("const-string"));
		_symbol_type = *type_table.get_type(*base_pkg.find_symbol("symbol"));
		// TODO: use C++ assertions with exceptions
		assert(_int32_type != nullptr && _float_type != nullptr && _list_type != nullptr
			&& _const_str_type != nullptr && _symbol_type != nullptr && _empty_type != nullptr);
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

	Type *VirtualMachine::int32_type() {
		return _int32_type;
	}

	Type *VirtualMachine::float_type() {
		return _float_type;
	}

	Type *VirtualMachine::symbol_type() {
		return _symbol_type;
	}

	Type *VirtualMachine::empty_type() {
		return _empty_type;
	}

	Type *VirtualMachine::list_type() {
		return _list_type;
	}

	Type *VirtualMachine::const_str_type() {
		return _const_str_type;
	}

}

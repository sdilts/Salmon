#include <compiler/vm/vm.hpp>

namespace salmon::vm {

	static void init_types(Package &base_package, TypeTable &t_table) {
		vm_ptr<Symbol> str_symb = base_package.intern_symbol("const-string");
		vm_ptr<Symbol> list_symb = base_package.intern_symbol("list");
		vm_ptr<Symbol> int_symb = base_package.intern_symbol("int-32");
		vm_ptr<Symbol> double_symb = base_package.intern_symbol("float-32");

		Type str =  { str_symb,  {}, "Constant string type used by the vm" };
		Type list = { list_symb, {}, "Linked list used by the vm" };
		Type int_type =    {int_symb, {}, "32 bit integer" };
		Type double_type = {double_symb, {}, "32 bit floating point"};

		t_table.insert(str);
		t_table.insert(list);
		t_table.insert(int_type);
		t_table.insert(double_type);
	}

	VirtualMachine::VirtualMachine(const std::string &base_package) :
		mem_manager{}, type_table{}, packages{} {
		packages.emplace(std::string(base_package), Package(base_package, mem_manager));

		auto base_pkg_iter = packages.find(base_package);
	    init_types(base_pkg_iter->second, type_table);
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

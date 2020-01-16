#include <compiler/vm/vm.hpp>

namespace salmon::vm {

	// static void init_types(VirtualMachine &vm) {

	// }

	VirtualMachine::VirtualMachine(const std::string &base_package) :
		mem_manager{}, type_table{}, packages{} {
		packages.emplace(std::string(base_package), Package(base_package, mem_manager));
	}

}

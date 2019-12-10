#include <compiler/vm/vm.hpp>

namespace salmon::vm {

	// static void init_types(VirtualMachine &vm) {

	// }

	VirtualMachine::VirtualMachine() :
		mem_manager{}, type_table{}, packages{} {
		// packages.insert(std::make_pair("salmon", Package("salmon", mem_manager)));
	}

}

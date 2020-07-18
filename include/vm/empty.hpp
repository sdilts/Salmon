#ifndef SALMON_COMPILER_VM_EMPTY
#define SALMON_COMPILER_VM_EMPTY

#include <iostream>

#include <vm/allocateditem.hpp>


namespace salmon::vm {

	struct Empty {

	};

	std::ostream& operator<<(std::ostream &os, const Empty &item);

}

#endif

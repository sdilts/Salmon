#ifndef SALMON_COMPILER_VM_ALLOCATED_ITEM
#define SALMON_COMPILER_VM_ALLOCATED_ITEM

#include <compiler/vm/allocateditem.hpp>

namespace salmon::vm {

	struct AllocatedItem {

		virtual ~AllocatedItem() = 0;
	};

}

#endif

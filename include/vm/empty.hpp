#ifndef SALMON_COMPILER_VM_EMPTY
#define SALMON_COMPILER_VM_EMPTY

#include <iostream>

#include <vm/allocateditem.hpp>


namespace salmon::vm {

	struct Empty {
		auto constexpr operator<=>(const Empty&) const {
			return std::strong_ordering::equal;
		}

		bool constexpr operator==(const Empty&) const {
			return true;
		}
	};

	std::ostream& operator<<(std::ostream &os, const Empty &item);
}

#endif

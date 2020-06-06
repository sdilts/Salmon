#ifndef SALMON_COMPILER_VM_ALLOCATED_ITEM
#define SALMON_COMPILER_VM_ALLOCATED_ITEM

#include <vector>
#include <functional>

namespace salmon::vm {

	struct AllocatedItem {
		virtual ~AllocatedItem() = 0;

		/**
		 * Return the immediate roots of the item.
		 * The return value may include items that also contain roots, and need to be searched.
		 **/
		virtual void get_roots(const std::function<void(AllocatedItem*)> &) const {

		};

		virtual void print_debug_info() const = 0;
		virtual size_t allocated_size() const = 0;
	};
}

#endif

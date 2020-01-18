#ifndef SALMON_COMPILER_VM_ALLOCATED_ITEM
#define SALMON_COMPILER_VM_ALLOCATED_ITEM

#include <compiler/vm/allocateditem.hpp>
#include <unordered_set>

namespace salmon::vm {

	struct AllocatedItem {
		virtual ~AllocatedItem() = 0;

		/**
		 * Return the immedeate roots of the item.
		 * The return value may include items that also contain roots, and need to be searched.
		 **/
		virtual std::unordered_set<AllocatedItem*> get_roots() const {
			return {};
		};

		virtual void print_debug_info() const = 0;
	};
}

#endif

#include <iostream>

#include <vm/box.hpp>

namespace salmon::vm {
	List::List(const Box &itm) :
		itm{itm.bare()}, next{nullptr} {

	}

	void List::get_roots(const std::function<void(AllocatedItem*)>& inserter) const {
		itm.get_roots(inserter);
		if (next != nullptr) {
			inserter(next);
		}
	}

	void List::print_debug_info() const {
		std::cerr << "List " << this << std::endl;
	}

	size_t List::allocated_size() const {
		return sizeof(List);
	}
}

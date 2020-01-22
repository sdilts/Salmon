#include <iostream>

#include <compiler/vm/box.hpp>

namespace salmon::vm {
	List::List(Box &itm) :
		itm{itm}, next{nullptr} {

	}

	std::vector<AllocatedItem*> List::get_roots() const {
		auto children = itm.get_roots();
		if (next != nullptr) {
			children.push_back(next);
		}
		return children;
	}

	void List::print_debug_info() const {
		std::cerr << "List " << this << std::endl;
	}

	size_t List::allocated_size() const {
		return sizeof(List);
	}
}

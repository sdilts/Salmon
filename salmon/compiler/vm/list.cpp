#include <iostream>

#include <compiler/vm/box.hpp>

namespace salmon::vm {
	List::List(Box &itm) :
		itm{itm}, next{nullptr} {

	}

	std::unordered_set<AllocatedItem*> List::get_roots() const {
		auto children = itm.get_roots();
		if (next != nullptr) {
			children.insert(next);
		}
		return children;
	}

	void List::print_debug_info() const {
		std::cerr << "List " << this << std::endl;
	}
}

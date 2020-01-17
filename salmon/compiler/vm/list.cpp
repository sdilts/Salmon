#include <compiler/vm/box.hpp>

namespace salmon::vm {
	List::List(Box &itm) :
		itm{itm} {

	}

	std::unordered_set<AllocatedItem*> List::get_roots() const {
		return { next };
	}
}

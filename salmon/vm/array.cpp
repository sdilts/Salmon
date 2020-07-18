#include <iostream>

#include <vm/box.hpp>

namespace salmon::vm {

	Array::Array(int32_t size) :
		items{} {
		items.reserve(size);
	}

	void Array::get_roots(const std::function<void(AllocatedItem*)>& inserter) const {
		for(const InternalBox &box : items) {
			box.get_roots(inserter);

		}
	}

	void Array::push_back(const Box &item) {
		items.push_back(item.bare());
	}

	std::vector<InternalBox>::iterator Array::begin() {
		return items.begin();
	}

	std::vector<InternalBox>::iterator Array::end() {
		return items.end();
	}

	void Array::print_debug_info() const {
		std::cerr << "Array " << items.size() << " " << this << std::endl;
	}

	size_t Array::allocated_size() const {
		return sizeof(Array);
	}
}

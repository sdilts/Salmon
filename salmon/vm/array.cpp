#include <compare>
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

	InternalBox &Array::operator[](size_t index) {
		return items[index];
	}

	const InternalBox &Array::operator[](size_t index) const {
		return items[index];
	}

	InternalBox &Array::at(size_t index) {
		return items.at(index);
	}

	const InternalBox &Array::at(size_t index) const {
		return items.at(index);
	}

	size_t Array::size() const {
		return items.size();
	}
}

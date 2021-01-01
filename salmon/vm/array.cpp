#include <compare>
#include <iostream>

#include <vm/box.hpp>

namespace salmon::vm {

	Vector::Vector(int32_t size) :
		items{} {
		items.reserve(size);
	}

	void Vector::get_roots(const std::function<void(AllocatedItem*)>& inserter) const {
		for(const InternalBox &box : items) {
			box.get_roots(inserter);

		}
	}

	void Vector::push_back(const Box &item) {
		items.push_back(item.bare());
	}

	void Vector::push_back(const InternalBox item) {
		items.push_back(std::move(item));
	}

	std::vector<InternalBox>::iterator Vector::begin() {
		return items.begin();
	}

	std::vector<InternalBox>::iterator Vector::end() {
		return items.end();
	}

	void Vector::print_debug_info() const {
		std::cerr << "Array " << items.size() << " " << this << std::endl;
	}

	size_t Vector::allocated_size() const {
		return sizeof(Vector);
	}

	InternalBox &Vector::operator[](size_t index) {
		return items[index];
	}

	const InternalBox &Vector::operator[](size_t index) const {
		return items[index];
	}

	InternalBox &Vector::at(size_t index) {
		return items.at(index);
	}

	const InternalBox &Vector::at(size_t index) const {
		return items.at(index);
	}

	size_t Vector::size() const {
		return items.size();
	}
}

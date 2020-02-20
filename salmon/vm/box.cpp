#include <iostream>

#include <cassert>

#include <vm/box.hpp>
#include <vm/type.hpp>

namespace salmon::vm {

	std::vector<AllocatedItem*> InternalBox::get_roots() const {
		return std::visit([](auto &&arg) {
			using T = std::decay_t<decltype(arg)>;
			if constexpr (std::is_pointer<T>::value) {
				std::vector<AllocatedItem*> set = { dynamic_cast<AllocatedItem*>(arg) };
				return set;
			} else {
				static_assert(!std::is_pointer<T>::value);
				std::vector<AllocatedItem*> set;
				return set;
			} }, elem);
	}

	Box::Box(std::unordered_set<Box*> &table) :
		instances{&table} {
	    auto [pos, newly_added] = instances->insert(this);
		assert(newly_added);
	}

	Box::Box(const Box &other) :
		InternalBox(other),
		instances{other.instances}  {
		assert(instances != nullptr);
		auto [pos, newly_added] = instances->insert(this);
		assert(newly_added);
	}

	Box::~Box() {
		assert(instances != nullptr);
		auto pos = instances->find(this);
		assert(pos != instances->end());
		instances->erase(pos);
	}

	Box & Box::operator=(const Box &other) {
		if(this != &other) {
			// ensure the current box is registered:
			instances->insert(this);
			elem = other.elem;
			type = other.type;
			instances = other.instances;
		}
		return *this;
	}
}

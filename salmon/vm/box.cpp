#include <iostream>

#include <util/assert.hpp>
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
	    [[maybe_unused]] auto [pos, newly_added] = instances->insert(this);
		salmon_check(newly_added, "Box wasn't added to the box table");
	}

	Box::Box(const Box &other) :
		InternalBox(other),
		instances{other.instances}  {
		salmon_check(instances != nullptr, "Box constructor expects a valid table");
		[[maybe_unused]] auto [pos, newly_added] = instances->insert(this);
		salmon_check(newly_added, "Box wasn't added to the box table");
	}

	Box::~Box() {
		salmon_check(instances != nullptr, "Box constructor should have a valid table");
		auto pos = instances->find(this);
		salmon_check(pos != instances->end(), "Deleted box isn't in the box table");
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

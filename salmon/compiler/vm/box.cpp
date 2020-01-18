#include <iostream>

#include <cassert>

#include <compiler/vm/box.hpp>
#include <compiler/vm/type.hpp>

namespace salmon::vm {

	std::unordered_set<AllocatedItem*> InternalBox::get_roots() const {
		return std::visit([](auto &&arg) {
			using T = std::decay_t<decltype(arg)>;
			if constexpr (std::is_pointer<T>::value) {
				std::unordered_set<AllocatedItem*> set = { dynamic_cast<AllocatedItem*>(arg) };
				return set;
			} else {
				static_assert(!std::is_pointer<T>::value);
				std::unordered_set<AllocatedItem*> set;
				return set;
			} }, elem);
	}

	Box::Box(std::unordered_map<AllocatedItem*, unsigned int> &table) :
		instances{&table} {
	    auto [pos, newly_added] = instances->insert({this, 1});
		assert(newly_added);
	}

	Box::Box(const Box &other) :
		InternalBox(other),
		instances{other.instances}  {
		assert(instances != nullptr);
		auto [pos, newly_added] = instances->insert({this, 1});
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
			instances->insert({this, 1});
			elem = other.elem;
			type = other.type;
			instances = other.instances;
		}
		return *this;
	}

	void Box::print_debug_info() const {
		std::cerr << this << "Box: " << *type << " " << std::endl;
	};

	std::unordered_set<AllocatedItem*> Box::get_roots() const {
		return InternalBox::get_roots();
	};
}

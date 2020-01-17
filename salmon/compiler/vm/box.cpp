#include <cassert>

#include <compiler/vm/box.hpp>


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
		auto [pos, newly_added] = instances->insert({this, 1});
		assert(newly_added);
	}

	Box::~Box() {
		auto pos = instances->find(this);
		assert(pos != instances->end());
		instances->erase(pos);
	}

	std::unordered_set<AllocatedItem*> Box::get_roots() const {
		return InternalBox::get_roots();
	};
}

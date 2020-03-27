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
}

#include <iostream>

#include <util/assert.hpp>
#include <vm/box.hpp>
#include <vm/type.hpp>
#include <util/assert.hpp>

namespace salmon::vm {

	std::vector<AllocatedItem*> InternalBox::get_roots() const {
		salmon_check(type != nullptr, "Type is null");
		std::vector<AllocatedItem*> set = { type };

		return std::visit([&set](auto &&arg) {
			using T = std::decay_t<decltype(arg)>;
			if constexpr (std::is_pointer<T>::value) {
				set.push_back(arg);
				return set;
			} else {
				static_assert(!std::is_pointer<T>::value);
				return set;
			} }, elem);
	}
}

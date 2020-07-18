#include <iostream>

#include <util/assert.hpp>
#include <vm/box.hpp>
#include <vm/type.hpp>
#include <util/assert.hpp>

namespace salmon::vm {

	void InternalBox::get_roots(const std::function<void(AllocatedItem*)>& inserter) const {
		salmon_check(type != nullptr, "Type is null");
		inserter(type);

		std::visit([&inserter](auto &&arg) {
			using T = std::decay_t<decltype(arg)>;
			if constexpr (std::is_pointer<T>::value) {
				inserter(arg);
			} else {
				static_assert(!std::is_pointer<T>::value);
			} }, elem);
	}

	std::ostream& operator<<(std::ostream &os, const Empty &) {
		return os << "Empty";
	}
}

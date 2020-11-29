#include <iostream>

#include <util/assert.hpp>
#include <vm/box.hpp>
#include <vm/type.hpp>
#include <util/assert.hpp>

namespace salmon::vm {

	std::partial_ordering operator<=>(const InternalBox &lhs, const InternalBox &rhs) {
		bool lhs_absent = lhs.elem.valueless_by_exception();
		bool rhs_absent = rhs.elem.valueless_by_exception();
		if(lhs_absent && rhs_absent) {
			return std::strong_ordering::equal;
		} else if(lhs_absent) {
			return std::strong_ordering::less;
		} else if(rhs_absent) {
			return std::strong_ordering::greater;
		} else {
			// both exist, compare either by value or by pointer depending on what it is:
			return std::visit([&rhs](auto &&arg) -> std::partial_ordering {
				using T = std::decay_t<decltype(arg)>;
				if constexpr (std::is_pointer<T>::value) {
					return *arg <=> *std::get<T>(rhs.elem);
				} else {
					static_assert(std::is_fundamental<T>::value || std::is_same<T,Empty>::value);
					T value = std::get<T>(rhs.elem);
					return arg <=> value;
				}
			}, lhs.elem);
	}
}

	bool operator==(const InternalBox &lhs, const InternalBox &rhs) {
		return lhs.elem == rhs.elem;
	}

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

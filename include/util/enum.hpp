#pragma once
#include <type_traits>

namespace salmon {
	template<typename T>
	[[nodiscard]] auto constexpr as(const T value)
		-> typename std::underlying_type<T>::type
	{
		return static_cast<typename std::underlying_type<T>::type>(value);
	}
}

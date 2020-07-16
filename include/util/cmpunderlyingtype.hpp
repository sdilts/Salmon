#pragma once

#include <vm/vm_ptr.hpp>

namespace salmon {
	template<typename T>
	struct cmpUnderlyingType {
		bool operator()(const T* a,
						const T* b) const {
			return *a < *b;
		}

		bool operator()(const vm::vm_ptr<T> &a, const vm::vm_ptr<T> &b) const {
			return *a < *b;
		}
	};
}

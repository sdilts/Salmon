#ifndef SALMON_COMPILER_STRING
#define SALMON_COMPILER_STRING

#include <string>

#include <compiler/vm/allocateditem.hpp>

namespace salmon::compiler {
	struct StaticString : public salmon::vm::AllocatedItem {
		const std::string contents;
		StaticString(const std::string &str);
		StaticString() = delete;
		~StaticString() = default;

	};
}

#endif

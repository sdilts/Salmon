#ifndef SALMON_COMPILER_BOX
#define SALMON_COMPILER_BOX

#include <variant>

#include <compiler/vm/allocateditem.hpp>
#include <compiler/symbol.hpp>

namespace salmon::compiler {
	struct Box : public salmon::vm::AllocatedItem {
		~Box();
		struct Type *type;
		std::variant<int32_t, double,void*> elem;
	};
}

#endif

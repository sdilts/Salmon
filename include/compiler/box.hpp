#ifndef SALMON_COMPILER_BOX
#define SALMON_COMPILER_BOX

#include <variant>

#include <compiler/vm/allocateditem.hpp>
#include <compiler/vm/vm_ptr.hpp>
#include <compiler/symbol.hpp>
#include <compiler/string.hpp>

namespace salmon::compiler {
	struct Box : public salmon::vm::AllocatedItem {
		~Box();
		struct Type *type;
		std::variant<int32_t, double,salmon::vm::vm_ptr<Symbol>,
			salmon::vm::vm_ptr<StaticString>> elem;
	};
}

#endif

#ifndef SALMON_COMPILER_BOX
#define SALMON_COMPILER_BOX

#include <variant>

#include <compiler/vm/allocateditem.hpp>
#include <compiler/vm/vm_ptr.hpp>
#include <compiler/vm/symbol.hpp>
#include <compiler/vm/string.hpp>

namespace salmon::vm {
 	struct Box {
		struct Type *type;
		std::variant<int32_t, float,salmon::vm::vm_ptr<Symbol>,
			salmon::vm::vm_ptr<StaticString>> elem;
	};
}

#endif

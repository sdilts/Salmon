#ifndef SALMON_COMPILER_BOX
#define SALMON_COMPILER_BOX

#include <variant>

namespace salmon::compiler {
	struct Box {
		struct Type *type;
		std::variant<int32_t, double,void*> elem;
		//! tag used for garbage collection
		unsigned char tag;
	};
}

#endif

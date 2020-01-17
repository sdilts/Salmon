#ifndef SALMON_COMPILER_BOX
#define SALMON_COMPILER_BOX

#include <variant>

#include <compiler/vm/allocateditem.hpp>
#include <compiler/vm/vm_ptr.hpp>
#include <compiler/vm/symbol.hpp>
#include <compiler/vm/string.hpp>

namespace salmon::vm {

	struct List;

 	struct Box {
		struct Type *type;
		std::variant<int32_t,
					 float,
					 vm_ptr<Symbol>,
					 vm_ptr<StaticString>,
					 vm_ptr<List>> elem;
	};

	struct List : public AllocatedItem {
		List(Box &itm);
		List() = delete;
		~List() = default;

		Box itm;
		List *next;

		std::unordered_set<AllocatedItem*> get_roots() const override;
	};
}

#endif

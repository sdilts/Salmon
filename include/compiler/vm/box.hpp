#ifndef SALMON_COMPILER_BOX
#define SALMON_COMPILER_BOX

#include <variant>

#include <compiler/vm/allocateditem.hpp>
#include <compiler/vm/vm_ptr.hpp>
#include <compiler/vm/symbol.hpp>
#include <compiler/vm/string.hpp>

namespace salmon::vm {

	struct List;

	struct InternalBox {
		struct Type *type;
		std::variant<int32_t,
					 float,
					 Symbol*,
					 List*,
					 StaticString*>
					 elem;
		std::unordered_set<AllocatedItem*> get_roots() const;
	};

	struct Box : public InternalBox, public AllocatedItem {

		Box(std::unordered_map<AllocatedItem*, unsigned int> &table);
		Box(const Box&);
		Box(Box&&) = default;
		Box() = delete;

		~Box();

		std::unordered_set<AllocatedItem*> get_roots() const override;

	private:
		std::unordered_map<AllocatedItem*,unsigned int> *instances;
	};

	struct List : public AllocatedItem {
		List(Box &itm);
		List() = delete;
		~List() = default;

		InternalBox itm;
		List *next;

		std::unordered_set<AllocatedItem*> get_roots() const override;
	};
}

#endif

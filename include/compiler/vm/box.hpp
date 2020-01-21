#ifndef SALMON_COMPILER_BOX
#define SALMON_COMPILER_BOX

#include <variant>

#include <compiler/vm/allocateditem.hpp>
#include <compiler/vm/vm_ptr.hpp>
#include <compiler/vm/symbol.hpp>
#include <compiler/vm/string.hpp>
#include <compiler/vm/empty.hpp>

namespace salmon::vm {

	struct List;

	struct InternalBox {
		struct Type *type;
		std::variant<int32_t,
					 float,
					 bool,
					 Empty,
					 Symbol*,
					 List*,
					 StaticString*>
					 elem;
		std::vector<AllocatedItem*> get_roots() const;
	};

	struct Box : public InternalBox, public AllocatedItem {

		Box(std::unordered_map<AllocatedItem*, unsigned int> &table);
		Box(const Box&);
		Box() = delete;
		~Box();

		Box &operator=(const Box &);

		void print_debug_info() const override;
		std::vector<AllocatedItem*> get_roots() const;

	private:
		std::unordered_set<Box*> *instances;
	};

	struct List : public AllocatedItem {
		List(Box &itm);
		List() = delete;
		~List() = default;

		InternalBox itm;
		List *next;

		void print_debug_info() const override;
		std::vector<AllocatedItem*> get_roots() const override;
	};
}

#endif

#ifndef SALMON_COMPILER_BOX
#define SALMON_COMPILER_BOX

#include <variant>
#include <unordered_set>

#include <vm/allocateditem.hpp>
#include <vm/vm_ptr.hpp>
#include <vm/symbol.hpp>
#include <vm/string.hpp>
#include <vm/empty.hpp>

namespace salmon::vm {

	struct List;
	struct Array;

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

	struct Box : public InternalBox {

		Box(std::unordered_set<Box*> &table);
		Box(const Box&);
		Box(Box&& other) = default;
		Box() = delete;
		~Box();

		Box &operator=(const Box &);

	private:
		std::unordered_set<Box*> *instances;
	};

	struct Array : public AllocatedItem {
		Array(int32_t size);

		std::vector<InternalBox> items;

		void print_debug_info() const override;
		std::vector<AllocatedItem*> get_roots() const override;
		size_t allocated_size() const override;
	};

	struct List : public AllocatedItem {
		List(Box &itm);
		List() = delete;
		~List() = default;

		InternalBox itm;
		List *next;

		void print_debug_info() const override;
		std::vector<AllocatedItem*> get_roots() const override;
		size_t allocated_size() const override;
	};
}

#endif

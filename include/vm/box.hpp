#ifndef SALMON_COMPILER_BOX
#define SALMON_COMPILER_BOX

#include <variant>
#include <unordered_set>

#include <vm/allocateditem.hpp>
#include <vm/vm_ptr.hpp>
#include <vm/symbol.hpp>
#include <vm/string.hpp>
#include <vm/empty.hpp>
#include <vm/type.hpp>

namespace salmon::vm {

	struct List;
	struct Array;

	using BoxVariant = std::variant<int32_t,
									float,
									bool,
									Empty,
									Symbol*,
									List*,
									StaticString*>;

	struct InternalBox {
		TypePtr type;
		BoxVariant elem;
		std::vector<AllocatedItem*> get_roots() const;
	};

	struct Box : public InternalBox {

		template<typename T>
		Box(const vm_ptr<T> &elem_ptr) :
			smart_ptr{elem_ptr} {
			if(elem_ptr) {
				elem = elem_ptr.get();
			}
		}
		Box(const Box&) = default;
		Box(Box &&) = default;
		Box() = delete;

		Box &operator=(const Box &) = default;

		template<typename T>
		void set_value(const vm_ptr<T> &value) {
			elem = value.get();
			smart_ptr = &*value;
		}

		template<typename T>
		void set_value(T value) {
			elem = value;
			smart_ptr = nullptr;
		}

	private:
		vm_ptr<AllocatedItem> smart_ptr;
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

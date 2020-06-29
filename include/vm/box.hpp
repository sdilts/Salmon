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
#include <util/assert.hpp>

namespace salmon::vm {

	struct List;
	struct Array;

	using BoxVariant = std::variant<int32_t,
					double,
					bool,
					Empty,
					Array*,
					Symbol*,
					List*,
					StaticString*>;

	struct InternalBox {
		Type* type;
		BoxVariant elem;
	        void get_roots(const std::function<void(AllocatedItem*)>&) const;
	};

	struct Box : private InternalBox {

		template<typename T>
		Box(const vm_ptr<T> &elem_ptr, const vm_ptr<Type> type) :
			elem_ptr{elem_ptr},
			type_ptr{type} {
			this->type = type.get();
			type_ptr = type;
			if(elem_ptr) {
				elem = elem_ptr.get();
			}
			salmon_check(this->type != nullptr, "Type shouldn't be null");
		}

		template <typename T>
		Box(T scalar, const vm_ptr<Type> type) :
			elem_ptr{type},
			type_ptr{type} {
			static_assert(std::is_same<vm::Empty, T>::value
						  || (!std::is_class<T>::value && !std::is_pointer<T>::value));
			elem_ptr = nullptr;
			elem = scalar;
			this->type = type.get();
			type_ptr = type;
			salmon_check(this->type != nullptr, "Type shouldn't be null");
		}

		Box(const Box&) = default;
		Box(Box &&) = default;
		Box() = delete;

		Box &operator=(const Box &) = default;

		template<typename T>
		void set_value(const vm_ptr<T> &value) {
			elem = value.get();
			elem_ptr = &*value;
		}

		template<typename T>
		void set_value(T value) {
			elem = value;
			elem_ptr = nullptr;
		}

		const vm_ptr<Type> elem_type() {
			return type_ptr;
		}

		const BoxVariant &value() const {
			return elem;
		}

		// these need to access the InternalBox:
		friend struct Array;
		friend struct List;

	private:
		vm_ptr<AllocatedItem> elem_ptr;
		vm_ptr<Type> type_ptr;
	};

	struct Array : public AllocatedItem {
		Array(int32_t size);

		void push_back(const Box &item);
		void push_back(Box&& item);

		void print_debug_info() const override;
		void get_roots(const std::function<void(AllocatedItem*)>&) const override;
		size_t allocated_size() const override;

	private:
		std::vector<InternalBox> items;
	};

	struct List : public AllocatedItem {
		List(Box &itm);
		List() = delete;
		~List() = default;

		InternalBox itm;
		List *next;

		void print_debug_info() const override;
		void get_roots(const std::function<void(AllocatedItem*)>&) const override;
		size_t allocated_size() const override;
	};
}

#endif

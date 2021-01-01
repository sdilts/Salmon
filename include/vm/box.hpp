#ifndef SALMON_COMPILER_BOX
#define SALMON_COMPILER_BOX

#include <compare>
#include <type_traits>
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
	struct Vector;

	using BoxVariant = std::variant<int32_t,
					double,
					bool,
					Empty,
					Vector*,
					Symbol*,
					List*,
					StaticString*>;

	struct InternalBox {
		Type* type;
		BoxVariant elem;
	        void get_roots(const std::function<void(AllocatedItem*)>&) const;
	};
	std::partial_ordering operator<=>(const InternalBox &lhs, const InternalBox &rhs);
	bool operator==(const InternalBox &lhs, const InternalBox &rhs);

	struct Box {

		template<typename T>
		Box(const vm_ptr<T> &elem_ptr, const vm_ptr<Type> &type) :
			elem_ptr{elem_ptr},
			type_ptr{type} {
			internal.type = type.get();
			if(elem_ptr) {
				internal.elem = elem_ptr.get();
			}
			salmon_check(internal.type != nullptr, "Type shouldn't be null");
		}

		template<typename T>
		Box(const vm_ptr<T> &&elem_ptr, const vm_ptr<Type> &type) :
			elem_ptr{elem_ptr},
			type_ptr{type} {
			internal.type = type.get();
			if(elem_ptr) {
				internal.elem = elem_ptr.get();
			}
			salmon_check(internal.type != nullptr, "Type shouldn't be null");
		}

		Box(InternalBox internal, const vm_ptr<AllocatedItem> &seed) :
			internal{internal},
			elem_ptr{seed},
			type_ptr{seed.from(internal.type)} {
			std::visit([this](auto &&arg) {
				using T = std::decay<decltype(arg)>;
				if constexpr (std::is_pointer<T>::value) {
					this->elem_ptr = arg;
				}
			}, this->internal.elem);
		}

		template <typename T>
		Box(T scalar, const vm_ptr<Type> &type) :
			elem_ptr{type},
			type_ptr{type} {
			static_assert(std::is_same<vm::Empty, T>::value
						  || (!std::is_class<T>::value && !std::is_pointer<T>::value));
			elem_ptr = nullptr;
			internal.elem = scalar;
			internal.type = type.get();
			type_ptr = type;
			salmon_check(internal.type != nullptr, "Type shouldn't be null");
		}

		Box(const Box&) = default;
		Box(Box &&) = default;
		Box() = delete;

		Box &operator=(const Box &) = default;

		auto operator<=>(const Box& other) const {
			return internal <=> other.internal;
		}

		template<typename T>
		void set_value(const vm_ptr<T> &value) {
			internal.elem = value.get();
			elem_ptr = &*value;
		}

		template<typename T>
		void set_value(T value) {
			internal.elem = value;
			elem_ptr = nullptr;
		}

		vm_ptr<Type> elem_type() const {
			return type_ptr;
		}

		const BoxVariant &value() const {
			return internal.elem;
		}

		const InternalBox &bare() const {
			return internal;
		}

	private:
		InternalBox internal;
		vm_ptr<AllocatedItem> elem_ptr;
		vm_ptr<Type> type_ptr;
	};

	struct Vector : public AllocatedItem {
		Vector(int32_t size);

		void push_back(const Box &item);
		void push_back(const InternalBox item);

		std::vector<InternalBox>::iterator begin();
		std::vector<InternalBox>::iterator end();

		void print_debug_info() const override;
		void get_roots(const std::function<void(AllocatedItem*)>&) const override;
		size_t allocated_size() const override;

		const InternalBox &operator[](size_t) const;
		InternalBox &operator[](size_t);

		InternalBox &at(size_t index);
		const InternalBox &at(size_t) const;

		size_t size() const;

		auto operator<=>(const Vector &other) const {
			return other.items <=> this->items;
		}
		bool operator==(const Vector &other) const {
			return this->items == other.items;
		}
	private:
		std::vector<InternalBox> items;
	};

	struct List : public AllocatedItem {
		List(const Box &itm);
		List() = delete;
		~List() = default;

		InternalBox itm;
		List *next;

		void print_debug_info() const override;
		void get_roots(const std::function<void(AllocatedItem*)>&) const override;
		size_t allocated_size() const override;

		bool operator==(const List &other) const {
			if(itm == other.itm) {
				return next == other.next;
			} else return false;
		}

		std::partial_ordering operator<=>(const List &other) const {
			const auto compare =  itm <=> other.itm;
			if(compare == 0) {
				if(next != nullptr && other.next != nullptr) {
					return *this->next <=> *other.next;
				} else if(next != nullptr) {
					return std::strong_ordering::greater;
				} else if(other.next != nullptr) {
					return std::strong_ordering::less;
				} else {
					std::cout << "Items equal\n";
					return std::strong_ordering::equal;
				}
			} else return compare;
		}
	};
}

#endif

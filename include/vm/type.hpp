#ifndef SALMON_CORE_TYPE
#define SALMON_CORE_TYPE

#include <map>
#include <set>
#include <string>
#include <variant>
#include <optional>
#include <ostream>

#include <stdlib.h>

#include <vm/symbol.hpp>
#include <vm/vm_ptr.hpp>
#include <vm/memory.hpp>
#include <vm/typespec.hpp>
#include <util/cmpunderlyingtype.hpp>

namespace salmon::vm {

	struct TypeInterface {

		virtual ~TypeInterface();
		/**
		 * Return the size of the type in bytes.
		 *
		 * Error if the type isn't concrete.
		 **/
		virtual size_t size() const = 0;
		//! Check if the type can be instantiated
		virtual bool concrete() const = 0;

		virtual void get_roots(const std::function<void(AllocatedItem*)>&) const = 0;
	};

	struct PrimitiveType : TypeInterface {
		PrimitiveType(const vm_ptr<Symbol> &name, const std::string &documentation,
					  const size_t size);

		~PrimitiveType();

		Symbol *name;
		const std::string documentation;
		const size_t num_bytes;

		bool equivalent_to(const PrimitiveType &) const {
			return false;
		}

		size_t size() const override;
		bool concrete() const override;
		void get_roots(const std::function<void(AllocatedItem*)>&) const override;

		bool operator==(const PrimitiveType &other) const;
		bool operator!=(const PrimitiveType &other) const;
		bool operator>(const PrimitiveType &other) const;
		bool operator<(const PrimitiveType &other) const;
	};

	std::ostream &operator<<(std::ostream &out, const PrimitiveType &type);

	struct FunctionType : TypeInterface {
		FunctionType(const TypeSpecification &ret_spec, const TypeSpecification &arg_spec);

		~FunctionType();

		//! Return the arity of the function.
		int arity() const;
		//! Match the return types and the arguments. Order is return types, then args.
		bool match(const std::vector<vm_ptr<Type>> &type_list) const;
		bool match(const FunctionType &other) const;
		//! Just match the arguments
		bool match_args(const std::vector<vm_ptr<Type>> &type_list) const;

		const std::vector<Type*> arg_types() const;
		bool equivalent_to(const FunctionType &other) const;

		size_t size() const override;
		bool concrete() const override;

		void get_roots(const std::function<void(AllocatedItem*)>&) const override;

		bool operator==(const FunctionType &other) const;
		bool operator!=(const FunctionType &other) const;
		bool operator>(const FunctionType &other) const;
		bool operator<(const FunctionType &other) const;

		friend std::ostream &operator<<(std::ostream &out, const FunctionType &fn);
	private:
		TypeSpecification arg_spec;
		TypeSpecification ret_spec;
	};

	struct Type : AllocatedItem {
		using TypeVar = std::variant<PrimitiveType,
						   // ProductType,
						   // SumType,
					     FunctionType>;

		Type(const TypeVar &type);

		const TypeVar type;

		size_t size() const;
		bool concrete() const;

		void print_debug_info() const override;
		size_t allocated_size() const override;
		void get_roots(const std::function<void(AllocatedItem*)>&) const override;

		bool operator==(const Type &other) const;
		bool operator!=(const Type &other) const;
		bool operator>(const Type &other) const;
		bool operator<(const Type &other) const;

		bool equivalent_to(const Type &other) const;
	};
	std::ostream &operator<<(std::ostream &out, const Type &type);
	using TypePtr = vm_ptr<Type>;

	class TypeTable {
	public:
		TypeTable() = delete;
		TypeTable(MemoryManager &mem_manager);

		std::optional<TypePtr> get_named(const vm_ptr<Symbol> &name) const;
		TypePtr get_fn_type(const TypeSpecification &arg_types, const TypeSpecification &ret_types);

		bool make_alias(const vm_ptr<Symbol> &alias, TypePtr &type);

		TypePtr make_primitive(const vm_ptr<Symbol> &name, const std::string &doc, std::size_t size);

	private:
		MemoryManager &mem_manager;

		std::unordered_map<vm_ptr<Symbol>, vm_ptr<Type>> named_types;
		std::set<TypePtr, cmpUnderlyingType<Type>> functions;
	};
}

#endif

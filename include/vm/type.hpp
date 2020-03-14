#ifndef SALMON_CORE_TYPE
#define SALMON_CORE_TYPE

#include <map>
#include <set>
#include <string>
#include <variant>
#include <optional>
#include <ostream>
#include <memory>

#include <stdlib.h>

#include <vm/symbol.hpp>
#include <vm/vm_ptr.hpp>
#include <optional>

namespace salmon::vm {
	struct Type;

	class TypeSpecification {
	public:
		using ItemMask = std::variant<vm_ptr<Symbol>, std::shared_ptr<const Type>>;

		TypeSpecification(const std::vector<TypeSpecification::ItemMask> &types);

		//! Check if the given types conform to the specifictation
		bool matches(const std::vector<std::shared_ptr<const Type>> &type_list) const;

		std::optional<std::map<vm_ptr<Symbol>,std::shared_ptr<const Type>>>
		match_symbols(const std::vector<std::shared_ptr<const Type>> &type_list) const;

		std::string str() const;

		//! checks to see if this specification can be synthesised
		bool concrete() const;

		//! Get the number of types:
		int num_types() const;

		bool operator==(const TypeSpecification &other) const;
		bool operator!=(const TypeSpecification &other) const;
		bool operator>(const TypeSpecification &other) const;
		bool operator<(const TypeSpecification &other) const;

		friend std::ostream &operator<<(std::ostream &out, const TypeSpecification& spec);
	private:
		const std::vector<ItemMask> specification;

		//! bare types are specified with specific symbol, i.e. A
		const std::map<vm_ptr<Symbol>,std::vector<size_t>> unspecified_types;
		//! Indexes of types that must be the same
		const std::vector<size_t> concrete_types;
		// TODO: allow unresolved types in type specifications
		const bool is_concrete;
	};

	// struct InstanceType {

	// 	InstanceType(bool constant, bool is_static);

	// 	Type *actual;

	// 	bool is_constant() const;
	// 	bool is_static() const;
	// private:
	// 	uint8_t properties;

	// 	static const uint8_t CONSTANT_MASK = 1;
	// 	static const uint8_t STATIC_MASK = 1 << 1;
	// };

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
	};

	struct PrimitiveType : TypeInterface {
		PrimitiveType(const vm_ptr<Symbol> &name, const std::string &documentation,
					  const size_t size);

		~PrimitiveType();

		const vm_ptr<Symbol> name;
		const std::string documentation;
		const size_t num_bytes;

		size_t size() const;
		bool concrete() const;

		bool operator==(const PrimitiveType &other) const;
		bool operator!=(const PrimitiveType &other) const;
		bool operator>(const PrimitiveType &other) const;
		bool operator<(const PrimitiveType &other) const;
	};
	std::ostream &operator<<(std::ostream &out, const PrimitiveType &type);

	struct FunctionType : TypeInterface {
		FunctionType(const TypeSpecification &ret_spec, const TypeSpecification &arg_spec);
		FunctionType(const std::vector<TypeSpecification::ItemMask> &ret_types,
					 const std::vector<TypeSpecification::ItemMask> &arg_types);
		~FunctionType();

		//! Return the arity of the function.
		int arity() const;
		//! Match the return types and the arguments. Order is return types, then args.
		bool match(const std::vector<std::shared_ptr<const Type>> &type_list) const;
		//! Just match the arguments
		bool match_args(const std::vector<std::shared_ptr<const Type>> &type_list) const;


		size_t size() const;
		bool concrete() const;

		bool operator==(const FunctionType &other) const;
		bool operator!=(const FunctionType &other) const;
		bool operator>(const FunctionType &other) const;
		bool operator<(const FunctionType &other) const;

		friend std::ostream &operator<<(std::ostream &out, const FunctionType &fn);
	private:
		TypeSpecification arg_spec;
		TypeSpecification ret_spec;
	};

	struct Type {
		using TypeVar = std::variant<PrimitiveType,
						   // ProductType,
						   // SumType,
									 FunctionType>;

		Type(const TypeVar &type);

		const TypeVar type;

		size_t size() const;
		bool concrete() const;

		bool operator==(const Type &other) const;
		bool operator!=(const Type &other) const;
		bool operator>(const Type &other) const;
		bool operator<(const Type &other) const;
	};
	std::ostream &operator<<(std::ostream &out, const Type &type);
	using TypePtr = std::shared_ptr<const Type>;

	class TypeTable {
	public:
		std::optional<TypePtr> get_named(const vm_ptr<Symbol> &name) const;
		TypePtr get_fn_type(const TypeSpecification &arg_types, const TypeSpecification &ret_types);

		bool make_alias(const vm_ptr<Symbol> &alias, TypePtr &type);

		TypePtr make_primitive(const vm_ptr<Symbol> &name, const std::string &doc, std::size_t size);

	private:
		struct cmpUnderlyingType {
			bool operator()(const std::shared_ptr<const Type>& a,
							const std::shared_ptr<const Type>& b) const {
				return *a < *b;
			}
		};
		std::unordered_map<vm_ptr<Symbol>, TypePtr> named_types;
		std::set<TypePtr, cmpUnderlyingType> functions;
	};
}

#endif

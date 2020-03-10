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

#include <vm/box.hpp>
#include <vm/symbol.hpp>
#include <vm/vm_ptr.hpp>

namespace salmon::vm {

	class TypeSpecification {
	public:
		using ItemMask = std::variant<vm_ptr<Symbol>, std::shared_ptr<const Type>>;

		TypeSpecification(const std::vector<TypeSpecification::ItemMask> &types);

		//! Check if the given types conform to the specifictation
		bool matches(const std::vector<std::shared_ptr<const Type>> &type_list) const;
		std::string str() const;

		//! checks to see if this specification can be synthesised
		bool concrete() const;
		// TODO: wrap return val with std::shared_ptr or a custom smart pointer
		/**
		 * Return a TypeSpecification with the unrealized types filled in.
		 *
		 * You can partially realize the specification by using std::nullopt where the type
		 * isn't yet known.
		 */
		TypeSpecification *concretize(const std::vector<std::optional<Type*>> type);

		bool operator>(const TypeSpecification &other) const;
		bool operator<(const TypeSpecification &other) const;
		bool operator==(const TypeSpecification &other) const;
		bool operator!=(const TypeSpecification &other) const;

		std::ostream &operator<<(std::ostream &out) const;

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
		//! Return a type that can be instantiated
		virtual std::shared_ptr<const Type>
		concretize(const std::vector<std::optional<Type*>> type) const = 0;
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
		std::shared_ptr<const Type> concretize(const std::vector<std::optional<Type*>> type) const;

		bool operator==(const PrimitiveType &other) const;
		bool operator!=(const PrimitiveType &other) const;
	};

	// // TODO: Can the record types ProductType and SumType be merged/have a common interface?
	// struct ProductType : TypeInterface {
	// 	~ProductType();

	// 	const vm_ptr<Symbol> name;
	// 	std::string documentation;

	// 	std::vector<FieldEntry> fields;

	// 	size_t size() const;
	// 	bool concrete() const;
	// 	Type *concretize(const std::vector<st::optional<Type*>> type) const;
	// };

	// struct SumType : TypeInterface {
	// 	~SumType();

	// 	const vm_ptr<Symbol> name;
	// 	std::string documentation;

	// 	std::vector<FieldEntry> fields;

	// 	size_t size() const;
	// 	bool concrete() const;
	// 	Type *concretize(const std::vector<st::optional<Type*>> type) const;
	// };

	// struct FunctionType : TypeInterface {
	// 	~FunctionType();

	// 	std::vector<InstanceType> arg_types;
	// 	std::vector<InstanceType> ret_types;

	// 	/**
	// 	 * Return the arity of the function.
	// 	 **/
	// 	int arity();

	// 	size_t size() const;
	// 	bool concrete() const;
	// 	Type *concretize(const std::vector<std::optional<Type*>> type) const;
	// };

	struct Type {
		using TypeVar = std::variant<PrimitiveType //,
						   // ProductType,
						   // SumType,
						   // FunctionType
						   >;

		Type(const TypeVar &type);

		const TypeVar type;

		size_t size() const;
		bool concrete() const;
		std::shared_ptr<const Type> concretize(const std::vector<std::optional<Type*>> type) const;

		bool operator==(const Type &other) const;
		bool operator!=(const Type &other) const;
	};
	std::ostream &operator<<(std::ostream &out, const Type &type);

	// class TypeTable {
	// public:
	// 	std::optional<Type*> get_named(const vm_ptr<Symbol> &name);
	// 	Type *get_fn(std::vector<Type*> arg_types, std::vector<Type*> ret_types);

	// 	void make_alias(const vm_ptr<Symbol> &alias, Type *type);

	// 	Type *make_primitive(const vm_ptr<Symbol> &name);
	// 	Type *make_primitive(const vm_ptr<Symbol> &name, std::optional<std::string> &doc);

	// 	Type *make_sumtype(const vm_ptr<Symbol> &name, std::vector<FieldEntry> fields);
	// 	Type *make_sumtype(const vm_ptr<Symbol> &name, std::vector<FieldEntry> fields,
	// 					   std::optional<std::string> doc);

	// 	Type *make_producttype(const vm_ptr<Symbol> &name, std::vector<FieldEntry> fields);
	// 	Type *make_producttype(const vm_ptr<Symbol> &name, std::vector<FieldEntry> fields,
	// 					   std::optional<std::string> doc);

	// 	class FuncSpec {
	// 		TypeSpecification args;
	// 		TypeSpecification return_values;

	// 		bool operator>(const FuncSpec &other);
	// 		bool operator<(const FuncSpec &other);
	// 		bool operator==(const TypeMask &other);
	// 		bool operator!=(const TypeMask &other);
	// 	};
	// private:
	// 	std::unordered_map<vm_ptr<Symbol>, Type> named_types;
	// 	std::map<FuncSpec,FunctionType> functions;
	// };
}

#endif

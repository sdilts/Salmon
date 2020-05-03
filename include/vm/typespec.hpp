#ifndef SALMON_VM_TYPESPEC
#define SALMON_VM_TYPESPEC

#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <memory>
#include <ostream>

#include <vm/vm_ptr.hpp>
#include <vm/symbol.hpp>

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

	class SpecBuilder {

	public:
		//! add a paramenter to the specification
		void add_parameter(vm_ptr<Symbol> param);
		//! add a type to the specification
		void add_type(std::shared_ptr<Type> type);

		//! the the spec that you have built
		TypeSpecification get();

	private:
		std::vector<TypeSpecification::ItemMask> types;
	};
}

#endif

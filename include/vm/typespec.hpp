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

	struct VariableProperties {
		VariableProperties(bool constant, bool is_static);

		bool is_constant() const;
		bool is_static() const;

		std::ostream &pretty_print(std::ostream &out) const;

		bool operator==(const VariableProperties &other) const;
		bool operator!=(const VariableProperties &other) const;
		bool operator>(const VariableProperties &other) const;
		bool operator<(const VariableProperties &other) const;
		bool operator>=(const VariableProperties &other) const;
		bool operator<=(const VariableProperties &other) const;
	private:
		uint8_t properties;

		static const uint8_t CONSTANT_MASK = 1;
		static const uint8_t STATIC_MASK = 1 << 1;
	};

	class TypeSpecification {
	public:

		//! Check if the given types conform to the specifictation
		bool matches(const std::vector<vm_ptr<Type>> &type_list) const;
		bool matches(const TypeSpecification &other) const;

		std::optional<std::map<vm_ptr<Symbol>,vm_ptr<Type>>>
		match_symbols(const std::vector<vm_ptr<Type>> &type_list) const;

		std::string str() const;

		//! checks to see if this specification can be synthesised
		bool concrete() const;

		//! Get the number of types:
		size_t size() const;

		void get_roots(const std::function<void(AllocatedItem*)>&) const;

		bool equivalentTo(const TypeSpecification &other) const;

		bool operator==(const TypeSpecification &other) const;
		bool operator!=(const TypeSpecification &other) const;
		bool operator>(const TypeSpecification &other) const;
		bool operator<(const TypeSpecification &other) const;

		friend std::ostream &operator<<(std::ostream &out, const TypeSpecification& spec);
	private:
		friend class SpecBuilder;
		TypeSpecification(std::map<vm_ptr<Symbol>, std::vector<size_t>> &params,
				  std::vector<std::pair<vm_ptr<Type>, size_t>> &concrete_types,
				  std::vector<VariableProperties> &properties);
		// TODO: investigate how efficent this storage config is:

		//! bare types are specified with specific symbol, i.e. A
		const std::map<Symbol*,std::vector<size_t>> parameters;
		// TODO: allow non-concrete types in type specifications
		//! Indexes of types that must be the same
		const std::vector<std::pair<Type*,size_t>> concrete_types;
		const std::vector<VariableProperties> properties;

		const bool is_concrete;
	};

	class SpecBuilder {

	public:
		//! add a paramenter to the specification
		void add_parameter(vm_ptr<Symbol> &param);
		void add_parameter(vm_ptr<Symbol> &param, bool constant, bool is_static);
		//! add a type to the specification
		void add_type(vm_ptr<Type> &type);
		void add_type(vm_ptr<Type> &type, bool constant, bool is_static);

		//! the the spec that you have built
		TypeSpecification build();

	private:
		size_t num_elems = 0;

		//! bare types are specified with specific symbol, i.e. A
		std::map<vm_ptr<Symbol>,std::vector<size_t>> parameters;
		//! Indexes of types that must be the same
		std::vector<std::pair<vm_ptr<Type>,size_t>> concrete_types;
		// TODO: allow non-concrete types in type specifications
		std::vector<VariableProperties> properties;
	};
}

#endif

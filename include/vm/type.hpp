#ifndef SALMON_CORE_TYPE
#define SALMON_CORE_TYPE

#include <map>
#include <set>
#include <string>
#include <variant>
#include <optional>
#include <ostream>

#include <stdlib.h>

#include <vm/box.hpp>
#include <vm/symbol.hpp>
#include <vm/vm_ptr.hpp>

namespace salmon::vm {

	/**
	 * An instance of this struct represents the cannonical metadata
	 * for a salmon type. It holds the name of the type, as well as any
	 * possible attributes or transformations that can be applied to an object
	 * of the type. This metadata applies to properties of an instance of an object,
	 * i.e. the actual length of an array type.
	 **/
	struct Type {
		vm_ptr<Symbol> name;
		//! the potential metadata for an instance of the type with documentation
		const std::map<vm_ptr<Symbol>, std::pair<const Type *const, std::string>> meta;
		std::string documentation;
	};

	std::ostream &operator<<(std::ostream &stream, const Type &type);

	/**
	 * An instance of this struct represents the particular metadata associated with
	 * a variable at compile time, as outlined by the actual type field of the structure.
	 **/
	struct TypeInstance {
		const  Type &actual_type;
		const std::map<std::string, Box> actual_attrs;
	};

	class TypeTable {
	public:
		std::optional<Type*> get_type(const vm_ptr<Symbol> &name);

		bool insert(const Type &type);
	private:
		std::unordered_map<vm_ptr<Symbol>, Type> table;
	};
}

#endif

#ifndef SALMON_CORE_TYPE
#define SALMON_CORE_TYPE

#include <map>
#include <set>
#include <string>
#include <variant>
#include <optional>
#include <ostream>

#include <stdlib.h>

#include <compiler/box.hpp>
#include <compiler/vm/memory.hpp>


namespace salmon::compiler {

	/**
	 * An instance of this struct represents the cannonical metadata
	 * for a salmon type. It holds the name of the type, as well as any
	 * possible attributes or transformations that can be applied to an object
	 * of the type.
	 **/
	struct Type {
		std::string name;
		//! the potential metadata for an instance of the type with documentation
		const std::map<std::string, std::pair<const Type *const, std::string>> meta;
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
		const Type &get_type(const std::string &name) const;

		bool insert(const std::string &name, const Type &type);
	private:
		std::map<const std::string, const Type> table;
	};
}

#endif

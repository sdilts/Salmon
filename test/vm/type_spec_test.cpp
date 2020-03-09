#include <test/catch.hpp>

#include <vm/memory.hpp>
#include <vm/package.hpp>
#include <vm/new_type.hpp>

namespace salmon::vm {

	MemoryManager manager;

	Package base_package("test", manager);
	vm_ptr<Symbol> type_name_symb = base_package.intern_symbol("float-32");

	SCENARIO("Type objects delegate to their internal variant", "[type]") {
		PrimitiveType ptype(type_name_symb, "documentation", sizeof(int));
		Type::TypeVar varaint = ptype;
		std::shared_ptr<const Type> type(new Type(varaint));

		WHEN("The size is queried") {
			size_t size = type->size();
			THEN("The value is correct") {
				REQUIRE(sizeof(int) == size);
			}
		}

		WHEN("The concrete() method is queried") {
			bool val = type->concrete();
			THEN("The value is correct") {
				REQUIRE(val == true);
			}
		}
	}

	SCENARIO("TypeSpecifcation objects extract the correct info from their constructor arguments",
			 "[type]") {
		vm_ptr<Symbol> placeholder_symb = base_package.intern_symbol("a");
		PrimitiveType ptype(type_name_symb, "documentation", sizeof(int));
		Type::TypeVar variant = ptype;
		std::shared_ptr<const Type> type(new Type(variant));

		using ItemMask = TypeSpecification::ItemMask;
		ItemMask symb_variant = placeholder_symb;
		ItemMask type_variant = type;

		WHEN("A specification with symbols is created") {
			std::vector<ItemMask> mask = {symb_variant, type_variant, symb_variant };
			TypeSpecification spec(mask);

			THEN("The spec isn't concrete") {
				REQUIRE(spec.concrete() == false);
			}
		}

		WHEN("A specification made out of concrete types is created") {
			std::vector<ItemMask> mask = {type_variant};
			TypeSpecification spec(mask);

			THEN("The spec is concrete") {
				REQUIRE(spec.concrete() == true);
			}
		}
	}

}

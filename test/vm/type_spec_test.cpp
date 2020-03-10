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

	SCENARIO("TypeSpecification objects match the correct type lists") {
		PrimitiveType ptype(type_name_symb, "documentation", sizeof(int));
		PrimitiveType p_i32(type_name_symb, "documentation", sizeof(int));
		Type::TypeVar variant = ptype;
		Type::TypeVar v_i32 = p_i32;

		std::shared_ptr<const Type> f32_type(new Type(variant));
		std::shared_ptr<const Type> i32_type(new Type(v_i32));

		using ItemMask = TypeSpecification::ItemMask;
		ItemMask a_symb_variant = base_package.intern_symbol("a");
		ItemMask b_symb_variant = base_package.intern_symbol("b");
		ItemMask f32_type_variant = f32_type;

		WHEN("A Specification is matched with a type list with a different number of elements") {
			std::vector<ItemMask> mask = {b_symb_variant, a_symb_variant };
			TypeSpecification spec(std::move(mask));

			THEN("The list doesn't match") {
				std::vector<std::shared_ptr<const Type>> lst;
				REQUIRE(spec.matches(lst) == false);
			}
		}

		GIVEN("The specification (A (Type float-32) A)") {
			std::vector<ItemMask> mask = {a_symb_variant, f32_type_variant, a_symb_variant };
			TypeSpecification spec(std::move(mask));

			THEN("The type list (float-32 float-32 float-32) matches") {
				std::vector<std::shared_ptr<const Type>> lst = {
					f32_type, f32_type, f32_type
				};
				REQUIRE(spec.matches(lst) == true);
			}

			THEN("The type list (int-32 float-32 int-32) matches") {
				std::vector<std::shared_ptr<const Type>> lst = {
					i32_type, f32_type, i32_type
				};
				REQUIRE(spec.matches(lst) == true);
			}

			THEN("The type list (float-32 int-32 float-32) doesn't match.") {
				std::vector<std::shared_ptr<const Type>> lst = {
					f32_type, i32_type, f32_type
				};
				REQUIRE(spec.matches(lst) == false);
			}

			THEN("The type list (int-32 int-32 float-32) doesn't match.") {
				std::vector<std::shared_ptr<const Type>> lst = {
					i32_type, i32_type, f32_type
				};
				REQUIRE(spec.matches(lst) == false);
			}
		}

		GIVEN("The specification (A B)") {
			std::vector<ItemMask> mask = {a_symb_variant, b_symb_variant };
			TypeSpecification spec(std::move(mask));

			THEN("The type list (float-32 int-32) matches") {
				std::vector<std::shared_ptr<const Type>> lst = {
					f32_type, i32_type
				};
				REQUIRE(spec.matches(lst) == true);
			}

			THEN("The type list (float-32 float-32) matches") {
				std::vector<std::shared_ptr<const Type>> lst = {
					f32_type, f32_type
				};
				REQUIRE(spec.matches(lst) == true);
			}
		}
	}

}

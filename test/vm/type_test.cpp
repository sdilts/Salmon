#include <test/catch.hpp>

#include <vm/memory.hpp>
#include <vm/package.hpp>
#include <vm/new_type.hpp>

namespace salmon::vm {

	MemoryManager manager;

	Package base_package("test", manager);
	vm_ptr<Symbol> a_symb = base_package.intern_symbol("a");
	vm_ptr<Symbol> b_symb = base_package.intern_symbol("b");

	SCENARIO("Primitive types are created correctly") {
		TypeTable table;
		WHEN("A primitive is created using TypeTable::make_primitive") {
			auto p_type = table.make_primitive(a_symb, "Some doc", sizeof(vm_ptr<Symbol>));

			THEN("It can be retrieved from the table") {
				auto other_type = table.get_named(a_symb);
				REQUIRE(other_type);
				REQUIRE(*other_type == p_type);
				REQUIRE(**other_type == *p_type);
			}
		}
	}
	SCENARIO("Type Aliases are created correctly") {
		TypeTable table;
		GIVEN("A type object") {
			auto p_type = table.make_primitive(a_symb, "Some doc", sizeof(vm_ptr<Symbol>));

			WHEN("An alias is made for it") {
				bool success = table.make_alias(b_symb, p_type);
				REQUIRE(success);
				THEN("The type can be retreived using th alias") {
					auto other_type = table.get_named(b_symb);
					REQUIRE(other_type);
					REQUIRE(*other_type == p_type);
					REQUIRE(**other_type == *p_type);
				}
			}
			WHEN("An alias of an already existing type is requested") {
				auto other_type = table.make_primitive(b_symb, "Some doc", sizeof(vm_ptr<Symbol>));
				bool success = table.make_alias(b_symb, p_type);
				THEN("The alias isn't created") {
					REQUIRE(success == false);
				}
				THEN("The already existing type can still be accessed") {
					auto t = table.get_named(b_symb);
					REQUIRE(other_type == *t);
					REQUIRE(*other_type == **t);
				}
			}
		}
	}

}

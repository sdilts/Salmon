#include <vm/symbol.hpp>
#include <vm/memory.hpp>
#include <vm/package.hpp>

#include <test/catch.hpp>

namespace salmon::vm {

	SCENARIO("Symbol equality functions work.", "[package]") {
		MemoryManager mem_manager;

		GIVEN( "Two symbols from a different package with the same name") {
			Package pkg1 = Package("Test1",mem_manager);
			Package pkg2 = Package("Test2", mem_manager);
			const Symbol first_symb("Foo", &pkg1);
			const Symbol second_symb("Foo", &pkg2);

			THEN( "The two symbols are not reported as equal") {
				REQUIRE(first_symb != second_symb);
			}
			THEN("The correct one is greater than the other.") {
				REQUIRE(pkg1 < pkg2);
				REQUIRE(first_symb < second_symb);
			}
			THEN("The correct one is less than the other.") {
				REQUIRE(second_symb > first_symb);
			}
		}

		GIVEN( "Two symbols from the same package with a different name") {
			Package pkg1 = Package("Test1",mem_manager);
			const Symbol foo_symb("achoo", &pkg1);
			const Symbol bar_symb("boo", &pkg1);

			THEN( "The two symbols are not reported as equal") {
				REQUIRE(foo_symb != bar_symb);
			}
			THEN("The correct one is greater than the other.") {
				REQUIRE(foo_symb < bar_symb);
			}
			THEN("The correct one is less than the other.") {
				REQUIRE(!(foo_symb > bar_symb));
				REQUIRE(bar_symb > foo_symb);
			}
		}

		GIVEN( "Two symbols with the same name but not interned") {
			const Symbol foo_symb("foo");
			const Symbol bar_symb("foo");

			THEN( "The two symbols aren't equal") {
				REQUIRE(foo_symb != bar_symb);
			}

			THEN("They are neither greater or less than each other") {
				REQUIRE(!(foo_symb > bar_symb));
				REQUIRE(!(foo_symb < bar_symb));
			}
		}
		GIVEN( "A symbol without a package") {
			const Symbol foo_symb("foo");

			THEN( "It is equal to itself." ) {
				REQUIRE(foo_symb == foo_symb);
			}
		}

		GIVEN( "A symbol with a package") {
			Package pkg = Package("Test", mem_manager);
			const Symbol foo_symb("foo", &pkg);

			THEN( "It is equal to itself." ) {
				REQUIRE(foo_symb == foo_symb);
			}
		}
	}
}

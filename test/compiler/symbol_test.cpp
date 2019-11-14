#include <compiler/symbol.hpp>
#include <compiler/vm/memory.hpp>
#include <compiler/package.hpp>

#include <test/catch.hpp>

namespace salmon::compiler {

	SCENARIO("Symbol equality functions work.", "[package]") {
		MemoryManager mem_manager;

		GIVEN( "Two symbols from a different package") {
			Package pkg1 = Package("Test1",mem_manager);
			Package pkg2 = Package("Test2", mem_manager);
			const Symbol foo_symb("Foo", &pkg1);
			const Symbol bar_symb("Foo", &pkg2);

			THEN( "The two symbols are not reported as equal") {
				REQUIRE(foo_symb != bar_symb);
			}
		}
		GIVEN( "Two symbols with the same name but not interned") {
			const Symbol foo_symb("foo", std::nullopt);
			const Symbol bar_symb("foo", std::nullopt);

			THEN( "The two symbols aren't equal") {
				REQUIRE(foo_symb != bar_symb);
			}
		}
		GIVEN( "A symbol without a package") {
			const Symbol foo_symb("foo", std::nullopt);

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

#include <compiler/symbol.hpp>
#include <compiler/package.hpp>

#include <test/catch.hpp>

namespace salmon::compiler {

	SCENARIO("Symbol equality functions work.", "[package]") {
		GIVEN( "Two symbols from a different package") {
			Package pkg1 = Package("Test1");
			Package pkg2 = Package("Test2");
			const Symbol &foo_symb = pkg1.intern_symbol("Foo");
			const Symbol &bar_symb = pkg2.intern_symbol("Foo");

			THEN( "The two symbols are not reported as equal") {
				REQUIRE(foo_symb != bar_symb);
			}
		}
		GIVEN( "Two symbols with the same name but not interned") {
			const Symbol foo_symb = {"foo", std::nullopt };
			const Symbol bar_symb = {"foo", std::nullopt };

			THEN( "The two symbols aren't equal") {
				REQUIRE(foo_symb != bar_symb);
			}
		}
		GIVEN( "A symbol without a package") {
			const Symbol foo_symb = {"foo", std::nullopt };

			THEN( "It is equal to itself." ) {
				REQUIRE(foo_symb == foo_symb);
			}
		}

		GIVEN( "A symbol with a package") {
			Package pkg = Package("Test");
			const Symbol &foo_symb = pkg.intern_symbol("foo");

			THEN( "It is equal to itself." ) {
				REQUIRE(foo_symb == foo_symb);
			}
		}
	}
}

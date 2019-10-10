#define CATCH_CONFIG_MAIN

#include <compiler/symbol.hpp>
#include <test/catch.hpp>

namespace salmon::compiler {

	SCENARIO( "Symbols are inserted properly" "[package]") {

		GIVEN( "A package with no symbols" ) {
			salmon::compiler::Package package("test");

			REQUIRE(package.name == "test");

			WHEN( "The same string is interned twice" ) {
				const Symbol &symbol = intern_symbol("foo", package);
				const Symbol &other = intern_symbol("foo", package);

				THEN( "The two returned symbols are the same.") {
					REQUIRE(&symbol == &other);
				}
			}
		}
		GIVEN( "A package that uses some non-empty package") {
			Package parent1("parent1");
			Package parent2("parent2");
			const Symbol &foo_symb = intern_symbol("foo", parent1);
			const Symbol &bar_symb = intern_symbol("bar", parent2);

			Package child("child", { parent1, parent2 });

			WHEN( "A string from the first parent is interned" ) {
				const Symbol &other = intern_symbol("foo", child);

				THEN( "The given symbol is the one from the parent.") {
					REQUIRE(&other == &foo_symb);
					REQUIRE(&other.package == &parent1);
				}
			}
			WHEN( "A string from the second parent is interned") {
				const Symbol &other = intern_symbol("bar", child);

				THEN( "The given symbol is the one from the parent.") {
					REQUIRE(&other == &bar_symb);
					REQUIRE(&other.package == &parent2);
				}
			}
		}
	}
}

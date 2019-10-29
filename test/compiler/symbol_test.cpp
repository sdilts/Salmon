#define CATCH_CONFIG_MAIN

#include <compiler/symbol.hpp>
#include <test/catch.hpp>

namespace salmon::compiler {

	SCENARIO( "Symbols are only interned once.", "[package]") {

		GIVEN( "A package with no symbols" ) {
			salmon::compiler::Package package("test");

			REQUIRE(package.name == "test");

			WHEN( "The same string is interned twice" ) {
				const Symbol &symbol = package.intern_symbol("foo");
				const Symbol &other = package.intern_symbol("foo");

				THEN( "The two returned symbols are the same.") {
					REQUIRE(symbol == other);
				}
			}
		}
	}

	SCENARIO( "Unexported symbols aren't inherited.", "[package]") {
		GIVEN( "A package that uses some non-empty package") {
			Package parent1("parent1");
			Package parent2("parent2");
			const Symbol &foo_symb = parent1.intern_symbol("foo");
			const Symbol &bar_symb = parent2.intern_symbol("bar");

			Package child("child", { parent1, parent2 });

			WHEN( "A string from the first parent is interned" ) {
				const Symbol &other = child.intern_symbol("foo");

				THEN( "The given symbol is the one from the parent.") {
					REQUIRE(&other != &foo_symb);
					if(other.package) {
						REQUIRE((*other.package) != &parent1);
					} else {
						REQUIRE(false);
					}
				}
			}
			WHEN( "A string from the second parent is interned") {
				const Symbol &other = child.intern_symbol("bar");

				THEN( "The given symbol is the one from the parent.") {
					REQUIRE(other != bar_symb);
					if(other.package) {
						REQUIRE((*other.package) != &parent2);
					} else {
						REQUIRE(false);
					}
				}
			}
		}
	}

	SCENARIO( "Exported symbols should be inherited.", "[package]") {
		GIVEN( "A package with an exported symbol") {
			Package parent("parent");
			const Symbol &symb = parent.intern_symbol("test");
			parent.export_symbol(symb);

			WHEN( "The package is used") {
				Package child = Package("Child", { parent });
				THEN( "The parent's symbols should be used.") {
					const Symbol &child_symb = parent.intern_symbol("test");

					REQUIRE(child_symb == symb);
				}
			}
		}
	}
}

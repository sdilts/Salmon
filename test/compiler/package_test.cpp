#include <compiler/symbol.hpp>
#include <compiler/package.hpp>

#include <test/catch.hpp>

namespace salmon::compiler {

	MemoryManager manager;

	SCENARIO( "A symbol is interned", "[package]") {
		GIVEN("A package with no symbols") {
			salmon::compiler::Package package("find_symbol", manager);
			WHEN( "A symbol is interned" ) {
				vm_ptr<Symbol> symbol = package.intern_symbol("test");
				THEN( "it is found by find_symbol") {
					std::optional<vm_ptr<Symbol>> item = package.find_symbol("test");
					REQUIRE(item);
					REQUIRE(*(*item) == *symbol);
				}
			}
			std::cerr << "done\n";
		}
		manager.do_gc();
	}

	SCENARIO( "Symbols are only interned once.", "[package]") {

		GIVEN( "A package with no symbols" ) {
			salmon::compiler::Package package("test", manager);

			REQUIRE(package.name == "test");

			WHEN( "The same string is interned twice" ) {
			    vm_ptr<Symbol> symbol = package.intern_symbol("foo");
				vm_ptr<Symbol> other = package.intern_symbol("foo");

				THEN( "The two returned symbols are the same.") {
					REQUIRE(symbol);
					REQUIRE(other);
					REQUIRE(*symbol == *other);
				}
			}
		}
		manager.do_gc();
	}

	SCENARIO( "Unexported symbols aren't inherited.", "[package]") {
		GIVEN( "A package that uses some non-empty package") {
			Package parent1("parent1", manager);
			Package parent2("parent2", manager);
			vm_ptr<Symbol> foo_symb = parent1.intern_symbol("foo");
			vm_ptr<Symbol> bar_symb = parent2.intern_symbol("bar");

			Package child("child", manager, { parent1, parent2 });

			WHEN( "A string from the first parent is interned" ) {
				vm_ptr<Symbol> other = child.intern_symbol("foo");

				THEN( "The given symbol is the one from the parent.") {
					REQUIRE(*other != *foo_symb);
					if((*other).package) {
						REQUIRE(*(*other).package != &parent1);
					} else {
						REQUIRE(false);
					}
				}
			}
			WHEN( "A string from the second parent is interned") {
				vm_ptr<Symbol> other = child.intern_symbol("bar");

				THEN( "The given symbol is the one from the parent.") {
					REQUIRE(*other != *bar_symb);
					if((*other).package) {
						REQUIRE(*(*other).package != &parent2);
					} else {
						REQUIRE(false);
					}
				}
			}
		}
		manager.do_gc();
	}

	SCENARIO( "Exported symbols should be inherited.", "[package]") {
		GIVEN( "A package with an exported symbol") {
			Package parent("parent", manager);
			vm_ptr<Symbol> symb = parent.intern_symbol("test");
			parent.export_symbol(*symb);

			WHEN( "The package is used") {
				Package child = Package("Child", manager, { parent });
				THEN( "The parent's symbols should be used.") {
					vm_ptr<Symbol> child_symb = parent.intern_symbol("test");

					REQUIRE(*child_symb == *symb);
				}
			}
		}
		manager.do_gc();
	}
}

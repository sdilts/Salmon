#include <iostream>

#include <vm/box.hpp>
#include <vm/memory.hpp>

#include <test/catch.hpp>

namespace salmon::vm {

	MemoryManager manager;

	auto type_symb = manager.allocate_obj<Symbol>("type");

	TypeTable table(manager);
	auto type = table.make_primitive(type_symb, "", sizeof(int));

	SCENARIO( "An InternalBox contains a integer", "[box, vm]") {
		InternalBox box = { type.get(), 32 };
		WHEN("The roots are given") {
			std::vector<AllocatedItem*> roots;
			box.get_roots([&roots](AllocatedItem* item) {
				roots.push_back(item);
			});
			THEN("Then it isn't added to the roots") {
				REQUIRE(roots.size() == 1);
			}
		}
	}

	SCENARIO( "An InternalBox contains a float", "[box, vm]") {
		InternalBox box = { type.get(), 32.1f };
		WHEN("The roots are given") {
			std::vector<AllocatedItem*> roots;
			box.get_roots([&roots](AllocatedItem* item) {
				roots.push_back(item);
			});
			THEN("Then it isn't added to the roots") {
				REQUIRE(roots.size() == 1);
			}
		}
	}

	SCENARIO( "An InternalBox contains a symbol", "[box, vm]") {
		Symbol *symb = new Symbol("foo");
		InternalBox box = { type.get(), symb };
		WHEN("The roots are given") {
			std::vector<AllocatedItem*> roots;
			box.get_roots([&roots](AllocatedItem* item) {
				roots.push_back(item);
			});
			THEN("There are two roots") {
				REQUIRE(roots.size() == 2);
			}
		}
		delete symb;
	}

	SCENARIO( "operator<=> works for InternalBox", "[box, vm]") {

		WHEN("Two boxes with the same integer value are compared") {
			InternalBox one = { type.get(), 1 };
			InternalBox two = { type.get(), 1 };
			THEN("They are reported as equal") {
				REQUIRE(one == two);
				REQUIRE((one < two) == false);
				REQUIRE((two < one) == false);
			}
		}

		WHEN("Two boxes with with different integer values are compared") {
			InternalBox one = { type.get(), 1 };
			InternalBox two = { type.get(), 2 };
			THEN("They are reported as equal") {
				REQUIRE(one < two);
				REQUIRE(two > one);
				REQUIRE(two != one);
			}
		}
	}
}

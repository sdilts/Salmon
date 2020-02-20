#include <iostream>

#include <compiler/vm/box.hpp>

#include <test/catch.hpp>

namespace salmon::vm {

	SCENARIO( "An InternalBox contains a integer", "[box, vm]") {
		InternalBox box = { nullptr, 32 };
		WHEN("The roots are given") {
			auto roots = box.get_roots();
			THEN("There are no roots") {
				REQUIRE(roots.empty());
			}
		}
	}

	SCENARIO( "An InternalBox contains a float", "[box, vm]") {
		InternalBox box = { nullptr, 32.1f };
		WHEN("The roots are given") {
			auto roots = box.get_roots();
			THEN("There are no roots") {
				REQUIRE(roots.empty());
			}
		}
	}

	SCENARIO( "An InternalBox contains a symbol", "[box, vm]") {
		Symbol *symb = new Symbol("foo", std::nullopt);
		InternalBox box = { nullptr, symb };
		WHEN("The roots are given") {
			auto roots = box.get_roots();
			THEN("There is one root") {
				REQUIRE(!roots.empty());
			}
		}
		delete symb;
	}
}

#include <vm/vm.hpp>
#include <vm/box.hpp>
#include <vm/memory.hpp>

#include <test/catch.hpp>

namespace salmon::vm {

	SCENARIO("Comparison functions work correctly") {
		Config fakeConfig;
		VirtualMachine vm(fakeConfig, "list-test");
		WHEN("The lists (1) and (1) are compared") {
			Box one = vm.make_boxed(1);
			Box two = vm.make_boxed(1);
			List first = List(one);
			List second = List(two);
			THEN("They are equal") {
				REQUIRE(first == second);
			}
			THEN("They are not greater or less than each other.") {
				REQUIRE((first > second) == false);
				REQUIRE((first < second) == false);
			}
		}

		WHEN("The lists (1) and (2) are compared") {
			Box one = vm.make_boxed(1);
			Box two = vm.make_boxed(2);

			List first = List(one);
			List second = List(two);
			THEN("(1) is less than (2)") {
				REQUIRE(first < second);
				REQUIRE(one.bare() < two.bare());
			}
			THEN("(2) is greater than (1)") {
				REQUIRE(second > first);
				REQUIRE(two.bare() > one.bare());
			}
			THEN("They are not equal to each other.") {
				// REQUIRE(first != second);
				REQUIRE(first.itm != second.itm);
			}
		}

		WHEN("The lists (1) and (1 1) are compared") {
			Box one = vm.make_boxed(1);
			List single = List(one);
			List second = List(one);
			List second_2 = List(one);
			second.next = &second_2;
			THEN("(1) is less than (1 1)") {
				REQUIRE(single < second);
			}
			THEN("(1 1) is greater than (1)") {
				REQUIRE(second > single);
			}
			THEN("They are not equal to each other.") {
				REQUIRE(single != second);
			}
		}

		WHEN("The lists (1 1) and (1 2) are compared") {
			Box one = vm.make_boxed(1);
			Box two = vm.make_boxed(2);

			List all_ones = List(one);
			List first_1 = List(one);
			all_ones.next = &first_1;
			List second = List(one);
			List second_1 = List(two);
			second.next = &second_1;
			THEN("(1 1) is less than (1 2)") {
				REQUIRE(all_ones < second);
			}
			THEN("(1 2) is greater than (1 1)") {
				REQUIRE(second > all_ones);
			}
			THEN("They are not equal to each other.") {
				REQUIRE(all_ones != second);
			}
		}
	}
}

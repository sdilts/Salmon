#include <test/catch.hpp>

#include "vm/vm.hpp"

namespace salmon::vm {

	SCENARIO("Comparison functions work correctly") {
		Config fakeConfig;
		VirtualMachine vm(fakeConfig, "dyn-array-test");
		WHEN("The empty array is compared") {
			Vector first(0);
			Vector second(0);
			THEN("They are equal") {
				REQUIRE(first == second);
			}
			THEN("They are not greater or equal to each other") {
				REQUIRE((first > second) == false);
				REQUIRE((first < second) == false);
			}
		}

		WHEN("Two identical arrays are compared") {
			Box one = vm.make_boxed(1);
			Box two = vm.make_boxed(2);
			Vector first(2);
			first.push_back(one);
			first.push_back(two);
			Vector second(2);
			second.push_back(one);
			second.push_back(two);
			THEN("They are equal") {
				REQUIRE(first == second);
			}
			THEN("They are not greater or equal to each other") {
				REQUIRE((first > second) == false);
				REQUIRE((first < second) == false);
			}
		}

		WHEN("[1 2] and [1] are compared") {
			Box one = vm.make_boxed(1);
			Box two = vm.make_boxed(2);
			Vector first(2);
			first.push_back(two);
			Vector second(2);
			second.push_back(one);
			second.push_back(two);
			THEN("They are not equal") {
				REQUIRE(first != second);
			}
			THEN("Greater than or less than works") {
				REQUIRE(second > first);
			}
		}
	}
}

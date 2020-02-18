#include <iostream>

#include <test/catch.hpp>

#include <compiler/vm/memory.hpp>
#include <compiler/vm/builtinfunction.hpp>

namespace salmon::vm {

	Box foo(Box &one, Box &two) {
		std::ignore = two;
		return one;
	}

	MemoryManager manager;

	auto func_name = manager.make_symbol("foo");
	auto arg1 = manager.make_symbol("arg1");
	auto arg2 = manager.make_symbol("arg2");

	BuiltinFunction<Box&,Box&> func(func_name, {arg1, arg2}, std::nullopt, std::nullopt, foo);


	SCENARIO("Builtin functions check their argument lengths", "[functions]") {

		WHEN("The wrong number of arguments is given") {

			Box b = manager.make_box();
			std::vector<Box> input = { b };
			THEN("An ArityException is thrown") {
				REQUIRE_THROWS_AS(func(input), ArityException);
			}
		}
		WHEN("The correct number of arguments is given") {
			Box b = manager.make_box();
			std::vector<Box> input = { b, b };
			THEN("An ArityException isn't thrown") {
				REQUIRE_NOTHROW(func(input));
			}
		}
	}

	SCENARIO("Arity Exceptions report the correct info", "[vm, functions]") {
		try {
			Box b = manager.make_box();
			std::vector<Box> input = { b };
			func(input);
			REQUIRE(false);
		} catch(const ArityException &err) {
			REQUIRE(err.desired == 2);
			REQUIRE(err.given == 1);
		}
	}
}

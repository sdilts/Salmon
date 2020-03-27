#include <iostream>

#include <test/catch.hpp>

#include <vm/memory.hpp>
#include <vm/builtinfunction.hpp>

namespace salmon::vm {

	static Box foo(Box &one, Box &two) {
		std::ignore = two;
		return one;
	}

	MemoryManager manager;

	auto func_name = manager.allocate_obj<Symbol>("foo");
	auto arg1 = manager.allocate_obj<Symbol>("arg1");
	auto arg2 = manager.allocate_obj<Symbol>("arg2");

	BuiltinFunction<Box&,Box&> func({arg1, arg2}, std::nullopt, std::nullopt, foo);


	SCENARIO("Builtin functions check their argument lengths", "[vm, functions]") {

		WHEN("The wrong number of arguments is given") {
			auto ptr = manager.make_vm_ptr<AllocatedItem>();
			Box b(ptr);
			std::vector<Box> input = { b };
			THEN("An ArityException is thrown") {
				REQUIRE_THROWS_AS(func(input), ArityException);
			}
		}
		WHEN("The correct number of arguments is given") {
			auto ptr = manager.make_vm_ptr<AllocatedItem>();
			Box b(ptr);
			std::vector<Box> input = { b, b };
			THEN("An ArityException isn't thrown") {
				REQUIRE_NOTHROW(func(input));
			}
		}
	}

	SCENARIO("ArityExceptions report the correct info", "[vm, functions]") {
		try {
			auto ptr = manager.make_vm_ptr<AllocatedItem>();
			Box b(ptr);
			std::vector<Box> input = { b };
			func(input);
			REQUIRE(false);
		} catch(const ArityException &err) {
			REQUIRE(err.desired == 2);
			REQUIRE(err.given == 1);
			std::string err_str = err.what();
			auto check_str = "Wrong number of arguments given to function (given 1, expected 2)";
			REQUIRE(err_str == check_str);
		}
	}
}

#include <iostream>

#include <test/catch.hpp>

#include <vm/memory.hpp>
#include <vm/builtinfunction.hpp>
#include <vm/vm.hpp>


namespace salmon::vm {

	static Box foo(VirtualMachine*vm, InternalBox one,InternalBox) {
		Box b(one, vm->mem_manager.make_vm_ptr<AllocatedItem>());
		return b;
	}

	Config config;
	VirtualMachine vm(config, "base-package");
	MemoryManager manager;

	auto func_name = manager.allocate_obj<Symbol>("foo");
	auto arg1 = manager.allocate_obj<Symbol>("arg1");
	auto arg2 = manager.allocate_obj<Symbol>("arg2");
	auto type_name = manager.allocate_obj<Symbol>("type");
	// This isn't the correct type, hope it doesn't bite me in the butt:
	SpecBuilder builder;
	FunctionType p(builder.build(), builder.build());
	auto type = manager.allocate_obj<Type>(p);

	BuiltinFunction<InternalBox,InternalBox> func(foo, type,{arg1, arg2}, std::nullopt, std::nullopt);

	SCENARIO("Builtin functions check their argument lengths", "[vm, functions]") {

		WHEN("The wrong number of arguments is given") {
			auto symb = manager.allocate_obj<Symbol>("b");
			Box b(symb, type);
			std::vector<Box> input = { b };
			THEN("An ArityException is thrown") {
				REQUIRE_THROWS_AS(func.invoke(&vm, input), ArityException);
			}
		}
		WHEN("The correct number of arguments is given") {
			auto symb = manager.allocate_obj<Symbol>("b");
			Box b(symb, type);
			std::vector<Box> input = { b, b };
			THEN("An ArityException isn't thrown") {
				REQUIRE_NOTHROW(func.invoke(&vm, input));
			}
		}
	}

	SCENARIO("ArityExceptions report the correct info", "[vm, functions]") {
		try {
			auto symb = manager.allocate_obj<Symbol>("b");
			Box b(symb, type);

			std::vector<Box> input = { b };
			func.invoke(&vm, input);
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

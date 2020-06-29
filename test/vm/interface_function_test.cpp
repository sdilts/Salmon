#include <iostream>

#include <test/catch.hpp>

#include <vm/vm.hpp>
#include <vm/builtinfunction.hpp>

namespace salmon::vm {

	static const std::string base_package_name = "package";

	static Box add_double(VirtualMachine *vm, Box &one) {
		double val = std::get<double>(one.value());
		val = val + 1;
		Box ret = vm->make_boxed(val);
		return ret;
	}

	static Box add_int(VirtualMachine *vm, Box &one) {
		int val = std::get<int>(one.value());
		val = val + 1;
		Box ret = vm->make_boxed(val);
		return ret;
	}

	static InterfaceFunction init_interface(VirtualMachine &vm) {
		Package &package = vm.find_package(base_package_name)->get();
		vm_ptr<Symbol> param_name = package.intern_symbol("param");
		auto int_type = vm.get_builtin_type<int>();
		auto double_type = vm.get_builtin_type<double>();

		std::vector<vm_ptr<Symbol>> lambda_list = {param_name};
		SpecBuilder int_builder;
		int_builder.add_type(int_type);
		FunctionType int_fn_t(int_builder.build(), int_builder.build());
		vm_ptr<Type> int_fn_type = vm.mem_manager.allocate_obj<Type>(int_fn_t);
		vm_ptr<VmFunction> int_fn =
			vm.mem_manager.allocate_obj<BuiltinFunction<Box&>>(add_int, int_fn_type, lambda_list,
															   std::nullopt, std::nullopt);

		SpecBuilder double_builder;
		double_builder.add_type(double_type);
		FunctionType double_fn_t(double_builder.build(), double_builder.build());
		vm_ptr<Type> double_fn_type = vm.mem_manager.allocate_obj<Type>(double_fn_t);
		assert(double_type->concrete());
		vm_ptr<VmFunction> double_fn =
			vm.mem_manager.allocate_obj<BuiltinFunction<Box&>>(add_double, double_fn_type,
															   lambda_list,
															   std::nullopt, std::nullopt);

		SpecBuilder arg_builder;
		arg_builder.add_parameter(param_name);
		SpecBuilder ret_builder;
		ret_builder.add_parameter(param_name);
		FunctionType fn_t(ret_builder.build(), arg_builder.build());
		auto interface_type = vm.mem_manager.allocate_obj<Type>(fn_t);
		InterfaceFunction interface(interface_type, lambda_list);
		assert(interface.add_impl(double_fn));
		assert(interface.add_impl(int_fn));


		return interface;
	}

	SCENARIO("Function dispatch works correctly") {
		Config config;
		VirtualMachine vm(config, base_package_name);
		InterfaceFunction func = init_interface(vm);
		WHEN("A double is given as an arg") {
			double val = 2;
			std::vector<Box> arg = { vm.make_boxed(val) };
			THEN("The double function is called") {
				Box result = func(&vm,arg);
				REQUIRE(result.elem_type() == vm.get_builtin_type<double>());
				REQUIRE(std::get<double>(result.value()) == 3);
			}
		}
		WHEN("An int is given as an arg") {
			int val = 2;
			std::vector<Box> arg = { vm.make_boxed(val) };
			THEN("The int function is called") {
				Box result = func(&vm,arg);
				REQUIRE(result.elem_type() == vm.get_builtin_type<int>());
				REQUIRE(std::get<int>(result.value()) == 3);
			}
		}
		WHEN("A boolean is given as an arg") {
			bool val = true;
			std::vector<Box> arg = { vm.make_boxed(val) };
			THEN("An exception is thrown") {
				REQUIRE_THROWS_AS(func(&vm,arg),NoSuchFunction);
			}
		}
	}
}

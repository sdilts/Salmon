#include <vm/vm.hpp>
#include <vm/function.hpp>

#include <test/catch.hpp>

namespace salmon::vm {

	class TestFunction : public VmFunction {
	public:
		TestFunction(const vm_ptr<Type> &type,
					 const std::vector<vm_ptr<Symbol>> &lambda_list) :
			VmFunction(type, lambda_list) {

		}

		TestFunction(const vm_ptr<Type> &type,
					 const std::vector<vm_ptr<Symbol>> &lambda_list,
					 std::optional<std::string> doc, std::optional<std::string> file,
					 const std::optional<vm_ptr<List>> &source) :
			VmFunction(type,lambda_list, doc,file, source) {}

		~TestFunction() {}

		Box operator()(VirtualMachine *vm, std::vector<Box>&) {
			Box b = vm->make_boxed(1);
			return b;
		}

		void print_debug_info() const {}
		size_t allocated_size() const { return sizeof(TestFunction); }
	};

	SCENARIO("Function types record their roots correctly","[vm]") {
		Config config;
		VirtualMachine vm(config, "base-package");
		MemoryManager &manager = vm.mem_manager;
		auto type_name = manager.allocate_obj<Symbol>("type");
		SpecBuilder builder;
		FunctionType p(builder.build(), builder.build());
		auto type = manager.allocate_obj<Type>(p);

		auto symb = manager.allocate_obj<Symbol>("symbol");

		WHEN("The roots of a VmFunction with no source are grabbed") {
			TestFunction test(type, {symb});
			std::set<AllocatedItem*> roots;
			test.get_roots([&roots](AllocatedItem* item) {
				roots.insert(item);
			});
			THEN("The type is returned") {
				AllocatedItem *item = static_cast<AllocatedItem*>(type.get());
				REQUIRE(roots.find(item) != roots.end());
			}
			THEN("The symbols in the lambda list are returned") {
				AllocatedItem *item = static_cast<AllocatedItem*>(symb.get());
				REQUIRE(roots.find(item) != roots.end());
			}

			THEN("Only the type and lambda list is returned") {
				REQUIRE(roots.size() == 2);
			}
		}

		WHEN("The roots of a VmFunction with a source specified is grabbed") {
			vm_ptr<List> list = manager.allocate_obj<List>(vm.make_boxed(1));
			TestFunction test(type, {symb}, std::nullopt, std::nullopt,
							  list);
			std::set<AllocatedItem*> roots;
			test.get_roots([&roots](AllocatedItem* item) {
				roots.insert(item);
			});
			THEN("The list is returned") {
				AllocatedItem *item = static_cast<AllocatedItem*>(list.get());
				REQUIRE(roots.find(item) != roots.end());
			}
		}
	}
}

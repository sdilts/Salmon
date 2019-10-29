#include <test/catch.hpp>

#include <compiler/vm/vm_ptr.hpp>

namespace salmon::vm {

    SCENARIO( "A single vm_ptr records its root correctly.", "[vm_ptr]") {

		GIVEN( "A vm_ptr with a non-null pointer to keep track of") {
			Box *item = new Box();
			vm_ptr ptr(item);

			THEN( "The box and only the box is shown in vm_ptr::get_instances()") {
				std::vector<Box*> instances = vm_ptr::get_instances();

				REQUIRE(!instances.empty());
				REQUIRE(instances.size() == 1);
				REQUIRE(instances[0] == item);
			}

			WHEN( "The vm_ptr is deleted") {
				ptr.~vm_ptr();

				THEN( "The box is not in vm_ptr::get_instances()") {
					std::vector<Box*> instances = vm_ptr::get_instances();
					REQUIRE(instances.empty());
				}
			}
			delete item;
		}
	}

	SCENARIO( "Two vm_ptrs work correctly when created with the same Box pointer", "[vm_ptr]") {
		GIVEN( "Two vm_ptrs created with the same Box pointer") {
			Box *item = new Box();
			vm_ptr ptr1(item);
			vm_ptr ptr2(item);

			THEN( "The box and only the box is shown in vm_ptr::get_instances()") {
				std::vector<Box*> instances = vm_ptr::get_instances();

				REQUIRE(!instances.empty());
				REQUIRE(instances.size() == 1);
				REQUIRE(instances[0] == item);
			}

			WHEN("One is deleted") {
				ptr2.~vm_ptr();

				THEN( "The box is still in vm_ptr::get_instances") {
					std::vector<Box*> instances = vm_ptr::get_instances();

					REQUIRE(!instances.empty());
					REQUIRE(instances.size() == 1);
					REQUIRE(instances[0] == item);
				}
			}

			WHEN("Both are deleted") {
				ptr2.~vm_ptr();
				ptr1.~vm_ptr();

				THEN( "The vm_ptr::get_instances is empty") {
					std::vector<Box*> instances = vm_ptr::get_instances();

					REQUIRE(instances.empty());
				}
			}
			delete item;
		}
	}
	SCENARIO( "Copying an vm_ptr should behave correctly", "[vm_ptr]") {
		GIVEN("A vm_ptr managing a Box and a copy of the vm_ptr") {
			Box *item = new Box();
			vm_ptr ptr(item);
			vm_ptr copy(ptr);

			THEN("The copy points to the same object") {
				REQUIRE(&*ptr == &*copy);
			}
			WHEN("The original is deleted") {
				ptr.~vm_ptr();
				THEN("The new one still works.") {
					REQUIRE(&*copy == item);
				}
				THEN("The item is still in the instances list.") {
					std::vector<Box*> instances = vm_ptr::get_instances();

					REQUIRE(!instances.empty());
					REQUIRE(instances[0] == item);
				}
			}

			WHEN("Both are deleted") {
				ptr.~vm_ptr();
				copy.~vm_ptr();

				THEN( "The vm_ptr::get_instances is empty") {
					std::vector<Box*> instances = vm_ptr::get_instances();

					REQUIRE(instances.empty());
				}
			}
			delete item;
		}
	}

	SCENARIO("Assigning vm_ptrs using operator= functions correctly", "[vm_ptr]") {
		Box *item = new Box();
		vm_ptr ptr(item);

		WHEN("An empty vm_ptr is assigned to") {
			vm_ptr other(nullptr);
			other = ptr;
			THEN("Both pointers point to the same object.") {
				REQUIRE(other.get() == ptr.get());
				REQUIRE(other.get() == item);
			}
		}

		WHEN("An non-empty vm_ptr is assigned to") {
			Box *other_box = new Box();
			vm_ptr other(other_box);

			other = ptr;
			THEN("The pointer it used to contain is freed.") {
				std::vector<Box*> instances = vm_ptr::get_instances();

				REQUIRE(!instances.empty());
				REQUIRE(instances[0] == item);
				REQUIRE(instances.size() == 1);
			}
			delete other_box;
		}
		delete item;
	}

	SCENARIO("vm_ptrs initialized with nullptr still function", "[vm_ptr]") {
		vm_ptr ptr(nullptr);

		WHEN("The pointer is deleted") {
			THEN("Nothing bad happens") {
				ptr.~vm_ptr();
			}
		}
	}
}

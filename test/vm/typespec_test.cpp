#include <test/catch.hpp>

#include <vm/memory.hpp>
#include <vm/package.hpp>
#include <vm/type.hpp>

namespace salmon::vm {

	SCENARIO("TypeSpecifcation objects extract the correct info from their constructor arguments",
			 "[type]") {
		MemoryManager manager;
		Package base_package("test", manager);
		vm_ptr<Symbol> type_name_symb = base_package.intern_symbol("float-32");

		vm_ptr<Symbol> placeholder_symb = base_package.intern_symbol("a");
		PrimitiveType ptype(type_name_symb, "documentation", sizeof(int));
		vm_ptr<Type> type = manager.allocate_obj<Type>(ptype);


		WHEN("A specification with symbols is created") {
			SpecBuilder builder;
			builder.add_parameter(placeholder_symb);
			builder.add_type(type);
			builder.add_parameter(placeholder_symb);
			TypeSpecification spec = builder.build();

			THEN("The spec isn't concrete") {
				REQUIRE(spec.concrete() == false);
			}
		}

		WHEN("A specification made out of concrete types is created") {
			SpecBuilder builder;
			builder.add_type(type);
			TypeSpecification spec = builder.build();

			THEN("The spec is concrete") {
				REQUIRE(spec.concrete() == true);
			}
		}
	}

	SCENARIO("TypeSpecification objects match the correct type lists") {
		MemoryManager manager;
		Package base_package("test", manager);
		vm_ptr<Symbol> f32_name_symb = base_package.intern_symbol("float-32");
		vm_ptr<Symbol> i32_name_symb = base_package.intern_symbol("int-32");
		PrimitiveType ptype(f32_name_symb, "documentation", sizeof(int));
		PrimitiveType p_i32(i32_name_symb, "documentation", sizeof(int));

		auto f32_type = manager.allocate_obj<Type>(ptype);
		auto i32_type = manager.allocate_obj<Type>(p_i32);

		vm_ptr<Symbol> a_symb = base_package.intern_symbol("a");
		vm_ptr<Symbol> b_symb = base_package.intern_symbol("b");

		WHEN("A Specification is matched with a type list with a different number of elements") {
			SpecBuilder builder;
			builder.add_parameter(a_symb);
			builder.add_parameter(b_symb);
			TypeSpecification spec = builder.build();

			THEN("The list doesn't match") {
				std::vector<vm_ptr<Type>> lst = { f32_type };
				REQUIRE(spec.matches(lst) == false);
			}
		}

		GIVEN("The specification (A (Type float-32) A)") {
			// std::vector<ItemMask> mask = {a_symb_variant, f32_type_variant, a_symb_variant };
			SpecBuilder builder;
			builder.add_parameter(a_symb);
			builder.add_type(f32_type);
			builder.add_parameter(a_symb);
			TypeSpecification spec = builder.build();;

			THEN("The type list (float-32 float-32 float-32) matches") {
				std::vector<vm_ptr<Type>> lst = {
					f32_type, f32_type, f32_type
				};
				REQUIRE(spec.matches(lst) == true);
			}

			THEN("The type list (int-32 float-32 int-32) matches") {
				std::vector<vm_ptr<Type>> lst = {
					i32_type, f32_type, i32_type
				};
				REQUIRE(spec.matches(lst) == true);
			}

			THEN("The type list (float-32 int-32 float-32) doesn't match.") {
				std::vector<vm_ptr<Type>> lst = {
					f32_type, i32_type, f32_type
				};
				REQUIRE(spec.matches(lst) == false);
			}

			THEN("The type list (int-32 int-32 float-32) doesn't match.") {
				std::vector<vm_ptr<Type>> lst = {
					i32_type, i32_type, f32_type
				};
				REQUIRE(spec.matches(lst) == false);
			}
		}

		GIVEN("The specification (A B)") {
			SpecBuilder builder;
			builder.add_parameter(a_symb);
			builder.add_parameter(b_symb);
			TypeSpecification spec = builder.build();

			THEN("The type list (float-32 int-32) matches") {
				std::vector<vm_ptr<Type>> lst = {
					f32_type, i32_type
				};
				REQUIRE(spec.matches(lst) == true);
			}

			THEN("The type list (float-32 float-32) matches") {
				std::vector<vm_ptr<Type>> lst = {
					f32_type, f32_type
				};
				REQUIRE(spec.matches(lst) == true);
			}
		}
	}

	SCENARIO("When types match, they infer the correct types") {
		MemoryManager manager;
		Package base_package("test", manager);
		vm_ptr<Symbol> f32_name_symb = base_package.intern_symbol("float-32");
		vm_ptr<Symbol> i32_name_symb = base_package.intern_symbol("int-32");
		PrimitiveType ptype(f32_name_symb, "documentation", sizeof(int));
		PrimitiveType p_i32(i32_name_symb, "documentation", sizeof(int));

	        vm_ptr<Type> f32_type = manager.allocate_obj<Type>(ptype);
		vm_ptr<Type> i32_type = manager.allocate_obj<Type>(p_i32);

		vm_ptr<Symbol> a_symb = base_package.intern_symbol("a");
		vm_ptr<Symbol> b_symb = base_package.intern_symbol("b");


		GIVEN("The spec (A B) and the typelist [f32, i32]") {
			// std::vector<ItemMask> mask = {a_symb_variant, b_symb_variant };
			SpecBuilder builder;
			builder.add_parameter(a_symb);
			builder.add_parameter(b_symb);
			TypeSpecification spec = builder.build();

			WHEN("spec is matched with the type list") {
				std::vector<vm_ptr<Type>> lst = {
					f32_type, i32_type
				};
				auto opt_table = spec.match_symbols(lst);
				THEN("The types should match") {
					REQUIRE(static_cast<bool>(opt_table));
				}
				auto symb_table = *opt_table;
				THEN("A should match to f32") {
					auto thing = symb_table.at(a_symb);
					REQUIRE(thing == f32_type);
				}
				THEN("B should match to i32") {
					auto thing = symb_table.at(b_symb);
					REQUIRE(thing == i32_type);
				}
			}
		}
	}

	SCENARIO("Type specification object equality functions work") {
		MemoryManager manager;
		Package base_package("test", manager);
		vm_ptr<Symbol> type_name_symb = base_package.intern_symbol("float-32");

		PrimitiveType ptype(type_name_symb, "documentation", sizeof(int));
		PrimitiveType p_i32(type_name_symb, "documentation", sizeof(int));

		auto f32_type = manager.allocate_obj<Type>(ptype);
		auto i32_type = manager.allocate_obj<Type>(p_i32);

		SpecBuilder builder;
		builder.add_type(f32_type);
		builder.add_parameter(type_name_symb);
		builder.add_type(i32_type);
		auto base = builder.build();

		GIVEN("Two identical objects") {
			SpecBuilder other;
			other.add_type(f32_type);
			other.add_parameter(type_name_symb);
			other.add_type(i32_type);
			auto other_spec = other.build();

			THEN("They should be equal") {
				REQUIRE(other_spec == base);
			}
		}

		GIVEN("Specs with different properties") {
			SpecBuilder other;
			other.add_type(f32_type, true, true);
			other.add_parameter(type_name_symb);
			other.add_type(i32_type);
			auto other_spec = other.build();

			THEN("They are not equal") {
				REQUIRE(other_spec != base);
			}
		}

		GIVEN("Two specs with different sizes") {
			SpecBuilder other;
			other.add_parameter(type_name_symb);
			other.add_type(i32_type);
			auto other_spec = other.build();

			THEN("They shouldn't be equal") {
				REQUIRE(other_spec != base);
			}
		}

		GIVEN("Two equivalent specs with different parameter names") {
			auto symb = base_package.intern_symbol("other");
			SpecBuilder other;
			other.add_type(f32_type);
			other.add_parameter(symb);
			other.add_type(i32_type);

			auto other_spec = other.build();

			THEN("They are not equal") {
				REQUIRE(other_spec != base);
			}

			THEN("They are equivalent") {
				REQUIRE(other_spec.equivalentTo(base));
			}
		}
	}

	SCENARIO("TypeSpec objects report the correct roots", "[gc]") {
		MemoryManager manager;
		Package base_package("test", manager);
		vm_ptr<Symbol> type_name_symb = base_package.intern_symbol("float-32");
		vm_ptr<Symbol> symb_a = base_package.intern_symbol("a");

		PrimitiveType ptype(type_name_symb, "documentation", sizeof(int));
		PrimitiveType p_i32(type_name_symb, "documentation", sizeof(int));

		auto f32_type = manager.allocate_obj<Type>(ptype);
		auto i32_type = manager.allocate_obj<Type>(p_i32);

		SpecBuilder builder;
		builder.add_type(f32_type);
		builder.add_parameter(symb_a);
		builder.add_type(i32_type);
		builder.add_type(i32_type);

		auto spec = builder.build();

		WHEN("The roots are extracted") {
			std::vector<AllocatedItem*> roots;
			spec.get_roots([&roots](AllocatedItem *item) {
				roots.push_back(item);
			});
			THEN("All of the roots are accounted for") {
				std::set<AllocatedItem*> to_check = { f32_type.get(), i32_type.get(), symb_a.get() };
				for (AllocatedItem *item : roots) {
					to_check.erase(item);
				}
				REQUIRE(to_check.size() == 0);
			}
		}
	}
}

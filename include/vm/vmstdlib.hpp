#include <vm/vm.hpp>
#include <iostream>
#include <array>

namespace salmon::vm {

	Box print_list(VirtualMachine *vm, InternalBox list) {
		salmon_check(*list.type == *vm->get_builtin_type<List>(), "Given type is not a list");
		List *first = std::get<List*>(list.elem);
		vm_ptr<Symbol> print_symb = vm->base_package().intern_symbol("print");
		vm_ptr<VmFunction> print_fn = *vm->fn_table.get_fn(print_symb);

		std::array<InternalBox, 1> arg_arr = {first->itm };
		std::span<InternalBox,1> arg_span(arg_arr);

		std::cout << '(';
		(*print_fn)(vm, arg_span);
		for(List *head = first->next; head != nullptr; head = head->next) {
			std::cout << ' ';
			arg_span[0] = head->itm;
			(*print_fn)(vm, arg_span);
		}
		std::cout << ')';
		Box ret(list, vm->mem_manager.make_vm_ptr<AllocatedItem>());
		return ret;
	}

	Box print_array(VirtualMachine *vm, InternalBox box) {
                salmon_check(*box.type == *vm->get_builtin_type<Vector>(), "Given type is not an array");
		vm_ptr<Symbol> print_symb = vm->base_package().intern_symbol("print");
		vm_ptr<VmFunction> print_fn = *vm->fn_table.get_fn(print_symb);
                Vector *arr = std::get<Vector*>(box.elem);

		std::cout << '[';
		auto iter = arr->begin();
		if(iter != arr->end()) {
			std::array<InternalBox, 1> arg_arr = { *iter };
			std::span<InternalBox,1>  arg_span(arg_arr);
			(*print_fn)(vm, arg_span);
			for(iter = std::next(iter); iter != arr->end(); ++iter) {
				std::cout << ' ';
				arg_span[0] = *iter;
				(*print_fn)(vm, arg_span);
			}
		}
		std::cout << ']';
		Box ret(box, vm->mem_manager.make_vm_ptr<AllocatedItem>());
		return ret;
	}

	template<typename T>
	Box print_pointer_primitive(VirtualMachine *vm, InternalBox box) {
		salmon_check(*box.type == *vm->get_builtin_type<T>(), "Given type is not correct");
		std::cout << *std::get<T*>(box.elem);
		Box ret(box, vm->mem_manager.make_vm_ptr<AllocatedItem>());
		return ret;
	}

	template<typename T>
	Box print_primitive(VirtualMachine *vm, InternalBox box) {
		salmon_check(*box.type == *vm->get_builtin_type<T>(), "Given type is not correct");
		std::cout << std::get<T>(box.elem);
		Box ret(box, vm->mem_manager.make_vm_ptr<AllocatedItem>());
		return ret;
	}

	template<typename T>
	Box add(VirtualMachine *vm, InternalBox one, InternalBox two) {
		salmon_check(*one.type == *vm->get_builtin_type<T>()
					 && *two.type == *vm->get_builtin_type<T>(),
					 "Given types are not correct");
		T first = std::get<T>(one.elem);
		T second = std::get<T>(two.elem);
		T result = first + second;

		Box ret(result, vm->get_builtin_type<T>());
		return ret;
	}

	template<typename T>
	Box subtract(VirtualMachine *vm, InternalBox one, InternalBox two) {
		salmon_check(*one.type == *vm->get_builtin_type<T>()
					 && *two.type == *vm->get_builtin_type<T>(),
					 "Given types are not correct");
		T first = std::get<T>(one.elem);
		T second = std::get<T>(two.elem);
		T result = first - second;

		Box ret(result, vm->get_builtin_type<T>());
		return ret;
	}

	template<typename T>
	Box multiply(VirtualMachine *vm, InternalBox one, InternalBox two) {
		salmon_check(*one.type == *vm->get_builtin_type<T>()
					 && *two.type == *vm->get_builtin_type<T>(),
					 "Given types are not correct");
		T first = std::get<T>(one.elem);
		T second = std::get<T>(two.elem);
		T result = first * second;

		Box ret(result, vm->get_builtin_type<T>());
		return ret;
	}

	template<typename T>
	Box divide(VirtualMachine *vm, InternalBox one, InternalBox two) {
		salmon_check(*one.type == *vm->get_builtin_type<T>()
					 && *two.type == *vm->get_builtin_type<T>(),
					 "Given types are not correct");
		T first = std::get<T>(one.elem);
		T second = std::get<T>(two.elem);
		T result = first / second;

		Box ret(result, vm->get_builtin_type<T>());
		return ret;
	}
}

#ifndef SALMON_COMPILER_VM_BUILTINFUNCTION
#define SALMON_COMPILER_VM_BUILTINFUNCTION

#include <vm/function.hpp>

namespace salmon::vm {

	template<typename ... Args>
	class BuiltinFunction : public VmFunction {
	public:
		using FunctionType = Box(*)(VirtualMachine*, Args ...);

		BuiltinFunction(FunctionType fn,
						vm_ptr<Type> type,
						const std::vector<vm_ptr<Symbol>> &lambda_list)
			: BuiltinFunction(fn, type, lambda_list, std::nullopt, std::nullopt) {
		}

		BuiltinFunction(FunctionType fn,
						vm_ptr<Type> type,
						const std::vector<vm_ptr<Symbol>> &lambda_list,
						std::optional<std::string> docs,
						std::optional<std::string> file) :
			VmFunction(type,lambda_list, docs, file, std::nullopt),
			actual_function(fn) {
			salmon_check(lambda_list.size() == sizeof...(Args),
						 "Mismatched lambda list to built-in function");
		}

		~BuiltinFunction() = default;

		Box operator()(VirtualMachine *vm, std::vector<Box> &args) override {
			return unpack_vector(vm, args);
		}

		void print_debug_info() const override {
			std::cerr << "Built-in function " << actual_function;
		}

		size_t allocated_size() const override {
			return sizeof(*this);
		};

	private:

		FunctionType actual_function;

		template<std::size_t... S>
		Box unpack_vector(VirtualMachine *vm, std::vector<Box>& vec, std::index_sequence<S...>) {
			return actual_function(vm, vec[S]...);
		}

		Box unpack_vector(VirtualMachine *vm, std::vector<Box>& vec) {
			if (vec.size() != sizeof...(Args)) {
				throw ArityException::build(vm, _lambda_list, vec.size(), sizeof...(Args));
			}
			return unpack_vector(vm, vec, std::make_index_sequence<sizeof...(Args)>());
		}
	};
}

#endif

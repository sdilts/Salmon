#ifndef SALMON_COMPILER_VM_BUILTINFUNCTION
#define SALMON_COMPILER_VM_BUILTINFUNCTION

#include <cassert>

#include <compiler/vm/function.hpp>

namespace salmon::vm {

	template<typename ... Args>
	class BuiltinFunction : public VmFunction {
	public:
		using FunctionType = Box(*)(Args ...);

		BuiltinFunction(const vm_ptr<Symbol> &name,
						std::vector<vm_ptr<Symbol>> lambda_list,
						std::optional<std::string> docs,
						std::optional<std::string> file,
						FunctionType fn) :
			VmFunction(name, lambda_list, docs, file, std::nullopt),
			actual_function(fn) {
			// TODO: use C++ style assert with exceptions
			assert(lambda_list.size() == arg_count);
		}

		~BuiltinFunction() = default;

		const size_t arg_count = sizeof...(Args);
		Box operator()(std::vector<Box> &args) override {
			return unpack_vector(args);
		}

		void describe() const override { }

	private:

		FunctionType actual_function;

		template<std::size_t... S>
		Box unpack_vector(std::vector<Box>& vec, std::index_sequence<S...>) {
			return actual_function(vec[S]...);
		}

		Box unpack_vector(std::vector<Box>& vec) {
			if (vec.size() != sizeof...(Args)) {
				throw ArityException(vec.size(), sizeof...(Args));
			}
			return unpack_vector(vec, std::make_index_sequence<sizeof...(Args)>());
		}
	};
}

#endif

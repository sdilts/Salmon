#include <compiler/vm/function.hpp>


namespace salmon::vm {

	ArityException::ArityException(size_t num_given, size_t num_desired) :
		std::runtime_error("Wrong number of arguments given"),
		given(num_given),
		desired(num_desired) {}

	VmFunction::VmFunction(const vm_ptr<Symbol> &func_name, std::vector<vm_ptr<Symbol>> lambda_list) :
		name(func_name),
		lambda_list(lambda_list),
		documentation{std::nullopt},
		source_file{std::nullopt} { }

	VmFunction::~VmFunction() {}
}

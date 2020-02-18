#include <compiler/vm/function.hpp>


namespace salmon::vm {

	ArityException::ArityException(const std::vector<vm_ptr<Symbol>> &lambda_list,
								   size_t num_given, size_t num_desired) :
		std::runtime_error("Wrong number of arguments given"),
		lambda_list(lambda_list),
		given(num_given),
		desired(num_desired) {}

	VmFunction::VmFunction(const vm_ptr<Symbol> &name, std::vector<vm_ptr<Symbol>>lambda_list,
						   std::optional<std::string> doc, std::optional<std::string> file,
						   std::optional<vm_ptr<List>> source) :
		name(name),
		lambda_list(lambda_list),
		documentation{doc},
		source_file{file},
		source_form{source}
	{ }

	VmFunction::~VmFunction() {}
}

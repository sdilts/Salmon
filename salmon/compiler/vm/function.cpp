#include <sstream>

#include <compiler/vm/function.hpp>


namespace salmon::vm {

	ArityException ArityException::build(const vm_ptr<Symbol> &func_name,
										 const std::vector<vm_ptr<Symbol>> &lambda_list,
										 const size_t given, const size_t desired) {
		std::stringstream out;
		out << "Wrong number of arguments given to function " << *func_name;
		out << " (given " << given << ", expected " << desired << ")";
		return ArityException(out.str(), func_name, lambda_list, given, desired);
	}

	ArityException::ArityException(const std::string &msg,
								   const vm_ptr<Symbol> &func_name,
								   const std::vector<vm_ptr<Symbol>> &lambda_list,
								   const size_t num_given, const size_t num_desired) :
		std::runtime_error(msg),
		func_name(func_name),
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

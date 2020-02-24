#include <sstream>

#include <vm/function.hpp>


namespace salmon::vm {

	ArityException ArityException::build(const std::vector<vm_ptr<Symbol>> &lambda_list,
										 const size_t given, const size_t desired) {
		std::stringstream out;
		out << "Wrong number of arguments given to function ";
		out << "(given " << given << ", expected " << desired << ")";
		return ArityException(out.str(), lambda_list, given, desired);
	}

	ArityException::ArityException(const std::string &msg,
								   const std::vector<vm_ptr<Symbol>> &lambda_list,
								   const size_t num_given, const size_t num_desired) :
		std::runtime_error(msg),
		lambda_list(lambda_list),
		given(num_given),
		desired(num_desired) {}

	VmFunction::VmFunction(std::vector<vm_ptr<Symbol>>lambda_list,
						   std::optional<std::string> doc, std::optional<std::string> file,
						   std::optional<vm_ptr<List>> source) :
		lambda_list(lambda_list),
		documentation{doc},
		source_file{file},
		source_form{source}
	{}

	VmFunction::~VmFunction() {}

	void VmFunction::describe_helper(const std::string &fn_type, std::ostream &stream) const {
		stream << "is a " << fn_type << " function.\n";
		stream << "  Lambda-list: [";
		// TODO: use array print function instead
		if(lambda_list.size() > 0) {
			stream << *lambda_list[0];
			for(size_t i = 1; i < lambda_list.size(); ++i) {
				stream << " " << *lambda_list[i];
			}
		}
		stream << "]\n";
		if(documentation) {
			stream << "  Documentation\n";
			stream << "  " << *documentation << "\n";
		}
		if(source_form) {
			// TODO: acutally print this out
			stream << "  Source form\n";
		}
		if(source_file) {
			stream << "  Source file: " << *source_file << "\n";
		}
		stream << std::endl;
	}

}

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

	static std::vector<Symbol*> convert(const std::vector<vm_ptr<Symbol>> &list) {
			std::vector<Symbol*> arr;
			arr.reserve(list.size());
			for(auto &item : list) {
				arr.push_back(item.get());
			}
			return arr;
	}

	static std::optional<List*> convert(const std::optional<vm_ptr<List>> &source) {
		if(source) {
			return std::make_optional(source->get());
		} else {
			return std::nullopt;
		}
	}

	VmFunction::VmFunction(const vm_ptr<Type> &type,
						   const std::vector<vm_ptr<Symbol>> &lambda_list,
						   std::optional<std::string> doc, std::optional<std::string> file,
						   const std::optional<vm_ptr<List>> &source) :
		_lambda_list(convert(lambda_list)),
		fn_type{type.get()},
		_documentation{doc},
		_source_file{file},
		_source_form{convert(source)}
	{}

	VmFunction::~VmFunction() {}

	void VmFunction::get_roots(const std::function<void(AllocatedItem*)> &fn) const {
		fn(fn_type);
		if(_source_form) {
			fn(_source_form.value());
		}
	}

	const Type* VmFunction::type() const {
		return fn_type;
	}
	const std::optional<std::string> &VmFunction::documentation() const {
		return _documentation;
	}
	const std::optional<std::string> &VmFunction::source_file() const {
		return _source_file;
	}
	const std::optional<List*> &VmFunction::source_form() const {
		return _source_form;
	}
}

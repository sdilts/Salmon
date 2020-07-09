#include <sstream>

#include <vm/function.hpp>

namespace salmon::vm {

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
						   const std::vector<vm_ptr<Symbol>> &lambda_list) :
		VmFunction(type, lambda_list, std::nullopt, std::nullopt, std::nullopt) {}

	VmFunction::VmFunction(const vm_ptr<Type> &type,
						   const std::vector<vm_ptr<Symbol>> &lambda_list,
						   std::optional<std::string> doc, std::optional<std::string> file,
						   const std::optional<vm_ptr<List>> &source) :
		_lambda_list(convert(lambda_list)),
		fn_type{type.get()},
		_documentation{doc},
		_source_file{file},
		_source_form{convert(source)} {
		salmon_check(std::holds_alternative<FunctionType>(type->type), "type must be a function type");
	}

	VmFunction::~VmFunction() {}

	void VmFunction::get_roots(const std::function<void(AllocatedItem*)> &fn) const {
		fn(fn_type);
		for(Symbol *item : _lambda_list) {
			fn(item);
		}
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

	InterfaceFunction::InterfaceFunction(const vm_ptr<Type> &type,
										 const std::vector<vm_ptr<Symbol>> &lambda_list,
										 std::optional<std::string> doc,
										 std::optional<std::string> file) :
		VmFunction(type, lambda_list, doc, file, std::nullopt)
	{


	}
	InterfaceFunction::InterfaceFunction(const vm_ptr<Type> &type,
										 const std::vector<vm_ptr<Symbol>> &lambda_list) :
		InterfaceFunction(type,lambda_list, std::nullopt, std::nullopt)
	{

	}

	static std::vector<Type*> get_signature(std::vector<Box> &args) {
		std::vector<Type*> ret;
		ret.reserve(args.size());
		for(const auto &item : args) {
			ret.push_back(item.elem_type().get());
		}
		return ret;
	}

	static std::vector<vm_ptr<Type>> vm_ptr_signature(std::vector<Box> &args) {
		std::vector<vm_ptr<Type>> ret;
		ret.reserve(args.size());
		for(const auto &item : args) {
			ret.push_back(item.elem_type());
		}
		return ret;
	}

	Box InterfaceFunction::operator()(VirtualMachine *vm, std::vector<Box> &args)  {
		const std::vector<Type*> arg_types = get_signature(args);
		try {
			// TODO: fix this to not throw an exception if a value isn't found:
			VmFunction *actual = functions.at(arg_types);
			return (*actual)(vm, args);
		} catch(std::out_of_range *ex) {
			std::vector<vm_ptr<Type>> sig = vm_ptr_signature(args);
			throw NoSuchFunction(sig);
		}
	}

	bool InterfaceFunction::add_impl(const vm_ptr<VmFunction> &fn) {
		auto other_fn_type = std::get<FunctionType>(fn->type()->type);
		if(fn->type()->concrete() && std::get<FunctionType>(fn_type->type).match(other_fn_type)) {
			const std::vector<Type*> arg_types = other_fn_type.arg_types();
		    functions.insert_or_assign(arg_types, fn.get());
			return true;
		} else {
			return false;
		}
	}

	void InterfaceFunction::get_roots(const std::function<void(AllocatedItem*)> &inserter) const {
		functions.all_values([&inserter](Type* const&item) {
			inserter(item);
		}, [&inserter](VmFunction* const&item) {
			inserter(item);
		});
		VmFunction::get_roots(inserter);
	}

	void InterfaceFunction::print_debug_info() const {
		std::cerr << "Interface function" << std::endl;
	}

	size_t InterfaceFunction::allocated_size() const { return sizeof(InterfaceFunction); }
}

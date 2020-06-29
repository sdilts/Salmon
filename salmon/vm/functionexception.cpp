#include <vector>

#include <vm/vm.hpp>
#include <util/assert.hpp>
#include <vm/function.hpp>

namespace salmon::vm {

	ArityException ArityException::build(VirtualMachine *vm,
										 const std::vector<Symbol*> &lambda_list,
										 const size_t given, const size_t desired) {
		std::vector<vm_ptr<Symbol>> list;
		list.reserve(lambda_list.size());
		for(const auto &symb : lambda_list) {
			list.push_back(vm->mem_manager.make_vm_ptr(symb));
		}
		std::stringstream out;
		out << "Wrong number of arguments given to function ";
		out << "(given " << given << ", expected " << desired << ")";
		return ArityException(out.str(), list, given, desired);
	}

	ArityException::ArityException(const std::string &msg,
								   const std::vector<vm_ptr<Symbol>> &lambda_list,
								   const size_t num_given, const size_t num_desired) :
		std::runtime_error(msg),
		lambda_list(lambda_list),
		given(num_given),
		desired(num_desired) {}

	NoSuchFunction::NoSuchFunction(std::vector<vm_ptr<Type>> &&sig) :
		std::runtime_error("No such function"),
		_sig(sig),
		_name{std::nullopt} {}

	NoSuchFunction::NoSuchFunction(const std::vector<vm_ptr<Type>> &sig) :
		std::runtime_error("No such function"),
		_sig(sig),
		_name{std::nullopt} {}

	const std::vector<vm_ptr<Type>> &NoSuchFunction::signature() const {
		return _sig;
	}
	const std::optional<vm_ptr<Symbol>> NoSuchFunction::func_name() const {
		return _name;
	}
	void NoSuchFunction::func_name(const vm_ptr<Symbol> &name) {
		salmon_check(!_name.has_value(), "Shouldn't assign a func name twice");
		_name = name;
	}
	void NoSuchFunction::func_name(vm_ptr<Symbol> &&name) {
		salmon_check(!_name.has_value(), "Shouldn't assign a func name twice");
		_name = name;
	}
}

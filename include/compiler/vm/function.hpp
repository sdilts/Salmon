#ifndef SALMON_COMPILER_VM_FUNCTION
#define SALMON_COMPILER_VM_FUNCTION

#include <string>
#include <optional>
#include <memory>
#include <vector>
#include <exception>

#include <compiler/vm/box.hpp>

namespace salmon::vm {

	struct ArityException : std::runtime_error {

		ArityException(size_t num_given, size_t num_desired);
		const size_t given;
		const size_t desired;
	};

	class VmFunction {
	public:
		VmFunction() = delete;
		VmFunction(const vm_ptr<Symbol> &name,std::vector<vm_ptr<Symbol>>lambda_list);
		virtual ~VmFunction();

		const vm_ptr<Symbol> name;

		/**
		 * Call the underlying function with the given arguments
		 *
		 * Objects must be boxed
		 **/
		virtual Box operator()(std::vector<Box> &args) = 0;

		virtual void describe() const = 0;
	private:
		std::vector<vm_ptr<Symbol>> lambda_list;

		std::optional<std::string> documentation;
		std::optional<std::string> source_file;
		std::optional<vm_ptr<List>> source_form;
	};
}

#endif

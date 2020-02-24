#ifndef SALMON_COMPILER_VM_FUNCTION
#define SALMON_COMPILER_VM_FUNCTION

#include <string>
#include <optional>
#include <memory>
#include <vector>
#include <exception>
#include <iostream>

#include <vm/box.hpp>

namespace salmon::vm {

	struct ArityException : std::runtime_error {

		const std::vector<vm_ptr<Symbol>> lambda_list;
		const size_t given;
		const size_t desired;

		static ArityException build(const std::vector<vm_ptr<Symbol>> &lambda_list,
									size_t num_given, size_t num_desired);

	private:
		ArityException(const std::string &msg,
					   const std::vector<vm_ptr<Symbol>> &lambda_list,
					   const size_t num_given, const size_t num_desired);
	};

	class VmFunction {
	public:
		VmFunction() = delete;
		VmFunction(std::vector<vm_ptr<Symbol>>lambda_list,
				   std::optional<std::string> doc, std::optional<std::string> file,
				   std::optional<vm_ptr<List>> source);
		virtual ~VmFunction();

		/**
		 * Call the underlying function with the given arguments.
		 *
		 * @throw ArityException if the vector is the incorrect length.
		 */
		virtual Box operator()(std::vector<Box> &args) = 0;

		virtual void describe(std::ostream &stream=std::cout) const = 0;
	protected:
		void describe_helper(const std::string &fn_type, std::ostream &stream) const;

		// TODO: use boxed object instead?:
		std::vector<vm_ptr<Symbol>> lambda_list;

		std::optional<std::string> documentation;
		std::optional<std::string> source_file;
		std::optional<vm_ptr<List>> source_form;
	};
}

#endif

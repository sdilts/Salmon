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

	class VmFunction : AllocatedItem {
	public:
		VmFunction() = delete;
		VmFunction(const vm_ptr<Type> &type,
				   const std::vector<vm_ptr<Symbol>> &lambda_list,
				   std::optional<std::string> doc, std::optional<std::string> file,
				   const std::optional<vm_ptr<List>> &source);
		virtual ~VmFunction();

		/**
		 * Call the underlying function with the given arguments.
		 *
		 * @throw ArityException if the vector is the incorrect length.
		 */
		virtual Box operator()(std::vector<Box> &args) = 0;

		void get_roots(const std::function<void(AllocatedItem*)> &) const override;

		const Type* type() const;
		const std::optional<std::string> &documentation() const;
		const std::optional<std::string> &source_file() const;
		const std::optional<List*> &source_form() const;
	protected:
		std::vector<Symbol*> _lambda_list;
		Type *fn_type;

		std::optional<std::string> _documentation;
		std::optional<std::string> _source_file;
		std::optional<List*> _source_form;
	};
}

#endif

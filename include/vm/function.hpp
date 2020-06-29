#ifndef SALMON_COMPILER_VM_FUNCTION
#define SALMON_COMPILER_VM_FUNCTION

#include <string>
#include <optional>
#include <memory>
#include <vector>
#include <exception>
#include <iostream>

#include <vm/box.hpp>
#include <util/prefixtrie.hpp>
#include <util/cmpunderlyingtype.hpp>

namespace salmon::vm {

	class VirtualMachine;

	struct ArityException : std::runtime_error {

		const std::vector<vm_ptr<Symbol>> lambda_list;
		const size_t given;
		const size_t desired;

		static ArityException build(VirtualMachine *vm,
									const std::vector<Symbol*> &lambda_list,
									size_t num_given, size_t num_desired);

	private:
		ArityException(const std::string &msg,
					   const std::vector<vm_ptr<Symbol>> &lambda_list,
					   const size_t num_given, const size_t num_desired);
	};

	struct NoSuchFunction : std::runtime_error {
		NoSuchFunction(std::vector<vm_ptr<Type>> &&sig);
		NoSuchFunction(const std::vector<vm_ptr<Type>> &sig);

		const std::vector<vm_ptr<Type>> &signature() const;
		const std::optional<vm_ptr<Symbol>> func_name() const;
		void func_name(const vm_ptr<Symbol> &);
		void func_name(vm_ptr<Symbol> &&);
	private:
		std::vector<vm_ptr<Type>> _sig;
		std::optional<vm_ptr<Symbol>> _name;
	};

	class VmFunction : public AllocatedItem {
	public:
		VmFunction() = delete;
		VmFunction(const vm_ptr<Type> &type,
				   const std::vector<vm_ptr<Symbol>> &lambda_list,
				   std::optional<std::string> doc, std::optional<std::string> file,
				   const std::optional<vm_ptr<List>> &source);
		VmFunction(const vm_ptr<Type> &type,
				   const std::vector<vm_ptr<Symbol>> &lambda_list);
		virtual ~VmFunction();

		/**
		 * Call the underlying function with the given arguments.
		 *
		 * @throw ArityException if the vector is the incorrect length.
		 */
		virtual Box operator()(VirtualMachine *vm, std::vector<Box> &args) = 0;

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

	class InterfaceFunction : public VmFunction {
	public:
		InterfaceFunction(const vm_ptr<Type> &type,
			   const std::vector<vm_ptr<Symbol>> &lambda_list,
			   std::optional<std::string> doc, std::optional<std::string> file);
		InterfaceFunction(const vm_ptr<Type> &type,
			   const std::vector<vm_ptr<Symbol>> &lambda_list);

		Box operator()(VirtualMachine *vm, std::vector<Box> &args) override;

		/**
		 * Add an implementation for this interface.
		 *
		 * If allow_overwrite is true, allow
		 * an existing implementation to be replaced
		 *
		 * @param fn the new implementation
		 * @param allow_overwrite allow an existing implementation to be replaced.
		 * @return whether the new implementation was added.
		 */
		bool add_impl(const vm_ptr<VmFunction> &fn);

		void get_roots(const std::function<void(AllocatedItem*)> &) const override;
		void print_debug_info() const override;
		size_t allocated_size() const override;
	private:
		PrefixTrie<Type*, VmFunction*, cmpUnderlyingType<Type>> functions;
	};
}

#endif

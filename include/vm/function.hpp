#ifndef SALMON_COMPILER_VM_FUNCTION
#define SALMON_COMPILER_VM_FUNCTION

#include <string>
#include <optional>
#include <memory>
#include <vector>
#include <exception>
#include <iostream>
#include <span>

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
		virtual Box invoke(VirtualMachine *vm, std::span<Box> args) {
			std::vector<InternalBox> base;
			base.reserve(args.size());
			std::transform(args.begin(), args.end(), std::back_inserter(base),
						   [](const Box& b) { return b.bare(); });
			return (*this)(vm, {base.data(), base.size()});
		}

		virtual Box operator()(VirtualMachine *vm, std::span<InternalBox> args) = 0;

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

		Box operator()(VirtualMachine *vm, std::span<InternalBox> args) override;

		/**
		 * Add an implementation for this interface.
		 *
		 * This function will overwrite previous implementations
		 *
		 * @param fn the new implementation
		 * @return whether the new implementation was added.
		 */
		bool add_impl(const vm_ptr<VmFunction> &fn);

		void get_roots(const std::function<void(AllocatedItem*)> &) const override;
		void print_debug_info() const override;
		size_t allocated_size() const override;
	private:
		PrefixTrie<Type*, VmFunction*, cmpUnderlyingType<Type>> functions;
	};

	class FunctionTable {
	public:
		FunctionTable();
		//TODO: figure out mechanicsm for changing the signature of functions at rutime.
		/**
		 * Adds or overwrites the function with the given name.
		 *
		 * If a function with the given name already exists, only overwrite it
		 * if the function types are identical.
		 */
		bool add_function(const vm_ptr<Symbol> &name, const vm_ptr<VmFunction> &fn);
		bool add_function(const vm_ptr<Symbol> &name, vm_ptr<VmFunction> &&fn);

		//TODO: figure out mechanicsm for changing the signature of functions at rutime.
		/**
		 * Adds an interface function with the given signature.
		 *
		 * If a interface with that name but with a different type, already exists,
		 * then then nothing happens. If an interface with the same name and type already exists
		 * in the table, then copy over the documentation string and other non-functional parts
		 * of the given function.
		 *
		 * @return whether or not a new implementation was added. If the type is equal to
		 * an implementation that already exists with the given name the return value is still
		 * true.
		 */
		bool new_interface(const vm_ptr<Symbol> &name, const vm_ptr<InterfaceFunction> &fn);

		std::optional<vm_ptr<VmFunction>> get_fn(const vm_ptr<Symbol> &name) const;

	private:
		std::map<vm_ptr<Symbol>, vm_ptr<VmFunction>,cmpUnderlyingType<Symbol>> functions;
		std::map<vm_ptr<Symbol>, vm_ptr<InterfaceFunction>, cmpUnderlyingType<Symbol>> interfaces;
	};
}

#endif

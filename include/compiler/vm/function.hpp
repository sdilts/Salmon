#ifndef SALMON_COMPILER_VM_FUNCTION
#define SALMON_COMPILER_VM_FUNCTION

#include <string>
#include <filesystem>
#include <optional>
#include <memory>
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
		virtual ~VmFunction();

		const vm_ptr<Symbol> name;

		/**
		 * Call the underlying function with the given arguments
		 *
		 * Objects must be boxed
		 **/
		template<typename ... Types>
		virtual Box operator()(Types ... args) = 0;

		virtual void describe() const = 0;
	private:
		vm_ptr<List> lambda_list;

		std::optional<std::string> documentation;
		std::optional<std::string> source_file;
		std::optional<vm_ptr<List>> source_form;
	};

	class NormalFunction : public VmFunction {
		~NormalFunction();
	private:
		std::vector<Type*> arg_types;

	public:
		template<typename ... Types>
		Box operator()(Types ... args) override;
		void describe() const override;
	};

	class InterfaceFunction : public VmFunction {
	public:
		~InterfaceFunction();

		template<typename ... Types>
		Box operator()(Types ... args) override;
		void describe() const override;

	private:
		class ArgTable {
			using TableEntry = std::variant<std::unique_ptr<ArgTable>, std::unique_ptr<VmFunction>>;
			std::unordered_map<Type*, TableEntry> functions;

			bool add_function(vm_ptr<List*> &type_list, VmFunction &function);
			VmFunction &get_function(vm_ptr<List*> type_list);
		};

		ArgTable functions;
	};
}

#endif

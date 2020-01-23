#ifndef SALMON_COMPILER_VM_FUNCTION
#define SALMON_COMPILER_VM_FUNCTION

#include <string>
#include <filesystem>
#include <optional>
#include <memory>

#include <compiler/vm/box.hpp>

namespace salmon::vm {

	using FuncType = std::function<Box(vm_ptr<List>)>;

	class VmFunction {
	public:
		virtual ~VmFunction();

		virtual Box operator()(vm_ptr<List> &args) const = 0;
		virtual void describe() const = 0;
	private:
		vm_ptr<List> lambda_list;

		std::optional<std::string> documentation;
		std::optional<std::filesystem::path> source_file;
		std::optional<vm_ptr<List>> source_form;
	};

	class NormalFunction : public VmFunction {
		~NormalFunction();
	private:
		FuncType function;
		std::vector<Type*> arg_types;

	public:
	    Box operator()(vm_ptr<List> &args) const override;
		void describe() const override;
	};

	class InterfaceFunction : public VmFunction {
	public:
		~InterfaceFunction();
		Box operator()(vm_ptr<List> &args) const override;
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

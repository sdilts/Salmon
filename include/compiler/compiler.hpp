#ifndef SALMON_COMPILER_COMPILER
#define SALMON_COMPILER_COMPILER

#include <compiler/config.hpp>

#include <compiler/vm/vm.hpp>

namespace salmon::compiler {

	struct Compiler {
		Compiler(const CompilerConfig &config);
		Compiler(const Compiler&) = delete;

		CompilerConfig config;

		salmon::vm::VirtualMachine vm;

		bool set_current_package(const std::string &name);

		salmon::vm::Package *current_package();
		salmon::vm::Package *keyword_package();

	private:
		salmon::vm::Package *_current_package;
		salmon::vm::Package *_keyword_package;
	};
}

#endif

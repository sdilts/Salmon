#ifndef SALMON_COMPILER_COMPILER
#define SALMON_COMPILER_COMPILER

#include <salmon/config.hpp>

#include <vm/vm.hpp>

namespace salmon::compiler {

	struct Compiler {
		Compiler(const Config &config);
		Compiler(const Compiler&) = delete;

		Config config;

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

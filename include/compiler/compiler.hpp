#ifndef SALMON_COMPILER_COMPILER
#define SALMON_COMPILER_COMPILER

#include <compiler/config.hpp>

#include <compiler/vm/vm.hpp>

namespace salmon::compiler {

	struct Compiler {
		Compiler(const CompilerConfig &config);
		CompilerConfig config;

		salmon::vm::VirtualMachine vm;
	};
}

#endif

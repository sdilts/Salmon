#ifndef SALMON_COMPILER_COMPILER
#define SALMON_COMPILER_COMPILER

#include <compiler/config.hpp>
#include <compiler/type.hpp>
#include <compiler/vm/memory.hpp>


namespace salmon::compiler {

	class Compiler {
	public:

		Compiler(const CompilerConfig &config);

	private:
		CompilerConfig config;
		TypeTable type_table;

		salmon::vm::MemoryManager gc;
	};
}

#endif

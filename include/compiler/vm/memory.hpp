#ifndef SALMON_COMPILER_VM_MEMORY
#define SALMON_COMPILER_VM_MEMORY

#include <vector>

#include <map>
#include <unordered_set>

#include <compiler/vm/vm_ptr.hpp>

#include <compiler/box.hpp>
#include <compiler/symbol.hpp>

namespace salmon::vm {
	using namespace salmon::compiler;

	class MemoryManager {

	public:
		vm_ptr<Box> make_box();
		vm_ptr<Symbol> make_symbol(const std::string &name);

		void do_gc();
	private:
		std::vector<void*> allocated;
	};
}
#endif

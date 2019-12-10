#ifndef SALMON_COMPILER_VM_MEMORY
#define SALMON_COMPILER_VM_MEMORY

#include <vector>

#include <unordered_set>

#include <compiler/vm/allocateditem.hpp>
#include <compiler/vm/vm_ptr.hpp>

#include <compiler/vm/box.hpp>
#include <compiler/vm/symbol.hpp>

namespace salmon::vm {

	class MemoryManager {

	public:
		vm_ptr<Symbol> make_symbol(const std::string &name);
		vm_ptr<StaticString> make_static_string(const std::string &str);

		void do_gc();
	private:
		std::unordered_set<AllocatedItem*> allocated;
		//! refrence count of allocated objects: kept track of using vm_ptrs
		std::unordered_map<AllocatedItem*, unsigned int> roots;
	};
}
#endif

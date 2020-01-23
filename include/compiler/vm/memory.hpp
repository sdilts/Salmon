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
		MemoryManager() = default;
		~MemoryManager();

		// While it may be possible to copy a MemoryManager, the default implementation is wrong:
		MemoryManager(const Package&) = delete;

		Box make_box();
		vm_ptr<Symbol> make_symbol(const std::string &name);
		vm_ptr<StaticString> make_static_string(const std::string &str);
		vm_ptr<List> make_list(Box &itm);
		vm_ptr<Array> make_array(int32_t size);

		void do_gc();
	private:
		std::unordered_set<AllocatedItem*> allocated;
		//! reference count of allocated objects kept track of using vm_ptrs
		std::unordered_map<AllocatedItem*, unsigned int> roots;
		std::unordered_set<Box*> box_roots;
		size_t total_allocated;
	};
}
#endif

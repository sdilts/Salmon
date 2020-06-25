#ifndef SALMON_COMPILER_VM_MEMORY
#define SALMON_COMPILER_VM_MEMORY

#include <vector>

#include <unordered_set>

#include <vm/allocateditem.hpp>
#include <vm/vm_ptr.hpp>

namespace salmon::vm {

	class MemoryManager {

	public:
		MemoryManager() = default;
		~MemoryManager();

		// While it may be possible to copy a MemoryManager, the default implementation is wrong:
		MemoryManager(const MemoryManager&) = delete;

		template<typename T>
		vm_ptr<T> make_vm_ptr() {
			vm_ptr<T> tmp(nullptr, roots);
			return tmp;
		}

		template<typename T>
		vm_ptr<T> make_vm_ptr(T* item) {
			vm_ptr<T> tmp(item, roots);
			return tmp;
		}

		template<typename T, typename ... ConstructorArgs>
		vm_ptr<T> allocate_obj(ConstructorArgs... args) {
			T *chunk = new T(args...);
			total_allocated += sizeof(T);
			this->allocated.insert(chunk);
			vm_ptr<T> thing(chunk, roots);
			return thing;
		}

		void do_gc();
	private:
		std::unordered_set<AllocatedItem*> allocated;
		//! reference count of allocated objects kept track of using vm_ptrs
		std::unordered_map<AllocatedItem*, unsigned int> roots;
		size_t total_allocated;
	};
}
#endif

#include <vector>
#include <algorithm>

#include <assert.h>
#include <algorithm>

#include <compiler/vm/vm_ptr.hpp>
#include <compiler/vm/memory.hpp>


namespace salmon::vm {

	template<>
	std::map<Symbol*,unsigned int> vm_ptr<Symbol>::instances = {};

	template<>
	std::map<Box*,unsigned int> vm_ptr<Box>::instances = {};

	vm_ptr<Box> MemoryManager::make_box() {
		Box *chunk = new Box();
		this->allocated.insert(chunk);
		vm_ptr<Box> box(chunk);
		return box;
	}

	vm_ptr<Symbol> MemoryManager::make_symbol(const std::string &name) {
		Symbol *chunk = new Symbol(name, std::nullopt);
		this->allocated.insert(chunk);
		vm_ptr<Symbol> symb(chunk);
		return symb;
	}

	/**
	 * this functions implements mark and sweep garbage collection.
	 **/
	void MemoryManager::do_gc() {
		std::cerr << "Before GC: " << allocated.size() << "\n";
		std::unordered_set<AllocatedItem*> marked = {};
		for(Box* root : vm_ptr<Box>::get_instances()) {
			marked.insert(root);
		}
		for(Symbol* root : vm_ptr<Symbol>::get_instances()) {
			marked.insert(root);
		}
		std::vector<AllocatedItem*> to_delete;
		std::set_difference(allocated.begin(), allocated.end(), marked.begin(), marked.end(),
							std::inserter(to_delete,to_delete.begin()));

		for(AllocatedItem *item : to_delete) {
			allocated.erase(item);
			delete item;
		}
		std::cerr << "After GC: " << allocated.size() << "\n";
	}

}

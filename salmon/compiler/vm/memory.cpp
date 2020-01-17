#include <vector>
#include <algorithm>

#include <assert.h>
#include <algorithm>
#include <iostream>

#include <compiler/vm/vm_ptr.hpp>
#include <compiler/vm/memory.hpp>


namespace salmon::vm {

	MemoryManager::~MemoryManager() {
		do_gc();
	}

	vm_ptr<Symbol> MemoryManager::make_symbol(const std::string &name) {
		Symbol *chunk = new Symbol(name, std::nullopt);
		this->allocated.insert(chunk);
		vm_ptr<Symbol> symb(chunk, roots);
		return symb;
	}

	vm_ptr<StaticString> MemoryManager::make_static_string(const std::string &str) {
		StaticString *chunk = new StaticString(str);
		this->allocated.insert(chunk);
		vm_ptr<StaticString> string(chunk, roots);
		return string;
	}

	/**
	 * this functions implements mark and sweep garbage collection.
	 **/
	void MemoryManager::do_gc() {
		std::cerr << "Before GC: " << allocated.size() << "\n";
		std::unordered_set<AllocatedItem*> marked = {};
		for(auto [root, count] : roots) {
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

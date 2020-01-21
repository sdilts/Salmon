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

	Box MemoryManager::make_box() {
		Box box(roots);
		return box;
	}

	vm_ptr<Symbol> MemoryManager::make_symbol(const std::string &name) {
		Symbol *chunk = new Symbol(name, std::nullopt);
		this->allocated.insert(chunk);
		vm_ptr<Symbol> symb(chunk, roots);
		return symb;
	}

	vm_ptr<List> MemoryManager::make_list(Box &itm) {
		List *chunk = new List(itm);
		this->allocated.insert(chunk);
		vm_ptr<List> list(chunk, roots);
		return list;
	}

	vm_ptr<StaticString> MemoryManager::make_static_string(const std::string &str) {
		StaticString *chunk = new StaticString(str);
		this->allocated.insert(chunk);
		vm_ptr<StaticString> string(chunk, roots);
		return string;
	}

	static bool set_contains(const std::unordered_set<AllocatedItem*> &set, AllocatedItem* item) {
		auto itr = set.find(item);
		return itr != set.end();
	}

	static AllocatedItem* set_pop(std::unordered_set<AllocatedItem*> &set) {
		auto itr = set.begin();
		AllocatedItem *itm = *itr;
		set.erase(itr);
		return itm;
	}

	/**
	 * this functions implements mark and sweep garbage collection.
	 **/
	void MemoryManager::do_gc() {
		std::cerr << "Before GC: " << allocated.size() << "\n";
		std::unordered_set<AllocatedItem*> marked = {};
		// TODO: check if breadth-first search would be faster:
		for(auto [root, count] : roots) {
			if(!set_contains(marked, root)) {
				std::unordered_set<AllocatedItem*> to_check;
				to_check.insert(root);
				do {
					AllocatedItem *cur = set_pop(to_check);
					marked.insert(cur);
					std::unordered_set<AllocatedItem*> children = cur->get_roots();
					std::copy_if(children.begin(), children.end(),
								 std::inserter(to_check, to_check.begin()),
								 [&to_check] (AllocatedItem *needle) {
									 return to_check.find(needle) == to_check.end()
										 && marked.find(needle) == marked.end();
								 });
				} while (!to_check.empty());
			}
		}

		std::vector<AllocatedItem*> to_delete;
		std::copy_if(allocated.begin(), allocated.end(),
					 std::inserter(to_delete, to_delete.begin()),
					 [&marked] (AllocatedItem *needle) {
						 return marked.find(needle) == marked.end();
					 });

		for(AllocatedItem *item : to_delete) {
			allocated.erase(item);
			delete item;
		}
		std::cerr << "After GC: " << allocated.size() << "\n";
	}

}

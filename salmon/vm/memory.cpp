#include <vector>
#include <algorithm>

#include <assert.h>
#include <algorithm>
#include <iostream>

#include <vm/vm_ptr.hpp>
#include <vm/memory.hpp>


namespace salmon::vm {

	MemoryManager::~MemoryManager() {
		do_gc();
	}

	Box MemoryManager::make_box() {
		Box box(box_roots);
		return box;
	}

	vm_ptr<Symbol> MemoryManager::make_symbol(const std::string &name) {
		Symbol *chunk = new Symbol(name, std::nullopt);
		total_allocated += sizeof(Symbol);
		this->allocated.insert(chunk);
		vm_ptr<Symbol> symb(chunk, roots);
		return symb;
	}

	vm_ptr<List> MemoryManager::make_list(Box &itm) {
		List *chunk = new List(itm);
		total_allocated += sizeof(List);
		this->allocated.insert(chunk);
		vm_ptr<List> list(chunk, roots);
		return list;
	}

	vm_ptr<Array> MemoryManager::make_array(int32_t size) {
		Array *chunk = new Array(size);
		total_allocated += sizeof(Array);
		this->allocated.insert(chunk);
		vm_ptr<Array> arr(chunk, roots);
		return arr;
	}

	vm_ptr<StaticString> MemoryManager::make_static_string(const std::string &str) {
		StaticString *chunk = new StaticString(str);
		total_allocated += sizeof(StaticString);
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

	static void check_item(AllocatedItem *item,
						   std::unordered_set<AllocatedItem*> &to_check,
						   std::unordered_set<AllocatedItem*> &marked) {
		marked.insert(item);
		std::vector<AllocatedItem*> children = item->get_roots();
		// If a child isn't marked, attempt to add it to to_check:
		for(AllocatedItem *child : children) {
			if(!set_contains(marked, child)) {
				to_check.insert(child);
			}
		}
	}

	/**
	 * this functions implements mark and sweep garbage collection.
	 **/
	void MemoryManager::do_gc() {
		std::cerr << "Before GC: " << allocated.size() << "\n";
		std::unordered_set<AllocatedItem*> marked = {};
		std::unordered_set<AllocatedItem*> to_check;

		for(auto [root, count] : roots) {
			// each root is guaranteed to be unique, so no need to check if it has
			// already been marked.
		    check_item(root, to_check, marked);
		}

		for(Box *box : box_roots) {
			std::vector<AllocatedItem*> children = box->get_roots();
			for(AllocatedItem *child : children) {
				// ensure that marked union to_check = ()
				if(!set_contains(marked, child)) {
					to_check.insert(child);
				}
			}
		}

		while (!to_check.empty()) {
			AllocatedItem *cur = set_pop(to_check);
			// check_item ensures that marked union to_check = (),
			// so no need to check if cur is in marked.
			check_item(cur, to_check, marked);
		}

		std::vector<AllocatedItem*> to_delete;
		std::copy_if(allocated.begin(), allocated.end(),
					 std::inserter(to_delete, to_delete.begin()),
					 [&marked] (AllocatedItem *needle) {
						 return marked.find(needle) == marked.end();
					 });

		for(AllocatedItem *item : to_delete) {
			total_allocated = total_allocated - item->allocated_size();
			allocated.erase(item);
			delete item;
		}
		std::cerr << "After GC: " << allocated.size() << "\n";
	}

}

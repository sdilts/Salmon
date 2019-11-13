#include <vector>
#include <algorithm>

#include <assert.h>

#include <compiler/vm/vm_ptr.hpp>
#include <compiler/vm/memory.hpp>


namespace salmon::vm {

	template<>
	std::map<Box*,unsigned int> vm_ptr<Box>::instances = {};

	Box *MemoryManager::allocate_box() {
		Box *chunk = new Box();
		chunk->tag = this->last_tag;
		this->allocated.push_back(chunk);
		return chunk;
	}

	void MemoryManager::free(Box *box) {
		auto place = std::find(allocated.begin(), allocated.end(), box);
		assert(place != allocated.end());
		allocated.erase(place);
		delete box;
	}

	static void mark_box(Box* root, const unsigned char tag) {
		root->tag = tag;
	}

	static void remove_old(std::vector<Box*> &pointers, const unsigned char current_tag) {
		auto iterator = pointers.begin();
		while(iterator != pointers.end()) {
			Box *item = *iterator;
			if(item->tag != current_tag) {
				iterator = pointers.erase(iterator);
				delete item;
			} else {
				// move onto the next element
				iterator++;
			}
		}
	}

	/**
	 * this functions implements mark and sweep garbage collection.
	 **/
	void MemoryManager::do_gc() {
		const unsigned char new_tag = last_tag + 1;

		// mark each reachable box
		for(Box *root : vm_ptr<Box>::get_instances()) {
			mark_box(root, new_tag);
		}

		remove_old(allocated, new_tag);
		this->last_tag = new_tag;
	}

}

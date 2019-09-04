#include <vector>
#include <algorithm>

#include <assert.h>

#include <compiler/vm/memory.hpp>


namespace salmon::vm {

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

	void MemoryManager::add_root(const std::list<Box*>& root) {
		roots.push_back(std::ref(root));
	}

	void MemoryManager::remove_root(const std::list<Box*>& root) {
		auto place = std::find_if(roots.begin(), roots.end(), [root](const auto &other) {
			return &root == &other.get();
		});
		assert(place != roots.end());
		roots.erase(place);
	}

	void MemoryManager::add_root(const std::map<std::string, Box*>& root) {
		named_roots.push_back(std::ref(root));
	}

	void MemoryManager::remove_root(const std::map<std::string, Box*>& root) {
		auto place = std::find_if(named_roots.begin(), named_roots.end(), [root](const auto &other) {
			return &root == &other.get();
		});
		assert(place != named_roots.end());
		named_roots.erase(place);
	}

	static void mark_box(Box* root, const unsigned char tag) {
		root->tag = tag;
	}

	static void mark_roots(const std::list<Box*> &container, const unsigned char tag) {
		for(const auto &item : container) {
			mark_box(item, tag);
		}
	}

	static void mark_roots(const std::map<std::string,Box*> &container, const unsigned char tag) {
		for(const auto &item : container) {
			mark_box(item.second, tag);
		}
	}

	static void remove_old(std::list<Box*> &pointers, const unsigned char current_tag) {
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
		for(const auto& root : roots) {
			mark_roots(root, new_tag);
		}
		for(const auto& root : named_roots) {
			mark_roots(root, new_tag);
		}

		remove_old(allocated, new_tag);
		this->last_tag = new_tag;
	}

}

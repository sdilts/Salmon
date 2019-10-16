#include <compiler/vm/vm_ptr.hpp>

namespace salmon::vm {

	std::vector<Box*> vm_ptr::get_instances() {
		std::vector<Box*> things;
		for(const auto& [key, value] : vm_ptr::instances) {
			things.push_back(key);
		}
		return things;
	}

	vm_ptr::vm_ptr(std::nullptr_t) : ptr(nullptr), position(vm_ptr::instances.end()) {}

	vm_ptr::vm_ptr(Box *box) : ptr(box) {
		auto [pos, newly_added] = vm_ptr::instances.insert({box, 1});
		position = pos;
		if(!newly_added) {
			pos->second += 1;
		}
	}

	vm_ptr::vm_ptr(const vm_ptr& other) :
		ptr(other.ptr),
		position(other.position) {
		// Unless we point to something, don't do anything:
		if(ptr) {
			position->second += 1;
		}
	}

	vm_ptr::vm_ptr(vm_ptr&& moving) noexcept {
		moving.swap(*this);
	}

	vm_ptr::~vm_ptr() {
		// Unless we point to something, don't do anything:
		if(ptr) {
			position->second -= 1;
			if(position->second == 0) {
				vm_ptr::instances.erase(position);
				position = vm_ptr::instances.end();
			}
		}
	}

	Box& vm_ptr::operator*() const {
		return *ptr;
	}

	Box * vm_ptr::operator->() const {
		return ptr;
	}

	vm_ptr::operator bool() const {
		return ptr != nullptr;
	}

	vm_ptr& vm_ptr::operator=(const vm_ptr &other) {
		if(this != &other) {
			vm_ptr copy(other);
			copy.swap(*this);
		}
		return *this;
	}

	vm_ptr& vm_ptr::operator=(vm_ptr&& moving) noexcept {
		if(this != &moving) {
			moving.swap(*this);
		}
		return *this;
	}

	vm_ptr& vm_ptr::operator=(Box* newPtr) {
		if(ptr != newPtr) {
			vm_ptr tmp(newPtr);
			tmp.swap(*this);
		}
		return *this;
	}

	Box* vm_ptr::get() const {
		return ptr;
	}

	void vm_ptr::swap(vm_ptr& other) noexcept {
		std::swap(ptr, other.ptr);
		std::swap(position, other.position);
	}

	void swap(vm_ptr& first, vm_ptr& second) {
		first.swap(second);
	}
}

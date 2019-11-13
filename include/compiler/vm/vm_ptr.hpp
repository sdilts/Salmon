#ifndef SALMON_COMPILER_VM_VM_PTR
#define SALMON_COMPILER_VM_VM_PTR

#include <map>
#include <vector>

namespace salmon::vm {

	template<typename T>
	class vm_ptr {

	private:

		static typename std::map<T*,unsigned int> instances;

		T *ptr;
		typename std::map<T*,unsigned int>::iterator position;

	public:

		// TODO: this could be costly, we should just return a const reference to the map:
		static std::vector<T*> get_instances()  {
			std::vector<T*> things;
			for(const auto [key, value] : vm_ptr<T>::instances) {
				things.push_back(key);
			}
			return things;
		}

		explicit vm_ptr(T *box) : ptr(box) {
			auto [pos, newly_added] = vm_ptr::instances.insert({box, 1});
			position = pos;
			if(!newly_added) {
				pos->second += 1;
			}
		}

		vm_ptr(std::nullptr_t) : ptr(nullptr), position(vm_ptr<T>::instances.end()) {}

		vm_ptr(const vm_ptr<T>& other)  :
			ptr(other.ptr),
			position(other.position) {
			// Unless we point to something, don't do anything:
			if(ptr) {
				position->second += 1;
			}
		}

		vm_ptr(vm_ptr<T>&& moving) noexcept  {
			moving.swap(*this);
		}

		~vm_ptr()  {
			// Unless we point to something, don't do anything:
			if(ptr) {
				position->second -= 1;
				if(position->second == 0) {
					vm_ptr::instances.erase(position);
					position = vm_ptr::instances.end();
				}
			}
		}

		T& operator*() const {
			return *ptr;
		}

		T* operator->() const {
			return ptr;
		}

		explicit operator bool() const  {
			return ptr != nullptr;
		}

		vm_ptr<T>& operator=(const vm_ptr<T> &other) {
			if(this != &other) {
				vm_ptr copy(other);
				copy.swap(*this);
			}
			return *this;
		}

		vm_ptr& operator=(vm_ptr<T>&& moving) noexcept  {
			if(this != &moving) {
				moving.swap(*this);
			}
			return *this;
		}

		vm_ptr& operator=(T* newPtr)  {
			if(ptr != newPtr) {
				vm_ptr tmp(newPtr);
				tmp.swap(*this);
			}
			return *this;
		}

		T* get() const {
			return ptr;
		}

		void swap(vm_ptr<T>& other) noexcept  {
			std::swap(ptr, other.ptr);
			std::swap(position, other.position);
		}
	};

	template<typename T>
	void swap(vm_ptr<T>& first, vm_ptr<T>& second) {
		first.swap(second);
	}
}

#endif

#ifndef SALMON_COMPILER_VM_VM_PTR
#define SALMON_COMPILER_VM_VM_PTR

#include <unordered_map>
#include <vector>
#include <functional>
#include <type_traits>

#include <vm/allocateditem.hpp>

namespace salmon::vm {

	template<typename T>
	class vm_ptr {

		static_assert(std::is_base_of<AllocatedItem, T>::value);

	private:

	    std::unordered_map<AllocatedItem*,unsigned int> *instances;
	    T *ptr;

	public:

		template<typename> friend class vm_ptr;

		explicit vm_ptr(T *thing, std::unordered_map<AllocatedItem*, unsigned int> &table) :
		    instances{&table},
			ptr(thing) {
			auto [pos, newly_added] = instances->insert({ptr, 1});
			if(!newly_added) {
				pos->second += 1;
			}
		}

		vm_ptr() = delete;

		vm_ptr(std::nullptr_t,  std::unordered_map<AllocatedItem*, unsigned int> &table) :
			instances{&table}, ptr(nullptr) {}

		vm_ptr(const vm_ptr<T>& other) :
			instances{other.instances},
			ptr(other.ptr) {
			// Unless we point to something, don't do anything:
			if(ptr) {
				auto pos = instances->find(ptr);
				pos->second += 1;
			}
		}

		template<typename Other>
		vm_ptr(const vm_ptr<Other> other) :
			instances{other.instances},
			ptr(static_cast<T*>(other.get())) {
			static_assert(std::is_base_of<T, Other>::value);
			// Unless we point to something, don't do anything:
			if(ptr) {
				auto pos = instances->find(ptr);
				pos->second += 1;
			}
		}

		vm_ptr(vm_ptr<T>&& moving) noexcept :
			instances{nullptr},
			ptr(nullptr) {
			moving.swap(*this);
		}

		~vm_ptr()  {
			// Unless we point to something, don't do anything:
			if(ptr) {
				auto pos = instances->find(ptr);
				pos->second -= 1;
				if(pos->second == 0) {
					instances->erase(pos);
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

		vm_ptr& operator=(T *newPtr)  {
			if(ptr != newPtr) {
				vm_ptr tmp(newPtr, std::ref(*instances));
				tmp.swap(*this);
			}
			return *this;
		}

		vm_ptr& operator=(std::nullptr_t)  {
			// Maybe just call the destructor?
			// This is copied from that
			if(ptr) {
				auto pos = instances->find(ptr);
				pos->second -= 1;
				if(pos->second == 0) {
					instances->erase(pos);
				}
				ptr = nullptr;
			}
			return *this;
		}

		T* get() const {
			return ptr;
		}

		void swap(vm_ptr<T>& other) noexcept  {
			std::swap(ptr, other.ptr);
			std::swap(instances, other.instances);
		}
	};

	template<typename T>
	void swap(vm_ptr<T>& first, vm_ptr<T>& second) {
		first.swap(second);
	}

	template<typename T>
	bool operator<(const vm_ptr<T> &lhs, const vm_ptr<T> &rhs) {
		return lhs.get() < rhs.get();
	}

	template<typename T>
	bool operator>(const vm_ptr<T> &lhs, const vm_ptr<T> &rhs) {
		return lhs.get() > rhs.get();
	}

	template<typename T>
	bool operator==(const vm_ptr<T>& lhs, const vm_ptr<T>& rhs) {
			return lhs.get() == rhs.get();
	}

}

namespace std {
	template<typename T>
	struct hash<salmon::vm::vm_ptr<T>> {
		size_t operator()(const salmon::vm::vm_ptr<T>& thing) const {
			return std::hash<T*>{}(thing.get());
		}
	};
}

#endif

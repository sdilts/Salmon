#ifndef SALMON_COMPILER_VM_VM_PTR
#define SALMON_COMPILER_VM_VM_PTR

#include <map>
#include <vector>
#include <compiler/box.hpp>

namespace salmon::vm {

	using namespace salmon::compiler;
	class vm_ptr {

	private:
		static std::map<Box*,unsigned int> instances;

		Box *ptr;
		std::map<Box*,unsigned int>::iterator position;

	public:
		static std::vector<Box*> get_instances();

		explicit vm_ptr(Box *);
		vm_ptr(std::nullptr_t);
		vm_ptr(const vm_ptr& other);
		vm_ptr(vm_ptr&& moving) noexcept;
		~vm_ptr();

		Box& operator*() const;
		Box* operator->() const;

		explicit operator bool() const;

		vm_ptr& operator=(const vm_ptr&);
		vm_ptr& operator=(vm_ptr&&) noexcept;
		vm_ptr& operator=(Box* newPtr);

		Box* get() const;
		void swap(vm_ptr& other) noexcept;
	};
	void swap(vm_ptr& first, vm_ptr& second);
}

#endif

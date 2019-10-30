#ifndef SALMON_COMPILER_VM_MEMORY
#define SALMON_COMPILER_VM_MEMORY

#include <vector>

#include <stdint.h>
#include <map>

#include <compiler/box.hpp>

namespace salmon::vm {
	using namespace salmon::compiler;

	class MemoryManager {

	public:
		Box *allocate_box();
		void free(Box *);

		void do_gc();
	private:
		unsigned char last_tag;
		std::vector<Box*> allocated;
	};
}
#endif

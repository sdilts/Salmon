#ifndef SALMON_COMPILER_VM_MEMORY
#define SALMON_COMPILER_VM_MEMORY

#include <list>
#include <vector>

#include <stdint.h>
#include <map>

#include <compiler/box.hpp>

namespace salmon::vm {
	using namespace salmon::compiler;

	class MemoryManager {

	public:
		Box *allocate_box();
		//Box *calloc(size_t num, size_t size);
		void free(Box *);

		void do_gc();

		void add_root(const std::list<Box*>&);
		void add_root(const std::map<std::string, Box*>&);

		void remove_root(const std::list<Box*>&);
		void remove_root(const std::map<std::string,Box*>&);
	private:
		unsigned char last_tag;
		std::list<Box*> allocated;
		std::vector<std::reference_wrapper<const std::list<Box*>>> roots;
		std::vector<std::reference_wrapper<const std::map<std::string, Box*>>> named_roots;
	};
}
#endif

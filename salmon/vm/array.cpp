#include <iostream>
#include <vm/box.hpp>

namespace salmon::vm {

  Array::Array(int32_t size) :
    items(size) { }

  std::vector<AllocatedItem*> Array::get_roots() const {
    std::vector<AllocatedItem*> to_return;
    for(const InternalBox &box : items) {
      std::vector<AllocatedItem*> children = box.get_roots();
      to_return.insert(to_return.end(), children.begin(), children.end());
    }
    return to_return;
  }

  void Array::print_debug_info() const {
    std::cerr << "Array " << items.size() << " " << this << std::endl;
  }

  size_t Array::allocated_size() const {
    return sizeof(Array);
  }
}

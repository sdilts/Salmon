#include <assert.h>

#include <compiler/vm/type.hpp>

namespace salmon::vm {

	const Type &TypeTable::get_type(const std::string &name) const {
		const auto &item = this->table.find(name);
		assert(item != this->table.end());
		const Type &t = (*item).second;
		return t;
	}

	bool TypeTable::insert(const std::string &name, const Type &type) {
		const auto &item = this->table.find(name);
		if(item != this->table.end()) {
			return false;
		}
		this->table.emplace(std::make_pair(name, type));
		return true;
	}

	std::ostream &operator<<(std::ostream &stream, const Type &type) {
		stream << "<TYPE: \"" << *type.name << "\"";
		bool print_meta = !type.meta.empty();
		if(print_meta) {
			stream << " (:meta";
			for(const auto &item : type.meta) {
				stream << " " << *item.first;
			}
			stream << ")";
		}
		stream << ">";
		return stream;
	}
}

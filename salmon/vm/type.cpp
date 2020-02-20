#include <assert.h>

#include <vm/type.hpp>

namespace salmon::vm {

	std::optional<Type*> TypeTable::get_type(const vm_ptr<Symbol> &name) {
		auto item = this->table.find(name);
		if(item == this->table.end()) {
			return std::nullopt;
		}
		return std::make_optional(&(*item).second);
	}

	bool TypeTable::insert(const Type &type) {
		const auto &item = this->table.find(type.name);
		if(item != this->table.end()) {
			return false;
		}
		this->table.emplace(std::make_pair(type.name, type));
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

#include <assert.h>

#include <compiler/type.hpp>

namespace salmon::compiler {

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
}

#include <assert.h>

#include <vm/symbol.hpp>
#include <vm/new_type.hpp>

namespace salmon::vm {

	//! Helper class for spec_is_concrete
	template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
	template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

	static bool
	spec_is_concrete(const std::vector<TypeSpecification::ItemMask> &types) {
		for(const auto &item : types) {
			bool check = std::visit(overloaded {
					[](const std::shared_ptr<const Type> &arg) {
						return arg->concrete(); },
					[](const vm_ptr<Symbol> &symb) {
						std::ignore = symb;
						return false;
					}
				}, item);

			if(!check) {
				return false;
			}
		}
		return true;
	}

	static std::map<vm_ptr<Symbol>, std::vector<size_t>>
	get_unspecified_types(const std::vector<TypeSpecification::ItemMask> &types) {
		std::map<vm_ptr<Symbol>, std::vector<size_t>> unspecified_types;
		size_t index = 0;
		for(const auto &item : types) {
			auto symbol = std::get_if<vm_ptr<Symbol>>(&item);
			if(symbol) {
				auto loc = unspecified_types.find(*symbol);
				if(loc == unspecified_types.end()) {
					std::vector<size_t> tmp = { index };
					unspecified_types.insert(std::make_pair(*symbol, std::move(tmp)));
				} else {
					auto &vec = std::get<std::vector<size_t>>(*loc);
				    vec.push_back(index);
				}
			}
			index = index + 1;
		}
		return unspecified_types;
	}

	static std::vector<size_t>
	get_concrete_indices(const std::vector<TypeSpecification::ItemMask> &types) {
		std::vector<size_t> indices;
		size_t index = 0;
		for(const auto &item : types) {
			auto type = std::get_if<std::shared_ptr<const Type>>(&item);
			if(type) {
				// TODO: allow unresolved types in type specification
				assert((*type)->concrete());
				indices.push_back(index);
			}
			index = index + 1;
		}
		return indices;
	}

	TypeSpecification::TypeSpecification(const std::vector<TypeSpecification::ItemMask> &types) :
		specification{types},
		unspecified_types(get_unspecified_types(types)),
		concrete_types(get_concrete_indices(types)),
		is_concrete(spec_is_concrete(types)) { }

	bool TypeSpecification::matches(const std::vector<std::shared_ptr<const Type>> &type_list) const {
		if(type_list.size() != specification.size()) {
			return false;
		}
		// Check to make sure the unspecified types match where they should:
		for(const auto &[symb, indicies] : unspecified_types) {
			std::shared_ptr<const Type> type = type_list[indicies[0]];

			for(size_t index = 1; index < indicies.size(); index++) {
				if(type != type_list[indicies[index]]) {
					return false;
				}
			}
		}
		// Now check that the specified types match the given types:
		for(size_t index : concrete_types) {
			auto type = std::get_if<std::shared_ptr<const Type>>(&specification[index]);
			if(*type != type_list[index]) {
				return false;
			}
		}
		return true;
	}

	TypeInterface::~TypeInterface() {}

	PrimitiveType::PrimitiveType(const vm_ptr<Symbol> &name, const std::string &documentation,
								 const size_t size) :
		name{name},
		documentation(documentation),
		num_bytes(size) { }

	PrimitiveType::~PrimitiveType() { }

	size_t PrimitiveType::size() const {
		return num_bytes;
	}

	bool PrimitiveType::concrete() const {
		return true;
	}

	bool PrimitiveType::operator==(const PrimitiveType &other) const {
		return name == other.name;
	}

	bool PrimitiveType::operator!=(const PrimitiveType &other) const {
		return !(*this == other);
	}

	bool TypeSpecification::concrete() const {
		return is_concrete;
	}

	Type::Type(const TypeVar &type) :
		type(type) {

	}

	size_t Type::size() const {
		return std::visit([](auto &&arg) {
			return arg.size();
		}, type);
	}

	bool Type::concrete() const {
		return std::visit([](auto &&arg) {
			return arg.concrete();
		}, type);
	}

	bool Type::operator==(const Type &other) const {
		return type == other.type;
	}

	bool Type::operator!=(const Type &other) const {
		return type != other.type;
	}
}

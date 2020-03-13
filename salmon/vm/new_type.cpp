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

	std::optional<std::map<vm_ptr<Symbol>,std::shared_ptr<const Type>>>
	TypeSpecification::match_symbols(const std::vector<std::shared_ptr<const Type>> &type_list) const {
		if(type_list.size() != specification.size()) {
			return std::nullopt;
		}
		std::map<vm_ptr<Symbol>,std::shared_ptr<const Type>> symb_table;
		// Check to make sure the unspecified types match where they should:
		for(const auto &[symb, indicies] : unspecified_types) {
			std::shared_ptr<const Type> type = type_list[indicies[0]];

			for(size_t index = 1; index < indicies.size(); index++) {
				if(type != type_list[indicies[index]]) {
					return std::nullopt;
				}
			}
			symb_table.insert(std::make_pair(symb, std::move(type)));
		}
		// Now check that the specified types match the given types:
		for(size_t index : concrete_types) {
			auto type = std::get_if<std::shared_ptr<const Type>>(&specification[index]);
			if(*type != type_list[index]) {
				return std::nullopt;
			}
		}
		return std::make_optional(symb_table);
	}

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

	int TypeSpecification::num_types() const {
		return specification.size();
	}

	bool TypeSpecification::operator==(const TypeSpecification &other) const {
		if(specification.size() == other.specification.size()) {
			auto this_iter = specification.begin();
			auto other_iter = other.specification.begin();
			while(this_iter != specification.end()) {
				const auto &this_item = *this_iter;
				const auto &other_item = *other_iter;
				auto this_symb = std::get_if<vm_ptr<Symbol>>(&this_item);
				auto other_symb = std::get_if<vm_ptr<Symbol>>(&other_item);
				if(this_symb && other_symb) {
					return **this_symb == **other_symb;
				} else {
					auto this_type = std::get_if<std::shared_ptr<const Type>>(&this_item);
					auto other_type = std::get_if<std::shared_ptr<const Type>>(&other_item);
					if(this_type && other_type) {
						return **this_type == **other_type;
					} else {
						return false;
					}
				}
				++this_iter;
				++other_iter;
			}
			return true;
		}
		return false;
	}

	bool TypeSpecification::operator!=(const TypeSpecification &other) const {
		return !(*this == other);
	}

	TypeInterface::~TypeInterface() {}

	FunctionType::FunctionType(const TypeSpecification &ret_spec, const TypeSpecification &arg_spec) :
		arg_spec(arg_spec),
		ret_spec(ret_spec) { }
	FunctionType::FunctionType(const std::vector<TypeSpecification::ItemMask> &ret_types,
							   const std::vector<TypeSpecification::ItemMask> &arg_types) :
		arg_spec(arg_types),
		ret_spec(ret_types) { }

	FunctionType::~FunctionType() {}

	bool
	FunctionType::match(const std::vector<std::shared_ptr<const Type>> &type_list) const {
		const std::vector<std::shared_ptr<const Type>> ret_list(type_list.begin(),
																type_list.begin() + arg_spec.num_types());
		const std::vector<std::shared_ptr<const Type>> arg_list(type_list.begin() + arg_spec.num_types(),
																type_list.end());
		auto arg_opt = arg_spec.match_symbols(arg_list);
		if(!arg_opt) {
			return false;
		}
		auto arg_matches = *arg_opt;
		auto ret_opt = ret_spec.match_symbols(ret_list);
		if(!ret_opt) {
			return false;
		}
		auto ret_matches = *ret_opt;
		// The ret unspecified types should be a subset of the argument types:
		return std::all_of(ret_matches.begin(), ret_matches.end(),
						   [&arg_matches](const auto &pair) {
							   auto symb = std::get<0>(pair);
							   const auto iter = arg_matches.find(symb);
							   if(iter != arg_matches.end()) {
								   return *iter == pair;
							   } else return false;
						   });
	}

	int FunctionType::arity() const {
		return arg_spec.num_types();
	}

	size_t FunctionType::size() const {
		// TODO: change to allow cross-compiling
		return sizeof(void*);
	}

	bool FunctionType::concrete() const {
		return arg_spec.concrete() && ret_spec.concrete();
	}

	bool FunctionType::operator==(const FunctionType &other) const {
		bool arg_same = arg_spec == other.arg_spec;
		if(arg_same) {
			return ret_spec == other.ret_spec;
		}
		return false;
	}

	bool FunctionType::operator!=(const FunctionType &other) const {
		return !(*this == other);
	}

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
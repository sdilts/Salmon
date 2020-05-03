#include <vm/typespec.hpp>
#include <vm/type.hpp>
#include <util/assert.hpp>

namespace salmon::vm {
	// Helper class for spec_is_concrete
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
				salmon_check((*type)->concrete(), "All types in spec must be concrete!");
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

	bool TypeSpecification::operator>(const TypeSpecification &other) const {
		if(specification.size() > other.specification.size()) {
			return true;
		}
		auto this_iter = specification.begin();
		auto other_iter = other.specification.begin();
		while(this_iter != specification.end()) {
			const auto &this_item = *this_iter;
			const auto &other_item = *other_iter;
			// TODO: use spaceship operator
			if(this_item > other_item) {
				return true;
			} else if(this_item < other_item) {
				return false;
			}
		}
		// They are equal:
		return false;
	}

	bool TypeSpecification::operator<(const TypeSpecification &other) const {
		if(specification.size() < other.specification.size()) {
			return true;
		}
		auto this_iter = specification.begin();
		auto other_iter = other.specification.begin();
		while(this_iter != specification.end()) {
			const auto &this_item = *this_iter;
			const auto &other_item = *other_iter;
			// TODO: use spaceship operator
			if(this_item < other_item) {
				return true;
			} else if(this_item > other_item) {
				return false;
			}
		}
		// They are equal:
		return false;
	}

	bool TypeSpecification::operator!=(const TypeSpecification &other) const {
		return !(*this == other);
	}

	std::ostream &operator<<(std::ostream &out, const TypeSpecification& spec) {
		auto iter = spec.specification.cbegin();
		if(iter != spec.specification.cend()) {
			const auto &item = *iter;
			std::visit([&out](auto &&item) {
				out << *item;
			}, item);
			++iter;
			while(iter != spec.specification.cend()) {
				const auto &item = *iter;
				std::visit([&out](auto &&item) {
					out << " " << *item;
				}, item);
				++iter;
			}
		}
		return out;
	}

	bool TypeSpecification::concrete() const {
		return is_concrete;
	}
}

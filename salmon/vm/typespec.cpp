#include <vm/typespec.hpp>
#include <vm/type.hpp>
#include <util/assert.hpp>

namespace salmon::vm {

	VariableProperties::VariableProperties(bool constant, bool is_static) :
	        properties{0} {

		if (constant) {
			properties |= CONSTANT_MASK;
		}
		if (is_static) {
			properties |= STATIC_MASK;
		}
	}

        bool VariableProperties::is_static() const {
		return properties & STATIC_MASK;
	}

        bool VariableProperties::is_constant() const {
		return properties & CONSTANT_MASK;
	}

        void SpecBuilder::add_parameter(vm_ptr<Symbol> &param) {
		add_parameter(param, false, false);
	}

        void SpecBuilder::add_parameter(vm_ptr<Symbol> &param, bool constant,
					bool is_static) {
		auto place = parameters.lower_bound(param);
		if( place == parameters.end() || *(*place).first != *param ) {
                        parameters.emplace_hint(place, param, num_elems);
                } else {
			(*place).second.push_back(num_elems);
                }
		num_elems += 1;
		properties.emplace_back(constant, is_static);
		salmon_check(properties.size() == num_elems, "didn't increment num_elems");
        }

        void SpecBuilder::add_type(vm_ptr<Type> &type) {
		add_type(type, false, false);
	}

        void SpecBuilder::add_type(vm_ptr<Type> &type, bool constant, bool is_static) {
		concrete_types.emplace_back(type, num_elems);
		num_elems += 1;
		properties.emplace_back(constant, is_static);
		salmon_check(properties.size() == num_elems, "didn't increment num_elems");
	}

	// TypeSpecification::TypeSpecification(const std::vector<TypeSpecification::ItemMask> &types) :
	// 	specification{types},
	// 	unspecified_types(get_unspecified_types(types)),
	// 	concrete_types(get_concrete_indices(types)),
	// 	is_concrete(spec_is_concrete(types)) { }

	// std::optional<std::map<vm_ptr<Symbol>,std::shared_ptr<const Type>>>
	// TypeSpecification::match_symbols(const std::vector<std::shared_ptr<const Type>> &type_list) const {
	// 	if(type_list.size() != specification.size()) {
	// 		return std::nullopt;
	// 	}
	// 	std::map<vm_ptr<Symbol>,std::shared_ptr<const Type>> symb_table;
	// 	// Check to make sure the unspecified types match where they should:
	// 	for(const auto &[symb, indicies] : unspecified_types) {
	// 		std::shared_ptr<const Type> type = type_list[indicies[0]];

	// 		for(size_t index = 1; index < indicies.size(); index++) {
	// 			if(type != type_list[indicies[index]]) {
	// 				return std::nullopt;
	// 			}
	// 		}
	// 		symb_table.insert(std::make_pair(symb, std::move(type)));
	// 	}
	// 	// Now check that the specified types match the given types:
	// 	for(size_t index : concrete_types) {
	// 		auto type = std::get_if<std::shared_ptr<const Type>>(&specification[index]);
	// 		if(*type != type_list[index]) {
	// 			return std::nullopt;
	// 		}
	// 	}
	// 	return std::make_optional(symb_table);
	// }

	// bool TypeSpecification::matches(const std::vector<std::shared_ptr<const Type>> &type_list) const {
	// 	if(type_list.size() != specification.size()) {
	// 		return false;
	// 	}
	// 	// Check to make sure the unspecified types match where they should:
	// 	for(const auto &[symb, indicies] : unspecified_types) {
	// 		std::shared_ptr<const Type> type = type_list[indicies[0]];

	// 		for(size_t index = 1; index < indicies.size(); index++) {
	// 			if(type != type_list[indicies[index]]) {
	// 				return false;
	// 			}
	// 		}
	// 	}
	// 	// Now check that the specified types match the given types:
	// 	for(size_t index : concrete_types) {
	// 		auto type = std::get_if<std::shared_ptr<const Type>>(&specification[index]);
	// 		if(*type != type_list[index]) {
	// 			return false;
	// 		}
	// 	}
	// 	return true;
	// }

	// int TypeSpecification::num_types() const {
	// 	return specification.size();
	// }

	// bool TypeSpecification::operator==(const TypeSpecification &other) const {
	// 	if(specification.size() == other.specification.size()) {
	// 		auto this_iter = specification.begin();
	// 		auto other_iter = other.specification.begin();
	// 		while(this_iter != specification.end()) {
	// 			const auto &this_item = *this_iter;
	// 			const auto &other_item = *other_iter;
	// 			auto this_symb = std::get_if<vm_ptr<Symbol>>(&this_item);
	// 			auto other_symb = std::get_if<vm_ptr<Symbol>>(&other_item);
	// 			if(this_symb && other_symb) {
	// 				return **this_symb == **other_symb;
	// 			} else {
	// 				auto this_type = std::get_if<std::shared_ptr<const Type>>(&this_item);
	// 				auto other_type = std::get_if<std::shared_ptr<const Type>>(&other_item);
	// 				if(this_type && other_type) {
	// 					return **this_type == **other_type;
	// 				} else {
	// 					return false;
	// 				}
	// 			}
	// 			++this_iter;
	// 			++other_iter;
	// 		}
	// 		return true;
	// 	}
	// 	return false;
	// }

	// bool TypeSpecification::operator>(const TypeSpecification &other) const {
	// 	if(specification.size() > other.specification.size()) {
	// 		return true;
	// 	}
	// 	auto this_iter = specification.begin();
	// 	auto other_iter = other.specification.begin();
	// 	while(this_iter != specification.end()) {
	// 		const auto &this_item = *this_iter;
	// 		const auto &other_item = *other_iter;
	// 		// TODO: use spaceship operator
	// 		if(this_item > other_item) {
	// 			return true;
	// 		} else if(this_item < other_item) {
	// 			return false;
	// 		}
	// 	}
	// 	// They are equal:
	// 	return false;
	// }

	// bool TypeSpecification::operator<(const TypeSpecification &other) const {
	// 	if(specification.size() < other.specification.size()) {
	// 		return true;
	// 	}
	// 	auto this_iter = specification.begin();
	// 	auto other_iter = other.specification.begin();
	// 	while(this_iter != specification.end()) {
	// 		const auto &this_item = *this_iter;
	// 		const auto &other_item = *other_iter;
	// 		// TODO: use spaceship operator
	// 		if(this_item < other_item) {
	// 			return true;
	// 		} else if(this_item > other_item) {
	// 			return false;
	// 		}
	// 	}
	// 	// They are equal:
	// 	return false;
	// }

	// bool TypeSpecification::operator!=(const TypeSpecification &other) const {
	// 	return !(*this == other);
	// }

	// std::ostream &operator<<(std::ostream &out, const TypeSpecification& spec) {
	// 	auto iter = spec.specification.cbegin();
	// 	if(iter != spec.specification.cend()) {
	// 		const auto &item = *iter;
	// 		std::visit([&out](auto &&item) {
	// 			out << *item;
	// 		}, item);
	// 		++iter;
	// 		while(iter != spec.specification.cend()) {
	// 			const auto &item = *iter;
	// 			std::visit([&out](auto &&item) {
	// 				out << " " << *item;
	// 			}, item);
	// 			++iter;
	// 		}
	// 	}
	// 	return out;
	// }

	// bool TypeSpecification::concrete() const {
	// 	return is_concrete;
	// }

	// void SpecBuilder::add_parameter(vm_ptr<Symbol> param) {
	// 	types.push_back(param);
	// }

	// void SpecBuilder::add_type(std::shared_ptr<Type> type) {
	// 	types.push_back(type);
	// }

	// TypeSpecification SpecBuilder::get() {
	// 	TypeSpecification spec(this->types);
	// 	return spec;
	// }
}

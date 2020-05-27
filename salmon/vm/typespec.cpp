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

        std::ostream &VariableProperties::pretty_print(std::ostream &out) const {
		if (is_static()) {
			out << ":static";
			if (is_constant()) {
				out << " :const";
			}
		} else if (is_constant()) {
			out << ":const";
		}
		return out;
        }

        bool VariableProperties::operator==(const VariableProperties &other) const {
		return properties == other.properties;
	}

        bool
        VariableProperties::operator!=(const VariableProperties &other) const {
		return properties != other.properties;
	}

        bool
        VariableProperties::operator>(const VariableProperties &other) const {
		return properties > other.properties;
	}

        bool
        VariableProperties::operator<(const VariableProperties &other) const {
		return properties < other.properties;
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

        TypeSpecification SpecBuilder::build() {
		TypeSpecification spec(parameters, concrete_types, properties);
		return spec;
	}

        static std::map<Symbol *, std::vector<size_t>>
        copy_args(const std::map<vm_ptr<Symbol>, std::vector<size_t>> &args) {
		std::map<Symbol*, std::vector<size_t>> ret;
                for (const auto &val : args) {
			ret.emplace(val.first.get(), val.second);
                }
		return ret;
        }

        static std::vector<std::pair<Type*, size_t>>
        copy_args(const std::vector<std::pair<vm_ptr<Type>, size_t>> &args) {
		std::vector<std::pair<Type*, size_t>> ret;
		ret.reserve(args.size());
                for (const auto &val : args) {
			ret.emplace_back(val.first.get(), val.second);
                }
		return ret;
        }

        TypeSpecification::TypeSpecification(
            std::map<vm_ptr<Symbol>, std::vector<size_t>> &params,
            std::vector<std::pair<vm_ptr<Type>, size_t>> &concrete_types,
            std::vector<VariableProperties> &properties) :
		parameters{copy_args(params)},
		concrete_types{copy_args(concrete_types)},
		properties{properties},
		is_concrete(concrete_types.size() == properties.size()) {

                #ifndef NDEBUG
		size_t sum = 0;
                for (const auto &val : params) {
			sum += val.second.size();
                }
		sum += concrete_types.size();
		salmon_check(sum == properties.size(), "Property size mismatch");
		#endif
	}

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

	size_t TypeSpecification::size() const {
		// Arbitrary container: they all should be the same size.
		return properties.size();
	}

	bool TypeSpecification::operator==(const TypeSpecification &other) const {
		if (std::equal(concrete_types.begin(), concrete_types.end(),
			       other.concrete_types.begin())
		    && properties == other.properties) {
			for (const auto &val : parameters) {
				bool found = false;
				for (const auto &v_other : other.parameters) {
					if (val.second == v_other.second) {
						found = true;
						break;
					}
				}
				if (!found) {
					return false;
				}
			}
			return true;
		} else {
			return false;
		}
        }

	bool TypeSpecification::operator!=(const TypeSpecification &other) const {
	 	return !(*this == other);
	}

	bool TypeSpecification::operator>(const TypeSpecification &other) const {
		return parameters > other.parameters &&
			concrete_types > other.concrete_types &&
			properties > other.properties;
	}

	bool TypeSpecification::operator<(const TypeSpecification &other) const {
		return (parameters < other.parameters)
			&& concrete_types < other.concrete_types
			&& (properties < other.properties);
	}

	using ItemMask = std::variant<Symbol*, Type*>;

	std::ostream &operator<<(std::ostream &out, const TypeSpecification& spec) {
		if(spec.size() > 0) {
			std::vector<ItemMask> to_print;
			to_print.reserve(spec.size());
			for (const auto &[symb, array] : spec.parameters) {
				for (const size_t index : array) {
					to_print[index] = symb;
				}
			}
			for (const auto &[type, index] : spec.concrete_types) {
				to_print[index] = type;
			}

			spec.properties[0].pretty_print(out);
			std::visit([&out](const auto &arg) { out << *arg; },
				   to_print[0]);
			for (size_t i = 1; i < spec.size(); ++i) {
				spec.properties[i].pretty_print(out);
				std::visit([&out](const auto &arg) { out << *arg; },
					   to_print[0]);
			}
                }
                return out;
        }

	bool TypeSpecification::concrete() const {
		return is_concrete;
	}

        void TypeSpecification::get_roots(std::vector<AllocatedItem*> &add) const {
		add.reserve(add.size() + parameters.size() + concrete_types.size());
                for (const auto &val : parameters) {
			add.push_back(val.first);
                }
                for (const auto &val : concrete_types) {
			add.push_back(val.first);
                }
        }
}

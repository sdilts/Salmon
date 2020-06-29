#include <ostream>

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
			std::vector<size_t> vec = { num_elems };
                        parameters.emplace_hint(place, param, std::move(vec));
                } else {
			(*place).second.push_back(num_elems);
                }
		num_elems += 1;
		properties.emplace_back(constant, is_static);
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
			ret.push_back({val.first.get(), val.second});
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

	TypeSpecification::TypeSpecification(std::map<Symbol*, std::vector<size_t>> &&params,
								std::vector<std::pair<Type*, size_t>> &&concrete_types,
								std::vector<VariableProperties> &&properties) :
		parameters(params),
		concrete_types(concrete_types),
		properties(properties),
		is_concrete(this->concrete_types.size() == this->properties.size()) {
	}

	bool TypeSpecification::matches(const TypeSpecification &other) const {
		if(this->size() != other.size()) {
			std::cerr << "sizes are not the same" << std::endl;
			return false;
		}
		std::vector<Type*> other_concrete_types(other.size(),nullptr);
		for(const auto &[type, index] : other.concrete_types) {
			other_concrete_types[index] = type;
		}
		// if other doesn't have concrete types where this one does, the two don't match.
		for(const auto &[type, index] : concrete_types) {
			if(other_concrete_types[index] == nullptr || type != other_concrete_types[index]) {
				return false;
			}
		}
		for(const auto &[type, indices] : parameters) {
			Type *val = other_concrete_types[indices[0]];
			bool all_same = std::all_of(++indices.begin(), indices.end(),
										[val,&other_concrete_types](size_t index) {
											return val == other_concrete_types[index];
										});
			if(all_same) {
				if(val == nullptr) {
					// check to see if any of other's parameter lists match the current
					// parameter list:
					bool found = false;
					for(const auto &[symbol, other_indicies] : other.parameters) {
						if(std::equal(indices.begin(), indices.end(), other_indicies.begin())) {
							found = true;
							break;
						}
					}
					if(!found) {
						return false;
					}
				} else continue;
			} else {
				return false;
			}
		}
		// TODO: check properties:
		return true;
	}

	bool TypeSpecification::matches(const std::vector<vm_ptr<Type>> &type_list) const {
		if(this->size() != type_list.size()) {
			return false;
		}
		// Check to make sure the unspecified types match where they should:
		for(const auto &[symb, indicies] : parameters) {
			const vm_ptr<Type> &type = type_list[indicies[0]];

			for(size_t index = 1; index < indicies.size(); index++) {
				auto cur_type = type_list[indicies[index]];
				if(*type != *cur_type) {
					return false;
				}
			}
		}
		// Now check that the concrete types match the given types:
		for(const auto &[type, index] : concrete_types) {
			if(*type != *type_list[index]) {
				return false;
			}
		}
		return true;
	}

        std::optional<std::map<vm_ptr<Symbol>, vm_ptr<Type>>>
        TypeSpecification::match_symbols(const std::vector<vm_ptr<Type>> &type_list) const {
		if(type_list.size() != this->size()) {
			return std::nullopt;
		}
		std::map<vm_ptr<Symbol>,vm_ptr<Type>> table;
                for (const auto &[symb, indicies] : parameters) {
			vm_ptr<Type> type = type_list[indicies[0]];
			for(size_t index = 1; index < indicies.size(); index++) {
				auto cur_type = type_list[indicies[index]];
				if(*type != *cur_type) {
					return std::nullopt;
				}
			}
			vm_ptr<Symbol> ptr = type.from(symb);
			table.insert(std::make_pair(ptr, type));
                }

		for(const auto &[type, index] : concrete_types) {
			if(*type != *type_list[index]) {
				return std::nullopt;
			}
		}

                return std::make_optional(table);
	}

        size_t TypeSpecification::size() const {
		// Arbitrary container: they all should be the same size.
		return properties.size();
	}

	bool TypeSpecification::equivalentTo(const TypeSpecification &other) const {
		// use equivalent() method for non-concrete types when we get there:
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

	bool TypeSpecification::operator==(const TypeSpecification &other) const {
		// There should only be one instance of each of these instances,
		// so comparing vm_ptrs is okay.
		return std::equal(concrete_types.begin(), concrete_types.end(),
				  other.concrete_types.begin())
			&& parameters == other.parameters
			&& properties == other.properties;
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

			out << '(';
			spec.properties[0].pretty_print(out);
			out << ' ';
			std::visit([&out](const auto &arg) { out << *arg; },
				   to_print[0]);
			out << ')';
			for (size_t i = 1; i < spec.size(); ++i) {
				out << " (";
				spec.properties[i].pretty_print(out);
				out << ' ';
				std::visit([&out](const auto &arg) { out << *arg; },
					   to_print[0]);
				out << ')';
			}
                }
                return out;
        }

	bool TypeSpecification::concrete() const {
		return is_concrete;
	}

	void TypeSpecification::get_roots(const std::function<void(AllocatedItem*)>& inserter) const {
                for (const auto &val : parameters) {
			inserter(val.first);
                }
		// If types appear more than once, they will added twice, but that is
		// (probably) okay.
                for (const auto &val : concrete_types) {
			inserter(val.first);
                }
        }
	TypeSpecification TypeSpecification::combine(const TypeSpecification &first,
												 const TypeSpecification &second) {
		const size_t offset = first.size();
		std::map<Symbol*, std::vector<size_t>> new_params(first.parameters);
		for(const auto &[symb, indicies] : second.parameters) {
			std::vector<size_t> new_indicies;
			new_indicies.reserve(indicies.size());
			std::transform(indicies.begin(), indicies.end(), new_indicies.begin(),
						   [offset](size_t index) { return index + offset; });
			new_params.emplace(symb, new_indicies);
		}
		std::vector<std::pair<Type*,size_t>> new_concrete_types(first.concrete_types);
		new_concrete_types.reserve(new_concrete_types.size() + second.concrete_types.size());
		for(const auto &[type, index] : second.concrete_types) {
			new_concrete_types.push_back(std::make_pair(type, index + offset));
		}
		std::vector<VariableProperties> new_properties;
		new_properties.reserve(first.properties.size() + second.properties.size());
		for(const auto &item : first.properties) {
			new_properties.push_back(item);
		}
		for(const auto &item : second.properties) {
			new_properties.push_back(item);
		}
		TypeSpecification spec(std::move(new_params),
							   std::move(new_concrete_types),
							   std::move(new_properties));
		return spec;
	}
}

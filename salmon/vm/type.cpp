#include <assert.h>

#include <vm/symbol.hpp>
#include <vm/type.hpp>
#include <util/assert.hpp>

namespace salmon::vm {


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

	bool
	FunctionType::match_args(const std::vector<std::shared_ptr<const Type>> &type_list) const {
		return arg_spec.matches(type_list);
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

	bool FunctionType::operator<(const FunctionType &other) const {
		bool arg_smaller = arg_spec < other.arg_spec;
		if(arg_smaller) {
			return true;
		} else {
			return ret_spec < other.ret_spec;
		}
	}

	bool FunctionType::operator>(const FunctionType &other) const {
		bool arg_bigger = arg_spec > other.arg_spec;
		if(arg_bigger) {
			return true;
		} else {
			return ret_spec > other.ret_spec;
		}
	}

	std::ostream &operator<<(std::ostream &out, const FunctionType &fn) {
		std::ignore = fn;
		out << "<TYPE: (fn ";
		out << "[" << fn.arg_spec << "] ";
		out << "(" << fn.ret_spec << ") ";
		return out << ") >";
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

	bool PrimitiveType::operator>(const PrimitiveType &other) const {
		return *name > *other.name;
	}

	bool PrimitiveType::operator<(const PrimitiveType &other) const {
		return *name < *other.name;
	}

	std::ostream &operator<<(std::ostream &out, const PrimitiveType &other) {
		return out << "<TYPE: " << *other.name << ">";
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

	bool Type::operator>(const Type &other) const {
		return type > other.type;
	}

	bool Type::operator<(const Type &other) const {
		return type < other.type;
	}

	std::ostream &operator<<(std::ostream &out, const Type &type) {
		std::visit([&out](auto &&arg) {
			out << arg;
		}, type.type);
		return out;
	}

	std::optional<TypePtr> TypeTable::get_named(const vm_ptr<Symbol> &name) const {
		auto place = named_types.find(name);
		if(place == named_types.end()) {
			return std::nullopt;
		} else {
			return std::get<TypePtr>(*place);
		}
	}

	bool TypeTable::make_alias(const vm_ptr<Symbol> &alias, TypePtr &type) {
		auto place = named_types.find(alias);
		if(place != named_types.end()) {
			return false;
		}
		named_types.insert({alias, type});
		return true;
	}

	TypePtr
	TypeTable::make_primitive(const vm_ptr<Symbol> &name, const std::string &doc, std::size_t size) {
		auto place = named_types.find(name);
		salmon_check(place == named_types.end(),
					 "We shouldn't be overwriting primitive types with `make_primitive`");
		PrimitiveType tmp(name, doc,size);
		auto type_ptr = std::make_shared<Type>(std::move(tmp));
		named_types.insert({name, type_ptr});
		return type_ptr;
	}

	TypePtr
	TypeTable::get_fn_type(const TypeSpecification &arg_types, const TypeSpecification &ret_types) {
		FunctionType fn_t(ret_types, arg_types);
		auto fn_ptr = std::make_shared<Type>(std::move(fn_t));
		auto place = functions.lower_bound(fn_ptr);
		if(place == functions.end() || (*place) != fn_ptr) {
			functions.insert(place, fn_ptr);
			return fn_ptr;
		}
		return *place;
	}
}

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

	static std::unordered_map<vm_ptr<Symbol>, std::vector<size_t>>
	get_unspecified_types(const std::vector<TypeSpecification::ItemMask> &types) {
		std::unordered_map<vm_ptr<Symbol>, std::vector<size_t>> unspecified_types;
		size_t index = 0;
		for(const auto &item : types) {
			auto symbol = std::get_if<vm_ptr<Symbol>>(&item);
			if(symbol) {
				auto loc = unspecified_types.find(*symbol);
				if(loc == unspecified_types.end()) {
					unspecified_types.insert(std::make_pair(*symbol, index));
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

	std::shared_ptr<const Type>
	PrimitiveType::concretize(const std::vector<std::optional<Type*>> type) const {
		std::ignore = type;
		//TODO: throw exception, return this, etc.
		return std::shared_ptr<const Type>();
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

	std::shared_ptr<const Type>
	Type::concretize(const std::vector<std::optional<Type*>> types) const {
		return std::visit([types](auto &&arg) {
			return arg.concretize(types);
		}, type);
	}
}

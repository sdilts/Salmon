#ifndef SALMON_COMPILER_STRING
#define SALMON_COMPILER_STRING

#include <string>
#include <compare>

#include <vm/allocateditem.hpp>

namespace salmon::vm {
	struct StaticString : public AllocatedItem {
		const std::string contents;
		StaticString(const std::string &str);
		StaticString(std::string &&str);
		StaticString() = delete;
		~StaticString() = default;

		void print_debug_info() const override;
		size_t allocated_size() const override;
		auto operator<=>(const StaticString &other) const {
			return contents <=> other.contents;
		}
	};

	std::ostream& operator<<(std::ostream &os, const StaticString &string);
}

#endif

#ifndef SALMON_UTIL_ASSERT
#define SALMON_UTIL_ASSERT

#include <string>
#include <stdexcept>
#include <source_location>

namespace salmon {

	void throw_assert(const char* predicate, const char *file, unsigned int line,
					  bool condition, const std::string &msg);

	struct AssertionException : std::logic_error {
		std::source_location loc;

		friend void salmon::throw_assert(const char* predicate, const char *file, unsigned int line,
								 bool condition, const std::string &msg);

	private:
		AssertionException(const std::string &msg, std::source_location loc);
	};
}

#ifndef NDEBUG
#define salmon_check(cond, msg) salmon::throw_assert(#cond, __FILE__, __LINE__, \
						     static_cast<bool>(cond), msg)
#else
#define salmon_check(cond,msg) static_cast<void>(0)
#endif

#define salmon_ensure(cond, msg) salmon::throw_assert(#cond, __FILE__, __LINE__, \
						      static_cast<bool>(cond), msg)

#define salmon_abort(msg) salmon::throw_assert("false", __FILE__, __LINE__, false, msg)

#endif

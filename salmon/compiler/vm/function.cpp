#include <compiler/vm/function.hpp>


namespace salmon::vm {

	ArityException::ArityException(size_t num_given, size_t num_desired) :
		std::runtime_error("Wrong number of arguments given"),
		given(num_given),
		desired(num_desired) {}

}

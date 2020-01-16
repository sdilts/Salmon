#include <cassert>

#include <compiler/compiler.hpp>

namespace salmon::compiler {

	Compiler::Compiler(const CompilerConfig &config) :
		config{config}, vm{} {

		// TODO: use C++ style assert with exceptions:
		assert(current_package != nullptr);
	}

	bool Compiler::set_current_package(const std::string &name) {
		auto internal_pkg = vm.packages.find(name);
		if(internal_pkg != vm.packages.end()) {
			this->current_package = &internal_pkg->second;
			return true;
		}
		return false;
	}

}

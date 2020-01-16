#include <cassert>

#include <compiler/compiler.hpp>

namespace salmon::compiler {

	Compiler::Compiler(const CompilerConfig &config) :
		config{config}, vm{} {

		// TODO: use C++ style assert with exceptions:
		assert(_current_package != nullptr);
	}

	salmon::vm::Package *Compiler::current_package() {
		return _current_package;
	}

	bool Compiler::set_current_package(const std::string &name) {
		auto internal_pkg = vm.packages.find(name);
		if(internal_pkg != vm.packages.end()) {
			this->_current_package = &internal_pkg->second;
			return true;
		}
		return false;
	}

}

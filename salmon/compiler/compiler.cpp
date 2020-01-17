#include <cassert>

#include <compiler/compiler.hpp>

namespace salmon::compiler {

	static void create_default_packages(Compiler &compiler) {
        // add the default current package to "sal":
		auto internal_pkg = compiler.vm.find_package("sal-int");
		assert(internal_pkg);
		compiler.vm.packages.emplace(std::string("sal"),
									 salmon::vm::Package("sal", compiler.vm.mem_manager,
														 { *internal_pkg }));
		// add the keyword package:
		compiler.vm.packages.emplace(std::string("keyword"),
									 salmon::vm::Package("keyword", compiler.vm.mem_manager));
	}

	Compiler::Compiler(const Config &config) :
		config{config}, vm{"sal-int"} {

		// setup default packages:
		create_default_packages(*this);
		set_current_package("sal");
		auto keyword_pkg = vm.find_package("keyword");
		assert(keyword_pkg);
		_keyword_package = &(*keyword_pkg).get();

		// TODO: use C++ style assert with exceptions:
		assert(_current_package != nullptr && _keyword_package != nullptr);
	}

	salmon::vm::Package *Compiler::current_package() {
		return _current_package;
	}

	salmon::vm::Package *Compiler::keyword_package() {
		return _keyword_package;
	}

	bool Compiler::set_current_package(const std::string &name) {
		auto internal_pkg = vm.find_package(name);
		if(internal_pkg) {
			this->_current_package = &(*internal_pkg).get();
			return true;
		}
		return false;
	}

}

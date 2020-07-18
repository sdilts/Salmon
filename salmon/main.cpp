#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <filesystem>

#include <unistd.h>

#include <replxx.hxx>

#include <util/environment.hpp>
#include <util/assert.hpp>
#include <compiler/parser.hpp>
#include <salmon/config.hpp>
#include <compiler/compiler.hpp>

static salmon::Config get_config() {
	return {
		0, salmon::get_cache_dir(), salmon::get_config_dir(), salmon::get_data_dir()
	};
}

namespace salmon {

	static void process_files(char **filenames, const int length, compiler::Compiler &engine) {
		for(int i = 0; i < length; i++) {
			std::filesystem::path filepath(filenames[i]);
			if(std::filesystem::is_regular_file(filepath)) {
				std::cout << "Processing file " << filepath.string() << std::endl;
				try {
					std::ifstream file;
					file.open(filepath);
					while(auto token = salmon::compiler::read(file, engine)) {
						std::cout << *token->elem_type() << "\n";

						// token = salmon::compiler::read(file, engine);
						engine.vm.mem_manager.do_gc();
					}
					file.close();
				} catch(salmon::compiler::ParseException &error) {
					error.add_file_info(std::filesystem::canonical(filepath));
					std::cout << error.build_error_str() << std::endl;
				}
			} else {
				std::cout << "Cannot process file" << filepath.string() << std::endl;
			}
		}
	}

	static void repl(salmon::compiler::Compiler& engine) {
		using namespace salmon;
		const std::filesystem::path history_file = engine.config.data_dir / "history.txt";

		replxx::Replxx rx;
		rx.install_window_change_handler();

		rx.history_load(history_file.string());
		rx.set_max_history_size(100);

		vm::vm_ptr<vm::VmFunction> print_fn = *engine.vm.fn_table.get_fn(engine.vm.base_package().intern_symbol("print"));
		std::vector<vm::Box> print_args = { engine.vm.make_boxed(vm::Empty()) };
		print_args.reserve(1);
		char const* line{nullptr};
		while((line = rx.input(" > ")) != nullptr) {
			if(line[0] != '\0') {
				try {
					auto token = compiler::read_from_string(line, engine);
					if(token) {
						std::cout << "token: " << *token->elem_type() << std::endl;
						print_args.pop_back();
						print_args.push_back(*token);
						print_fn->invoke(&engine.vm, print_args);
						std::cout << std::endl;
					}
					rx.history_add(line);
					engine.vm.mem_manager.do_gc();
				} catch(const compiler::ParseException &error) {
					std::cout << error.build_error_str() << std::endl;
				}
			}
			line = "";
		}
		std::cout << "\n";
		rx.history_save(history_file);
	}
}

int main(int argc ,char **argv) {
	bool repl_flag = false;
	bool invalid_flag = false;

	int verbosity_level = 0;

	int read_in;
	while( (read_in = getopt(argc, argv, "rv")) != -1) {
		char c = static_cast<char>(read_in);
		switch(c) {
		case 'r':
			repl_flag = true;
			break;
		case '?':
			invalid_flag = true;
			break;
		case 'v':
			if(verbosity_level < salmon::Config::max_verbose_lvl) {
				verbosity_level++;
			} else {
				std::cerr << "Warning: highest verbosity level possible is "
						  << salmon::Config::max_verbose_lvl << std::endl;
			}
			break;
		default:
			salmon_abort("Unknown option in getopt");
		}
	}

	if (invalid_flag) {
		return -1;
	}

	salmon::Config config = get_config();
	config.verbosity_level = verbosity_level;
	if(std::optional<std::error_code> errc = salmon::Config::ensure_required_dirs(config)) {
		std::cerr << "FATAL: Could not create required directories.\n";
		return errc->value();
	}

	salmon::compiler::Compiler engine(config);

	std::cout << "Welcome to salmon version 0.0.1!" << std::endl;

	if (verbosity_level > 0) {
		std::cerr << "\n" << config << std::endl;
	}

	const int num_to_process = argc - optind;

	if(num_to_process) {
		salmon::process_files(argv+optind, num_to_process, engine);
	} else if(!repl_flag) {
		std::cout << "No files specified. Nothing to do.\n";
	}

	if (repl_flag) {
		salmon::repl(engine);
	}

	return 0;
}

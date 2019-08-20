#include <iostream>
#include <stdlib.h>
#include <string>
#include <filesystem>
#include <linenoise.h>

#include <unistd.h>

#include <util/environment.hpp>
#include <compiler/parser.hpp>
#include <config.hpp>

static salmon::CompilerConfig get_config() {
	return {
		0, salmon::get_cache_dir(), salmon::get_config_dir(), salmon::get_data_dir()
	};
}

static void repl(const salmon::CompilerConfig& config) {
	using namespace salmon;
	const std::filesystem::path history_file = config.data_dir / "history.txt";

	linenoiseHistoryLoad(history_file.string().c_str());

	char *line;
	while((line = linenoise(" > ")) != nullptr) {
		if(line[0] != '\0') {

			try {
				std::string token = parser::read_from_string(line);
				std::cout << " token: " << token << std::endl;
				linenoiseHistoryAdd(line);
			} catch(const parser::ParseException &error) {
				std::cout << error.build_error_str() << std::endl;
			}
		}
		free(line);
	}
	if(!linenoiseHistorySave(history_file.string().c_str())) {
		std::cerr << "Failed to save history" << std::endl;
	}
}

int main(int argc ,char **argv) {
	bool repl_flag = false;
	bool invalid_flag = false;

	const int max_verbose_lvl = 3;
	int verbosity_level = 0;

	char c;
	while( (c = getopt(argc, argv, "rv")) != -1) {
		switch(c) {
		case 'r':
			repl_flag = true;
			break;
		case '?':
			invalid_flag = true;
			break;
		case 'v':
			if(verbosity_level < max_verbose_lvl) {
				verbosity_level++;
			} else {
				std::cerr << "Warning: highest verbosity level possible is " << max_verbose_lvl << std::endl;
			}
			break;
		default:
			abort();
		}
	}

	if (invalid_flag) {
		return -1;
	}

	salmon::CompilerConfig config = get_config();
	config.verbosity_level = verbosity_level;

	std::cout << "Welcome to salmon version 0.0.1!" << std::endl;

	if (verbosity_level > 0) {
		std::cerr << "\n" << config << std::endl;
	}

	if (repl_flag) {
		repl(config);
	}

	return 0;
}

#include <iostream>
#include <stdlib.h>
#include <string>
#include <filesystem>
#include <linenoise.h>

#include <unistd.h>

#include "util/environment.hpp"
#include "compiler/parser.hpp"

static void repl(const std::filesystem::path &data_dir) {
	using namespace salmon;
	const std::filesystem::path history_file = data_dir / "history.txt";

	linenoiseHistoryLoad(history_file.string().c_str());

	char *line;
	while((line = linenoise(" > ")) != nullptr) {
		if(line[0] != '\0') {

			try {
				std::string token;
				parser::ReadResult result = parser::read_from_string(line, token);
				std::cout << "Result: " << result << "\n token: " << token << std::endl;
				linenoiseHistoryAdd(line);
			} catch(const parser::ParseException &error) {
				std::cout << error.build_error_str() << std::endl;
			}
		}
		free(line);
	}
	linenoiseHistorySave(history_file.string().c_str());
}

int main(int argc ,char **argv) {
	bool repl_flag = false;
	bool invalid_flag = false;

	const short max_verbose_lvl = 3;
	short verbosity_level = 0;

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

	const std::filesystem::path data_dir = salmon::get_data_dir();
	const std::filesystem::path config_dir = salmon::get_config_dir();
	const std::filesystem::path cache_dir = salmon::get_cache_dir();

	if (verbosity_level > 0) {
		std::cerr << "Verbosity level: " << verbosity_level
				  << "\nUsing cache dir:  " << cache_dir
				  << "\nUsing config dir: " << config_dir
				  << "\nUsing data dir:   " << data_dir<< "\n" << std::endl;
	}

	if (repl_flag) {
		repl(data_dir);
	}

	return 0;
}

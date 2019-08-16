#include <iostream>
#include <stdlib.h>
#include <string>
#include <filesystem>
#include <linenoise.h>

#include <unistd.h>

#include "util/environment.hpp"
#include "compiler/parser.hpp"

static void repl(const std::filesystem::path &config_dir) {
	using namespace salmon;
	const std::filesystem::path history_file = config_dir / "history.txt";

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

	char c;
	while( (c = getopt(argc, argv, "r")) != -1) {
		switch(c) {
		case 'r':
			repl_flag = true;
			break;
		case '?':
			invalid_flag = true;
			break;
		default:
			abort();
		}
	}

	if (invalid_flag) {
		return -1;
	}

	std::filesystem::path config_dir = salmon::get_config_dir();
	std::cout << "Using config dir: " << config_dir << std::endl;

	if (repl_flag) {
		repl(config_dir);
	}

	return 0;
}

#include <iostream>
#include <stdlib.h>
#include <string>
#include <filesystem>
#include <linenoise.h>

#include "util/environment.hpp"
#include "compiler/parser.hpp"

int main() {
  using namespace salmon;
  std::filesystem::path config_dir = get_config_dir();
  std::cout << "Using config dir: " << config_dir << std::endl;

  std::filesystem::path history_file = config_dir / "history.txt";

  linenoiseHistoryLoad(history_file.string().c_str());

  char *line;
  while((line = linenoise(" > ")) != nullptr) {
    if(line[0] != '\0') {
      linenoiseHistoryAdd(line);
      linenoiseHistorySave(history_file.string().c_str());
      std::cout << "You entered " << line  << std::endl;

      std::string token;
      parser::ReadResult result = parser::read_from_string(line, token);
      std::cout << "Result: " << result << "\n token: " << token << std::endl;
    }
    free(line);
  }
  return 0;
}

#pragma once

#include <sstream>
#include <string>

namespace salmon {

  namespace parser {

    enum class ReadResult {
      // built-in seperator tokens:
      L_PAREN,
      R_PAREN,
      L_BRACKET,
      R_BRACKET,
      L_BRACE,
      R_BRACE,
      // some salmon object, usually a symbol
      ITEM,
      END
    };

    std::ostream& operator<<(std::ostream &os, const ReadResult& result);

    ReadResult read(std::istream &input, std::string &item);

    ReadResult read_from_string(std::string input, std::string &item);

  }

}

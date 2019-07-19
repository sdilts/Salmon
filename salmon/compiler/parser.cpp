#include <sstream>
#include <cctype>
#include <string>
#include <iostream>
#include <streambuf>
#include <limits>
#include <set>
#include <cassert>

#include "compiler/parser.hpp"

namespace salmon {

  namespace parser {

    std::ostream& operator<<(std::ostream &os, const ReadResult& result) {
      switch(result) {
      case ReadResult::L_PAREN:
	return os << "ReadResult::L_PAREN";
      case ReadResult::R_PAREN:
	return os << "ReadResult::R_PAREN";
      case ReadResult::L_BRACKET:
	return os << "ReadResult::L_BRACKET";
      case ReadResult::R_BRACKET:
	return os << "ReadResult::R_BRACKET";
      case ReadResult::L_BRACE:
	return os << "ReadResult::L_BRACE";
      case ReadResult::R_BRACE:
	return os << "ReadResult::R_BRACE";
      case ReadResult::ITEM:
	return os << "ReadResult::ITEM";
      case ReadResult::END:
	return os << "ReadResult::END";
      }
      return os << "";
    }

    static void trim_stream(std::istream &input) {
      char ch;
      ch = input.peek();
      while(!(ch == EOF || !std::isspace(static_cast<unsigned char>(ch)))) {
	input.get();
	ch = input.peek();
      }
    }

    static std::string reader_macro(std::istream &input) {
      input.peek();
      std::cerr << "Not implemented at " << __FILE__ << ":" << __LINE__ << std::endl;
      exit(-1);
      return "";
    }

    static char escape(std::istream &input) {
      char ch = input.get();
      switch(ch) {
      case 'a':  return '\a';
      case 'b':  return '\b';
      case 'e':  return '\e';
      case 'f':  return '\f';
      case 'n':  return '\n';
      case 'r':  return '\r';
      case 't':  return '\t';
      case 'v':  return '\v';
      case '\\': return '\\';
      case '\'': return '\'';
      case '"':  return '"';
      default:
	std::cerr << "Invalid escape character: " << ch << std::endl;
	std::cerr << "Implement error handling at " << __FILE__ << ":" << __LINE__ << std::endl;
	exit(-1);
      }
    }

    static std::string parse_string(std::istream &input) {
      // count number of quotes:
      char ch;
      std::size_t count = 0;
      std::ostringstream token;

      while( (ch = input.peek()) && ch == '"') {
	count++;
	input.get();
      };
      std::size_t curCount = 0;
      while( curCount != count && (ch = input.get()) && ch != EOF) {
	if(ch == '\\') {
	  //convert escape to character:
	  token << escape(input);
	} else {
	  token << ch;
	  if (ch == '"') {
	    curCount++;
	  } else {
	    curCount = 0;
	  }
	}
      }
      if(input.eof()) {
	std::cerr << "Implemlement error handling at " << __FILE__ << ": " << __LINE__ << std::endl;
	std::cerr << "Found EOF while parsing string" << std::endl;
	exit(-1);
      }
      std::string toReturn = token.str();
      toReturn.erase(toReturn.length()-count);
      return toReturn;
    }

    static std::string parse_symbol(std::istream &input) {
      static const std::set<char> terminating_chars =
	{ ')', '(', '[', ']', '{', '}', '"'};
      std::ostringstream token;
      char ch;

      while( (ch = input.peek()) && ch != EOF) {
	if(std::isspace(static_cast<unsigned char>(ch)) ||
	   terminating_chars.count(ch) == 1) {
	  break;
	} else {
	  token << ch;
	  input.get();
	}
      }

      std::string toReturn = token.str();
      assert(!toReturn.empty());
      return toReturn;
    }

    static std::string parse_item(std::istream &input) {
      char ch = input.peek();
      switch(ch) {
      case '#':
	return reader_macro(input);
      case '"':
	return parse_string(input);
      default:
	return parse_symbol(input);
      }
    }

    ReadResult lex(std::istream &input, std::string &item) {
      do {
	//consume any preceding whitespace:
	trim_stream(input);
	char ch = input.peek();
	if(ch != EOF) {
	  std::cerr << "Read char: " << ch << "\n";

	  switch(ch) {
	  case ';':
	    // discard the comment:
	    input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	    break;
	  case '(':
	    return ReadResult::L_PAREN;
	  case ')':
	    return ReadResult::R_PAREN;
	  case '[':
	    return ReadResult::L_BRACKET;
	  case ']':
	    return ReadResult::R_BRACKET;
	  case '{':
	    return ReadResult::L_BRACE;
	  case '}':
	    return ReadResult::R_BRACE;
	  default:
	    item = parse_item(input);
	    return ReadResult::ITEM;
	  }
	} else return ReadResult::END;
      } while(true);
    }
  }
}

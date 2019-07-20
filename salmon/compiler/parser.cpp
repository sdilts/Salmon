#include <sstream>
#include <cctype>
#include <string>
#include <iostream>
#include <streambuf>
#include <limits>
#include <set>
#include <list>
#include <cassert>
#include <iterator>

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

    static ReadResult read_next(std::istream &input, std::string &item);

    static std::list<std::string> collect_list(std::istream &input, const ReadResult &terminator) {
      std::list<std::string> items;
      while(true) {
	std::string item;
	ReadResult result = read_next(input, item);
	if(result == ReadResult::ITEM) {
	  items.push_back(item);
	} else if(result == terminator) {
	  return items;
	} else if(result == ReadResult::END) {
	  std::cerr << "EOF reached while parsing\n";
	  std::cerr << "Implemlement error handling at " << __FILE__ << ": " << __LINE__ << std::endl;
	  exit(-1);
	} else {
	  std::cerr << "Unmatched closing character: " << result;
	  std::cerr << "Implemlement error handling at " << __FILE__ << ": " << __LINE__ << std::endl;
	  exit(-1);
	}
      }
    }

    static std::string read_thing(std::istream &input, char start, char end,
					     ReadResult terminator) {
      std::ostringstream collected_items;
      std::list<std::string> possible_output;

      collected_items << " " << start;

      possible_output = collect_list(input, terminator);
      std::copy(possible_output.begin(), possible_output.end(), std::ostream_iterator<std::string>(collected_items, " "));
      collected_items << end;
      std::string result = collected_items.str();
      std::cerr << result << '\n';
      return result;
    }

    static std::string read_list(std::istream &input) {
      return read_thing(input, '(', ')', ReadResult::R_PAREN);
    }

    static std::string read_array(std::istream &input) {
      return read_thing(input, '[', ']', ReadResult::R_BRACKET);
    }

    static std::string read_set(std::istream &input) {
      return read_thing(input, '{', '}', ReadResult::R_BRACE);
    }

    static ReadResult read_next(std::istream &input, std::string &item) {
      do {
	//consume any preceding whitespace:
	trim_stream(input);
	char ch = input.peek();
	if(ch != EOF) {
	  switch(ch) {
	  case ';':
	    // discard the comment:
	    input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	    break;
	  case '(':
	    input.get();
	    item = read_list(input);
	    return ReadResult::ITEM;
	  case ')':
	    input.get();
	    return ReadResult::R_PAREN;
	  case '[':
	    input.get();
	    item = read_array(input);
	    return ReadResult::ITEM;
	  case ']':
	    input.get();
	    return ReadResult::R_BRACKET;
	  case '{':
	    input.get();
	    item = read_set(input);
	    return ReadResult::ITEM;
	  case '}':
	    input.get();
	    return ReadResult::R_BRACE;
	  default:
	    item = parse_item(input);
	    return ReadResult::ITEM;
	  }
	} else return ReadResult::END;
      } while(true);
    }

    ReadResult read(std::istream &input, std::string &item) {
      ReadResult result = read_next(input, item);
      switch(result) {
      case ReadResult::R_PAREN:
      case ReadResult::R_BRACKET:
      case ReadResult::R_BRACE:
	// this is an error:
	std::cerr << "Unmatched closing character: " << result;
	std::cerr << "\nImplemlement error handling at " << __FILE__ << ": " << __LINE__ << std::endl;
	return ReadResult::END;
	break;
      default:
	return result;
      }
    }

    ReadResult read_from_string(std::string input, std::string &item) {
      std::istringstream input_stream(input);
      return read(input_stream, item);
    }
  }
}

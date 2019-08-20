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
#include <utility>
#include <optional>

#include <compiler/parser.hpp>
#include <compiler/CountingStream.hpp>
#include <core/salmon_type.h>

namespace salmon::parser {

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

	ParseException::ParseException(const std::string &msg, const salmon::meta::position_info &start,
								   const salmon::meta::position_info &end) : std::runtime_error(msg), expression_start(start), expression_end(end) {}

	std::string ParseException::build_error_str() const {
		std::stringstream out;
		out << this->runtime_error::what() << "\n";
		out << "Error parsing expression starting at " <<
			this->expression_start.line << ":" << this->expression_start.column;
		out << "\nand ending at " <<
			this->expression_end.line << ":" << this->expression_end.column;
		return out.str();
	}

	void ParseException::add_file_info(const std::filesystem::path &file) {
		source_file = file;
	}

	static std::string build_unmatched_error_str(const std::string &message, const char ch) {
		std::stringstream out;
		out << message;
		out << ch;
		return out.str();
	}

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

	static char toTerminator(const ReadResult &result) {
		switch(result) {
		case ReadResult::L_PAREN:
			return '(';
		case ReadResult::R_PAREN:
			return ')';
		case ReadResult::L_BRACKET:
			return '[';
		case ReadResult::R_BRACKET:
			return ']';
		case ReadResult::L_BRACE:
			return '{';
		case ReadResult::R_BRACE:
			return '}';
		default:
			abort();
		}
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
		std::cerr << "Reader macros aren't implemented yet" << __FILE__ << ":" << __LINE__ << std::endl;
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

	static inline CountingStreamBuffer *tracker_from_stream(const std::istream &stream) {
		return static_cast<CountingStreamBuffer*>(stream.std::ios::rdbuf());
	}

	static std::string parse_string(std::istream &input) {
		CountingStreamBuffer *countStreamBuf = tracker_from_stream(input);

		salmon::meta::position_info start_info = countStreamBuf->positionInfo();
		salmon::meta::position_info end_info;

		// count number of quotes:
		char ch;
		std::size_t num_quotes = 0;
		while( (ch = input.peek()) && ch == '"') {
			num_quotes++;
			input.get();
		};

		std::size_t curCount = 0;
		std::ostringstream token;
		while( curCount != num_quotes && (ch = input.get()) && ch != EOF) {
			if(ch == '\\') {
				//convert escape sequence to character:
				token << escape(input);
			} else {
				if (ch == '"') {
					curCount++;
				} else {
					if (curCount != 0) {
						std::fill_n(std::ostreambuf_iterator(token), curCount, '"');
						curCount = 0;
					}
					token << ch;
				}
			}
		}
		if(input.eof()) {
			end_info = countStreamBuf->positionInfo();
			throw ParseException("EOF reached while parsing string",
								 start_info, end_info);
		}
		std::string toReturn = token.str();
		return toReturn;
	}

	static bool isNumber(const std::string &symbol) {
		bool found_decimal = false;
		for(size_t i = 0; i < symbol.length(); i++) {
			auto c = symbol[i];
			if(c == '.') {
				if(!found_decimal) {
					found_decimal = true;
				} else {
					return false;
				}
			} else if(!std::isdigit(static_cast<unsigned char>(c))) {
				return false;
			}
		}
		return true;
	}

	static bool isKeyword(const std::string &symbol) {
		assert(!symbol.empty());
		return symbol[0] == ':';
	}

	static std::string parse_primitive(std::istream &input) {
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

		if(isNumber(toReturn)) {
			//std::cerr << "Number found " << toReturn << std::endl;
		} else if(isKeyword(toReturn)) {
			// std::cerr << "Keyword found " << toReturn << std::endl;
		} else {
			// std::cerr << "Symbol found " << toReturn << std::endl;
		}

		return toReturn;
	}

	static std::pair<ReadResult, std::optional<std::string>> read_next(std::istream &input);

	static std::list<std::string> collect_list(std::istream &input, const ReadResult &terminator) {
		CountingStreamBuffer *countStreamBuf = tracker_from_stream(input);

		salmon::meta::position_info start_info = countStreamBuf->positionInfo();
		salmon::meta::position_info end_info;

		// consume the starting bracket/brace/etc.
		input.get();

		std::list<std::string> items;
		{
			auto [result, cur_item] = read_next(input);
			while(result != terminator) {
				if(result == ReadResult::ITEM) {
					items.push_back(*cur_item);
				} else if(result == ReadResult::END) {
					end_info = countStreamBuf->positionInfo();
					throw ParseException("EOF reached while parsing",
										 start_info, end_info);
				} else {
					char term_char = toTerminator(result);
					end_info = countStreamBuf->positionInfo();
					throw ParseException(build_unmatched_error_str("Unexpected closing character: ",term_char),
										 start_info, end_info);
				}
				std::tie(result, cur_item) = read_next(input);
			}
		}
		return items;
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

	static std::pair<ReadResult, std::optional<std::string>> read_next(std::istream &input) {
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
					return std::make_pair(ReadResult::ITEM, read_list(input));
				case ')':
					input.get();
					return std::make_pair(ReadResult::R_PAREN, std::nullopt);
				case '[':
					return std::make_pair(ReadResult::ITEM, read_array(input));
				case ']':
					input.get();
					return std::make_pair(ReadResult::R_BRACKET, std::nullopt);
				case '{':
					return std::make_pair(ReadResult::ITEM, read_set(input));
				case '}':
					input.get();
					return std::make_pair(ReadResult::R_BRACE, std::nullopt);
				case '#':
					return std::make_pair(ReadResult::ITEM, reader_macro(input));
				case '"':
					return std::make_pair(ReadResult::ITEM, parse_string(input));
				default:
					return std::make_pair(ReadResult::ITEM, parse_primitive(input));
				}
			} else return std::make_pair(ReadResult::END, std::nullopt);
		} while(true);
	}

	std::string read(std::istream &input) {
		CountingStreamBuffer countStreamBuf(input.rdbuf());
		std::istream inStream(&countStreamBuf);
		assert(tracker_from_stream(inStream) == &countStreamBuf);

		salmon::meta::position_info start_info = countStreamBuf.positionInfo();
		salmon::meta::position_info end_info;

		// if there is an error we need to have the first character, as read_next() consumes it.
		char first_char = inStream.peek();

		auto [result, item] = read_next(inStream);

		switch(result) {
		case ReadResult::R_PAREN:
		case ReadResult::R_BRACKET:
		case ReadResult::R_BRACE:
			// this is an error:
			end_info = countStreamBuf.positionInfo();
			throw ParseException(build_unmatched_error_str("Unexpected closing character: ",
														   first_char),
								 start_info, end_info);
		default:
			return *item;
		}
	}

	std::string read_from_string(std::string input) {
		std::istringstream input_stream(input);
		return read(input_stream);
	}
}

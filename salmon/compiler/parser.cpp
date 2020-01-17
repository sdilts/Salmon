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

namespace salmon::compiler {

	enum class ReadResult {
		// built-in seperator tokens:
		L_PAREN = '(',
		R_PAREN = ')',
		L_BRACKET = '[',
		R_BRACKET = ']',
		L_BRACE = '{',
		R_BRACE = '}',
		// some salmon object, usually a symbol
		ITEM,
		END
	};

	ParseException::ParseException(const std::string &msg, const salmon::meta::position_info &start,
								   const salmon::meta::position_info &end) :
		std::runtime_error(msg),
		expression_start(start),
		expression_end(end) {}

	std::string ParseException::build_error_str() const {
		std::stringstream out;
		out << this->runtime_error::what() << "\n";
		out << "Error parsing expression starting at " <<
			this->expression_start.line << ":" << this->expression_start.column;
		out << "\nand ending at " <<
			this->expression_end.line << ":" << this->expression_end.column;
		if(source_file) {
			out << "\n in file " << *source_file;
		}
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

	static void trim_stream(std::istream &input) {
		int ch;
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

	static CountingStreamBuffer *tracker_from_stream(const std::istream &stream) {
		return static_cast<CountingStreamBuffer*>(stream.std::ios::rdbuf());
	}

	static std::string parse_string(std::istream &input, Compiler &compiler) {
		std::ignore = compiler;
		CountingStreamBuffer *countStreamBuf = tracker_from_stream(input);

		salmon::meta::position_info start_info = countStreamBuf->positionInfo();
		salmon::meta::position_info end_info;

		std::size_t num_quotes = 0;
		// count number of quotes:
		{
			int ch;
			while( (ch = input.peek()) && ch == '"') {
				num_quotes++;
				input.get();
			};
		}

		std::size_t curCount = 0;
		std::ostringstream token;
		int read_in;
		while(curCount != num_quotes && (read_in = input.get()) != EOF) {
			char ch = static_cast<char>(read_in);
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

	static std::string parse_primitive(std::istream &input, Compiler &compiler) {
		std::ignore = compiler
		static const std::set<char> terminating_chars =
			{ '(', ')', '[', ']', '{', '}', '"'};
		std::ostringstream token;
		int read_in;
		// use peek() so terminating chars aren't consumed:
		while( (read_in = input.peek()) && read_in != EOF) {
			char ch = static_cast<char>(read_in);
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

	static std::pair<ReadResult, std::optional<std::string>> read_next(std::istream &input, Compiler &compiler);

	static std::list<std::string> collect_list(std::istream &input, const ReadResult &terminator,
		Compiler &compiler) {
		CountingStreamBuffer *countStreamBuf = tracker_from_stream(input);

		salmon::meta::position_info start_info = countStreamBuf->positionInfo();
		salmon::meta::position_info end_info;

		// consume the starting bracket/brace/etc.
		input.get();

		std::list<std::string> items;
		{
			auto [result, cur_item] = read_next(input, compiler);
			while(result != terminator) {
				if(result == ReadResult::ITEM) {
					items.push_back(*cur_item);
				} else if(result == ReadResult::END) {
					end_info = countStreamBuf->positionInfo();
					throw ParseException("EOF reached while parsing",
										 start_info, end_info);
				} else {
					char term_char = static_cast<char>(result);
					end_info = countStreamBuf->positionInfo();
					throw ParseException(build_unmatched_error_str("Unexpected closing character: ",term_char),
										 start_info, end_info);
				}
				std::tie(result, cur_item) = read_next(input, compiler);
			}
		}
		return items;
	}

	// temporary function to put list items into a string:
	static std::string read_thing(std::istream &input, char start, char end,
								  ReadResult terminator, Compiler &compiler) {
		std::ostringstream collected_items;
		std::list<std::string> possible_output;

		collected_items << start << " ";

		possible_output = collect_list(input, terminator, compiler);
		std::copy(possible_output.begin(), possible_output.end(), std::ostream_iterator<std::string>(collected_items, " "));
		collected_items << end;
		std::string result = collected_items.str();
		return result;
	}

	static std::string read_list(std::istream &input, Compiler &compiler) {
		return read_thing(input, '(', ')', ReadResult::R_PAREN, compiler);
	}

	static std::string read_array(std::istream &input, Compiler &compiler) {
		return read_thing(input, '[', ']', ReadResult::R_BRACKET, compiler);
	}

	static std::string read_set(std::istream &input, Compiler &compiler) {
		return read_thing(input, '{', '}', ReadResult::R_BRACE, compiler);
	}

	static std::pair<ReadResult, std::optional<std::string>> read_next(std::istream &input,
																	   Compiler &compiler) {
		do {
			//consume any preceding whitespace:
			trim_stream(input);
			int read_in = input.peek();

			if(read_in != EOF) {
				char ch = static_cast<char>(read_in);
				switch(ch) {
				case ';':
					// discard the comment:
					input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					break;
				case '(':
					return std::make_pair(ReadResult::ITEM, read_list(input, compiler));
				case ')':
					input.get();
					return std::make_pair(ReadResult::R_PAREN, std::nullopt);
				case '[':
					return std::make_pair(ReadResult::ITEM, read_array(input, compiler));
				case ']':
					input.get();
					return std::make_pair(ReadResult::R_BRACKET, std::nullopt);
				case '{':
					return std::make_pair(ReadResult::ITEM, read_set(input, compiler));
				case '}':
					input.get();
					return std::make_pair(ReadResult::R_BRACE, std::nullopt);
				case '#':
					return std::make_pair(ReadResult::ITEM, reader_macro(input));
				case '"':
					return std::make_pair(ReadResult::ITEM, parse_string(input, compiler));
				default:
					return std::make_pair(ReadResult::ITEM, parse_primitive(input, compiler));
				}
			} else {
				// push the stream so the eof bit is set
				input.get();
				return std::make_pair(ReadResult::END, std::nullopt);
			}
		} while(true);
	}

	std::optional<std::string> read(std::istream &input, Compiler &compiler) {
		CountingStreamBuffer countStreamBuf(input.rdbuf());
		std::istream inStream(&countStreamBuf);
		assert(tracker_from_stream(inStream) == &countStreamBuf);

		salmon::meta::position_info start_info = countStreamBuf.positionInfo();
		salmon::meta::position_info end_info;

		// if there is an error we need to have the first character, as read_next() consumes it.
		char first_char = inStream.peek();

		const auto [result, item] = read_next(inStream, compiler);

		switch(result) {
		case ReadResult::R_PAREN:
		case ReadResult::R_BRACKET:
		case ReadResult::R_BRACE:
			// this is an error:
			end_info = countStreamBuf.positionInfo();
			throw ParseException(build_unmatched_error_str("Unexpected closing character: ",
														   first_char),
								 start_info, end_info);
		case ReadResult::END:
			// push  the origin stream past the end of the stream so it's
			// eof bit is set.
			input.get();
			[[fallthrough]];
		default:
			return item;
		}
	}

	std::optional<std::string> read_from_string(const std::string& input, Compiler &compiler) {
		std::istringstream input_stream(input);
		return read(input_stream, compiler);
	}
}

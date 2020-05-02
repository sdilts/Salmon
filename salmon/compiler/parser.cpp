#include <sstream>
#include <cctype>
#include <string>
#include <iostream>
#include <streambuf>
#include <limits>
#include <climits>
#include <set>
#include <list>
#include <iterator>
#include <utility>
#include <optional>
#include <algorithm>

#include <util/assert.hpp>
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

	static salmon::vm::Box parse_string(std::istream &input, Compiler &compiler) {
		CountingStreamBuffer *countStreamBuf = tracker_from_stream(input);

		const salmon::meta::position_info start_info = countStreamBuf->positionInfo();
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

		if(num_quotes == 2) {
			goto end;
		}

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

	end:

		auto str = compiler.vm.mem_manager.allocate_obj<vm::StaticString>(token.str());
		vm::Box box(str);
		box.type = compiler.vm.const_str_type();

		return box;
	}

	enum class NumberType {
		NOT_A_NUM,
		FLOAT,
		INTEGER
	};

	static NumberType get_num_type(const std::string &symbol) {
		NumberType num_type = NumberType::INTEGER;
		for(size_t i = 0; i < symbol.length(); i++) {
			auto c = symbol[i];
			if(c == '.') {
				if(num_type == NumberType::INTEGER) {
				    num_type = NumberType::FLOAT;
				} else {
					// there are more than two '.', not a number
					return NumberType::NOT_A_NUM;
				}
			} else if(!std::isdigit(static_cast<unsigned char>(c))) {
				return NumberType::NOT_A_NUM;
			}
		}
		return num_type;
	}

	static std::string read_atom(std::istream &input) {
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
		return token.str();
	}

	salmon::vm::Box read_uninterned_symbol(std::istream &input, Compiler &compiler) {
		CountingStreamBuffer *countStreamBuf = tracker_from_stream(input);

		const salmon::meta::position_info start_info = countStreamBuf->positionInfo();

		std::string chunk = read_atom(input);

		if(chunk.empty()) {
			throw ParseException("Reached EOF while parsing reader macro #:",
			 					 start_info, countStreamBuf->positionInfo());
		} else if(NumberType type = get_num_type(chunk); type != NumberType::NOT_A_NUM) {
			throw ParseException("Encountered number while parsing reader macro #:",
								 start_info, countStreamBuf->positionInfo());
		} else if(chunk.find(':') != std::string::npos) {
			throw ParseException("Encountered package prefix or keyword while parsing reader macro #:",
								 start_info, countStreamBuf->positionInfo());
		} else {
			vm::vm_ptr<vm::Symbol> tmp_symb = compiler.vm.mem_manager.allocate_obj<vm::Symbol>(chunk);
			vm::Box box(tmp_symb);
			box.type = compiler.vm.symbol_type();
			return box;
		}
	}

	struct HexTable {
		unsigned int tab[128];
		constexpr HexTable() : tab {} {
			tab[static_cast<int>('1')] = 1; tab[static_cast<int>('2')] = 2;
			tab[static_cast<int>('3')] = 3; tab[static_cast<int>('4')] = 4;
			tab[static_cast<int>('5')] = 5; tab[static_cast<int>('6')] = 6;
			tab[static_cast<int>('7')] = 7; tab[static_cast<int>('8')] = 8;
			tab[static_cast<int>('9')] = 9; tab[static_cast<int>('a')] = 10;
			tab[static_cast<int>('A')] = 10; tab[static_cast<int>('b')] = 11;
			tab[static_cast<int>('B')] = 11; tab[static_cast<int>('c')] = 12;
			tab[static_cast<int>('C')] = 12; tab[static_cast<int>('d')] = 13;
			tab[static_cast<int>('D')] = 13; tab[static_cast<int>('e')] = 14;
			tab[static_cast<int>('E')] = 14; tab[static_cast<int>('f')] = 15;
			tab[static_cast<int>('F')] = 15;
		}
		constexpr unsigned int operator[](char const idx) const {
			return tab[static_cast<std::size_t>(idx)];
		}
	} constexpr table;

	constexpr unsigned int hextoint(char number) {
		return table[(std::size_t)number];
	}

	salmon::vm::Box read_hex_atom(std::istream &input, Compiler &compiler) {
		CountingStreamBuffer *countStreamBuf = tracker_from_stream(input);

		const salmon::meta::position_info start_info = countStreamBuf->positionInfo();

		std::string chunk = read_atom(input);

		if(chunk.empty()) {
			throw ParseException("Reached EOF while parsing reader macro #x",
			 					 start_info, countStreamBuf->positionInfo());
		}

		bool is_valid_hex = std::all_of(chunk.begin(), chunk.end(), [](const auto c) {
			salmon_check(c != EOF, "Character in string is EOF char");
			return std::isxdigit(static_cast<unsigned char>(c));
		});
		if(!is_valid_hex)  {
			// TODO: implement error handling here.
			std::cerr << "Invalid hex string: " << chunk << '\n';
			std::cerr << "Implement error handling at " << __FILE__ << ":" << __LINE__ << std::endl;
			exit(-1);
		}

		// FIXME: check if it needs more than 32 bytes and use the smallest possible type
		if(chunk.size() > 8) {
			// value is too big!
			std::cerr << "Hex string is too big to fit into a 32 bit integer: #x" << chunk << '\n';
			std::cerr << "Implement error handling at " << __FILE__ << ":" << __LINE__ << std::endl;
			exit(-1);
		}
		uint32_t sum = 0;
		for(const auto c : chunk) {
		    uint32_t digit = table[static_cast<std::size_t>(c)];
			sum <<= 4;
			sum |= digit;
		}

		// TODO: add unsigned integers:
		vm::Box box(compiler.vm.mem_manager.make_vm_ptr<vm::AllocatedItem>());
		box.set_value(static_cast<int32_t>(sum));
		box.type = compiler.vm.int32_type();
		return box;
	}

	static salmon::vm::Box reader_macro(std::istream &input, Compiler &compiler) {
		CountingStreamBuffer *countStreamBuf = tracker_from_stream(input);

		const salmon::meta::position_info start_info = countStreamBuf->positionInfo();
		// calling function doesn't consume '#' token:
		input.get();

		int ch = input.get();

		switch(ch) {
		case ':':
			return read_uninterned_symbol(input, compiler);
		case 'x':
			return read_hex_atom(input, compiler);
		case '\n':
			throw ParseException("Reached EOF while parsing reader macro",
								 start_info, countStreamBuf->positionInfo());
		case EOF:
			throw ParseException("Reached EOF while parsing reader macro",
								 start_info, countStreamBuf->positionInfo());
		default:
			throw ParseException(build_unmatched_error_str("Unkown macro dispatch character: ", ch),
								 start_info, countStreamBuf->positionInfo());
		}
	}

	static bool isKeyword(const std::string &symbol) {
		salmon_check(!symbol.empty(), "Symbol isn't supposed to be empty");
		return symbol[0] == ':';
	}

	static salmon::vm::Box parse_primitive(std::istream &input, Compiler &compiler) {

		std::string chunk = read_atom(input);
		salmon_check(!chunk.empty(), "Atom shouldn't be empty");

		salmon::vm::Box box(compiler.vm.mem_manager.make_vm_ptr<vm::AllocatedItem>());
		if(NumberType type = get_num_type(chunk); type != NumberType::NOT_A_NUM) {
			switch(type) {
			case NumberType::FLOAT:
				box.set_value(std::stof(chunk));
				box.type = compiler.vm.float_type();
				break;
			case NumberType::INTEGER:
				box.set_value(std::stoi(chunk));
				box.type = compiler.vm.int32_type();
				break;
			case NumberType::NOT_A_NUM:
				salmon_abort("Primitive type should always be a number");
			}
		} else if(isKeyword(chunk)) {
			auto symb = compiler.keyword_package()->intern_symbol(chunk.substr(1));
			box.set_value(symb);
			box.type = compiler.vm.symbol_type();
		} else {
			auto symb = compiler.current_package()->intern_symbol(chunk);
			box.set_value(symb);
			box.type = compiler.vm.symbol_type();
		}

		return box;
	}

	static std::pair<ReadResult, std::optional<salmon::vm::Box>>
	read_next(std::istream &input, Compiler &compiler);

	static std::list<salmon::vm::Box> collect_list(std::istream &input, const ReadResult &terminator,
		Compiler &compiler) {
		CountingStreamBuffer *countStreamBuf = tracker_from_stream(input);

		const salmon::meta::position_info start_info = countStreamBuf->positionInfo();
		salmon::meta::position_info end_info;

		// consume the starting bracket/brace/etc.
		int opening_char = input.get();

		std::list<salmon::vm::Box> items;
		{
			auto [result, cur_item] = read_next(input, compiler);
			while(result != terminator) {
				if(result == ReadResult::ITEM) {
					salmon_check(cur_item != std::nullopt, "Unexpected std::nullopt");
					items.push_back(*cur_item);
				} else if(result == ReadResult::END) {
					end_info = countStreamBuf->positionInfo();
					throw ParseException(build_unmatched_error_str("EOF reached while parsing ",
																   opening_char),
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

	static salmon::vm::Box read_list(std::istream &input, Compiler &compiler) {
		std::list<salmon::vm::Box> collected_items = collect_list(input, ReadResult::R_PAREN, compiler);
		if (!collected_items.empty()) {
			vm::Box first = collected_items.front();
			vm::vm_ptr<vm::List> head = compiler.vm.mem_manager.allocate_obj<vm::List>(first);
			collected_items.pop_front();
			vm::vm_ptr<vm::List> tail = head;
			for(salmon::vm::Box &box : collected_items) {
				vm::vm_ptr<vm::List> next = compiler.vm.mem_manager.allocate_obj<vm::List>(box);
				tail->next = &*next;
				tail = next;
			}
			vm::Box box(head);
			box.type = compiler.vm.list_type();
			return box;
		} else {
			vm::Box box(compiler.vm.mem_manager.make_vm_ptr<vm::AllocatedItem>());
			vm::Empty empty;
			box.type = compiler.vm.empty_type();
			box.set_value(empty);
			return box;
		}
	}

	static salmon::vm::Box read_array(std::istream &input, Compiler &compiler) {
		std::list<salmon::vm::Box> collected_items = collect_list(input, ReadResult::R_BRACKET, compiler);
		vm::vm_ptr<vm::Array> array = compiler.vm.mem_manager.allocate_obj<vm::Array>(collected_items.size());
		for(salmon::vm::Box &box : collected_items) {
			array->push_back(box);
		}
		vm::Box box(array);
		box.type = compiler.vm.dyn_array_type();
		return box;
	}

	static std::pair<ReadResult, std::optional<salmon::vm::Box>> read_next(std::istream &input,
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
					std::cerr << "Reading sets aren't implemented yet"
							  << __FILE__ << ":" << __LINE__ << std::endl;
					exit(-1);
					return std::make_pair(ReadResult::R_BRACE, std::nullopt);
				case '}':
					input.get();
					return std::make_pair(ReadResult::R_BRACE, std::nullopt);
				case '#':
					return std::make_pair(ReadResult::ITEM, reader_macro(input, compiler));
				case '"':
					return std::make_pair(ReadResult::ITEM,
										  parse_string(input, compiler));
				default:
					return std::make_pair(ReadResult::ITEM,
										  parse_primitive(input, compiler));
				}
			} else {
				// push the stream so the eof bit is set
				input.get();
				return std::make_pair(ReadResult::END, std::nullopt);
			}
		} while(true);
	}

	std::optional<salmon::vm::Box> read(std::istream &input, Compiler &compiler) {
		CountingStreamBuffer countStreamBuf(input);
		std::istream inStream(&countStreamBuf);
		salmon_check(tracker_from_stream(inStream) == &countStreamBuf, "tracker_from_stream works");

		const salmon::meta::position_info start_info = countStreamBuf.positionInfo();
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

	std::optional<salmon::vm::Box> read_from_string(const std::string& input, Compiler &compiler) {
		std::istringstream input_stream(input);
		return read(input_stream, compiler);
	}
}

#pragma once

#include <sstream>
#include <string>
#include <exception>
#include <filesystem>

#include "compiler/CountingStream.hpp"

namespace salmon::parser {

	/***
	 * A parse exception holds data for:
	 *   start_info:    the start of the form where the error occured
	 *   end_info: where the actual error occured
	 *   file_info:     the name of the source file (if available) where the error took place.
	 ***/
	struct ParseException : public std::runtime_error {
		//ParseException(const std::string &msg);
		ParseException(const std::string &msg, const salmon::meta::position_info &start,
					   const salmon::meta::position_info &end);

	public:
		void add_file_info(const std::filesystem::path &file);
		std::string build_error_str() const;
	private:
		const salmon::meta::position_info expression_start;
		const salmon::meta::position_info expression_end;
		std::filesystem::path source_file;
	};

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

	std::string read(std::istream &input);

	std::string read_from_string(std::string input);

}

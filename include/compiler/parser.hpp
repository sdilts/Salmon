#pragma once

#include <sstream>
#include <string>
#include <exception>
#include <filesystem>
#include <optional>

#include <compiler/CountingStream.hpp>
#include <compiler/compiler.hpp>

namespace salmon::compiler {

	/***
	 * A parse exception holds data for:
	 *   start_info:    the start of the form where the error occured
	 *   end_info: where the actual error occured
	 *   file_info:     the name of the source file (if available) where the error took place.
	 ***/
	struct ParseException : public std::runtime_error {
		ParseException(const std::string &msg, const salmon::meta::position_info &start,
					   const salmon::meta::position_info &end);

	public:
		void add_file_info(const std::filesystem::path &file);
		std::string build_error_str() const;
	private:
		const salmon::meta::position_info expression_start;
		const salmon::meta::position_info expression_end;
		std::optional<std::filesystem::path> source_file;
	};

	//! Read a single form from the input stream.
	std::optional<salmon::vm::Box> read(CountingStreamBuffer &countStreamBuf, Compiler &compiler);

	//! Read a single form from the string
	std::optional<salmon::vm::Box> read_from_string(const std::string& input, Compiler &compiler);

}

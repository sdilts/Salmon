#include <streambuf>
#include <iostream>

#include "compiler/CountingStream.hpp"

namespace salmon::parser {

	// constructor
	CountingStreamBuffer::CountingStreamBuffer(std::streambuf* sbuf) :
		streamBuf_(sbuf),
		lineNumber_(1),
		lastLineNumber_(1),
		column_(0),
		prevColumn_(static_cast<unsigned int>(-1)),
		filePos_(0)
	{
	}

	// Get current line number
	unsigned int CountingStreamBuffer::lineNumber() const  {
		return lineNumber_;
	}

	// Get line number of previously read character
	unsigned int CountingStreamBuffer::prevLineNumber() const {
		return lastLineNumber_;
	}

	// Get current column
	unsigned int CountingStreamBuffer::column() const   {
		return column_;
	}

	// Get file position
	std::streamsize CountingStreamBuffer::filepos() const {
		return filePos_;
	}

	position_info CountingStreamBuffer::positionInfo() const {
		return { lineNumber_, column_};
	}

	position_info CountingStreamBuffer::prevPositionInfo() const {
		return { lastLineNumber_, prevColumn_};
	}

	// extract next character from stream w/o advancing read pos
	std::streambuf::int_type CountingStreamBuffer::underflow() {
		return streamBuf_->sgetc();
	}

	// extract next character from stream
	std::streambuf::int_type CountingStreamBuffer::uflow() {
		int_type rc = streamBuf_->sbumpc();

		lastLineNumber_ = lineNumber_;
		if(traits_type::eq_int_type(rc, traits_type::to_int_type('\n'))) {
			++lineNumber_;
			prevColumn_ = column_ + 1;
			column_ = static_cast<unsigned int>(-1);
		}

		++column_;
		++filePos_;
		return rc;
	}

	// put back last character
	std::streambuf::int_type CountingStreamBuffer::pbackfail(std::streambuf::int_type c) {
		if(traits_type::eq_int_type(c, traits_type::to_int_type('\n'))) {
			--lineNumber_;
			lastLineNumber_ = lineNumber_;
			column_ = prevColumn_;
			prevColumn_ = 0;
		}

		--column_;
		--filePos_;

		if (c != traits_type::eof()) {
			return streamBuf_->sputbackc(traits_type::to_char_type(c));
		}
		else {
			return streamBuf_->sungetc();
		}
	}

	// change position by offset, according to way and mode
	std::ios::pos_type CountingStreamBuffer::seekoff(std::ios::off_type pos,
													 std::ios_base::seekdir dir,
													 std::ios_base::openmode mode) {
		if (dir == std::ios_base::beg
			&& pos == static_cast<std::ios::off_type>(0)) {
			lastLineNumber_ = 1;
			lineNumber_ = 1;
			column_ = 0;
			prevColumn_ = static_cast<unsigned int>(-1);
			filePos_ = 0;

			return streamBuf_->pubseekoff(pos, dir, mode);
		}
		else {
			return std::streambuf::seekoff(pos, dir, mode);
		}
	}

	// change to specified position, according to mode
	std::ios::pos_type CountingStreamBuffer::seekpos(std::ios::pos_type pos,
													 std::ios_base::openmode mode) {
		if (pos == static_cast<std::ios::pos_type>(0)) {
			lastLineNumber_ = 1;
			lineNumber_ = 1;
			column_ = 0;
			prevColumn_ = static_cast<unsigned int>(-1);
			filePos_ = 0;

			return streamBuf_->pubseekpos(pos, mode);
		}
		else {
			return std::streambuf::seekpos(pos, mode);
		}
	}
}

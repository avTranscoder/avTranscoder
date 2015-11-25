#ifndef _AV_TRANSCODER_JSONWRITER_HPP_
#define _AV_TRANSCODER_JSONWRITER_HPP_

#include "common.hpp"

#include <iostream>
#include <sstream>

namespace avtranscoder {
namespace json {

// To manage 'null' element in JSON
struct JsonNull {};

/**
 * 
 */
class AvExport JsonStreamWriter
{
public:
	JsonStreamWriter() : first(true) {}
	virtual ~JsonStreamWriter() {}

	virtual std::string build() {
		finish();
		return stream.str();
	}

protected:
	friend class JsonObjectStreamWriter;
	friend class JsonArrayStreamWriter;
	std::ostringstream stream;
	bool first;

	template <typename T>
	JsonStreamWriter& operator<<(T value)
	{
		stream << value;
		return *this;
	}

	JsonStreamWriter& addSep()
	{
		if (first) {
			first = false;
		} else {
			stream << ',';
		}
		return *this;
	}

	virtual std::ostream& finish() = 0;
};

// Write a boolean to the stream.
template <> JsonStreamWriter& JsonStreamWriter::operator<<(bool value);
// Write a string to the stream.
template <> JsonStreamWriter& JsonStreamWriter::operator<<(const char *string);
// Write null (empty value) to the stream.
template <> JsonStreamWriter& JsonStreamWriter::operator<<(JsonNull);

// Write an object to the stream.
class AvExport JsonObjectStreamWriter : public JsonStreamWriter
{
public:
	JsonObjectStreamWriter() { stream << '{'; }

	template <typename T>
	JsonObjectStreamWriter& operator<<(const std::pair<const char *, T> pair)
	{
		addSep() << pair.first << ':' << pair.second;
		return *this;
	}

protected:
	virtual std::ostream& finish() { return stream << '}'; }
};

// Write an array to the stream.
class AvExport JsonArrayStreamWriter : public JsonStreamWriter
{
public:
	JsonArrayStreamWriter() { stream << '['; }

	template <typename T>
	JsonArrayStreamWriter& operator<<(T value)
	{
		addSep() << value;
		return *this;
	}

protected:
	virtual std::ostream& finish() { return stream << ']'; }
};

}
}

#endif

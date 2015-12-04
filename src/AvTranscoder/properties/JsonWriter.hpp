#ifndef _AV_TRANSCODER_MEDIA_PROPERTY_JSONWRITER_HPP_
#define _AV_TRANSCODER_MEDIA_PROPERTY_JSONWRITER_HPP_

#include <AvTranscoder/common.hpp>

#include <iostream>
#include <sstream>

namespace avtranscoder {
namespace json {

/**
 * @brief To manage 'null' element in JSON
 */
struct JsonNull {};

/**
 * @brief Based class to write element to a stream.
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

	// Escape strings accordingly to the JSON standard
	std::string escapeJsonString(const std::string& input);
};

// Write a boolean to the stream.
template <> JsonStreamWriter& JsonStreamWriter::operator<<(bool value);
// Write a string to the stream.
template <> JsonStreamWriter& JsonStreamWriter::operator<<(const char *string);
// Write null (empty value) to the stream.
template <> JsonStreamWriter& JsonStreamWriter::operator<<(JsonNull);

/**
 * @brief Write an object to a stream.
 */
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

template<>
JsonObjectStreamWriter& JsonObjectStreamWriter::operator<<(const std::pair<const char *, const char*> pair);

/**
 * @brief Write an array to a stream.
 */
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

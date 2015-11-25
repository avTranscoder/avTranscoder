#include "JsonWriter.hpp"

namespace avtranscoder {
namespace json {

template <>
JsonStreamWriter& JsonStreamWriter::operator<<(bool value)
{
	stream << (value ? "true" : "false");
	return *this;
}
template <>
JsonStreamWriter& JsonStreamWriter::operator<<(const char *string)
{
	stream << '"' << string << '"';
	return *this;
}
template <>
JsonStreamWriter& JsonStreamWriter::operator<<(JsonNull)
{
	stream << "null";
	return *this;
}

}
}

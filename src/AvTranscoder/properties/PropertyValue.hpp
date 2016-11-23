#ifndef _AV_TRANSCODER_PROPERTIES_PROPERTY_VALUE_HPP_
#define _AV_TRANSCODER_PROPERTIES_PROPERTY_VALUE_HPP_

#include <AvTranscoder/common.hpp>
#include <string>
#include <vector>

namespace avtranscoder
{

enum EPropertyValueKind
{
    EPropertyValueKindString = 0,
    EPropertyValueKindBoolean,
    EPropertyValueKindInt,
    EPropertyValueKindUInt,
    EPropertyValueKindFloat,
    EPropertyValueKindDouble,
    EPropertyValueKindRational,
};

class PropertyValue
{
public:
    PropertyValue(const EPropertyValueKind kind, const std::string& data);
    PropertyValue(const EPropertyValueKind kind, const bool data);
    PropertyValue(const EPropertyValueKind kind, const int data);
    PropertyValue(const EPropertyValueKind kind, const size_t data);
    PropertyValue(const EPropertyValueKind kind, const float data);
    PropertyValue(const EPropertyValueKind kind, const double data);
    PropertyValue(const EPropertyValueKind kind, const Rational data);

    const EPropertyValueKind getType() const;
    const std::string getString() const;
    const bool getBool() const;
    const size_t getUInt() const;
    const int getInt() const;
    const float getFloat() const;
    const double getDouble() const;
    const Rational getRational() const;

    const std::string asString() const;

private:
    EPropertyValueKind _kind;
    std::vector<unsigned char> _data;
};

std::ostream& operator<<(std::ostream& stream, const PropertyValue& value);

}

#endif

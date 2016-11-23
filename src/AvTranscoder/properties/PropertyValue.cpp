
#include "PropertyValue.hpp"
#include <iostream>
#include <sstream>

namespace avtranscoder
{

PropertyValue::PropertyValue(const EPropertyValueKind kind, const std::string& data)
    : _kind(kind)
    , _data(data.c_str(), data.c_str() + data.size() + 1)
{}

PropertyValue::PropertyValue(const EPropertyValueKind kind, const bool data)
    : _kind(kind)
    , _data(sizeof(bool), data)
{}

PropertyValue::PropertyValue(const EPropertyValueKind kind, const size_t data)
    : _kind(kind)
    , _data(sizeof(size_t), data)
{
    ::memcpy(_data.data(), &data, sizeof(size_t));
}

PropertyValue::PropertyValue(const EPropertyValueKind kind, const int data)
    : _kind(kind)
    , _data(sizeof(int), data)
{}

PropertyValue::PropertyValue(const EPropertyValueKind kind, const float data)
    : _kind(kind)
    , _data(sizeof(float), data)
{}

PropertyValue::PropertyValue(const EPropertyValueKind kind, const double data)
    : _kind(kind)
    , _data(sizeof(double), data)
{}

PropertyValue::PropertyValue(const EPropertyValueKind kind, const Rational data)
    : _kind(kind)
    , _data(2 * sizeof(int), 0)
{
    _data[sizeof(int)] = data.num;
    _data[0] = data.den;
}


const EPropertyValueKind PropertyValue::getType() const
{
    return _kind;
}

const std::string PropertyValue::getString() const
{
    return std::string(_data.begin(), _data.begin() + _data.size());
}

const bool PropertyValue::getBool() const
{
    return (bool)_data[0];
}

const size_t PropertyValue::getUInt() const
{
    size_t v = 0;
    ::memcpy(&v, _data.data(), sizeof(size_t));
    return v;
}

const int PropertyValue::getInt() const
{
    return (int)_data[0];
}

const float PropertyValue::getFloat() const
{
    return (float)_data[0];
}

const double PropertyValue::getDouble() const
{
    return (double)_data[0];
}

const Rational PropertyValue::getRational() const
{
    Rational r;
    r.num = (int)_data[sizeof(int)];
    r.den = (int)_data[0];
    return r;
}

const std::string PropertyValue::asString() const
{
    std::ostringstream os;
    switch(getType())
    {
        case EPropertyValueKind::EPropertyValueKindString:
        {
            os << getString();
            break;
        }
        case EPropertyValueKind::EPropertyValueKindUInt:
        {
            os << getUInt();
            break;
        }
        case EPropertyValueKind::EPropertyValueKindInt:
        {
            os << getInt();
            break;
        }
        case EPropertyValueKind::EPropertyValueKindFloat:
        {
            os << getFloat();
            break;
        }
        case EPropertyValueKind::EPropertyValueKindDouble:
        {
            os << getDouble();
            break;
        }
        case EPropertyValueKind::EPropertyValueKindBoolean:
        {
            os << (getBool() ? "true": "false");
            break;
        }
        case EPropertyValueKind::EPropertyValueKindRational:
        {
            Rational r = getRational();
            os << r.num << "/" << r.den;
            break;
        }
    }
    return os.str();
}

std::ostream& operator<<(std::ostream& stream, const PropertyValue& value)
{
    stream << value.asString();
    return stream;
}

}

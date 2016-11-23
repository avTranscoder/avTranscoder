#ifndef _AV_TRANSCODER_MEDIA_PROPERTY_UTIL_HPP_
#define _AV_TRANSCODER_MEDIA_PROPERTY_UTIL_HPP_

#include <AvTranscoder/common.hpp>
#include <AvTranscoder/properties/PropertyValue.hpp>

extern "C" {
#include <libavutil/dict.h>
}

#include <vector>
#include <utility>
#include <map>
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>

namespace avtranscoder
{

/**
 * @brief PropertyVector is a vector of pair, because the order of properties matters to us.
 */
typedef std::vector<std::pair<std::string, PropertyValue> > PropertyVector;
typedef std::map<std::string, PropertyValue> PropertyMap;

namespace detail
{

/**
 * Variables to print properties class to the terminal.
 */
const size_t keyWidth = 32;
const std::string separator = "====================";

/**
 * If cannot access the property, get this value.
 */
const std::string propertyValueIfError = "null";

template <typename T>
void add(PropertyVector& propertyVector, const std::string& key, const T& value)
{
    std::stringstream ss;
    ss << value;
    add(propertyVector, key, ss.str());
}

template <>
void add(PropertyVector& propertyVector, const std::string& key, const size_t& value);

template <>
void add(PropertyVector& propertyVector, const std::string& key, const int& value);

template <>
void add(PropertyVector& propertyVector, const std::string& key, const float& value);

template <>
void add(PropertyVector& propertyVector, const std::string& key, const std::string& value);

template <>
void add(PropertyVector& propertyVector, const std::string& key, const bool& value);

template <>
void add(PropertyVector& propertyVector, const std::string& key, const Rational& value);

#ifndef SWIG
template <typename T>
void addWithKind(PropertyVector& propertyVector, const std::string& key, const EPropertyValueKind kind, const T& value)
{
    PropertyValue pv = PropertyValue(kind, value);
    propertyVector.push_back(std::make_pair(key, pv));
}

template <class C, typename T>
void addProperty(PropertyVector& data, const std::string& key, const C* obj, T (C::*getter)(void) const)
{
    try
    {
        add(data, key, (obj->*getter)());
    }
    catch(const std::exception& e)
    {
        add(data, key, detail::propertyValueIfError);
    }
}
#endif

/**
 * @brief Fill metadata parameter with the given AVDictionary.
 */
void AvExport fillMetadataDictionnary(AVDictionary* avdictionnary, PropertyVector& metadata);
}
}

#endif

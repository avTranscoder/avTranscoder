#include "util.hpp"

extern "C" {
#include <libavutil/avutil.h>
}

#include <sstream>
#include <iostream>

namespace avtranscoder
{

namespace detail
{

template <>
void add(PropertyVector& propertyVector, const std::string& key, const size_t& value)
{
    addWithKind(propertyVector, key, EPropertyValueKind::EPropertyValueKindUInt, value);
}

template <>
void add(PropertyVector& propertyVector, const std::string& key, const int& value)
{
    addWithKind(propertyVector, key, EPropertyValueKind::EPropertyValueKindInt, value);
}

template <>
void add(PropertyVector& propertyVector, const std::string& key, const float& value)
{
    addWithKind(propertyVector, key, EPropertyValueKind::EPropertyValueKindFloat, value);
}

template <>
void add(PropertyVector& propertyVector, const std::string& key, const double& value)
{
    addWithKind(propertyVector, key, EPropertyValueKind::EPropertyValueKindDouble, value);
}

template <>
void add(PropertyVector& propertyVector, const std::string& key, const std::string& value)
{
    addWithKind(propertyVector, key, EPropertyValueKind::EPropertyValueKindString, value);
}

template <>
void add(PropertyVector& propertyVector, const std::string& key, const bool& value)
{
    addWithKind(propertyVector, key, EPropertyValueKind::EPropertyValueKindBoolean, value);
}

template <>
void add(PropertyVector& propertyVector, const std::string& key, const Rational& value)
{
    addWithKind(propertyVector, key, EPropertyValueKind::EPropertyValueKindRational, value);
}

void fillMetadataDictionnary(AVDictionary* avdictionnary, PropertyVector& metadata)
{
    AVDictionaryEntry* tag = NULL;
    while((tag = av_dict_get(avdictionnary, "", tag, AV_DICT_IGNORE_SUFFIX)))
    {
        addWithKind(metadata, tag->key, EPropertyValueKind::EPropertyValueKindString, std::string(tag->value));
    }
}
}
}

#include "util.hpp"

extern "C" {
#include <libavutil/avutil.h>
}

#include <sstream>

namespace avtranscoder
{

namespace detail
{

template <>
void add(PropertyVector& propertyVector, const std::string& key, const size_t& value)
{
    std::stringstream ss;
    if(value == (size_t)AV_NOPTS_VALUE)
        ss << "N/A";
    else
        ss << value;
    add(propertyVector, key, ss.str());
}

template <>
void add(PropertyVector& propertyVector, const std::string& key, const float& value)
{
    std::stringstream ss;
    if(value <= AV_NOPTS_VALUE)
        ss << "N/A";
    else
        ss << value;
    add(propertyVector, key, ss.str());
}

template <>
void add(PropertyVector& propertyVector, const std::string& key, const std::string& value)
{
    propertyVector.push_back(std::make_pair(key, value));
}

template <>
void add(PropertyVector& propertyVector, const std::string& key, const bool& value)
{
    add(propertyVector, key, value ? "True" : "False");
}

template <>
void add(PropertyVector& propertyVector, const std::string& key, const Rational& value)
{
    add(propertyVector, key, value.num / (double)value.den);
}

void fillMetadataDictionnary(AVDictionary* avdictionnary, PropertyVector& metadata)
{
    AVDictionaryEntry* tag = NULL;
    while((tag = av_dict_get(avdictionnary, "", tag, AV_DICT_IGNORE_SUFFIX)))
    {
        metadata.push_back(std::make_pair(tag->key, tag->value));
    }
}
}
}

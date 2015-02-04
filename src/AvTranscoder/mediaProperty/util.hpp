#ifndef _AV_TRANSCODER_MEDIA_PROPERTY_UTIL_HPP_
#define _AV_TRANSCODER_MEDIA_PROPERTY_UTIL_HPP_

#include <AvTranscoder/common.hpp>

extern "C" {
#include <libavutil/dict.h>
}

#include <vector>
#include <utility>
#include <string>
#include <sstream>

namespace avtranscoder
{

/**
 * @brief PropertyMap is a vector of pair, because the order of properties matters to us.
 */
typedef std::vector< std::pair<std::string, std::string> > PropertiesMap;

namespace detail
{

template<typename T>
void add( PropertiesMap& propertiesMap, const std::string& key, const T& value )
{
	std::stringstream ss;
	ss << value;
	add( propertiesMap, key, ss.str() );
}

template<>
void add( PropertiesMap& propertiesMap, const std::string& key, const std::string& value );

template<>
void add( PropertiesMap& propertiesMap, const std::string& key, const bool& value );

template<>
void add( PropertiesMap& propertiesMap, const std::string& key, const Rational& value );

/**
 * @brief Fill metadata parameter with the given AVDictionary.
 */
void AvExport fillMetadataDictionnary( AVDictionary* avdictionnary, PropertiesMap& metadata );

}

}

#endif

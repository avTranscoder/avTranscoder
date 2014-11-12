#ifndef _AV_TRANSCODER_MEDIA_PROPERTY_UTIL_HPP_
#define _AV_TRANSCODER_MEDIA_PROPERTY_UTIL_HPP_

#include <AvTranscoder/common.hpp>

#include <vector>
#include <utility>
#include <string>
#include <sstream>

namespace avtranscoder
{

/**
 * @brief Can get all data of Properties structures by getDataMap(), which return a MetadatasMap.
 */
typedef std::vector< std::pair<std::string, std::string> > MetadatasMap;

namespace detail
{

template<typename T>
void add( MetadatasMap& dataMap, const std::string& key, const T& value )
{
	std::stringstream ss;
	ss << value;
	add( dataMap, key, ss.str() );
}

template<>
void add( MetadatasMap& dataMap, const std::string& key, const std::string& value );

template<>
void add( MetadatasMap& dataMap, const std::string& key, const bool& value );

/**
 * @brief Fill metadata parameter with the given AVDictionary.
 */
void AvExport fillMetadataDictionnary( AVDictionary* avdictionnary, MetadatasMap& metadata );

}

}

#endif
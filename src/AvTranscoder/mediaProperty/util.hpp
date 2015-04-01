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
 * @brief PropertyVector is a vector of pair, because the order of properties matters to us.
 */
typedef std::vector< std::pair<std::string, std::string> > PropertyVector;

namespace detail
{

template<typename T>
void add( PropertyVector& propertyVector, const std::string& key, const T& value )
{
	std::stringstream ss;
	ss << value;
	add( propertyVector, key, ss.str() );
}

template<>
void add( PropertyVector& propertyVector, const std::string& key, const std::string& value );

template<>
void add( PropertyVector& propertyVector, const std::string& key, const bool& value );

template<>
void add( PropertyVector& propertyVector, const std::string& key, const Rational& value );

/**
 * @brief Fill metadata parameter with the given AVDictionary.
 */
void AvExport fillMetadataDictionnary( AVDictionary* avdictionnary, PropertyVector& metadata );

}

}

#endif

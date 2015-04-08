#include "util.hpp"

#include <sstream>

namespace avtranscoder
{

namespace detail
{

template<>
void add( PropertyVector& propertyVector, const std::string& key, const std::string& value )
{
	propertyVector.push_back( std::make_pair( key, value ) );
}

template<>
void add( PropertyVector& propertyVector, const std::string& key, const bool& value )
{
	add( propertyVector, key, value ? "True" : "False" );
}

template<>
void add( PropertyVector& propertyVector, const std::string& key, const Rational& value )
{
	add( propertyVector, key, value.num / (double) value.den );
}

void fillMetadataDictionnary( AVDictionary* avdictionnary, PropertyVector& metadata )
{
	AVDictionaryEntry* tag = NULL;
	while( ( tag = av_dict_get( avdictionnary, "", tag, AV_DICT_IGNORE_SUFFIX ) ) )
	{
		metadata.push_back( std::make_pair( tag->key, tag->value ) );
	}
}

}

}

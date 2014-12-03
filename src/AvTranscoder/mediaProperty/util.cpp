#include "util.hpp"

extern "C" {
#include <libavutil/dict.h>
}

namespace avtranscoder
{

namespace detail
{

template<>
void add( PropertiesMap& propertiesMap, const std::string& key, const std::string& value )
{
	propertiesMap.push_back( std::make_pair( key, value ) );
}

template<>
void add( PropertiesMap& propertiesMap, const std::string& key, const bool& value )
{
	add( propertiesMap, key, value ? "True" : "False" );
}

void fillMetadataDictionnary( AVDictionary* avdictionnary, PropertiesMap& metadata )
{
	AVDictionaryEntry* tag = NULL;
	while( ( tag = av_dict_get( avdictionnary, "", tag, AV_DICT_IGNORE_SUFFIX ) ) )
	{
		metadata.push_back( std::make_pair( tag->key, tag->value ) );
	}
}

}

}

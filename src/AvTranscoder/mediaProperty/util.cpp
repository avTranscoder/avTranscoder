#include "util.hpp"

extern "C" {
#include <libavutil/dict.h>
}

namespace avtranscoder
{

namespace detail
{

template<>
void add( MetadatasMap& dataMap, const std::string& key, const std::string& value )
{
	dataMap.push_back( std::make_pair( key, value ) );
}

template<>
void add( MetadatasMap& dataMap, const std::string& key, const bool& value )
{
	add( dataMap, key, value ? "True" : "False" );
}

void fillMetadataDictionnary( AVDictionary* avdictionnary, MetadatasMap& metadata )
{
	AVDictionaryEntry* tag = NULL;
	while( ( tag = av_dict_get( avdictionnary, "", tag, AV_DICT_IGNORE_SUFFIX ) ) )
	{
		metadata.push_back( std::make_pair( tag->key, tag->value ) );
	}
}

}

}

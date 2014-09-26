#include "DataCodec.hpp"

namespace avtranscoder
{

DataCodec::DataCodec( const std::string& codecName )
	: ICodec( "" )
{
}

DataCodec::DataCodec( const AVCodecID codecId )
	: ICodec( "" )
{
}

DataCodec::DataCodec( const ICodec& essenceDesc )
	: ICodec( "" )
{
}

}

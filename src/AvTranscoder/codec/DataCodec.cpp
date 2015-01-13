#include "DataCodec.hpp"

namespace avtranscoder
{

DataCodec::DataCodec( const ECodecType type, const std::string& codecName )
	: ICodec( type, codecName )
{
}

DataCodec::DataCodec( const ECodecType type, const AVCodecID codecId )
	: ICodec( type, codecId )
{
}

DataCodec::DataCodec( const ECodecType type, AVCodecContext& avCodecContext )
	: ICodec( type, avCodecContext )
{
}

DataCodec::DataCodec( const ICodec& codec )
	: ICodec( codec )
{
}

}

#ifndef _AV_TRANSCODER_CODEC_DATA_CODEC_HPP_
#define _AV_TRANSCODER_CODEC_DATA_CODEC_HPP_

#include "ICodec.hpp"

namespace avtranscoder
{

class AvExport DataCodec : public ICodec
{
public:
	DataCodec( const std::string& codecName = "" );
	DataCodec( const AVCodecID codecId );
	DataCodec( const ICodec& essenceDesc );
};

}

#endif
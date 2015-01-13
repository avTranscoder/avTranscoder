#ifndef _AV_TRANSCODER_CODEC_DATA_CODEC_HPP_
#define _AV_TRANSCODER_CODEC_DATA_CODEC_HPP_

#include "ICodec.hpp"

namespace avtranscoder
{

class AvExport DataCodec : public ICodec
{
public:
	DataCodec( const ECodecType type, const std::string& codecName = "" );
	DataCodec( const ECodecType type, const AVCodecID codecId );
	DataCodec( const ECodecType type, AVCodecContext& avCodecContext );
};

}

#endif
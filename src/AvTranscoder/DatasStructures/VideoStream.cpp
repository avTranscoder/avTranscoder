#include "VideoStream.hpp"

namespace avtranscoder
{

void VideoStream::setCodecFromName( const std::string& codecName )
{
	AVCodec* codec = avcodec_find_encoder_by_name( codecName.c_str() );
	m_codecId = codec->id;
}

}
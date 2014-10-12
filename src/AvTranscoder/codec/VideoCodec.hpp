#ifndef _AV_TRANSCODER_CODEC_VIDEO_CODEC_HPP_
#define _AV_TRANSCODER_CODEC_VIDEO_CODEC_HPP_

#include "ICodec.hpp"
#include <AvTranscoder/frame/VideoFrame.hpp>

#include <utility>

namespace avtranscoder
{

class AvExport VideoCodec : public ICodec
{
public:
	VideoCodec( const std::string& codecName = "" );
	VideoCodec( const AVCodecID codecId );
	VideoCodec( const ICodec& codec );
	
	VideoFrameDesc getVideoFrameDesc() const;
	std::pair< size_t, size_t > getTimeBase() const;
	
	void setImageParameters( const VideoFrameDesc& videoFrameDesc );
	void setImageParameters( const size_t width, const size_t height, const Pixel& pixel );
	void setImageParameters( const size_t width, const size_t height, const AVPixelFormat& pixel );

	void setTimeBase( const size_t num, const size_t den, const size_t ticksPerFrame = 1 );
};

}

#endif
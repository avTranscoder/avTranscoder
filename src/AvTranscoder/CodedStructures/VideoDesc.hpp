#ifndef _AV_TRANSCODER_CODED_STRUCTURES_VIDEO_DESC_HPP_
#define _AV_TRANSCODER_CODED_STRUCTURES_VIDEO_DESC_HPP_

#include <AvTranscoder/EssenceStructures/VideoFrame.hpp>
#include "CodedDesc.hpp"
#include <AvTranscoder/common.hpp>

#include <string>

class AVCodec;

namespace avtranscoder
{

class AvExport VideoDesc : public CodedDesc
{
public:
	VideoDesc( const std::string& codecName = "" );
	VideoDesc( const AVCodecID codecId );
	VideoDesc( const CodedDesc& essenceDesc );
	
	VideoFrameDesc getVideoFrameDesc() const;
	std::pair< size_t, size_t > getTimeBase() const;
	
	void setImageParameters( const VideoFrameDesc& videoFrameDesc );
	void setImageParameters( const size_t width, const size_t height, const Pixel& pixel );
	void setImageParameters( const size_t width, const size_t height, const AVPixelFormat& pixel );

	void setTimeBase( const size_t num, const size_t den, const size_t ticksPerFrame = 1 );
};

}

#endif
#ifndef _MEDIA_MANAGER_IO_CODEC_VIDEO_CODEC_HPP_
#define _MEDIA_MANAGER_IO_CODEC_VIDEO_CODEC_HPP_

#include "ICodec.hpp"
#include <mediaCore/frame/VideoFrame.hpp>

#include <utility>

namespace mediamanager
{
namespace mediaio
{

class MediaManagerExport VideoCodec : public ICodec
{
public:
	VideoCodec( const ECodecType type, const std::string& codecName = "" );
	VideoCodec( const ECodecType type, const AVCodecID codecId );
	VideoCodec( const ICodec& codec );
	
	mediacore::VideoFrameDesc getVideoFrameDesc() const;
	std::pair< size_t, size_t > getTimeBase() const;
	
	void setImageParameters( const mediacore::VideoFrameDesc& videoFrameDesc );
	void setImageParameters( const size_t width, const size_t height, const mediacore::Pixel& pixel );
	void setImageParameters( const size_t width, const size_t height, const AVPixelFormat& pixel );

	void setTimeBase( const size_t num, const size_t den, const size_t ticksPerFrame = 1 );
};

}
}

#endif
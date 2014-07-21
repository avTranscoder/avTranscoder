#ifndef _AV_TRANSCODER_DATA_VIDEO_DESC_HPP_
#define _AV_TRANSCODER_DATA_VIDEO_DESC_HPP_

#include <string>

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
	#define __STDC_CONSTANT_MACROS
#endif
#ifndef INT64_C
	#define INT64_C(c) (c ## LL)
	#define UINT64_C(c) (c ## ULL)
#endif
#include <libavcodec/avcodec.h>
}

#include <AvTranscoder/common.hpp>
#include <AvTranscoder/DatasStructures/EssenceDesc.hpp>
#include <AvTranscoder/DatasStructures/Image.hpp>

namespace avtranscoder
{

class AvExport VideoDesc : public EssenceDesc
{
public:
	VideoDesc( const std::string& codecName = "" );
	VideoDesc( const AVCodecID codecId );
	VideoDesc( const EssenceDesc& essenceDesc );
	
	ImageDesc getImageDesc() const;
	std::pair< size_t, size_t > getTimeBase() const;
	
	void setImageParameters( const ImageDesc& imageDesc );
	void setImageParameters( const size_t width, const size_t height, const Pixel& pixel );
	void setImageParameters( const size_t width, const size_t height, const AVPixelFormat& pixel );

	void setTimeBase( const size_t num, const size_t den, const size_t ticksPerFrame = 1 );
};

}

#endif
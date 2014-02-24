#ifndef _AV_TRANSCODER_DATA_VIDEO_DESC_HPP_
#define _AV_TRANSCODER_DATA_VIDEO_DESC_HPP_

#include "Image.hpp"
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

namespace avtranscoder
{

class AvExport VideoDesc
{
public:
	VideoDesc( const std::string& codecName = "" );
	VideoDesc( const AVCodecID codecId );

	void setVideoCodec( const std::string& codecName );
	void setVideoCodec( const AVCodecID codecId );

	void setImageParameters( const Image& image );
	void setImageParameters( const size_t width, const size_t height, const Pixel& pixel );
	void setImageParameters( const size_t width, const size_t height, const AVPixelFormat& pixel );

	void setTimeBase( const size_t num, const size_t den );
	
	void set( const std::string& key, const std::string& flag, const bool enable );
	void set( const std::string& key, const bool value );
	void set( const std::string& key, const int value );
	void set( const std::string& key, const int num, const int den );
	void set( const std::string& key, const double value );
	void set( const std::string& key, const std::string& value );

	std::string getVideoCodec()  const;
	AVCodecID   getVideoCodecId()  const;
	std::pair< size_t, size_t > getTimeBase() const;

#ifndef SWIG
	AVCodec*        getCodec()        const { return m_codec; }
	AVCodecContext* getCodecContext() const { return m_codecContext; }
#endif

private:
	void initCodecContext( );

	void checkError( int error );

	AVCodec*        m_codec;
	AVCodecContext* m_codecContext;
};

}

#endif
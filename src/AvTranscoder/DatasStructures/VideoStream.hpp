#ifndef _AV_TRANSCODER_DATA_VIDEO_STREAM_HPP_
#define _AV_TRANSCODER_DATA_VIDEO_STREAM_HPP_

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

class VideoStream
{
public:
	VideoStream( const std::string& codecName = "" );
	VideoStream( const AVCodecID codecId );

	void setVideoCodec( const std::string& codecName );
	void setVideoCodec( const AVCodecID codecId );

	void setParametersFromImage( const Image& image );

	void setBitrate( const size_t bitRate );
	void setTimeBase( const size_t num, const size_t den );

	void set( const std::string& key, const bool value );
	void set( const std::string& key, const int value );
	void set( const std::string& key, const int num, const int den );
	void set( const std::string& key, const double value );
	void set( const std::string& key, const std::string& value );

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
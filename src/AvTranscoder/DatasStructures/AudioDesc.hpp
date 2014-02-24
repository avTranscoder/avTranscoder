#ifndef _AV_TRANSCODER_DATA_AUDIO_DESC_HPP_
#define _AV_TRANSCODER_DATA_AUDIO_DESC_HPP_

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

class AvExport AudioDesc
{
public:
	AudioDesc( const std::string& codecName = "" );
	AudioDesc( const AVCodecID codecId );

	void setAudioCodec( const std::string& codecName );
	void setAudioCodec( const AVCodecID codecId );

	void set( const std::string& key, const std::string& flag, const bool enable );
	void set( const std::string& key, const bool value );
	void set( const std::string& key, const int value );
	void set( const std::string& key, const int num, const int den );
	void set( const std::string& key, const double value );
	void set( const std::string& key, const std::string& value );

	std::string getAudioCodec()  const;
	AVCodecID   getAudioCodecId()  const;

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
#ifndef _AV_TRANSCODER_DATA_ESSENCE_DESC_HPP_
#define	_AV_TRANSCODER_DATA_ESSENCE_DESC_HPP_

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

namespace avtranscoder
{

class AvExport CodedDesc
{
public:
	CodedDesc( const std::string& codecName );
	CodedDesc( const AVCodecID codecId );
	
	virtual ~CodedDesc() {}
	
	std::string getCodecName()  const;
	AVCodecID   getCodecId()  const;
	
	void setCodec( const std::string& codecName );
	void setCodec( const AVCodecID codecId );
	
	void set( const std::string& key, const std::string& flag, const bool enable );
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

protected:
	AVCodec*        m_codec;
	AVCodecContext* m_codecContext;
};

}

#endif


#ifndef _AV_TRANSCODER_CODED_STRUCTURES_ESSENCE_DESC_HPP_
#define	_AV_TRANSCODER_CODED_STRUCTURES_ESSENCE_DESC_HPP_

#include <AvTranscoder/common.hpp>

#include <string>

class AVCodec;
class AVCodecContext;

namespace avtranscoder
{

class AvExport CodedDesc
{
public:
	CodedDesc( const std::string& codecName );
	CodedDesc( const AVCodecID codecId );
	CodedDesc( AVCodec& avCodec, AVCodecContext& avCodecContext );
	
	virtual ~CodedDesc() {}
	
	std::string getCodecName()  const;
	AVCodecID   getCodecId()  const;
	
	int getLatency()  const;

	void setCodec( const std::string& codecName );
	void setCodec( const AVCodecID codecId );
	
#ifndef SWIG
	AVCodec*        getCodec()        const { return _codec; }
	AVCodecContext* getCodecContext() const { return _codecContext; }
#endif

private:
	void initCodecContext( );

	void checkError( int error );

protected:
	AVCodec*        _codec;
	AVCodecContext* _codecContext;
};

}

#endif


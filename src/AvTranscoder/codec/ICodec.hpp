#ifndef _AV_TRANSCODER_CODED_STRUCTURES_ESSENCE_DESC_HPP_
#define	_AV_TRANSCODER_CODED_STRUCTURES_ESSENCE_DESC_HPP_

#include <AvTranscoder/common.hpp>

#include <string>

struct AVCodec;
struct AVCodecContext;

namespace avtranscoder
{

class AvExport ICodec
{
public:
	ICodec( const std::string& codecName );
	ICodec( const AVCodecID codecId );
	
	virtual ~ICodec() {};
	
	std::string getCodecName()  const;
	AVCodecID   getCodecId()  const;
	
	int getLatency()  const;

	void setEncoderCodec( const std::string& codecName );
	void setEncoderCodec( const AVCodecID codecId );
	
#ifndef SWIG
	AVCodec*        getAVCodec()        const { return _codec; }
	AVCodecContext* getAVCodecContext() const { return _codecContext; }
#endif

private:
	void initCodecContext( );

	void checkError( int error );

protected:
	AVCodec*        _codec; ///< Codec abstract description
	AVCodecContext* _codecContext; ///< Full codec instance description
};

}

#endif


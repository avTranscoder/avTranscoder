#ifndef _AV_TRANSCODER_CODED_STRUCTURES_ESSENCE_DESC_HPP_
#define	_AV_TRANSCODER_CODED_STRUCTURES_ESSENCE_DESC_HPP_

#include <AvTranscoder/common.hpp>

#include <string>

class AVCodec;
class AVCodecContext;

namespace avtranscoder
{

class AvExport ICodec
{
public:
	ICodec( const std::string& codecName );
	ICodec( const AVCodecID codecId );
	ICodec( AVCodec& avCodec, AVCodecContext& avCodecContext );
	
	virtual ~ICodec() {};
	
	std::string getCodecName()  const;
	AVCodecID   getCodecId()  const;
	
	int getLatency()  const;

	void setCodec( const std::string& codecName );
	void setCodec( const AVCodecID codecId );
	
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


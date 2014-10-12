#ifndef _AV_TRANSCODER_CODEC_ICODEC_HPP_
#define _AV_TRANSCODER_CODEC_ICODEC_HPP_

#include <AvTranscoder/common.hpp>

#include <string>

struct AVCodec;
struct AVCodecContext;
enum AVCodecID;

namespace avtranscoder
{

/**
 * @brief Define if a codec is for encoding or decoding.
 */
enum ECodecType
{
	eCodecTypeEncoder,
	eCodecTypeDecoder
};

class AvExport ICodec
{
public:
	ICodec( const ECodecType type, const std::string& codecName );
	ICodec( const ECodecType type, const AVCodecID codecId );

	virtual ~ICodec() {};
	
	std::string getCodecName()  const;
	AVCodecID   getCodecId()  const;
	
	int getLatency()  const;

	void setEncoderCodec( const std::string& codecName );
	void setEncoderCodec( const AVCodecID codecId );

	void setDecoderCodec( const std::string& codecName );
	void setDecoderCodec( const AVCodecID codecId );
	
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


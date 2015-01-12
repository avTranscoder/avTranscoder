#ifndef _AV_TRANSCODER_CODEC_ICODEC_HPP_
#define _AV_TRANSCODER_CODEC_ICODEC_HPP_

#include <AvTranscoder/common.hpp>
#include <AvTranscoder/option/CodecContext.hpp>

#include <string>

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

	virtual ~ICodec() = 0;

	std::string getCodecName() const;
	AVCodecID getCodecId() const;
	ECodecType getCodecType() const { return _type; }
	int getLatency() const;

	CodecContext& getCodecContext() const { return *_codecContext; }

	void setCodec( const ECodecType type, const std::string& codecName );
	void setCodec( const ECodecType type, const AVCodecID codecId );
	
#ifndef SWIG
	AVCodec& getAVCodec() const { return *_avCodec; }
	AVCodecContext& getAVCodecContext() const { return _codecContext->getAVCodecContext(); }
#endif

protected:
	CodecContext* _codecContext; ///< Contains the AVCodecContext, which is the full codec instance description (has ownership)
	AVCodec* _avCodec; ///< Codec abstract description

	ECodecType _type;
};

}

#endif


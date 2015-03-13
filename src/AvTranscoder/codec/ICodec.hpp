#ifndef _AV_TRANSCODER_CODEC_ICODEC_HPP_
#define _AV_TRANSCODER_CODEC_ICODEC_HPP_

#include <AvTranscoder/common.hpp>
#include <AvTranscoder/Option.hpp>

extern "C" {
#include <libavcodec/avcodec.h>
}

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
private:
	ICodec( const ICodec& iCodec );
	ICodec& operator=( const ICodec& iCodec );

public:
	ICodec( const ECodecType type, const std::string& codecName );
	ICodec( const ECodecType type, const AVCodecID codecId );
	ICodec( const ECodecType type, AVCodecContext& avCodecContext );

	virtual ~ICodec() = 0;

	/// Initialize the codec context.
	void open();

	std::string getCodecName() const;
	AVCodecID getCodecId() const;
	ECodecType getCodecType() const { return _type; }
	int getLatency() const;

	OptionArray getOptions();  ///< Get options as array
	OptionMap& getOptionsMap() { return _options; }  ///< Get options as map

	Option& getOption( const std::string& optionName ) { return _options.at(optionName); }
	
#ifndef SWIG
	AVCodecContext& getAVCodecContext() { return *_avCodecContext; }
	const AVCodecContext& getAVCodecContext() const { return *_avCodecContext; }
	AVCodec& getAVCodec() { return *_avCodec; }
	const AVCodec& getAVCodec() const { return *_avCodec; }
#endif

private:
	void setCodec( const ECodecType type, const std::string& codecName );
	void setCodec( const ECodecType type, const AVCodecID codecId );
	void allocateContext();
	void loadCodecOptions();

protected:
	AVCodecContext* _avCodecContext; ///< Full codec instance description (has ownership)
	AVCodec* _avCodec; ///< Codec abstract description
	const bool _isCodecContextAllocated;  ///< Is the AVCodecContext allocated by the class

	ECodecType _type;

	OptionMap _options;
};

}

#endif


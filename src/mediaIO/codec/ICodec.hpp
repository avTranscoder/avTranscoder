#ifndef _MEDIA_MANAGER_IO_CODEC_ICODEC_HPP_
#define _MEDIA_MANAGER_IO_CODEC_ICODEC_HPP_

#include <mediaCore/common.hpp>

#include <string>

namespace mediamanager
{
namespace mediaio
{

/**
 * @brief Define if a codec is for encoding or decoding.
 */
enum ECodecType
{
	eCodecTypeEncoder,
	eCodecTypeDecoder
};

class MediaManagerExport ICodec
{
public:
	ICodec( const ECodecType type, const std::string& codecName );
	ICodec( const ECodecType type, const AVCodecID codecId );

	virtual ~ICodec() = 0;
	
	std::string getCodecName()  const;
	AVCodecID   getCodecId()  const;
	
	int getLatency()  const;

	void setCodec( const ECodecType type, const std::string& codecName );
	void setCodec( const ECodecType type, const AVCodecID codecId );
	
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
}

#endif


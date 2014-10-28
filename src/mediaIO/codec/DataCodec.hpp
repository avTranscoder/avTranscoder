#ifndef _MEDIA_MANAGER_IO_CODEC_DATA_CODEC_HPP_
#define _MEDIA_MANAGER_IO_CODEC_DATA_CODEC_HPP_

#include "ICodec.hpp"

namespace mediamanager
{
namespace mediaio
{

class MediaManagerExport DataCodec : public ICodec
{
public:
	DataCodec( const ECodecType type, const std::string& codecName = "" );
	DataCodec( const ECodecType type, const AVCodecID codecId );
	DataCodec( const ICodec& codec );
};

}
}

#endif
#ifndef _AV_TRANSCODER_CODED_STRUCTURES_DATA_DESC_HPP_
#define _AV_TRANSCODER_CODED_STRUCTURES_DATA_DESC_HPP_

#include "ICodec.hpp"
#include <AvTranscoder/common.hpp>

#include <string>

namespace avtranscoder
{

class AvExport DataCodec : public ICodec
{
public:
	DataCodec( const std::string& codecName = "" );
	DataCodec( const AVCodecID codecId );
	DataCodec( const ICodec& essenceDesc );
};

}

#endif
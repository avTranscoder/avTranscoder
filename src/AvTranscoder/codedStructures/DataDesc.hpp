#ifndef _AV_TRANSCODER_CODED_STRUCTURES_DATA_DESC_HPP_
#define _AV_TRANSCODER_CODED_STRUCTURES_DATA_DESC_HPP_

#include "CodedDesc.hpp"
#include <AvTranscoder/common.hpp>

#include <string>

namespace avtranscoder
{

class AvExport DataDesc : public CodedDesc
{
public:
	DataDesc( const std::string& codecName = "" );
	DataDesc( const AVCodecID codecId );
	DataDesc( const CodedDesc& essenceDesc );
};

}

#endif
#include "DataDesc.hpp"

namespace avtranscoder
{

DataDesc::DataDesc( const std::string& codecName )
	: CodedDesc( "" )
{
}

DataDesc::DataDesc( const AVCodecID codecId )
	: CodedDesc( "" )
{
}

DataDesc::DataDesc( const CodedDesc& essenceDesc )
	: CodedDesc( "" )
{
}

}

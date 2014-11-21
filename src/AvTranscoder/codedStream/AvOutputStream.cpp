#include "AvOutputStream.hpp"

#include <AvTranscoder/file/OutputFile.hpp>

#include <cassert>

namespace avtranscoder
{

AvOutputStream::AvOutputStream( OutputFile& outputFile, const size_t streamIndex )
	: IOutputStream()
	, _outputFile( &outputFile )
	, _streamIndex( streamIndex )
{
}

IOutputStream::EWrappingStatus AvOutputStream::wrap( const CodedData& data )
{
	assert( _outputFile != NULL );
	return _outputFile->wrap( data, _streamIndex );
}

}

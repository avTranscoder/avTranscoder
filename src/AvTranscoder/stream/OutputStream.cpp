#include "OutputStream.hpp"

#include <AvTranscoder/file/OutputFile.hpp>

#include <cassert>

namespace avtranscoder
{

OutputStream::OutputStream( OutputFile& outputFile, const size_t streamIndex )
	: IOutputStream()
	, _outputFile( &outputFile )
	, _streamIndex( streamIndex )
{
}

IOutputStream::EWrappingStatus OutputStream::wrap( const CodedData& data )
{
	assert( _outputFile != NULL );
	return _outputFile->wrap( data, _streamIndex );
}

}

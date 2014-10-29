#include "AvOutputStream.hpp"

#include <mediaIO/file/OutputFile.hpp>

#include <cassert>

namespace mediaengine
{
namespace mediaio
{

AvOutputStream::AvOutputStream( OutputFile& outputFile, const size_t streamIndex )
	: IOutputStream()
	, _outputFile( &outputFile )
	, _streamIndex( streamIndex )
{
}

bool AvOutputStream::wrap( const mediacore::CodedData& data )
{
	assert( _outputFile != NULL );
	return _outputFile->wrap( data, _streamIndex );
}

}
}

#include "AvOutputStream.hpp"

#include <AvTranscoder/File/OutputFile.hpp>

#include <cassert>
#include <iostream>

namespace avtranscoder
{

AvOutputStream::AvOutputStream( )
	: OutputStream()
	, _outputFile( NULL )
	, _streamIndex( 0 )
{
}

AvOutputStream::AvOutputStream( OutputFile& outputFile, const size_t streamIndex )
	: OutputStream()
	, _outputFile( &outputFile )
	, _streamIndex( streamIndex )
{
}

AvOutputStream::~AvOutputStream()
{
}

bool AvOutputStream::wrap( DataStream& data )
{
	assert( _outputFile != NULL );
	
	return _outputFile->wrap( data, _streamIndex );
}

VideoDesc AvOutputStream::getVideoDesc() const
{
	return _videoDesc;
}

AudioDesc AvOutputStream::getAudioDesc() const
{
	return _audioDesc;
}

}

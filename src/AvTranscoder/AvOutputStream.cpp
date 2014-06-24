#include "AvOutputStream.hpp"

#include "OutputFile.hpp"

namespace avtranscoder
{

AvOutputStream::AvOutputStream( )
	: _outputFile( NULL )
	, _streamIndex( 0 )
{
}

AvOutputStream::AvOutputStream( OutputFile& outputFile, const size_t streamIndex )
	: _outputFile( &outputFile )
	, _streamIndex( streamIndex )
{
}

AvOutputStream::~AvOutputStream()
{
}

bool AvOutputStream::wrap( DataStream& data )
{
	_outputFile->wrap( data, _streamIndex );
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

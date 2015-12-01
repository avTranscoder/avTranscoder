#include "IReader.hpp"

#include <cassert>

namespace avtranscoder
{

IReader::IReader( const std::string& filename, const size_t streamIndex, const int channelIndex )
	: _inputFile( NULL )
	, _streamProperties( NULL )
	, _decoder( NULL )
	, _srcFrame( NULL )
	, _dstFrame( NULL )
	, _transform( NULL )
	, _streamIndex( streamIndex )
	, _channelIndex( channelIndex )
	, _currentFrame( -1 )
	, _inputFileAllocated( true )
{
	_inputFile = new InputFile( filename );
}

IReader::IReader( InputFile& inputFile, const size_t streamIndex, const int channelIndex )
	: _inputFile( &inputFile )
	, _streamProperties( NULL )
	, _decoder( NULL )
	, _srcFrame( NULL )
	, _dstFrame( NULL )
	, _transform( NULL )
	, _streamIndex( streamIndex )
	, _channelIndex( channelIndex )
	, _currentFrame( -1 )
	, _inputFileAllocated( false )
{}

IReader::~IReader()
{
	if( _inputFileAllocated )
		delete _inputFile;
}

Frame* IReader::readNextFrame()
{
	return readFrameAt( _currentFrame + 1 );
}

Frame* IReader::readPrevFrame()
{
	return readFrameAt( _currentFrame - 1 );
}

Frame* IReader::readFrameAt( const size_t frame )
{
	assert( _decoder != NULL );
	assert( _transform != NULL );
	assert( _srcFrame != NULL );
	assert( _dstFrame != NULL );

	// seek
	if( (int)frame != _currentFrame + 1 )
	{
		_inputFile->seekAtFrame( frame );
		_decoder->flushDecoder();
	}
	_currentFrame = frame;
	// decode
	if( _channelIndex != -1 )
		_decoder->decodeNextFrame( *_srcFrame, _channelIndex );
	else
		_decoder->decodeNextFrame( *_srcFrame );
	// transform
	_transform->convert( *_srcFrame, *_dstFrame );
	// return buffer
	return _dstFrame;
}

}

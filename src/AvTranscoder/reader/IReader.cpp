#include "IReader.hpp"

namespace avtranscoder
{

IReader::IReader( const std::string& filename, const size_t streamIndex )
	: _inputFile( NULL )
	, _streamProperties( NULL )
	, _decoder( NULL )
	, _srcFrame( NULL )
	, _dstFrame( NULL )
	, _transform( NULL )
	, _streamIndex( streamIndex )
	, _currentFrame( 0 )
	, _inputFileAllocated( true )
{
	_inputFile = new InputFile( filename );
}

IReader::IReader( InputFile& inputFile, const size_t streamIndex )
	: _inputFile( &inputFile )
	, _streamProperties( NULL )
	, _decoder( NULL )
	, _srcFrame( NULL )
	, _dstFrame( NULL )
	, _transform( NULL )
	, _streamIndex( streamIndex )
	, _currentFrame( 0 )
	, _inputFileAllocated( false )
{}

IReader::~IReader()
{
	if( _inputFileAllocated )
		delete _inputFile;
}

const char* IReader::readNextFrame()
{
	return readFrameAt( _currentFrame + 1 );
}

const char* IReader::readPrevFrame()
{
	return readFrameAt( _currentFrame - 1 );
}

const char* IReader::readFrameAt( const size_t frame )
{
	_currentFrame = frame;
	// seek
	_inputFile->seekAtFrame( frame );
	_decoder->flushDecoder();
	// decode
	_decoder->decodeNextFrame( *_srcFrame );
	_transform->convert( *_srcFrame, *_dstFrame );
	// return buffer
	return (const char*)_dstFrame->getData();
}
}

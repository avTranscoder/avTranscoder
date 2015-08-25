#include "AudioReader.hpp"

#include <AvTranscoder/progress/NoDisplayProgress.hpp>
#include <AvTranscoder/mediaProperty/print.hpp>

namespace avtranscoder
{

AudioReader::AudioReader( const std::string& filename, const size_t audioStreamIndex )
	: _inputFile( filename )
	, _audioProperties( NULL )
	, _audioDecoder( NULL )
	, _sourceFrame( NULL )
	, _dstFrame( NULL )
	, _audioTransform()
	, _audioStreamIndex( audioStreamIndex )
{
	avtranscoder::NoDisplayProgress p;
	_inputFile.analyse( p );
	_audioProperties = &static_cast<const AudioProperties&>(_inputFile.getProperties().getStreamPropertiesWithIndex(_audioStreamIndex));
	_inputFile.activateStream( _audioStreamIndex );

	_audioDecoder = new avtranscoder::AudioDecoder( _inputFile.getStream( _audioStreamIndex ) );
	_audioDecoder->setupDecoder();

	_sourceFrame = new avtranscoder::AudioFrame( _inputFile.getStream( _audioStreamIndex ).getAudioCodec().getAudioFrameDesc() );

	avtranscoder::AudioFrameDesc dstAudioFrame( _sourceFrame->desc().getSampleRate(), _sourceFrame->desc().getChannels(), _sourceFrame->desc().getSampleFormat() );
	_dstFrame = new avtranscoder::AudioFrame( dstAudioFrame );
}

AudioReader::~AudioReader()
{
	delete _audioDecoder;
	delete _sourceFrame;
	delete _dstFrame;
}

size_t AudioReader::getSampleRate()
{
	return _audioProperties->getSampleRate();
}

size_t AudioReader::getChannels()
{
	return _audioProperties->getChannels();
}

const char* AudioReader::readNextFrame()
{
	return readFrameAt( _currentFrame + 1 );
}

const char* AudioReader::readPrevFrame()
{
	return readFrameAt( _currentFrame - 1 );
}

const char* AudioReader::readFrameAt( const size_t frame )
{
	_currentFrame = frame;
	// seek
	_inputFile.seekAtFrame( frame );
	_audioDecoder->flushDecoder();
	// decode
	_audioDecoder->decodeNextFrame( *_sourceFrame );
	_audioTransform.convert( *_sourceFrame, *_dstFrame );
	// return buffer
	return (const char*)_dstFrame->getData();
}

void AudioReader::printInfo()
{
	std::cout << *_audioProperties << std::endl;
}

}

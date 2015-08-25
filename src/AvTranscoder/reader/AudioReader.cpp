#include "AudioReader.hpp"

#include <AvTranscoder/decoder/AudioDecoder.hpp>
#include <AvTranscoder/frame/AudioFrame.hpp>
#include <AvTranscoder/transform/AudioTransform.hpp>
#include <AvTranscoder/progress/NoDisplayProgress.hpp>
#include <AvTranscoder/mediaProperty/print.hpp>

namespace avtranscoder
{

AudioReader::AudioReader( const std::string& filename, const size_t audioStreamIndex )
	: IReader( filename, audioStreamIndex )
	, _audioStreamProperties(NULL)
{
	init();
}

AudioReader::AudioReader( InputFile& inputFile, const size_t audioStreamIndex )
	: IReader( inputFile, audioStreamIndex )
	, _audioStreamProperties(NULL)
{
	init();
}

void AudioReader::init()
{
	// analyse InputFile
	avtranscoder::NoDisplayProgress p;
	_inputFile->analyse( p );
	_streamProperties = &_inputFile->getProperties().getStreamPropertiesWithIndex(_streamIndex);
	_audioStreamProperties = static_cast<const AudioProperties*>(_streamProperties);
	_inputFile->activateStream( _streamIndex );

	// setup decoder
	_decoder = new AudioDecoder( _inputFile->getStream( _streamIndex ) );
	_decoder->setupDecoder();

	// create src and dst frames
	_srcFrame = new AudioFrame( _inputFile->getStream( _streamIndex ).getAudioCodec().getAudioFrameDesc() );
	AudioFrame* srcFrame = static_cast<AudioFrame*>(_srcFrame);
	AudioFrameDesc dstAudioFrame( srcFrame->desc().getSampleRate(), srcFrame->desc().getChannels(), srcFrame->desc().getSampleFormat() );
	_dstFrame = new AudioFrame( dstAudioFrame );

	// create transform
	_transform = new AudioTransform();
}

AudioReader::~AudioReader()
{
	delete _decoder;
	delete _srcFrame;
	delete _dstFrame;
	delete _transform;
}

size_t AudioReader::getSampleRate()
{
	return _audioStreamProperties->getSampleRate();
}

size_t AudioReader::getChannels()
{
	return _audioStreamProperties->getChannels();
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
	_inputFile->seekAtFrame( frame );
	// decode
	_decoder->decodeNextFrame( *_srcFrame );
	_transform->convert( *_srcFrame, *_dstFrame );
	// return buffer
	return (const char*)_dstFrame->getData();
}

void AudioReader::printInfo()
{
	std::cout << *_audioStreamProperties << std::endl;
}

}

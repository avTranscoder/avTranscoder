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
	, _sampleRate( 0 )
	, _nbChannels( 0 )
	, _sampleFormat( AV_SAMPLE_FMT_S16 )
{
	init();
}

AudioReader::AudioReader( InputFile& inputFile, const size_t audioStreamIndex )
	: IReader( inputFile, audioStreamIndex )
	, _audioStreamProperties(NULL)
	, _sampleRate( 0 )
	, _nbChannels( 0 )
	, _sampleFormat( AV_SAMPLE_FMT_S16 )
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

	// create transform
	_transform = new AudioTransform();

	// create src frame
	_srcFrame = new AudioFrame( _inputFile->getStream( _streamIndex ).getAudioCodec().getAudioFrameDesc() );
	AudioFrame* srcFrame = static_cast<AudioFrame*>(_srcFrame);
	// create dst frame
	_sampleRate = srcFrame->desc().getSampleRate();
	_nbChannels = srcFrame->desc().getChannels();
	_dstFrame = new AudioFrame( AudioFrameDesc( _sampleRate, _nbChannels, _sampleFormat ) );
}

AudioReader::~AudioReader()
{
	delete _decoder;
	delete _srcFrame;
	delete _dstFrame;
	delete _transform;
}

void AudioReader::updateOutput( const size_t sampleRate, const size_t nbChannels, const std::string& sampleFormat )
{
	_sampleRate = sampleRate;
	_nbChannels = nbChannels;
	_sampleFormat = av_get_sample_fmt( sampleFormat.c_str() );
	// update dst frame
	delete _dstFrame;
	_dstFrame = new AudioFrame( AudioFrameDesc( _sampleRate, _nbChannels, _sampleFormat ) );
}

size_t AudioReader::getSampleRate()
{
	return _sampleRate;
}

size_t AudioReader::getChannels()
{
	return _nbChannels;
}

AVSampleFormat AudioReader::getSampleFormat()
{
	return _sampleFormat;
}

void AudioReader::printInfo()
{
	std::cout << *_audioStreamProperties << std::endl;
}

}

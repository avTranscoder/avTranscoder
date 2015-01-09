
#include "StreamTranscoder.hpp"

#include <AvTranscoder/stream/InputStream.hpp>

#include <AvTranscoder/decoder/VideoDecoder.hpp>
#include <AvTranscoder/decoder/AudioDecoder.hpp>
#include <AvTranscoder/decoder/VideoGenerator.hpp>
#include <AvTranscoder/decoder/AudioGenerator.hpp>
#include <AvTranscoder/encoder/VideoEncoder.hpp>
#include <AvTranscoder/encoder/AudioEncoder.hpp>

#include <AvTranscoder/transform/AudioTransform.hpp>
#include <AvTranscoder/transform/VideoTransform.hpp>

#include <cassert>
#include <iostream>
#include <limits>

namespace avtranscoder
{

StreamTranscoder::StreamTranscoder(
		IInputStream& inputStream,
		OutputFile& outputFile
	)
	: _inputStream( &inputStream )
	, _outputStream( NULL )
	, _sourceBuffer( NULL )
	, _frameBuffer( NULL )
	, _inputDecoder( NULL )
	, _generator( NULL )
	, _currentDecoder( NULL )
	, _outputEncoder( NULL )
	, _transform( NULL )
	, _subStreamIndex( -1 )
	, _frameProcessed( 0 )
	, _offset( 0 )
	, _takeFromGenerator( false )
	, _verbose( false )
	, _offsetPassed( false )
	, _infinityStream( false )
{
	// create a re-wrapping case
	switch( _inputStream->getStreamType() )
	{
		case AVMEDIA_TYPE_VIDEO :
		{
			_outputStream = &outputFile.addVideoStream( _inputStream->getVideoCodec() );
			break;
		}
		case AVMEDIA_TYPE_AUDIO :
		{
			_outputStream = &outputFile.addAudioStream( _inputStream->getAudioCodec() );
			break;
		}
		case AVMEDIA_TYPE_DATA :
		{
			_outputStream = &outputFile.addDataStream( _inputStream->getDataCodec() );
			break;
		}
		default:
			break;
	}
}

StreamTranscoder::StreamTranscoder(
		IInputStream& inputStream,
		OutputFile& outputFile,
		const ProfileLoader::Profile& profile,
		const int subStreamIndex,
		const size_t offset
	)
	: _inputStream( &inputStream )
	, _outputStream( NULL )
	, _sourceBuffer( NULL )
	, _frameBuffer( NULL )
	, _inputDecoder( NULL )
	, _generator( NULL )
	, _currentDecoder( NULL )
	, _outputEncoder( NULL )
	, _transform( NULL )
	, _subStreamIndex( subStreamIndex )
	, _frameProcessed( 0 )
	, _offset( offset )
	, _takeFromGenerator( false )
	, _verbose( false )
	, _offsetPassed( false )
	, _infinityStream( false )
{
	// create a transcode case
	switch( _inputStream->getStreamType() )
	{
		case AVMEDIA_TYPE_VIDEO :
		{
			_inputDecoder = new VideoDecoder( *static_cast<InputStream*>( _inputStream ) );
			_inputDecoder->setup();

			VideoEncoder* outputVideo = new VideoEncoder( profile.at( constants::avProfileCodec ) );

			_outputEncoder = outputVideo;

			VideoFrameDesc outputFrameDesc = _inputStream->getVideoCodec().getVideoFrameDesc();
			outputFrameDesc.setParameters( profile );
			outputVideo->setProfile( profile, outputFrameDesc );
			
			_outputStream = &outputFile.addVideoStream( outputVideo->getVideoCodec() );

			_sourceBuffer = new VideoFrame( _inputStream->getVideoCodec().getVideoFrameDesc() );
			_frameBuffer = new VideoFrame( outputVideo->getVideoCodec().getVideoFrameDesc() );
			
			_transform = new VideoTransform();

			VideoGenerator* generatorVideo = new VideoGenerator();
			generatorVideo->setVideoFrameDesc( outputVideo->getVideoCodec().getVideoFrameDesc() );
			_generator = generatorVideo;
			
			break;
		}
		case AVMEDIA_TYPE_AUDIO :
		{
			_inputDecoder = new AudioDecoder( *static_cast<InputStream*>( _inputStream ) );
			_inputDecoder->setup();
			
			AudioEncoder* outputAudio = new AudioEncoder( profile.at( constants::avProfileCodec )  );

			_outputEncoder = outputAudio;
			
			AudioFrameDesc outputFrameDesc( _inputStream->getAudioCodec().getAudioFrameDesc() );
			outputFrameDesc.setParameters( profile );
			if( subStreamIndex > -1 )
			{
				// @todo manage downmix ?
				outputFrameDesc.setChannels( 1 );
			}
			outputAudio->setProfile( profile, outputFrameDesc );

			_outputStream = &outputFile.addAudioStream( outputAudio->getAudioCodec() );

			AudioFrameDesc inputFrameDesc( _inputStream->getAudioCodec().getAudioFrameDesc() );
			if( subStreamIndex > -1 )
				inputFrameDesc.setChannels( 1 );
			
			_sourceBuffer = new AudioFrame( inputFrameDesc );
			_frameBuffer  = new AudioFrame( outputAudio->getAudioCodec().getAudioFrameDesc() );
			
			_transform = new AudioTransform();

			AudioGenerator* generatorAudio = new AudioGenerator();
			generatorAudio->setAudioFrameDesc( outputAudio->getAudioCodec().getAudioFrameDesc() );
			_generator = generatorAudio;

			break;
		}
		default:
		{
			throw std::runtime_error( "unupported stream type" );
			break;
		}
	}
	switchEssence( offset != 0 );
}

StreamTranscoder::StreamTranscoder(
		const ICodec& inputCodec,
		OutputFile& outputFile,
		const ProfileLoader::Profile& profile
	)
	: _inputStream( NULL )
	, _outputStream( NULL )
	, _sourceBuffer( NULL )
	, _frameBuffer( NULL )
	, _inputDecoder( NULL )
	, _generator( NULL )
	, _currentDecoder( NULL )
	, _outputEncoder( NULL )
	, _transform( NULL )
	, _subStreamIndex( -1 )
	, _frameProcessed( 0 )
	, _offset( 0 )
	, _takeFromGenerator( false )
	, _verbose( false )
	, _offsetPassed( false )
	, _infinityStream( false )
{
	// create a coding case based on a InputEssence (aka generator)
	if( ! profile.count( constants::avProfileType ) )
		throw std::runtime_error( "unable to found stream type (audio, video, etc.)" );

	if( profile.find( constants::avProfileType )->second == constants::avProfileTypeVideo )
	{
		// Create input essence based on a given input VideoCodec
		VideoGenerator* generatorVideo = new VideoGenerator();
		const VideoCodec& inputVideoCodec = static_cast<const VideoCodec&>( inputCodec );
		generatorVideo->setVideoFrameDesc( inputVideoCodec.getVideoFrameDesc() );
		_inputDecoder = generatorVideo;

		// Create inputFrame, and outputFrame which is based on a given profile
		VideoFrameDesc inputFrameDesc = inputVideoCodec.getVideoFrameDesc();
		VideoFrameDesc outputFrameDesc = inputFrameDesc;
		outputFrameDesc.setParameters( profile );
		_sourceBuffer = new VideoFrame( inputFrameDesc );
		_frameBuffer  = new VideoFrame( outputFrameDesc );

		// Create output essence
		VideoEncoder* outputVideo = new VideoEncoder( profile.at( constants::avProfileCodec ) );
		outputVideo->setProfile( profile, outputFrameDesc );
		_outputEncoder = outputVideo;

		// Create a video stream in the output file
		_outputStream = &outputFile.addVideoStream( outputVideo->getVideoCodec() );

		_transform = new VideoTransform();

		_currentDecoder = _inputDecoder;
	}
	else if( profile.find( constants::avProfileType )->second == constants::avProfileTypeAudio )
	{
		// Create input essence based on a given input AudioCodec
		AudioGenerator* generatorAudio = new AudioGenerator();
		const AudioCodec& inputAudioCodec = static_cast<const AudioCodec&>( inputCodec );
		generatorAudio->setAudioFrameDesc( inputAudioCodec.getAudioFrameDesc() );
		_inputDecoder = generatorAudio;

		// Create inputFrame, and outputFrame which is based on a given profile
		AudioFrameDesc inputFrameDesc = inputAudioCodec.getAudioFrameDesc();
		AudioFrameDesc outputFrameDesc = inputFrameDesc;
		outputFrameDesc.setParameters( profile );
		_sourceBuffer = new AudioFrame( inputFrameDesc );
		_frameBuffer  = new AudioFrame( outputFrameDesc );

		// Create output essence
		AudioEncoder* outputAudio = new AudioEncoder( profile.at( constants::avProfileCodec ) );
		outputAudio->setProfile( profile, outputFrameDesc );
		_outputEncoder = outputAudio;

		// Create an audio stream in the output file
		_outputStream = &outputFile.addAudioStream( outputAudio->getAudioCodec() );

		_transform = new AudioTransform();

		_currentDecoder = _inputDecoder;
	}
	else
	{
		throw std::runtime_error( "unupported stream type" );
	}
}

StreamTranscoder::~StreamTranscoder()
{
	delete _frameBuffer;
	delete _sourceBuffer;
	delete _generator;
	delete _outputEncoder;
	delete _transform;
	delete _inputDecoder;
}

void StreamTranscoder::preProcessCodecLatency()
{
	// rewrap case: no need to take care of the latency of codec
	if( ! _inputDecoder )
		return;

	int latency = _outputEncoder->getCodec().getLatency();
	if( _verbose )
		std::cout << "latency of stream: " << latency << std::endl;

	if( ! latency ||
		latency < _outputEncoder->getCodec().getAVCodecContext().frame_number )
		return;

	while( ( --latency ) > 0 )
	{
		processFrame();
	}
}

bool StreamTranscoder::processFrame()
{
	++_frameProcessed;

	if( ! _inputDecoder )
	{
		return processRewrap();
	}

	if( _subStreamIndex < 0 )
	{
		return processTranscode();
	}
	return processTranscode( _subStreamIndex );	
}

bool StreamTranscoder::processRewrap()
{
	assert( _inputStream  != NULL );
	assert( _outputStream != NULL );
	
	CodedData data;

	if( ! _inputStream->readNextPacket( data ) )
		return false;

	IOutputStream::EWrappingStatus wrappingStatus = _outputStream->wrap( data );

	switch( wrappingStatus )
	{
		case IOutputStream::eWrappingSuccess:
			return true;
		case IOutputStream::eWrappingWaitingForData:
			// the wrapper needs more data to write the current packet
			return processRewrap();
		case IOutputStream::eWrappingError:
			return false;
	}

	return true;
}

bool StreamTranscoder::processTranscode( const int subStreamIndex )
{
	assert( _inputDecoder   != NULL );
	assert( _currentDecoder != NULL );
	assert( _outputEncoder  != NULL );
	assert( _sourceBuffer   != NULL );
	assert( _frameBuffer    != NULL );
	assert( _transform      != NULL );

	CodedData data;
	if( _verbose )
		std::cout << "transcode a frame " << std::endl;

	if( _offset &&
		_frameProcessed > _offset &&
		! _offsetPassed &&
		_takeFromGenerator )
	{
		switchToInputEssence();
		_offsetPassed = true;
	}

	bool decodingStatus = false;
	if( subStreamIndex == -1 )
		decodingStatus = _currentDecoder->decodeNextFrame( *_sourceBuffer );
	else
		decodingStatus = _currentDecoder->decodeNextFrame( *_sourceBuffer, subStreamIndex );

	if( decodingStatus )
	{
		if( _verbose )
			std::cout << "convert (" << _sourceBuffer->getSize() << " bytes)" << std::endl;
		_transform->convert( *_sourceBuffer, *_frameBuffer );
		if( _verbose )
			std::cout << "encode (" << _frameBuffer->getSize() << " bytes)" << std::endl;
		_outputEncoder->encodeFrame( *_frameBuffer, data );
	}
	else
	{
		if( _verbose )
			std::cout << "encode last frame(s)" << std::endl;
		if( ! _outputEncoder->encodeFrame( data ) )
		{
			if( _infinityStream )
			{
				switchToGeneratorEssence();
				return processTranscode();
			}
			return false;
		}
	}
	if( _verbose )
		std::cout << "wrap (" << data.getSize() << " bytes)" << std::endl;

	IOutputStream::EWrappingStatus wrappingStatus = _outputStream->wrap( data );

	switch( wrappingStatus )
	{
		case IOutputStream::eWrappingSuccess:
			return true;
		case IOutputStream::eWrappingWaitingForData:
			// the wrapper needs more data to write the current packet
			return processTranscode( subStreamIndex );
		case IOutputStream::eWrappingError:
			return false;
	}

	return true;
}

void StreamTranscoder::switchEssence( bool swithToGenerator )
{
	_takeFromGenerator = swithToGenerator;
	_currentDecoder = swithToGenerator ? _generator : _inputDecoder;
	assert( _currentDecoder != NULL );
}

void StreamTranscoder::switchToGeneratorEssence()
{
	switchEssence( true );
}

void StreamTranscoder::switchToInputEssence()
{
	switchEssence( false );
}

double StreamTranscoder::getDuration() const
{	
	if( _inputStream )
	{
		double totalDuration = 0;
		totalDuration += _inputStream->getDuration();
		// @todo add offset
		return totalDuration;
	}
	else
		return std::numeric_limits<double>::max();
}

}

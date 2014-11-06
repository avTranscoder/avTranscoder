
#include "StreamTranscoder.hpp"

#include <AvTranscoder/codedStream/AvInputStream.hpp>

#include <AvTranscoder/essenceStream/AvInputVideo.hpp>
#include <AvTranscoder/essenceStream/AvInputAudio.hpp>
#include <AvTranscoder/essenceStream/AvOutputVideo.hpp>
#include <AvTranscoder/essenceStream/AvOutputAudio.hpp>
#include <AvTranscoder/essenceStream/GeneratorVideo.hpp>
#include <AvTranscoder/essenceStream/GeneratorAudio.hpp>

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
	, _inputEssence( NULL )
	, _generatorEssence( NULL )
	, _currentEssence( NULL )
	, _outputEssence( NULL )
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
	, _inputEssence( NULL )
	, _generatorEssence( NULL )
	, _currentEssence( NULL )
	, _outputEssence( NULL )
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
			_inputEssence = new AvInputVideo( *static_cast<AvInputStream*>( _inputStream ) );
			_inputEssence->setup();

			AvOutputVideo* outputVideo = new AvOutputVideo();

			_outputEssence = outputVideo;

			VideoFrameDesc outputFrameDesc = _inputStream->getVideoCodec().getVideoFrameDesc();
			outputFrameDesc.setParameters( profile );
			outputVideo->setProfile( profile, outputFrameDesc );
			
			_outputStream = &outputFile.addVideoStream( outputVideo->getVideoCodec() );

			_sourceBuffer = new VideoFrame( _inputStream->getVideoCodec().getVideoFrameDesc() );
			_frameBuffer = new VideoFrame( outputVideo->getVideoCodec().getVideoFrameDesc() );
			
			_transform = new VideoTransform();

			GeneratorVideo* generatorVideo = new GeneratorVideo();
			generatorVideo->setVideoCodec( outputVideo->getVideoCodec() );
			_generatorEssence = generatorVideo;
			
			break;
		}
		case AVMEDIA_TYPE_AUDIO :
		{
			_inputEssence = new AvInputAudio( *static_cast<AvInputStream*>( _inputStream ) );
			_inputEssence->setup();
			
			AvOutputAudio* outputAudio = new AvOutputAudio();

			_outputEssence = outputAudio;
			
			AudioFrameDesc outputFrameDesc( _inputStream->getAudioCodec().getFrameDesc() );
			outputFrameDesc.setParameters( profile );
			if( subStreamIndex > -1 )
			{
				// @todo manage downmix ?
				outputFrameDesc.setChannels( 1 );
			}
			outputAudio->setProfile( profile, outputFrameDesc );

			_outputStream = &outputFile.addAudioStream( outputAudio->getAudioCodec() );

			AudioFrameDesc inputFrameDesc( _inputStream->getAudioCodec().getFrameDesc() );
			if( subStreamIndex > -1 )
				inputFrameDesc.setChannels( 1 );
			
			_sourceBuffer = new AudioFrame( inputFrameDesc );
			_frameBuffer  = new AudioFrame( outputAudio->getAudioCodec().getFrameDesc() );
			
			_transform = new AudioTransform();

			GeneratorAudio* generatorAudio = new GeneratorAudio();
			generatorAudio->setAudioCodec( outputAudio->getAudioCodec() );
			_generatorEssence = generatorAudio;

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
	, _inputEssence( NULL )
	, _generatorEssence( NULL )
	, _currentEssence( NULL )
	, _outputEssence( NULL )
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
		GeneratorVideo* generatorVideo = new GeneratorVideo();
		const VideoCodec& inputVideoCodec = static_cast<const VideoCodec&>( inputCodec );
		generatorVideo->setVideoCodec( inputVideoCodec );
		_inputEssence = generatorVideo;

		// Create inputFrame, and outputFrame which is based on a given profile
		VideoFrameDesc inputFrameDesc = inputVideoCodec.getVideoFrameDesc();
		VideoFrameDesc outputFrameDesc = inputFrameDesc;
		outputFrameDesc.setParameters( profile );
		_sourceBuffer = new VideoFrame( inputFrameDesc );
		_frameBuffer  = new VideoFrame( outputFrameDesc );

		// Create output essence
		AvOutputVideo* outputVideo = new AvOutputVideo();
		outputVideo->setProfile( profile, outputFrameDesc );
		_outputEssence = outputVideo;

		// Create a video stream in the output file
		_outputStream = &outputFile.addVideoStream( outputVideo->getVideoCodec() );

		_transform = new VideoTransform();

		_currentEssence = _inputEssence;
	}
	else if( profile.find( constants::avProfileType )->second == constants::avProfileTypeAudio )
	{
		// Create input essence based on a given input AudioCodec
		GeneratorAudio* generatorAudio = new GeneratorAudio();
		const AudioCodec& inputAudioCodec = static_cast<const AudioCodec&>( inputCodec );
		generatorAudio->setAudioCodec( inputAudioCodec );
		_inputEssence = generatorAudio;

		// Create inputFrame, and outputFrame which is based on a given profile
		AudioFrameDesc inputFrameDesc = inputAudioCodec.getFrameDesc();
		AudioFrameDesc outputFrameDesc = inputFrameDesc;
		outputFrameDesc.setParameters( profile );
		_sourceBuffer = new AudioFrame( inputFrameDesc );
		_frameBuffer  = new AudioFrame( outputFrameDesc );

		// Create output essence
		AvOutputAudio* outputAudio = new AvOutputAudio();
		outputAudio->setProfile( profile, outputFrameDesc );
		_outputEssence = outputAudio;

		// Create an audio stream in the output file
		_outputStream = &outputFile.addAudioStream( outputAudio->getAudioCodec() );

		_transform = new AudioTransform();

		_currentEssence = _inputEssence;
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
	delete _generatorEssence;
	delete _outputEssence;
	delete _transform;
	delete _inputEssence;
}

void StreamTranscoder::init()
{
	// rewrap
	if( ! _inputEssence )
		return;

	int latency = _outputEssence->getCodec().getLatency();
	if( _verbose )
		std::cout << "latency of stream: " << latency << std::endl;

	if( ! latency ||
		latency < _outputEssence->getCodec().getAVCodecContext()->frame_number )
		return;

	while( ( --latency ) > 0 )
	{
		processFrame();
	}
}

bool StreamTranscoder::processFrame()
{
	++_frameProcessed;

	if( ! _inputEssence )
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

	_outputStream->wrap( data );
	return true;
}

bool StreamTranscoder::processTranscode()
{
	assert( _inputEssence   != NULL );
	assert( _currentEssence != NULL );
	assert( _outputEssence  != NULL );
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

	if( _currentEssence->readNextFrame( *_sourceBuffer ) )
	{
		if( _verbose )
			std::cout << "convert " << _sourceBuffer->getSize() << std::endl;
		_transform->convert( *_sourceBuffer, *_frameBuffer );
		if( _verbose )
			std::cout << "encode " << _frameBuffer->getSize() << std::endl;
		_outputEssence->encodeFrame( *_frameBuffer, data );
	}
	else
	{
		if( _verbose )
			std::cout << "encode last frame(s)" << std::endl;
		if( ! _outputEssence->encodeFrame( data ) )
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
		std::cout << "wrap (" << data.getSize() << ")" << std::endl;
	_outputStream->wrap( data );
	return true;
}

bool StreamTranscoder::processTranscode( const int subStreamIndex )
{
	assert( _inputEssence   != NULL );
	assert( _currentEssence != NULL );
	assert( _outputEssence  != NULL );
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

	if( _currentEssence->readNextFrame( *_sourceBuffer, subStreamIndex ) )
	{
		if( _verbose )
			std::cout << "convert " << std::endl;
		_transform->convert( *_sourceBuffer, *_frameBuffer );
		if( _verbose )
			std::cout << "encode" << std::endl;
		_outputEssence->encodeFrame( *_frameBuffer, data );
	}
	else
	{
		if( _verbose )
			std::cout << "encode last frame(s)" << std::endl;
		if( ! _outputEssence->encodeFrame( data ) )
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
		std::cout << "wrap (" << data.getSize() << ")" << std::endl;
	_outputStream->wrap( data );
	return true;
}

void StreamTranscoder::switchEssence( bool swithToGenerator )
{
	_takeFromGenerator = swithToGenerator;
	_currentEssence = swithToGenerator ? _generatorEssence : _inputEssence;
	assert( _currentEssence != NULL );
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

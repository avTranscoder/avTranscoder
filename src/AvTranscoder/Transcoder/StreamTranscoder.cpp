
#include "StreamTranscoder.hpp"

#include <AvTranscoder/CodedStream/AvInputStream.hpp>

#include <AvTranscoder/EssenceStream/InputVideo.hpp>
#include <AvTranscoder/EssenceStream/InputAudio.hpp>
#include <AvTranscoder/EssenceStream/OutputVideo.hpp>
#include <AvTranscoder/EssenceStream/OutputAudio.hpp>
#include <AvTranscoder/EssenceStream/DummyVideo.hpp>
#include <AvTranscoder/EssenceStream/DummyAudio.hpp>

#include <AvTranscoder/EssenceTransform/AudioEssenceTransform.hpp>
#include <AvTranscoder/EssenceTransform/VideoEssenceTransform.hpp>

#include <cassert>
#include <iostream>
#include <limits>

namespace avtranscoder
{

StreamTranscoder::StreamTranscoder(
		InputStream& inputStream,
		OutputFile& outputFile
	)
	: _inputStream( &inputStream )
	, _outputStream( NULL )
	, _sourceBuffer( NULL )
	, _frameBuffer( NULL )
	, _inputEssence( NULL )
	, _dummyEssence( NULL )
	, _currentEssence( NULL )
	, _outputEssence( NULL )
	, _transform( NULL )
	, _subStreamIndex( -1 )
	, _frameProcessed( 0 )
	, _offset( 0 )
	, _takeFromDummy( false )
	, _verbose( false )
	, _offsetPassed( false )
	, _infinityStream( false )
{
	// create a re-wrapping case
	switch( _inputStream->getStreamType() )
	{
		case AVMEDIA_TYPE_VIDEO :
		{
			_outputStream = &outputFile.addVideoStream( _inputStream->getVideoDesc() );
			break;
		}
		case AVMEDIA_TYPE_AUDIO :
		{
			_outputStream = &outputFile.addAudioStream( _inputStream->getAudioDesc() );
			break;
		}
		case AVMEDIA_TYPE_DATA :
		{
			_outputStream = &outputFile.addDataStream( _inputStream->getDataDesc() );
			break;
		}
		default:
			break;
	}
}

StreamTranscoder::StreamTranscoder(
		InputStream& inputStream,
		OutputFile& outputFile,
		const Profile::ProfileDesc& profile,
		const int subStreamIndex,
		const size_t offset
	)
	: _inputStream( &inputStream )
	, _outputStream( NULL )
	, _sourceBuffer( NULL )
	, _frameBuffer( NULL )
	, _inputEssence( NULL )
	, _dummyEssence( NULL )
	, _currentEssence( NULL )
	, _outputEssence( NULL )
	, _transform( NULL )
	, _subStreamIndex( subStreamIndex )
	, _frameProcessed( 0 )
	, _offset( offset )
	, _takeFromDummy( false )
	, _verbose( false )
	, _offsetPassed( false )
	, _infinityStream( false )
{
	// create a transcode case
	switch( _inputStream->getStreamType() )
	{
		case AVMEDIA_TYPE_VIDEO :
		{
			_inputEssence = new InputVideo( *static_cast<AvInputStream*>( _inputStream ) );
			_inputEssence->setup();

			OutputVideo* outputVideo = new OutputVideo();

			_outputEssence = outputVideo;

			VideoFrameDesc outputFrameDesc = _inputStream->getVideoDesc().getVideoFrameDesc();
			outputFrameDesc.setParameters( profile );
			outputVideo->setProfile( profile, outputFrameDesc );
			
			_outputStream = &outputFile.addVideoStream( outputVideo->getVideoDesc() );

			_sourceBuffer = new VideoFrame( _inputStream->getVideoDesc().getVideoFrameDesc() );
			_frameBuffer = new VideoFrame( outputVideo->getVideoDesc().getVideoFrameDesc() );
			
			_transform = new VideoEssenceTransform();

			DummyVideo* dummyVideo = new DummyVideo();
			dummyVideo->setVideoDesc( outputVideo->getVideoDesc() );
			_dummyEssence = dummyVideo;
			
			break;
		}
		case AVMEDIA_TYPE_AUDIO :
		{
			_inputEssence = new InputAudio( *static_cast<AvInputStream*>( _inputStream ) );
			_inputEssence->setup();
			
			OutputAudio* outputAudio = new OutputAudio();

			_outputEssence = outputAudio;
			
			AudioFrameDesc outputFrameDesc( _inputStream->getAudioDesc().getFrameDesc() );
			outputFrameDesc.setParameters( profile );
			if( subStreamIndex > -1 )
			{
				// @todo manage downmix ?
				outputFrameDesc.setChannels( 1 );
			}
			outputAudio->setProfile( profile, outputFrameDesc );

			_outputStream = &outputFile.addAudioStream( outputAudio->getAudioDesc() );

			AudioFrameDesc inputFrameDesc( _inputStream->getAudioDesc().getFrameDesc() );
			if( subStreamIndex > -1 )
				inputFrameDesc.setChannels( 1 );
			
			_sourceBuffer = new AudioFrame( inputFrameDesc );
			_frameBuffer  = new AudioFrame( outputAudio->getAudioDesc().getFrameDesc() );
			
			_transform = new AudioEssenceTransform();

			DummyAudio* dummyAudio = new DummyAudio();
			dummyAudio->setAudioDesc( outputAudio->getAudioDesc() );
			_dummyEssence = dummyAudio;

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
		InputEssence& inputEssence,
		OutputFile& outputFile,
		const Profile::ProfileDesc& profile
	)
	: _inputStream( NULL )
	, _outputStream( NULL )
	, _sourceBuffer( NULL )
	, _frameBuffer( NULL )
	, _inputEssence( &inputEssence )
	, _dummyEssence( NULL )
	, _currentEssence( NULL )
	, _outputEssence( NULL )
	, _transform( NULL )
	, _subStreamIndex( -1 )
	, _frameProcessed( 0 )
	, _offset( 0 )
	, _takeFromDummy( false )
	, _verbose( false )
	, _offsetPassed( false )
	, _infinityStream( false )
{
	// create a coding case based on a InputEssence (aka dummy reader)
	if( ! profile.count( Profile::avProfileType ) )
		throw std::runtime_error( "unable to found stream type (audio, video, etc.)" );

	if( profile.find( Profile::avProfileType )->second == Profile::avProfileTypeVideo )
	{
		OutputVideo* outputVideo = new OutputVideo();
		
		_outputEssence = outputVideo;

		VideoFrameDesc inputFrameDesc = static_cast<DummyVideo*>( _inputEssence )->getVideoDesc().getVideoFrameDesc();

		VideoFrameDesc outputFrameDesc = inputFrameDesc;
		outputFrameDesc.setParameters( profile );
		outputVideo->setProfile( profile, outputFrameDesc );

		_outputStream = &outputFile.addVideoStream( outputVideo->getVideoDesc() );
		_sourceBuffer = new VideoFrame( inputFrameDesc );
		_frameBuffer  = new VideoFrame( outputFrameDesc );

		_transform = new VideoEssenceTransform();

		_currentEssence = _inputEssence;		

		return;
	}

	if( profile.find( Profile::avProfileType )->second == Profile::avProfileTypeAudio )
	{
		OutputAudio* outputAudio = new OutputAudio();

		_outputEssence = outputAudio;

		AudioFrameDesc inputFrameDesc = static_cast<DummyAudio*>( _inputEssence )->getAudioDesc().getFrameDesc();

		AudioFrameDesc outputFrameDesc = inputFrameDesc;
		outputFrameDesc.setParameters( profile );
		outputAudio->setProfile( profile, outputFrameDesc );

		_outputStream = &outputFile.addAudioStream( outputAudio->getAudioDesc() );
		_sourceBuffer = new AudioFrame( inputFrameDesc );
		_frameBuffer  = new AudioFrame( outputFrameDesc );

		_transform = new AudioEssenceTransform();
		
		_currentEssence = _inputEssence;
		return;
	}

	throw std::runtime_error( "unupported stream type" );
}

StreamTranscoder::~StreamTranscoder()
{
	if( _frameBuffer )
		delete _frameBuffer;
	if( _sourceBuffer )
		delete _sourceBuffer;
	if( _inputEssence && _inputStream )
		delete _inputEssence;
	if( _dummyEssence )
		delete _dummyEssence;
	if( _outputEssence )
		delete _outputEssence;
	if( _transform )
		delete _transform;
}

void StreamTranscoder::init()
{
	// rewrap
	if( ! _inputEssence )
		return;

	int latency = _outputEssence->getCodedDesc().getLatency();
	if( _verbose )
		std::cout << "latency of stream: " << latency << std::endl;

	if( ! latency ||
		latency < _outputEssence->getCodedDesc().getCodecContext()->frame_number )
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
		if( _subStreamIndex < 0 )
		{
			return processRewrap();
		}
		return processRewrap( _subStreamIndex );
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
	
	DataStream dataStream;

	if( ! _inputStream->readNextPacket( dataStream ) )
		return false;

	_outputStream->wrap( dataStream );
	return true;
}

bool StreamTranscoder::processRewrap( const int subStreamIndex )
{
	assert( _inputStream  != NULL );
	assert( _outputStream != NULL );
	
	DataStream dataStream;

	if( ! _inputStream->readNextPacket( dataStream ) )
		return false;
	_outputStream->wrap( dataStream );

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

	DataStream dataStream;
	if( _verbose )
		std::cout << "transcode a frame " << std::endl;

	if( _offset &&
		_frameProcessed > _offset &&
		! _offsetPassed &&
		_takeFromDummy )
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
		_outputEssence->encodeFrame( *_frameBuffer, dataStream );
	}
	else
	{
		if( _verbose )
			std::cout << "encode last frame(s)" << std::endl;
		if( ! _outputEssence->encodeFrame( dataStream ) )
		{
			if( _infinityStream )
			{
				switchToDummyEssence();
				return processTranscode();
			}
			return false;
		}
	}

	if( _verbose )
		std::cout << "wrap (" << dataStream.getSize() << ")" << std::endl;
	_outputStream->wrap( dataStream );
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

	DataStream dataStream;
	if( _verbose )
		std::cout << "transcode a frame " << std::endl;

	if( _offset &&
		_frameProcessed > _offset &&
		! _offsetPassed &&
		_takeFromDummy )
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
		_outputEssence->encodeFrame( *_frameBuffer, dataStream );
	}
	else
	{
		if( _verbose )
			std::cout << "encode last frame(s)" << std::endl;
		if( ! _outputEssence->encodeFrame( dataStream ) )
		{
			if( _infinityStream )
			{
				switchToDummyEssence();
				return processTranscode();
			}
			return false;
		}
	}
	if( _verbose )
		std::cout << "wrap (" << dataStream.getSize() << ")" << std::endl;
	_outputStream->wrap( dataStream );
	return true;
}

void StreamTranscoder::switchEssence( bool swithToDummy )
{
	_takeFromDummy = swithToDummy;
	_currentEssence = swithToDummy ? _dummyEssence : _inputEssence;
	assert( _currentEssence != NULL );
}

void StreamTranscoder::switchToDummyEssence()
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
	// dummy
	else
		return std::numeric_limits<double>::max();
}

}

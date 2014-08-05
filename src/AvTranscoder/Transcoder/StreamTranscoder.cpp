
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
	, _outputEssence( NULL )
	, _transform( NULL )
	, _subStreamIndex( -1 )
	, _transcodeStream( false )
	, _infiniteProcess( false )
	, _verbose( false )
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
		default:
			break;
	}
}

StreamTranscoder::StreamTranscoder(
		InputStream& inputStream,
		OutputFile& outputFile,
		const Profile::ProfileDesc& profile,
		const int subStreamIndex
	)
	: _inputStream( &inputStream )
	, _outputStream( NULL )
	, _sourceBuffer( NULL )
	, _frameBuffer( NULL )
	, _inputEssence( NULL )
	, _dummyEssence( NULL )
	, _outputEssence( NULL )
	, _transform( NULL )
	, _subStreamIndex( subStreamIndex )
	, _transcodeStream( true )
	, _infiniteProcess( false )
	, _verbose( false )
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

			_currentEssence = _inputEssence;
			
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

			_currentEssence = _inputEssence;

			break;
		}
		default:
		{
			throw std::runtime_error( "unupported stream type" );
			break;
		}
	}
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
	, _outputEssence( NULL )
	, _transform( NULL )
	, _subStreamIndex( -1 )
	, _transcodeStream( true )
	, _infiniteProcess( false )
	, _verbose( false )
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

bool StreamTranscoder::processFrame()
{
	if( _transcodeStream )
	{
		if( _subStreamIndex < 0 )
		{
			return processTranscode();
		}

		return processTranscode( _subStreamIndex );
	}

	if( _subStreamIndex < 0 )
	{
		return processRewrap();
	}

	return processRewrap( _subStreamIndex );	
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
	// std::vector<DataStream> dataStream;

	if( ! _inputStream->readNextPacket( dataStream ) )
		return false;
	_outputStream->wrap( dataStream );
	// outputStream.wrap( dataStream.at( subStreamIndex ) );

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
		if( _infiniteProcess )
		{
			switchToDummyEssence();
			return processTranscode( );
		}

		if( _verbose )
			std::cout << "encode last frame(s)" << std::endl;
		if( ! _outputEssence->encodeFrame( dataStream ) )
		{
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
		if( _infiniteProcess )
		{
			switchToDummyEssence();
			return processTranscode( );
		}

		if( ! _outputEssence->encodeFrame( dataStream ) )
		{
			return false;
		}
	}
	if( _verbose )
		std::cout << "wrap (" << dataStream.getSize() << ")" << std::endl;
	_outputStream->wrap( dataStream );
	return true;
}

void StreamTranscoder::switchToDummyEssence()
{
	if( _dummyEssence == NULL )
		return;
	_takeFromDummy = true;
	_currentEssence = _dummyEssence;
	assert( _currentEssence != NULL );
}

void StreamTranscoder::switchToInputEssence()
{
	if( _inputEssence == NULL )
		return;
	_takeFromDummy = false;
	_currentEssence = _inputEssence;
	assert( _currentEssence != NULL );
}

}


#include "StreamTranscoder.hpp"

#include <AvTranscoder/CodedStream/AvInputStream.hpp>

#include <AvTranscoder/EssenceStream/DummyVideo.hpp>
#include <AvTranscoder/EssenceStream/DummyAudio.hpp>

#include <AvTranscoder/EssenceTransform/AudioEssenceTransform.hpp>
#include <AvTranscoder/EssenceTransform/VideoEssenceTransform.hpp>

#include <cassert>

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
	, _outputEssence( NULL )
	, _transform( NULL )
	, _subStreamIndex( -1 )
	, _transcodeStream( false )
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
		Profile::ProfileDesc& profile,
		const int subStreamIndex
	)
	: _inputStream( &inputStream )
	, _outputStream( NULL )
	, _sourceBuffer( NULL )
	, _frameBuffer( NULL )
	, _inputEssence( NULL )
	, _outputEssence( NULL )
	, _transform( NULL )
	, _subStreamIndex( subStreamIndex )
	, _transcodeStream( true )
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

			ImageDesc outputImageDesc = _inputStream->getVideoDesc().getImageDesc();

			outputImageDesc.setPixel( Pixel( profile[ Profile::avProfilePixelFormat ].c_str() ) );

			outputVideo->setProfile( profile, outputImageDesc );
			
			_outputStream = &outputFile.addVideoStream( outputVideo->getVideoDesc() );

			_sourceBuffer = new Image( _inputStream->getVideoDesc().getImageDesc() );

			// outputVideo->getVideoDesc().setImageParameters( _inputStream->getVideoDesc().getImageDesc().getWidth(), _inputStream->getVideoDesc().getImageDesc().getHeight(), av_get_pix_fmt( desc[ Profile::avProfilePixelFormat ].c_str() ) );

			_frameBuffer = new Image( outputVideo->getVideoDesc().getImageDesc() );
			
			_transform = new VideoEssenceTransform();

			break;
		}
		case AVMEDIA_TYPE_AUDIO :
		{
			_inputEssence = new InputAudio( *static_cast<AvInputStream*>( _inputStream ) );
			_inputEssence->setup();
			
			OutputAudio* outputAudio = new OutputAudio();

			_outputEssence = outputAudio;
			AudioFrameDesc audioFrameDesc( _inputStream->getAudioDesc().getFrameDesc() );
			
			if( subStreamIndex > -1 )
				audioFrameDesc.setChannels( 1 );

			outputAudio->setProfile( profile, audioFrameDesc );

			_outputStream = &outputFile.addAudioStream( outputAudio->getAudioDesc() );

			_sourceBuffer = new AudioFrame( audioFrameDesc );
			_frameBuffer  = new AudioFrame( outputAudio->getAudioDesc().getFrameDesc() );
			
			_transform = new AudioEssenceTransform();

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
		Profile::ProfileDesc& profile
	)
	: _inputStream( NULL )
	, _outputStream( NULL )
	, _sourceBuffer( NULL )
	, _frameBuffer( NULL )
	, _inputEssence( &inputEssence )
	, _outputEssence( NULL )
	, _transform( NULL )
	, _subStreamIndex( -1 )
	, _transcodeStream( true )
	, _verbose( false )
{
	// create a coding case based on a InputEssence (aka dummy reader)
	if( ! profile.count( Profile::avProfileType ) )
		throw std::runtime_error( "unable to found stream type (audio, video, etc.)" );

	if( profile.find( Profile::avProfileType )->second == Profile::avProfileTypeAudio )
	{
		OutputAudio* outputAudio = new OutputAudio();

		_outputEssence = outputAudio;
		AudioFrameDesc srcAudioFrameDesc; // @todo better solution ?
		outputAudio->setProfile( profile, srcAudioFrameDesc );
		
		static_cast<DummyAudio*>( _inputEssence )->setAudioDesc( outputAudio->getAudioDesc() );
		
		_outputStream = &outputFile.addAudioStream( outputAudio->getAudioDesc() );
		_sourceBuffer = new AudioFrame( outputAudio->getAudioDesc().getFrameDesc() );
		_frameBuffer  = new AudioFrame( outputAudio->getAudioDesc().getFrameDesc() );

		_transform = new AudioEssenceTransform();
		
		return;
	}

	if( profile.find( Profile::avProfileType )->second == Profile::avProfileTypeVideo )
	{
		OutputVideo* outputVideo = new OutputVideo();
		
		_outputEssence = outputVideo;
		ImageDesc srcImageDesc; // @todo better solution ?
		outputVideo->setProfile( profile, srcImageDesc );

		_outputStream = &outputFile.addVideoStream( outputVideo->getVideoDesc() );
		_sourceBuffer = new Image( outputVideo->getVideoDesc().getImageDesc() );
		_frameBuffer  = new Image( outputVideo->getVideoDesc().getImageDesc() );
		
		_transform = new VideoEssenceTransform();
		
		return;
	}

	throw std::runtime_error( "unupported stream type" );
}

StreamTranscoder::~StreamTranscoder()
{
	if( _frameBuffer )
		delete _frameBuffer;
	if( _inputEssence )
		delete _inputEssence;
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
	assert( _inputEssence  != NULL );
	assert( _outputEssence != NULL );
	assert( _sourceBuffer  != NULL );
	assert( _frameBuffer   != NULL );
	assert( _transform     != NULL );

	DataStream dataStream;
	if( _verbose )
		std::cout << "transcode a frame " << std::endl;
	if( _inputEssence->readNextFrame( *_sourceBuffer ) )
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
	assert( _inputEssence  != NULL );
	assert( _outputEssence != NULL );
	assert( _sourceBuffer  != NULL );
	assert( _frameBuffer   != NULL );
	assert( _transform     != NULL );

	DataStream dataStream;
	if( _verbose )
		std::cout << "transcode a frame " << std::endl;
	if( _inputEssence->readNextFrame( *_sourceBuffer, subStreamIndex ) )
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

}

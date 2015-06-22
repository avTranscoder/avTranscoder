
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
#include <limits>
#include <sstream>

namespace avtranscoder
{

StreamTranscoder::StreamTranscoder(
		IInputStream& inputStream,
		IOutputFile& outputFile,
		const float offset
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
	, _offset( offset )
	, _canSwitchToGenerator( false )
{
	// create a re-wrapping case
	switch( _inputStream->getStreamType() )
	{
		case AVMEDIA_TYPE_VIDEO :
		{
			VideoFrameDesc inputFrameDesc( _inputStream->getVideoCodec().getVideoFrameDesc() );

			// generator decoder
			VideoGenerator* generatorVideo = new VideoGenerator();
			generatorVideo->setVideoFrameDesc( inputFrameDesc );
			_generator = generatorVideo;

			// buffers to process
			_sourceBuffer = new VideoFrame( inputFrameDesc );
			_frameBuffer = new VideoFrame( inputFrameDesc );

			// transform
			_transform = new VideoTransform();

			// output encoder
			VideoEncoder* outputVideo = new VideoEncoder( _inputStream->getVideoCodec().getCodecName() );
			outputVideo->setupVideoEncoder( inputFrameDesc );
			_outputEncoder = outputVideo;

			// output stream
			_outputStream = &outputFile.addVideoStream( _inputStream->getVideoCodec() );

			break;
		}
		case AVMEDIA_TYPE_AUDIO :
		{
			AudioFrameDesc inputFrameDesc( _inputStream->getAudioCodec().getAudioFrameDesc() );

			// generator decoder
			AudioGenerator* generatorAudio = new AudioGenerator();
			generatorAudio->setAudioFrameDesc( inputFrameDesc );
			_generator = generatorAudio;

			// buffers to process
			_sourceBuffer = new AudioFrame( inputFrameDesc );
			_frameBuffer  = new AudioFrame( inputFrameDesc );

			// transform
			_transform = new AudioTransform();

			// output encoder
			AudioEncoder* outputAudio = new AudioEncoder( _inputStream->getAudioCodec().getCodecName()  );
			outputAudio->setupAudioEncoder( inputFrameDesc );
			_outputEncoder = outputAudio;

			// output stream
			_outputStream = &outputFile.addAudioStream( _inputStream->getAudioCodec() );

			break;
		}
		case AVMEDIA_TYPE_DATA :
		{
			// @warning: rewrap a data stream can't be lengthen by a generator (end of rewrapping will end the all process)
			_outputStream = &outputFile.addDataStream( _inputStream->getDataCodec() );
			break;
		}
		default:
			break;
	}
}

StreamTranscoder::StreamTranscoder(
		IInputStream& inputStream,
		IOutputFile& outputFile,
		const ProfileLoader::Profile& profile,
		const int subStreamIndex,
		const float offset
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
	, _offset( offset )
	, _canSwitchToGenerator( false )
{
	// create a transcode case
	switch( _inputStream->getStreamType() )
	{
		case AVMEDIA_TYPE_VIDEO :
		{
			// input decoder
			VideoDecoder* inputVideo = new VideoDecoder( *static_cast<InputStream*>( _inputStream ) );
			inputVideo->setupDecoder();
			_inputDecoder = inputVideo;
			_currentDecoder = _inputDecoder;

			// output encoder
			VideoEncoder* outputVideo = new VideoEncoder( profile.at( constants::avProfileCodec ) );
			_outputEncoder = outputVideo;

			VideoFrameDesc outputFrameDesc = _inputStream->getVideoCodec().getVideoFrameDesc();
			outputFrameDesc.setParameters( profile );
			outputVideo->setupVideoEncoder( outputFrameDesc, profile );

			// output stream
			_outputStream = &outputFile.addVideoStream( outputVideo->getVideoCodec() );

			// buffers to process
			_sourceBuffer = new VideoFrame( _inputStream->getVideoCodec().getVideoFrameDesc() );
			_frameBuffer = new VideoFrame( outputVideo->getVideoCodec().getVideoFrameDesc() );

			// transform
			_transform = new VideoTransform();

			// generator decoder
			VideoGenerator* generatorVideo = new VideoGenerator();
			generatorVideo->setVideoFrameDesc( outputVideo->getVideoCodec().getVideoFrameDesc() );
			_generator = generatorVideo;

			break;
		}
		case AVMEDIA_TYPE_AUDIO :
		{
			// input decoder
			AudioDecoder* inputAudio = new AudioDecoder( *static_cast<InputStream*>( _inputStream ) );
			inputAudio->setupDecoder();
			_inputDecoder = inputAudio;
			_currentDecoder = _inputDecoder;

			// output encoder
			AudioEncoder* outputAudio = new AudioEncoder( profile.at( constants::avProfileCodec )  );
			_outputEncoder = outputAudio;

			AudioFrameDesc outputFrameDesc( _inputStream->getAudioCodec().getAudioFrameDesc() );
			outputFrameDesc.setParameters( profile );
			if( subStreamIndex > -1 )
			{
				// @todo manage downmix ?
				outputFrameDesc.setChannels( 1 );
			}
			outputAudio->setupAudioEncoder( outputFrameDesc, profile );

			// output stream
			_outputStream = &outputFile.addAudioStream( outputAudio->getAudioCodec() );

			// buffers to process
			AudioFrameDesc inputFrameDesc( _inputStream->getAudioCodec().getAudioFrameDesc() );
			if( subStreamIndex > -1 )
				inputFrameDesc.setChannels( 1 );

			_sourceBuffer = new AudioFrame( inputFrameDesc );
			_frameBuffer  = new AudioFrame( outputAudio->getAudioCodec().getAudioFrameDesc() );

			// transform
			_transform = new AudioTransform();

			// generator decoder
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
}

StreamTranscoder::StreamTranscoder(
		const ICodec& inputCodec,
		IOutputFile& outputFile,
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
	, _offset( 0 )
	, _canSwitchToGenerator( false )
{
	if( profile.find( constants::avProfileType )->second == constants::avProfileTypeVideo )
	{
		// generator decoder
		VideoGenerator* generatorVideo = new VideoGenerator();
		const VideoCodec& inputVideoCodec = static_cast<const VideoCodec&>( inputCodec );
		generatorVideo->setVideoFrameDesc( inputVideoCodec.getVideoFrameDesc() );
		_generator = generatorVideo;
		_currentDecoder = _generator;

		// buffers to process
		VideoFrameDesc inputFrameDesc = inputVideoCodec.getVideoFrameDesc();
		VideoFrameDesc outputFrameDesc = inputFrameDesc;
		outputFrameDesc.setParameters( profile );
		_sourceBuffer = new VideoFrame( inputFrameDesc );
		_frameBuffer  = new VideoFrame( outputFrameDesc );

		// transform
		_transform = new VideoTransform();

		// output encoder
		VideoEncoder* outputVideo = new VideoEncoder( profile.at( constants::avProfileCodec ) );
		outputVideo->setupVideoEncoder( outputFrameDesc, profile );
		_outputEncoder = outputVideo;

		// output stream
		_outputStream = &outputFile.addVideoStream( outputVideo->getVideoCodec() );
	}
	else if( profile.find( constants::avProfileType )->second == constants::avProfileTypeAudio )
	{
		// generator decoder
		AudioGenerator* generatorAudio = new AudioGenerator();
		const AudioCodec& inputAudioCodec = static_cast<const AudioCodec&>( inputCodec );
		generatorAudio->setAudioFrameDesc( inputAudioCodec.getAudioFrameDesc() );
		_generator = generatorAudio;
		_currentDecoder = _generator;

		// buffers to process
		AudioFrameDesc inputFrameDesc = inputAudioCodec.getAudioFrameDesc();
		AudioFrameDesc outputFrameDesc = inputFrameDesc;
		outputFrameDesc.setParameters( profile );
		_sourceBuffer = new AudioFrame( inputFrameDesc );
		_frameBuffer  = new AudioFrame( outputFrameDesc );

		// transform
		_transform = new AudioTransform();

		// output encoder
		AudioEncoder* outputAudio = new AudioEncoder( profile.at( constants::avProfileCodec ) );
		outputAudio->setupAudioEncoder( outputFrameDesc, profile );
		_outputEncoder = outputAudio;

		// output stream
		_outputStream = &outputFile.addAudioStream( outputAudio->getAudioCodec() );
	}
	else
	{
		throw std::runtime_error( "unupported stream type" );
	}
}

StreamTranscoder::~StreamTranscoder()
{
	delete _sourceBuffer;
	delete _frameBuffer;
	delete _generator;
	delete _outputEncoder;
	delete _transform;
	delete _inputDecoder;
}

void StreamTranscoder::preProcessCodecLatency()
{
	int latency = _outputEncoder->getCodec().getLatency();

	LOG_DEBUG( "Latency of stream: " << latency )

	if( ! latency ||
		latency < _outputEncoder->getCodec().getAVCodecContext().frame_number )
		return;

	// set a decoder to preload generated frames
	if( getProcessCase() == eProcessCaseRewrap )
		switchToGeneratorDecoder();

	while( ( latency-- ) > 0 )
	{
		processFrame();
	}

	if( getProcessCase() == eProcessCaseRewrap )
		_currentDecoder = NULL;
}

bool StreamTranscoder::processFrame()
{
	// Manage offset
	if( _offset > 0 )
	{
		bool endOfOffset = _outputStream->getStreamDuration() >= _offset;
		if( endOfOffset )
		{
			LOG_INFO( "End of positive offset" )

			if( getProcessCase() == eProcessCaseTranscode )
				switchToInputDecoder();
			else
				_currentDecoder = NULL;
			_offset = 0;
		}
		else
		{
			// process generator
			if( _currentDecoder != _generator )
			{
				LOG_INFO( "Switch to generator to process offset" )
				switchToGeneratorDecoder();
			}
		}
	}
	else if( _offset < 0 )
	{
		bool endOfStream = _outputStream->getStreamDuration() >= ( _inputStream->getDuration() + _offset );
		if( endOfStream )
		{
			LOG_INFO( "End of negative offset" )

			switchToGeneratorDecoder();
			_offset = 0;
		}
 	}

	if( getProcessCase() == eProcessCaseRewrap )
		return processRewrap();

	return processTranscode( _subStreamIndex );	
}

bool StreamTranscoder::processRewrap()
{
	assert( _inputStream  != NULL );
	assert( _outputStream != NULL );
	assert( _inputDecoder == NULL );

	LOG_DEBUG( "Rewrap a frame" )

	// if switched to generator, process frame
	if( _currentDecoder == _generator )
	{
		return processTranscode();
	}

	LOG_DEBUG( "read next packet" )
	CodedData data;
	if( ! _inputStream->readNextPacket( data ) )
	{
		if( _canSwitchToGenerator )
		{
			switchToGeneratorDecoder();
			return processTranscode();
		}
		return false;
	}

	LOG_DEBUG( "wrap (" << data.getSize() << " bytes)" )
	IOutputStream::EWrappingStatus wrappingStatus = _outputStream->wrap( data );

	switch( wrappingStatus )
	{
		case IOutputStream::eWrappingSuccess:
			return true;
		case IOutputStream::eWrappingWaitingForData:
			// the wrapper needs more data to write the current packet
			return processFrame();
		case IOutputStream::eWrappingError:
			return false;
	}

	return true;
}

bool StreamTranscoder::processTranscode( const int subStreamIndex )
{
	assert( _outputStream   != NULL );
	assert( _currentDecoder != NULL );
	assert( _outputEncoder  != NULL );
	assert( _sourceBuffer   != NULL );
	assert( _frameBuffer    != NULL );
	assert( _transform      != NULL );

	LOG_DEBUG( "Transcode a frame" )

	LOG_DEBUG( "decode next frame" )
	bool decodingStatus = false;
	if( subStreamIndex < 0 )
		decodingStatus = _currentDecoder->decodeNextFrame( *_sourceBuffer );
	else
		decodingStatus = _currentDecoder->decodeNextFrame( *_sourceBuffer, subStreamIndex );

	CodedData data;
	if( decodingStatus )
	{
		LOG_DEBUG( "convert (" << _sourceBuffer->getSize() << " bytes)" )
		_transform->convert( *_sourceBuffer, *_frameBuffer );

		LOG_DEBUG( "encode (" << _frameBuffer->getSize() << " bytes)" )
		_outputEncoder->encodeFrame( *_frameBuffer, data );
	}
	else
	{
		LOG_DEBUG( "encode last frame(s)" )
		if( ! _outputEncoder->encodeFrame( data ) )
		{
			if( _canSwitchToGenerator )
			{
				switchToGeneratorDecoder();
				return processTranscode();
			}
			return false;
		}
	}

	LOG_DEBUG( "wrap (" << data.getSize() << " bytes)" )
	IOutputStream::EWrappingStatus wrappingStatus = _outputStream->wrap( data );

	switch( wrappingStatus )
	{
		case IOutputStream::eWrappingSuccess:
			return true;
		case IOutputStream::eWrappingWaitingForData:
			// the wrapper needs more data to write the current packet
			return processFrame();
		case IOutputStream::eWrappingError:
			return false;
	}

	return true;
}

void StreamTranscoder::switchToGeneratorDecoder()
{
	LOG_INFO( "Switch to generator decoder" )

	_currentDecoder = _generator;
	assert( _currentDecoder != NULL );
}

void StreamTranscoder::switchToInputDecoder()
{
	LOG_INFO( "Switch to input decoder" )

	_currentDecoder = _inputDecoder;
	assert( _currentDecoder != NULL );
}

double StreamTranscoder::getDuration() const
{	
	if( _inputStream )
	{
		double totalDuration = _inputStream->getDuration() + _offset;
		if( totalDuration < 0 )
		{
			LOG_WARN( "Offset of " << _offset << "s applied to a stream with a duration of " << _inputStream->getDuration() << "s. Set its duration to 0s." )
			return 0.;
		}
		return totalDuration;
	}
	else
		return std::numeric_limits<double>::max();
}

StreamTranscoder::EProcessCase StreamTranscoder::getProcessCase() const
{
	if( _inputStream && _inputDecoder )
		return eProcessCaseTranscode;
	else if( _inputStream && ! _inputDecoder )
		return eProcessCaseRewrap;
	else
		return eProcessCaseGenerator;
}

}

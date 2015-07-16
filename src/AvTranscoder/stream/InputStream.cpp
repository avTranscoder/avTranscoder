#include "InputStream.hpp"

#include <AvTranscoder/file/InputFile.hpp>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

#include <stdexcept>
#include <cassert>

namespace avtranscoder
{

InputStream::InputStream( InputFile& inputFile, const size_t streamIndex )
	: IInputStream( )
	, _inputFile( &inputFile )
	, _codec( NULL )
	, _streamCache()
	, _streamIndex( streamIndex )
	, _isActivated( false )
{
	AVCodecContext* context = _inputFile->getFormatContext().getAVStream( _streamIndex ).codec;

	switch( context->codec_type )
	{
		case AVMEDIA_TYPE_VIDEO:
		{
			_codec = new VideoCodec( eCodecTypeDecoder, *context );
			break;
		}
		case AVMEDIA_TYPE_AUDIO:
		{
			_codec = new AudioCodec( eCodecTypeDecoder, *context );
			break;
		}
		case AVMEDIA_TYPE_DATA:
		{
			_codec = new DataCodec( eCodecTypeDecoder, *context );
			break;
		}
		default:
			break;
	}
}

InputStream::~InputStream( )
{
	delete _codec;
}

bool InputStream::readNextPacket( CodedData& data )
{
	if( ! _isActivated )
		throw std::runtime_error( "Can't read packet on non-activated input stream." );

	// if packet is already cached
	if( ! _streamCache.empty() )
	{
		data.copyData( _streamCache.front().getData(), _streamCache.front().getSize() );
		_streamCache.pop();
	}
	// else read next packet
	else
	{
		return _inputFile->readNextPacket( data, _streamIndex ) && _streamCache.empty();
	}

	return true;
}

VideoCodec& InputStream::getVideoCodec()
{
	assert( _streamIndex <= _inputFile->getFormatContext().getNbStreams() );

	if( getStreamType() != AVMEDIA_TYPE_VIDEO )
	{
		throw std::runtime_error( "unable to get video descriptor on non-video stream" );
	}

	return *static_cast<VideoCodec*>( _codec );
}

AudioCodec& InputStream::getAudioCodec()
{
	assert( _streamIndex <= _inputFile->getFormatContext().getNbStreams() );

	if( getStreamType() != AVMEDIA_TYPE_AUDIO )
	{
		throw std::runtime_error( "unable to get audio descriptor on non-audio stream" );
	}

	return *static_cast<AudioCodec*>( _codec );
}

DataCodec& InputStream::getDataCodec()
{
	assert( _streamIndex <= _inputFile->getFormatContext().getNbStreams() );

	if( getStreamType() != AVMEDIA_TYPE_DATA )
	{
		throw std::runtime_error( "unable to get data descriptor on non-data stream" );
	}

	return *static_cast<DataCodec*>( _codec );
}

AVMediaType InputStream::getStreamType() const
{
	return _inputFile->getFormatContext().getAVStream( _streamIndex ).codec->codec_type;
}

float InputStream::getDuration() const
{
	return _inputFile->getProperties().getStreamPropertiesWithIndex( _streamIndex ).getDuration();
}

void InputStream::addPacket( AVPacket& packet )
{
	// Do not cache data if the stream is declared as unused in process
	if( ! _isActivated )
		return;

	_streamCache.push( CodedData() );
	_streamCache.back().copyData( packet.data, packet.size );
}

void InputStream::clearBuffering()
{
	_streamCache = std::queue<CodedData>();
}

}

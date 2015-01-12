#include "AvInputStream.hpp"

#include <AvTranscoder/file/InputFile.hpp>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

#include <stdexcept>
#include <cassert>

namespace avtranscoder
{

AvInputStream::AvInputStream( InputFile& inputFile, const size_t streamIndex )
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
			VideoCodec* videoCodec = new VideoCodec( eCodecTypeDecoder, context->codec_id );
			videoCodec->setImageParameters( context->width, context->height, context->pix_fmt );
			videoCodec->setTimeBase( context->time_base.num, context->time_base.den, context->ticks_per_frame );

			_codec = videoCodec;
			break;
		}
		case AVMEDIA_TYPE_AUDIO:
		{
			AudioCodec* audioCodec = new AudioCodec( eCodecTypeDecoder, context->codec_id );
			audioCodec->setAudioParameters( context->sample_rate, context->channels, context->sample_fmt );

			_codec = audioCodec;
			break;
		}
		case AVMEDIA_TYPE_DATA:
		{
			DataCodec* dataCodec = new DataCodec( eCodecTypeDecoder, context->codec_id );

			_codec= dataCodec;
			break;
		}
		default:
			break;
	}
}

AvInputStream::AvInputStream( const AvInputStream& inputStream )
	: IInputStream( )
	, _inputFile( inputStream._inputFile )
	, _codec( inputStream._codec )
	, _streamCache()
	, _streamIndex( inputStream._streamIndex )
	, _isActivated( inputStream._isActivated )
{
}

AvInputStream::~AvInputStream( )
{
	delete _codec;
}

bool AvInputStream::readNextPacket( CodedData& data )
{
	if( ! _isActivated )
		throw std::runtime_error( "Can't read packet on non-activated input stream." );

	// if packet is already cached
	if( ! _streamCache.empty() )
	{
		_streamCache.front().getBuffer().swap( data.getBuffer() );
		_streamCache.pop();
	}
	// else read next packet
	else
	{
		return _inputFile->readNextPacket( data, _streamIndex ) && _streamCache.empty();
	}

	return true;
}

VideoCodec& AvInputStream::getVideoCodec()
{
	assert( _streamIndex <= _inputFile->getFormatContext().getNbStreams() );

	if( getStreamType() != AVMEDIA_TYPE_VIDEO )
	{
		throw std::runtime_error( "unable to get video descriptor on non-video stream" );
	}

	return *static_cast<VideoCodec*>( _codec );;
}

AudioCodec& AvInputStream::getAudioCodec()
{
	assert( _streamIndex <= _inputFile->getFormatContext().getNbStreams() );

	if( getStreamType() != AVMEDIA_TYPE_AUDIO )
	{
		throw std::runtime_error( "unable to get audio descriptor on non-audio stream" );
	}

	return *static_cast<AudioCodec*>( _codec );;
}

DataCodec& AvInputStream::getDataCodec()
{
	assert( _streamIndex <= _inputFile->getFormatContext().getNbStreams() );

	if( getStreamType() != AVMEDIA_TYPE_DATA )
	{
		throw std::runtime_error( "unable to get data descriptor on non-data stream" );
	}

	return *static_cast<DataCodec*>( _codec );;
}

AVMediaType AvInputStream::getStreamType() const
{
	return _inputFile->getFormatContext().getAVStream( _streamIndex ).codec->codec_type;
}

double AvInputStream::getDuration() const
{
	return 1.0 * _inputFile->getFormatContext().getDuration() / AV_TIME_BASE;
}

void AvInputStream::addPacket( AVPacket& packet )
{
	// Do not cache data if the stream is declared as unused in process
	if( ! _isActivated )
		return;

	CodedData data;
	_streamCache.push( data );
	_streamCache.back().copyData( packet.data, packet.size );
}

void AvInputStream::clearBuffering()
{
	_streamCache = std::queue<CodedData>();
}

}

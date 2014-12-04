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
	, _streamIndex( streamIndex )
	, _bufferized( false )
{
	AVCodecContext* context = _inputFile->getFormatContext().streams[_streamIndex]->codec;

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
	, _streamIndex( inputStream._streamIndex )
	, _bufferized( inputStream._bufferized )
{
}

AvInputStream::~AvInputStream( )
{
	delete _codec;
}

bool AvInputStream::readNextPacket( CodedData& data )
{
	if( ! _bufferized )
		throw std::runtime_error( "Can't read packet on non-bufferized input stream." );

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

void AvInputStream::addPacket( AVPacket& packet )
{
	// Do not cache data if the stream is declared as unused in process
	if( ! _bufferized )
		return;

	CodedData data;
	_streamCache.push( data );
	_streamCache.back().getBuffer().resize( packet.size );
	if( packet.size != 0 )
		memcpy( _streamCache.back().getPtr(), packet.data, packet.size );
}

VideoCodec& AvInputStream::getVideoCodec()
{
	assert( _streamIndex <= _inputFile->getFormatContext().nb_streams );

	if( getAVStream()->codec->codec_type != AVMEDIA_TYPE_VIDEO )
	{
		throw std::runtime_error( "unable to get video descriptor on non-video stream" );
	}

	return *static_cast<VideoCodec*>( _codec );;
}

AudioCodec& AvInputStream::getAudioCodec()
{
	assert( _streamIndex <= _inputFile->getFormatContext().nb_streams );

	if( getAVStream()->codec->codec_type != AVMEDIA_TYPE_AUDIO )
	{
		throw std::runtime_error( "unable to get audio descriptor on non-audio stream" );
	}

	return *static_cast<AudioCodec*>( _codec );;
}

DataCodec& AvInputStream::getDataCodec()
{
	assert( _streamIndex <= _inputFile->getFormatContext().nb_streams );

	if( getAVStream()->codec->codec_type != AVMEDIA_TYPE_DATA )
	{
		throw std::runtime_error( "unable to get data descriptor on non-data stream" );
	}

	return *static_cast<DataCodec*>( _codec );;
}

AVMediaType AvInputStream::getStreamType() const
{
	return _inputFile->getStreamType( _streamIndex );
}

double AvInputStream::getDuration() const
{
	return 1.0 * _inputFile->getFormatContext().duration / AV_TIME_BASE;
}

void AvInputStream::clearBuffering()
{
	_streamCache = std::queue<CodedData>();
}

AVStream* AvInputStream::getAVStream() const
{
	return _inputFile->getFormatContext().streams[_streamIndex];
}

}

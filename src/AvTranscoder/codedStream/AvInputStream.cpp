#include "AvInputStream.hpp"

#include <AvTranscoder/file/InputFile.hpp>

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
 #define __STDC_CONSTANT_MACROS
#endif

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
	, _packetDuration( 0 )
	, _streamIndex( streamIndex )
	, _bufferized( false )
{
	AVCodecContext* context = _inputFile->getFormatContext().streams[_streamIndex]->codec;

	switch( context->codec_type )
	{
		case AVMEDIA_TYPE_VIDEO:
		{
			VideoCodec* videoCodec = new VideoCodec( context->codec_id );
			videoCodec->setImageParameters( context->width, context->height, context->pix_fmt );
			videoCodec->setTimeBase( context->time_base.num, context->time_base.den, context->ticks_per_frame );

			_codec = videoCodec;
			break;
		}
		case AVMEDIA_TYPE_AUDIO:
		{
			double outputFps = 25;
			size_t bytePerSample = av_get_bytes_per_sample( context->sample_fmt );

			context->block_align = 1.0 * context->sample_rate * context->channels * bytePerSample / outputFps;
			// std::cout << "channels " << context->channel_layout << std::endl;
			// std::cout << "audio buffer read size " << context->block_align << std::endl;

			AudioCodec* audioCodec = new AudioCodec( context->codec_id );
			audioCodec->setAudioParameters( context->sample_rate, context->channels, context->sample_fmt );

			_codec = audioCodec;
			break;
		}
		case AVMEDIA_TYPE_DATA:
		{
			DataCodec* dataCodec = new DataCodec( context->codec_id );

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

	if( _streamCache.empty() )
		_inputFile->readNextPacket( _streamIndex );

	if( _streamCache.empty() )
		return false;

	_streamCache.front().getBuffer().swap( data.getBuffer() );

	_streamCache.erase( _streamCache.begin() );

	return true;
}

void AvInputStream::addPacket( AVPacket& packet )
{
	//std::cout << "add packet for stream " << _streamIndex << std::endl;
	CodedData data;
	_streamCache.push_back( data );
	_packetDuration = packet.duration;

	if( ! _bufferized )
		return;

	// is it possible to remove this copy ?
	// using : av_packet_unref ?
	_streamCache.back().getBuffer().resize( packet.size );
	if( packet.size != 0 )
		memcpy( _streamCache.back().getPtr(), packet.data, packet.size );

	// std::vector<unsigned char> tmpData( 0,0 );
	// &tmpData[0] = packet.data;
	// tmpData.size( packet.size );

	// remove reference on packet because it's passed to CodedData
	// packet.data = NULL;
	// packet.size = 0;

	// std::cout << this << " buffer size " << _streamCache.size() << std::endl;
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

double AvInputStream::getPacketDuration() const
{
	return _packetDuration * av_q2d( _inputFile->getFormatContext().streams[_streamIndex]->time_base );
}

void AvInputStream::clearBuffering()
{
	_streamCache.clear();
}

AVStream* AvInputStream::getAVStream() const
{
	return _inputFile->getFormatContext().streams[_streamIndex];
}

}

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
		, _packetDuration( 0 )
		, _streamIndex( streamIndex )
		, _bufferized( false )
{
	AVCodecContext* context = _inputFile->getFormatContext().streams[_streamIndex]->codec;
	if( context->codec_type == AVMEDIA_TYPE_AUDIO )
	{
		double outputFps = 25;
		size_t bytePerSample = av_get_bytes_per_sample( context->sample_fmt );

		context->block_align = 1.0 * context->sample_rate * context->channels * bytePerSample / outputFps;
		// std::cout << "channels " << context->channel_layout << std::endl;
		// std::cout << "audio buffer read size " << context->block_align << std::endl;
	}
}

AvInputStream::~AvInputStream( )
{
}

bool AvInputStream::readNextPacket( DataStream& data )
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
	DataStream data;
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

	// remove reference on packet because it's passed to DataStream
	// packet.data = NULL;
	// packet.size = 0;

	// std::cout << this << " buffer size " << _streamCache.size() << std::endl;
}

VideoCodec AvInputStream::getVideoCodec() const
{
	assert( _streamIndex <= _inputFile->getFormatContext().nb_streams );

	if( getAVStream()->codec->codec_type != AVMEDIA_TYPE_VIDEO )
	{
		throw std::runtime_error( "unable to get video descriptor on non-video stream" );
	}

	AVCodecContext* codecContext = _inputFile->getFormatContext().streams[_streamIndex]->codec;

	VideoCodec desc( codecContext->codec_id );

	desc.setImageParameters( codecContext->width, codecContext->height, codecContext->pix_fmt );
	desc.setTimeBase( codecContext->time_base.num, codecContext->time_base.den, codecContext->ticks_per_frame );

	return desc;
}

AudioCodec AvInputStream::getAudioCodec() const
{
	assert( _streamIndex <= _inputFile->getFormatContext().nb_streams );

	if( getAVStream()->codec->codec_type != AVMEDIA_TYPE_AUDIO )
	{
		throw std::runtime_error( "unable to get audio descriptor on non-audio stream" );
	}

	AVCodecContext* codecContext = _inputFile->getFormatContext().streams[_streamIndex]->codec;

	AudioCodec desc( codecContext->codec_id );

	desc.setAudioParameters( codecContext->sample_rate, codecContext->channels, codecContext->sample_fmt );

	return desc;
}

DataCodec AvInputStream::getDataCodec() const
{
	DataCodec desc;
	return desc;
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

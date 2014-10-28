#include "AvInputVideo.hpp"

#include <mediaCore/option/Context.hpp>
#include <mediaCore/frame/VideoFrame.hpp>
#include <mediaIO/codedStream/AvInputStream.hpp>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/pixdesc.h>
}

#include <stdexcept>
#include <iostream>

namespace mediamanager
{
namespace mediaio
{

AvInputVideo::AvInputVideo( AvInputStream& inputStream )
	: IInputEssence()
	, _inputStream   ( &inputStream )
	, _codec( eCodecTypeDecoder, inputStream.getVideoCodec().getCodecId() )
	, _frame         ( NULL )
	, _selectedStream( -1 )
{
}

AvInputVideo::~AvInputVideo()
{
	if( _frame != NULL )
	{
#if LIBAVCODEC_VERSION_MAJOR > 54
		av_frame_free( &_frame );
#else
 #if LIBAVCODEC_VERSION_MAJOR > 53
		avcodec_free_frame( &_frame );
 #else
		av_free( _frame );
 #endif
#endif
		_frame = NULL;
	}
}

void AvInputVideo::setup()
{
	AVCodecContext* avCodecContext = _codec.getAVCodecContext();
	AVCodec* avCodec = _codec.getAVCodec();

	// if( avCodec->capabilities & CODEC_CAP_TRUNCATED )
	// 	avCodecContext->flags |= CODEC_FLAG_TRUNCATED;

	int ret = avcodec_open2( avCodecContext, avCodec, NULL );

	if( ret < 0 || avCodecContext == NULL || avCodec == NULL )
	{
		std::string msg = "unable open video codec: ";
		msg +=  avCodec->long_name;
		msg += " (";
		msg += avCodec->name;
		msg += ")";
		avcodec_close( avCodecContext );
		throw std::runtime_error( msg );
	}

#if LIBAVCODEC_VERSION_MAJOR > 54
	_frame = av_frame_alloc();
#else
	_frame = avcodec_alloc_frame();
#endif
	if( _frame == NULL )
	{
		throw std::runtime_error( "unable to setup frame buffer" );
	}
}

bool AvInputVideo::readNextFrame( mediacore::Frame& frameBuffer )
{
	int got_frame = 0;

	while( ! got_frame )
	{
		mediacore::CodedData data;
		if( ! _inputStream->readNextPacket( data ) )
			return false;

		AVPacket packet;
		av_init_packet( &packet );

		packet.stream_index = _selectedStream;
		packet.data         = data.getPtr();
		packet.size         = data.getSize();
		
		int ret = avcodec_decode_video2( _codec.getAVCodecContext(), _frame, &got_frame, &packet );
		
		if( ret < 0 )
		{
			char err[250];
			av_strerror( ret, err, 250);
			
			throw std::runtime_error( "an error occured during video decoding - " + std::string(err) );
		}
		
		av_free_packet( &packet );
	}

	mediacore::VideoFrame& imageBuffer = static_cast<mediacore::VideoFrame&>( frameBuffer );

	size_t decodedSize = avpicture_get_size( (AVPixelFormat)_frame->format, _frame->width, _frame->height );
	if( imageBuffer.getBuffer().size() != decodedSize )
		imageBuffer.getBuffer().resize( decodedSize );

	// Copy pixel data from an AVPicture into one contiguous buffer.
	avpicture_layout( (AVPicture*)_frame, (AVPixelFormat)_frame->format, _frame->width, _frame->height, &imageBuffer.getBuffer()[0], frameBuffer.getBuffer().size() );

	return true;
}

bool AvInputVideo::readNextFrame( mediacore::Frame& frameBuffer, const size_t subStreamIndex )
{
	return false;
}

void AvInputVideo::flushDecoder()
{
	avcodec_flush_buffers( _codec.getAVCodecContext() );
}

void AvInputVideo::setProfile( const mediacore::Profile::ProfileDesc& desc )
{
	mediacore::Context codecContext( _codec.getAVCodecContext() );

	for( mediacore::Profile::ProfileDesc::const_iterator it = desc.begin(); it != desc.end(); ++it )
	{
		if( (*it).first == mediacore::constants::mediaProfileIdentificator ||
			(*it).first == mediacore::constants::mediaProfileIdentificatorHuman ||
			(*it).first == mediacore::constants::mediaProfileType )
			continue;

		try
		{
			mediacore::Option& decodeOption = codecContext.getOption( (*it).first );
			decodeOption.setString( (*it).second );
		}
		catch( std::exception& e )
		{
			std::cout << "[InputVideo] warning: " << e.what() << std::endl;
		}
	}
}

}
}

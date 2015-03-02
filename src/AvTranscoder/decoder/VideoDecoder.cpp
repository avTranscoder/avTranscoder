#include "VideoDecoder.hpp"

#include <AvTranscoder/codec/ICodec.hpp>
#include <AvTranscoder/stream/InputStream.hpp>
#include <AvTranscoder/frame/VideoFrame.hpp>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/pixdesc.h>
}

#include <stdexcept>
#include <iostream>

namespace avtranscoder
{

VideoDecoder::VideoDecoder( InputStream& inputStream )
	: _inputStream   ( &inputStream )
	, _frame         ( NULL )
{
}

VideoDecoder::~VideoDecoder()
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

void VideoDecoder::setup()
{
	_inputStream->getVideoCodec().open();

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

bool VideoDecoder::decodeNextFrame( Frame& frameBuffer )
{
	if( ! decodeNextFrame() )
		return false;

	size_t decodedSize = avpicture_get_size( (AVPixelFormat)_frame->format, _frame->width, _frame->height );
	if( decodedSize == 0 )
		return false;

	VideoFrame& imageBuffer = static_cast<VideoFrame&>( frameBuffer );
	imageBuffer.resize( decodedSize );

	// Copy pixel data from an AVPicture into one contiguous buffer.
	avpicture_layout( (AVPicture*)_frame, (AVPixelFormat)_frame->format, _frame->width, _frame->height, imageBuffer.getData(), frameBuffer.getSize() );

	return true;
}

bool VideoDecoder::decodeNextFrame( Frame& frameBuffer, const size_t subStreamIndex )
{
	return false;
}

bool VideoDecoder::decodeNextFrame()
{
	int got_frame = 0;
	while( ! got_frame )
	{
		CodedData data;

		bool nextPacketRead = _inputStream->readNextPacket( data );
		if( ! nextPacketRead ) // error or end of file
			return false;

		int ret = avcodec_decode_video2( &_inputStream->getVideoCodec().getAVCodecContext(), _frame, &got_frame, &data.getAVPacket() );
		if( ret == 0 && got_frame == 0 ) // no frame could be decompressed
			return false;

		if( ret < 0 )
		{
			char err[AV_ERROR_MAX_STRING_SIZE];
			av_strerror( ret, err, sizeof(err) );
			throw std::runtime_error( "an error occured during video decoding - " + getDescriptionFromErrorCode( ret ) );
		}
	}
	return true;
}

void VideoDecoder::flushDecoder()
{
	avcodec_flush_buffers( &_inputStream->getVideoCodec().getAVCodecContext() );
}

void VideoDecoder::setProfile( const ProfileLoader::Profile& profile )
{
	// set threads if not in profile
	if( ! profile.count( "threads" ) )
		_inputStream->getVideoCodec().getOption( "threads" ).setString( "auto" );

	for( ProfileLoader::Profile::const_iterator it = profile.begin(); it != profile.end(); ++it )
	{
		if( (*it).first == constants::avProfileIdentificator ||
			(*it).first == constants::avProfileIdentificatorHuman ||
			(*it).first == constants::avProfileType )
			continue;

		try
		{
			Option& decodeOption = _inputStream->getVideoCodec().getOption( (*it).first );
			decodeOption.setString( (*it).second );
		}
		catch( std::exception& e )
		{
			std::cout << "[VideoDecoder] warning - can't set option " << (*it).first << " to " << (*it).second << ": " << e.what() << std::endl;
		}
	}
}

}

#include "AvInputAudio.hpp"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/pixdesc.h>
#include <libavutil/channel_layout.h>
}

#include <AvTranscoder/codedStream/AvInputStream.hpp>
#include <AvTranscoder/frame/AudioFrame.hpp>

#include <iostream>
#include <stdexcept>

namespace avtranscoder
{

AvInputAudio::AvInputAudio( AvInputStream& inputStream ) 
	: IInputEssence()
	, _inputStream   ( &inputStream )
	, _codec( eCodecTypeDecoder, inputStream.getAudioCodec().getCodecId() )
	, _frame         ( NULL )
	, _selectedStream( -1 )
{
}

AvInputAudio::~AvInputAudio()
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


void AvInputAudio::setup()
{
	AVCodecContext* avCodecContext = _codec.getAVCodecContext();
	AVCodec* avCodec = _codec.getAVCodec();

	avCodecContext->channels = _inputStream->getAudioCodec().getChannels();
	
	int ret = avcodec_open2( avCodecContext, avCodec, NULL );

	if( ret < 0 || avCodecContext == NULL || avCodec == NULL )
	{
		std::string msg = "unable open audio codec: ";
		msg +=  avCodec->long_name;
		msg += " (";
		msg += avCodec->name;
		msg += ")";
		avcodec_close( avCodecContext );

		char err[250];

		av_strerror( ret, err, 250 );
		std::cout << err << std::endl;
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

bool AvInputAudio::readNextFrame( Frame& frameBuffer )
{
	if( ! getNextFrame() )
		return false;

	AVCodecContext* avCodecContext = _codec.getAVCodecContext();

	size_t decodedSize = av_samples_get_buffer_size(
		NULL, avCodecContext->channels,
		_frame->nb_samples,
		avCodecContext->sample_fmt, 1 );
	
	AudioFrame& audioBuffer = static_cast<AudioFrame&>( frameBuffer );

	audioBuffer.setNbSamples( _frame->nb_samples );
	
	if( decodedSize )
	{
		if( audioBuffer.getSize() != decodedSize )
			audioBuffer.getBuffer().resize( decodedSize, 0 );
		
		unsigned char* dst = audioBuffer.getPtr();
		av_samples_copy(
			&dst, (uint8_t* const* )_frame->data, 0,
			0, _frame->nb_samples, avCodecContext->channels,
			avCodecContext->sample_fmt );
	}

	return true;
}

bool AvInputAudio::readNextFrame( Frame& frameBuffer, const size_t subStreamIndex )
{
	if( ! getNextFrame() )
		return false;

	const int output_nbChannels = 1;
	const int output_align = 1;
	size_t decodedSize = av_samples_get_buffer_size(NULL, output_nbChannels, _frame->nb_samples, _codec.getAVCodecContext()->sample_fmt, output_align);
	
	size_t nbSubStreams = _codec.getAVCodecContext()->channels;
	size_t bytePerSample = av_get_bytes_per_sample( (AVSampleFormat)_frame->format );

	if( subStreamIndex > nbSubStreams - 1 )
	{
		throw std::runtime_error( "The subStream doesn't exist");
	}
	
	AudioFrame& audioBuffer = static_cast<AudioFrame&>( frameBuffer );
	audioBuffer.setNbSamples( _frame->nb_samples );
	
	if( decodedSize )
	{
		if( audioBuffer.getSize() != decodedSize )
			audioBuffer.getBuffer().resize( decodedSize, 0 );

		// @todo manage cases with data of frame not only on data[0] (use _frame.linesize)
		unsigned char* src = _frame->data[0];
		unsigned char* dst = audioBuffer.getPtr();

		// offset
		src += subStreamIndex * bytePerSample;
		
		for( int sample = 0; sample < _frame->nb_samples; ++sample )
		{
			// std::cout << "sample " << sample << " ==| ";
			// std::cout << "src " << static_cast<void *>(src) << " -> ";
			// std::cout << "dst " << static_cast<void *>(dst) << std::endl;
			memcpy( dst, src, bytePerSample );
			dst += bytePerSample;
			src += bytePerSample * nbSubStreams;
		}
	}
	return true;
}

bool AvInputAudio::getNextFrame()
{
	int got_frame = 0;
	while( ! got_frame )
	{
		CodedData data;
		if( ! _inputStream->readNextPacket( data ) ) // error or end of file
			return false;

		AVPacket packet;
		av_init_packet( &packet );
		
		packet.stream_index = _selectedStream;
		packet.data         = data.getPtr();
		packet.size         = data.getSize();
		
		int ret = avcodec_decode_audio4( _codec.getAVCodecContext(), _frame, &got_frame, &packet );

		if( ret < 0 )
		{
			char err[250];
			av_strerror( ret, err, 250 );
			
			throw std::runtime_error( "an error occured during audio decoding" + std::string( err ) );
		}

		av_free_packet( &packet );
	}
	return true;
}

}

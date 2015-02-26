#include "AudioDecoder.hpp"

#include <AvTranscoder/codec/ICodec.hpp>
#include <AvTranscoder/stream/InputStream.hpp>
#include <AvTranscoder/frame/AudioFrame.hpp>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/pixdesc.h>
#include <libavutil/channel_layout.h>
}

#include <iostream>
#include <stdexcept>

namespace avtranscoder
{

AudioDecoder::AudioDecoder( InputStream& inputStream ) 
	: _inputStream   ( &inputStream )
	, _frame         ( NULL )
{
}

AudioDecoder::~AudioDecoder()
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


void AudioDecoder::setup()
{
	_inputStream->getAudioCodec().open();

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

bool AudioDecoder::decodeNextFrame( Frame& frameBuffer )
{
	if( ! decodeNextFrame() )
		return false;

	AVCodecContext& avCodecContext = _inputStream->getAudioCodec().getAVCodecContext();

	size_t decodedSize = av_samples_get_buffer_size( NULL, avCodecContext.channels, _frame->nb_samples, avCodecContext.sample_fmt, 1 );
	if( decodedSize == 0 )
		return false;

	AudioFrame& audioBuffer = static_cast<AudioFrame&>( frameBuffer );
	audioBuffer.setNbSamples( _frame->nb_samples );
	audioBuffer.resize( decodedSize );

	// @todo manage cases with data of frame not only on data[0] (use _frame.linesize)
	unsigned char* const src = _frame->data[0];
	unsigned char* dst = audioBuffer.getData();

	av_samples_copy( &dst, &src, 0, 0, _frame->nb_samples, avCodecContext.channels, avCodecContext.sample_fmt );

	return true;
}

bool AudioDecoder::decodeNextFrame( Frame& frameBuffer, const size_t subStreamIndex )
{
	if( ! decodeNextFrame() )
		return false;

	AVCodecContext& avCodecContext = _inputStream->getAudioCodec().getAVCodecContext();

	const int output_nbChannels = 1;
	const int output_align = 1;
	size_t decodedSize = av_samples_get_buffer_size(NULL, output_nbChannels, _frame->nb_samples, avCodecContext.sample_fmt, output_align);
	
	size_t nbSubStreams = avCodecContext.channels;
	size_t bytePerSample = av_get_bytes_per_sample( (AVSampleFormat)_frame->format );

	if( subStreamIndex > nbSubStreams - 1 )
	{
		throw std::runtime_error( "The subStream doesn't exist");
	}

	if( decodedSize == 0 )
		return false;

	AudioFrame& audioBuffer = static_cast<AudioFrame&>( frameBuffer );
	audioBuffer.setNbSamples( _frame->nb_samples );
	audioBuffer.resize( decodedSize );

	// @todo manage cases with data of frame not only on data[0] (use _frame.linesize)
	unsigned char* src = _frame->data[0];
	unsigned char* dst = audioBuffer.getData();

	// offset
	src += subStreamIndex * bytePerSample;

	for( int sample = 0; sample < _frame->nb_samples; ++sample )
	{
		memcpy( dst, src, bytePerSample );
		dst += bytePerSample;
		src += bytePerSample * nbSubStreams;
	}

	return true;
}

bool AudioDecoder::decodeNextFrame()
{
	int got_frame = 0;
	while( ! got_frame )
	{
		CodedData data;

		bool nextPacketRead = _inputStream->readNextPacket( data );
		if( ! nextPacketRead ) // error or end of file
			data.clear();

		int ret = avcodec_decode_audio4( &_inputStream->getAudioCodec().getAVCodecContext(), _frame, &got_frame, &data.getAVPacket() );
		if( ! nextPacketRead && ret == 0 && got_frame == 0 ) // no frame could be decompressed
			return false;
		
		if( ret < 0 )
		{
			char err[AV_ERROR_MAX_STRING_SIZE];
			av_strerror( ret, err, sizeof(err) );
			throw std::runtime_error( "an error occured during audio decoding" + std::string( err ) );
		}
	}
	return true;
}

void AudioDecoder::setProfile( const ProfileLoader::Profile& profile )
{
	// set threads if not in profile
	if( ! profile.count( "threads" ) )
		_inputStream->getAudioCodec().getOption( "threads" ).setString( "auto" );

	for( ProfileLoader::Profile::const_iterator it = profile.begin(); it != profile.end(); ++it )
	{
		if( (*it).first == constants::avProfileIdentificator ||
			(*it).first == constants::avProfileIdentificatorHuman ||
			(*it).first == constants::avProfileType )
			continue;

		try
		{
			Option& decodeOption = _inputStream->getAudioCodec().getOption( (*it).first );
			decodeOption.setString( (*it).second );
		}
		catch( std::exception& e )
		{
			std::cout << "[AudioDecoder] warning - can't set option " << (*it).first << " to " << (*it).second << ": " << e.what() << std::endl;
		}
	}
}

}
